/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorGetHealthHandler.h"
#include "Host.h"
#include "Device.h"
#include "DeviceManager.h"
#include "LogCollectorService.h"

using namespace std;


void LogCollectorGetHealthResponse::SetHealth(const ChunkHealthReport& report, ClientType clientType)
{
    switch (clientType)
    {
    case ClientType::DM_TOOLS:
    {
        // need to divide state and rest data for future proper serialization in DmTools
        // as we do not export state to manifest file
        Json::Value reportWrapper;
        reportWrapper["State"] = LoggingStateToString(report.State);
        report.ToJson(reportWrapper["HealthData"], false); // report is nested value
        m_jsonResponseValue["HealthReport"] = reportWrapper;
        break;
    }
    case ClientType::SHELL_11_AD:
        report.ToJson(m_jsonResponseValue, false);
        break;
    default:
        Fail("Unsupported client type provided in request, expected 'shell_11ad or DmTools'");
    }
}

void LogCollectorGetHealthHandler::HandleRequest(const LogCollectorGetHealthRequest& jsonRequest,
    LogCollectorGetHealthResponse& jsonResponse)
{
    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    const std::string deviceName = spDevice->GetDeviceName();

    // Determine CPU
    const JsonValueBoxed<CpuType> cpuTypeBoxed = jsonRequest.GetCpuType();
    if (cpuTypeBoxed.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(cpuTypeBoxed.BuildStateReport());
        return;
    }
    if (cpuTypeBoxed == CPU_TYPE_BOTH) // support only single CPU type request
    {
        ostringstream errorBuilder;
        errorBuilder << "Cannot get health for " << deviceName << ", expected a single CPU type";
        jsonResponse.Fail(errorBuilder.str());
        return;
    }

    // Determine logging type
    LoggingType loggingType = TxtRecorder;
    const JsonValueBoxed<RecordingType> recordingTypeBoxed = jsonRequest.GetRecordingTypeBoxed();
    if (recordingTypeBoxed.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(recordingTypeBoxed.BuildStateReport());
        return;
    }
    switch (recordingTypeBoxed.GetValue())
    {
    case RECORDING_TYPE_TXT:
        loggingType = TxtRecorder;
        break;
    case RECORDING_TYPE_XML:
        loggingType = XmlRecorder;
        break;
    case RECORDING_TYPE_PUBLISH:
        loggingType = RawPublisher;
        break;
    default:
        ostringstream errorBuilder;
        errorBuilder << "Provided recording type: " << recordingTypeBoxed.GetValue() << " is not supported";
        jsonResponse.Fail(errorBuilder.str());
        return;
    }

    LOG_VERBOSE << "Handling get health request."
        << " Device: " << deviceName
        << " Requested CPU: " << cpuTypeBoxed
        << " Health Report requested for consumer: " << loggingType
        << std::endl;

    // Recorder is not disabled, get health report
    ChunkHealthReport healthReport;
    os = Host::GetHost().GetLogCollectorService().GetLoggingHealth(deviceName, cpuTypeBoxed, loggingType, healthReport);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetHealth(healthReport, jsonRequest.GetClientType());
}
