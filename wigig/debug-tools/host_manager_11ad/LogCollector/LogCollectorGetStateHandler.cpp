/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
*
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorGetStateHandler.h"

#include "Device.h"
#include "Host.h"
#include "DeviceManager.h"
#include "LogCollectorService.h"

void LogCollectorGetStateHandler::HandleRequest(
    const LogCollectorGetStateRequest& jsonRequest, LogCollectorGetStateResponse& jsonResponse)
{
    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    const std::string deviceName = spDevice->GetDeviceName();

    LoggingState loggingState = LoggingState::DISABLED;
    std::string loggingStateMsg;

    os = Host::GetHost().GetLogCollectorService().GetLoggingState(deviceName, CPU_TYPE_FW, XmlRecorder, loggingState, loggingStateMsg); // active
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetXmlRecordingState(loggingState, loggingStateMsg, CPU_TYPE_FW);

    os = Host::GetHost().GetLogCollectorService().GetLoggingState(deviceName, CPU_TYPE_FW, TxtRecorder, loggingState, loggingStateMsg); // active
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetTxtRecordingState(loggingState, loggingStateMsg, CPU_TYPE_FW);

    os = Host::GetHost().GetLogCollectorService().GetLoggingState(deviceName, CPU_TYPE_FW, RawPublisher, loggingState, loggingStateMsg);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetPublishingState(loggingState, loggingStateMsg, CPU_TYPE_FW);

    // uCode

    os = Host::GetHost().GetLogCollectorService().GetLoggingState(deviceName, CPU_TYPE_UCODE, XmlRecorder, loggingState, loggingStateMsg);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetXmlRecordingState(loggingState, loggingStateMsg, CPU_TYPE_UCODE);

    os = Host::GetHost().GetLogCollectorService().GetLoggingState(deviceName, CPU_TYPE_UCODE, TxtRecorder, loggingState, loggingStateMsg);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetTxtRecordingState(loggingState, loggingStateMsg, CPU_TYPE_UCODE);

    os = Host::GetHost().GetLogCollectorService().GetLoggingState(deviceName, CPU_TYPE_UCODE, RawPublisher, loggingState, loggingStateMsg);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetPublishingState(loggingState, loggingStateMsg, CPU_TYPE_UCODE);

    // PMC Data
    os = Host::GetHost().GetLogCollectorService().GetLoggingState(deviceName, CPU_TYPE_FW, PmcDataRecorder, loggingState, loggingStateMsg);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetPmcDataRecordingState(loggingState, loggingStateMsg, CPU_TYPE_FW);
}

void LogCollectorGetStateResponse::SetXmlRecordingState(LoggingState loggingState, const std::string& loggingStateMsg, CpuType cpuType) const
{
    const std::string value = FormatState(loggingState, loggingStateMsg);
    if (cpuType == CPU_TYPE_FW)
    {
        m_jsonResponseValue["FwRawRecording"] = value;
    }
    else
    {
        m_jsonResponseValue["uCodeRawRecording"] = value;
    }
}

void LogCollectorGetStateResponse::SetTxtRecordingState(LoggingState loggingState, const std::string& loggingStateMsg, CpuType cpuType) const
{
    const std::string value = FormatState(loggingState, loggingStateMsg);
    if (cpuType == CPU_TYPE_FW)
    {
        m_jsonResponseValue["FwTxtRecording"] = value;
    }
    else
    {
        m_jsonResponseValue["uCodeTxtRecording"] = value;
    }
}

void LogCollectorGetStateResponse::SetPublishingState(LoggingState loggingState, const std::string& loggingStateMsg, CpuType cpuType) const
{
    const std::string value = FormatState(loggingState, loggingStateMsg);
    if (cpuType == CPU_TYPE_FW)
    {
        m_jsonResponseValue["FwPublishing"] = value;
    }
    else
    {
        m_jsonResponseValue["uCodePublishing"] = value;
    }
}

void LogCollectorGetStateResponse::SetPmcDataRecordingState(LoggingState loggingState, const std::string& loggingStateMsg, CpuType cpuType) const
{
    (void)cpuType;
    const std::string value = FormatState(loggingState, loggingStateMsg);
    m_jsonResponseValue["PmcDataRecording"] = value;
}

std::string LogCollectorGetStateResponse::FormatState(LoggingState loggingState, const std::string& loggingStateMsg)
{
    std::ostringstream ss;
    ss << loggingState;
    if (!loggingStateMsg.empty())
    {
        ss << " [" << loggingStateMsg << "]";
    }
    return ss.str();
}
