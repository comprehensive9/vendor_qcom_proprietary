/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "LogCollectorJsonValueParser.h"

// *************************************************************************************************

class LogCollectorGetVerbosityRequest final : public JsonDeviceRequest
{
public:
    explicit LogCollectorGetVerbosityRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<CpuType> GetCpuType() const
    {
        return LogCollectorJsonValueParser::ParseCpuType(m_jsonRequestValue, "CpuType");
    }
};

// *************************************************************************************************

class LogCollectorGetVerbosityResponse final : public JsonResponse
{
public:
    LogCollectorGetVerbosityResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorGetVerbosityResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetModuleVerbosity(std::vector<LogCollectorModuleVerbosity>& moduleVerbosity, bool isEnforced)
    {
        m_jsonResponseValue["Enforced"] = isEnforced;

        for (const auto& v : moduleVerbosity)
        {
            m_jsonResponseValue[LogCollectorJsonValueParser::GetModuleName(v.GetModuleIndex())] = v.ToString();
        }
    }
};

class LogCollectorGetVerbosityHandler : public JsonOpCodeHandlerBase<LogCollectorGetVerbosityRequest, LogCollectorGetVerbosityResponse>
{
    void HandleRequest(const LogCollectorGetVerbosityRequest& jsonRequest, LogCollectorGetVerbosityResponse& jsonResponse) override;
};

// *************************************************************************************************

class LogCollectorGetEnforcedVerbosityRequest final : public JsonDeviceRequest
{
public:
    explicit LogCollectorGetEnforcedVerbosityRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<CpuType> GetCpuType() const
    {
        return LogCollectorJsonValueParser::ParseCpuType(m_jsonRequestValue, "CpuType");
    }
};

// *************************************************************************************************

class LogCollectorGetEnforcedVerbosityResponse final : public JsonResponse
{
public:
    LogCollectorGetEnforcedVerbosityResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorGetEnforcedVerbosityResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetEnforcedModuleVerbosity(std::vector<LogCollectorModuleVerbosity>& moduleVerbosity)
    {
        for (const auto& v : moduleVerbosity)
        {
            m_jsonResponseValue[LogCollectorJsonValueParser::GetModuleName(v.GetModuleIndex())] = v.ToString();
        }
    }
};

class LogCollectorGetEnforcedVerbosityHandler :
    public JsonOpCodeHandlerBase<LogCollectorGetEnforcedVerbosityRequest, LogCollectorGetEnforcedVerbosityResponse>
{
    void HandleRequest(
        const LogCollectorGetEnforcedVerbosityRequest& jsonRequest,
        LogCollectorGetEnforcedVerbosityResponse& jsonResponse) override;
};

// *************************************************************************************************

class LogCollectorSetVerbosityRequest final : public JsonDeviceRequest
{
public:
    explicit LogCollectorSetVerbosityRequest(const Json::Value& jsonRequestValue)
        : JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<CpuType> GetCpuType() const
    {
        return LogCollectorJsonValueParser::ParseCpuType(m_jsonRequestValue, "CpuType");
    }

    OperationStatus GetModuleVerbosity(std::vector<LogCollectorModuleVerbosity>& moduleVerbosity) const
    {
        return LogCollectorJsonValueParser::ParseAllModuleVerbosity(m_jsonRequestValue, moduleVerbosity);
    }
};

class LogCollectorSetVerbosityResponse final : public JsonResponse
{
public:
    LogCollectorSetVerbosityResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorSetVerbosityResponse", jsonRequestValue, jsonResponseValue)
    {
    }
};

class LogCollectorSetVerbosityHandler final
    : public JsonOpCodeHandlerBase<LogCollectorSetVerbosityRequest, LogCollectorSetVerbosityResponse>
{
    void HandleRequest(const LogCollectorSetVerbosityRequest& jsonRequest, LogCollectorSetVerbosityResponse& jsonResponse) override;
};
