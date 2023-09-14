/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LegacyPmcHandler.h"
#include "SharedVersionInfo.h"
#include "ShellCommandExecutor.h"
#include "DebugLogger.h"
#include "Device.h"
#include "DeviceManager.h"
#include "FileReader.h"
#include "Host.h"
#include "OperationStatus.h"

#include <string>
#include <vector>

using namespace std;

// *************************************************************************************************

void LegacyPmcSetAspmHandler::HandleRequest(const LegacyPmcSetAspmRequest& jsonRequest, JsonResponse& jsonResponse)
{
    if (IsWindows())
    {
        jsonResponse.Fail("Not supported on Windows platform");
        return;
    }

    if (jsonRequest.Enable().GetState() == JsonValueState::JSON_VALUE_MALFORMED ||
        jsonRequest.Enable().GetState() == JsonValueState::JSON_VALUE_MISSING)
    {
        jsonResponse.Fail(jsonRequest.Enable().BuildStateReport());
        return;
    }

    const bool enablePcieAspm = jsonRequest.Enable().GetValue();
    LOG_DEBUG << "Setting PCIe ASPM mode to " << (enablePcieAspm ? "enable" : "disable") << " (SPARROW-only feature)" << endl;
    const char* const szShellCommand = enablePcieAspm ? "setpci -d:310 80.b=2:3" : "setpci -d:310 80.b=0:3";

    const ShellCommandExecutor disableAspmCommand(szShellCommand);

    // setpci always exits with zero, even when failure occurs
    if (0 != disableAspmCommand.ExitStatus() || !disableAspmCommand.Output().empty())
    {
        ostringstream oss;
        oss << "Error executing command " << szShellCommand << endl;
        jsonResponse.Fail(oss.str());
    }
}

// *************************************************************************************************

void LegacyPmcGetAspmHandler::HandleRequest(const JsonDeviceRequest& jsonRequest, LegacyPmcGetAspmResponse& jsonResponse)
{
    (void)jsonRequest;

    if (IsWindows())
    {
        jsonResponse.Fail("Not supported on Windows platform");
        return;
    }

    LOG_DEBUG << "Querying PCIe ASPM mode (SPARROW-only)" << std::endl;
    const char* const szShellCommand = "setpci -d:310 80.b";
    const ShellCommandExecutor queryAspmCommand(szShellCommand);

    // setpci always exits with zero, even when failure occurs
    if (0 != queryAspmCommand.ExitStatus() || 1 != queryAspmCommand.Output().size())
    {
        ostringstream oss;
        oss << "Error executing command " << szShellCommand << endl;
        jsonResponse.Fail(oss.str());
        return;
    }

    const bool pcieAspmEnabled = (queryAspmCommand.Output()[0] != "00");
    jsonResponse.SetEnabled(pcieAspmEnabled);
}

// *************************************************************************************************

void LegacyPmcRingAllocHandler::HandleRequest(const LegacyPmcRingAllocRequest& jsonRequest, JsonResponse& jsonResponse)
{
    if (IsWindows())
    {
        jsonResponse.Fail("Not supported on Windows platform");
        return;
    }

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    if (jsonRequest.GetDescriptorSize().GetState() == JsonValueState::JSON_VALUE_MALFORMED ||
        jsonRequest.GetDescriptorSize().GetState() == JsonValueState::JSON_VALUE_MISSING)
    {
        jsonResponse.Fail(jsonRequest.GetDescriptorSize().BuildStateReport());
        return;
    }

    if (jsonRequest.GetDescriptorNumber().GetState() == JsonValueState::JSON_VALUE_MALFORMED ||
        jsonRequest.GetDescriptorNumber().GetState() == JsonValueState::JSON_VALUE_MISSING)
    {
        jsonResponse.Fail(jsonRequest.GetDescriptorNumber().BuildStateReport());
        return;
    }

    const unsigned descSize = jsonRequest.GetDescriptorSize().GetValue();
    const unsigned descNum = jsonRequest.GetDescriptorNumber().GetValue();

    os = spDevice->AllocPmc(descSize, descNum);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
    }
};

// *************************************************************************************************

void LegacyPmcRingFreeHandler::HandleRequest(const LegacyPmcRingFreeRequest& jsonRequest, JsonResponse& jsonResponse)
{
    if (IsWindows())
    {
        jsonResponse.Fail("Not supported on Windows platform");
        return;
    }

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    if (jsonRequest.GetSuppressError().GetState() == JsonValueState::JSON_VALUE_MALFORMED ||
        jsonRequest.GetSuppressError().GetState() == JsonValueState::JSON_VALUE_MISSING)
    {
        jsonResponse.Fail(jsonRequest.GetSuppressError().BuildStateReport());
        return;
    }

    const bool bSuppressError = jsonRequest.GetSuppressError().GetValue();
    os = spDevice->DeallocPmc(bSuppressError);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
    }
}

// *************************************************************************************************

void LegacyPmcGetDataHandler::HandleRequest(const LegacyPmcGetDataRequest& jsonRequest, LegacyPmcGetDataResponse& jsonResponse)
{
    if (IsWindows())
    {
        jsonResponse.Fail("Not supported on Windows platform");
        return;
    }

    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    if (jsonRequest.GetRequestId().GetState() == JsonValueState::JSON_VALUE_MALFORMED ||
        jsonRequest.GetRequestId().GetState() == JsonValueState::JSON_VALUE_MISSING)
    {
        jsonResponse.Fail(jsonRequest.GetRequestId().BuildStateReport());
        return;
    }

    const uint32_t requestId = jsonRequest.GetRequestId().GetValue();
    os = spDevice->CreatePmcFiles(requestId);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    std::string foundPmcDataFilePath;
    os = spDevice->FindPmcDataFile(requestId, foundPmcDataFilePath);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    vector<unsigned char> pmcData;
    os = ReadFile(foundPmcDataFilePath, pmcData);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetPmcData(pmcData);

    // PMC ring Metadata is only available for a newer driver

    std::string foundPmcRingFilePath;
    os = spDevice->FindPmcRingFile(requestId, foundPmcRingFilePath);
    if (!os)
    {
        LOG_WARNING << "No PMC ring metadata available" << endl;
        return;
    }

    vector<unsigned char> pmcRingMetaData;  // Optional content, missing for old driver
    os = ReadFile(foundPmcRingFilePath, pmcRingMetaData);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetPmcRingMetadataData(pmcRingMetaData);

}

OperationStatus LegacyPmcGetDataHandler::ReadFile(const string& filePath, vector<unsigned char>& fileContent)
{
    FileReader fileReader(filePath.c_str());
    const size_t fileSize = fileReader.GetFileSize();
    LOG_VERBOSE << "Reading file: " << filePath << " Size: " << fileSize << " B" << std::endl;

    if (0 == fileSize)
    {
        return OperationStatus(false, "Empty file");
    }

    fileContent.resize(fileSize);
    const size_t pmcDataSize = fileReader.ReadChunk(fileContent.data(), fileContent.size());
    LOG_ASSERT(pmcDataSize == fileSize);

    return OperationStatus();
}