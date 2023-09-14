/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sstream>

#include "UnixDriverEventsTransport.h"
#include "DriverAPI.h"
#include "DriverContracts.h"
#include "EventsDefinitions.h"
#include "FwStateProvider.h"
#include "DebugLogger.h"
#include "Host.h"
#include "HostInfo.h"
#include "nl_contracts.h"
#include "nl_services.h"

using namespace std;

UnixDriverEventsTransport::UnixDriverEventsTransport(const std::string& interfaceName, bool localSocket) :
    m_interfaceName(interfaceName),
    m_isLocalSocket(localSocket),
    m_exitSockets{ { -1, -1 } }, // aggregate initialization
    m_stopRunningDriverControlEventsThread(false)
{
    m_ignoredFwEvents = Host::GetHost().GetHostInfo().Get3ppIgnoreEvents();
}

// Initialize driver transport and update the capability mask
void UnixDriverEventsTransport::InitializeTransports(bitset<32>& driverCapabilities)
{
    driverCapabilities.reset(); // reset-all-bits initialization for case we fail to get it from the driver
    if (!m_isLocalSocket)
    {
        driverCapabilities.set(static_cast<size_t>(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_IOCTL_WRITE)); // no actual knowledge
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, m_exitSockets.data()) == -1) // shouldn't happen
    {
        LOG_ERROR << "On Device " << m_interfaceName << ", failed to create cancellation socket pair, abort netlink interface initialization" << endl;
        return;
    }

    m_upNlService.reset(new NlService(m_interfaceName.c_str(), m_isLocalSocket));
    OperationStatus os = m_upNlService->GetLastStatus();
    if (!os) // can happen for old driver that doesn't support driver events
    {
        CloseSocketPair(); // release the exit sockets
                           // display warning instead of error to prevent SI test failure
        LOG_WARNING << "On Device " << m_interfaceName << ", failed to initialize netlink interface: " << os.GetStatusMessage() << endl;
        return;
    }

    // get capabilities from the driver
    uint32_t capabilityMask = 0U;
    os = m_upNlService->NlGetCapabilities(capabilityMask);
    if (os)
    {
        driverCapabilities = bitset<32>(capabilityMask);
        return;
    }

    // otherwise, GET_DRIVER_CAPA command is not supported on the driver side or it's a transport error
    // netlink is initialized, set WMI transport & IOCTL_WRITE bits anyway (backward compatibility with driver before NL_60G_GEN_GET_DRIVER_CAPA)
    // IOCTL_WRITE was set during init
    driverCapabilities.set(static_cast<size_t>(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_WMI));

    LOG_WARNING << "On Device " << m_interfaceName << ", failed to get driver capabilities (driver upgrade may be required): " << os.GetStatusMessage() << endl;
}

// Register for driver events
void UnixDriverEventsTransport::RegisterForDriverEvents(FwStateProvider* fwStateProvider)
{
    if (!m_upNlService)
    {
        // working with Debug mailbox, do nothing
        LOG_DEBUG << m_interfaceName << " is working with Debug mailbox, registration for driver events is not required" << endl;
        return;
    }

    // Creating a thread to wait on the netlink blocking poll, pass the interface name
    // To prevent miss of published events the thread is created before asking to send events to the user space
    m_waitOnSignalThread = thread(&UnixDriverEventsTransport::DriverControlEventsThread, this, fwStateProvider);

    if (!m_isLocalSocket)
    {
        const OperationStatus os = m_upNlService->NlEnableDriverEventsTransport(true /*enable*/);
        if (!os)
        {
            LOG_ERROR << "On Device " << m_interfaceName
                << ", cannot enable events transport (driver upgrade may be required): "
                << os.GetStatusMessage() << endl;
            return;
        }
    }
}

