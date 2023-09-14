/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h> // for opendir
#include <unistd.h> // for write
#include <libgen.h> // for basename/dirname
#include <poll.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <thread>
#include <string>
#include <utility>
#include <set>
#include <algorithm>
#include <iterator>

#include "UnixNetworkInterfaceMonitor.h"
#include "FileSystemOsAbstraction.h"
#include "DebugLogger.h"
#include "Utils.h"
#include "DriverAPI.h" // for DeviceType

namespace
{
    const char* const PCI_END_POINT_FOLDER = "/sys/module/wil6210/drivers/pci:wil6210/";

    enum { NETLINK_MSG_BUFFER_SIZE = 32768 };

    // Returns the message name corresponding to given RTM type or nullptr when given type is not relevant
    const char* GetMessageNameFromRtmType(int type)
    {
        #define MESSAGE_NAME_FROM_RTM_TYPE(rtmType) case (rtmType): return #rtmType;

        // the only supported message types are RTM_NEWLINK and RTM_DELLINK
        switch (type)
        {
            MESSAGE_NAME_FROM_RTM_TYPE(RTM_NEWLINK);
            MESSAGE_NAME_FROM_RTM_TYPE(RTM_DELLINK);
        default:
            return nullptr;
        }
    }

    std::pair<std::string, int> GetInterfaceInfoFromPciEndPoint(const std::string& pciEndPoint)
    {
        // const definitions
        static const char* const CURRENT_DIRECTORY = ".";
        static const char* const PARENT_DIRECTORY = "..";

        auto interfaceInfo = std::make_pair(std::string(), 0); // init as invalid

        std::ostringstream interfaceNameContaingFolder; // path of a folder which contains the interface name of the specific EP
        interfaceNameContaingFolder << PCI_END_POINT_FOLDER << pciEndPoint << "/net";
        DIR* dp = opendir(interfaceNameContaingFolder.str().c_str());
        if (!dp)
        {
            return interfaceInfo;
        }

        // For supporting multi vap (=virtual access point), we get all the interfaces from the same end point,
        // and choose the one without the '-' (which supposed to be the physical interface).
        // If there is more than one interface without '-', we choose the first one.
        // We assume that only virtual interfaces has '-' in their name.
        // TODO : This is kind of a hack until we fully support multi VIF
        dirent* de = nullptr;
        bool foundInterface = false;
        while ((de = readdir(dp)) != nullptr)
        {
            if ((strncmp(CURRENT_DIRECTORY, de->d_name, strlen(CURRENT_DIRECTORY)) == 0) || (strncmp(PARENT_DIRECTORY, de->d_name, strlen(PARENT_DIRECTORY)) == 0))
            { // The first two directories that returned from readdir are "." and "..", of course we ignore them
                continue;
            }

            if (strchr(de->d_name, '-') == nullptr)
            { // Interface without '-' means that it is physical interface and not virtual
                LOG_VERBOSE << "Interface name from the directory '" << interfaceNameContaingFolder.str() << "' is " << de->d_name << std::endl;
                interfaceInfo.first = de->d_name;
                break;
            }
            else if (!foundInterface)
            { // In case we can't find an interface name without '-', we take the first one available
                foundInterface = true;
                interfaceInfo.first = de->d_name;
            }
        }

        closedir(dp);

        // update interface index
        interfaceInfo.second = if_nametoindex(interfaceInfo.first.c_str());
        LOG_VERBOSE << "Interface index for " << interfaceInfo.first << " is " << interfaceInfo.second << std::endl;

        return interfaceInfo;
    }

