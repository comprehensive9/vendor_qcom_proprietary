/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "PmcJsonResponse.h"

class PmcStopResponse: public PmcJsonResponse
{
    public:
        PmcStopResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
            PmcJsonResponse("PmcStopResponse", jsonRequestValue, jsonResponseValue)
        {
        }

        void SetRecordingDirectory(const char* szRecDirectory)
        {
            m_jsonResponseValue["RecordingDirectory"] = szRecDirectory;
        }

        void SetPmcData(const std::vector<uint8_t>& ringData, uint32_t numberOfDescriptors, uint32_t ringPayloadSizeBytes);
};

class PmcStopHandler: public JsonOpCodeHandlerBase<JsonDeviceRequest, PmcStopResponse>
{
    void HandleRequest(const JsonDeviceRequest& jsonRequest, PmcStopResponse& jsonResponse) override;
};