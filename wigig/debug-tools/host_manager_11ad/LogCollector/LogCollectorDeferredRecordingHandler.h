/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "LogCollectorJsonValueParser.h"
#include "LogCollectorDefinitions.h"
#include "LogCollectorService.h"
#include "DeferredRecordingContext.h"

// *************************************************************************************************

class LogCollectorStartDeferredRecordingRequest final : public JsonRequest
{
public:
    explicit LogCollectorStartDeferredRecordingRequest(const Json::Value& jsonRequestValue)
        : JsonRequest(jsonRequestValue)
    {}

    JsonValueBoxed<RecordingType> GetRecordingTypeBoxed() const
    {
        return LogCollectorJsonValueParser::ParseRecordingType(m_jsonRequestValue, "RecordingType");
    }

    JsonValueBoxed<bool> GetCompressBoxed() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "Compress");
    }

    JsonValueBoxed<bool> GetUploadBoxed() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "Upload");
    }

    JsonValueBoxed<bool> GetDebugModeBoxed() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "Debug");
    }

    OperationStatus GetModuleVerbosity(std::vector<LogCollectorModuleVerbosity>& moduleVerbosity) const
    {
        return LogCollectorJsonValueParser::ParseAllModuleVerbosity(m_jsonRequestValue, moduleVerbosity);
    }
};

class LogCollectorStartDeferredRecordingResponse final : public JsonResponse
{
public:
    LogCollectorStartDeferredRecordingResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorStartDeferredRecordingResponse", jsonRequestValue, jsonResponseValue)
    {
    }
};

// *************************************************************************************************

class LogCollectorStartDeferredRecordingHandler final
    : public JsonOpCodeHandlerBase<LogCollectorStartDeferredRecordingRequest, LogCollectorStartDeferredRecordingResponse>
{
private:
    void HandleRequest(
        const LogCollectorStartDeferredRecordingRequest& jsonRequest,
        LogCollectorStartDeferredRecordingResponse& jsonResponse) override;
};

// *************************************************************************************************

class LogCollectorStopDeferredRecordingResponse final : public JsonResponse
{
public:
    LogCollectorStopDeferredRecordingResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorStopDeferredRecordingResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetMessage(const std::string& message)
    {
        m_jsonResponseValue["Message"] = message;
    }
};

// *************************************************************************************************

class LogCollectorStopDeferredRecordingHandler final
    : public JsonOpCodeHandlerBase<Json::Value, LogCollectorStopDeferredRecordingResponse>
{
private:
    void HandleRequest(
        const Json::Value& jsonRequest, LogCollectorStopDeferredRecordingResponse& jsonResponse) override;
};

// *************************************************************************************************

class LogCollectorGetDeferredRecordingResponse final : public JsonResponse
{
public:
    LogCollectorGetDeferredRecordingResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue)
        : JsonResponse("LogCollectorGetDeferredRecordingResponse", jsonRequestValue, jsonResponseValue)
    {
        m_jsonResponseValue["Enabled"] = false;
    }

    void Set(std::shared_ptr<DeferredRecordingContext> spDeferredRecordingContext)
    {
        if (!spDeferredRecordingContext) return;    // Enabled property is set to false by default

        m_jsonResponseValue["Enabled"] = true;
        m_jsonResponseValue["RecordingType"] = LoggingTypeToString(spDeferredRecordingContext->GetLoggingType());
        m_jsonResponseValue["Compress"] = spDeferredRecordingContext->IsCompress();
        m_jsonResponseValue["Upload"] = spDeferredRecordingContext->IsUpload();

        for (const auto& v : spDeferredRecordingContext->GetModuleVerbosity())
        {
            m_jsonResponseValue[LogCollectorJsonValueParser::GetModuleName(v.GetModuleIndex())] = v.ToString();
        }
    }
};

// *************************************************************************************************

class LogCollectorGetDeferredRecordingHandler final
    : public JsonOpCodeHandlerBase<Json::Value, LogCollectorGetDeferredRecordingResponse>
{
private:
    void HandleRequest(
        const Json::Value& jsonRequest, LogCollectorGetDeferredRecordingResponse& jsonResponse) override;
};