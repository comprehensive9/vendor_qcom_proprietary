/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"

// *************************************************************************************************

class TraceGetSeverityRequest final : public JsonRequest
{
public:
    explicit TraceGetSeverityRequest(const Json::Value& jsonRequestValue) :
        JsonRequest(jsonRequestValue)
    {
    }
};

class TraceGetSeverityResponse final : public JsonResponse
{
public:
    TraceGetSeverityResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("TraceGetSeverityResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetSeverity(const std::array<LogSeverity, static_cast<int>(DTM_MAX)>& modulesSeverity) const
    {
        for (size_t i = 0; i < modulesSeverity.size(); i++)
        {
            m_jsonResponseValue[ModuleToString(static_cast<ModuleId>(i))] = SeverityToString(modulesSeverity[i]);
        }
    }
};

class TraceGetSeverityHandler
    : public JsonOpCodeHandlerBase<TraceGetSeverityRequest, TraceGetSeverityResponse>
{
    void HandleRequest(
        const TraceGetSeverityRequest& jsonRequest, TraceGetSeverityResponse& jsonResponse) override;
};

// *************************************************************************************************

class TraceSetSeverityRequest final : public JsonRequest
{
public:
    explicit TraceSetSeverityRequest(const Json::Value& jsonRequestValue)
        : JsonRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<LogSeverity>GetSeverity() const
    {
        return ParseSeverityType(m_jsonRequestValue, "Severity");
    }

    JsonValueBoxed<ModuleId>GetModule() const
    {
        return ParseModuleType(m_jsonRequestValue, "Module");
    }

    static JsonValueBoxed<LogSeverity> ParseSeverityType(const Json::Value& jsonValue, const char* szKey);
    static JsonValueBoxed<ModuleId> ParseModuleType(const Json::Value& jsonValue, const char* szKey);
};

class TraceSetSeverityResponse final : public JsonResponse
{
public:
    TraceSetSeverityResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("TraceSetSeverityResponse", jsonRequestValue, jsonResponseValue)
    {
    }
};

class TraceSetSeverityHandler final
    : public JsonOpCodeHandlerBase<TraceSetSeverityRequest, TraceSetSeverityResponse>
{
    void HandleRequest(
        const TraceSetSeverityRequest& jsonRequest, TraceSetSeverityResponse& jsonResponse) override;
};

// *************************************************************************************************

class TraceSplitResponse : public JsonResponse
{
public:
    TraceSplitResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("TraceSplitResponse", jsonRequestValue, jsonResponseValue)
    {
    }
};

class TraceSplitHandler : public JsonOpCodeHandlerBase<Json::Value, TraceSplitResponse>
{
    void HandleRequest(const Json::Value& jsonRequest, TraceSplitResponse& jsonResponse) override;
};

// *************************************************************************************************

class TraceSetPathRequest final : public JsonRequest
{
public:
    explicit TraceSetPathRequest(const Json::Value& jsonRequestValue)
        : JsonRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<std::string> GetPath() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, "Path");
    }
};

class TraceSetPathHandler final
    : public JsonOpCodeHandlerBase<TraceSetPathRequest, JsonResponse>
{
    void HandleRequest(
        const TraceSetPathRequest& jsonRequest, JsonResponse& jsonResponse) override;
};

// *************************************************************************************************

class TraceGetPathResponse final : public JsonResponse
{
public:
    TraceGetPathResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("TraceGetPathResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void GetPath(std::string path) const
    {
        m_jsonResponseValue["Path"] = path;
    }
};

class TraceGetPathHandler
    : public JsonOpCodeHandlerBase<JsonRequest, TraceGetPathResponse>
{
    void HandleRequest(
        const JsonRequest& jsonRequest, TraceGetPathResponse& jsonResponse) override;
};