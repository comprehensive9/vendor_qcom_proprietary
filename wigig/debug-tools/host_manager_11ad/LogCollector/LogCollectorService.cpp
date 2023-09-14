/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorService.h"
#include "Device.h"
#include "DriverContracts.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "DebugLogger.h"
#include "DeviceManager.h"
#include "FileSystemOsAbstraction.h"
#include "DeviceLoggingContext.h"
#include "SharedVersionInfo.h"
#include "LogCollectorEvents.h"

#include <locale> // for std::tolower
#include <string>
#include <cstring>

using namespace std;

const std::string LogCollectorService::sc_fwStringConversionFilename = "fw_image_trace_string_load.bin";
const std::string LogCollectorService::sc_ucodeStringConversionFilename = "ucode_image_trace_string_load.bin";

// Should be defined here to allow forward declaration in the header
LogCollectorService::LogCollectorService() = default;
LogCollectorService::~LogCollectorService() = default;

void LogCollectorService::OnDeviceDiscovered(const std::string& deviceName)
{
    /* 1. check if in map
     * 1.1 if not in map:
     * 1.1.1 add to map.
     * 1.1.2 start deferred collectors
     * 1.2 if in map:
     * 1.2.1 activate previous recordings
     * 1.2.2 add deferred recordings if needed
     * 1.2.3 report in new log file
     */

    auto deviceContext = m_DeviceLoggingContextMap.find(deviceName);
    if (deviceContext == m_DeviceLoggingContextMap.end())
    {
        // This is the first time this device is discovered - no persistent recording is required
        LOG_DEBUG << "Log Service: no context exists for: " << deviceName << endl;
        std::unique_ptr<DeviceLoggingContext> newDeviceLoggingContext(new DeviceLoggingContext(deviceName));

        if (m_spDeferredRecordingCtx) // if deferred recording is active
        {
            LOG_INFO << "Trigger deferred recording."
                << " Context: " << *m_spDeferredRecordingCtx
                << " Device: " << deviceName
                << endl;

            OperationStatus os = newDeviceLoggingContext->ActivateLogging(
                CPU_TYPE_FW,
                m_spDeferredRecordingCtx->GetLoggingType(),
                false,
                m_spDeferredRecordingCtx->IsCompress(),
                m_spDeferredRecordingCtx->IsUpload(),
                m_spDeferredRecordingCtx->GetModuleVerbosity(),
                RecordingTrigger::Deferred, m_spDeferredRecordingCtx->IsDebugMode());

            if (!os)
            {
                LOG_ERROR << os.GetStatusMessage() << endl;
            }

            os = newDeviceLoggingContext->ActivateLogging(
                CPU_TYPE_UCODE,
                m_spDeferredRecordingCtx->GetLoggingType(),
                false,
                m_spDeferredRecordingCtx->IsCompress(),
                m_spDeferredRecordingCtx->IsUpload(),
                m_spDeferredRecordingCtx->GetModuleVerbosity(),
                RecordingTrigger::Deferred, m_spDeferredRecordingCtx->IsDebugMode());
            if (!os)
            {
                LOG_ERROR << os.GetStatusMessage() << endl;
            }
        }

        LOG_DEBUG << "Log Service: added new logging context for: " << deviceName << endl;
        m_DeviceLoggingContextMap.insert(std::make_pair(deviceName, std::move(newDeviceLoggingContext)));

        return;
    }

    LOG_DEBUG << "Log Service: context exists for: " << deviceName << endl;
    // device was rediscovered
    deviceContext->second->RestoreLoggingState();

    // if deferred recording is active, trigger it only if no persistent recording was activated
    // note: we do not trigger deferred recording if the persistent recording was activated for a single CPU type
    if (m_spDeferredRecordingCtx
        && !deviceContext->second->IsPollerActive(CPU_TYPE_FW)
        && !deviceContext->second->IsPollerActive(CPU_TYPE_UCODE))
    {
        LOG_INFO << "Trigger deferred recording."
            << " Context: " << *m_spDeferredRecordingCtx
            << " Device: " << deviceName
            << endl;

        OperationStatus os = deviceContext->second->ActivateLogging(
            CPU_TYPE_FW,
            m_spDeferredRecordingCtx->GetLoggingType(),
            false,
            m_spDeferredRecordingCtx->IsCompress(),
            m_spDeferredRecordingCtx->IsUpload(),
            m_spDeferredRecordingCtx->GetModuleVerbosity(),
            RecordingTrigger::Deferred, m_spDeferredRecordingCtx->IsDebugMode());
        if (!os)
        {
            LOG_ERROR << os.GetStatusMessage() << endl;
        }

        os = deviceContext->second->ActivateLogging(
            CPU_TYPE_UCODE,
            m_spDeferredRecordingCtx->GetLoggingType(),
            false,
            m_spDeferredRecordingCtx->IsCompress(),
            m_spDeferredRecordingCtx->IsUpload(),
            m_spDeferredRecordingCtx->GetModuleVerbosity(),
            RecordingTrigger::Deferred, m_spDeferredRecordingCtx->IsDebugMode());
        if (!os)
        {
            LOG_ERROR << os.GetStatusMessage() << endl;
        }
    }
}

