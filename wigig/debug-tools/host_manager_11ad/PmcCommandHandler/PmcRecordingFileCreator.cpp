/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PmcRecordingFileCreator.h"
#include "DeviceManager.h"
#include "Device.h"
#include "PmcSequenceBase.h"
#include "PmcContext.h"
#include "FileSystemOsAbstraction.h"
#include "ShellCommandExecutor.h"
#include "Utils.h"

#include <cstdlib>     // srand, rand
#include <ctime>       // time
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>

#ifdef _WINDOWS
    #include <winsock2.h>       // For gethostname
    #define HOST_NAME_MAX 64
#else
    #include <unistd.h>         // For gethostname
    #include <limits.h>         // For HOST_NAME_MAX
#endif

using namespace std;

// ===========================================================================================//
OperationStatus ManifestCreator::CreateManifestFile(const Device& device, const std::string& hostName,
    const std::string& pmcDataPath, const std::string& baseName)
{
    uint32_t swHead = 0U;
    const bool isContinuousPmc = device.IsContinuousPmcSupported();
    if (!isContinuousPmc)
    {
        const auto res = device.GetPmcContext().GetSequence().ReadPmcHead(swHead);
        if (!res.IsSuccess())
        {
            return OperationStatus(false, "Failed reading SW head");
        }
    }

    const auto pRingSpec = device.GetPmcContext().GetSequence().GetPmcRingSpec();
    if (!pRingSpec) // shouldn't happen
    {
        return OperationStatus(false, "Unknown PMC Ring spec.");
    }
    const uint32_t numDescriptors = pRingSpec->PmcRingDescriptorsCount;
    const uint32_t payloadSizeBytes = pRingSpec->PmcRingPayloadSizeBytes;

    const std::string xmlString = GetManifestXmlString(device, hostName,
        swHead, numDescriptors, payloadSizeBytes, baseName, isContinuousPmc);

    std::ostringstream oss;
    oss << pmcDataPath << FileSystemOsAbstraction::GetDirectoryDelimiter() << baseName << ".manifest";
    std::ofstream out(oss.str());
    if ( !(out << xmlString) )
    {
        return OperationStatus(false, "Failed writing manifest file");
    }
    out.close(); // not strictly needed, called by destructor
    return OperationStatus();
}

std::string ManifestCreator::GetManifestXmlString(const Device& device, const std::string& hostName,
    uint32_t swHead, uint32_t numDescriptors, uint32_t payloadSizeBytes,
    const std::string& baseName, bool isContinuousPmc)
{
    const std::string& endRecordingTime = device.GetPmcContext().GetDeviceRecordingState().GetStopRecordingTime();
    const std::string& startRecordingTime = device.GetPmcContext().GetDeviceRecordingState().GetStartRecordingTime();
    const char* XML_INDENT = "    ";

    // Note: DataContractSerializer, used on DmTools side, expects all fields to be ordered alphabetically!
    std::ostringstream oss;
    oss << R"(<?xml version="1.0" encoding="utf-8"?>)" << std::endl
        << R"(<PmcRawDataSet xmlns:i="http://www.w3.org/2001/XMLSchema-instance" xmlns="http://schemas.datacontract.org/2004/07/DmTools.Actors">)"
        << std::endl
        << XML_INDENT << "<BaseName>" << baseName << "</BaseName>" << std::endl
        << XML_INDENT << "<BasebandRevision>" << device.GetBasebandRevision() << "</BasebandRevision>" << std::endl
        << XML_INDENT << "<BasebandType>" << GetBasebandTypeString(device) << "</BasebandType>" << std::endl
        << XML_INDENT << "<ContinuousPmc>" << (isContinuousPmc ? "true" : "false") << "</ContinuousPmc>" << std::endl // do not use formatter to control lower case
        << XML_INDENT << "<DescriptorSize>" << payloadSizeBytes << "</DescriptorSize>" << std::endl
        << XML_INDENT << "<DeviceName>" << device.GetDeviceName() << "</DeviceName>" << std::endl
        << XML_INDENT << "<EndRecordingTime>" << endRecordingTime << "</EndRecordingTime>" << std::endl
        << XML_INDENT << "<HostName>" << hostName << "</HostName>" << std::endl
        << XML_INDENT << "<NumberOfDescriptors>" << numDescriptors << "</NumberOfDescriptors>" << std::endl
        << XML_INDENT << "<StartRecordingTime>" << startRecordingTime << "</StartRecordingTime>" << std::endl
        << XML_INDENT << "<SwHead>" << swHead << "</SwHead>" << std::endl
        << "</PmcRawDataSet>" << std::endl;
    return oss.str();
}

