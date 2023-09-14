/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <string>

#include "Host.h"
#include "WriteHandler.h"
#include "DebugLogger.h"
#include "OperationStatus.h"
#include "DeviceManager.h"
#include "Device.h"

using namespace std;

void WriteHandler::HandleRequest(const WriteRequest& jsonRequest, WriteResponse& jsonResponse)
{
    LOG_VERBOSE << "Access commands, Write" << std::endl;

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const JsonValueBoxed<uint32_t> address = jsonRequest.GetWriteAddress();

    if (address.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(address.BuildStateReport());
        return;
    }

    const JsonValueBoxed<uint32_t> value = jsonRequest.GetValue();

    if (value.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(value.BuildStateReport());
        return;
    }

    // We know that the address is provided and valid.
    LOG_VERBOSE << "Writing the value " << value << " to: " << Address(address) << endl;
    os = spDevice->Write(address, value);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
    }
}

void WriteBlockHandler::HandleRequest(const WriteBlockRequest& jsonRequest, WriteBlockResponse& jsonResponse)
{
    LOG_VERBOSE << "Access commands, WriteBlock" << std::endl;

    shared_ptr<Device> spDevice;

    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const JsonValueBoxed<uint32_t> address = jsonRequest.GetWriteAddress();

    if (address.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(address.BuildStateReport());
        return;
    }

    vector<uint32_t> values;

    if (!Utils::ConvertHexStringToDwordVector(jsonRequest.GetValues(), m_array_delimiter, values))
    {
        const string errStr = "should provide values in Hex";
        LOG_WARNING << "Failed to write block, " << errStr << endl;
        jsonResponse.Fail(errStr);
        return;
    }
    // perform write block
    const OperationStatus status = spDevice->WriteBlock(address, values);
    if (!status)
    {
        LOG_ERROR << "Failed to write block: " << status.GetStatusMessage() << endl;
        jsonResponse.Fail(status.GetStatusMessage());
    }

}
