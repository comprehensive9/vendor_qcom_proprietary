/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorDumpHandler.h"
#include "PersistentConfiguration.h"
#include "DeviceManager.h"
#include "Device.h"
#include "LogCollectorService.h"
#include "Host.h"
#include "LogFmtStringsContainer.h"

#include <sstream>
#include <fstream>
#include <cstring>

void LogCollectorDumpHandler::HandleRequest(const LogCollectorDumpRequest& jsonRequest, LogCollectorDumpResponse& jsonResponse)
{
    std::shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
    const std::string deviceName = spDevice->GetDeviceName();

    // Determine CPU
    CpuType cpuType = CPU_TYPE_BOTH;
    const JsonValueBoxed<CpuType> cpuTypeBoxed = jsonRequest.GetCpuType();
    switch (cpuTypeBoxed.GetState())
    {
        case JsonValueState::JSON_VALUE_MISSING:
            break;
        case JsonValueState::JSON_VALUE_PROVIDED:
            cpuType = cpuTypeBoxed;
            break;
        default:
            jsonResponse.Fail(cpuTypeBoxed.BuildStateReport());
            return;
    }

    LOG_DEBUG << "Requested to dump " << CpuTypeToString(cpuType) << " string conversion file(s)" << std::endl;
    if (cpuType == CPU_TYPE_FW || cpuType == CPU_TYPE_BOTH)
    {
        os = StringsDump(deviceName, CPU_TYPE_FW);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }
    if (cpuType == CPU_TYPE_UCODE || cpuType == CPU_TYPE_BOTH)
    {
        os = StringsDump(deviceName, CPU_TYPE_UCODE);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    jsonResponse.SetStringsDumpPath(Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation);
}

OperationStatus LogCollectorDumpHandler::StringsDump(const std::string& deviceName, CpuType cpuType)
{
    // get format strings by offsets
    LogFmtStringsContainer fmtStringsContainer;
    OperationStatus os = Host::GetHost().GetLogCollectorService().GetFmtStrings(deviceName, cpuType, TxtRecorder, fmtStringsContainer);
    if (!os)
    {
        return os;
    }

    if (!fmtStringsContainer.IsValid())
    {
        std::ostringstream errorMsgBuilder;
        errorMsgBuilder << "Failed to read string conversion file for: " << cpuType;
        return OperationStatus(false, errorMsgBuilder.str());
    }

    // dump to recording location
    std::ostringstream fileNameBuilder;
    fileNameBuilder << std::string(Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation)
        << "dump_" << cpuType << "_string_conversion" << ".json";
    const auto fileName = fileNameBuilder.str();

    LOG_VERBOSE << "Creating " << CpuTypeToString(cpuType) << " string conversion file dump: " << fileName << std::endl;

    std::ofstream file;
    file.open(fileName);
    if (file.fail())
    {
        std::ostringstream errorMsgBuilder;
        errorMsgBuilder << "Creating strings dump file failed: " << fileName << " Error: " << strerror(errno);
        return OperationStatus(false, errorMsgBuilder.str());
    }

    file << fmtStringsContainer;
    file.close();

    LOG_DEBUG << "Created " << CpuTypeToString(cpuType) << " string conversion file dump to: " << fileName << std::endl;
    return OperationStatus();
}
