/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "LogCollectorJsonValueParser.h"

class LogCollectorGetConfigRequest : public JsonDeviceRequest
{
public:
    LogCollectorGetConfigRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }
};

class LogCollectorGetConfigResponse : public JsonResponse
{
public:
    LogCollectorGetConfigResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorGetConfigResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetPollingIntervalMS(uint32_t value)
    {
        m_jsonResponseValue[POLLING_INTERVAL_MS] = value;
    }

    void SetMaxSingleFileSizeMB(uint32_t value)
    {
        m_jsonResponseValue[MAX_SINGLE_FILE_SIZE_MB] = value;
    }

    void SetMaxNumOfLogFiles(uint32_t value)
    {
        m_jsonResponseValue[MAX_NUM_OF_LOG_FILES] = value;
    }

    void SetFlushChunk(bool value)
    {
        m_jsonResponseValue[FLUSH_CHUNK] = value;
    }

    void SetConversionFilePath(const std::string& value)
    {
        m_jsonResponseValue[CONVERSION_FILE_PATH] = value;
    }

    void SetLogFilesDirectoryPath(const std::string& value)
    {
        m_jsonResponseValue[LOG_FILES_DIRECTORY] = value;
    }

    void SetTargetServer(const std::string& value)
    {
        m_jsonResponseValue[TARGET_SERVER] = value;
    }

    void SetUserName(const std::string& value)
    {
        m_jsonResponseValue[USER_NAME] = value;
    }

    void SetRemotePath(const std::string& value)
    {
        m_jsonResponseValue[REMOTE_PATH] = value;
    }
};

class LogCollectorGetConfigHandler : public JsonOpCodeHandlerBase<LogCollectorGetConfigRequest, LogCollectorGetConfigResponse>
{
private:
    void HandleRequest(const LogCollectorGetConfigRequest& jsonRequest, LogCollectorGetConfigResponse& jsonResponse) override;
};
