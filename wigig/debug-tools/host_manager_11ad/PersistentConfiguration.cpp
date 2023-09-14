/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <fstream>
#include <string>
#include <cerrno>
#include <cstring>

#include "PersistentConfiguration.h"
#include "FileSystemOsAbstraction.h"

// Place holder to allow different PMC ring parameters per OS
//should check __ANDROID__ and _OPENWRT_ first since thy also set __LINUX flag
#ifdef __ANDROID__
    #define HOST_PMC_RING_DESCRIPTORS_COUNT   1024
    #define HOST_PMC_RING_PAYLOAD_SIZE_BYTES  2048
#elif _OPENWRT_
    #define HOST_PMC_RING_DESCRIPTORS_COUNT   1024
    #define HOST_PMC_RING_PAYLOAD_SIZE_BYTES  2048
#else
    // __linux, _WINDOWS, OS3
    #define HOST_PMC_RING_DESCRIPTORS_COUNT   1024
    #define HOST_PMC_RING_PAYLOAD_SIZE_BYTES  2048
#endif

// *************************************************************************************************

JsonTree::JsonTree(const char* szName)
    : m_name(szName ? szName : "")
{
}

void JsonTree::Import(const Json::Value& jsonHostingContainer)
{
    const Json::Value& jsonContainer = m_name.empty() ? jsonHostingContainer : jsonHostingContainer[m_name];

    LOG_VERBOSE << "Importing json container"
        << " Name: " << (m_name.empty() ? "<empty>" : m_name.c_str())
        << " Content:\n" << jsonContainer
        << std::endl;

    if (Json::Value::nullSingleton() == jsonContainer)
    {
        LOG_WARNING << "No json container " << m_name << " found during json import" << std::endl;
        return;
    }

    for (auto& pParameter : m_children)
    {
        pParameter->Import(jsonContainer);
    }
}

void JsonTree::Export(Json::Value& jsonHostingContainer) const
{
    Json::Value jsonContainer;

    for (auto& pParameter : m_children)
    {
        pParameter->Export(jsonContainer);
    }

    if (m_name.empty())
    {
        jsonHostingContainer = jsonContainer;
    }
    else
    {
        jsonHostingContainer[m_name] = jsonContainer;
    }

    LOG_VERBOSE << "Exported json container"
        << " Name: " << m_name
        << " Content: \n" << jsonContainer
        << std::endl;
}

void JsonTree::Reset()
{
    for (auto& pParameter : m_children)
    {
        pParameter->Reset();
    }
}

void JsonTree::AddChildNode(IJsonSerializable* pJsonValueMap)
{
    m_children.emplace_back(pJsonValueMap);
}

// *************************************************************************************************

ConfigurationSection::ConfigurationSection(const char* szName, JsonTree& configTreeRoot)
    : JsonTree(szName)
{
    configTreeRoot.AddChildNode(this);
}

// *************************************************************************************************

HostConfigurationSection::HostConfigurationSection(JsonTree& configTreeRoot)
    : ConfigurationSection("Host", configTreeRoot)
    , StringCommandPort("StringCommandPort", 12348, *this)
    , JsonCommandPort("JsonCommandPort", 12346, *this)
    , EventPort("EventPort", 12339, *this)
    , HttpPort("HttpPort", 3000, *this)
    , KeepAliveIntervalSec("KeepAliveIntervalSec", 1, *this)
    , PeriodicEnumeration("PeriodicEnumeration", true, *this)
    , Ignore3ppFwEventIds("Ignore3ppFwEventIds", "0x1A0A,0x1A0B", *this)
{
}

// *************************************************************************************************

LogConfigurationSection::LogConfigurationSection(JsonTree& configTreeRoot)
    : ConfigurationSection("LogCollector", configTreeRoot)
    , PollingIntervalMs("PollingIntervalMs", 10, *this)
    , TargetLocation("TargetLocation", FileSystemOsAbstraction::GetOutputFilesLocation() + "Logs" + FileSystemOsAbstraction::GetDirectoryDelimiter(), *this)
    , ConversionFileLocation("ConversionFileLocation", FileSystemOsAbstraction::GetDefaultLogConversionFilesLocation(), *this)
    , MaxLogFileSize("MaxLogFileSize", 10, *this)
    , MaxNumOfLogFiles("MaxNumOfLogFiles", 10, *this)
    , FlushChunk("FlushChunk", false, *this)
    , TargetServer("TargetServer", "Not-Configured", *this)
    , UserName("UserName", "Not-Configured", *this)
    , RemotePath("RemotePath", "Not-Configured", *this)
    , CompressionApp("CompressionApp", "gzip", *this)
    , VerifyCompressionApp("VerifyCompressionApp", true, *this)
{
}

// *************************************************************************************************

PmcConfigurationSection::PmcConfigurationSection(JsonTree& configTreeRoot)
    : ConfigurationSection("Pmc", configTreeRoot)
    , NormalizedTargetLocation("NormalizedTargetLocation", FileSystemOsAbstraction::GetDefaultPmcFilesLocation(), *this)
    , HostPmcRingDescriptorsCount("HostPmcRingDescriptorsCount", HOST_PMC_RING_DESCRIPTORS_COUNT, *this)
    , HostPmcRingPayloadSizeBytes("HostPmcRingPayloadSizeBytes", HOST_PMC_RING_PAYLOAD_SIZE_BYTES, *this)
    , MaxDataFileSize("MaxLogFileSizeDebug", 1, *this)
    , MaxNumOfDataFiles("MaxNumOfLogFilesDebug", 3, *this)
    , DumpRawDataForDebug("DumpRawDataForDebug", false, *this)
{
}

