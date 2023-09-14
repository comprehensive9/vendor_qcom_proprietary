/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "PmcJsonResponse.h"

class PmcStartResponse: public PmcJsonResponse
{
    public:
        PmcStartResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
                PmcJsonResponse("PmcStartResponse", jsonRequestValue, jsonResponseValue)
        {
        }

        void SetImplicitInit(bool implicitInit)
        {
            m_jsonResponseValue["ImplicitInit"] = implicitInit;
        }
};

class PmcStartHandler: public JsonOpCodeHandlerBase<JsonDeviceRequest, PmcStartResponse>
{
    void HandleRequest(const JsonDeviceRequest& jsonRequest, PmcStartResponse& jsonResponse) override;
};