/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <mutex>
#include <queue>
#include <string>
#include <functional>

#include "Utils.h"

class OperationStatus;


class CyclicFileRecorder
{
public:
    CyclicFileRecorder(bool autoSplit, std::function<void(std::string&)> fileClosingTrigger, std::string filesFolder,
                       std::string filePrefix, std::string fileSuffix, std::string tracePrefix,
                       int maxSingleFileSizeInMb, int maxNumOfFiles, bool printTraceOnlyToCout);

    CyclicFileRecorder(bool autoSplit, std::string filesFolder, std::string filePrefix, std::string fileSuffix,
                       std::string tracePrefix, int maxSingleFileSizeInMb, int maxNumOfFiles, bool printTraceOnlyToCout);

    ~CyclicFileRecorder();

    OperationStatus WriteToOutputFile(const char* pData, size_t length, bool doFlush = false);
    OperationStatus WriteToOutputFile(const std::string& logLine, bool doFlush = false);
    void FlushToFile() const;

    void SplitOutputFile();
    void SetFilesHeader(const std::string & newHeader) { m_fileHeader = newHeader; }
    OperationStatus ResetFileIfNeeded();
    OperationStatus SetOutputFilesFolder(std::string path);
    std::string GetOutputFilesFolder() const;

    void Pause();
    void Resume();

    void ResetPerfCounter() { m_perfCounter.Reset(); }
    const Timer& GetPerfCounter() const { return m_perfCounter; }
    std::queue<std::string> GetFiles() const;
    double GetTotalContentMB() const { return m_totalContextBytes + m_currentOutputFileSize; }
    uint32_t GetTotalFilesNumber() const { return m_totalFilesNumber; }

private:
    void OldFileHandler();
    OperationStatus CyclicFileRecordersSetup();
    OperationStatus ResetFileIfNeededUnsafe();
    OperationStatus CreateNewOutputFileUnsafe();
    void RemoveOldFilesIfNeeded();
    void CloseFileUnsafe();
    void LogErrorTrace(const std::string& msg) const;
    void LogWarningTrace(const std::string& msg) const;
    void LogInfoTrace(const std::string& msg) const;
    void LogDebugTrace(const std::string &msg) const;

    const bool m_autoSplit;     // If set, file split will be controlled automatically, otherwise it's a caller's responsibility

    bool m_printTraceOnlyToCout;

    std::function<void(std::string&)> m_fileClosingTrigger;
    FILE* m_outputFile;
    const std::string m_filePrefix;
    std::string m_fullOutputFileName;
    const std::string m_fileSuffix;
    std::string m_filesFolder;
    const std::string m_tracePrefix;
    std::string m_fileHeader;
    size_t m_currentOutputFileSize = 0;
    size_t m_maxSingleFileSizeInByte;

    //all log files
    uint32_t m_maxNumOfFiles;
    std::queue<std::string> m_fileQueue;

    bool m_traceRecorderFailure;
    mutable std::mutex m_currentFileMutex;
    bool m_paused;

    Timer m_perfCounter;
    double m_totalContextBytes;
    uint32_t m_totalFilesNumber;
    std::queue<std::string> m_currentSessionFileQueue; // old (by pattern) files not included
};
