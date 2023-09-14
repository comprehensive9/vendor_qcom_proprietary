/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*
* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "PmcData.h"
#include "DebugLogger.h"
#include "OperationStatus.h"
#include "PersistentConfiguration.h"
#include "Host.h"
#include "DriverAPI.h"
#include "DriverContracts.h"

#include <cstring>
#include <cerrno>

#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>

#include <sys/stat.h>
#include <sys/types.h>

// *************************************************************************************************

PmcFileLocator::PmcFileLocator(int fileId)
    : m_Directory(Host::GetHost().GetConfiguration().PmcConfiguration.NormalizedTargetLocation)
    , m_FileId(fileId)
{
    // target location contains trailing directory delimeter
    std::stringstream ss;
    ss << m_Directory << "pmc_data_" << fileId;
    m_DataFileName = ss.str();

    ss.str(std::string());
    ss << m_Directory << "pmc_ring_" << fileId;
    m_RingFileName = ss.str();

    LOG_DEBUG << "PMC file names #" << fileId << " generated: " << m_DataFileName << ", " << m_RingFileName<< std::endl;
}

bool PmcFileLocator::FileExists(const std::string& fileName) const
{
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    struct stat st = {};
    #pragma GCC diagnostic pop
    return (stat(fileName.c_str(), &st) != -1);
}

std::ostream& operator<<(std::ostream& os, const PmcFileLocator& pmcFilesLocator)
{
    return os << "PMC file #" << pmcFilesLocator.GetFileId()
              << " (" << pmcFilesLocator.GetDataFileName() << ',' << pmcFilesLocator.GetRingFileName() << ')';
}

// *************************************************************************************************
PmcFileWriterBase::PmcFileWriterBase(int fileId)
    : m_PmcFilesLocator(fileId)
{}

OperationStatus PmcFileWriterBase::MeetWritePrerequisites() const
{
    // Forbid file overwrite

    if (m_PmcFilesLocator.DataFileExists())
    {
        std::stringstream msgBuilder;
        msgBuilder << "Destination PMC data file already exists: " << m_PmcFilesLocator.GetDataFileName();
        return OperationStatus(false, msgBuilder.str().c_str());
    }

    if (m_PmcFilesLocator.RingFileExists())
    {
        std::stringstream msgBuilder;
        msgBuilder << "Destination PMC ring file already exists: " << m_PmcFilesLocator.GetRingFileName();
        return OperationStatus(false, msgBuilder.str().c_str());
    }

    // Create a PMC directory if does not exist

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    struct stat st = {};
#pragma GCC diagnostic pop

    if (stat(m_PmcFilesLocator.GetDirectory(), &st) != -1)
    {
        LOG_DEBUG << "Found existing PMC data directory " << m_PmcFilesLocator.GetDirectory() << std::endl;
        return OperationStatus(true);
    }

    LOG_DEBUG << "Creating a PMC data directory: " << m_PmcFilesLocator.GetDirectory() << std::endl;

    int status = mkdir(m_PmcFilesLocator.GetDirectory(), S_IRWXU);
    if (0 != status)
    {
        std::stringstream msgBuilder;
        msgBuilder << "Cannot create a PMC data directory " << m_PmcFilesLocator.GetDirectory()
            << " Error: " << strerror(errno);
        return OperationStatus(false, msgBuilder.str().c_str());
    }

    return OperationStatus(true);
}

// *************************************************************************************************
PmcFileWriter::PmcFileWriter(int fileId, const std::string& debugFsPath)
    : PmcFileWriterBase(fileId)
{
    if (debugFsPath.empty())
    {
        LOG_ERROR << "No Debug FS path is provided" << std::endl;
    }

    std::ostringstream pathBuilder;
    pathBuilder << debugFsPath << '/' << "pmcdata";
    m_SrcPmcDataFilePath = pathBuilder.str();

    pathBuilder.str(std::string());
    pathBuilder << debugFsPath << '/' << "pmcring";
    m_SrcPmcRingFilePath = pathBuilder.str();

    std::ifstream src(m_SrcPmcRingFilePath, std::ifstream::binary);
    m_driverSupportsRingFlush = src.is_open();

    LOG_DEBUG << "PMC Data File Writer Created"
        << "\n    Debug FS Path: " << debugFsPath
        << "\n    Src PMC Data: " << m_SrcPmcDataFilePath
        << "\n    Dst PMC Data: " << m_PmcFilesLocator.GetDataFileName()
        << "\n    Src PMC Ring: " << m_SrcPmcRingFilePath
        << "\n    Dst PMC Ring: " << m_PmcFilesLocator.GetRingFileName()
        << "\n    Driver supports PMC Ring flush: " << m_driverSupportsRingFlush
        << std::endl;
}

