/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <fstream>
#include <vector>
#include <cstring>

#include "DeviceDumpHandler.h"
#include "AddressTranslator.h"
#include "Device.h"
#include "DeviceManager.h"
#include "Host.h"
#include "FileSystemOsAbstraction.h"
#include "DriverContracts.h"
#include "PersistentConfiguration.h"

using namespace std;

void DeviceDumpHandler::HandleRequest(
    const DeviceDumpRequest& jsonRequest, DeviceDumpResponse& jsonResponse)
{
    shared_ptr<Device> spDevice;
    OperationStatus os = Host::GetHost().GetDeviceManager().GetDeviceByName(jsonRequest.GetDeviceName(), spDevice);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    const JsonValueBoxed<string> customDumpLocation = jsonRequest.GetDumpLocation();
    const JsonValueBoxed<string> customDumpName = jsonRequest.GetDumpName();

    std::string dumpLocation;
    if (customDumpLocation.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        dumpLocation = customDumpLocation.GetValue();
        LOG_DEBUG << "Using custom dump location: " << dumpLocation << endl;
    }
    else
    {
        dumpLocation = Host::GetHost().GetConfiguration().AccessConfiguration.DefaultDumpLocation;
        LOG_DEBUG << "Using preconfigured default dump location: " << dumpLocation << endl;

    }
    LOG_DEBUG << "Creating dump location folder if needed: " << dumpLocation << endl;
    os = FileSystemOsAbstraction::CreateFolder(dumpLocation);
    if (!os)
    {
        return;
    }
    std::string dumpName;
    if (customDumpName.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        dumpName = customDumpName.GetValue();
        LOG_DEBUG << "Using custom dump name: " << dumpName << endl;
    }
    else
    {
        dumpName = "Dump";
        //dumpFolderPathBuilder << dumpLocation << optionalDelimiter << dumpName << '_' << deviceName

        LOG_DEBUG << "Using default dump name: " << dumpName << endl;
    }

    dumpName = dumpName + '_' + spDevice->GetDeviceName() + '_' + Utils::GetCurrentLocalTimeForFileName() + ".json";

    DumpBuilder memoryDump(dumpName, spDevice->GetDeviceName(), spDevice->GetBasebandRevision());

    os = CreateDeviceDump(spDevice, memoryDump);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    if (jsonRequest.GetClientType() == ClientType::DM_TOOLS)
    {
        jsonResponse.SetDumpContent(memoryDump.GetDump());
    }
    else // on-target or unknown client
    {
        ostringstream dumpJsonPathBuilder;
        dumpJsonPathBuilder << dumpLocation << dumpName;
        string dumpFolderPath = dumpJsonPathBuilder.str();
        ofstream dumpJsonFile;
        dumpJsonFile.open(dumpFolderPath);

        if (dumpJsonFile.fail())
        {
            ostringstream errorBuilder;
            errorBuilder << "Error creating dump json file: " << dumpFolderPath << " Error: " << strerror(errno);
        }

        Json::StyledStreamWriter writer;
        writer.write(dumpJsonFile, memoryDump.GetDump());
        dumpJsonFile.close();
        jsonResponse.SetDumpPath(dumpFolderPath);
    }
}

OperationStatus DeviceDumpHandler::CreateDeviceDump(
    shared_ptr<Device> spDevice, DumpBuilder& memoryDump)
{

    const std::vector<struct fw_map> *const pAddressMap = AddressTranslator::GetAddressMap(spDevice->GetBasebandRevision());
    if (!pAddressMap)
    {
        ostringstream errorBuilder;
        errorBuilder << "Cannot find address map for " << spDevice->GetBasebandRevision();
        return OperationStatus(false, errorBuilder.str());
    }


    int errors = 0;
    for (const auto& region : *pAddressMap)
    {

        if (!region.CrashDump)
        {
            memoryDump.AddSkippedSection(region.RegionName, region.LinkerRegionBegin, region.LinkerRegionEnd, region.AhbRegionBegin);
            LOG_DEBUG << "Skipping memory region: " << region << endl;
            continue;
        }

        OperationStatus os = DumpMemoryRegion(spDevice, region, memoryDump);
        if (!os)
        {
            errors++;
        }
    }
    memoryDump.AddNumberOfErrors(errors);
    return OperationStatus();
}


OperationStatus DeviceDumpHandler::DumpMemoryRegion(
    shared_ptr<Device> spDevice, const fw_map& region, DumpBuilder& memoryDump)
{
    LOG_DEBUG << "Reading memory region: " << region << endl;
    const uint32_t regionSizeDwords = (region.LinkerRegionEnd - region.LinkerRegionBegin) / 4;
    LOG_DEBUG << "Reading " << regionSizeDwords << " DW from " << Address(region.AhbRegionBegin) << endl;

    vector<uint32_t> regionData(regionSizeDwords);
    OperationStatus os = spDevice->ReadBlock(region.AhbRegionBegin, regionSizeDwords, regionData);

    if (!os)
    {
        LOG_ERROR << "Error reading device memory region: " << region.RegionName << " Error: " << os.GetStatusMessage() << endl;
        memoryDump.AddContentSection(region.RegionName, region.LinkerRegionBegin, region.LinkerRegionEnd, region.AhbRegionBegin,"", os.GetStatusMessage());
        return os;
    }

    unsigned char* content = reinterpret_cast<unsigned char*>(regionData.data());

    string data = Utils::Base64Encode(content, regionData.size()*4);
    memoryDump.AddContentSection(region.RegionName, region.LinkerRegionBegin, region.LinkerRegionEnd, region.AhbRegionBegin, data);
    return OperationStatus();
}

DumpBuilder::DumpBuilder(const std::string& dumpName, const std::string& deviceName, const BasebandRevision baseBand)
{
    m_dump["DumpName"] = dumpName;
    m_dump["Device"] = deviceName;
    m_dump["BaseBand"] = baseBand;
}

void DumpBuilder::AddContentSection(const std::string& regionName, const uint32_t linkerRegionBegin, const uint32_t linkerRegionEnd, const uint32_t ahbRegionBegin,
    const std::string& regionContent, const std::string& regionError)
{
    Json::Value dataSection;
    dataSection["Name"] = regionName;
    dataSection["LinkerStart"] = linkerRegionBegin;
    dataSection["LinkerEnd"] = linkerRegionEnd;
    dataSection["AhbStart"] = ahbRegionBegin;
    dataSection["SizeKB"] = (linkerRegionEnd - linkerRegionBegin) / 1024;
    dataSection["Error"] = regionError;
    dataSection["Data"] = regionContent;
    m_dump["Content"].append(dataSection);
}

void DumpBuilder::AddSkippedSection(const std::string& regionName, const uint32_t linkerRegionBegin, const uint32_t linkerRegionEnd, const uint32_t ahbRegionBegin)
{
    Json::Value skippedSection;
    skippedSection["Name"] = regionName;
    skippedSection["LinkerStart"] = linkerRegionBegin;
    skippedSection["LinkerEnd"] = linkerRegionEnd;
    skippedSection["AhbStart"] = ahbRegionBegin;
    skippedSection["SizeKB"] = (linkerRegionEnd - linkerRegionBegin) / 1024;
    m_dump["SkippedSections"].append(skippedSection);
}

void DumpBuilder::AddNumberOfErrors(int number)
{
    m_dump["Errors"] = number;
}

const Json::Value& DumpBuilder::GetDump()
{
    return m_dump;
}