const char* ManifestCreator::GetBasebandTypeString(const Device& device)
{
    if (device.GetBasebandType() == BasebandType::BASEBAND_TYPE_TALYN)
    {
        return "TALYN";
    }
    if (device.GetBasebandType() == BasebandType::BASEBAND_TYPE_SPARROW)
    {
        return "SPARROW";
    }
    return "UNKNOWN";
}

// ===========================================================================================//
OperationStatus PmcRecordingPathBuilder::CreateFinalPmcFileLocation(const std::string& deviceName, const std::string& originalPmcFilePath,
    std::string& newPmcDataPath, const std::string& baseName /* = "" */)
{
    const std::string parentFolder = originalPmcFilePath.substr(0, originalPmcFilePath.find_last_of("/\\"));
    ostringstream oss;
    oss << parentFolder << FileSystemOsAbstraction::GetDirectoryDelimiter();
    oss << (baseName.empty() ? FormatBaseName(deviceName) : baseName);
    oss << "_" << Utils::GetCurrentLocalTimeForFileName() << FileSystemOsAbstraction::GetDirectoryDelimiter(); // add suffix and trailing slash
    newPmcDataPath = oss.str();

    if (FileSystemOsAbstraction::DoesFolderExist(newPmcDataPath))
    {
        std::ostringstream msgBuilder;
        msgBuilder << "Failed creating a new directory for pmc data files, output folder already exists: " << newPmcDataPath;
        return OperationStatus(false, msgBuilder.str());
    }

    OperationStatus os = FileSystemOsAbstraction::CreateFolder(newPmcDataPath);
    if (!os)
    {
        os.AddPrefix("Failed creating a new directory for pmc data files");
        return os;
    }

    return OperationStatus();
}

std::string PmcRecordingPathBuilder::FormatBaseName(const std::string& deviceName, const std::string& hostName /* = "" */)
{
    std::ostringstream oss;
    oss << "pmc_" << (hostName.empty() ? GetHostName() : hostName) << "_" << deviceName;
    return oss.str();
}

std::string PmcRecordingPathBuilder::GetHostName()
{
    char hostName[HOST_NAME_MAX + 1];
    if (gethostname(hostName, HOST_NAME_MAX + 1) != 0)
    {
        ::SafeStringCopy(hostName, "<unknown>", HOST_NAME_MAX + 1);
        LOG_ERROR << "Failed reading host name" << std::endl;
    }
    return std::string(hostName);
}

// ===========================================================================================//
PmcRecordingFileCreator::PmcRecordingFileCreator(const Device& device)
    : m_device(device)
    , m_hostName(PmcRecordingPathBuilder::GetHostName())
    , m_baseName(PmcRecordingPathBuilder::FormatBaseName(m_device.GetDeviceName(), m_hostName))
{
    srand(static_cast<uint32_t>(time(nullptr)));
    m_refNum = rand();

    LOG_INFO << "PmcRecordingFileCreator baseName is: " << m_baseName << std::endl;
}

