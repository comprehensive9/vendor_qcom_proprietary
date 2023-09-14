/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "PmcJsonResponse.h"

class PmcPauseResponse: public PmcJsonResponse
{
    public:
        PmcPauseResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
            PmcJsonResponse("PmcPauseResponse", jsonRequestValue, jsonResponseValue)
        {
        }
};

class PmcPauseHandler: public JsonOpCodeHandlerBase<JsonDeviceRequest, PmcPauseResponse>
{
    private:
        void HandleRequest(const JsonDeviceRequest& jsonRequest, PmcPauseResponse& jsonResponse) override;
};