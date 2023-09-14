/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#include <sys/inotify.h>
#include <sys/timerfd.h>
#include <sys/un.h> // for socket address
#include <dirent.h> // for opendir
#include <unistd.h> // for write
#include <net/if.h>
#include <string.h>
#include <limits.h>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "UnixDPDKInterfaceMonitor.h"
#include "FileSystemOsAbstraction.h"
#include "DebugLogger.h"
#include "Utils.h"
#include "DriverAPI.h" // for DeviceType

namespace
{
    const char *const path = "/dev/wil6210";
    static const std::string EXIT_SOCKET = "exitSocket";
    static const std::string INOTIFY = "inotify";
    const static int MAX_NUM_RETRY = 10;
    static const int RECONNECT_TIMEOUT_MSEC = 5;

    std::string FormatInterfaceNames(const char* prefix, const std::set<std::string>& container)
    {
        std::ostringstream ss;
        if (prefix)
        {
            ss << prefix;
        }

        if (container.empty())
        {
            ss << "none";
        }
        else if (container.size() == 1U)
        {
            ss << *container.cbegin();
        }
        else
        {
            std::copy(container.cbegin(), std::prev(container.cend()), std::ostream_iterator<std::string>(ss, ", "));
            ss << *std::prev(container.cend());
        }

        return ss.str();
    }
}
UnixDPDKInterfaceMonitor::UnixDPDKInterfaceMonitor() :
    m_deviceType(DeviceType::DPDK),
    m_debugLogPrefix("[DPDK Interface Monitor] "),
    m_exitSockets{ { -1, -1 } }, // aggregate initialization
    m_stopDPDKInterfaceMonitorThread(false)
{
}

UnixDPDKInterfaceMonitor::~UnixDPDKInterfaceMonitor()
{
    for (auto it = m_fds.begin(); it != m_fds.end(); it++)
    {
        close(it->fd);
    }

    StopMonitorThread();
}

// Cancel blocking polling for incoming netlink events to allow graceful thread shutdown
void UnixDPDKInterfaceMonitor::StopMonitorThread()
{
    // Stop driver control event thread:
    m_stopDPDKInterfaceMonitorThread.store(true);

    // unblocks the monitor receive socket for termination
    // sockets pair serves as a pipe - a value written to one of its sockets, is also written to the second one
    // actual value written has no importance
    if (m_exitSockets[0] >= 0)
    {
        write(m_exitSockets[0], "T", 1);
    }

    if (m_monitorThread.joinable())
    {
        m_monitorThread.join();
    }

    // release sockets from the sockets pair used to stop the monitor thread
    CloseSocketPair();
}

// Release sockets from the sockets pair, used to stop the monitor thread
void UnixDPDKInterfaceMonitor::CloseSocketPair()
{
    if (m_exitSockets[0] >= 0)
    {
        close(m_exitSockets[0]);
        m_exitSockets[0] = -1;
    }
    if (m_exitSockets[1] >= 0)
    {
        close(m_exitSockets[1]);
        m_exitSockets[1] = -1;
    }
}

void UnixDPDKInterfaceMonitor::ReportIntefaces(const std::set<std::string> &prevInterfaceNames)
{
    LOG_INFO << m_debugLogPrefix << FormatInterfaceNames("list of interfaces before current event handling: ", prevInterfaceNames) << std::endl;
    LOG_INFO << m_debugLogPrefix << FormatInterfaceNames("list of interfaces after current event handling: ", m_currentInterfaceNames) << std::endl;

    decltype(m_currentInterfaceNames) interfacesAdded, interfacesRemoved;
    // updated \ original => added
    std::set_difference(m_currentInterfaceNames.cbegin(), m_currentInterfaceNames.cend(),
        prevInterfaceNames.cbegin(), prevInterfaceNames.cend(),
        std::inserter(interfacesAdded, interfacesAdded.begin()));
    // original \ updated => removed
    std::set_difference(prevInterfaceNames.cbegin(), prevInterfaceNames.cend(),
        m_currentInterfaceNames.cbegin(), m_currentInterfaceNames.cend(),
        std::inserter(interfacesRemoved, interfacesRemoved.begin()));

    // report interface changes, if any
    // assume callbacks are valid, checked before monitor thread creation
    // in case of interface rename, report removal before new interface addition
    if (!interfacesRemoved.empty())
    {
        LOG_INFO << FormatInterfaceNames("Removed existing interface: ", interfacesRemoved) << std::endl;
        m_interfaceRemovedFunc(m_deviceType, interfacesRemoved);
    }
    if (!interfacesAdded.empty())
    {
        LOG_INFO << FormatInterfaceNames("Added new interface: ", interfacesAdded) << std::endl;
        m_interfaceAddedFunc(m_deviceType, interfacesAdded);
    }
}

