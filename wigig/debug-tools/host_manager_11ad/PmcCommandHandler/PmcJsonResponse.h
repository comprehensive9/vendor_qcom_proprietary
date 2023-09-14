/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
*
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "PmcRecordingState.h"

class PmcJsonResponse : public JsonResponse
{
public:
    PmcJsonResponse(const char* szTypeName, const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue):
        JsonResponse(szTypeName, jsonRequestValue, jsonResponseValue)
    {
    }

    void SetCurrentState(PMC_RECORDING_STATE state)
    {
        m_jsonResponseValue["CurrentState"] = ToString(state);
    }

};