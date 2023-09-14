/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <sstream>

#include "LogCollectorSetConfigHandler.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "DeviceManager.h"
#include "LogCollectorService.h"

using namespace std;

void LogCollectorSetConfigHandler::HandleRequest(
    const LogCollectorSetConfigRequest& jsonRequest, LogCollectorSetConfigResponse& jsonResponse)
{
    // This flow only update the persistent configuration and does not affect existing recording
    LOG_DEBUG << "Log Collector set configuration request " << std::endl;
    bool isChangeProvided = false;

    // check if any recording exist
    if (Host::GetHost().GetLogCollectorService().IsLogging())
    {
        jsonResponse.Fail("Cannot change configuration when log collection is active");
        return;
    }

    const JsonValueBoxed<uint32_t> pollingIntervalMs = jsonRequest.GetPollingIntervalMs();
    if (pollingIntervalMs.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        jsonResponse.Fail(pollingIntervalMs.BuildStateReport());
        return;
    }
    if (pollingIntervalMs.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting polling interval to: " << pollingIntervalMs << endl;
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.PollingIntervalMs = pollingIntervalMs;
    }

    const JsonValueBoxed<uint32_t> maxSingleFileSizeMB = jsonRequest.GetMaxSingleFileSizeMb();
    if (maxSingleFileSizeMB.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        jsonResponse.Fail(maxSingleFileSizeMB.BuildStateReport());
        return;
    }
    if (maxSingleFileSizeMB.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting log file size to: " << maxSingleFileSizeMB << endl;
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxLogFileSize = maxSingleFileSizeMB;
    }

    const JsonValueBoxed<uint32_t> maxNumOfLogFiles = jsonRequest.GetMaxNumOfLogFiles();
    if (maxNumOfLogFiles.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        jsonResponse.Fail(maxNumOfLogFiles.BuildStateReport());
        return;
    }
    if (maxNumOfLogFiles.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting max log file number to: " << pollingIntervalMs << endl;
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxNumOfLogFiles = maxNumOfLogFiles;
    }

    const JsonValueBoxed<bool> flushChunk = jsonRequest.GetFlushChunk();
    if (flushChunk.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        jsonResponse.Fail(flushChunk.BuildStateReport());
        return;
    }
    if (flushChunk.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting flush chunk flag to: " << flushChunk << endl;
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.FlushChunk = flushChunk;
    }

    const JsonValueBoxed<std::string>& conversionFilePath = jsonRequest.GetConversionFilePath();
    if (conversionFilePath.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting conversion path to: " << conversionFilePath << endl;
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.ConversionFileLocation = conversionFilePath;
    }

    const JsonValueBoxed<std::string>& logFilesDirectoryPath = jsonRequest.GetLogFilesDirectoryPath();
    if (logFilesDirectoryPath.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting log output path to: " << logFilesDirectoryPath << endl;
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetLocation = logFilesDirectoryPath;
    }

    // Config for post actions:
    const JsonValueBoxed<std::string>& targetServerBoxed = jsonRequest.GetTargetServer();
    if (targetServerBoxed.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting log target server to: " << targetServerBoxed << endl;
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetServer = targetServerBoxed;
    }
    const JsonValueBoxed<std::string>& userNameBoxed = jsonRequest.GetUserName();
    if (userNameBoxed.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting UserName to: " << userNameBoxed << endl;
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.UserName = userNameBoxed;
    }
    const JsonValueBoxed<std::string>& remotePathBoxed = jsonRequest.GetRemotePath();
    if (remotePathBoxed.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        isChangeProvided = true;
        LOG_DEBUG << "Setting RemotePath to: " << remotePathBoxed << endl;
        Host::GetHost().GetConfiguration().LogCollectorConfiguration.RemotePath = remotePathBoxed;
    }

    if (!isChangeProvided)
    {
        jsonResponse.Fail("No configuration parameters have been provided");
        return;
    }

    Host::GetHost().GetConfiguration().CommitChanges();
}

void LogCollectorReSetConfigHandler::HandleRequest(
    const Json::Value& jsonRequestValue, LogCollectorReSetConfigResponse & jsonResponse)
{
    (void)jsonRequestValue;

    // check if any recording exist
    if (Host::GetHost().GetLogCollectorService().IsLogging())
    {
        jsonResponse.Fail("Cannot change configuration when log collection is active");
        return;
    }

    Host::GetHost().GetConfiguration().Reset();
    Host::GetHost().GetConfiguration().CommitChanges();
}
