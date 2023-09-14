/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once
#include "JsonHandlerSDK.h"
#include "LogCollectorDefinitions.h"
#include "LogCollectorJsonValueParser.h"

class LogCollectorDumpRequest : public JsonDeviceRequest
{
public:
    explicit LogCollectorDumpRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<CpuType> GetCpuType() const
    {
        return LogCollectorJsonValueParser::ParseCpuType(m_jsonRequestValue, "CpuType");
    }
};

class LogCollectorDumpResponse : public JsonResponse
{
public:
    LogCollectorDumpResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorDumpResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetStringsDumpPath(const std::string& dumpFilePath)
    {
        m_jsonResponseValue["StringsDumpPath"] = dumpFilePath;
    }
};

class LogCollectorDumpHandler : public JsonOpCodeHandlerBase<LogCollectorDumpRequest, LogCollectorDumpResponse>
{
    void HandleRequest(const LogCollectorDumpRequest& jsonRequest, LogCollectorDumpResponse& jsonResponse) override;
    static OperationStatus StringsDump(const std::string& deviceName, CpuType cpuType);
};