/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <cerrno>
#include <string>
#include <cstdlib>
#include <iostream>
#include <cerrno>
#include <cstring>

#ifndef _WINDOWS
    #include <netinet/tcp.h>
    #define SOCKET_ERROR            (-1)
#else
    #include <winsock2.h>
#endif

#include "TcpClient.h"
#include "LogHandler.h"

using namespace std;

TcpClient::TcpClient(): m_socketFD(-1)
{
#ifdef _WINDOWS
    // Calling WSAStartup is necessary to be called once for a process. we Must call this function
    // in order to start using the winsock2 under Windows platforms. When not needed any more,
    // WSACleanup is needed to be called (not calling it wouldn't be so bad, but it is preferred)

    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 0), &wsa);
    if (0 != err)
    {
        LOG_ERROR << "Cannot initialize network library (WSAStartup) with error: " << err << std::endl;
    }
#endif
}


TcpClient::~TcpClient()
{
#ifdef _WINDOWS
    closesocket(m_socketFD);
    WSACleanup();
#else
    close(m_socketFD);
#endif
}

bool TcpClient::ConnectSendCommandGetResponse(const string & command, const string & ip, const string &port, string &response)
{
    return (ConnectToHost(ip, port) && SendCommand(command) && GetResponse(response));
}


bool TcpClient::ConnectToHost(const string& hostName, const string& port)
{
    int portNum = atoi(port.c_str());
    if (portNum < 1023 || portNum > 65535) // min port is 1023, max port is 65535
    {
        LOG_ERROR << "Invalid Port was provided: " << port << endl;
        return false;
    }

    struct sockaddr_in6 localAddress {};
    struct addrinfo hints {};
    struct addrinfo  *pRes = nullptr;

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_socktype = SOCK_STREAM;
    if (inet_pton(AF_INET, hostName.c_str(), &localAddress) == 1)    /* valid IPv4 text address? */
    {
        hints.ai_family = AF_INET;
        hints.ai_flags |= AI_NUMERICHOST;
    }
    else if (inet_pton(AF_INET6, hostName.c_str(), &localAddress) == 1) /* valid IPv6 text address? */
    {
        hints.ai_family = AF_INET6;
        hints.ai_flags |= AI_NUMERICHOST;
    }
    else // The provided hostName is a machine name and not an IP
    {
        hints.ai_family = AF_UNSPEC;
    }

    // Here hints.ai_family is set to right v
    int rc = getaddrinfo(hostName.c_str(), port.c_str(), &hints, &pRes);
    if (rc != 0)
    {
        LOG_ERROR << "Invalid target was provided: " << hostName << endl;
        return false;
    }

    m_socketFD = socket(pRes->ai_family, pRes->ai_socktype, pRes->ai_protocol);
    if (m_socketFD < 0)
    {
        LOG_ERROR << "Could not create socket. Error: " << strerror(errno) << endl;
        return false;
    }

    if (connect(m_socketFD, pRes->ai_addr, static_cast<int>(pRes->ai_addrlen)) < 0)
    {
        LOG_ERROR << "Connect failed to: " << hostName << ": " << port << endl;
        return false;
    }

    return true;
}


bool TcpClient::SendCommand(const std::string& command) const
{
    const char* const pBuf = command.c_str();
    const size_t  bufSize = command.size();

    LOG_INFO << "Sending command: " << command << std::endl;
    if (bufSize == 0)
    {
        LOG_ERROR << "Cannot send empty command" << endl;
        return false;
    }
    size_t sentSize = 0;
    const char* pCurrent = pBuf;

    while (sentSize < bufSize)
    {
        int chunkSize = send(m_socketFD, pCurrent, static_cast<int>(bufSize - sentSize), 0);
        if (chunkSize < 0)
        {
            LOG_ERROR << "Error sending data."
                << " Error: " << strerror(errno)
                << " Sent till now (B): " << sentSize
                << " To be sent (B): " << bufSize
                << endl;
            return false;
        }

        sentSize += chunkSize;
        pCurrent += chunkSize;

        LOG_INFO << "Sent data chunk."
            << " Chunk Size (B): " << chunkSize
            << " Sent till now (B): " << sentSize
            << " To be sent (B): " << bufSize
            << endl;
    }

    LOG_INFO << "Buffer sent successfully."
        << " Sent (B): " << sentSize
        << " Buffer Size (B): " << bufSize
        << endl;

    if (sentSize != bufSize)
    {
        LOG_ERROR << "Fail to send JSON command, check connection to target" << std::endl;
        return false;
    }
    return true;
}


bool TcpClient::GetResponse(std::string& response) const
{
    struct timeval timeout {};
    timeout.tv_sec = 5; // set timeout to 5 seconds
    timeout.tv_usec = 0;

    // Response will be accumulated from received chunks
    response.clear();

    // Temporary storage for data receiving
    const size_t BUFFER_SIZE_BYTES = 4096;
    char buffer[BUFFER_SIZE_BYTES];

    fd_set set;
    FD_ZERO(&set);
    FD_SET(m_socketFD, &set);

    do
    {
        int rv = select(m_socketFD+1, &set, nullptr, nullptr, &timeout);
        if (rv == SOCKET_ERROR) // No FD available for selection
        {
            LOG_ERROR << "Select error: " << strerror(errno) << endl;
            return false;
        }

        if (rv == 0) // Timeout occurred (connection stopped not closed properly by the target)
        {
            LOG_ERROR << "Connection timeout" << std::endl;
            return false;
        }

        // Response chunk has been received
        // socket has something to read
        const int bytesReceived = recv(m_socketFD, buffer, BUFFER_SIZE_BYTES, 0 /*flags*/);
        if (bytesReceived == SOCKET_ERROR) //unexpected error
        {
            LOG_ERROR << "Error while receiving from a TCP socket: " << strerror(errno) << std::endl;
            return false;
        }

        if (bytesReceived == 0) // nothing to read or buffer is full
        {
            LOG_INFO << "Connection closed by peer Response length (B): " << response.size() << std::endl;
            return true;
        }

        LOG_INFO << "Received response chunk of " << bytesReceived << " B" << endl;
        response.append(buffer, buffer + bytesReceived);
    }
    while (true);
}
