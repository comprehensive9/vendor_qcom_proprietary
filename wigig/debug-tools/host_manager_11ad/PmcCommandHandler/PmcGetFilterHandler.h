/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "PmcJsonResponse.h"

class PmcGetFilterResponse: public PmcJsonResponse
{
    public:
        PmcGetFilterResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue):
            PmcJsonResponse("PmcGetFilterResponse", jsonRequestValue, jsonResponseValue)
        {
            // Make sure default values are always set
            SetCollectIdleSmEvents(false);
            SetCollectRxPpduEvents(false);
            SetCollectTxPpduEvents(false);
            SetCollectUCodeEvents(false);
        }

        void SetCollectIdleSmEvents(bool value)
        {
            m_jsonResponseValue["CollectIdleSmEvents"] = value;
        }
        void SetCollectRxPpduEvents(bool value)
        {
            m_jsonResponseValue["CollectRxPpduEvents"] = value;
        }
        void SetCollectTxPpduEvents(bool value)
        {
            m_jsonResponseValue["CollectTxPpduEvents"] = value;
        }
        void SetCollectUCodeEvents(bool value)
        {
            m_jsonResponseValue["CollectUCodeEvents"] = value;
        }
};

class PmcGetFilterHandler: public JsonOpCodeHandlerBase<JsonDeviceRequest, PmcGetFilterResponse>
{
        void HandleRequest(const JsonDeviceRequest& jsonRequest, PmcGetFilterResponse& jsonResponse) override;
};