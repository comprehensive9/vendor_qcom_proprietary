/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "OperationStatus.h"

#include <string>
#include <vector>

class Device;

// This class is responsible for creating PMC recording manifest for the given device and location
class ManifestCreator
{
public:
    static OperationStatus CreateManifestFile(const Device& device, const std::string& hostName,
        const std::string& pmcDataPath, const std::string& baseName);

private:
    static std::string GetManifestXmlString(const Device& device, const std::string& hostName,
        uint32_t swHead, uint32_t numDescriptors, uint32_t payloadSizeBytes,
        const std::string& baseName, bool isContinuousPmc);
    static const char* GetBasebandTypeString(const Device& device);
};

// This class is responsible for creating PMC recording session folder
// Provides services of basename formatting and host name extraction
class PmcRecordingPathBuilder
{
public:
    static OperationStatus CreateFinalPmcFileLocation(const std::string& deviceName, const std::string& originalPmcFilePath,
        std::string& newPmcDataPath, const std::string& baseName = "");
    static std::string FormatBaseName(const std::string& deviceName, const std::string& hostName = "");
    static std::string GetHostName();
};

// This class is responsible for creating the PMC data file and manifest file in a location which will be then used as an output for the user
class PmcRecordingFileCreator
{
    public:
        explicit PmcRecordingFileCreator(const Device& m_device);

        OperationStatus CreatePmcFiles(std::string& pmcFilesPath) const;

    private:
        OperationStatus CreatePmcDataFiles(std::string& pmcFilesPath) const;

        const Device& m_device;
        const std::string m_hostName;
        const std::string m_baseName;
        uint32_t m_refNum;
};

// This class is responsible for extraction of the PMC data from given recording folder
class PmcRecordingExtractor
{
public:
    PmcRecordingExtractor(const Device& device, const std::string& targetLocation, bool onTargetClient);

    OperationStatus FinalizePmcRecordingSession(std::vector<uint8_t>& pmcData) const;

    static std::string s_dataFileNamePattern;
    static std::string s_dataFileExtension;

private:
    OperationStatus GetSortedFiles(std::vector<std::string>& pmcDataFileNames) const;
    OperationStatus ExtractPmcData(std::vector<uint8_t>& pmcData) const;
    OperationStatus MergePmcData() const;

    const Device& m_device;
    const std::string m_hostName;
    const std::string m_targetLocation;
    const bool m_onTargetClient;
};
