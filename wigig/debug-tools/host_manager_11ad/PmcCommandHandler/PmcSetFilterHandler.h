/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "PmcJsonResponse.h"

class PmcSetFilterRequest: public JsonDeviceRequest
{
    public:
        PmcSetFilterRequest(const Json::Value& jsonRequestValue) :
                JsonDeviceRequest(jsonRequestValue)
        {
        }

        JsonValueBoxed<bool> GetCollectIdleSmEvents() const
        {
            return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "CollectIdleSmEvents");
        }

        JsonValueBoxed<bool> GetCollectRxPpduEvents() const
        {
            return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "CollectRxPpduEvents");
        }

        JsonValueBoxed<bool> GetCollectTxPpduEvents() const
        {
            return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "CollectTxPpduEvents");
        }

        JsonValueBoxed<bool> GetCollectUCodeEvents() const
        {
            return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "CollectUCodeEvents");
        }
};

class PmcSetFilterResponse: public PmcJsonResponse
{
    public:
        PmcSetFilterResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
                PmcJsonResponse("PmcSetFilterResponse", jsonRequestValue, jsonResponseValue)
        {
        }
};

class PmcSetFilterHandler: public JsonOpCodeHandlerBase<PmcSetFilterRequest, PmcSetFilterResponse>
{
    void HandleRequest(const PmcSetFilterRequest& jsonRequest, PmcSetFilterResponse& jsonResponse) override;
};