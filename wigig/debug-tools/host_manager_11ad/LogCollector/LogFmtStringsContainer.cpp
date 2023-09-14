/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogFmtStringsContainer.h"
#include "DebugLogger.h"

#include <fstream>
#include <cerrno>
#include <cstring>
#include <sstream>

using namespace std;

OperationStatus LogFmtStringsContainer::Reset(const std::string& stringConversionFileName)
{
    OperationStatus os = ReadStringConversionFile(stringConversionFileName);
    if (!os)
    {
        m_valid = false;
        return os;
    }

    m_valid = true;
    ReadModuleNames();

    return OperationStatus();
}

OperationStatus  LogFmtStringsContainer::ReadStringConversionFile(const string& fileName)
{
    std::ifstream stringsFile(fileName);
    if (!stringsFile.is_open())
    {
        // could not open file
        ostringstream errorBuilder;
        errorBuilder << "Could not open conversion strings file: " << fileName << ", Error: " << strerror(errno);
        LOG_ERROR << errorBuilder.str() << std::endl;
        return OperationStatus(false, errorBuilder.str());
    }

    stringsFile.seekg(0, stringsFile.end);
    std::streampos size = stringsFile.tellg();
    const auto stringConversionBufferSizeBytes = static_cast<size_t>(size);
    if (stringConversionBufferSizeBytes == 0)
    {
        // empty file
        ostringstream errorBuilder;
        errorBuilder << "Empty conversion strings file: " << fileName << std::endl;
        LOG_ERROR << errorBuilder.str();
        return OperationStatus(false, errorBuilder.str());
    }

    m_stringConversionBuffer.resize(stringConversionBufferSizeBytes);
    stringsFile.seekg(0, stringsFile.beg);
    stringsFile.read(m_stringConversionBuffer.data(), size);
    stringsFile.close();

    // create bit map where true bit represents beginning of fmt string
    m_fmtStringOffsetBitMap.resize(stringConversionBufferSizeBytes);

    uint32_t currentIndex = 0;
    bool nullState = true;
    while (currentIndex < stringConversionBufferSizeBytes)
    {
        if (m_stringConversionBuffer[currentIndex] != '\0')
        {
            if (nullState) // first not null appearance - beginning of fmt string
            {
                m_fmtStringOffsetBitMap[currentIndex] = true;
                nullState = false;
            }
            // else - still looking at fmt string, nothing to do
        }
        else
        {
            // null character appearance
            nullState = true;
        }
        ++currentIndex;
    }

    // if we got here, we finished iterating over string conversion buffer
    // and we want to be sure that it is terminated with null character
    if (!nullState)
    {
        ostringstream errorBuilder;
        errorBuilder << "Truncated string in conversion file: " << fileName << endl;
        LOG_ERROR << errorBuilder.str();
        return OperationStatus(false, errorBuilder.str());
    }

    return OperationStatus();
}

const std::string& LogFmtStringsContainer::GetModuleNameByIndex(unsigned index) const
{
    if (index >= MaxModules)
    {
        return m_moduleNames[MaxModules];
    }
    return m_moduleNames[index];
}

const char *LogFmtStringsContainer::GetNextModule(const char *mod) const
{
    mod = strchr(mod, '\0') + 1;
    while (*mod == '\0')
    {
        mod++;
    }
    return mod;
}

void LogFmtStringsContainer::ReadModuleNames()
{
    const char* mod = m_stringConversionBuffer.data();
    for (size_t i = 0; i < MaxModules; i++, mod = GetNextModule(mod))
    {
        std::stringstream ssModuleName;
        ssModuleName << std::left;
        ssModuleName << "[" << std::setw(9) << mod << "]";
        m_moduleNames[i] = ssModuleName.str();
    }
    m_moduleNames[MaxModules] = "[INVALID MODULE INDEX]";
}

std::ostream& operator<<(std::ostream &output, const LogFmtStringsContainer& fmtStringsContainer)
{
    for (size_t i = 0; i < fmtStringsContainer.m_fmtStringOffsetBitMap.size(); ++i)
    {
        if (fmtStringsContainer.m_fmtStringOffsetBitMap[i])
        {
            output << i << ": " << fmtStringsContainer.m_stringConversionBuffer.data() + i << endl;
        }
    }
    return output;
}