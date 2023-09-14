/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "LogCollectorJsonValueParser.h"

class LogCollectorSetConfigRequest : public JsonDeviceRequest
{
public:
    explicit LogCollectorSetConfigRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<uint32_t> GetPollingIntervalMs() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, POLLING_INTERVAL_MS.c_str());
    }

    JsonValueBoxed<uint32_t> GetMaxSingleFileSizeMb() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, MAX_SINGLE_FILE_SIZE_MB.c_str());
    }

    JsonValueBoxed<uint32_t> GetMaxNumOfLogFiles() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, MAX_NUM_OF_LOG_FILES.c_str());
    }

    JsonValueBoxed<bool> GetFlushChunk() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, FLUSH_CHUNK.c_str());
    }

    JsonValueBoxed<std::string> GetConversionFilePath() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, CONVERSION_FILE_PATH.c_str());
    }
    JsonValueBoxed<std::string> GetLogFilesDirectoryPath() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, LOG_FILES_DIRECTORY.c_str());
    }

    JsonValueBoxed<std::string> GetTargetServer() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, TARGET_SERVER.c_str());
    }
    JsonValueBoxed<std::string> GetUserName() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, USER_NAME.c_str());
    }
    JsonValueBoxed<std::string> GetRemotePath() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, REMOTE_PATH.c_str());
    }
};

class LogCollectorSetConfigResponse : public JsonResponse
{
public:
    LogCollectorSetConfigResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorSetConfigResponse", jsonRequestValue, jsonResponseValue)
    {
    }
};

class LogCollectorSetConfigHandler : public JsonOpCodeHandlerBase<LogCollectorSetConfigRequest, LogCollectorSetConfigResponse>
{
private:
    void HandleRequest(const LogCollectorSetConfigRequest& jsonRequest, LogCollectorSetConfigResponse& jsonResponse) override;
};

class LogCollectorReSetConfigResponse : public JsonResponse
{
public:
    LogCollectorReSetConfigResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorReSetConfigResponse", jsonRequestValue, jsonResponseValue)
    {
    }
};

class LogCollectorReSetConfigHandler
    : public JsonOpCodeHandlerBase<Json::Value, LogCollectorReSetConfigResponse>
{
private:
    void HandleRequest(const Json::Value& jsonRequestValue, LogCollectorReSetConfigResponse& jsonResponse) override;
};
