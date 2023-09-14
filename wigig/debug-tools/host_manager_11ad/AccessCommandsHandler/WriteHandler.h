/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#pragma once

#ifndef _WRITE_HANDLER_H_
#define _WRITE_HANDLER_H_

#include "JsonHandlerSDK.h"

class WriteRequest : public JsonDeviceRequest
{
public:
    WriteRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<uint32_t> GetWriteAddress() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "Address");
    }

    JsonValueBoxed<uint32_t> GetValue() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "Value");
    }

};


class WriteResponse : public JsonResponse
{
public:
    WriteResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("WriteResponse", jsonRequestValue, jsonResponseValue)
    {
    }

};

class WriteHandler : public JsonOpCodeHandlerBase<WriteRequest, WriteResponse>
{
private:
    void HandleRequest(const WriteRequest& jsonRequest, WriteResponse& jsonResponse) override;
};

// *************************************************************************************************
//Note: No need if encoded write block, not needed for perfomance

class WriteBlockRequest : public JsonDeviceRequest
{
public:
    WriteBlockRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<uint32_t> GetWriteAddress() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "Address");
    }

    JsonValueBoxed<std::string> GetValues() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, "Values");
    }

};

class WriteBlockResponse : public JsonResponse
{
public:
    WriteBlockResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("WriteBlockResponse", jsonRequestValue, jsonResponseValue)
    {
    }

};

class WriteBlockHandler : public JsonOpCodeHandlerBase<WriteBlockRequest, WriteBlockResponse>
{
    const char m_array_delimiter = ',';
    void HandleRequest(const WriteBlockRequest& jsonRequest, WriteBlockResponse& jsonResponse) override;
};

#endif  // _WRITE_HANDLER_H_