bool UnixDPDKInterfaceMonitor::M_fdsEraseFd(int fd, const char* interfaceName)
{
    auto iterator = std::find_if(m_fds.begin(), m_fds.end(), [&fd](const pollfd& ppfd) { return ppfd.fd == fd; });

    if (iterator == m_fds.end())
    {
        LOG_ERROR << m_debugLogPrefix << "Could not find fd " << fd << " for interface " << interfaceName << " to delete" << std::endl;
        return false;
    }
    m_fds.erase(iterator);
    return true;
}

void UnixDPDKInterfaceMonitor::M_fdsAddPollfd(int fd, int numOfRetry, const std::string interfaceName)
{
    struct pollfd pfd {};
    pfd.fd = fd;
    pfd.events = POLLIN;
    m_fds.emplace_back(pfd);
    m_fdToIface.insert({ fd, {interfaceName, numOfRetry} });
}

void UnixDPDKInterfaceMonitor::FindExistingInterfaces()
{
    DIR *dp = opendir(path);
    if (!dp)
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to open dir: " << path << std::endl;
        return;
    }

    struct dirent *de = nullptr;
    while ((de = readdir(dp)) != nullptr)
    {
        std::string iface(de->d_name);
        std::size_t pos = iface.find("wil6210_");
        if (pos > 0)
        {
            LOG_VERBOSE << m_debugLogPrefix << "File with unmatched name found: " << de->d_name << std::endl;
            continue;
        }

        LOG_DEBUG << m_debugLogPrefix << "Existing interface found: " << de->d_name << std::endl;

        int fd = TryConnectToInterface(de->d_name);
        if (fd < 0)
        {
            fd = SetTimerFd(de->d_name);
            if (fd < 0)
            {
                LOG_ERROR << m_debugLogPrefix << "Failed to create timer fd for " << de->d_name  << std::endl;
                continue;
            }
            LOG_INFO << m_debugLogPrefix << "Set timerfd for interface: " << de->d_name  << std::endl;
        }

        M_fdsAddPollfd(fd, 1, de->d_name);
    }

    closedir(dp);
}

void UnixDPDKInterfaceMonitor::StartMonitoring(InterfaceChangedFunc interfaceAddedFunc,
                                               InterfaceChangedFunc interfaceRemovedFunc)
{
    if ( !(interfaceAddedFunc && interfaceRemovedFunc) ) // shouldn't happen
    {
        LOG_ERROR << m_debugLogPrefix << "Cannot start monitoring with invalid callbacks" << std::endl;
        return;
    }

    m_interfaceAddedFunc = std::move(interfaceAddedFunc);
    m_interfaceRemovedFunc = std::move(interfaceRemovedFunc);

    if (!InitInotify())
    {
        return;
    }

    if (!InitExitSocket())
    {
        return;
    }

    // create thread to wait on the netlink blocking poll
    // Thread should be created before the first enumeration to prevent race
    m_monitorThread = std::thread(&UnixDPDKInterfaceMonitor::DPDKInterfaceMonitorThread, this);

    return;
}

bool UnixDPDKInterfaceMonitor::InitExitSocket()
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, m_exitSockets.data()) == -1) // shouldn't happen
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to create cancellation socket pair" << std::endl;
        return false;
    }

    M_fdsAddPollfd(m_exitSockets[1], 0, EXIT_SOCKET);

    return true;
}

bool UnixDPDKInterfaceMonitor::InitInotify()
{
    // folder must exist or else inotify watch will fail
    OperationStatus os = FileSystemOsAbstraction::CreateFolder(path);
    if (!os)
    {
        LOG_WARNING << m_debugLogPrefix << "Failed to create path: " << path << ", " << os.GetStatusMessage() << ", Ignore this warning for non DPDK platforms." << std::endl;
        return false;
    }

    int fd = inotify_init1(IN_NONBLOCK);
    if (fd < 0)
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to init inotify: " << strerror(errno) << std::endl;
        return false;
    }

    // Mark directorie for events
    //- file was opened
    //- file was closed
    int ret = inotify_add_watch(fd, path, IN_CREATE | IN_DELETE);
    if (ret < 0)
    {
        LOG_ERROR << m_debugLogPrefix << "Cannot watch " << path << " : " << strerror(errno) << std::endl;
        return false;
    }

    M_fdsAddPollfd(fd, 0, INOTIFY);

    return true;
}

