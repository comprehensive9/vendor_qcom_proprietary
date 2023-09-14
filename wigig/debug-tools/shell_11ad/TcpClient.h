/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#ifdef _WINDOWS
    #pragma comment(lib, "Ws2_32.lib")
#endif

#include <string>
#include <vector>

#ifdef _WINDOWS
    #include <winsock2.h>
    #include <Ws2tcpip.h>/* Added to support IPv6*/
#elif __linux
    #include <unistd.h>
    #include <netdb.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <sys/wait.h>
#else
    #include <sys/socket.h>
    #include <net/route.h>
    #include <net/if.h>
    #include <arpa/inet.h>
#endif

#ifdef _WINDOWS
    typedef int socklen_t;
#endif

class TcpClient
{
public:
    TcpClient();
    ~TcpClient();

    bool ConnectSendCommandGetResponse(
        const std::string& command, const std::string &ip, const std::string &port, std::string &response);

private:
    bool ConnectToHost(const std::string& hostName, const std::string& port);
    bool SendCommand(const std::string &command) const;
    bool GetResponse(std::string &response) const;

    int m_socketFD;
};