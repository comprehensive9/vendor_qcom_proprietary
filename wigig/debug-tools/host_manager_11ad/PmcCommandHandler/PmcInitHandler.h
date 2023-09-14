/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "PmcJsonResponse.h"

class PmcInitResponse: public PmcJsonResponse
{
    public:
        PmcInitResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
                PmcJsonResponse("PmcInitResponse", jsonRequestValue, jsonResponseValue)
        {
        }
};

class PmcInitHandler: public JsonOpCodeHandlerBase<JsonDeviceRequest, PmcInitResponse>
{
    void HandleRequest(const JsonDeviceRequest& jsonRequest, PmcInitResponse& jsonResponse) override;
};