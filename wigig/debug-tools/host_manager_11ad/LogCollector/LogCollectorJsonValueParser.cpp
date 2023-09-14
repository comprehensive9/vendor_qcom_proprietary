/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorJsonValueParser.h"

// configuration parameters
const std::string POLLING_INTERVAL_MS = "PollingIntervalMs";
const std::string MAX_SINGLE_FILE_SIZE_MB = "MaxSingleFileSizeMb";
const std::string MAX_NUM_OF_LOG_FILES = "MaxNumberOfLogFiles";
const std::string FLUSH_CHUNK = "FlushChunk";
const std::string CONVERSION_FILE_PATH = "ConversionFilePath";
const std::string LOG_FILES_DIRECTORY = "LogFilesDirectory";
const std::string TARGET_SERVER = "TargetServer";
const std::string USER_NAME = "UserName";
const std::string REMOTE_PATH = "RemotePath";

std::vector<const char*> LogCollectorJsonValueParser::s_moduleName =
{
    "Module0", "Module1", "Module2", "Module3",
    "Module4", "Module5", "Module6", "Module7",
    "Module8", "Module9", "Module10", "Module11",
    "Module12", "Module13", "Module14", "Module15"
};

const char* LogCollectorJsonValueParser::GetModuleName(unsigned moduleIndex)
{
    return moduleIndex < s_moduleName.size() ? s_moduleName[moduleIndex] : "UnsupportedModule";
}

JsonValueBoxed<CpuType> LogCollectorJsonValueParser::ParseCpuType(const Json::Value& jsonValue, const char* szKey)
{
    const Json::Value& value = jsonValue[szKey];
    if (Json::Value::nullSingleton() == value)
    {
        return { szKey, JsonValueState::JSON_VALUE_MISSING, "fw|ucode" };
    }

    const std::string valueStr = value.asString();

    if (valueStr == "fw")
    {
        return JsonValueBoxed<CpuType>(szKey, CPU_TYPE_FW);
    }
    if (valueStr == "ucode")
    {
        return JsonValueBoxed<CpuType>(szKey, CPU_TYPE_UCODE);
    }
    if (valueStr == "both")
    {
        return JsonValueBoxed<CpuType>(szKey, CPU_TYPE_BOTH);
    }
    return { szKey, JsonValueState::JSON_VALUE_MALFORMED, "fw|ucode" };
}

JsonValueBoxed<RecordingType> LogCollectorJsonValueParser::ParseRecordingType(const Json::Value& jsonValue, const char* szKey)
{
    const Json::Value& value = jsonValue[szKey];
    if (Json::Value::nullSingleton() == value)
    {
        return JsonValueBoxed<RecordingType>(szKey, JsonValueState::JSON_VALUE_MISSING, "raw|txt");
    }

    const std::string valueStr = value.asString();

    if (valueStr == "raw")
    {
        return JsonValueBoxed<RecordingType>(szKey, RECORDING_TYPE_XML);
    }
    if (valueStr == "txt")
    {
        return JsonValueBoxed<RecordingType>(szKey, RECORDING_TYPE_TXT);
    }
    if (valueStr == "both")
    {
        return JsonValueBoxed<RecordingType>(szKey, RECORDING_TYPE_BOTH);
    }
    if (valueStr == "publish")
    {
        return JsonValueBoxed<RecordingType>(szKey, RECORDING_TYPE_PUBLISH);
    }
    if (valueStr == "pmc")
    {
        return JsonValueBoxed<RecordingType>(szKey, RECORDING_TYPE_PMC);
    }

    return { szKey, JsonValueState::JSON_VALUE_MALFORMED, "raw|txt" };
}

OperationStatus LogCollectorJsonValueParser::ParseAllModuleVerbosity(
    const Json::Value& jsonValue, std::vector<LogCollectorModuleVerbosity>& moduleVerbosity)
{
    moduleVerbosity.clear();

    for (unsigned moduleIndex = 0; moduleIndex < s_moduleName.size(); ++moduleIndex)
    {
        JsonValueBoxed<LogCollectorModuleVerbosity> moduleVerbosityBoxed =
            ParseModuleVerbosityOrDefault(jsonValue, s_moduleName[moduleIndex], moduleIndex);

        LOG_VERBOSE << "Parsed effective verbosity for module " << moduleIndex << ": " << moduleVerbosityBoxed << std::endl;

        switch (moduleVerbosityBoxed.GetState())
        {
        case JsonValueState::JSON_VALUE_PROVIDED:
            moduleVerbosity.emplace_back(moduleVerbosityBoxed.GetValue());
            break;
        case JsonValueState::JSON_VALUE_MISSING:
            break;
        default:
            return OperationStatus(false, moduleVerbosityBoxed.BuildStateReport());
        }
    }

    return OperationStatus();
}

JsonValueBoxed<LogCollectorModuleVerbosity> LogCollectorJsonValueParser::ParseModuleVerbosityOrDefault(
    const Json::Value& jsonValue, const char* szKey, unsigned moduleIndex)
{
    JsonValueBoxed<LogCollectorModuleVerbosity> moduleVerbosityBoxed =
        ParseModuleVerbosity(jsonValue, szKey, moduleIndex);

    if (moduleVerbosityBoxed.GetState() == JsonValueState::JSON_VALUE_PROVIDED ||
        moduleVerbosityBoxed.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        return moduleVerbosityBoxed;
    }

    JsonValueBoxed<LogCollectorModuleVerbosity>defaultVerbosityBoxed =
        ParseModuleVerbosity(jsonValue, "DefaultVerbosity", moduleIndex);
    if (defaultVerbosityBoxed.GetState() == JsonValueState::JSON_VALUE_PROVIDED ||
        defaultVerbosityBoxed.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        return defaultVerbosityBoxed;
    }

    return JsonValueBoxed<LogCollectorModuleVerbosity>(szKey, JsonValueState::JSON_VALUE_MISSING, "VIEW");
}

JsonValueBoxed<LogCollectorModuleVerbosity> LogCollectorJsonValueParser::ParseModuleVerbosity(
    const Json::Value& jsonValue, const char* szKey, unsigned moduleIndex)
{
    const Json::Value& value = jsonValue[szKey];
    if (Json::Value::nullSingleton() == value)
    {
        return { szKey, JsonValueState::JSON_VALUE_MISSING, szKey };
    }

    const std::string verbosityString = value.asString();
    bool isError = false;
    bool isWarning = false;
    bool isInfo = false;
    bool isVerbose = false;

    for (const auto& c : verbosityString)
    {
        switch (c)
        {
        case 'V':
            isVerbose = true;
            break;
        case 'I':
            isInfo = true;
            break;
        case 'E':
            isError = true;
            break;
        case 'W':
            isWarning = true;
            break;
        default:
            return { szKey, JsonValueState::JSON_VALUE_MALFORMED, "VIEW" };
        }
    }

    return JsonValueBoxed<LogCollectorModuleVerbosity>(
        szKey, LogCollectorModuleVerbosity(moduleIndex, isError, isWarning, isInfo, isVerbose));
}