/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <string>

#include "LogCollectorStartRecordingHandler.h"
#include "Device.h"
#include "LogCollectorDefinitions.h"
#include "Host.h"
#include "DeviceManager.h"
#include "LogCollectorService.h"
#include "PersistentConfiguration.h"

using namespace std;

void LogCollectorStartRecordingHandler::HandleRequest(
    const LogCollectorStartRecordingRequest& jsonRequest, LogCollectorStartRecordingResponse& jsonResponse)
{
    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    const std::string deviceName = spDevice->GetDeviceName();

    bool txtRecordingRequested = false;
    bool binRecordingRequested = true;
    bool publishingRequested = false;
    bool pmcDataRecordingRequested = false;
    const JsonValueBoxed<RecordingType> recordingTypeBoxed = jsonRequest.GetRecordingTypeBoxed();
    switch (recordingTypeBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_MISSING:
        // Use default settings (bin recording only)
        break;
    case JsonValueState::JSON_VALUE_PROVIDED:
    {
        const auto& recordingType = recordingTypeBoxed.GetValue();
        txtRecordingRequested = recordingType == RECORDING_TYPE_TXT || recordingType == RECORDING_TYPE_BOTH;
        binRecordingRequested = recordingType == RECORDING_TYPE_XML || recordingType == RECORDING_TYPE_BOTH;
        publishingRequested = recordingType == RECORDING_TYPE_PUBLISH;
        pmcDataRecordingRequested = recordingType == RECORDING_TYPE_PMC;
        break;
    }
    default:
        jsonResponse.Fail(recordingTypeBoxed.BuildStateReport());
        return;
    }

    if (pmcDataRecordingRequested)
    {
        jsonResponse.Fail("Invalid recording type 'pmc'. PMC data collection shall be managed through 'pmc' command group.");
        return;
    }

    // Determine CPU to record logs from
    bool fwRecordingRequested = true;
    bool uCodeRecordingRequested = true;
    const JsonValueBoxed<CpuType> cpuTypeBoxed = jsonRequest.GetCpuType();

    switch (cpuTypeBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_MISSING:
        break;
    case JsonValueState::JSON_VALUE_PROVIDED:
        fwRecordingRequested = (cpuTypeBoxed.GetValue() == CPU_TYPE_FW);
        uCodeRecordingRequested = (cpuTypeBoxed.GetValue() == CPU_TYPE_UCODE);
        break;
    default:
        jsonResponse.Fail(cpuTypeBoxed.BuildStateReport());
        return;
    }

    bool compressRequested = false;
    const JsonValueBoxed<bool> compressBoxed = jsonRequest.GetCompressBoxed();
    switch (compressBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_PROVIDED:
        compressRequested = compressBoxed.GetValue();
        break;
     case JsonValueState::JSON_VALUE_MISSING:
        break;
     default:
        jsonResponse.Fail(compressBoxed.BuildStateReport());
        return;
    }

    bool uploadRequested = false;
    const JsonValueBoxed<bool> uploadBoxed = jsonRequest.GetUploadBoxed();
    switch (uploadBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_PROVIDED:
        uploadRequested = uploadBoxed.GetValue();
        break;
    case JsonValueState::JSON_VALUE_MISSING:
        break;
    default:
        jsonResponse.Fail(uploadBoxed.BuildStateReport());
        return;
    }

    bool ignoreHistory = false;
    const JsonValueBoxed<bool> ignoreHistoryBoxed = jsonRequest.GetIgnoreHistoryBoxed();
    switch (ignoreHistoryBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_MISSING:
        // Use default settings (consume log history if available)
        break;
    case JsonValueState::JSON_VALUE_PROVIDED:
        ignoreHistory = ignoreHistoryBoxed.GetValue();
        break;
    default:
        jsonResponse.Fail(ignoreHistoryBoxed.BuildStateReport());
        return;
    }

    bool debugMode = false;
    const JsonValueBoxed<bool> debugModeBoxed = jsonRequest.GetDebugModeBoxed();
    switch (debugModeBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_MISSING:
        // Use default settings
        break;
    case JsonValueState::JSON_VALUE_PROVIDED:
        debugMode = debugModeBoxed.GetValue();
        break;
    default:
        jsonResponse.Fail(debugModeBoxed.BuildStateReport());
        return;
    }

    std::vector<LogCollectorModuleVerbosity> enforcedVerbosity;
    os = jsonRequest.GetModuleVerbosity(enforcedVerbosity);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    LOG_DEBUG << "Handling start recording request."
        << " Device: " << deviceName
        << " TXT Request: " << BoolStr(txtRecordingRequested)
        << " BIN Request: " << BoolStr(binRecordingRequested)
        << " FW Log Requested: " << BoolStr(fwRecordingRequested)
        << " uCode Log Requested: " << BoolStr(uCodeRecordingRequested)
        << " Publishing Requested: " << BoolStr(publishingRequested)
        << " Ignore log history: " << BoolStr(ignoreHistory)
        << " Compress Request: " << BoolStr(compressRequested)
        << " Upload Request: " << BoolStr(uploadRequested)
        << " Debug Mode Request: " << BoolStr(debugMode)
        << " Enforced verbosity: " << enforcedVerbosity
        << std::endl;

    if (txtRecordingRequested)
    {
        os = StartTxtRecording(
            deviceName, fwRecordingRequested, uCodeRecordingRequested, ignoreHistory, compressRequested, uploadRequested,
            debugMode, enforcedVerbosity);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    if (binRecordingRequested)
    {
        os = StartBinRecording(
            deviceName, fwRecordingRequested, uCodeRecordingRequested, ignoreHistory, compressRequested, uploadRequested,
            debugMode, enforcedVerbosity);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    if (publishingRequested)
    {
        os = StartPublishing(deviceName, fwRecordingRequested, uCodeRecordingRequested, ignoreHistory, enforcedVerbosity);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    jsonResponse.SetRecordingPath(Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation);
}

OperationStatus LogCollectorStartRecordingHandler::StartTxtRecording(
    const std::string& deviceName,
    bool fwRecordingRequested, bool uCodeRecordingRequested,
    bool ignoreLogHistory, bool compressRequested, bool uploadRequested, bool debugMode,
    const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity)
{
    if (fwRecordingRequested)
    {
        OperationStatus os = Host::GetHost().GetLogCollectorService().StartLogging(
            deviceName, CPU_TYPE_FW, TxtRecorder, ignoreLogHistory, compressRequested, uploadRequested,
            debugMode, enforcedVerbosity);
        if (!os)
        {
            return os;
        }
    }

    if (uCodeRecordingRequested)
    {
        OperationStatus os = Host::GetHost().GetLogCollectorService().StartLogging(
            deviceName, CPU_TYPE_UCODE, TxtRecorder, ignoreLogHistory, compressRequested, uploadRequested,
            debugMode, enforcedVerbosity);
        if (!os)
        {
            return os;
        }
    }

    return OperationStatus();
}

OperationStatus LogCollectorStartRecordingHandler::StartBinRecording(
    const std::string& deviceName,
    bool fwRecordingRequested, bool uCodeRecordingRequested,
    bool ignoreLogHistory, bool compressRequested, bool uploadRequested, bool debugMode,
    const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity)
{
    if (fwRecordingRequested)
    {
        OperationStatus os = Host::GetHost().GetLogCollectorService().StartLogging(
            deviceName, CPU_TYPE_FW, XmlRecorder, ignoreLogHistory, compressRequested, uploadRequested,
            debugMode, enforcedVerbosity);
        if (!os)
        {
            return os;
        }
    }
    if (uCodeRecordingRequested)
    {
        OperationStatus os = Host::GetHost().GetLogCollectorService().StartLogging(
            deviceName, CPU_TYPE_UCODE, XmlRecorder, ignoreLogHistory, compressRequested, uploadRequested,
            debugMode, enforcedVerbosity);
        if (!os)
        {
            return os;
        }
    }

    return OperationStatus();
}

OperationStatus LogCollectorStartRecordingHandler::StartPublishing(
    const std::string& deviceName,
    bool fwRecordingRequested, bool uCodeRecordingRequested, bool ignoreLogHistory,
    const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity)
{
    LoggingState fwPublishingState = LoggingState::DISABLED;
    LoggingState uCodePublishingState = LoggingState::DISABLED;
    string msg;

    OperationStatus os = Host::GetHost().GetLogCollectorService().GetLoggingState(
        deviceName, CPU_TYPE_FW, RawPublisher, fwPublishingState, msg);
    if (!os)
    {
        return os;
    }

    os = Host::GetHost().GetLogCollectorService().GetLoggingState(
        deviceName, CPU_TYPE_UCODE, RawPublisher, uCodePublishingState, msg);
    if (!os)
    {
        return os;
    }

    LOG_DEBUG << "Got publishing request."
        << " Current FW publishing state: " << fwPublishingState
        << " Current uCode publishing state: " << uCodePublishingState
        << endl;

    if (fwRecordingRequested && fwPublishingState != LoggingState::ACTIVE && fwPublishingState != LoggingState::ENABLED)
    {
        os = Host::GetHost().GetLogCollectorService().StartLogging(
            deviceName, CPU_TYPE_FW, RawPublisher, ignoreLogHistory, false, false, false, enforcedVerbosity);
        if (!os)
        {
            return os;
        }
    }

    if (uCodeRecordingRequested && uCodePublishingState != LoggingState::ACTIVE && uCodePublishingState != LoggingState::ENABLED)
    {
        os = Host::GetHost().GetLogCollectorService().StartLogging(
            deviceName, CPU_TYPE_UCODE, RawPublisher, ignoreLogHistory, false, false, false, enforcedVerbosity);
        if (!os)
        {
            return os;
        }
    }

    return OperationStatus();
}
