/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include <vector>

struct StaInfo;

class StaInfoRequest : public JsonDeviceRequest
{
public:
    explicit StaInfoRequest(const Json::Value& jsonRequestValue): JsonDeviceRequest(jsonRequestValue) {}
};

class StaInfoResponse : public JsonResponse
{
public:
    StaInfoResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("StaInfoResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetStaInfo(const std::vector<StaInfo>& ctaInfo);

};

class StaInfoHandler : public JsonOpCodeHandlerBase<StaInfoRequest, StaInfoResponse>
{
private:
    void HandleRequest(const StaInfoRequest& jsonRequest, StaInfoResponse& jsonResponse) override;
};

// *************************************************************************************************

class DriverCommandRequest : public JsonDeviceRequest
{
public:
    explicit DriverCommandRequest(const Json::Value& jsonRequestValue) : JsonDeviceRequest(jsonRequestValue) {}

    JsonValueBoxed<uint32_t> GetCmdId() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "CmdId");
    }

    JsonValueBoxed<uint32_t> GetBufSize() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "BufSize");
    }

    JsonValueBoxed<std::string> GetBinaryData() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, "DataBase64");
    }
};

class DriverCommandResponse : public JsonResponse
{
public:
    DriverCommandResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("DriverCommandResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetPayload(std::string payload);
};

class DriverCommandHandler : public JsonOpCodeHandlerBase<DriverCommandRequest, DriverCommandResponse>
{
private:
    void HandleRequest(const DriverCommandRequest& jsonRequest, DriverCommandResponse& jsonResponse) override;
};

// *************************************************************************************************

class SetDebugModeRequest : public JsonDeviceRequest
{
public:
    explicit SetDebugModeRequest(const Json::Value& jsonRequestValue) : JsonDeviceRequest(jsonRequestValue) {}

    JsonValueBoxed<bool> GetDebugMode() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "DebugMode");
    }
};

class SetDebugModeResponse : public JsonResponse
{
public:
    SetDebugModeResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("SetDebugModeResponse", jsonRequestValue, jsonResponseValue)
    {
    }

};

class SetDebugModeHandler : public JsonOpCodeHandlerBase<SetDebugModeRequest, SetDebugModeResponse>
{
private:
    void HandleRequest(const SetDebugModeRequest& jsonRequest, SetDebugModeResponse& jsonResponse) override;
};

// *************************************************************************************************

class InterfaceResetResponse : public JsonResponse
{
public:
    InterfaceResetResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("InterfaceResetResponse", jsonRequestValue, jsonResponseValue)
    {
    }

};

class InterfaceResetHandler : public JsonOpCodeHandlerBase<JsonDeviceRequest, InterfaceResetResponse>
{
private:
    void HandleRequest(const JsonDeviceRequest& jsonRequest, InterfaceResetResponse& jsonResponse) override;
};

// *************************************************************************************************

class GetDebugModeResponse : public JsonResponse
{
public:
    GetDebugModeResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("GetDebugModeResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetResponse(bool debugMode);
};

class GetDebugModeHandler : public JsonOpCodeHandlerBase<JsonDeviceRequest, GetDebugModeResponse>
{
private:
    void HandleRequest(const JsonDeviceRequest& jsonRequest, GetDebugModeResponse& jsonResponse) override;
};

// *************************************************************************************************

class GetDeviceCapabilitiesResponse : public JsonResponse
{
public:
    GetDeviceCapabilitiesResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("GetDeviceCapabilitiesResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetDeviceCapabilitiesMask(uint32_t deviceCapabilities);
    void SetDeviceCapabilities(Json::Value driverCapaNames);
};

class GetDeviceCapabilitiesHandler : public JsonOpCodeHandlerBase<JsonDeviceRequest, GetDeviceCapabilitiesResponse>
{
private:
    void HandleRequest(const JsonDeviceRequest& jsonRequest, GetDeviceCapabilitiesResponse& jsonResponse) override;
};

// *************************************************************************************************

class GetFwHealthStateResponse : public JsonResponse
{
public:
    GetFwHealthStateResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("GetFwHealthStateResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetFwHealthState(int fwHealthState);
    void SetFwHealthStateStr(const std::string fwHealthStateStr);
};

class GetFwHealthStateHandler : public JsonOpCodeHandlerBase<JsonDeviceRequest, GetFwHealthStateResponse>
{
private:
    void HandleRequest(const JsonDeviceRequest& jsonRequest, GetFwHealthStateResponse& jsonResponse) override;
};