void LogCollectorService::OnDeviceRemoved(const std::string& deviceName)
{
    LOG_DEBUG << "Log Service: Device removal reported for " << deviceName << std::endl;

    /* 1. if not in map report warning and return true!
     * 2. if in map:
     * 2.1 pause all recordings (consider stop all pollers)
     * 2.2 report to end of current log
     *
     */
    auto deviceContext = m_DeviceLoggingContextMap.find(deviceName);
    if (deviceContext == m_DeviceLoggingContextMap.end())
    {
        LOG_ERROR << "The device: " << deviceName << " was removed form device manager but was not found on LogCollectorService" << endl;
        return;
    }

    deviceContext->second->UnregisterPollerAndReportDeviceRemoved();
}


OperationStatus LogCollectorService::StartLogging(
    const std::string& deviceName, CpuType cpuType, LoggingType loggingType,
    bool ignoreLogHistory, bool compress, bool upload, bool debugMode,
    const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity)
{
    static const char* szWinOsName = "Windows";

    // Post collection steps are not supported on Windows!
    if((compress || upload) && (strncmp(GetOperatingSystemName(), szWinOsName, strlen(szWinOsName)) == 0))
    {
        return OperationStatus(false, "Compress and Upload are not supported on non linux system");
    }

    const auto it = m_DeviceLoggingContextMap.find(deviceName);
    if (it == m_DeviceLoggingContextMap.end())
    {
        return OperationStatus(false, "No Logging Context for : " + deviceName);
    }

    // note: single cpu type enforcement is done inside activate logging implementation
    return it->second->ActivateLogging(
        cpuType, loggingType, ignoreLogHistory, compress, upload, enforcedVerbosity, RecordingTrigger::ExplicitStart, debugMode);
}

OperationStatus LogCollectorService::StopLogging(const std::string& deviceName, CpuType cpuType, LoggingType loggingType)
{
    auto DeviceLoggingContext = m_DeviceLoggingContextMap.find(deviceName);
    if (DeviceLoggingContext == m_DeviceLoggingContextMap.end())
    {
        return OperationStatus(false, "No Logging Context for this device: " + deviceName);
    }

    OperationStatus os1, os2;
    if (cpuType == CPU_TYPE_FW || cpuType == CPU_TYPE_BOTH)
    {
        os1 = DeviceLoggingContext->second->DeactivateLogging(CPU_TYPE_FW, loggingType);
    }
    if (cpuType == CPU_TYPE_UCODE || cpuType == CPU_TYPE_BOTH)
    {
        os2 = DeviceLoggingContext->second->DeactivateLogging(CPU_TYPE_UCODE, loggingType);
    }

    return OperationStatus::Merge(os1, os2);
}

