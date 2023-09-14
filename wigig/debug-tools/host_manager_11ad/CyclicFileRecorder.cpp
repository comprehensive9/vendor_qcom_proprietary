/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#include "CyclicFileRecorder.h"
#include "DebugLogger.h"
#include "Utils.h"
#include "FileSystemOsAbstraction.h"

#ifdef __linux
    #include <cerrno>
    #include <cstring>
    #include <unistd.h>
#elif _WINDOWS
    #include <windows.h>
#endif
#include <cstdio>
#include <iostream>
#include <utility>
#include <algorithm>
#include <sstream>

using namespace std;

//Logs from previous runs may stay, not be handled and use needed space
//OldFileHandler() looking for those old file and add them the current log queue for managing them
void CyclicFileRecorder::OldFileHandler()
{
    ostringstream ss1;
    ss1 << m_tracePrefix << "Looking for old files in " << m_filesFolder;
    LogDebugTrace(ss1.str());

    ostringstream ss2;
    vector<string> oldFilesVector;
    const OperationStatus os = FileSystemOsAbstraction::GetFilesByPattern(m_filesFolder, m_filePrefix, m_fileSuffix, oldFilesVector);
    if (os)
    {
        if (!oldFilesVector.empty())
        {
            ss2 << m_tracePrefix << oldFilesVector.size() <<" old files found";
            LogDebugTrace(ss2.str());
        }
        else
        {
            ss2 << m_tracePrefix << "Old files not found";
            LogDebugTrace(ss2.str());
            return;
        }
    }
    else
    {
        ss2 << m_tracePrefix << os.GetStatusMessage();
        LogWarningTrace(ss2.str());
        return;
    }

    sort(oldFilesVector.begin(), oldFilesVector.end());
    for (const auto& oldFile : oldFilesVector)
    {
        m_fileQueue.push(oldFile);
        RemoveOldFilesIfNeeded();
    }
}

OperationStatus CyclicFileRecorder::CyclicFileRecordersSetup()
{
    OperationStatus os = FileSystemOsAbstraction::CreateFolder(m_filesFolder);
    if (!os)
    {
        ostringstream ss;
        ss << m_tracePrefix << os.GetStatusMessage();
        LogErrorTrace(ss.str());
        m_traceRecorderFailure = true;
        return os;
    }

    OldFileHandler();
    return OperationStatus();
}

CyclicFileRecorder::CyclicFileRecorder(
    bool autoSplit, function<void(string&)> fileClosingTrigger,
    string filesFolder, string filePrefix, string fileSuffix, string tracePrefix,
    int maxSingleFileSizeInMb, int maxNumOfFiles, bool printTraceOnlyToCout)
    : CyclicFileRecorder(autoSplit, move(filesFolder), move(filePrefix), move(fileSuffix), move(tracePrefix), maxSingleFileSizeInMb, maxNumOfFiles, printTraceOnlyToCout)
{
    m_fileClosingTrigger = move(fileClosingTrigger);
}

CyclicFileRecorder::CyclicFileRecorder(
    bool autoSplit, string filesFolder, string filePrefix, string fileSuffix, string tracePrefix, int maxSingleFileSizeInMb, int maxNumOfFiles, bool printTraceOnlyToCout)
    : m_autoSplit(autoSplit)
    , m_printTraceOnlyToCout(printTraceOnlyToCout)
    , m_outputFile(nullptr)
    , m_filePrefix(move(filePrefix))
    , m_fileSuffix(move(fileSuffix))
    , m_filesFolder(move(filesFolder))
    , m_tracePrefix(move(tracePrefix))
    , m_maxSingleFileSizeInByte(maxSingleFileSizeInMb * 1024 * 1024)
    , m_maxNumOfFiles(maxNumOfFiles)
    , m_traceRecorderFailure(false)
    , m_paused(false)
    , m_totalContextBytes(0)
    , m_totalFilesNumber(0)
{
    CyclicFileRecordersSetup();
}

