/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "WmiHandler.h"
#include "DeviceManager.h"
#include "Device.h"
#include "Host.h"
#include "DriverContracts.h"
#include "FwStateProvider.h"

using namespace std;

/* structure with payload of driver command that forces WMI commands in Sys Assert*/
struct ForceWmiInSysAssertCmd
{
    uint32_t commandId;
    uint32_t enable;
};

void WmiResponse::SetEventParameters(const string& eventParameters) const
{
    m_jsonResponseValue["EventParameters"] = eventParameters;
}

void WmiHandler::HandleRequest(const WmiRequest & jsonRequest, WmiResponse & jsonResponse)
{
    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const JsonValueBoxed<uint32_t> commandIdBoxed = jsonRequest.GetCommandId();
    if (commandIdBoxed.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(commandIdBoxed.BuildStateReport());
        return;
    }

    bool isEventExpected = false;
    const JsonValueBoxed<uint32_t> eventIdBoxed = jsonRequest.GetEventId();
    switch (eventIdBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_PROVIDED:
        isEventExpected = true;
        break;
    case JsonValueState::JSON_VALUE_MISSING:
        LOG_DEBUG << "No reply expected for WMI command id=" << Hex<>(commandIdBoxed) << endl;
        break;
    default:
        jsonResponse.Fail(eventIdBoxed.BuildStateReport());
    }

    unsigned waitForEventTimeout = WmiConvertor::DefaultEventTimeoutSec;
    if (isEventExpected) // timeout is relevant only if eventId to wait for provided
    {
        const JsonValueBoxed<uint32_t> waitForEventTimeoutBoxed = jsonRequest.GetTimeOut();
        switch (waitForEventTimeoutBoxed.GetState())
        {
        case JsonValueState::JSON_VALUE_PROVIDED:
            waitForEventTimeout = waitForEventTimeoutBoxed;
            break;
        case JsonValueState::JSON_VALUE_MISSING:
            // using default value
            break;
        default:
            jsonResponse.Fail(waitForEventTimeoutBoxed.BuildStateReport());
        }
    }

    const JsonValueBoxed<string>& parametersAsStringBoxed = jsonRequest.GetParameters();
    switch (parametersAsStringBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_PROVIDED:
        break;
    case JsonValueState::JSON_VALUE_MISSING:
        LOG_DEBUG << "No parameters provided for WMI command id=" << Hex<>(commandIdBoxed) << endl;
        break;
    default:
        jsonResponse.Fail(parametersAsStringBoxed.BuildStateReport());
    }

    vector<unsigned char> binaryBuffer;
    os = WmiConvertor::ConstructWmiBinaryData(commandIdBoxed, parametersAsStringBoxed, binaryBuffer);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    if (ShouldPrint(LOG_SEV_DEBUG)) // spare all the formatting when not needed
    {
        LOG_DEBUG << WmiCommandToString(commandIdBoxed, binaryBuffer) << endl;
    }

    if (spDevice->GetFwStateProvider()->InSysAssert())
    {
        struct ForceWmiInSysAssertCmd binaryData = { static_cast<uint32_t>(wil_nl_60g_generic_cmd::NL_60G_GEN_FORCE_WMI_SEND), 1 };

        os = spDevice->DriverControl(static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_GENERIC_COMMAND), &binaryData,
            sizeof(ForceWmiInSysAssertCmd), nullptr, 0);
        if (!os)
        {
            os.AddPrefix("Failed to force WMI command in Sys Assert");
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    if (isEventExpected)
    {
        vector<unsigned char> eventBinaryData;
        os = spDevice->SendWmiWithEvent(binaryBuffer.data(), static_cast<uint32_t>(binaryBuffer.size()), eventIdBoxed, waitForEventTimeout,
            eventBinaryData, commandIdBoxed);
        if (!os)
        {
            if (spDevice->GetFwStateProvider()->InSysAssert())
            {
                os.AddSuffix("Device in Sys Assert. Make sure you released lock by writing value '0x0' to 'FW.g_sysassert_wmi_lock'");
            }
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }

        string eventHexParameters = WmiConvertor::ParseEvent(eventBinaryData, false);
        jsonResponse.SetEventParameters(eventHexParameters);
        return;
    }

    // No reply requested
    os = spDevice->DriverControl(static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_FW_WMI), binaryBuffer.data(),
        static_cast<uint32_t>(binaryBuffer.size()), nullptr, 0);

    if (!os)
    {
        if (spDevice->GetFwStateProvider()->InSysAssert())
        {
            os.AddSuffix("Device in Sys Assert. Make sure you released lock by writing value '0x0' to 'FW.g_sysassert_wmi_lock'");
        }
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
}

std::string WmiHandler::WmiCommandToString(uint32_t commandId, std::vector<unsigned char>& binaryData)
{
    ostringstream ss;
    ss << "Sending WMI: [command id: " << Hex<>(commandId) << "] ";
    ss << "Payload (bytes):" << endl;
    for (const auto& byteVal : binaryData)
    {
        ss << "\t" << Hex<>(byteVal) << endl;
    }
    return ss.str();
}


void WmiUTHandler::HandleRequest(const WmiUTRequest& jsonRequest, WmiResponse& jsonResponse)
{
    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const JsonValueBoxed<uint32_t> moduleIdBoxed = jsonRequest.GetModuleId();
    if (moduleIdBoxed.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(moduleIdBoxed.BuildStateReport());
        return;
    }

    const JsonValueBoxed<uint32_t> subtypeIdBoxed = jsonRequest.GetSubtypeId();
    if (subtypeIdBoxed.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(subtypeIdBoxed.BuildStateReport());
        return;
    }

    unsigned waitForEventTimeout = WmiConvertor::DefaultEventTimeoutSec;
    const JsonValueBoxed<uint32_t> waitForEventTimeoutBoxed = jsonRequest.GetTimeOut();
    switch (waitForEventTimeoutBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_PROVIDED:
        waitForEventTimeout = waitForEventTimeoutBoxed;
        break;
    case JsonValueState::JSON_VALUE_MISSING:
        // using default value
        break;
    default:
        jsonResponse.Fail(waitForEventTimeoutBoxed.BuildStateReport());
    }

    const JsonValueBoxed<string>& parametersAsStringBoxed = jsonRequest.GetParameters();
    switch (parametersAsStringBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_PROVIDED:
        break;
    case JsonValueState::JSON_VALUE_MISSING:
        LOG_DEBUG << "No parameters provided for WMI UT command module id=" << Hex<>(moduleIdBoxed) << " subtype id=" << subtypeIdBoxed << endl;
        break;
    default:
        jsonResponse.Fail(parametersAsStringBoxed.BuildStateReport());
    }

    vector<unsigned char> binaryBuffer;
    os = WmiConvertor::ConstructWmiUTBinaryData(moduleIdBoxed, subtypeIdBoxed, parametersAsStringBoxed, binaryBuffer);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    if (ShouldPrint(LOG_SEV_DEBUG)) // spare all the formatting when not needed
    {
        LOG_DEBUG << WmiUTCommandToString(moduleIdBoxed, subtypeIdBoxed, binaryBuffer) << endl;
    }

    if (spDevice->GetFwStateProvider()->InSysAssert())
    {
        struct ForceWmiInSysAssertCmd binaryCmd = { static_cast<uint32_t>(wil_nl_60g_generic_cmd::NL_60G_GEN_FORCE_WMI_SEND), 1 };

        os = spDevice->DriverControl(static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_GENERIC_COMMAND), &binaryCmd,
            sizeof(ForceWmiInSysAssertCmd), nullptr, 0);
        if (!os)
        {
            os.AddPrefix("Failed to force WMI command in Sys Assert");
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    // UT command always has event
    vector<unsigned char> eventBinaryData;
    os = spDevice->SendWmiWithEvent(binaryBuffer.data(), static_cast<uint32_t>(binaryBuffer.size()),
        WmiConvertor::WmiUTEventId, waitForEventTimeout, eventBinaryData, WmiConvertor::WmiUTCommandId, moduleIdBoxed, subtypeIdBoxed);
    if (!os)
    {
        if (spDevice->GetFwStateProvider()->InSysAssert())
        {
            os.AddSuffix("Device in Sys Assert. Make sure you released lock by writing value '0x0' to 'FW.g_sysassert_wmi_lock'");
        }
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    string eventHexParameters = WmiConvertor::ParseEvent(eventBinaryData, true);
    jsonResponse.SetEventParameters(eventHexParameters);
}

string WmiUTHandler::WmiUTCommandToString(uint32_t moduleId, uint32_t subtypeId, vector<unsigned char>& binaryData)
{
    ostringstream ss;
    ss << "Sending WMI UT" << ": [module id: " << Hex<>(moduleId) << ", subtype id:" << subtypeId << "] ";
    ss << "Payload (bytes):" << endl;
    for (const auto& byteVal : binaryData)
    {
        ss << "\t" << Hex<>(byteVal) << endl;
    }

    return ss.str();
}

// ======================================== WmiConvertor ========================================

OperationStatus WmiConvertor::ConstructWmiBinaryData(uint32_t commandId, const string& parametersAsString, vector<unsigned char>& binaryData)
{
    return ConstructBinaryData(commandId, false, 0, 0, parametersAsString, binaryData);
}

OperationStatus WmiConvertor::ConstructWmiUTBinaryData(uint32_t moduleId, uint32_t subtypeId, const std::string& parametersAsString, std::vector<unsigned char>& binaryData)
{
    return ConstructBinaryData(WmiUTCommandId, true, moduleId, subtypeId, parametersAsString, binaryData);
}

string WmiConvertor::ParseEvent(const vector<uint8_t>& eventBinaryData, bool isWmiUT)
{
    ostringstream ss;
    bool first = true;
    const auto eventParametersInDwords = reinterpret_cast<const uint32_t*>(eventBinaryData.data());
    const int size = static_cast<int>(eventBinaryData.size() / sizeof(uint32_t));
    const auto startIndex = isWmiUT ? 1 : 0; // in case WMI UT 1st DWORD in event payload is concatenated moduleId and subtypeId
    for (int i = startIndex; i < size; i++)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            ss << ", ";
        }
        ss << Hex<>(eventParametersInDwords[i]);
    }

    return ss.str();
}

OperationStatus WmiConvertor::ConstructBinaryData(uint32_t commandId, bool isWmiUT, uint32_t moduleId, uint32_t subtypeId,
    const string& parametersAsString, vector<unsigned char>& binaryData)
{
    const auto parametersHex = Utils::Split(parametersAsString, ',');
    // for wmiUT: moduleID and subtypeID are part of params concatenated in one DWORD
    const auto paramsSizeInBytes = (parametersHex.size() + (isWmiUT ? 1 : 0)) * sizeof(uint32_t);

    // Payload format:
    // struct SendRecieveWmi
    // {
    //    public uint CmdId;        // 4 bytes
    //    public UInt16 Context;    // 2 bytes - not in use, stays zero
    //    public byte DevId;        // 1 byte  - reserved for MID index, stays zero
    //    public UInt16 BufLen;     // 2 bytes (length is in bytes)
    //    public byte[] InputBuf;
    // }

    binaryData.resize(sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint16_t) + paramsSizeInBytes);

    // Add commandID
    int dstOffset = 0;
    AddDwordToBinaryData(commandId, binaryData, dstOffset);
    dstOffset += sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t);

    // Add params length in bytes (aligned to DWORD)
    AddDwordToBinaryData(static_cast<uint32_t>(paramsSizeInBytes), binaryData, dstOffset);
    dstOffset += sizeof(uint16_t);

    if (isWmiUT)
    {
        // Add moduleID and subTypeID (concatenated to one DWORD)
        AddDwordToBinaryData(SerializeModuleAndSubtypeIds(moduleId, subtypeId), binaryData, dstOffset);
        dstOffset += sizeof(uint32_t);

    }

    // Add params
    OperationStatus os = AddHexParametersToBinaryData(parametersHex, binaryData, dstOffset);
    return os;
}

OperationStatus WmiConvertor::AddHexParametersToBinaryData(const vector<string>& parameters, vector<unsigned char>& binaryData, int offset)
{
    LOG_VERBOSE << "Parsing " << parameters.size() << " hex parameters: " << endl;
    for (auto &p : parameters)
    {
        LOG_VERBOSE << p << endl;
        uint32_t x = 0;
        if (!Utils::ConvertHexStringToDword(p, x))
        {
            return OperationStatus(false, "Failed to parse parameter, invalid format: " + p);
        }
        AddDwordToBinaryData(x, binaryData, offset);
        offset += sizeof(uint32_t); // size of param is dword
    }

    return OperationStatus();
}

void WmiConvertor::AddDwordToBinaryData(uint32_t dword, vector<unsigned char>& binaryData, int offset)
{
    for (size_t i = 0; i < sizeof(uint32_t); i++)
    {
        binaryData[offset + i] = static_cast<unsigned char>((dword >> (8 * i)) & 0xFF);
    }
}

uint32_t WmiConvertor::SerializeModuleAndSubtypeIds(uint32_t moduleId, uint32_t subtypeId)
{
    return (subtypeId << 16) + ((moduleId << 16) >> 16);
}