OperationStatus LogCollectorService::SplitAllRecordings()
{
    OperationStatus os;
    for (const auto& dc : m_DeviceLoggingContextMap)
    {
        if (!IsOnTargetLogRecording(dc.first, CPU_TYPE_BOTH))
        {
            os = OperationStatus::Merge(os, OperationStatus(false, "No active recorders for device " + dc.first));
        }
        else
        {
            os = OperationStatus::Merge(os, dc.second->SplitRecordings());
        }
    }
    return os;
}

OperationStatus LogCollectorService::GetLoggingState(
    const std::string& deviceName, CpuType cpuType, LoggingType loggingType,
    LoggingState& loggingState, string& loggingStateMsg) const
{
    const auto deviceLoggingContext = m_DeviceLoggingContextMap.find(deviceName);
    if (deviceLoggingContext == m_DeviceLoggingContextMap.end())
    {
        return OperationStatus(false, "No Logging Context for this device: " + deviceName);
    }

    if (cpuType == CPU_TYPE_BOTH)
    {
        ostringstream oss;
        oss << "Cannot get logging state for " << deviceName << ", expected a single CPU type";
        return OperationStatus(false, oss.str());
    }

    loggingState = deviceLoggingContext->second->GetLoggingState(cpuType, loggingType);
    loggingStateMsg = deviceLoggingContext->second->GetLoggingStateMsg(cpuType, loggingType);
    return OperationStatus();
}

OperationStatus LogCollectorService::GetLoggingHealth(const std::string& deviceName, CpuType cpuType,
    LoggingType loggingType, ChunkHealthReport& healthReport) const
{
    const auto iteratorLoggingContext = m_DeviceLoggingContextMap.find(deviceName);
    if (iteratorLoggingContext == m_DeviceLoggingContextMap.end())
    {
        return OperationStatus(false, "No Logging Context for this device: " + deviceName);
    }

    healthReport = iteratorLoggingContext->second->GetLoggingHealth(cpuType, loggingType);
    return OperationStatus();
}

OperationStatus LogCollectorService::GetRecordersHealth(const std::string& deviceName, CpuType cpuType, LoggingType loggingType, std::vector<ChunkHealthReport>& reports) const
{
    ChunkHealthReport report;
    OperationStatus os1, os2;
    if (cpuType == CPU_TYPE_FW || cpuType == CPU_TYPE_BOTH)
    {
        os1 = GetLoggingHealth(deviceName, CPU_TYPE_FW, loggingType, report);
        reports.emplace_back(report);
    }
    if (cpuType == CPU_TYPE_UCODE || cpuType == CPU_TYPE_BOTH)
    {
        os2 = GetLoggingHealth(deviceName, CPU_TYPE_UCODE, loggingType, report);
        reports.emplace_back(report);
    }

    return OperationStatus::Merge(os1, os2);
}

OperationStatus LogCollectorService::GetFmtStrings(const std::string& deviceName, CpuType cpuType,
    LoggingType loggingType, LogFmtStringsContainer& fmtStrings) const
{
    const auto it = m_DeviceLoggingContextMap.find(deviceName);
    if (it == m_DeviceLoggingContextMap.end())
    {
        return OperationStatus(false, "No Logging Context for this device: " + deviceName);
    }

    fmtStrings = it->second->GetFmtStrings(cpuType, loggingType);
    return OperationStatus();
}

bool LogCollectorService::IsLogging()
{
    for (const auto& kv : m_DeviceLoggingContextMap)
    {
        if (kv.second->IsPollerActive(CPU_TYPE_FW) || kv.second->IsPollerActive(CPU_TYPE_UCODE))
        {
            // if poller is active then there is at least one not disabled chunk consumer
            return true;
        }
    }
    return false;
}