    // Extract PCI End Point name for the given interface name
    // Method: basename $(dirname $(dirname $(readlink /sys/class/net/_ifname_)))
    // (readlink result will be of the form of ../../devices/pci0000:00/0000:00:1c.5/0000:02:00.0/net/wlan0)
    std::string GetPciEndPointFromInterfaceName(const char* interfaceName)
    {
        std::ostringstream oss;
        oss << "/sys/class/net/" << interfaceName;
        std::string interfaceContaingFolder(oss.str());

        if (!FileSystemOsAbstraction::DoesFileExist(interfaceContaingFolder))
        {
            // can happen upon rmmod when we get NEWLINK
            LOG_VERBOSE << "Cannot extract PCI End Point name for " << interfaceName
                << ", path '" << interfaceContaingFolder << "' does not exist (possible for RTM_NEWLINK during rmmod)" << std::endl;
            return std::string();
        }

        std::string interfacePath;
        if (!FileSystemOsAbstraction::ReadLink(interfaceContaingFolder, interfacePath))
        {
            // error reported
            return std::string();
        }

        char* busTreePath = dirname(const_cast<char*>(interfacePath.data())); // dirname expects non-const and data() result is non-const since C++17
        busTreePath = dirname(busTreePath);
        const char* endPointName = basename(busTreePath);
        if (!endPointName)
        {
            LOG_ERROR << "Failed to extract dirname/basename to get PCI End Point name from " << interfacePath << std::endl;
            return std::string();
        }

        return std::string(endPointName);
    }

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

// === Monitor functionality === //
// Note:
// NL80211_CMD_NEW_INTERFACE & NL80211_CMD_DEL_INTERFACE events from "config" group of "nl80211" family are raised only in kernel 4.9 and later.
// These are events that dedicated for wifi interfaces, but we cannot assume that all customers have kernel version >= 4.9.
// Current method is to respond to more general network interface events and test their relevance based on corresponding
// interface name and associated host driver.

UnixNetworkInterfaceMonitor::UnixNetworkInterfaceMonitor()
    : m_deviceType(DeviceType::PCI)
    , m_debugLogPrefix("[Network Interface Monitor] ")
    , m_netlinkSocket(-1)
    , m_exitSockets{ { -1, -1 } } // aggregate initialization
    , m_eventsBuffer(std::vector<uint8_t>(NETLINK_MSG_BUFFER_SIZE))
    , m_stopNetworkInterfaceMonitorThread(false)
{
}

UnixNetworkInterfaceMonitor::~UnixNetworkInterfaceMonitor()
{
    // mark driver control event thread to exit
    m_stopNetworkInterfaceMonitorThread.store(true);

    // Cancel blocking polling for incoming netlink events to allow graceful thread shutdown
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

    // close netlink socket
    close(m_netlinkSocket);

    // release sockets from the sockets pair used to stop the monitor thread
    CloseSocketPair();
}

// Release sockets from the sockets pair, used to stop the monitor thread
void UnixNetworkInterfaceMonitor::CloseSocketPair()
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

bool UnixNetworkInterfaceMonitor::SetupNetlinkSocket()
{
    // open rtnetlink socket
    // use raw network protocol access since the netlink protocol does not distinguish between datagram and raw sockets
    m_netlinkSocket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (m_netlinkSocket < 0)
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to create netlink socket. Error: " << strerror(errno) << std::endl;
        return false;
    }

    struct sockaddr_nl nladdr;
    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_groups = RTMGRP_LINK;
    //addr.nl_pid = 0; // when set to zero, kernel will assign a unique pid

    if (bind(m_netlinkSocket, reinterpret_cast<struct sockaddr*>(&nladdr), sizeof(nladdr)) < 0)
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to bind netlink socket. Error: " << strerror(errno) << std::endl;
        close(m_netlinkSocket);
        return false;
    }

    return true;
}