// Send event to the driver
OperationStatus UnixDriverEventsTransport::SendDriverCommand(uint32_t id, const void *pInBuf, uint32_t inBufSize, void *pOutBuf, uint32_t outBufSize)
{
    LOG_ASSERT(pInBuf); // just in case, pInBuf validity should be tested by the caller

    if (pOutBuf && outBufSize != sizeof(uint32_t))
    {   // when response required, the only supported response size is 32 bit
        ostringstream oss;
        oss << "cannot send driver command for device " << m_interfaceName
            << ", given output buffer size " << outBufSize << " (Bytes) does not match expected " << sizeof(uint32_t);
        return OperationStatus(false, oss.str());
    }

    OperationStatus os = m_upNlService->NlSendDriverCommand(id, inBufSize, pInBuf, reinterpret_cast<uint32_t*>(pOutBuf));
    if (!os)
    {
        ostringstream oss;
        oss << "failed to send driver command " << Hex<>(id) << " for device " << m_interfaceName;
        os.AddPrefix(oss.str());
    }

    return os;
}

// Send WMI command and wait for event with timeout
OperationStatus UnixDriverEventsTransport::SendWmiWithEvent(const void *inBuf, uint32_t inBufSize, uint32_t commandId,  uint32_t moduleId, uint32_t subtypeId,
        uint32_t eventId, unsigned timeoutSec, vector<uint8_t>& eventBinaryData)
{
    std::shared_ptr<WmiEventTransmissionContext> eventContext;
    OperationStatus os = m_eventTransmissionContextContent.SubscribeToWmiEvent(eventId, eventContext, commandId, moduleId, subtypeId);
    if (!os)
    {
        os.AddPrefix("Cannot send WMI command");
        return os;
    }
    LOG_VERBOSE << "Subscribed to WMI event id=" << Hex<>(eventId) << "; timeout: " << timeoutSec << " sec" << endl;

    os = SendDriverCommand(static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_FW_WMI), inBuf, inBufSize, nullptr, 0);
    if (!os)
    {
        m_eventTransmissionContextContent.UnsubscribeFromWmiEvent(eventId);
        return os;
    }

    // Start timer and wait for response
    eventContext->WaitForReply(timeoutSec); // blocking

    m_eventTransmissionContextContent.UnsubscribeFromWmiEvent(eventId);
    LOG_VERBOSE << "Unsubscribed from WMI event id=" << Hex<>(eventId) << endl;

    if (!eventContext->IsEventArrived()) // error occured
    {
        return OperationStatus(false, eventContext->GetErrorMsg());
    }

    eventBinaryData = eventContext->GetBinaryData();
    return OperationStatus();
}

bool UnixDriverEventsTransport::ShouldIgnoreFwEvent(const driver_event_report& driverEventReport)
{
    if (driverEventReport.evt_type != wil_nl_60g_evt_type::DRIVER_EVENT_FW_WMI_EVENT)
    {
        return false;
    }

    const auto payload = reinterpret_cast<const wil_nl_60g_send_receive_wmi*>(driverEventReport.buf);
    auto it = m_ignoredFwEvents.find(payload->cmd_id);
    return it != m_ignoredFwEvents.end();
}

