/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once
#include "JsonHandlerSDK.h"
#include "OperationStatus.h"

class WmiRequest : public JsonDeviceRequest
{
public:
    explicit WmiRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<uint32_t> GetCommandId() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "Id");
    }

    JsonValueBoxed<uint32_t> GetEventId() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "EventId");
    }

    JsonValueBoxed<uint32_t> GetTimeOut() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "Timeout");
    }

    JsonValueBoxed<std::string> GetParameters() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, "Params");
    }

};

class WmiUTRequest : public JsonDeviceRequest
{
public:
    explicit WmiUTRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<uint32_t> GetModuleId() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "ModuleId");
    }

    JsonValueBoxed<uint32_t> GetSubtypeId() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "SubtypeId");
    }

    JsonValueBoxed<uint32_t> GetTimeOut() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "Timeout");
    }

    JsonValueBoxed<std::string> GetParameters() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, "Params");
    }
};

// Also used for WMI UT commands
class WmiResponse : public JsonResponse
{
public:
    WmiResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("WmiResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetEventParameters(const std::string& eventParameters) const;
};

class WmiHandler : public JsonOpCodeHandlerBase<WmiRequest, WmiResponse>
{
private:
    void HandleRequest(const WmiRequest& jsonRequest, WmiResponse& jsonResponse) override;
    static std::string WmiCommandToString(uint32_t commandId, std::vector<unsigned char>& binaryData);
};

class WmiUTHandler : public JsonOpCodeHandlerBase<WmiUTRequest, WmiResponse>
{
private:
    void HandleRequest(const WmiUTRequest& jsonRequest, WmiResponse& jsonResponse) override;
    static std::string WmiUTCommandToString(uint32_t moduleId, uint32_t subtypeId, std::vector<unsigned char>& binaryData);
};

class WmiConvertor
{
public:
    const static int  DefaultEventTimeoutSec = 3;
    const static uint32_t WmiUTCommandId = 0x900;
    const static uint32_t WmiUTEventId = 0x1900;

    static OperationStatus ConstructWmiBinaryData(uint32_t commandId, const std::string& parametersAsString, std::vector<unsigned char>& binaryData);
    static OperationStatus ConstructWmiUTBinaryData(uint32_t moduleId, uint32_t subtypeId, const std::string& parametersAsString, std::vector<unsigned char>& binaryData);
    static std::string ParseEvent(const std::vector<uint8_t>& eventBinaryData, bool isWmiUT);

private:
    static uint32_t SerializeModuleAndSubtypeIds(uint32_t moduleId, uint32_t subtypeId);
    static OperationStatus AddHexParametersToBinaryData(const std::vector<std::string>& parameters, std::vector<unsigned char>& binaryData, int offset);
    static void AddDwordToBinaryData(uint32_t dword, std::vector<unsigned char>& binaryData, int offset);
    static OperationStatus ConstructBinaryData(uint32_t commandId, bool isWmiUT, uint32_t moduleId, uint32_t subtypeId,
        const std::string& parametersAsString, std::vector<unsigned char>& binaryData);
};