OperationStatus PmcRecordingFileCreator::CreatePmcDataFiles(std::string& pmcFilesPath) const
{
    OperationStatus os = m_device.CreatePmcFiles(m_refNum);
    if (!os)
    {
        return os;
    }
    std::string foundPmcFilePath;
    os = m_device.FindPmcDataFile(m_refNum, foundPmcFilePath);
    if (!os)
    {
        return os;
    }

    os = PmcRecordingPathBuilder::CreateFinalPmcFileLocation(m_device.GetDeviceName(), foundPmcFilePath, pmcFilesPath, m_baseName);
    if(!os)
    {
        return os;
    }

    std::ostringstream ssdst;
    ssdst << pmcFilesPath << FileSystemOsAbstraction::GetDirectoryDelimiter() << m_baseName << ".bin";

    // copy data file (mandatory)
    std::ifstream  src(foundPmcFilePath, std::ios::binary);
    std::ofstream  dst(ssdst.str(), std::ios::binary);
    if(!(dst << src.rdbuf()))
    {
        std::ostringstream ss;
        ss << "Failed copying pmc data file " << foundPmcFilePath << " to on-target directory " << ssdst.str();
        return OperationStatus(false, ss.str());
    }
    std::remove(foundPmcFilePath.c_str());

    // copy ring file (optional)
    os = m_device.FindPmcRingFile(m_refNum, foundPmcFilePath);
    if (!os)
    {
        return OperationStatus(true, os.GetStatusMessage()); // true since it's an optional file, and it's possible that driver doesn't support it
    }

    std::ostringstream ssRingDst;
    ssRingDst << pmcFilesPath << FileSystemOsAbstraction::GetDirectoryDelimiter() << m_baseName << ".ring";

    std::ifstream  srcRing(foundPmcFilePath, std::ios::binary);
    std::ofstream  dstRing(ssRingDst.str(), std::ios::binary);
    if (!(dstRing << srcRing.rdbuf()))
    {
        return OperationStatus(false, "Failed copying pmc ring file to on target directory");
    }
    std::remove(foundPmcFilePath.c_str());

    return OperationStatus(true);
}

OperationStatus PmcRecordingFileCreator::CreatePmcFiles(std::string& pmcFilesPath) const
{
    auto pmcFileRes = CreatePmcDataFiles(pmcFilesPath);
    if(!pmcFileRes.IsSuccess())
    {
        return pmcFileRes;
    }
    auto manifestFileRes = ManifestCreator::CreateManifestFile(m_device, m_hostName, pmcFilesPath, m_baseName);
    if(!manifestFileRes.IsSuccess())
    {
        return manifestFileRes;
    }
    return OperationStatus();
}

// ===========================================================================================//
std::string PmcRecordingExtractor::s_dataFileNamePattern = "pmc_data";
std::string PmcRecordingExtractor::s_dataFileExtension = ".bin";

PmcRecordingExtractor::PmcRecordingExtractor(const Device& device, const std::string& targetLocation, bool onTargetClient)
    : m_device(device)
    , m_hostName(PmcRecordingPathBuilder::GetHostName())
    , m_targetLocation(targetLocation)
    , m_onTargetClient(onTargetClient)
{
}

OperationStatus PmcRecordingExtractor::FinalizePmcRecordingSession(std::vector<uint8_t>& pmcData) const
{
    pmcData.clear();

    if (m_onTargetClient)
    {
        // merge to single data file, delete input files and create a metadata file
        return MergePmcData();
    }

    // sort and unify all data files to a single data vector, remove input folder
    return ExtractPmcData(pmcData);
}

OperationStatus PmcRecordingExtractor::GetSortedFiles(std::vector<std::string>& pmcDataFileNames) const
{
    pmcDataFileNames.clear();

    OperationStatus os = FileSystemOsAbstraction::GetFilesByPattern(m_targetLocation, s_dataFileNamePattern, s_dataFileExtension, pmcDataFileNames);
    if (!os)
    {
        return os;
    }

    LOG_DEBUG << "Found " << pmcDataFileNames.size() << " PMC data files in " << m_targetLocation << endl;

    // sort the files by name
    // the filename patter is identical and suffix is the timestamp
    std::sort(pmcDataFileNames.begin(), pmcDataFileNames.end());

    return os;
}