// *************************************************************************************************

JtagConfigurationSection::JtagConfigurationSection(JsonTree& configTreeRoot)
    : ConfigurationSection("Jtag", configTreeRoot)
    , ClockFrequency("ClockFrequency", 10000000, *this)
    , JtagDevice("JtagDevice", "Talyn", *this)
    , ContinuousPmcSupported("ContinuousPmcSupported", false, *this)
{
}

// *************************************************************************************************

AccessConfigurationSection::AccessConfigurationSection(JsonTree& configTreeRoot)
    : ConfigurationSection("Access", configTreeRoot)
    , DefaultDumpLocation("DefaultDumpLocation", FileSystemOsAbstraction::GetOutputFilesLocation() + "Dump" + FileSystemOsAbstraction::GetDirectoryDelimiter(), *this)
{
}

// *************************************************************************************************

PersistentConfiguration::PersistentConfiguration(const std::string& filePath)
    : JsonTree("")
    , HostConfiguration(*this)
    , LogCollectorConfiguration(*this)
    , PmcConfiguration(*this)
    , JtagConfiguration(*this)
    , AccessConfiguration(*this)
    , m_filePath(BuildConfigFilePath(filePath))
{
    Load(); // Overwrite default configuration with persistent configuration
    Save(); // Align persistent configuration with the up-to-date content (remove redundant/add new fields)

    CleanupOldFiles();
}

PersistentConfiguration::~PersistentConfiguration()
{
    Save();
}

void PersistentConfiguration::CommitChanges()
{
    Save();
}

void PersistentConfiguration::Load()
{
    Json::Reader jsonReader;

    std::ifstream configFile(m_filePath);
    if (!configFile && ENOENT == errno)
    {
        // Configuration file does not exist
        LOG_INFO << "Configuration file does not exist at " << m_filePath << std::endl;
        return;
    }

    if (!configFile)
    {
        // Configuration will hold default hard-coded values
        LOG_ERROR << "Failed to load configuration file."
            << " Path: " << m_filePath
            << " Error: " << strerror(errno) << std::endl;
        return;
    }

    if (!jsonReader.parse(configFile, m_configRoot, true))
    {
        LOG_ERROR << "Failed to parse configuration file."
            << " Path: " << m_filePath
            << " Error: " << jsonReader.getFormattedErrorMessages()
            << std::endl;
        return;
    }

    LOG_INFO << "Loaded persistent configuration from: " << m_filePath << std::endl;
    LOG_VERBOSE << "Persistent configuration file content:\n" << m_configRoot << std::endl;

    Import(m_configRoot);
}

void PersistentConfiguration::Save()
{
    const std::string configFolder = FileSystemOsAbstraction::GetConfigurationFilesLocation();
    if (!FileSystemOsAbstraction::DoesFolderExist(configFolder))
    {
        const OperationStatus os = FileSystemOsAbstraction::CreateFolder(configFolder);
        if (!os)
        {
            LOG_ERROR << os.GetStatusMessage();
        }
    }



    // Create a configuration directory if does not exist. std::ofstream fails if no folder exists.
    std::ofstream configFile(m_filePath);
    if (!configFile)
    {
        // Configuration will hold default hard-coded values
        LOG_ERROR << "Failed to export configuration to file."
            << " Path: " << m_filePath
            << " Error: " << strerror(errno) << std::endl;
        return;
    }

    m_configRoot.clear();
    Export(m_configRoot);

    Json::StyledStreamWriter writer;
    writer.write(configFile, m_configRoot);

    LOG_INFO << "Saved configuration file at " << m_filePath << std::endl;
    //LOG_VERBOSE << "Persistent configuration file content:\n" << m_configRoot << std::endl;
}

std::string PersistentConfiguration::BuildConfigFilePath(const std::string& filePath)
{
    if (filePath.empty())
    {
        std::stringstream configFileBuilder;
        configFileBuilder << FileSystemOsAbstraction::GetConfigurationFilesLocation() << "host_manager_11ad.config";
        return configFileBuilder.str();
    }

    return filePath;
}

void PersistentConfiguration::CleanupOldFiles()
{
    std::stringstream ss;
    ss << FileSystemOsAbstraction::GetConfigurationFilesLocation() << "host_alias";
    const std::string oldAliasFile(ss.str());

    if (!FileSystemOsAbstraction::DoesFileExist(oldAliasFile))
    {
        LOG_DEBUG << "Backward compatibility cleanup: No old alias file found at: " << oldAliasFile << std::endl;
        return;
    }

    const OperationStatus os = FileSystemOsAbstraction::RemoveFile(oldAliasFile);
    if (os)
    {
        LOG_INFO << "Backward compatibility cleanup: Deleted old alias file at: " << oldAliasFile << std::endl;
    }
    else
    {
        LOG_ERROR << "Backward compatibility cleanup: " << os.GetStatusMessage();
    }
}
