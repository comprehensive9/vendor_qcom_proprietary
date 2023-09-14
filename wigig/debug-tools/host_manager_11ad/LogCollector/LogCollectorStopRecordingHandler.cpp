/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorStopRecordingHandler.h"
#include "Device.h"
#include "Host.h"
#include "DeviceManager.h"
#include "LogCollectorService.h"
#include "PersistentConfiguration.h"

#include <string>

using namespace std;

void LogCollectorStopRecordingHandler::HandleRequest(
    const LogCollectorStopRecordingRequest& jsonRequest, LogCollectorStopRecordingResponse& jsonResponse)
{
    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    const std::string deviceName = spDevice->GetDeviceName();

    const JsonValueBoxed<CpuType> cpuTypeBoxed = jsonRequest.GetCpuType();
    CpuType cpuType;
    switch (cpuTypeBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_MALFORMED:
        jsonResponse.Fail(cpuTypeBoxed.BuildStateReport());
        return;
    case JsonValueState::JSON_VALUE_MISSING:
        cpuType = CPU_TYPE_BOTH;
        break;
    default:
        // guaranty that value is provided.
        cpuType = cpuTypeBoxed;
        break;
    }

    const JsonValueBoxed<RecordingType> recordingTypeBoxed = jsonRequest.GetRecordingTypeBoxed();
    switch (recordingTypeBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_MISSING:
    {
        // Check if there are any not disabled recorders
        if (!Host::GetHost().GetLogCollectorService().IsOnTargetLogRecording(deviceName, cpuType))
        {
            jsonResponse.SetMessage("No active recorders");
            return;
        }
        // Before stop on-target log recording, get avg miss rate and fatal errors if relevant
        vector<ChunkHealthReport> reports;
        os = GetRecordersReducedHealth(deviceName, cpuType, reports);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }

        // Stop on-target log recording, do not touch publishing or PMC data recording
        os = StopOnTargetRecording(deviceName, cpuType);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
        jsonResponse.SetReducedHealth(reports);
        jsonResponse.SetRecordingPath(Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation);
        break;
    }
    case JsonValueState::JSON_VALUE_PROVIDED:
    {
        // Only publishing is supported in this flow
        if (recordingTypeBoxed.GetValue() != RECORDING_TYPE_PUBLISH)
        {
            jsonResponse.Fail("Wrong RecordingType. Supported: 'publish'");
            return;
        }

        os = Host::GetHost().GetLogCollectorService().StopLogging(deviceName, cpuType, RawPublisher);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
        break;
    }
    default:
        jsonResponse.Fail("Wrong RecordingType. Supported: 'publish'");
        return;
    }

    LOG_DEBUG << "Log Collector stop recording for Device: " << deviceName
        << " with CPU type: " << cpuTypeBoxed << std::endl;
}

OperationStatus LogCollectorStopRecordingHandler::StopOnTargetRecording(const string& deviceName, CpuType cpuType)
{
    // Note: Publishing is not stopped unless explicitly required
    const OperationStatus os1 = Host::GetHost().GetLogCollectorService().StopLogging(deviceName, cpuType, XmlRecorder);
    const OperationStatus os2 = Host::GetHost().GetLogCollectorService().StopLogging(deviceName, cpuType, TxtRecorder);
    return OperationStatus::Merge(os1, os2);
}

OperationStatus LogCollectorStopRecordingHandler::GetRecordersReducedHealth(const string& deviceName, CpuType cpuType, vector<ChunkHealthReport>& reports)
{
    const OperationStatus os1 = Host::GetHost().GetLogCollectorService().GetRecordersHealth(deviceName, cpuType, XmlRecorder, reports);
    const OperationStatus os2 = Host::GetHost().GetLogCollectorService().GetRecordersHealth(deviceName, cpuType, TxtRecorder, reports);
    return OperationStatus::Merge(os1, os2);
}