bool LogCollectorService::IsOnTargetLogRecording(const string& deviceName, CpuType cpuType) const
{
    bool isAnyRecordingActive = false;
    auto loggingState = LoggingState::DISABLED;
    string tmp;
    if (cpuType == CPU_TYPE_FW || cpuType == CPU_TYPE_BOTH)
    {
        GetLoggingState(deviceName, CPU_TYPE_FW, TxtRecorder, loggingState, tmp);
        isAnyRecordingActive |= loggingState != LoggingState::DISABLED;
        GetLoggingState(deviceName, CPU_TYPE_FW, XmlRecorder, loggingState, tmp);
        isAnyRecordingActive |= loggingState != LoggingState::DISABLED;
    }

    if (cpuType == CPU_TYPE_UCODE || cpuType == CPU_TYPE_BOTH)
    {
        GetLoggingState(deviceName, CPU_TYPE_UCODE, TxtRecorder, loggingState, tmp);
        isAnyRecordingActive |= loggingState != LoggingState::DISABLED;
        GetLoggingState(deviceName, CPU_TYPE_UCODE, XmlRecorder, loggingState, tmp);
        isAnyRecordingActive |= loggingState != LoggingState::DISABLED;
    }

    // Note: PmcDataRecorder is not consider as recording even when active, just as RawPublisher
    return isAnyRecordingActive;
}


void LogCollectorService::NotifyFwStateChange(const string& deviceName, wil_fw_state fwState)
{
    auto deviceLoggingContext = m_DeviceLoggingContextMap.find(deviceName);
    if (deviceLoggingContext == m_DeviceLoggingContextMap.end())
    {
        return;
    }

    if (wil_fw_state::WIL_FW_STATE_READY == fwState)
    {
        LOG_INFO << "FW is ready - Apply enforced verbosity if any" << endl;
        // FW state change may also indicate FW replacement, so previously discovered
        // logging parameters may be invalid.
        deviceLoggingContext->second->EnforceModuleVerbosity(CPU_TYPE_FW);
        deviceLoggingContext->second->EnforceModuleVerbosity(CPU_TYPE_UCODE);
    }
}


// The new method to start deferred recording/ publishing is to add the required chunk consumer type
// to the set m_deferredLoggingType and then every new device will get those consumers on creation!
OperationStatus LogCollectorService::EnableDeferredRecording(
    shared_ptr<DeferredRecordingContext> spDeferredRecordingContext)
{
    if (m_spDeferredRecordingCtx)
    {
        ostringstream oss;
        oss << "Deferred logging is already active: " << m_spDeferredRecordingCtx;
        return OperationStatus(false, oss.str());
    }

    LOG_INFO << "Enable deferred recording: " << *spDeferredRecordingContext << endl;

    if (spDeferredRecordingContext->GetLoggingType() == TxtRecorder)
    {
        // make sure conversion files exist in the configured location, make this common code to check conversion file validity.
        const string conversionFileDirectory = Host::GetHost().GetConfiguration().LogCollectorConfiguration.ConversionFileLocation;

        const bool fwConversionFileFound = FileSystemOsAbstraction::DoesFileExist(conversionFileDirectory + sc_fwStringConversionFilename);
        const bool ucodeConversionFileFound = FileSystemOsAbstraction::DoesFileExist(conversionFileDirectory + sc_ucodeStringConversionFilename);

        if ( !(fwConversionFileFound && ucodeConversionFileFound) )
        {
            ostringstream oss;
            oss << "Following conversion files are missing from "
                << conversionFileDirectory << " directory: "
                << (!fwConversionFileFound ? sc_fwStringConversionFilename : "")
                << " "
                << (!ucodeConversionFileFound ? sc_ucodeStringConversionFilename : "");
            return OperationStatus(false, oss.str());
        }
    }

    m_spDeferredRecordingCtx = spDeferredRecordingContext;
    return OperationStatus();
}

