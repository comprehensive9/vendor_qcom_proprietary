/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
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

#include "HostInfo.h"
#include "SharedVersionInfo.h"
#include "DebugLogger.h"
#include "FileSystemOsAbstraction.h"
#include "Host.h"
#include "PersistentConfiguration.h"

#include <iostream>
#include <cstring>

#ifdef _WINDOWS
#include <winsock2.h>       // For gethostname
#define HOST_NAME_MAX 64
#else
#include <unistd.h>         // For gethostname
#include <limits.h>         // For HOST_NAME_MAX
#endif

using namespace std;

namespace
{
    // Enumeration of Host capabilities
    // Note: This is a contract with DmTools, the order is important!
    enum CAPABILITIES
    {
        COLLECTING_LOGS = 0,        // capability of host manager to collect logs by itself and send them to DmTools
        KEEP_ALIVE,                 // capability of host manager to send keep-alive event to enable watchdog on the client side
        READ_BLOCK_FAST,            // RESERVED, not in use anymore and may be reused
        INTERFACE_NOTIFICATIONS,    // host manager notifies interface changes
        LOG_COLLECTOR_JSON_API,     // Support for LogCollector JSON API including DmTools support
        CONTINUOUS_PMC,             // capability of host manager to collect continuous PMC events
        DUMP_JSON_API,              // Support JSON command for Dump
        DEVICE_ACCESS_JSON_API,     // Support json I/O command
        LEGACY_PMC_JSON_API,        // Support legacy PMC JSON commands
        DEVICE_COMMANDS_JSON_API,   // Support json device commands/queries
        HOST_COMMANDS_JSON_API,     //Support json host commands/queries
    };
}

HostInfo::HostInfo() :
    m_capabilityMask(0U)
{
    CreateHostDirectories();
    SetHostCapabilities();
}

string HostInfo::GetHostName()
{
    char hostName[HOST_NAME_MAX + 1];
    if (gethostname(hostName, HOST_NAME_MAX + 1) != 0)
    {
        LOG_ERROR << "Failed reading host name" << std::endl;
        return string();
    }

    return string(hostName);
}


set<string> HostInfo::GetConnectedUsers() const
{
    lock_guard<mutex> lock(m_connectedUsersLock);
    return m_connectedUsers;
}


// Returns Host capability name corresponding to given host capability bit or the bit number when capability is not defined
std::string HostCapaToString(CAPABILITIES capa)
{
#define CAPA_NAME_FROM_CAPA_BIT(capaBit) case (capaBit): return #capaBit;

    // the only supported message types are RTM_NEWLINK and RTM_DELLINK
    switch (capa)
    {
        CAPA_NAME_FROM_CAPA_BIT(COLLECTING_LOGS);
        CAPA_NAME_FROM_CAPA_BIT(KEEP_ALIVE);
        CAPA_NAME_FROM_CAPA_BIT(READ_BLOCK_FAST);
        CAPA_NAME_FROM_CAPA_BIT(INTERFACE_NOTIFICATIONS);
        CAPA_NAME_FROM_CAPA_BIT(LOG_COLLECTOR_JSON_API);
        CAPA_NAME_FROM_CAPA_BIT(CONTINUOUS_PMC);
        CAPA_NAME_FROM_CAPA_BIT(DUMP_JSON_API);
        CAPA_NAME_FROM_CAPA_BIT(DEVICE_ACCESS_JSON_API);
        CAPA_NAME_FROM_CAPA_BIT(LEGACY_PMC_JSON_API);
        CAPA_NAME_FROM_CAPA_BIT(DEVICE_COMMANDS_JSON_API);
        CAPA_NAME_FROM_CAPA_BIT(HOST_COMMANDS_JSON_API);

    default:
        return "UNDEFINED CAPABILITY";
    }
}

std::vector<string> HostInfo::HostCapaToVector(uint32_t capabilities) const
{
    std::vector<string> hostCapaNames;
    static const int bitsInDword = 32;
    // go over the bits per dword
    for (int pos = 0; pos < bitsInDword; pos++)
    {
        if (capabilities & (1 << pos))
        {
            hostCapaNames.emplace_back(HostCapaToString(static_cast<CAPABILITIES>(pos)));
        }
    }
    return hostCapaNames;
}

void HostInfo::AddNewConnectedUser(const string& user)
{
    lock_guard<mutex> lock(m_connectedUsersLock);
    m_connectedUsers.insert(user);
}

void HostInfo::RemoveConnectedUser(const string& user)
{
    lock_guard<mutex> lock(m_connectedUsersLock);
    m_connectedUsers.erase(user);
}

void HostInfo::SetHostCapabilities()
{
    m_capabilityMask.set(COLLECTING_LOGS);
    m_capabilityMask.set(KEEP_ALIVE);
    m_capabilityMask.set(READ_BLOCK_FAST);
    m_capabilityMask.set(INTERFACE_NOTIFICATIONS);
    m_capabilityMask.set(LOG_COLLECTOR_JSON_API);
    m_capabilityMask.set(CONTINUOUS_PMC);
    m_capabilityMask.set(DUMP_JSON_API);
    m_capabilityMask.set(DEVICE_ACCESS_JSON_API);
    m_capabilityMask.set(LEGACY_PMC_JSON_API);
    m_capabilityMask.set(DEVICE_COMMANDS_JSON_API);
    m_capabilityMask.set(HOST_COMMANDS_JSON_API);
}

void HostInfo::CreateHostDirectories()
{
    // configuration folder

    // output files folder
    if (!FileSystemOsAbstraction::DoesFolderExist(FileSystemOsAbstraction::GetOutputFilesLocation()))
    {
        const OperationStatus os = FileSystemOsAbstraction::CreateFolder(FileSystemOsAbstraction::GetOutputFilesLocation());
        if (!os)
        {
            LOG_WARNING << os.GetStatusMessage();
        }
    }
}

unordered_set<uint32_t> HostInfo::Get3ppIgnoreEvents() const
{
    if (!Is3ppPlatform())
    {
        return {};
    }

    const string ignoreEventsStr = Host::GetHost().GetConfiguration().HostConfiguration.Ignore3ppFwEventIds;

    if (ignoreEventsStr.empty())
    {
        return {};
    }

    unordered_set<uint32_t> ignoreEvents;
    stringstream ss(ignoreEventsStr);
    string eventIdStr;
    uint32_t eventId;
    while(getline(ss, eventIdStr,','))
    {
        if(stringstream(eventIdStr) >> std::hex >> eventId)
        {
            ignoreEvents.insert(eventId);
        }
    }
    return ignoreEvents;
}

bool HostInfo::Is3ppPlatform() const
{
    return strncmp(GetPlatformName(), "3PP", 3) == 0;
}