CyclicFileRecorder::~CyclicFileRecorder()
{
    SplitOutputFile();
}

void CyclicFileRecorder::RemoveOldFilesIfNeeded()
{
    if (m_fileQueue.size() > m_maxNumOfFiles)
    {
        ostringstream ss;
        ss << m_tracePrefix << "Deleting old file: " << m_fileQueue.front();
        LogInfoTrace(ss.str());
        const OperationStatus os = FileSystemOsAbstraction::RemoveFile(m_fileQueue.front());
        if (!os)
        {
            ostringstream ss1;
            ss1 << m_tracePrefix << os.GetStatusMessage();
            LogErrorTrace(ss1.str());
            return;
        }

        // delete file from current session queue only if file was removed, i.e. it is m_fileQueue.front()
        if (!m_currentSessionFileQueue.empty() && m_fileQueue.front() == m_currentSessionFileQueue.front())
        {
            m_currentSessionFileQueue.pop();
        }
        m_fileQueue.pop();
    }
}

OperationStatus CyclicFileRecorder::CreateNewOutputFileUnsafe()
{
    m_fullOutputFileName = m_filesFolder + m_filePrefix + "_" + Utils::GetCurrentLocalTimeForFileName() + m_fileSuffix;

    ostringstream ss;
    ss << m_tracePrefix << "Creating output file: " << m_fullOutputFileName;
    LogDebugTrace(ss.str());

    if ((m_outputFile = fopen(m_fullOutputFileName.c_str(), "w")) == nullptr)
    {
        return OperationStatus(false, m_tracePrefix + "Error opening output file: " + m_fullOutputFileName + " Error: " + strerror(errno));
    }

    m_fileQueue.push(m_fullOutputFileName);
    m_currentSessionFileQueue.push(m_fullOutputFileName);
    m_totalFilesNumber++;

    if (!m_fileHeader.empty())
    {
        if (fwrite(m_fileHeader.c_str(), sizeof(char), m_fileHeader.size(), m_outputFile) != m_fileHeader.size())
        {
            m_traceRecorderFailure = true;
            return OperationStatus(false, m_tracePrefix + "Failed to write file header to the new file");
        }
        m_currentOutputFileSize += m_fileHeader.size();
    }
    return OperationStatus(true);
}

OperationStatus CyclicFileRecorder::WriteToOutputFile(const string& logLine, bool doFlush)
{
    return WriteToOutputFile(logLine.c_str(), logLine.size(), doFlush);
}

OperationStatus CyclicFileRecorder::WriteToOutputFile(const char *pData, size_t length, bool doFlush)
{
    lock_guard<mutex> lock(m_currentFileMutex);

    if (m_traceRecorderFailure)
    {
        return OperationStatus(false, "File recorder is in failure state");
    }

    if (m_paused)
    {
        // Consumer should not enter an error state
        return OperationStatus(true);
    }

    if (m_autoSplit ||      // Split is managed internally and checked for each read
        !m_outputFile)      // This is the first write after resuming streaming
    {
        const OperationStatus os = ResetFileIfNeededUnsafe();
        if (!os)
        {
            m_traceRecorderFailure = true;
            ostringstream ss;
            ss << m_tracePrefix << "Failed to reset log file: " << os.GetStatusMessage();
            LogErrorTrace(ss.str());
            return OperationStatus(false, ss.str());
        }
    }

    m_perfCounter.Start();

    if (fwrite(pData, sizeof(char), length, m_outputFile) != length)
    {
        m_traceRecorderFailure = true;
        ostringstream ss;
        ss << m_tracePrefix << "Failed to write to log file";
        LogErrorTrace(ss.str());
        return OperationStatus(false, ss.str());
    }
    m_perfCounter.Stop();

    m_currentOutputFileSize += length;
    if (doFlush)
    {
        FlushToFile();
    }
    return OperationStatus(true);
}

