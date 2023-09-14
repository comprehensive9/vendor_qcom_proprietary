/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "DeviceLoggingContext.h"
#include "DebugLogger.h"
#include "Host.h"
#include "DeviceManager.h"
#include "Device.h"
#include "PinnedMemorySwitchboard.h"
#include "LogBufferReader.h"
#include "RingReader.h"

using namespace std;

DeviceLoggingContext::DeviceLoggingContext(const std::string& deviceName)
    : m_deviceName(deviceName)
{
    shared_ptr<Device> spDevice;
    const OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(m_deviceName, spDevice);
    if (!os)
    {
        LOG_ERROR << "Cannot create device logging context: " << os.GetStatusMessage() << endl;
        return;
    }

    m_isContinuousPmc = spDevice->IsContinuousPmcSupported();
    LOG_DEBUG << "On device " << deviceName << ", device logging context created in "
        << (m_isContinuousPmc ? "continuous PMC mode" : "Legacy mode") << endl;

    const BasebandType basebandType = spDevice->GetBasebandType();
    // If BB type unknown, we cannot read from log buffer (info address is 0xDEADEAD) and LogReader will not perform poll

    const auto logBufferInfoAddressFw = PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::REG_FW_USAGE_1, basebandType);
    const auto ahbToLinkerDeltaFw = ComputeAhbToLinkerDelta(basebandType, CPU_TYPE_FW);

    const auto logBufferInfoAddressUc = PinnedMemorySwitchboard::GetAhbAddress(PINNED_MEMORY::REG_FW_USAGE_2, basebandType);
    const auto ahbToLinkerDeltaUc = ComputeAhbToLinkerDelta(basebandType, CPU_TYPE_UCODE);

    // create all readers to prevent potential null dereference
    m_upFwLogCollector.reset(new LogBufferReader(m_deviceName, CPU_TYPE_FW, logBufferInfoAddressFw, ahbToLinkerDeltaFw));
    m_upUcodeLogCollector.reset(new LogBufferReader(m_deviceName, CPU_TYPE_UCODE, logBufferInfoAddressUc, ahbToLinkerDeltaUc));
    m_upRingReader.reset(new RingReader(m_deviceName, logBufferInfoAddressFw, ahbToLinkerDeltaFw,
        logBufferInfoAddressUc, ahbToLinkerDeltaUc));
}

DeviceLoggingContext::~DeviceLoggingContext() = default;

unsigned DeviceLoggingContext::ComputeAhbToLinkerDelta(BasebandType basebandType, CpuType cpuType)
{
    const std::map<BasebandType, unsigned>& memory_start_address_ahb = cpuType == CPU_TYPE_FW
        ? baseband_to_peripheral_memory_start_address_ahb
        : baseband_to_ucode_dccm_start_address_ahb;

    const std::map<BasebandType, unsigned>& memory_start_address_linker = cpuType == CPU_TYPE_FW
        ? baseband_to_peripheral_memory_start_address_linker
        : baseband_to_ucode_dccm_start_address_linker;

    const auto iterAhb = memory_start_address_ahb.find(basebandType);
    const auto iterLinker = memory_start_address_linker.find(basebandType);

    if (iterAhb == memory_start_address_ahb.cend()
        || iterLinker == memory_start_address_linker.cend())
    {
        LOG_ERROR << cpuType << " Log collector does not support baseband type " << basebandType << endl;
        return 0U;
    }

    return iterAhb->second - iterLinker->second;
}

LogReaderBase* DeviceLoggingContext::GetLogReader(CpuType cpuType)
{
    return const_cast<LogReaderBase*>(const_cast<const DeviceLoggingContext*>(this)->GetLogReader(cpuType));
}
const LogReaderBase* DeviceLoggingContext::GetLogReader(CpuType cpuType) const
{
    const auto& logReader = m_isContinuousPmc ? m_upRingReader
        : cpuType == CPU_TYPE_FW ? m_upFwLogCollector : m_upUcodeLogCollector;
    if (!logReader) // shouldn't happen
    {
        LOG_ERROR << "Failed to retrieve " << cpuType << " log reader when Continuous PMC mode is " << BoolStr(m_isContinuousPmc)
            << ", going to use FW Log Reader as fallback!!!" << endl;
        return m_upFwLogCollector.get();
    }
    return logReader.get();
}

// Check log collection is the same as during its first activation
OperationStatus DeviceLoggingContext::CheckCollectionModeValidity() const
{
    shared_ptr<Device> device;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(m_deviceName, device);
    if (!os)
    {
        return os;
    }

    if (m_isContinuousPmc != device->IsContinuousPmcSupported())
    {
        ostringstream oss;
        oss << "On device " << m_deviceName
            << ", log collection mode changed from "
            << (m_isContinuousPmc ? "PMC to Legacy" : "Legacy to PMC");
        return OperationStatus(false, oss.str());
    }

    return OperationStatus();
}

