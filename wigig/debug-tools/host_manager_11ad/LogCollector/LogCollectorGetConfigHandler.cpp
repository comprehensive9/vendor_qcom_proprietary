/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorGetConfigHandler.h"
#include "Host.h"
#include "PersistentConfiguration.h"


void LogCollectorGetConfigHandler::HandleRequest(
    const LogCollectorGetConfigRequest& jsonRequest, LogCollectorGetConfigResponse& jsonResponse)
{
    (void)jsonRequest;
    // Get the configuration from the persistent configuration and not from device
    // Do we want to provide current configuration on the device

    const auto &config = Host::GetHost().GetConfiguration();

    jsonResponse.SetPollingIntervalMS(config.LogCollectorConfiguration.PollingIntervalMs);
    jsonResponse.SetMaxSingleFileSizeMB(config.LogCollectorConfiguration.MaxLogFileSize);
    jsonResponse.SetMaxNumOfLogFiles(config.LogCollectorConfiguration.MaxNumOfLogFiles);
    jsonResponse.SetFlushChunk(config.LogCollectorConfiguration.FlushChunk);

    jsonResponse.SetConversionFilePath(config.LogCollectorConfiguration.ConversionFileLocation);
    jsonResponse.SetLogFilesDirectoryPath(config.LogCollectorConfiguration.TargetLocation);

    jsonResponse.SetTargetServer(config.LogCollectorConfiguration.TargetServer);
    jsonResponse.SetUserName(config.LogCollectorConfiguration.UserName);
    jsonResponse.SetRemotePath(config.LogCollectorConfiguration.RemotePath);
}
