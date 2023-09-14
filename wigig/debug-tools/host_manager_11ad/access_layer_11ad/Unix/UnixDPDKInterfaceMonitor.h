/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <vector>
#include <thread>
#include <set>
#include <map>
#include <functional> // for std::function
#include <atomic>
#include <string>
#include <array>
#include <poll.h>

enum class DeviceType : int;

// callback for notification on added/removed network interface
using InterfaceChangedFunc = std::function<void(DeviceType deviceType, const std::set<std::string>&)>;



// Singleton class to enumerate and monitor DPDK network interfaces
class UnixDPDKInterfaceMonitor final
{
public:
    UnixDPDKInterfaceMonitor();
    ~UnixDPDKInterfaceMonitor();

    void StartMonitoring(InterfaceChangedFunc interfaceAddedFunc,
                         InterfaceChangedFunc interfaceRemovedFunc);

private:
    const DeviceType m_deviceType;
    const std::string m_debugLogPrefix;

    struct interface
    {
        std::string name;
        int numOfRetry;
    };

    std::array<int, 2U> m_exitSockets;      // exit socket used for graceful shutdown
    std::set<std::string> m_currentInterfaceNames;
    std::atomic<bool> m_stopDPDKInterfaceMonitorThread;

    InterfaceChangedFunc m_interfaceAddedFunc;
    InterfaceChangedFunc m_interfaceRemovedFunc;

    void StopMonitorThread();
    void CloseSocketPair();//close
    void DPDKInterfaceMonitorThread();
    bool WaitForEventsBlocking();
    void ReadAndHandleEvents();


    //comment all functions
    bool InitInotify();
    bool InitExitSocket();
    int TryConnectToInterface(const char* interfaceName);
    void HandleInotifyEvent();
    void HandleTimerFdEvent(int pfd);
    void HandleClosedConnection(int pfd);
    void HandleNewInterface(const char* interfaceName);
    void HandleDeletedInterface(const char* interfaceName);
    int SetTimerFd(const char* interfaceName);
    void FindExistingInterfaces();
    void ReportIntefaces(const std::set<std::string> &prevInterfaceNames);
    bool M_fdsEraseFd(int fd, const char* interfaceName);
    void M_fdsAddPollfd(int fd, int numOfRetry, const std::string interfaceName);

    std::thread m_monitorThread;

    std::vector<pollfd> m_fds;
    std::map<int, struct interface> m_fdToIface; //mapping from fd to interface
};