void UnixNetworkInterfaceMonitor::StartMonitoring(InterfaceChangedFunc interfaceAddedFunc,
                                                  InterfaceChangedFunc interfaceRemovedFunc)
{
    if ( !(interfaceAddedFunc && interfaceRemovedFunc) ) // shouldn't happen
    {
        LOG_ERROR << m_debugLogPrefix << "Cannot start monitoring with invalid callbacks" << std::endl;
        return;
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, m_exitSockets.data()) == -1) // shouldn't happen
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to create cancellation socket pair, abort netlink socket initialization" << std::endl;
        return;
    }

    if (!SetupNetlinkSocket()) // shouldn't happen
    {
        CloseSocketPair();
        return;
    }

    m_interfaceAddedFunc = std::move(interfaceAddedFunc);
    m_interfaceRemovedFunc = std::move(interfaceRemovedFunc);

    // create thread to wait on the netlink blocking poll
    // Thread should be created before the first enumeration to prevent race
    m_monitorThread = std::thread(&UnixNetworkInterfaceMonitor::NetworkInterfaceMonitorThread, this);
}

void UnixNetworkInterfaceMonitor::NetworkInterfaceMonitorThread()
{
    // report existing interfaces
    InitializeCurrentInterfaceNames(); // initializes current interfaces and PCI End Point structure
    ReportIntefaceChanges(std::set<std::string>() /*previous state is none*/);

    while (!m_stopNetworkInterfaceMonitorThread.load())
    {
        const bool dataReady = WaitForEventsBlocking();

        if (m_stopNetworkInterfaceMonitorThread.load())
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

    LOG_INFO << m_debugLogPrefix << "NetworkInterfaceMonitorThread was asked to quit" << std::endl;
}

// Poll for incoming events and return when data is available for read or asked to quit (write to a cancellation socket)
// Returns true when data is ready to read
bool UnixNetworkInterfaceMonitor::WaitForEventsBlocking()
{
    // m_exitSockets[1] is a file descriptor for one of the sockets from the cancellation sockets pair
    // sockets pair serves as a pipe - a value written to one of its sockets, is also written to the second one
    struct pollfd fds[2];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = m_netlinkSocket;
    fds[0].events |= POLLIN;
    fds[1].fd = m_exitSockets[1];
    fds[1].events |= POLLIN;

    int count = poll(fds, 2, -1); // infinite timeout

    if (count < 0) // shouldn't happen
    {
        LOG_ERROR << m_debugLogPrefix << "failed to poll for event, error: " << strerror(errno) << std::endl;
        return false;
    }
    // no need to check for zero, timeout is infinite

    if (fds[0].revents & POLLIN) // data may be read without blocking
    {
        return true;
    }

    LOG_DEBUG << m_debugLogPrefix << "poll for events was interrupted" << std::endl;
    return false;
}

void UnixNetworkInterfaceMonitor::ReadAndHandleEvents()
{
    auto prevInterfaceNames = m_currentInterfaceNames;
    ParseAndHandleMessagesInBuffer(ReadMessagesToBuffer()); // updates m_currentInterfaceNames
    ReportIntefaceChanges(prevInterfaceNames);
}

void UnixNetworkInterfaceMonitor::ReportIntefaceChanges(const std::set<std::string> &prevInterfaceNames)
{
    LOG_VERBOSE << m_debugLogPrefix << FormatInterfaceNames("list of interfaces before current event handling: ", prevInterfaceNames) << std::endl;
    LOG_VERBOSE << m_debugLogPrefix << FormatInterfaceNames("list of interfaces after current event handling: ", m_currentInterfaceNames) << std::endl;

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
        LOG_INFO << m_debugLogPrefix << FormatInterfaceNames("Removed existing interface/s: ", interfacesRemoved) << std::endl;
        m_interfaceRemovedFunc(m_deviceType, interfacesRemoved);
    }
    if (!interfacesAdded.empty())
    {
        LOG_INFO << m_debugLogPrefix << FormatInterfaceNames("Added new interface/s: ", interfacesAdded) << std::endl;
        m_interfaceAddedFunc(m_deviceType, interfacesAdded);
    }
}

