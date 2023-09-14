/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "DebugLogger.h"
#include "UnixDPDKDriver.h"
#include "DeviceContracts.h"
#include "DriverContracts.h"
#include "PmcCfg.h"
#include "PmcData.h"
#include "ShellCommandExecutor.h"
#include "OperationStatus.h"
#include "Utils.h"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <mutex>

#include <net/if.h> // for struct ifreq
#include <sys/socket.h>
#include <sys/ioctl.h> // for struct ifreq
#include <sys/types.h> // for opendir
#include <sys/un.h> // for socket address
#include <dirent.h> // for opendir
#include <string.h>
#include <unistd.h>
#include <errno.h>


#define EP_OPERATION_READ 0
#define EP_OPERATION_WRITE 1
#define WIL_IOCTL_MEMIO (SIOCDEVPRIVATE + 2)
#define WIL_IOCTL_MEMIO_BLOCK (SIOCDEVPRIVATE + 3)
#define WIL_IOCTL_MEMIO_RDR_GET_DATA (SIOCDEVPRIVATE + 4)

using namespace std;

UnixDPDKDriver::UnixDPDKDriver(const std::string& interfaceName)
    : DriverAPI(DeviceType::DPDK, interfaceName, false /*nonMonitored interface */)
    , m_fileDescriptor(-1)
    , m_lastPmcDscSize(0U)
    , m_lastPmcDescNum(0U)
    , m_driverEventsHandler(m_interfaceName, true)
{
    m_fileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_fileDescriptor < 0)
    {
        // do not report error here, legitimate scenario during enumeration
        return;
    }
}

UnixDPDKDriver::~UnixDPDKDriver()
{
    if (IsReady())
    {
        close(m_fileDescriptor);
        m_fileDescriptor = -1;
    }
}

bool UnixDPDKDriver::IsReady() const
{
    return m_fileDescriptor >= 0;
}


// Base access functions (to be implemented by specific device)
OperationStatus UnixDPDKDriver::Read(uint32_t address, uint32_t& value, bool& retriableError)
{
    if (!IsReady())
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready";
        return OperationStatus(false, oss.str());
    }

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_MEMIO))
    {
        return OperationStatus(false, "read access is not supported by the network driver");
    }

    return OperationStatus(m_driverEventsHandler.MemRead(address, value, retriableError));
}

OperationStatus UnixDPDKDriver::Read(uint32_t address, uint32_t& value)
{
    bool retriableError = false;

    return Read(address, value, retriableError);
}

OperationStatus UnixDPDKDriver::ReadBlock(uint32_t address, uint32_t blockSizeInDwords, vector<uint32_t>& values)
{
    values.clear();
    values.resize(blockSizeInDwords);

    char* buffer = reinterpret_cast<char*>(values.data());
    const uint32_t blockSizeInBytes = values.size() * sizeof(uint32_t);

    return ReadBlock(address, blockSizeInBytes, buffer);
}

OperationStatus UnixDPDKDriver::ReadBlock(uint32_t address, uint32_t blockSizeInBytes, char *arrBlock)
{
    if (!IsReady())
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready";
        return OperationStatus(false, oss.str());
    }

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_MEMIO))
    {
        return OperationStatus(false, "read access is not supported by the network driver");
    }

    return OperationStatus(m_driverEventsHandler.MemReadBlock(address, blockSizeInBytes, arrBlock));
}

OperationStatus UnixDPDKDriver::Write(uint32_t address, uint32_t value)
{
    if (!IsReady())
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready";
        return OperationStatus(false, oss.str());
    }

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_MEMIO_WRITE))
    {
        return OperationStatus(false, "write access is not supported by the network driver");
    }

    return OperationStatus(m_driverEventsHandler.MemWrite(address, value));
}

OperationStatus UnixDPDKDriver::WriteBlock(uint32_t address, uint32_t blockSizeInBytes, const char *valuesToWrite)
{
    if (!IsReady())
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready";
        return OperationStatus(false, oss.str());
    }

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_MEMIO_WRITE))
    {
        return OperationStatus(false, "write access is not supported by the network driver");
    }

    return OperationStatus(m_driverEventsHandler.MemWriteBlock(address, blockSizeInBytes, valuesToWrite));
}

