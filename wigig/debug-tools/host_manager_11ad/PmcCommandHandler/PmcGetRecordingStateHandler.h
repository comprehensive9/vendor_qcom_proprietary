/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "PmcJsonResponse.h"

class PmcGetRecordingStateResponse: public PmcJsonResponse
{
    public:
        PmcGetRecordingStateResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
            PmcJsonResponse("PmcGetRecordingStateResponse", jsonRequestValue, jsonResponseValue)
        {
            SetPmcGeneral0(0);
            SetPmcGeneral1(0);
            SetHwIsActive(false);
        }

        void SetPmcGeneral0(uint32_t value)
        {
            m_jsonResponseValue["PMC_GENERAL_0"] = value;
        }

        void SetPmcGeneral1(uint32_t value)
        {
            m_jsonResponseValue["PMC_GENERAL_1"] = value;
        }

        void SetHwIsActive(bool hwActive)
        {
            m_jsonResponseValue["HwActive"] = hwActive;
        }
};

class PmcGetRecordingStateHandler: public JsonOpCodeHandlerBase<JsonDeviceRequest, PmcGetRecordingStateResponse>
{
        void HandleRequest(const JsonDeviceRequest& jsonRequest, PmcGetRecordingStateResponse& jsonResponse) override;
};