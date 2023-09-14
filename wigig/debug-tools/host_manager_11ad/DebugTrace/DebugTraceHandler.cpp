/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "DebugTraceHandler.h"
#include "DebugTraceTarget.h"

using namespace std;

// *************************************************************************************************

void TraceGetSeverityHandler::HandleRequest(
    const TraceGetSeverityRequest& jsonRequest, TraceGetSeverityResponse& jsonResponse)
{
    (void)jsonRequest;

    const auto & modulesSeverity = DebugTraceService::GetInstance().GetAllModuleMaxSeverity();

    jsonResponse.SetSeverity(modulesSeverity);
}

JsonValueBoxed<LogSeverity>
TraceSetSeverityRequest::ParseSeverityType(const Json::Value& jsonValue, const char* szKey)
{
    const Json::Value& value = jsonValue[szKey];
    if (Json::Value::nullSingleton() == value)
    {
        return { szKey, JsonValueState::JSON_VALUE_MISSING, "Value does not exist"};
    }

    const std::string valueStr = value.asString();

    if (valueStr == SeverityToString(LOG_SEV_VERBOSE))
    {
        return JsonValueBoxed<LogSeverity>(szKey, LOG_SEV_VERBOSE);
    }
    if (valueStr == SeverityToString(LOG_SEV_DEBUG))
    {
        return JsonValueBoxed<LogSeverity>(szKey, LOG_SEV_DEBUG);
    }
    if (valueStr == SeverityToString(LOG_SEV_INFO))
    {
        return JsonValueBoxed<LogSeverity>(szKey, LOG_SEV_INFO);
    }
    if (valueStr == SeverityToString(LOG_SEV_WARNING))
    {
        return JsonValueBoxed<LogSeverity>(szKey, LOG_SEV_WARNING);
    }
    if (valueStr == SeverityToString(LOG_SEV_ERROR))
    {
        return JsonValueBoxed<LogSeverity>(szKey, LOG_SEV_ERROR);
    }
    return { szKey, JsonValueState::JSON_VALUE_MALFORMED, "VRB|DBG|INF|WRN|ERR" };
}

JsonValueBoxed<ModuleId>
TraceSetSeverityRequest::ParseModuleType(const Json::Value& jsonValue, const char* szKey)
{
    const Json::Value& value = jsonValue[szKey];
    if (Json::Value::nullSingleton() == value)
    {
        return { szKey, JsonValueState::JSON_VALUE_MISSING, "Value does not exist" };
    }

    const std::string valueStr = value.asString();

    if (valueStr == ModuleToString(DFLT))
    {
        return JsonValueBoxed<ModuleId>(szKey, DFLT);
    }
    if (valueStr == ModuleToString(RDWR))
    {
        return JsonValueBoxed<ModuleId>(szKey, RDWR);
    }

    return { szKey, JsonValueState::JSON_VALUE_MALFORMED, "DEFAULT|RDWR" };
}

// *************************************************************************************************
void TraceSetSeverityHandler::HandleRequest(
    const TraceSetSeverityRequest& jsonRequest, TraceSetSeverityResponse& jsonResponse)
{
    const JsonValueBoxed<LogSeverity> severity = jsonRequest.GetSeverity();

    if (severity.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        jsonResponse.Fail(severity.BuildStateReport());
        return;
    }

    const JsonValueBoxed<ModuleId> module = jsonRequest.GetModule();

    if (module.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        jsonResponse.Fail(severity.BuildStateReport());
        return;
    }

    // In case module is missing, need to set the given severity to all modules
    if (module.GetState() == JsonValueState::JSON_VALUE_MISSING)
    {
        DebugTraceService::GetInstance().SetModuleMaxSeverity(severity);
        return;
    }

    DebugTraceService::GetInstance().SetModuleMaxSeverity(module, severity);
}

// *************************************************************************************************

void TraceSplitHandler::HandleRequest(
    const Json::Value& jsonRequest, TraceSplitResponse& jsonResponse)
{
    (void)jsonRequest;
    (void)jsonResponse;
    DebugTraceService::GetInstance().Split();
}

// *************************************************************************************************

void TraceSetPathHandler::HandleRequest(
    const TraceSetPathRequest& jsonRequest, JsonResponse& jsonResponse)
{
    const JsonValueBoxed<std::string> path = jsonRequest.GetPath();

    OperationStatus os = DebugTraceService::GetInstance().SetTracePath(path.GetValue());
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
}

// *************************************************************************************************

void TraceGetPathHandler::HandleRequest(
    const JsonRequest& jsonRequest, TraceGetPathResponse& jsonResponse)
{
    (void)jsonRequest;
    std::string path = DebugTraceService::GetInstance().GetTracePath();
    jsonResponse.GetPath(path);
}