// Main loop of polling for incoming driver events
void UnixDriverEventsTransport::DriverControlEventsThread(FwStateProvider* fwStateProvider)
{
    if (m_isLocalSocket)
    {
        const OperationStatus os = m_upNlService->NlEnableDriverEventsTransport(true /*enable*/);
        if (!os)
        {
            LOG_ERROR << "On Device " << m_interfaceName
                << ", cannot enable events transport (driver upgrade may be required): "
                << os.GetStatusMessage() << endl;
            return;
        }
    }

    struct driver_event_report driverEvent {};
    IfindexState ifiState = IfindexState::IFINDEX_STATE_MISSING;
    while (!m_stopRunningDriverControlEventsThread.load())
    {
        memset(&driverEvent, 0, sizeof(driverEvent));
        ifiState = IfindexState::IFINDEX_STATE_MISSING;

        // blocking call to fetch the next event
        const OperationStatus os = m_upNlService->NlGetDriverEvent(m_exitSockets[1] /* cancellation socket file descriptor */, &driverEvent, ifiState);

        if (m_stopRunningDriverControlEventsThread.load()) // asked to quit
        {
            // Do not disable events transport on the Driver side. It may cause a disruption for other devices.
            // Previous code for reference:
            // Ignore the possible error because of backward compatibility with previous Driver version before this feature
            // nl_enable_driver_events_transport(m_nlState, false /*disable*/);
            break;
        }

        if (!os)
        {
            LOG_ERROR << "On Device " << m_interfaceName << ", failed to get driver event: " << os.GetStatusMessage() << std::endl;
            continue;
        }

        if (driverEvent.buf_len > DRIVER_MSG_MAX_LEN)
        {
            // shouldn't happen
            LOG_ERROR << "On Device " << m_interfaceName << ", got invalid driver event, buffer length is " << driverEvent.buf_len << std::endl;
            continue;
        }

        // handle driver event or push it to connected clients
        if (!HandleDriverEvent(fwStateProvider, driverEvent, ifiState))
        {
            if (ShouldIgnoreFwEvent(driverEvent))
            {
                continue;
            }

            if (driverEvent.evt_type == wil_nl_60g_evt_type::DRIVER_EVENT_FW_WMI_EVENT)
            {
                // if eventID is in m_eventTransmissionContextContent's map, handle it as ontarget callback and push to DmTools
                auto eventStruct = reinterpret_cast<const wil_nl_60g_send_receive_wmi*>(driverEvent.buf);
                m_eventTransmissionContextContent.OnEventArrived(eventStruct->cmd_id, eventStruct->buf_len, eventStruct->buf);
            }

            Host::GetHost().PushEvent(DriverEvent(m_interfaceName, static_cast<int>(driverEvent.evt_type),
                driverEvent.evt_id, driverEvent.listener_id, driverEvent.buf_len, driverEvent.buf));
        }

        // print received event ids only after ignoring irrelevant events
        if (ShouldPrint(LOG_SEV_DEBUG) && ifiState != IfindexState::IFINDEX_STATE_MISMATCH)
        {
            if (driverEvent.evt_type == wil_nl_60g_evt_type::DRIVER_EVENT_FW_WMI_EVENT)
            {
                uint32_t eventId = (reinterpret_cast<const wil_nl_60g_send_receive_wmi*>(driverEvent.buf))->cmd_id;
                LOG_DEBUG << "On Device " << m_interfaceName << ", received WMI event, id=" << Hex<>(eventId) << std::endl;
            }
            else if (driverEvent.evt_type == wil_nl_60g_evt_type::DRIVER_EVENT_FW_RMI_EVENT)
            {
                uint16_t eventId = (reinterpret_cast<const wil_nl_60g_send_receive_rmi*>(driverEvent.buf))->cmd_id;
                LOG_DEBUG << "On Device " << m_interfaceName << ", received RMI event, id=" << Hex<>(eventId) << std::endl;
            }
        }
    }

    LOG_DEBUG << "DriverControlEventsThread for device " << m_interfaceName << " was asked to quit." << std::endl;
}

// \returns True if event was handled
bool UnixDriverEventsTransport::HandleDriverEvent(FwStateProvider* fwStateProvider, const driver_event_report& driverEventReport, IfindexState ifiState)
{
    if (driverEventReport.evt_type != wil_nl_60g_evt_type::DRIVER_EVENT_DRIVER_GENERIC_EVENT)
    {
        if (ifiState == IfindexState::IFINDEX_STATE_MISMATCH)
        {
            return true;
        }
        return false;
    }

    const auto payload = reinterpret_cast<const wil_nl_60g_fw_state_event*>(driverEventReport.buf);

    if (payload->evt_id == wil_nl_60g_generic_evt::NL_60G_GEN_EVT_FW_STATE)
    {
        if (driverEventReport.buf_len != sizeof(wil_nl_60g_fw_state_event))
        {
            // shouldn't happen
            LOG_ERROR << m_interfaceName << " got invalid FW_STS_EVENT, expected " << sizeof(wil_nl_60g_fw_state_event)
                << "Bytes of payload, got " << driverEventReport.buf_len << endl;
            return true;
        }

        fwStateProvider->OnFwHealthStateChanged(payload->fw_sts);
        return true;
    }
    else if (payload->evt_id == wil_nl_60g_generic_evt::NL_60G_GEN_EVT_AUTO_RADAR_DATA_READY)
    {
        // ignore, mark handled
        return true;
    }
    // handle other generic driver event...

    // Not an error, may happen if working with a newer driver version
    LOG_WARNING << "On Device " << m_interfaceName << ", received unsupported driver generic event (type=" << static_cast<uint32_t>(payload->evt_id)
        << "), consider upgrading host_manager_11ad" << endl;
    return false; // was not handled...
}