OperationStatus UnixDPDKDriver::RetryWhenBusyRead(uint32_t address, uint32_t& value)
{
    static const unsigned maxNumRetries = 401U; // 40 intervals of 5 mses
    static const unsigned sleepIntervalMsec = 5U;
    bool retriableError = false;

    OperationStatus os = Read(address, value, retriableError);
    unsigned numRetries = 1U;

    LOG_VERBOSE << "RetryWhenBusyRead:" << " Iteration: " << numRetries << " retriableError:" << retriableError << endl;
    // in local socket mode BUSY will be nl error code
    // after driver restart we can get connection refused
    while (!os && retriableError && numRetries < maxNumRetries)
    {
        LOG_VERBOSE << "RetryWhenBusyRead (in retry loop): Iteration: " << numRetries << endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepIntervalMsec));
        os = Read(address, value, retriableError);
        ++numRetries;
    }

    if (!os)
    {
        ostringstream oss;
        oss << "[num iterations is " << numRetries << "]";
        os.AddPrefix(oss.str());
    }

    return os;
}

OperationStatus UnixDPDKDriver::WriteBlock(uint32_t addr, vector<uint32_t> values)
{
    const char* valuesToWrite = reinterpret_cast<const char*>(values.data());
    uint32_t sizeToWriteInBytes = values.size() * sizeof(uint32_t);

    return WriteBlock(addr, sizeToWriteInBytes, valuesToWrite);
}

OperationStatus UnixDPDKDriver::AllocPmc(unsigned descSize, unsigned descNum)
{
    LOG_DEBUG << "Request to allocate PMC ring with " << descNum << " descriptors, each " << descSize << " Bytes" << std::endl;

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_LEGACY_PMC_OVER_NL))
    {
        ostringstream oss;
        oss << "Legacy PMC commands are not supported on " << m_interfaceName;
        return OperationStatus(false, oss.str());
    }

    m_lastPmcDscSize = descSize;
    m_lastPmcDescNum = descNum;
    return m_driverEventsHandler.PmcAlloc(descNum, descSize);
}

OperationStatus UnixDPDKDriver::DeallocPmc(bool bSuppressError)
{
    LOG_DEBUG << "Request to release PMC ring " << std::endl;

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_LEGACY_PMC_OVER_NL))
    {
        ostringstream oss;
        oss << "Legacy PMC commands are not supported on " << m_interfaceName;
        return OperationStatus(false, oss.str());
    }

    OperationStatus os = m_driverEventsHandler.PmcFree();
    if (bSuppressError) // trinary operator will not compile
    {
        return OperationStatus(true);
    }
    return os;
}

OperationStatus UnixDPDKDriver::CreatePmcFiles(unsigned refNumber)
{
    LOG_DEBUG << "Creating PMC files #" << refNumber << std::endl;

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_LEGACY_PMC_OVER_NL))
    {
        ostringstream oss;
        oss << "Legacy PMC commands are not supported on " << m_interfaceName;
        return OperationStatus(false, oss.str());
    }

    using namespace std::placeholders;
    NetlinkPmcFileWriter pmcFileWriter(refNumber, std::bind(&UnixDPDKDriver::PmcGetDataInternal, this, _1, _2));

    OperationStatus st = pmcFileWriter.WriteFiles();
    if (!st.IsSuccess())
    {
        LOG_ERROR << "Error creating PMC files for #" << refNumber << std::endl;
        return st;
    }

    LOG_DEBUG << "PMC files created. Reported size: " << st.GetStatusMessage() << std::endl;
    return st;
}

OperationStatus UnixDPDKDriver::FindPmcDataFile(unsigned refNumber, std::string& foundPmcFilePath)
{
    LOG_DEBUG << "Looking for the PMC data file #" << refNumber << endl;

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_LEGACY_PMC_OVER_NL))
    {
        ostringstream oss;
        oss << "Legacy PMC commands are not supported on " << m_interfaceName;
        return OperationStatus(false, oss.str());
    }

    PmcFileLocator pmcFileLocator(refNumber);

    if (!pmcFileLocator.DataFileExists())
    {
        std::ostringstream errorMsgBuilder;
        errorMsgBuilder << "Cannot find PMC data file " << pmcFileLocator.GetDataFileName();
        return OperationStatus(false, errorMsgBuilder.str());
    }

    foundPmcFilePath = pmcFileLocator.GetDataFileName();
    return OperationStatus(true);
}

