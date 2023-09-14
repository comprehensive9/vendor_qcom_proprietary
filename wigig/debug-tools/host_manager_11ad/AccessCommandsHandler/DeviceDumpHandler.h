/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"
#include "DeviceTypes.h"
#include "OperationStatus.h"

#include <string>

class Device;
struct fw_map;

class DumpBuilder
{
public:
    DumpBuilder(const std::string& dumpName, const std::string& deviceName, const BasebandRevision baseBand);

    void AddContentSection(const std::string& regionName, const uint32_t linkerRegionBegin, const uint32_t linkerRegionEnd, const uint32_t ahbRegionBegin,
        const std::string& regionContent, const std::string& regionError = "");


    void AddSkippedSection(const std::string& regionName, const uint32_t linkerRegionBegin, const uint32_t linkerRegionEnd, const uint32_t ahbRegionBegin);

    void AddNumberOfErrors(int number);

    const Json::Value& GetDump();

private:
    Json::Value m_dump;
};

class DeviceDumpRequest final : public JsonDeviceRequest
{
public:
    explicit DeviceDumpRequest(const Json::Value& jsonRequestValue)
        : JsonDeviceRequest(jsonRequestValue)
    {
    }

    JsonValueBoxed<std::string>GetDumpLocation() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, "Location");
    }

        JsonValueBoxed<std::string>GetDumpName() const
    {
        return JsonValueParser::ParseStringValue(m_jsonRequestValue, "Name");
    }



};

class DeviceDumpResponse final : public JsonResponse
{
public:
    DeviceDumpResponse(const Json::Value& jsonRequestValue, Json::Value& jsonResponseValue) :
        JsonResponse("DeviceDumpResponse", jsonRequestValue, jsonResponseValue)
    {
    }

    void SetDumpContent(const Json::Value& JsonDumpFile)
    {
        m_jsonResponseValue["JsonDump"] = JsonDumpFile;
    }

    void SetDumpPath(const std::string& dumpFilePath)
    {
        m_jsonResponseValue["DumpPath"] = dumpFilePath;
    }
};


class DeviceDumpHandler final
    : public JsonOpCodeHandlerBase<DeviceDumpRequest, DeviceDumpResponse>
{
    void HandleRequest(const DeviceDumpRequest& jsonRequest, DeviceDumpResponse& jsonResponse) override;

    OperationStatus CreateDeviceDump(
        std::shared_ptr<Device> spDevice, DumpBuilder& memoryDump);

    OperationStatus DumpMemoryRegion(
        std::shared_ptr<Device> spDevice, const fw_map& region, DumpBuilder& memoryDump);
};

