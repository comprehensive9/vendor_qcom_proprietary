/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "LogCollectorJsonValueParser.h"
#include "LogCollectorDefinitions.h"
#include "OperationStatus.h"

class LogCollectorStartRecordingRequest : public JsonDeviceRequest
{
public:
    explicit LogCollectorStartRecordingRequest(const Json::Value& jsonRequestValue) :
        JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<CpuType> GetCpuType() const
    {
        return LogCollectorJsonValueParser::ParseCpuType(m_jsonRequestValue, "CpuType");
    }

    JsonValueBoxed<RecordingType> GetRecordingTypeBoxed() const
    {
        return LogCollectorJsonValueParser::ParseRecordingType(m_jsonRequestValue, "RecordingType");
    }

    JsonValueBoxed<bool> GetIgnoreHistoryBoxed() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "IgnoreLogHistory");
    }

    JsonValueBoxed<bool> GetCompressBoxed() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "Compress");
    }

    JsonValueBoxed<bool> GetUploadBoxed() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "Upload");
    }

    JsonValueBoxed<bool> GetDebugModeBoxed() const
    {
        return JsonValueParser::ParseBooleanValue(m_jsonRequestValue, "Debug");
    }

    OperationStatus GetModuleVerbosity(std::vector<LogCollectorModuleVerbosity>& moduleVerbosity) const
    {
        return LogCollectorJsonValueParser::ParseAllModuleVerbosity(m_jsonRequestValue, moduleVerbosity);
    }
};

class LogCollectorStartRecordingResponse : public JsonResponse
{
public:
    LogCollectorStartRecordingResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("LogCollectorStartRecordingResponse", jsonRequestValue, jsonResponseValue)
    {
    }
    void SetRecordingPath(const std::string & recPath)
    {
        m_jsonResponseValue["RecordingPath"] = recPath;
    }
    void SetRecordingType(RecordingType recType)
    {
        std::stringstream rtss;
        rtss << recType;
        m_jsonResponseValue["RecordingType"] = rtss.str();
    }
    void SetCompress(bool compress)
    {
        m_jsonResponseValue["Compress"] = compress;
    }
    void SetUpload(bool upload)
    {
        m_jsonResponseValue["Upload"] = upload;
    }
};

class LogCollectorStartRecordingHandler :
    public JsonOpCodeHandlerBase<LogCollectorStartRecordingRequest, LogCollectorStartRecordingResponse>
{
    void HandleRequest(
        const LogCollectorStartRecordingRequest& jsonRequest, LogCollectorStartRecordingResponse& jsonResponse) override;

    static OperationStatus StartTxtRecording(
        const std::string& deviceName,
        bool fwRecordingRequested, bool uCodeRecordingRequested,
        bool ignoreLogHistory, bool compressRequested, bool uploadRequested, bool debugMode,
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity);

    static OperationStatus StartBinRecording(
        const std::string& deviceName,
        bool fwRecordingRequested, bool uCodeRecordingRequested,
        bool ignoreLogHistory, bool compressRequested, bool uploadRequested, bool debugMode,
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity);

    static OperationStatus StartPublishing(
        const std::string& deviceName,
        bool fwRecordingRequested, bool uCodeRecordingRequested, bool ignoreLogHistory,
        const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity);
};