OperationStatus UnixDPDKDriver::FindPmcRingFile(unsigned refNumber, std::string& foundPmcFilePath)
{
    LOG_DEBUG << "Looking for the PMC ring file #" << refNumber << endl;

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_LEGACY_PMC_OVER_NL))
    {
        ostringstream oss;
        oss << "Legacy PMC commands are not supported on " << m_interfaceName;
        return OperationStatus(false, oss.str());
    }

    PmcFileLocator pmcFileLocator(refNumber);

    if (!pmcFileLocator.RingFileExists())
    {
        std::ostringstream errorMsgBuilder;
        errorMsgBuilder << "Cannot find PMC ring file " << pmcFileLocator.GetRingFileName();
        return OperationStatus(false, errorMsgBuilder.str());
    }

    foundPmcFilePath = pmcFileLocator.GetRingFileName();
    return OperationStatus(true);
}


OperationStatus UnixDPDKDriver::PmcGetDataInternal(PmcDataType dataType, std::vector<uint8_t>& pmcData)
{
    LOG_DEBUG << __FUNCTION__ << " called for " << dataType << endl;

    static const uint32_t MAX_CHUNK_LENGTH = 32768; // 32 KB limit
    static const uint32_t DESCRIPTOR_LENGTH = 32; // 8 DWs

    const uint32_t totalBufLenBytes = dataType == PmcDataType::PMC_RING_PAYLOAD
        ? m_lastPmcDescNum * m_lastPmcDscSize
        : m_lastPmcDescNum * DESCRIPTOR_LENGTH;
    pmcData.resize(totalBufLenBytes);

    uint32_t bytesRead = 0U;
    bool hasMoreData = false;

    do
    {
        uint32_t bufLenBytesInOut = std::min(totalBufLenBytes - bytesRead, MAX_CHUNK_LENGTH);

        auto os = m_driverEventsHandler.PmcGetData(dataType, bufLenBytesInOut, pmcData.data() + bytesRead, hasMoreData);
        if (!os)
        {
            return os;
        }

        if (bufLenBytesInOut == 0U)
        {
            // no data available
            ostringstream oss;
            oss << "on device " << m_interfaceName << ", cannot retrieve " << dataType
                << ", no data is available";
            return OperationStatus(false, oss.str());
        }

        bytesRead += bufLenBytesInOut;
    }
    while (hasMoreData && bytesRead < totalBufLenBytes);

    if (hasMoreData)
    {
        LOG_WARNING << "on device " << m_interfaceName
            << ", already retrieved " << bytesRead << " Bytes of " << dataType
            << " and driver reports that more data is available" << endl;
    }

    if (bytesRead < pmcData.size())
    {
        LOG_WARNING << "on device " << m_interfaceName
            << ", expected to get " << pmcData.size() << " Bytes of " << dataType
            << ", but retrieved only " << bytesRead << endl;
        pmcData.resize(bytesRead);
    }

    LOG_DEBUG << "on device " << m_interfaceName << ", retrieved " << bytesRead << " Bytes of " << dataType << endl;

    return OperationStatus();
}

OperationStatus UnixDPDKDriver::InterfaceReset()
{
    if (!IsReady())
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready";
        return OperationStatus(false, oss.str());
    }

    // For doing an interface reset, ifconfig down and up is done
    // When this method does not work, try reseting FW through a Driver command

    ostringstream ss;

    // Interface down:
    ss << "ifconfig " << m_interfaceName << " down";
    ShellCommandExecutor sce(ss.str().c_str());
    if (sce.ExitStatus() != 0)
    {
        ostringstream errStr;
        errStr << "Couldn't make '" << ss.str() << "': " << (sce.Output().size() > 0 ? sce.Output().front() : "");
        return OperationStatus(false, errStr.str());
    }

    ss.str(string());

    // Interface up:
    ss << "ifconfig " << m_interfaceName << " up";
    ShellCommandExecutor sce1(ss.str().c_str());
    if (sce1.ExitStatus() == 0)
    {
        LOG_INFO << "Interface reset for " << m_interfaceName << " completed through ifconfig" << endl;
        return OperationStatus(true);
    }
    else
    {
        // print this message only for debug
        LOG_DEBUG << "Couldn't make '" << ss.str() << "': " << (sce1.Output().size() > 0 ? sce1.Output().front() : "") << endl;
    }

    // If we got here, it may be a case of WMI_ONLY FW. Try to reset FW through a Driver command.
    // Note: This will not work when using Debug mailbox or using older driver before adding support for this command.
    return m_driverEventsHandler.TryFwReset();
}