OperationStatus PmcFileWriter::WriteFile(const char* srcFileName, const char* dstFileName) const
{
    // assumption: MeetWritePrerequisites was already checked

    std::ifstream src(srcFileName, std::ifstream::binary);
    if (!src.is_open())
    {
        std::ostringstream msgBuilder;
        msgBuilder << "Cannot open source " << srcFileName << ": " << strerror(errno);
        return OperationStatus(false, msgBuilder.str());
    }

    std::ofstream dst(dstFileName, std::ofstream::binary);
    if (!dst.is_open())
    {
        std::ostringstream msgBuilder;
        msgBuilder << "Cannot open destination " << dstFileName << ": " << strerror(errno);
        return OperationStatus(false, msgBuilder.str());
    }

    // Buffered copy through user space is required as pmcdata does not support offset functionality
    // and therefore its size cannot be queried. As a result, sendfile() cannot be used.

    static const size_t DATA_COPY_BUFFER_LEN = 1024 * 32;
    std::unique_ptr<char[]> spDataCopyBuffer(new char[DATA_COPY_BUFFER_LEN]);
    if (!spDataCopyBuffer)
    {
        std::ostringstream msgBuilder;
        msgBuilder << "Cannot allocate data copy buffer of " << DATA_COPY_BUFFER_LEN << " B";
        return OperationStatus(false, msgBuilder.str());
    }

    std::streamsize dataSize = 0;
    while (src && dst)
    {
        // Read a data chunk up to the buffer capacity
        src.read(spDataCopyBuffer.get(), DATA_COPY_BUFFER_LEN);
        std::streamsize chunkSize = src.gcount();
        LOG_VERBOSE << "Read chunk from " << srcFileName << ": " << chunkSize << " B" << std::endl;

        if (chunkSize > 0)
        {
            // Write the chunk
            dst.write(spDataCopyBuffer.get(), chunkSize);
            dataSize += chunkSize;
            LOG_VERBOSE << "Written PMC chunk: " << chunkSize << " Accumulated: " << dataSize << std::endl;

            if (!dst)
            {
                std::ostringstream msgBuilder;
                msgBuilder << "Cannot write to " << dstFileName << ": " << strerror(errno);
                return OperationStatus(false, msgBuilder.str());
            }
        }

        // Check stop conditions
        if (src.fail() && src.eof())
        {
            // EoF reached
            LOG_DEBUG << "Source: EoF reached" << std::endl;
            LOG_DEBUG << "Written PMC file: " << dstFileName << " Size: " << dataSize << " B" << std::endl;

            std::ostringstream msgBuilder;
            msgBuilder << dataSize;
            return OperationStatus(true, msgBuilder.str());
        }

        if (!src)
        {
            // Any non-EoF failure or I/O error
            std::ostringstream msgBuilder;
            msgBuilder << "Cannot read PMC content from " << srcFileName << ": " << strerror(errno);
            return OperationStatus(false, msgBuilder.str());
        }
    }

    // The function flow should not get here
    return OperationStatus(false, "Unknown Error");
}

OperationStatus PmcFileWriter::WriteFiles() const
{
    OperationStatus st = MeetWritePrerequisites();
    if (!st.IsSuccess())
    {
        return st;
    }

    st = WriteFile(m_SrcPmcDataFilePath.c_str(), m_PmcFilesLocator.GetDataFileName());
    if (!st.IsSuccess() || !m_driverSupportsRingFlush)
    {
        return st;
    }

    auto prevMessage = st.GetStatusMessage();
    OperationStatus st2 = WriteFile(m_SrcPmcRingFilePath.c_str(), m_PmcFilesLocator.GetRingFileName());

    if (!st2.IsSuccess())
    {
        return st2;
    }

    return OperationStatus::Merge(st, st2);
}

// *************************************************************************************************
NetlinkPmcFileWriter::NetlinkPmcFileWriter(int fileId, PmcDataProviderFunc pmcDataProviderFunc)
    : PmcFileWriterBase(fileId)
    , m_pmcDataProviderFunc(std::move(pmcDataProviderFunc))
{
    LOG_DEBUG << "Netlink PMC Data File Writer Created"
        << "\n    Dst PMC Data: " << m_PmcFilesLocator.GetDataFileName()
        << "\n    Dst PMC Ring: " << m_PmcFilesLocator.GetRingFileName()
        << "\n    Driver supports PMC Ring flush (over netlink): true"
        << std::endl;
}

OperationStatus NetlinkPmcFileWriter::WriteFile(PmcDataType dataType, const char* dstFileName) const
{
    if (!m_pmcDataProviderFunc) // shouldn't happen
    {
        return OperationStatus(false, "PMC data retrieval function is not provided for netlink PMC file writer");
    }

    std::ofstream dst(dstFileName, std::ofstream::binary);
    if (!dst.is_open())
    {
        std::ostringstream msgBuilder;
        msgBuilder << "Cannot open destination " << dstFileName << ": " << strerror(errno);
        return OperationStatus(false, msgBuilder.str());
    }

    std::vector<uint8_t> pmcData;
    auto os = m_pmcDataProviderFunc(dataType, pmcData);
    if (!os)
    {
        return os;
    }

    const auto dataSize = pmcData.size();
    dst.write(reinterpret_cast<char*>(pmcData.data()), dataSize);
    if (!dst)
    {
        std::ostringstream oss;
        oss << "Cannot write to " << dstFileName << ": " << strerror(errno);
        return OperationStatus(false, oss.str());
    }

    LOG_DEBUG << "Written PMC file: " << dstFileName << " Size: " << dataSize << " B" << std::endl;

    // return the created file size as part of the returned status
    std::ostringstream msgBuilder;
    msgBuilder << dataSize;
    return OperationStatus(true, msgBuilder.str());
}

OperationStatus NetlinkPmcFileWriter::WriteFiles() const
{
    OperationStatus st = MeetWritePrerequisites();
    if (!st.IsSuccess())
    {
        return st;
    }

    st = WriteFile(PmcDataType::PMC_RING_PAYLOAD, m_PmcFilesLocator.GetDataFileName());
    if (!st.IsSuccess())
    {
        return st;
    }

    // ring descriptrors extraction is unconditionally supported over the netlink path
    OperationStatus st2 = WriteFile(PmcDataType::PMC_RING_DESCRIPTORS, m_PmcFilesLocator.GetRingFileName());
    if (!st2.IsSuccess())
    {
        return st2;
    }

    return OperationStatus::Merge(st, st2);
}
