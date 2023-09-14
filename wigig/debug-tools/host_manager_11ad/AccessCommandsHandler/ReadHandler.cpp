/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <string>

#include "Host.h"
#include "ReadHandler.h"
#include "DebugLogger.h"
#include "DeviceManager.h"
#include "Device.h"

using namespace std;

void ReadHandler::HandleRequest(const ReadRequest& jsonRequest, ReadResponse& jsonResponse)
{
    LOG_VERBOSE << "Access commands, read" << std::endl;

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const JsonValueBoxed<uint32_t> address = jsonRequest.GetReadAddress();

    if (address.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(address.BuildStateReport());
        return;
    }

    // We know that the address is provided and valid.
    LOG_VERBOSE << "Reading from: " << Address(address) << endl;
    uint32_t val = 0;
    os = spDevice->Read(address, val);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    jsonResponse.SetValue(val);
}

void ReadResponse::SetValue(uint32_t value)
{
    m_jsonResponseValue["Value"] = value;
}

// *************************************************************************************************

void ReadBlockHandler::HandleRequest(const ReadBlockRequest& jsonRequest, ReadBlockResponse& jsonResponse)
{
    LOG_VERBOSE << "Access commands, ReadBlock" << std::endl;

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const JsonValueBoxed<uint32_t> address = jsonRequest.GetReadAddress();

    if (address.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(address.BuildStateReport());
        return;
    }

    const JsonValueBoxed<uint32_t> blockSize = jsonRequest.GetBlockSize();

    if (blockSize.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(blockSize.BuildStateReport());
        return;
    }

    // We know that the address is provided and valid.
    LOG_VERBOSE << "Reading from: " << Address(address) << endl;
    std::vector<uint32_t> values;
    os = spDevice->ReadBlock(address, blockSize, values);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.AddValuesToJson(values, jsonRequest.EncodedReply());


}


void ReadBlockResponse::AddValuesToJson(const std::vector<uint32_t>& valuesVec, bool encodedReply)
{
    if (encodedReply)
    {
        const unsigned char* buffer = reinterpret_cast<const unsigned char*>(valuesVec.data());
        const size_t blockSizeInBytes = valuesVec.size() * sizeof(uint32_t);
        std::string response = Utils::Base64Encode(buffer, blockSizeInBytes);
        m_jsonResponseValue["ValuesBase64"] = response;
    }
    else
    {
        Json::Value values(Json::arrayValue);
        for (const auto& value : valuesVec)
        {
            values.append(static_cast<uint32_t>(value));
        }
        m_jsonResponseValue["Values"] = values;
    }

}