bool UnixDPDKDriver::InitializeTransports()
{
    if (!IsReady())
    {   // shouldn't happen
        LOG_ERROR << "Failed to initialize driver transports for " << m_interfaceName << ", driver is not ready\n";
        return false;
    }

    m_driverEventsHandler.InitializeTransports(m_capabilityMask);
    LOG_INFO << "wil6210 driver capabilities are 32b'" << m_capabilityMask << endl;
    return true;
}

void UnixDPDKDriver::RegisterDriverControlEvents(FwStateProvider* fwStateProvider)
{
    if (!IsReady())
    {
        return;
    }

    if (IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_WMI))
    {
        m_driverEventsHandler.RegisterForDriverEvents(fwStateProvider);
    }
}

OperationStatus UnixDPDKDriver::DriverControl(uint32_t id,
                                             const void *inBuf, uint32_t inBufSize,
                                             void *outBuf, uint32_t outBufSize)
{
    if (!IsReady()) // shouldn't happen
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready (driver command with id " << Hex<>(id) << ")";
        return OperationStatus(false, oss.str());
    }

    return m_driverEventsHandler.SendDriverCommand(id, inBuf, inBufSize, outBuf, outBufSize);
}

OperationStatus UnixDPDKDriver::SendWmiWithEvent(const void *inBuf, uint32_t inBufSize, uint32_t commandId, uint32_t moduleId, uint32_t subtypeId,
    uint32_t eventId, unsigned timeoutSec, std::vector<uint8_t>& eventBinaryData)
{
    if (!IsReady()) // shouldn't happen
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready (WMI command)" << ")";
        return OperationStatus(false, oss.str());
    }

    return m_driverEventsHandler.SendWmiWithEvent(inBuf, inBufSize, commandId, moduleId, subtypeId, eventId, timeoutSec, eventBinaryData);
}

void UnixDPDKDriver::StopDriverControlEvents()
{
    m_driverEventsHandler.StopDriverControlEventsBlocking();
}

OperationStatus UnixDPDKDriver::GetStaInfo(vector<StaInfo>& staInfo)
{
    if (!IsReady()) // shouldn't happen
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready";
        return OperationStatus(false, oss.str());
    }

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_GET_STA_INFO))
    {
        ostringstream oss;
        oss << "Unsupported driver command (NL_60G_GEN_GET_STA_INFO) for device " << m_interfaceName;
        return OperationStatus(false, oss.str());
    }

    vector<struct nl_60g_sta_info_entry> staInfoBuf;

    OperationStatus os = m_driverEventsHandler.GetStaInfo(staInfoBuf);
    if (!os)
    {
        return os;
    }

    uint16_t i = 0;
    for (auto &staInfoEntry : staInfoBuf) {
        uint64_t mac = 0;
        memcpy(&mac, staInfoEntry.mac_addr, 6);
        staInfo.emplace_back(StaInfo(i, mac, static_cast<StaConnectionStatus>(staInfoEntry.status), staInfoEntry.mid, staInfoEntry.aid));
        i++;
    }

    return OperationStatus();
}

OperationStatus UnixDPDKDriver::GetFwState(wil_fw_state& fwState)
{
    if (!IsReady()) // shouldn't happen
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready";
        return OperationStatus(false, oss.str());
    }

    if (!IsCapabilitySet(wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_FW_STATE))
    {
        ostringstream oss;
        oss << "Unsupported driver command (NL_60G_GEN_GET_FW_STATE) for device " << m_interfaceName;
        return OperationStatus(false, oss.str());
    }

    return m_driverEventsHandler.GetFwState(fwState);
}

OperationStatus UnixDPDKDriver::GetFwCapa(std::vector<uint32_t>& fwCapa)
{
    if (!IsReady()) // shouldn't happen
    {
        ostringstream oss;
        oss << "driver for device " << m_interfaceName << " is not ready";
        return OperationStatus(false, oss.str());
    }

    return m_driverEventsHandler.GetFwCapa(fwCapa);
}