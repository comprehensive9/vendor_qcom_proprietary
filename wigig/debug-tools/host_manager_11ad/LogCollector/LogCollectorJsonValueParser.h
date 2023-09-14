/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "LogCollectorDefinitions.h"
#include "LogCollectorModuleVerbosity.h"
#include "OperationStatus.h"

// configuration parameters
extern const std::string POLLING_INTERVAL_MS;
extern const std::string MAX_SINGLE_FILE_SIZE_MB;
extern const std::string MAX_NUM_OF_LOG_FILES;
extern const std::string FLUSH_CHUNK;
extern const std::string CONVERSION_FILE_PATH;
extern const std::string LOG_FILES_DIRECTORY;
extern const std::string TARGET_SERVER;
extern const std::string USER_NAME;
extern const std::string REMOTE_PATH;

class LogCollectorJsonValueParser
{
public:

    static JsonValueBoxed<CpuType> ParseCpuType(
        const Json::Value& jsonValue, const char* szKey);

    static JsonValueBoxed<RecordingType> ParseRecordingType(
        const Json::Value& jsonValue, const char* szKey);

    static OperationStatus ParseAllModuleVerbosity(
        const Json::Value& jsonValue, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity);

    static const char* GetModuleName(unsigned moduleIndex);

private:

    static JsonValueBoxed<LogCollectorModuleVerbosity> ParseModuleVerbosityOrDefault(
        const Json::Value& jsonValue, const char* szKey, unsigned moduleIndex);

    static JsonValueBoxed<LogCollectorModuleVerbosity> ParseModuleVerbosity(
        const Json::Value& jsonValue, const char* szKey, unsigned moduleIndex);

    static std::vector<const char*> s_moduleName;
};
