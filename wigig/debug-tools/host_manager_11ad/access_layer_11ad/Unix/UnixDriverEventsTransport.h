/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "OperationStatus.h"
#include "Utils.h"
#include "WmiEventTransmissionContextContent.h"
#include "nl_services.h"

#include <array>
#include <vector>
#include <string>
#include <thread>
#include <bitset>
#include <atomic>
#include <unordered_set>

// forward declarations
struct driver_event_report;
class FwStateProvider;
enum class wil_fw_state;
enum class PmcDataType;

// Internal class for operations related to send/receive of Driver Events
class UnixDriverEventsTransport final
{
public:
    UnixDriverEventsTransport(const std::string& interfaceName, bool localSocket);

    // Initialize driver transport and the capability mask
    void InitializeTransports(std::bitset<32>& driverCapabilities);

    // Register for driver events
    void RegisterForDriverEvents(FwStateProvider* fwStateProvider);
    // cancel blocking polling for incoming events to allow graceful thread shutdown
    void StopDriverControlEventsBlocking();

    // Send event to the driver
    OperationStatus SendDriverCommand(uint32_t Id, const void *inBuf, uint32_t inBufSize, void *outBuf, uint32_t outBufSize);

    // Send WMI command to the driver and wait for reply with timeout
    OperationStatus SendWmiWithEvent(const void *inBuf, uint32_t inBufSize, uint32_t commandId, uint32_t moduleId, uint32_t subtypeId,
        uint32_t eventId, unsigned timeoutSec, std::vector<uint8_t>& eventBinaryData);


    // Reset FW through generic drive command
    // \remark May fail when:
    // - Working with Debug mailbox
    // - Driver in use does not support NL_60G_GEN_FW_RESET generic command
    // - Not WMI_ONLY FW in use
    OperationStatus TryFwReset();


    // read a uint32_t register from device memory
    // QCA_NL80211_VENDOR_SUBCMD_UNSPEC/NL_60G_CMD_MEMIO command
    OperationStatus MemRead(uint32_t address, uint32_t& value, bool& retriableError);
    OperationStatus MemReadBlock(uint32_t address, uint32_t blockSizeInBytes, char *arrBlock);
    OperationStatus MemWrite(uint32_t address, uint32_t& value);
    OperationStatus MemWriteBlock(uint32_t address, uint32_t blockSizeInBytes, const char * valuesToWrite);

    // Driver commands
    OperationStatus GetStaInfo(std::vector<struct nl_60g_sta_info_entry>& staInfo);
    OperationStatus GetFwState(wil_fw_state& fwState);
    OperationStatus GetFwCapa(std::vector<uint32_t>& fwCapa);

    // PMC commands
    OperationStatus PmcAlloc(uint32_t numDesc, uint32_t payloadLenBytes);
    OperationStatus PmcFree();
    OperationStatus PmcGetData(PmcDataType dataType, uint32_t& bufLenBytesInOut, uint8_t* pBuffer, bool& hasMoreData);

    // make it non-copyable
    UnixDriverEventsTransport(const UnixDriverEventsTransport& rhs) = delete;
    UnixDriverEventsTransport& operator=(const UnixDriverEventsTransport& rhs) = delete;

private:
    void DriverControlEventsThread(FwStateProvider* fwStateProvider); // main loop of polling for incoming driver events (blocking, no busy wait)
    bool HandleDriverEvent(FwStateProvider* fwStateProvider, const driver_event_report& driverEventReport, IfindexState ifiState);
    bool ShouldIgnoreFwEvent(const driver_event_report& driverEventReport);
    void CloseSocketPair();                 // release sockets from the sockets pair used to stop the driver events thread

    const std::string& m_interfaceName;     // alias to the interface name
    const bool m_isLocalSocket;
    std::unique_ptr<NlService> m_upNlService; // service that connect nl sockets
    std::array<int,2U> m_exitSockets;       // sockets pair to allow cancellation of blocking polling for incoming events
    std::atomic<bool> m_stopRunningDriverControlEventsThread;
    std::unordered_set<uint32_t> m_ignoredFwEvents; // holds all FW event IDs that shouldn't be published
    WmiEventTransmissionContextContent m_eventTransmissionContextContent; // responsible for wmi events management (from onTarget request)
    std::thread m_waitOnSignalThread;       // polling thread
};