void UnixDPDKInterfaceMonitor::DPDKInterfaceMonitorThread()
{
    FindExistingInterfaces();

    while (!m_stopDPDKInterfaceMonitorThread.load())
    {
        const bool dataReady = WaitForEventsBlocking();

        if (m_stopDPDKInterfaceMonitorThread.load())
        {
            // asked to quit
            break;
        }

        if (!dataReady)
        {
            // error was already displayed
            continue;
        }

        ReadAndHandleEvents();
    }

    LOG_INFO << "DPDKInterfaceMonitorThread was asked to quit" << std::endl;
}



// Poll for incoming events and return when data is available for read or asked to quit (write to a cancellation socket)
// Returns true when data is ready to read
bool UnixDPDKInterfaceMonitor::WaitForEventsBlocking()
{
    int count = poll(m_fds.data(), m_fds.size(), -1);

    if (count < 0)
    {
        // Interrupted system call, we do not want to see this message all the time
        if (errno == EINTR)
        {
            return false;
        }
        LOG_ERROR << m_debugLogPrefix << "Poll failed: " << strerror(errno) << std::endl;
        return false;
    }
    return (count > 0);
}

int UnixDPDKInterfaceMonitor::TryConnectToInterface(const char* interfaceName)
{
    int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (fd < 0)
    {
        return fd;
    }

    struct sockaddr_un addr {};
    addr.sun_family = AF_LOCAL;

    std::ostringstream fname;
    fname << path << "/" << interfaceName;

    SafeStringCopy(addr.sun_path, fname.str().c_str(), sizeof(addr.sun_path)); // check

    if (connect(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        close(fd);
        return -1;
    }

    LOG_DEBUG << m_debugLogPrefix << "Successful connection to interface: " << interfaceName << " fd=" << fd << std::endl;

    auto prevInterfaceNames = m_currentInterfaceNames;
    m_currentInterfaceNames.emplace(interfaceName);
    ReportIntefaces(prevInterfaceNames);

    return fd;
}

int UnixDPDKInterfaceMonitor::SetTimerFd(const char* interfaceName)
{
    int timerfd = timerfd_create(CLOCK_REALTIME, 0);
    if (timerfd < 0)
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to create timer fd for " << interfaceName << " : " << timerfd << std::endl;
        return timerfd;
    }

    // nsec cannot be higher than 1 sec or else faulty behavior will occur.
    struct itimerspec time{};
    time.it_value.tv_sec = 0;
    time.it_value.tv_nsec = RECONNECT_TIMEOUT_MSEC * 1000000;
    time.it_interval.tv_sec = 0;
    time.it_interval.tv_nsec = 0;

    if (timerfd_settime(timerfd, 0, &time, NULL) < 0) // on error return -1
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to start timer fd for " << interfaceName << " : " << strerror(errno) << std::endl;
        close(timerfd);
        return -1;
    }
    return timerfd;
}

void UnixDPDKInterfaceMonitor::HandleNewInterface(const char* interfaceName)
{
    LOG_DEBUG << m_debugLogPrefix << "New interface created: " << interfaceName << std::endl;

    int fd = TryConnectToInterface(interfaceName);
    if (fd < 0)
    {
        fd = SetTimerFd(interfaceName);
        if (fd < 0)
        {
            LOG_ERROR << m_debugLogPrefix << "Failed to create timer fd for " << interfaceName << " : " << fd << std::endl;
            return;
        }
        LOG_INFO << m_debugLogPrefix << "Set timerfd for interface: " << interfaceName << " fd=" << fd << std::endl;
    }

    M_fdsAddPollfd(fd, 1, interfaceName);
}

void UnixDPDKInterfaceMonitor::HandleDeletedInterface(const char* interfaceName)
{
    LOG_INFO << m_debugLogPrefix << "File deleted: " << interfaceName << std::endl;

    // range loop can be used here since we break the loop when element is found,
    // but it's more safe to use 'erase' result if the flow changes
    for (auto it = m_fdToIface.begin(); it != m_fdToIface.end(); )
    {
        if (!it->second.name.compare(interfaceName))
        {
            const int fd = it->first;
            LOG_INFO << m_debugLogPrefix << "Deleting interface: " << interfaceName << " fd=" << fd << std::endl;
            it = m_fdToIface.erase(it);

            struct itimerspec time {}; // zero disarms the timer
            if (timerfd_settime(fd, 0, &time, NULL) < 0) // on error return -1
            {
                LOG_WARNING << m_debugLogPrefix << "Failed to stop timer fd for " << interfaceName << " : " << strerror(errno) << std::endl;
            }
            close(fd);

            if (!M_fdsEraseFd(fd, interfaceName))
            {
                return;
            }

            auto prevInterfaceNames = m_currentInterfaceNames;
            m_currentInterfaceNames.erase(interfaceName);
            ReportIntefaces(prevInterfaceNames);
            break;
        }
        else
        {
            ++it;
        }
    }
}