int UnixNetworkInterfaceMonitor::ReadMessagesToBuffer()
{
    struct iovec iov = { m_eventsBuffer.data(), m_eventsBuffer.size() };
    struct sockaddr_nl addr;
    memset(&addr, 0, sizeof(addr));
    // inplace initialization of the message haeader was replaced because of a compiler warning
    struct msghdr hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.msg_name = &addr;
    hdr.msg_namelen = sizeof(addr);
    hdr.msg_iov = &iov;
    hdr.msg_iovlen = 1;
    hdr.msg_control = NULL;
    hdr.msg_controllen = 0;
    hdr.msg_flags = 0;

    int count = recvmsg(m_netlinkSocket, &hdr, 0);
    if (count <= 0)
    {
        LOG_ERROR << m_debugLogPrefix << "Failed to read from netlink socket. Error: " << strerror(errno) << std::endl;
        return count;
    }

    if (addr.nl_pid != 0)
    {
        // ignore non-kernel messages
        LOG_VERBOSE << m_debugLogPrefix << "Ignored non-kernel message" << std::endl;
        return -1;
    }

    LOG_VERBOSE << m_debugLogPrefix << "Received netlink message, length " << count << " Bytes" << std::endl;

    return count;
}

bool UnixNetworkInterfaceMonitor::ParseAndHandleMessagesInBuffer(int count)
{
    if (count <= 0)
    {
        // error was reported, if any
        return false;
    }

    bool allParsed = true;

    // single read from socket may return more than one message
    for (struct nlmsghdr* nh = reinterpret_cast<struct nlmsghdr *>(m_eventsBuffer.data());
        NLMSG_OK(nh, static_cast<unsigned>(count)) && nh->nlmsg_type != NLMSG_DONE;
        nh = NLMSG_NEXT(nh, count))
    {
        // check message relevance
        const char* messageName = GetMessageNameFromRtmType(nh->nlmsg_type);
        if (!messageName)
        {
            LOG_VERBOSE << m_debugLogPrefix << "Unsupported message type " << nh->nlmsg_type << std::endl;
            continue;
        }

        // parse and handle single message
        // do not stop after parse, may have more relevant messages in the buffer (although it's not likely)
        allParsed = ParseAndHandleSingleMessage(nh, messageName) && allParsed;
    }

    return allParsed;
}

bool UnixNetworkInterfaceMonitor::ParseAndHandleSingleMessage(const struct nlmsghdr *nh, const char* messageName)
{
    const struct ifinfomsg* ifi = static_cast<struct ifinfomsg*>(NLMSG_DATA(nh));

    if (nh->nlmsg_len < NLMSG_LENGTH(sizeof(*ifi))) // shouldn't happen
    {
        LOG_ERROR << m_debugLogPrefix << "Got invalid " << messageName << ". Expected length " << NLMSG_LENGTH(sizeof(*ifi))
            << ", actual " << nh->nlmsg_len << std::endl;
        return false;
    }

    if ((ifi->ifi_flags & IFF_LOOPBACK) != 0)
    {
        // ignore loopback interface
        LOG_VERBOSE << m_debugLogPrefix << "Ignore " << messageName << " received for loopback interface" << std::endl;
        return false;
    }

    int len = IFLA_PAYLOAD(nh); // cannot be const, reassigned during iteration
    for (struct rtattr* rta = IFLA_RTA(ifi); RTA_OK(rta, len); rta = RTA_NEXT(rta, len))
    {
        if (rta->rta_type != IFLA_IFNAME)
        {
            // ignore irrelevant attributes
            continue;
        }

        // Notes:
        // 1. During RTM_DELLINK handling the network device can be already removed and so its
        //    interface name may be not available through the if_indextoname(ifi->ifi_index, m_ifname) call.
        //    Thus we always fetch the interface name directly from IFLA_IFNAME netlink message attribute.
        // 2. If needed, the interface state (up/down) can be tested by (ifi->ifi_flags & IFF_UP/IFF_LOWER_UP)
        // 3. The only supported message types are RTM_NEWLINK and RTM_DELLINK.

        const char* interfaceName = static_cast<const char*>(RTA_DATA(rta));
        const bool isNewLink = nh->nlmsg_type == RTM_NEWLINK ? true : false;

        LOG_VERBOSE << m_debugLogPrefix << "received " << messageName << " message for interface " << interfaceName
            << " (index " << ifi->ifi_index << "), ifi_flags=" << Hex<>(ifi->ifi_flags) << std::endl;

        HandleSingleMessage(interfaceName, ifi->ifi_index, isNewLink, ifi->ifi_flags, messageName);

        // other attributes are not required, may return
        return true;
    }

    LOG_ERROR << m_debugLogPrefix << "Failed to find IFLA_IFNAME attribute for message " << messageName << std::endl;
    return false;
}

