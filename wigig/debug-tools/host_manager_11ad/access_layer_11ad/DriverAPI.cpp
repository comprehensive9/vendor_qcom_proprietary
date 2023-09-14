/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "DriverAPI.h"
#include "DriverContracts.h"


DriverAPI::DriverAPI(DeviceType deviceType, std::string interfaceName, bool isMonitored /*= false*/) :
    m_deviceType(deviceType),
    m_interfaceName(std::move(interfaceName)),
    m_capabilityMask(4U), // b'100, IOCTL WRITE capability is set by default, may be overriden when reported otherwise
    m_isMonitored(isMonitored)
{}

OperationStatus DriverAPI::Read(uint32_t /*address*/, uint32_t& /*value*/)
{
    return OperationStatus(false, "Not implemented");
}

OperationStatus DriverAPI::ReadBlock(uint32_t /*addr*/, uint32_t /*blockSizeInDwords*/, std::vector<uint32_t>& /*values*/)
{
    return OperationStatus(false, "Not implemented");
}

OperationStatus DriverAPI::ReadBlock(uint32_t /*addr*/, uint32_t /*blockSizeInBytes*/, char * /*arrBlock*/)
{
    return OperationStatus(false, "Not implemented");
}

OperationStatus DriverAPI::ReadRadarData(uint32_t /*maxBlockSizeInDwords*/, std::vector<uint32_t>& /*values*/)
{
    return OperationStatus(false, "Not implemented");
}

OperationStatus DriverAPI::Write(uint32_t /*address*/, uint32_t /*value*/)
{
    return OperationStatus(false, "Not implemented");
}

OperationStatus DriverAPI::WriteBlock(uint32_t /*addr*/, std::vector<uint32_t> /*values*/)
{
    return OperationStatus(false, "Not implemented");
}

OperationStatus DriverAPI::WriteBlock(uint32_t /*address*/, uint32_t /*blockSizeInBytes*/, const char* /*valuesToWrite*/)
{
    return OperationStatus(false, "Not implemented");
}

OperationStatus DriverAPI::RetryWhenBusyRead(uint32_t address, uint32_t& value)
{
    return Read(address, value); // no retries by default
}

OperationStatus DriverAPI::AllocPmc(unsigned /*descSize*/, unsigned /*descNum*/)
{
    return OperationStatus(false, "PMC is not supported on the current platform");
}

OperationStatus DriverAPI::DeallocPmc(bool /*bSuppressError*/)
{
    return OperationStatus(false, "PMC is not supported on the current platform");
}

OperationStatus DriverAPI::CreatePmcFiles(unsigned /*refNumber*/)
{
    return OperationStatus(false, "PMC is not supported on the current platform");
}

OperationStatus DriverAPI::FindPmcDataFile(unsigned /*refNumber*/, std::string& /*foundPmcFilePath*/)
{
    return OperationStatus(false, "PMC is not supported on the current platform");
}

OperationStatus DriverAPI::FindPmcRingFile(unsigned /*refNumber*/, std::string& /*foundPmcFilePath*/)
{
    return OperationStatus(false, "PMC is not supported on the current platform");
}

OperationStatus DriverAPI::PmcGetData(uint32_t& /*bufLenBytesInOut*/, uint8_t* /*pBuffer*/, bool& /*hasMoreData*/)
{
    return OperationStatus(false, "New PMC APIs are not supported on the current platform");
}

OperationStatus DriverAPI::DriverControl(
    uint32_t /*id*/, const void* /*inBuf*/, uint32_t /*inBufSize*/,
    void* /*outBuf*/, uint32_t /*outBufSize*/)
{
    return OperationStatus(false, "Driver Control not supported on the current platform");
}

OperationStatus DriverAPI::SendWmiWithEvent(const void* /*inBuf*/, uint32_t /*inBufSize*/, uint32_t /*commandId*/, uint32_t /*moduleId*/, uint32_t /*subtypeId*/,
                                        uint32_t /*eventId*/, unsigned /*timeout*/, std::vector<uint8_t>& /*eventBinaryData*/)
{
    return OperationStatus(false, "Sending WMI commands with reply is not supported on the current platform");
}

OperationStatus DriverAPI::GetDriverMode(int& /*currentState*/)
{
    return OperationStatus(false, "Not implemented");
}

OperationStatus DriverAPI::SetDriverMode(int /*newState*/, int& /*oldState*/)
{
    return OperationStatus(true); // by default does nothing and returns success
}

OperationStatus DriverAPI::InterfaceReset()
{
    return OperationStatus(false, "Interface Reset is not supported on the current platform");
}

OperationStatus DriverAPI::GetStaInfo(std::vector<StaInfo>& /*staInfo*/)
{
    return OperationStatus(false, "Get STA info not supported on the current platform");
}

OperationStatus DriverAPI::GetFwState(wil_fw_state& /*fwState*/ )
{
    return OperationStatus(false, "Get FW state is not supported on the current platform");
}

OperationStatus DriverAPI::GetFwCapa(std::vector<std::uint32_t>& /*fwCapa*/)
{
    return OperationStatus(false, "Get FW capabilities is not supported on the current platform");
}

uint32_t DriverAPI::GetCapabilityMask() const
{
    return static_cast<uint32_t>(m_capabilityMask.to_ulong());
}

bool DriverAPI::IsCapabilitySet(wil_nl_60g_driver_capa capability) const
{
    return m_capabilityMask.test(static_cast<size_t>(capability));
}

std::ostream& operator<<(std::ostream& os, const PmcDataType& dataType)
{
    switch (dataType)
    {
    case PmcDataType::PMC_RING_PAYLOAD:      return os << "PMC ring payload";
    case PmcDataType::PMC_RING_DESCRIPTORS:  return os << "PMC ring descriptors";
    default:                                 return os << "unknown PMC data type";
    }
}

// Returns FW capability name corresponding to given FW capability bit or the bit number when capability is not defined
const char* DriverCapaToString(int capability)
{
    const auto capa = static_cast<wil_nl_60g_driver_capa>(capability);
    switch (capa)
    {
    case wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_WMI:                return "WMI";
    case wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_FW_STATE:           return "FW_STATE";
    case wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_IOCTL_WRITE:        return "IOCTL_WRITE";
    case wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_MEMIO:              return "MEMIO";
    case wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_MEMIO_WRITE:        return "MEMIO_WRITE";
    case wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_GET_STA_INFO:       return "STA_INFO";
    case wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_LEGACY_PMC_OVER_NL: return "LEGACY_PMC_OVER_NL";
    case wil_nl_60g_driver_capa::NL_60G_DRIVER_CAPA_CONTINUOUS_PMC:     return "CONTINUOUS_PMC";
    default:                                                            return "Undefined";
    }
}