void UnixDPDKInterfaceMonitor::HandleInotifyEvent()
{
    static const int BUF_LEN = (10 * (sizeof(struct inotify_event) + NAME_MAX)); // 10 events is suppose to be enough
    static const int INOTIFY_EVENT_SIZE = (sizeof(struct inotify_event));

    for (;;)
    {
        char buf[BUF_LEN];
        int len = read(m_fds.front().fd, buf, BUF_LEN);
        if (len == -1 && errno != EAGAIN)
        {
            LOG_ERROR << m_debugLogPrefix << "Failed to read from inotify fd: " << strerror(errno) << std::endl;
            return;
        }

        // in case read returns -1 with errno set to EAGAIN it mean there is no events to read and we exit the loop.
        if (len <= 0)
        {
            break;
        }

        //process events one by one
        int i = 0;
        while (i < len)
        {
            const struct inotify_event *event = reinterpret_cast<struct inotify_event *>(&buf[i]);

            if (!event->len)
            {
                i += INOTIFY_EVENT_SIZE + event->len;
                continue;
            }

            if (event->mask & IN_CREATE)
            {
                HandleNewInterface(event->name);
            }

            if (event->mask & IN_DELETE)
            {
                HandleDeletedInterface(event->name);
            }

            i += INOTIFY_EVENT_SIZE + event->len;
        }
    }
}

void UnixDPDKInterfaceMonitor::HandleTimerFdEvent(int pfd)
{
    LOG_DEBUG << m_debugLogPrefix << "Timerfd event for interface: " << m_fdToIface[pfd].name.c_str() << " fd=" << pfd << " retry number " << (m_fdToIface[pfd].numOfRetry) << std::endl;

    uint64_t expirations = 0;
    ssize_t len = read(pfd, &expirations, sizeof(uint64_t)); //we wont have more that 1 expiration per fd so while not needed
    if (len < 0)
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to read from timerfd " << strerror(errno) << std::endl;
        return;
    }

    int fd = TryConnectToInterface(m_fdToIface[pfd].name.c_str());
    if (fd < 0)
    {
        if (m_fdToIface[pfd].numOfRetry < MAX_NUM_RETRY)
        {
            m_fdToIface[pfd].numOfRetry++;

            fd = SetTimerFd(m_fdToIface[pfd].name.c_str());
            if (fd < 0)
            {
                LOG_ERROR << m_debugLogPrefix << "Failed to create timer fd for " << m_fdToIface[pfd].name.c_str() << " : " << fd << std::endl;
                return;
            }
        }
        else
        {
            LOG_ERROR << m_debugLogPrefix << "Failed to connect to " << m_fdToIface[pfd].name.c_str() << " stop trying" << std::endl;
        }
    }

    if (fd >= 0)
    {
        // new fd for watching that the socket is alive
        M_fdsAddPollfd(fd, m_fdToIface[pfd].numOfRetry, m_fdToIface[pfd].name.c_str());
    }

    // delete the previous timerfd
    M_fdsEraseFd(pfd, m_fdToIface[pfd].name.c_str());
    m_fdToIface.erase(pfd);
    close(pfd);
}

void UnixDPDKInterfaceMonitor::HandleClosedConnection(int pfd)
{
    LOG_DEBUG << m_debugLogPrefix << "Remote connection hanged up: " << m_fdToIface[pfd].name.c_str() << " fd=" << pfd << std::endl;

    auto prevInterfaceNames = m_currentInterfaceNames;
    m_currentInterfaceNames.erase(m_fdToIface[pfd].name.c_str());
    ReportIntefaces(prevInterfaceNames);

    M_fdsEraseFd(pfd, m_fdToIface[pfd].name.c_str());
    m_fdToIface.erase(pfd);
    close(pfd);
}

void UnixDPDKInterfaceMonitor::ReadAndHandleEvents()
{
    //more than one fd can be triggered per poll
    const std::vector<pollfd> fds(m_fds);
    for (const auto &pfd : fds) // iterate over a local copy, source can be modified
    {
        auto it = m_fdToIface.find(pfd.fd);
        if (it == m_fdToIface.end())
        {
            return;
        }

        struct interface *iface = &it->second;

        if (iface->name.compare(INOTIFY) == 0)
        {
            if (pfd.revents & POLLIN)
            {
                HandleInotifyEvent();
            }
        }
        else
        {
            if (pfd.revents & POLLIN) // Data is ready
            {
                HandleTimerFdEvent(pfd.fd);
            }

            if (pfd.revents & POLLHUP) // Remote connection hanged up
            {
                HandleClosedConnection(pfd.fd);
            }
        }
    }
}