OperationStatus PmcRecordingExtractor::ExtractPmcData(std::vector<uint8_t>& pmcData) const
{
    // sort and unify all data files to a single data vector
    std::vector<std::string> pmcDataFileNames;
    auto os = GetSortedFiles(pmcDataFileNames);
    if (!os)
    {
        os.AddPrefix("Failed to extract PMC data");
        return os;
    }

    for (const auto& dataFileName : pmcDataFileNames)
    {
        std::ifstream src(dataFileName, std::ifstream::binary);
        if (!src.is_open())
        {
            std::ostringstream msgBuilder;
            msgBuilder << "Cannot open source " << dataFileName << ": " << strerror(errno);
            return OperationStatus(false, msgBuilder.str());
        }

        std::vector<uint8_t> ringData(std::istreambuf_iterator<char>{src}, {}); // implicit conversion to uint8_t
        if (!src)
        {
            // Any non-EoF failure or I/O error
            std::ostringstream msgBuilder;
            msgBuilder << "Cannot read PMC content from " << dataFileName << ": " << strerror(errno);
            return OperationStatus(false, msgBuilder.str());
        }
        src.close();
        LOG_DEBUG << "Extracted " << ringData.size() << " Bytes from PMC data file " << dataFileName << std::endl;

        pmcData.insert(pmcData.end(), ringData.begin(), ringData.end());
    }

    // post extraction action - remove the session folder
    std::string commandStr = "rm -rf " + m_targetLocation; // works for all Linux platforms
    ShellCommandExecutor command(commandStr.c_str());
    if (command.ExitStatus() != 0)
    {
        return OperationStatus(false, "Failed to remove directory for pmc data files at: " + m_targetLocation);
    }

    return OperationStatus();
}

// Note:
// Need to merge to a single file since Consumer code splits the incoming stream of PMC data events to multiple files (CyclicFileRecorder configuration).
// This is done to promise we have 2-3 MB of data upon stop of PMC data collection. If only a single file is used for recording, it's being removed
// immediately upon new file creation when reaching size limit, and thus content of the whole file is lost.
OperationStatus PmcRecordingExtractor::MergePmcData() const
{
    // merge to single data file, remove the rest
    std::vector<std::string> pmcDataFileNames;
    auto os = GetSortedFiles(pmcDataFileNames);
    if (!os)
    {
        os.AddPrefix("Failed to extract PMC data");
        return os;
    }

    if (pmcDataFileNames.size() > 1U) // otherwise, no merge needed
    {
        std::ofstream destinationFile(pmcDataFileNames.front(), std::ios_base::binary | std::ios_base::app);
        if (!destinationFile.is_open())
        {
            std::ostringstream msgBuilder;
            msgBuilder << "Cannot open source/destination " << pmcDataFileNames.front() << ": " << strerror(errno);
            return OperationStatus(false, msgBuilder.str());
        }
        destinationFile.seekp(0, std::ios_base::end); // skip to the file end

        // iterate starting from the 2nd file
        // cannot use range loop without condition, cannot use for_each since may return an error during iteration
        for (auto iter = std::next(pmcDataFileNames.begin()); iter != pmcDataFileNames.end(); ++iter)
        {
            std::ifstream src(*iter, std::ios_base::binary);
            if (!src.is_open())
            {
                std::ostringstream msgBuilder;
                msgBuilder << "Cannot open source " << *iter << ": " << strerror(errno);
                return OperationStatus(false, msgBuilder.str());
            }

            if (!(destinationFile << src.rdbuf()))
            {
                std::ostringstream ss;
                ss << "Failed to append pmc data file " << *iter << " to " << pmcDataFileNames.front();
                return OperationStatus(false, ss.str());
            }
            src.close();
            std::remove(iter->c_str()); // remove the source file, no more needed
        }
        destinationFile.close(); // not strictly needed, called by destructor
    }

    // create manifest file
    if (pmcDataFileNames.empty())
    {
        // no PMC data was recorded, create an empty file, timestamp suffix not needed
        std::ostringstream fullPath;
        fullPath << m_targetLocation << FileSystemOsAbstraction::GetDirectoryDelimiter()
            << s_dataFileNamePattern << s_dataFileExtension;

        pmcDataFileNames.push_back(fullPath.str());

        std::ofstream output(pmcDataFileNames.front());
        output.close();
    }

    std::string baseName = FileSystemOsAbstraction::ExtractFileName(pmcDataFileNames.front(), true /*remove extension*/);
    return ManifestCreator::CreateManifestFile(m_device, m_hostName, m_targetLocation, baseName);
}