OperationStatus DeviceLoggingContext::ActivateLogging(
    CpuType cpuType, LoggingType loggingType, bool ignoreLogHistory, bool compress, bool upload,
    const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity,
    RecordingTrigger recordingTrigger, bool debugMode)
{
    if (cpuType == CPU_TYPE_BOTH)
    {
        ostringstream oss;
        oss << "Cannot activate logging for " << m_deviceName << ", expected a single CPU type";
        return OperationStatus(false, oss.str());
    }

    // check log collection is the same as during its first activation
    OperationStatus os = CheckCollectionModeValidity();
    if (!os)
    {
        os.AddPrefix("Cannot activate logging");
        return os;
    }

    return GetLogReader(cpuType)->ActivateLogging(cpuType, loggingType, ignoreLogHistory, compress, upload, enforcedVerbosity, recordingTrigger, debugMode);
}

bool DeviceLoggingContext::IsPollerActive(CpuType cpuType) const
{
    return GetLogReader(cpuType)->IsPollerActive();
}

OperationStatus DeviceLoggingContext::DeactivateLogging(CpuType cpuType, LoggingType loggingType)
{
    return GetLogReader(cpuType)->DeactivateLogging(cpuType, loggingType);
}

LoggingState DeviceLoggingContext::GetLoggingState(CpuType cpuType, LoggingType loggingType) const
{
    return GetLogReader(cpuType)->GetLoggingState(cpuType, loggingType);
}

const std::string& DeviceLoggingContext::GetLoggingStateMsg(CpuType cpuType, LoggingType loggingType) const
{
    return GetLogReader(cpuType)->GetLoggingStateMsg(cpuType, loggingType);
}

ChunkHealthReport DeviceLoggingContext::GetLoggingHealth(CpuType cpuType, LoggingType loggingType) const
{
    return GetLogReader(cpuType)->GetLoggingHealth(cpuType, loggingType);
}

LogFmtStringsContainer DeviceLoggingContext::GetFmtStrings(CpuType cpuType, LoggingType loggingType) const
{
    return GetLogReader(cpuType)->GetFmtStrings(cpuType, loggingType);
}

OperationStatus DeviceLoggingContext::EnforceModuleVerbosity(CpuType cpuType)
{
    return GetLogReader(cpuType)->EnforceModuleVerbosity(cpuType);
}

OperationStatus DeviceLoggingContext::SetModuleVerbosity(CpuType cpuType, const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity)
{
    return GetLogReader(cpuType)->SetModuleVerbosity(cpuType, enforcedVerbosity);
}

OperationStatus DeviceLoggingContext::GetModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity)
{
    return GetLogReader(cpuType)->GetModuleVerbosity(cpuType, moduleVerbosity);
}

OperationStatus DeviceLoggingContext::GetEnforcedModuleVerbosity(CpuType cpuType, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity)
{
    return GetLogReader(cpuType)->GetEnforcedModuleVerbosity(cpuType, moduleVerbosity);
}

void DeviceLoggingContext::RestoreLoggingState()
{
    LOG_DEBUG << "Restoring logging state for " << m_deviceName << endl;

    // check log collection is the same as during its first activation
    OperationStatus os = CheckCollectionModeValidity();
    if (!os)
    {
        LOG_ERROR << "Cannot restore logging: " << os.GetStatusMessage() << endl;
        return;
    }

    if (m_isContinuousPmc)
    {
        // will restore logging for both cpu types
        os = m_upRingReader->RestoreLogging();
    }
    else
    {
        os = OperationStatus::Merge(
            m_upFwLogCollector->RestoreLogging(),
            m_upUcodeLogCollector->RestoreLogging());
    }

    if (!os)
    {
        LOG_ERROR << "Cannot restore logging: " << os.GetStatusMessage() << endl;
        return;
    }
}

void DeviceLoggingContext::UnregisterPollerAndReportDeviceRemoved()
{
    if (m_isContinuousPmc)
    {
        m_upRingReader->UnRegisterPoller();
        m_upRingReader->ReportDeviceRemoved();
        return;
    }

    m_upFwLogCollector->UnRegisterPoller();
    m_upUcodeLogCollector->UnRegisterPoller();

    m_upFwLogCollector->ReportDeviceRemoved();
    m_upUcodeLogCollector->ReportDeviceRemoved();
}

OperationStatus DeviceLoggingContext::SplitRecordings()
{
    if (m_isContinuousPmc)
    {
        // will restore logging for both cpu types
        return m_upRingReader->SplitRecordings();
    }

    return OperationStatus::Merge(
        m_upFwLogCollector->SplitRecordings(),
        m_upUcodeLogCollector->SplitRecordings());
}