void UnixNetworkInterfaceMonitor::HandleSingleMessage(const char* interfaceName, int interfaceIndex, bool isNewLink, uint32_t ifi_flags, const char* messageName)
{
    // cannot check interface relevance during rmmod for both RTM_NEWLINK and RTM_DELLINK events
    // therefore ignore RTM_NEWLINK in this case and mark interface as removed upon RTM_DELLINK without testing relevance
    // also ignore virtual interfaces during RTM_NEWLINK, no harm during RTM_NEWLINK since these are never actually added

    if (!isNewLink)
    {
        if ((ifi_flags & IFF_UP) != 0)
        {
            // ignore interface that is up, probably caused because the interface removed from the bridge (br-lan)
            LOG_VERBOSE << m_debugLogPrefix << "Ignore " << messageName << " received for interface that is up" << std::endl;
            return;
        }
        m_currentInterfaceNames.erase(interfaceName);
        RemoveEndPointIfOwner(interfaceName);
        return;
    }

    std::string endPointName;
    const auto eventAction = ClassifyNewLinkEvent(interfaceName, interfaceIndex, endPointName);
    switch (eventAction)
    {
    case EventAction::Ignore:
        LOG_VERBOSE << m_debugLogPrefix << "RTM_NEWLINK was ignored for for interface " << interfaceName << std::endl;
        break;
    case EventAction::Add:
        m_currentInterfaceNames.emplace(interfaceName);
        // updated endpoints list to be able to filter additional interfaces in the same batch
        m_currentEndPoints.emplace(std::move(endPointName), std::make_pair(std::string(interfaceName), interfaceIndex));
        break;
    case EventAction::Rename:
    {
        const auto endPointIter = m_currentEndPoints.find(endPointName);
        if (endPointIter == m_currentEndPoints.cend()) // shouldn't happen
        {
            LOG_ERROR << m_debugLogPrefix << "PCI End Points map is inconnsistent, failed to find " << endPointName << " after event classification" << std::endl;
            return;
        }
        m_currentInterfaceNames.erase(endPointIter->second.first);  // remove previous ifname
        m_currentInterfaceNames.emplace(interfaceName);             // add new ifname
        endPointIter->second.first = std::move(interfaceName);      // replace ifname for the found end point
    }
    break;
    default:
        LOG_ERROR << m_debugLogPrefix << "unknown event action " << static_cast<int>(eventAction)
            << " returned from RTM_NEWLINK classification" << std::endl;
    }
}

