/*
* Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*
* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Host.h"
#include "DebugLogger.h"
#include "PersistentConfiguration.h"
#include "ArgumentsParser.h"
#include "EventsDefinitions.h"
#include "JsonCommandsTcpServer.h"
#include "CommandsTcpServer.h"
#include "EventsTcpServer.h"
#include "DeviceManager.h"
#include "LogCollector/LogCollectorService.h"
#include "LogCollector/LogReaderBase.h"
#include "WebHttpServer.h"
#include "TaskManager.h"
#include "HostInfo.h"
#include "TcpNetworkInterface.h"

#include <iostream>
#include <thread>

using namespace std;

// The private singleton Cntr
Host::Host()
    : m_spHostInfo(new HostInfo())
    , m_spTaskManager(new TaskManager())
    , m_isEnumerating(true)
{
    m_deviceManager.reset(new DeviceManager());
    m_logCollectorService.reset(new LogCollectorService());
}

Host::~Host() = default; // To allow fwd declaration for unique_ptr

void Host::StartHost(bool logCollectorTracer3ppLogsFilterOn, std::unique_ptr<ArgumentsParser> argumentParser)
{
    LogReaderBase::Tracer3ppLogsFilterOn = logCollectorTracer3ppLogsFilterOn;

    // Should be initialized first to provide configuration to other modules
    m_argumentsParser = std::move(argumentParser);
    m_spPersistentConfiguration.reset(new PersistentConfiguration(m_argumentsParser->GetCustomConfigFile()));

    m_pEventsTcpServer.reset(new EventsTcpServer);
    m_pJsonCommandsTcpServer.reset(new JsonCommandsTcpServer(*this));
    m_pCommandsTcpServer.reset(new CommandsTcpServer(*this));

    if (!m_pEventsTcpServer->Init(static_cast<uint16_t>(m_spPersistentConfiguration->HostConfiguration.EventPort)) ||
        !m_pCommandsTcpServer->Init(static_cast<uint16_t>(m_spPersistentConfiguration->HostConfiguration.StringCommandPort)) ||
        !m_pJsonCommandsTcpServer->Init(static_cast<uint16_t>(m_spPersistentConfiguration->HostConfiguration.JsonCommandPort)))
    {
        LOG_ERROR << "Cannot start mandatory TCP servers, terminating the application" << endl;
        m_logCollectorService.reset();
        m_spPersistentConfiguration.reset();
        return;
    }

    thread threadEventsTcpServer = thread(&EventsTcpServer::Start, m_pEventsTcpServer.get());

    m_isEnumerating = GetConfiguration().HostConfiguration.PeriodicEnumeration;


    m_deviceManager->Init(); // needed since update connected devices is using log service and log service needs device manager.
    if (!m_spTaskManager->RegisterTask(
        std::bind(&Host::PublishKeepAliveEvent, this), m_keepAliveTaskName,
        std::chrono::seconds(static_cast<long long>(m_spPersistentConfiguration->HostConfiguration.KeepAliveIntervalSec))))
    {
        LOG_ERROR << "Cannot send keep alive messages to DmTools" << endl;
    }

    thread threadJsonCommandsTcpServer = thread(&JsonCommandsTcpServer::Start, m_pJsonCommandsTcpServer.get());
    thread threadCommandsTcpServer = thread(&CommandsTcpServer::Start, m_pCommandsTcpServer.get());

    m_pHttpServer.reset(new WebHttpServer(m_spPersistentConfiguration->HostConfiguration.HttpPort));
    thread threadHttpServer = thread(&WebHttpServer::StartServer, m_pHttpServer.get());

    threadEventsTcpServer.join();
    threadJsonCommandsTcpServer.join();
    threadCommandsTcpServer.join();
    threadHttpServer.join();
}

void Host::StopHost()
{
    m_spTaskManager->UnregisterTaskBlocking(m_keepAliveTaskName);
    m_spTaskManager->StopBlocking();
    m_pEventsTcpServer->Stop();
    m_pJsonCommandsTcpServer->Stop();
    m_pCommandsTcpServer->Stop();
    m_pHttpServer->Stop();

    LOG_INFO << "Done stopping all servers" << std::endl;

    // do not try to access Host after stop
    m_deviceManager.reset();
    LOG_DEBUG << "Reset of device manager completed" << std::endl;
}

// Push the given event through Events TCP Server
void Host::PushEvent(const HostManagerEventBase& event) const
{
    if (!m_pEventsTcpServer)
    {    // shouldn't happen
        LOG_ERROR << "Host::PushEvent: Events TCP Server is not ready!" << endl;
        return;
    }

    m_pEventsTcpServer->SendToAllConnectedClients(event.ToJson());
}

void Host::PublishKeepAliveEvent()
{
    PushEvent(KeepAliveEvent());
}
