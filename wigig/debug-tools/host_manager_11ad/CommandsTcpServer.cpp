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

#include "CommandsTcpServer.h"
#include "FileReader.h"
#include "Host.h"
#include "HostInfo.h"
#include "EventsDefinitions.h"
#include "TaskManager.h"
#include "DebugLogger.h"
#include "TcpNetworkInterface.h"
#include "CommandsHandler.h"

#include <thread>

using namespace std;

// *************************************************************************************************

CommandsTcpServer::CommandsTcpServer(Host& host) :
    m_pSocket(new TcpNetworkInterfaces::NetworkInterface("Commands")),
    m_host(host),
    m_running(true)
{
}

// *************************************************************************************************

bool CommandsTcpServer::Init(uint16_t port)
{
    LOG_INFO << "Init commands TCP server on port " << port << endl;
    return m_pSocket->Init(port);
}

// *************************************************************************************************

void CommandsTcpServer::Start()
{
    LOG_INFO << "Starting commands TCP server" << endl;

    //Infinite loop that waits for clients to connect to the commands TCP server unless stopped (by request of the user or a problem)
    while (m_running.load())
    {
        try
        {
            thread serverThread(&CommandsTcpServer::ServerThread, this, m_pSocket->Accept()); //open a new thread for each client
            serverThread.detach();
        }
        catch (const exception &e)
        {
            LOG_ERROR << "Couldn't make a new connection or starting a new thread in Commands TCP server for a new client " << e.what() << endl;
        }
    }

    LOG_INFO << "Command TCP Server stopped - no more client connections will be accepted" << std::endl;
}

void CommandsTcpServer::Stop()
{
    LOG_VERBOSE << "Stopping the commands TCP server" << endl;
    m_pSocket.reset();
    m_running.store(false);
}



// *************************************************************************************************
//A thread function to handle each client that connects to the server
void CommandsTcpServer::ServerThread(unique_ptr<TcpNetworkInterfaces::NetworkInterface> client)
{
    unique_ptr<CommandsHandler> pCommandsHandler(new CommandsHandler);
    bool keepConnectionAliveFromCommand = true; //A flag for the content of the command - says if the client wants to close connection
    bool keepConnectionAliveFromReply = true; //A flag for the reply status, for problems in sending reply etc..
    // notify that new client is connected to the host (send list of connected users before adding the new one, and a notification of the new one)
    m_host.PushEvent(ClientConnectedEvent(m_host.GetHostInfo().GetConnectedUsers(), client->GetPeerName()));
    m_host.GetHostInfo().AddNewConnectedUser(client->GetPeerName()); // add the user's to the host's connected users

    LOG_INFO << "Starting server thread for " << client->GetPeerName() << std::endl;

    try
    {
        do
        {
            string singleMessage;
            if (!client->Receive(singleMessage) || singleMessage.empty())
            {   //message back from the client is "", means the connection is closed
                break;
            }

            LOG_VERBOSE << "Message from Client to commands TCP server: " << singleMessage << endl;

            //Try to execute the command from the client, get back from function if to keep the connection with the client alive or not
            ResponseMessage referencedResponse;
            keepConnectionAliveFromCommand = pCommandsHandler->ExecuteCommand(singleMessage, referencedResponse);

            //Reply back to the client an answer for his command. If it wasn't successful - close the connection
            keepConnectionAliveFromReply = CommandsTcpServer::Reply(client.get(), referencedResponse);

       }
        while (keepConnectionAliveFromCommand && keepConnectionAliveFromReply);
    }
    catch (const exception &e)
    {
        LOG_ERROR << "There was a problem with the server thread of commands TCP server, error: " << e.what() << endl;
    }

    LOG_INFO << "Closed connection with the client: " << client->GetPeerName() << endl;
    m_host.GetHostInfo().RemoveConnectedUser(client->GetPeerName());
    //notify that new client is disconnected from the host
    m_host.PushEvent(ClientDisconnectedEvent(m_host.GetHostInfo().GetConnectedUsers(), client->GetPeerName()));
}

// *************************************************************************************************

bool CommandsTcpServer::Reply(TcpNetworkInterfaces::NetworkInterface *client, ResponseMessage &responseMessage)
{
    LOG_VERBOSE << "Replying from a buffer (" << responseMessage.length << "B) Content: " << responseMessage.message << endl;

    if (0 == responseMessage.length)
    {
        LOG_ERROR << "No reply generated by a command handler - connection will be closed" << endl;
        return false;
    }

    //TODO - maybe the sending format is ending with "\r\n"
    if (!client->SendString(responseMessage.message + "\r"))
    {
        LOG_ERROR << "Couldn't send the message to the client, closing connection" << endl;
        return false;
    }

    return true;
}