UnixNetworkInterfaceMonitor::EventAction
UnixNetworkInterfaceMonitor::ClassifyNewLinkEvent(const char* interfaceName, int interfaceIndex, std::string& endPointName) const
{
    /* RTM_NEWLINK handling policy:
    + ------------------------------+------------------+----------------------+
    | RTM_NEWLINK reason            | PCI End Point    | ifname   | ifindex   |
    + ------------------------------+------------------+----------------------+
    + ------------------------------+------------------+----------------------+
    | 1) Add new physical interface | new              | new      | new       | -> add
    + ------------------------------+------------------+----------------------+
    | 2) Interface Up (or periodic) | same             | same     | same      | -> ignore
    + ------------------------------+------------------+----------------------+
    | 3) Interface Down             | same             | same     | same      | -> ignore
    + ------------------------------+------------------+----------------------+
    | 4) Interface Down on rmmod    | NA, no driver is assosiated             | -> ignore, RTM_DELLINK will cause remove
    + ------------------------------+------------------+----------------------+
    | 5) Add virtual interface      | same as physical | new      | new       | -> ignore
    + ------------------------------+------------------+----------------------+
    | 6) Rename existing interface  | same as original | new      | same      | -> remove original, add new one
    + ------------------------------+------------------+----------------------+
    | 7) Rename virtual interface   | same as original | new      | different | -> ignore
    + ------------------------------+------------------+----------------------+
    | 8) Virtual interface Up/Down  | same as original | new      | different | -> ignore
    + ------------------------------+------------------+----------------------+
    | 9) Virtual Down on rmmond     | NA, no driver is assosiated             | -> ignore
    + ------------------------------+------------------+----------------------+
    | 10) Irrelevant interface      | NA, is assosiated with wrong driver     | -> ignore
    + ------------------------------+------------------+----------------------+

    (*) Queueing discipline name in IFLA_QDISC attribute is different for RTM_NEWLINK caused by Up and Add */

    static const std::string sc_driverName("wil6210");

    // Ensure this is a relevant interface by query for associated driver and rulling out virtual interfaces
    // Method:
    // - check file (symbolic link to a folder) exists: /sys/class/net/_ifname_/device/driver
    // - read link (will give something like ../../../../bus/pci/drivers/wil6210)
    // - take basename (e.g. wil6210)
    // (*) same as 'basename $(readlink /sys/class/net/_ifname_/device/driver)'
    std::ostringstream oss;
    oss << "/sys/class/net/" << interfaceName << "/device/driver";
    std::string driverContaingFolder(oss.str());

    if (!FileSystemOsAbstraction::DoesFileExist(driverContaingFolder))
    {
        // can happen upon rmmod when we get NEWLINK [cases 4 & 9]
        LOG_VERBOSE << m_debugLogPrefix << "Path '" << driverContaingFolder << "' does not exist (possible for RTM_NEWLINK during rmmod)" << std::endl;
        return EventAction::Ignore;
    }

    // compare basename of readlink with predefined driver name
    std::string driverPath;
    if (!FileSystemOsAbstraction::ReadLink(driverContaingFolder, driverPath))
    {
        // error reported
        return EventAction::Ignore;
    }

    const char* driverBaseName = basename(const_cast<char*>(driverPath.data())); // basename expects non-const and data() result is non-const since C++17
    if (sc_driverName.compare(driverBaseName) != 0)
    {
        // ignore irrelevant interface [case 10]
        LOG_VERBOSE << m_debugLogPrefix << "Ignore irrelevant interface " << interfaceName << " associated with driver " << driverBaseName << std::endl;
        return EventAction::Ignore;
    }

    // check it's a relevant event for a physical interface based on corresponding PCI End Point name, interface name and index
    endPointName = GetPciEndPointFromInterfaceName(interfaceName);
    if (endPointName.empty())
    {
        // reason reported [case 4, unless already caught by associated driver test]
        return EventAction::Ignore;
    }

    const auto endPointIter = m_currentEndPoints.find(endPointName);
    if (endPointIter == m_currentEndPoints.cend())
    {
        // new physical interface [case 1]
        LOG_VERBOSE << m_debugLogPrefix << "Detected new physical interface " << interfaceName << std::endl;
        return EventAction::Add;
    }
    // otherwise existing PCI End Point

    if (endPointIter->second.first == interfaceName)
    {
        // physical interface state change [cases 2 & 3]
        LOG_VERBOSE << m_debugLogPrefix << "Ignore state change for " << interfaceName << std::endl;
        return EventAction::Ignore;
    }

    if (endPointIter->second.second == interfaceIndex)
    {
        // rename of the physical iterface [case 6], display as info to mark the rename detection
        LOG_INFO << m_debugLogPrefix << "Detected rename for a physical interface from " << endPointIter->second.first << " to " << interfaceName << std::endl;
        return EventAction::Rename;
    }

    // otherwise it's same PCI End Point, different interface name and different interface index
    // [cases 5, 7 & 8]

    LOG_DEBUG << m_debugLogPrefix << "Ignored virtual interface " << interfaceName << std::endl;
    return EventAction::Ignore;
}

