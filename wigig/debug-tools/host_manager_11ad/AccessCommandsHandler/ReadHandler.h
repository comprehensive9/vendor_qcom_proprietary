/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef _READ_HANDLER_H_
#define _READ_HANDLER_H_

#include "JsonHandlerSDK.h"

class ReadRequest : public JsonDeviceRequest
{
public:
    ReadRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<uint32_t> GetReadAddress() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "Address");
    }

};


class ReadResponse: public JsonResponse
{
    public:
        ReadResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
            JsonResponse("ReadResponse", jsonRequestValue, jsonResponseValue)
        {
        }

        void SetValue(uint32_t value);

};

class ReadHandler: public JsonOpCodeHandlerBase<ReadRequest, ReadResponse>
{
    private:
        void HandleRequest(const ReadRequest& jsonRequest, ReadResponse& jsonResponse) override;
};

// *************************************************************************************************

class ReadBlockRequest : public JsonDeviceRequest
{
public:
    ReadBlockRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<uint32_t> GetReadAddress() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "Address");
    }
    JsonValueBoxed<uint32_t> GetBlockSize() const
    {
        return JsonValueParser::ParseUnsignedValue(m_jsonRequestValue, "Size");
    }

    JsonValueBoxed<bool> EncodedReply() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "EncodedReply");
    }

};

class ReadBlockResponse : public JsonResponse
{
public:
    ReadBlockResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("ReadBlockResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    //encodedReply is needed for better perfomance, true - returns base64 encoded string, false - returns List<uint>
    void AddValuesToJson(const std::vector<uint32_t>& valuesVec, bool encodedReply);
};

class ReadBlockHandler : public JsonOpCodeHandlerBase<ReadBlockRequest, ReadBlockResponse>
{
private:
    void HandleRequest(const ReadBlockRequest& jsonRequest, ReadBlockResponse& jsonResponse) override;
};

#endif  // _READ_HANDLER_H_