// Cancel blocking polling for incoming events to allow graceful thread shutdown
void UnixDriverEventsTransport::StopDriverControlEventsBlocking()
{
    // Stop driver control event thread:
    m_stopRunningDriverControlEventsThread.store(true);

    // unblocks the monitor receive socket for termination
    // sockets pair serves as a pipe - a value written to one of its sockets, is also written to the second one
    // actual value written has no importance
    if (m_exitSockets[0] >= 0)
    {
        write(m_exitSockets[0], "T", 1);
    }

    if (m_waitOnSignalThread.joinable())
    {
        m_waitOnSignalThread.join();
    }

    // release sockets from the sockets pair used to stop the driver events thread
    CloseSocketPair();
}

// Release sockets from the sockets pair used to stop the driver events thread
void UnixDriverEventsTransport::CloseSocketPair()
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

// Reset FW through generic drive command
// \remark May fail when:
// - Working with Debug mailbox
// - Driver in use does not support NL_60G_GEN_FW_RESET generic command
// - Not WMI_ONLY FW in use
OperationStatus UnixDriverEventsTransport::TryFwReset()
{
    LOG_DEBUG << __FUNCTION__ << endl;

    if (!m_upNlService)
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", FW reset is not supported for Debug mailbox";
        return OperationStatus(false, oss.str());
    }

    OperationStatus os = m_upNlService->NlFwReset();
    if (!os)
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", ";
        os.AddPrefix(oss.str(), false /*no separator*/);
    }

    return os;
}

// Driver commands
OperationStatus UnixDriverEventsTransport::MemRead(uint32_t address, uint32_t& value, bool& retriableError)
{
    OperationStatus os = m_upNlService->NlMemRead(address, value, retriableError);
    if (!os)
    {
        ostringstream oss;
        oss << os.GetStatusMessage() << ", address:" << Hex<8>(address);
        return OperationStatus(false, oss.str());
    }

    return OperationStatus();
}

// read block in chunks of 32k (libnl limited to 64k)
OperationStatus UnixDriverEventsTransport::MemReadBlock(uint32_t address, uint32_t blockSizeInBytes, char *arrBlock)
{
    uint32_t remainingBytes = blockSizeInBytes;
    uint32_t arrBlockOffset = 0;
    static const uint32_t MAX_CHUNK_SIZE = 32 * 1024;

    while(remainingBytes > 0)
    {
        uint32_t chunkSize = std::min(remainingBytes, MAX_CHUNK_SIZE);
        Timer rbTimer;
        OperationStatus os = m_upNlService->NlMemReadBlock(address + arrBlockOffset, chunkSize, arrBlock + arrBlockOffset);
        rbTimer.Stop();
        if (!os)
        {
            ostringstream oss;
            oss << os.GetStatusMessage() << ", address:" << Address(address) << " offset:" << arrBlockOffset;
            return OperationStatus(false, oss.str());
        }

        arrBlockOffset += chunkSize;
        remainingBytes -= chunkSize;

        LOG_VRB(RDWR) << "DPDK block read."
            << " Address: " << Address(address)
            << " Size: " << blockSizeInBytes << " (B)"
            << " Netto Time: " << rbTimer
            << endl;
    }

    return OperationStatus();
}

OperationStatus UnixDriverEventsTransport::MemWrite(uint32_t address, uint32_t& value)
{
    OperationStatus os = m_upNlService->NlMemWrite(address, value);
    if (!os)
    {
        ostringstream oss;
        oss << os.GetStatusMessage() << ", address:" << Hex<8>(address);
        return OperationStatus(false, oss.str());
    }

    return OperationStatus();
}

OperationStatus UnixDriverEventsTransport::MemWriteBlock(uint32_t address, uint32_t blockSizeInBytes,const char * valuesToWrite)
{
    OperationStatus os = m_upNlService->NlMemWriteBlock(address, blockSizeInBytes, valuesToWrite);
    if (!os)
    {
        ostringstream oss;
        oss << os.GetStatusMessage() << ", address:" << Hex<8>(address);
        return OperationStatus(false, oss.str());
    }

    return OperationStatus();
}