void UnixNetworkInterfaceMonitor::RemoveEndPointIfOwner(const char* interfaceName)
{
    // Note: it's not always possible to extract PCI End Point name during RTM_DELLINK (/sys/class/net/ifname already removed)
    // and threrefore we need to iterate over the map and delete entry related to the given interfacfe

    for (auto iter = m_currentEndPoints.begin(); iter != m_currentEndPoints.end(); ++iter)
    {
        if (iter->second.first == interfaceName)
        {
            LOG_VERBOSE << m_debugLogPrefix << "Removed endpoint record for interface " << interfaceName << std::endl;
            m_currentEndPoints.erase(iter); // leaving the loop, safe to erase in place
            return;
        }
    }

    // otherwise given interface is virtual/irrelevant
    LOG_VERBOSE << m_debugLogPrefix << "Ignored RTM_DELLINK for virtual/irrelevant interface " << interfaceName << std::endl;
}

// === Enumeration functionality === //
// Method:
// - check folder exists : /sys/module/wil6210/drivers/pci:wil6210/ (symbolic link to ../../../bus/pci/drivers/wil6210 folder)
// - ls and check for file having ":" (only PCI endpoint has ":" in its name, e.g. 0000:02:00.0)
//  - if not found, there is no wigig interface, return
// - get interface name from endpoint
//  - check folder exists : /sys/module/wil6210/drivers/pci:wil6210/_pciEndPoint_/net (e.g. /sys/module/wil6210/drivers/pci:wil6210/0000:02:00.0/net)
//    (this folder should contain '_iname_' folder for specific endpoint, e.g. /sys/module/wil6210/drivers/pci:wil6210/0000:02:00.0/net/wlan0)
//  - ls and find the first folder having NO "-" in the name, i.e.a physical interface => this is the iname
//  - if physical interface not found, have first one found(having "-" in the name) => this is the iname
void UnixNetworkInterfaceMonitor::InitializeCurrentInterfaceNames()
{
    m_currentInterfaceNames.clear();

    DIR* dp = opendir(PCI_END_POINT_FOLDER);
    if (!dp)
    {
        LOG_VERBOSE << m_debugLogPrefix << "Failed to open PCI End Point folder" << std::endl;
        return;
    }

    dirent* de = nullptr;
    while ((de = readdir(dp)) != nullptr) // iterate through directory content and search for End Point folders
    {
        const std::string potentialPciEndPoint(de->d_name);
        if (potentialPciEndPoint.find(":") != std::string::npos) // only PCI end point has ":" in its name
        {
            // Get interface name from End Point
            auto networkInterfaceInfo = GetInterfaceInfoFromPciEndPoint(potentialPciEndPoint);
            if (!networkInterfaceInfo.first.empty())
            {
                m_currentInterfaceNames.insert(networkInterfaceInfo.first);
                m_currentEndPoints.emplace(std::move(potentialPciEndPoint), std::move(networkInterfaceInfo));
            }
            else
            {
                LOG_DEBUG << m_debugLogPrefix << "Failed to find interface name for EP: " << potentialPciEndPoint << std::endl;
            }
        }
    }

    closedir(dp);

    LOG_DEBUG << m_debugLogPrefix << FormatInterfaceNames("Discovered WIGIG interface/s: ", m_currentInterfaceNames) << std::endl;
}
