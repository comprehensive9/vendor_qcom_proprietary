/*
* Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <memory>
#include <string>
#include <atomic>

namespace TcpNetworkInterfaces {
    class NetworkInterface;
}

class Host;

/*
* Json Commands TCP Server is a synchronous server that handles each client separately.
* When a client sends a new json command to the Json Commands TCP Server, the host processes it and sends back to the client an answer.
*/
class JsonCommandsTcpServer
{
public:

    explicit JsonCommandsTcpServer(Host& host);
    bool Init(uint16_t port);

    /*
    * Start the json commands TCP server at the given port (given in the constructor).
    * For each new client that is connecting to the server it opens a new thread and
    * continue to listen on the port for more clients.
    */
    void Start();

    /*
    * Stop the json commands TCP server by doing some clean ups for the sockets.
    */
    void Stop();

private:
    std::unique_ptr<TcpNetworkInterfaces::NetworkInterface> m_pSocket; //an object that holds the connection with each client
    Host& m_host; // reference to the host object (that is passed to jsonCommandsHandler each time a new TCP connection is created)

    void ServerThread(std::unique_ptr<TcpNetworkInterfaces::NetworkInterface> client);
    bool Reply(TcpNetworkInterfaces::NetworkInterface *client, std::string &responseMessage);

    std::atomic<bool> m_running;
};
