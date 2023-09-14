/*
* Copyright (c) 2017 - 2020 Qualcomm Technologies, Inc.
*
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "PmcJsonResponse.h"

class PmcShutdownResponse : public PmcJsonResponse
{
public:
    PmcShutdownResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        PmcJsonResponse("PmcShutdownResponse", jsonRequestValue, jsonResponseValue)
    {
    }
};

class PmcShutdownHandler : public JsonOpCodeHandlerBase<JsonDeviceRequest, PmcShutdownResponse>
{
    void HandleRequest(const JsonDeviceRequest& jsonRequest, PmcShutdownResponse& jsonResponse) override;
};