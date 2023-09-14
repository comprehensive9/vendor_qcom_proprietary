/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorVerbosityHandler.h"

#include "Device.h"
#include "Host.h"
#include "DeviceManager.h"
#include "LogCollectorService.h"

using namespace std;

void LogCollectorGetVerbosityHandler::HandleRequest(
    const LogCollectorGetVerbosityRequest& jsonRequest, LogCollectorGetVerbosityResponse& jsonResponse)
{
    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    const std::string deviceName = spDevice->GetDeviceName();

    const JsonValueBoxed<CpuType> cpuType = jsonRequest.GetCpuType();
    if (cpuType.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(cpuType.BuildStateReport());
        return;
    }

    LOG_DEBUG << "Get verbosity for CPU type: " << cpuType << endl;

    std::vector<LogCollectorModuleVerbosity> effectiveVerbosity;
    os = Host::GetHost().GetLogCollectorService().GetModuleVerbosity(deviceName, cpuType, effectiveVerbosity);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    std::vector<LogCollectorModuleVerbosity> enforcedVerbosity;
    os = Host::GetHost().GetLogCollectorService().GetEnforcedModuleVerbosity(deviceName, cpuType, enforcedVerbosity);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetModuleVerbosity(effectiveVerbosity, !enforcedVerbosity.empty());
}

// *************************************************************************************************

void LogCollectorGetEnforcedVerbosityHandler::HandleRequest(
    const LogCollectorGetEnforcedVerbosityRequest& jsonRequest, LogCollectorGetEnforcedVerbosityResponse& jsonResponse)
{
    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    const std::string deviceName = spDevice->GetDeviceName();

    const JsonValueBoxed<CpuType> cpuType = jsonRequest.GetCpuType();
    if (cpuType.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(cpuType.BuildStateReport());
        return;
    }

    LOG_DEBUG << "Get enforced verbosity for CPU type: " << cpuType << endl;

    std::vector<LogCollectorModuleVerbosity> enforcedVerbosity;
    os = Host::GetHost().GetLogCollectorService().GetEnforcedModuleVerbosity(deviceName, cpuType, enforcedVerbosity);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    jsonResponse.SetEnforcedModuleVerbosity(enforcedVerbosity);
}

// *************************************************************************************************

void LogCollectorSetVerbosityHandler::HandleRequest(
    const LogCollectorSetVerbosityRequest& jsonRequest, LogCollectorSetVerbosityResponse& jsonResponse)
{
    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    const std::string deviceName = spDevice->GetDeviceName();

    const JsonValueBoxed<CpuType> cpuType = jsonRequest.GetCpuType();
    if (cpuType.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        jsonResponse.Fail(cpuType.BuildStateReport());
        return;
    }

    LOG_DEBUG << "Log Collector set verbosity request for Device: " << deviceName << " with CPU type: " << cpuType << std::endl;

    std::vector<LogCollectorModuleVerbosity> enforcedVerbosity;
    os = jsonRequest.GetModuleVerbosity(enforcedVerbosity);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
    }

    os = Host::GetHost().GetLogCollectorService().SetModuleVerbosity(deviceName, cpuType, enforcedVerbosity);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
    }
}