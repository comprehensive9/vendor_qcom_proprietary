/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "LogCollectorJsonValueParser.h"
#include "OperationStatus.h"
#include "ChunkHealthReport.h"
#include <string>

namespace Json
{
    class Value;
}

class LogCollectorStopRecordingRequest final: public JsonDeviceRequest
{
public:
    explicit LogCollectorStopRecordingRequest(const Json::Value& jsonRequestValue) :
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

class LogCollectorStopRecordingResponse final: public JsonResponse
{
public:
    LogCollectorStopRecordingResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorStopRecordingResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetRecordingPath(const std::string & recPath) const
    {
        m_jsonResponseValue["RecordingPath"] = recPath;
    }

    void SetMessage(const std::string & msg) const
    {
        m_jsonResponseValue["Message"] = msg;
    }

    void SetReducedHealth(const std::vector<ChunkHealthReport>& reports) const
    {
        for (const auto &report: reports)
        {
            report.ToJson(m_jsonResponseValue, false, true);
        }
    }
};

class LogCollectorStopRecordingHandler final :
    public JsonOpCodeHandlerBase<LogCollectorStopRecordingRequest, LogCollectorStopRecordingResponse>
{
private:
    void HandleRequest(
        const LogCollectorStopRecordingRequest& jsonRequest, LogCollectorStopRecordingResponse& jsonResponse) override;

    static OperationStatus StopOnTargetRecording(const std::string& deviceName, CpuType cpuType);
    static OperationStatus GetRecordersReducedHealth(const std::string& deviceName, CpuType cpuType, std::vector<ChunkHealthReport>& reports);
};