void CyclicFileRecorder::SplitOutputFile()
{
    lock_guard<mutex> lock(m_currentFileMutex);
    CloseFileUnsafe();
}

void CyclicFileRecorder::CloseFileUnsafe()
{
    if (m_outputFile)
    {
        fclose(m_outputFile);
        m_outputFile = nullptr;
        ostringstream ss;
        ss << m_tracePrefix << "Closed output file " << m_fullOutputFileName;
        LogInfoTrace(ss.str());
    }
    else
    {
        return;
    }

    m_totalContextBytes += m_currentOutputFileSize;
    m_currentOutputFileSize = 0;

    if (m_fileClosingTrigger)
    {
        m_fileClosingTrigger(m_fullOutputFileName);
    }
}

OperationStatus CyclicFileRecorder::ResetFileIfNeeded()
{
    lock_guard<mutex> lock(m_currentFileMutex);
    return ResetFileIfNeededUnsafe();
}

OperationStatus CyclicFileRecorder::SetOutputFilesFolder(string path)
{
    lock_guard<mutex> lock(m_currentFileMutex);
    CloseFileUnsafe();
    m_fileQueue = queue<string>(); // clearing the Q
    m_currentSessionFileQueue = queue<string>(); // clearing the Q
    m_totalContextBytes = 0;
    m_totalFilesNumber = 0;
    m_filesFolder = move(path);
    return CyclicFileRecordersSetup();
}

std::string CyclicFileRecorder::GetOutputFilesFolder() const
{
    lock_guard<mutex> lock(m_currentFileMutex);
    return m_filesFolder;
}

std::queue<std::string> CyclicFileRecorder::GetFiles() const
{
    lock_guard<mutex> lock(m_currentFileMutex);
    return m_currentSessionFileQueue;
}

void CyclicFileRecorder::Pause()
{
    lock_guard<mutex> lock(m_currentFileMutex);
    m_paused = true;
    CloseFileUnsafe();
}

void CyclicFileRecorder::Resume()
{
    lock_guard<mutex> lock(m_currentFileMutex);
    m_paused = false;
}

OperationStatus CyclicFileRecorder::ResetFileIfNeededUnsafe()
{
    if (m_outputFile && m_currentOutputFileSize < m_maxSingleFileSizeInByte)
    {
        return OperationStatus(true);
    }

    if (m_outputFile)
    {
        CloseFileUnsafe();
    }

    OperationStatus os = CreateNewOutputFileUnsafe();
    if (!os)
    {
        return os;
    }

    RemoveOldFilesIfNeeded();
    return OperationStatus(true);
}

void CyclicFileRecorder::FlushToFile() const
{
    lock_guard<mutex> lock(m_currentFileMutex);
    if (m_outputFile)
    {
        fflush(m_outputFile);
    }
}

void CyclicFileRecorder::LogErrorTrace(const string &msg) const
{
    if (m_printTraceOnlyToCout)
    {
        LOG_ERROR_COUT << msg << endl;
    }
    else
    {
        LOG_ERROR << msg << endl;
    }
}

void CyclicFileRecorder::LogWarningTrace(const string &msg) const
{
    if (m_printTraceOnlyToCout)
    {
        LOG_WARNING_COUT << msg << endl;
    }
    else
    {
        LOG_WARNING << msg << endl;
    }
}

void CyclicFileRecorder::LogInfoTrace(const string &msg) const
{
    if (m_printTraceOnlyToCout)
    {
        LOG_INFO_COUT << msg << endl;
    }
    else
    {
        LOG_INFO << msg << endl;
    }
}

void CyclicFileRecorder::LogDebugTrace(const string &msg) const
{
    if (m_printTraceOnlyToCout)
    {
        LOG_DEBUG_COUT << msg << endl;
    }
    else
    {
        LOG_DEBUG << msg << endl;
    }
}