OperationStatus LogCollectorService::DisableDeferredLogging()
{
    if (!m_spDeferredRecordingCtx)
    {
        // not a failure
        return OperationStatus(true, "No deferred recording was enabled");
    }

    LOG_INFO << "Disable deferred recording: " << *m_spDeferredRecordingCtx << endl;
    // No need to stop existing recordings only disable deferred mode for future recordings
    m_spDeferredRecordingCtx.reset();
    return OperationStatus();
}

OperationStatus LogCollectorService::SetModuleVerbosity(
    const string& deviceName, CpuType cpuType, const vector<LogCollectorModuleVerbosity>& enforcedVerbosity)
{
    LOG_INFO << "Applying " << cpuType << " log verbosity: " << enforcedVerbosity << endl;

    auto deviceLoggingContext = m_DeviceLoggingContextMap.find(deviceName);
    if (deviceLoggingContext == m_DeviceLoggingContextMap.end())
    {
        ostringstream oss;
        oss << "Device not found: " << deviceName << endl;
        return OperationStatus(false, oss.str());
    }

    if (cpuType == CPU_TYPE_BOTH)
    {
        ostringstream oss;
        oss << "Cannot set module verbosity for " << deviceName << ", expected a single CPU type";
        return OperationStatus(false, oss.str());
    }

    return deviceLoggingContext->second->SetModuleVerbosity(cpuType, enforcedVerbosity);
}

OperationStatus LogCollectorService::GetModuleVerbosity(
    const string& deviceName, CpuType cpuType, vector<LogCollectorModuleVerbosity> &moduleVerbosity)
{
    auto deviceLoggingContext = m_DeviceLoggingContextMap.find(deviceName);
    if (deviceLoggingContext == m_DeviceLoggingContextMap.end())
    {
        ostringstream oss;
        oss << "Device not found: " << deviceName << endl;
        return OperationStatus(false, oss.str());
    }

    if (cpuType == CPU_TYPE_BOTH)
    {
        ostringstream oss;
        oss << "Cannot get module verbosity for " << deviceName << ", expected a single CPU type";
        return OperationStatus(false, oss.str());
    }

    OperationStatus os = deviceLoggingContext->second->GetModuleVerbosity(cpuType, moduleVerbosity);
    if (!os)
    {
        return os;
    }

    LOG_DEBUG << "Querying " << cpuType << " log verbosity: " << moduleVerbosity << endl;
    return OperationStatus();
}

OperationStatus LogCollectorService::GetEnforcedModuleVerbosity(
    const std::string& deviceName, CpuType cpuType, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity)
{
    auto deviceLoggingContext = m_DeviceLoggingContextMap.find(deviceName);
    if (deviceLoggingContext == m_DeviceLoggingContextMap.end())
    {
        ostringstream oss;
        oss << "Device not found: " << deviceName << endl;
        return OperationStatus(false, oss.str());
    }

    return deviceLoggingContext->second->GetEnforcedModuleVerbosity(cpuType, moduleVerbosity);
}

void LogCollectorService::PublishLoggingHealth(const std::string& deviceName) const
{
    ChunkHealthReport fwHealthReport;
    OperationStatus os = GetLoggingHealth(deviceName, CPU_TYPE_FW, RawPublisher, fwHealthReport);
    if (!os)
    {
        return;
    }

    ChunkHealthReport uCodeHealthReport;
    os = GetLoggingHealth(deviceName, CPU_TYPE_UCODE, RawPublisher, uCodeHealthReport);
    if (!os)
    {
        return;
    }

    if (fwHealthReport.State == LoggingState::DISABLED && uCodeHealthReport.State == LoggingState::DISABLED)
    {
        return; // raw publishing is not active, nothing to report
    }

    LOG_VERBOSE << "Sending FW and uCode logs health reports" << endl;
    Host::GetHost().PushEvent(LogHealthEvent(deviceName, fwHealthReport, uCodeHealthReport));
}

const string& LogCollectorService::GetStringConversionFilename(CpuType cpuType)
{
    return cpuType == CPU_TYPE_FW ? sc_fwStringConversionFilename : sc_ucodeStringConversionFilename;
}