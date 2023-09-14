/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "OperationStatus.h"

#include <string>
#include <vector>

// *************************************************************************************************

class LegacyPmcSetAspmRequest : public JsonDeviceRequest
{
public:
    LegacyPmcSetAspmRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<bool> Enable() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "Enable");
    }
};

// No LegacyPmcSetAspmResponse required

class LegacyPmcSetAspmHandler : public JsonOpCodeHandlerBase<LegacyPmcSetAspmRequest, JsonResponse>
{
    void HandleRequest(const LegacyPmcSetAspmRequest& jsonRequest, JsonResponse& jsonResponse) override;
};

// *************************************************************************************************

// No LegacyPmcGetAspmRequest required

class LegacyPmcGetAspmResponse : public JsonResponse
{
public:
    LegacyPmcGetAspmResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LegacyPmcGetAspmResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetEnabled(bool isEnabled)
    {
        m_jsonResponseValue["Enabled"] = isEnabled;
    }
};

class LegacyPmcGetAspmHandler : public JsonOpCodeHandlerBase<JsonDeviceRequest, LegacyPmcGetAspmResponse>
{
    void HandleRequest(const JsonDeviceRequest& jsonRequest, LegacyPmcGetAspmResponse& jsonResponse) override;
};

// *************************************************************************************************

class LegacyPmcRingAllocRequest : public JsonDeviceRequest
{
public:
    LegacyPmcRingAllocRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<uint32_t> GetDescriptorSize() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "DescriptorSize");
    }

    JsonValueBoxed<uint32_t> GetDescriptorNumber() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "DescriptorNumber");
    }
};

class LegacyPmcRingAllocHandler : public JsonOpCodeHandlerBase<LegacyPmcRingAllocRequest, JsonResponse>
{
    void HandleRequest(const LegacyPmcRingAllocRequest& jsonRequest, JsonResponse& jsonResponse) override;
};

// *************************************************************************************************

class LegacyPmcRingFreeRequest : public JsonDeviceRequest
{
public:
    LegacyPmcRingFreeRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<bool> GetSuppressError() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "SuppressError");
    }
};

class LegacyPmcRingFreeHandler : public JsonOpCodeHandlerBase<LegacyPmcRingFreeRequest, JsonResponse>
{
    void HandleRequest(const LegacyPmcRingFreeRequest& jsonRequest, JsonResponse& jsonResponse) override;
};

// *************************************************************************************************

class LegacyPmcGetDataRequest : public JsonDeviceRequest
{
public:
    LegacyPmcGetDataRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<uint32_t> GetRequestId() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "RequestId");
    }
};

class LegacyPmcGetDataResponse : public JsonResponse
{
public:
    LegacyPmcGetDataResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LegacyPmcGetDataResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetPmcData(const std::vector<unsigned char>& pmcData)
    {
        m_jsonResponseValue["DataBase64"] = Utils::Base64Encode(pmcData);
    }

    void SetPmcRingMetadataData(const std::vector<unsigned char>& pmcRingMetadata)
    {
        m_jsonResponseValue["RingMetadataBase64"] = Utils::Base64Encode(pmcRingMetadata);
    }
};

class LegacyPmcGetDataHandler : public JsonOpCodeHandlerBase<LegacyPmcGetDataRequest, LegacyPmcGetDataResponse>
{
    void HandleRequest(const LegacyPmcGetDataRequest& jsonRequest, LegacyPmcGetDataResponse& jsonResponse) override;
    static OperationStatus ReadFile(const std::string& filePath, std::vector<unsigned char>& fileContent);
};