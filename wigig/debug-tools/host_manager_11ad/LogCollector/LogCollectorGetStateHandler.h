/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "LogCollectorJsonValueParser.h"

// *************************************************************************************************

class LogCollectorGetStateRequest : public JsonDeviceRequest
{
public:
    LogCollectorGetStateRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<CpuType> GetCpuType() const
    {
        return LogCollectorJsonValueParser::ParseCpuType(m_jsonRequestValue, "CpuType");
    }
};

// *************************************************************************************************

class LogCollectorGetStateResponse : public JsonResponse
{
public:
    LogCollectorGetStateResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorGetStateResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetXmlRecordingState(LoggingState loggingState, const std::string& loggingStateMsg, CpuType cpuType) const;
    void SetTxtRecordingState(LoggingState loggingState, const std::string& loggingStateMsg, CpuType cpuType) const;
    void SetPublishingState(LoggingState loggingState, const std::string& loggingStateMsg, CpuType cpuType) const;
    void SetPmcDataRecordingState(LoggingState loggingState, const std::string& loggingStateMsg, CpuType cpuType) const;

private:
    static std::string FormatState(LoggingState loggingState, const std::string& loggingStateMsg);
};

// *************************************************************************************************

class LogCollectorGetStateHandler
: public JsonOpCodeHandlerBase<LogCollectorGetStateRequest, LogCollectorGetStateResponse>
{
private:
    void HandleRequest(
        const LogCollectorGetStateRequest& jsonRequest, LogCollectorGetStateResponse& jsonResponse) override;
};
