/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"

class HostGetStateResponse: public JsonResponse
{
    public:
        HostGetStateResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
            JsonResponse("HostGetStateResponse", jsonRequestValue, jsonResponseValue)
        {
            m_jsonResponseValue["Interfaces"] = Json::Value(Json::arrayValue);
            m_jsonResponseValue["Clients"] = Json::Value(Json::arrayValue);
        }

        void AddInterfaces();
        void AddClients();
        void SetEnumerationMode(const bool enumerationMode);
};

class HostGetStateHandler: public JsonOpCodeHandlerBase<Json::Value, HostGetStateResponse>
{
    private:
        void HandleRequest(const Json::Value& jsonRequest, HostGetStateResponse& jsonResponse) override;
};

// *************************************************************************************************

class HostGetInfoResponse : public JsonResponse
{
public:
    HostGetInfoResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("HostGetInfoResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetInfo(const std::string& operationSystem, const std::string& platform, const uint32_t capabilities, const std::string& alias);
};

class HostGetInfoHandler : public JsonOpCodeHandlerBase<Json::Value, HostGetInfoResponse>
{
private:
    void HandleRequest(const Json::Value& jsonRequest, HostGetInfoResponse& jsonResponse) override;
};

// *************************************************************************************************

class SetEnumerationModeRequest : public JsonRequest
{
public:
    explicit SetEnumerationModeRequest(const Json::Value& jsonRequestValue) : JsonRequest(jsonRequestValue) {}

    JsonValueBoxed<bool> GetMode() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "Mode");
    }
};

class SetEnumerationModeResponse : public JsonResponse
{
public:
    SetEnumerationModeResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("GenericResponse", jsonRequestValue, jsonResponseValue)
    {
    }
};

class SetEnumerationModeHandler : public JsonOpCodeHandlerBase<SetEnumerationModeRequest, SetEnumerationModeResponse>
{
private:
    void HandleRequest(const SetEnumerationModeRequest& jsonRequest, SetEnumerationModeResponse& jsonResponse) override;
};

// *************************************************************************************************

class HostGetInterfacesResponse : public JsonResponse
{
public:
    HostGetInterfacesResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("HostGetInterfacesResponse", jsonRequestValue, jsonResponseValue)
    {
        m_jsonResponseValue["Interfaces"] = Json::Value(Json::arrayValue);
    }

    void AddInterface(const std::string& interfaceName) const;
};

class HostGetInterfacesHandler : public JsonOpCodeHandlerBase<Json::Value, HostGetInterfacesResponse>
{
private:
    void HandleRequest(const Json::Value& jsonRequest, HostGetInterfacesResponse& jsonResponse) override;
};
