/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "FwStateProvider.h"
#include "JsonHandlerSDK.h"

class AtomicFwInfo;
class FwStateProvider;
enum class wil_fw_state;

class HostGetFwInfoRequest : public JsonDeviceRequest
{
public:
    HostGetFwInfoRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }
};

class HostGetFwInfoResponse : public JsonResponse
{
public:
    HostGetFwInfoResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("HostGetFwInfoResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetInfo(const AtomicFwInfo& fwState, std::vector<uint32_t>& fwCapa);
    const char* FwCapaToString(wmi_fw_capability capa);
};

class HostGetFwInfoHandler : public JsonOpCodeHandlerBase<HostGetFwInfoRequest, HostGetFwInfoResponse>
{
    void HandleRequest(const HostGetFwInfoRequest& jsonRequest, HostGetFwInfoResponse& jsonResponse) override;
};
