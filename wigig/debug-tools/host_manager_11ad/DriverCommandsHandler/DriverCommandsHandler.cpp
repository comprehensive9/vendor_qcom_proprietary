/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "DriverCommandsHandler.h"
#include "Device.h"
#include "DeviceContracts.h"
#include "DebugLogger.h"
#include "DeviceManager.h"
#include "EventsDefinitions.h"
#include "Host.h"
#include "FwStateProvider.h"
#include "DriverContracts.h"
#include <sstream>

using namespace std;

void StaInfoHandler::HandleRequest(const StaInfoRequest& jsonRequest, StaInfoResponse& jsonResponse)
{
    LOG_VERBOSE << "StaInfo handler" << std::endl;
    (void)jsonRequest;

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    vector<StaInfo> staInfo;
    os = spDevice->GetStaInfo(staInfo);
    if (!os)
    {
        LOG_ERROR << "Failed to send driver command: " << os.GetStatusMessage() << std::endl;
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetStaInfo(staInfo);
}

void StaInfoResponse::SetStaInfo(const vector<StaInfo>& allStaInfo)
{
    int i = 0;
    for(const auto& StaInfo: allStaInfo)
    {
        ostringstream oss;
        oss << StaInfo;
        m_jsonResponseValue["StaInfo"][i++] = oss.str();
    }
}

std::ostream& operator<<(std::ostream& os, const StaConnectionStatus& status)
{
    switch(status)
    {
        case StaConnectionStatus::PENDING:      return os << "pending";
        case StaConnectionStatus::CONNECTED:    return os << "connected";
        default:                                return os << "unused";

    }
}
ostream& operator<<(ostream& os, const StaInfo& staInfo)
{
    return os << '[' << staInfo.CidIndex << "] "
        << MacAddress(staInfo.MAC) << ' '
        << staInfo.Status
        << " MID " << staInfo.MID
        << " AID " << staInfo.AID;
}

// *************************************************************************************************

void DriverCommandResponse::SetPayload(string payload)
{
    m_jsonResponseValue["Payload"] = payload;
}


void DriverCommandHandler::HandleRequest(const DriverCommandRequest& jsonRequest, DriverCommandResponse& jsonResponse)
{
    shared_ptr<Device> spDevice;
    const OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const uint32_t commandId = jsonRequest.GetCmdId();
    const uint32_t outBufSize = jsonRequest.GetBufSize();
    std::vector<unsigned char> inputBuf;

    if (!(Utils::Base64Decode(jsonRequest.GetBinaryData(), inputBuf)))
    {
        jsonResponse.Fail("Invalid argument");
        return;
    }

    const uint32_t inBufSize = static_cast<uint32_t>(inputBuf.size());

    if (ShouldPrint(LOG_SEV_VERBOSE)) // spare all the formatting when not needed
    {
        ostringstream ss;
        ss << __FUNCTION__ << ": [command id: " << Hex<>(commandId);
        ss << " - " << (commandId == 0 ? "WMI" : commandId == 1 ? "Generic command" : "Unknown");
        ss << "], Payload (bytes):" << endl;
        for (const auto& byteVal : inputBuf)
        {
            ss << "\t" << Hex<>(byteVal) << endl;
        }
        LOG_VERBOSE << ss.str() << endl;
    }

    std::vector<unsigned char> outputBuf(outBufSize, 0);

    const OperationStatus status = spDevice->DriverControl(
        commandId, inputBuf.data(), inBufSize, outBufSize ? outputBuf.data() : nullptr, outBufSize);
    if (!status)
    {
        LOG_ERROR << "Failed to send driver command: " << status.GetStatusMessage() << endl;
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << ": Success" << endl;
        jsonResponse.SetPayload(Utils::Base64Encode(outputBuf)); // empty string if the buffer is empty
    }
}



void SetDebugModeHandler::HandleRequest(const SetDebugModeRequest& jsonRequest, SetDebugModeResponse& jsonResponse)
{
    shared_ptr<Device> spDevice;
    string deviceName = jsonRequest.GetDeviceName();
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const JsonValueBoxed<bool> debugModeBoxed = jsonRequest.GetDebugMode();
    switch (debugModeBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_PROVIDED:
    {
        const bool silentMode = !debugModeBoxed.GetValue();
        spDevice->SetSilenceMode(silentMode);
        Host::GetHost().PushEvent(SilenceStatusChangedEvent(deviceName, silentMode));
        return;
    }
    default:
        jsonResponse.Fail("Json value is missing.");
        return;
    }


}

void InterfaceResetHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, InterfaceResetResponse& jsonResponse)
{
    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }


    const OperationStatus status = spDevice->InterfaceReset();
    if (!status)
    {
        jsonResponse.Fail(status.GetStatusMessage());
        return;
    }
}


void GetDebugModeResponse::SetResponse(bool debugMode)
{
    m_jsonResponseValue["DebugMode"] = debugMode;
}


void GetDebugModeHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, GetDebugModeResponse& jsonResponse)
{
    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    //DebugMode == !SilenceMode
    bool debugMode = !spDevice->GetSilenceMode();
    jsonResponse.SetResponse(debugMode);
}



void GetDeviceCapabilitiesResponse::SetDeviceCapabilitiesMask(uint32_t deviceCapabilities)
{
    m_jsonResponseValue["DeviceCapabilities"] = deviceCapabilities;
}

void GetDeviceCapabilitiesResponse::SetDeviceCapabilities(Json::Value driverCapaNames)
{
    m_jsonResponseValue["DeviceCapabilitiesNames"] = driverCapaNames;
}


void GetDeviceCapabilitiesHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, GetDeviceCapabilitiesResponse& jsonResponse)
{
    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const uint32_t deviceCapabilitiesMask = spDevice->GetCapabilityMask();
    Json::Value driverCapaNames(Json::arrayValue);
    static const int bitsInDword = 32;

    for (int pos = 0; pos < bitsInDword; pos++)
    {
        if (deviceCapabilitiesMask & (1 << pos))
        {
            int bit_pos = pos;
            driverCapaNames.append(DriverCapaToString(bit_pos));
        }
    }
    if (jsonRequest.GetClientType() == ClientType::DM_TOOLS)
    {
        jsonResponse.SetDeviceCapabilitiesMask(deviceCapabilitiesMask);
    }
    else
    {
        jsonResponse.SetDeviceCapabilities(driverCapaNames);
    }

}


void GetFwHealthStateResponse::SetFwHealthState(int fwHealthState)
{
    m_jsonResponseValue["FwHealthState"] = fwHealthState;
}

void GetFwHealthStateResponse::SetFwHealthStateStr(const std::string fwHealthStateStr)
{
    m_jsonResponseValue["FwHealthStateStr"] = fwHealthStateStr;
}

void GetFwHealthStateHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, GetFwHealthStateResponse& jsonResponse)
{
    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    ostringstream ss;
    ss << spDevice->GetFwStateProvider()->GetFwInfo().FwHealthState;
    const std::string fwHealthStateStr = ss.str();
    const int fwHealthState = static_cast<int>(spDevice->GetFwStateProvider()->GetFwInfo().FwHealthState);
    if (jsonRequest.GetClientType() == ClientType::DM_TOOLS)
    {
        jsonResponse.SetFwHealthState(fwHealthState);
    }
    else
    {
        jsonResponse.SetFwHealthStateStr(fwHealthStateStr);
    }
}
