/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "LogCollectorDefinitions.h"
#include "LogCollectorJsonValueParser.h"
#include "ChunkHealthReport.h"

class LogCollectorGetHealthRequest : public JsonDeviceRequest
{
public:
    explicit LogCollectorGetHealthRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<CpuType> GetCpuType() const
    {
        return LogCollectorJsonValueParser::ParseCpuType(m_jsonRequestValue, "CpuType");
    }

    JsonValueBoxed<RecordingType> GetRecordingTypeBoxed() const
    {
        return LogCollectorJsonValueParser::ParseRecordingType(m_jsonRequestValue, "RecordingType");
    }
};

class LogCollectorGetHealthResponse : public JsonResponse
{
public:
    LogCollectorGetHealthResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorGetHealthResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetHealth(const ChunkHealthReport& report, ClientType clientType);
};

class LogCollectorGetHealthHandler : public JsonOpCodeHandlerBase<LogCollectorGetHealthRequest, LogCollectorGetHealthResponse>
{
private:
    void HandleRequest(const LogCollectorGetHealthRequest& jsonRequest, LogCollectorGetHealthResponse& jsonResponse) override;
};