// Send GetStaInfo command to the driver
OperationStatus UnixDriverEventsTransport::GetStaInfo(vector<struct nl_60g_sta_info_entry>&staInfo)
{
    if (!m_upNlService) // shouldn't happen
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", netlink transport is not initialized";
        return OperationStatus(false, oss.str());
    }

    OperationStatus os = m_upNlService->NlGetStaInfo(staInfo);
    if (!os)
    {
        ostringstream oss;
        oss << "failed to send driver command NL_60G_GEN_GET_STA_INFO for device " << m_interfaceName;
        os.AddPrefix(oss.str());
    }

    return os;
}

OperationStatus UnixDriverEventsTransport::GetFwState(wil_fw_state& fwState)
{
    LOG_DEBUG << __FUNCTION__ << endl;

    if (!m_upNlService) // shouldn't happen
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", netlink transport is not initialized";
        return OperationStatus(false, oss.str());
    }

    OperationStatus os = m_upNlService->NlGetFwState(fwState);
    if (!os)
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", ";
        os.AddPrefix(oss.str(), false /*no separator*/);
    }

    return os;
}

OperationStatus UnixDriverEventsTransport::GetFwCapa(std::vector<uint32_t>& fwCapa)
{
    if (!m_upNlService) // shouldn't happen
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", netlink transport is not initialized";
        return OperationStatus(false, oss.str());
    }

    OperationStatus os = m_upNlService->NlGetFwCapa(fwCapa);
    if (!os)
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", ";
        os.AddPrefix(oss.str(), false /*no separator*/);
    }

    return os;
}

// New PMC APIs
OperationStatus UnixDriverEventsTransport::PmcAlloc(uint32_t numDesc, uint32_t payloadLenBytes)
{
    if (!m_upNlService) // shouldn't happen
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", netlink transport is not initialized";
        return OperationStatus(false, oss.str());
    }

    OperationStatus os = m_upNlService->NlPmcAlloc(numDesc, payloadLenBytes);
    if (!os)
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", ";
        os.AddPrefix(oss.str(), false /*no separator*/);
    }

    return os;
}

OperationStatus UnixDriverEventsTransport::PmcFree()
{
    if (!m_upNlService) // shouldn't happen
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", netlink transport is not initialized";
        return OperationStatus(false, oss.str());
    }

    OperationStatus os = m_upNlService->NlPmcFree();
    if (!os)
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", ";
        os.AddPrefix(oss.str(), false /*no separator*/);
    }

    return os;
}

OperationStatus UnixDriverEventsTransport::PmcGetData(PmcDataType dataType,
    uint32_t& bufLenBytesInOut, uint8_t* pBuffer, bool& hasMoreData)
{
    if (!m_upNlService) // shouldn't happen
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", netlink transport is not initialized";
        return OperationStatus(false, oss.str());
    }

    const uint32_t bufLenBytesIn = bufLenBytesInOut; // store the 'in' value

    uint32_t minDataLengthBytes = 0U;
    OperationStatus os;
    if (dataType == PmcDataType::PMC_RING_PAYLOAD)
    {
        os = m_upNlService->NlPmcGetData(bufLenBytesInOut, pBuffer, minDataLengthBytes, hasMoreData);
    }
    else
    {
        os = m_upNlService->NlPmcGetDescData(bufLenBytesInOut, pBuffer, minDataLengthBytes, hasMoreData);
    }
    if (!os)
    {
        ostringstream oss;
        oss << "on device " << m_interfaceName << ", failed to retrieve " << dataType << ": ";
        os.AddPrefix(oss.str(), false /*no separator*/);
        return os;
    }

    if (minDataLengthBytes > 0)
    {
        std::ostringstream oss;
        oss << "on device " << m_interfaceName
            << ", failed to retrieve " << dataType
            << ": buffer of " << bufLenBytesIn
            << " Bytes is too small, minimum required is " << minDataLengthBytes;
        return OperationStatus(false, oss.str());
    }

    if (bufLenBytesInOut > bufLenBytesIn) // shouldn't happen
    {
        std::ostringstream oss;
        oss << "on device " << m_interfaceName << ", failed to retrieve " << dataType << ": allocated " << bufLenBytesIn;
        oss << " Bytes, but driver response contains " << bufLenBytesInOut;
        return OperationStatus(false, oss.str());
    }

    return os;
}
