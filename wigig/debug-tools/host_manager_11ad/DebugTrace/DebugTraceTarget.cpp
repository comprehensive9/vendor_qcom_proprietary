/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#include "DebugTraceTarget.h"
#include "DebugLogger.h"
#include "CyclicFileRecorder.h"
#include "FileSystemOsAbstraction.h"

#include <cstdio>
#include <utility>

using namespace std;

// *************************************************************************************************

void DebugTraceTargetStdout::SubmitBuffer(const char *pData, size_t length)
{
    fwrite(pData, sizeof(char), length, stdout);
}

void DebugTraceTargetStdout::Flush() const
{
    fflush(stdout);
}

// *************************************************************************************************
DebugTraceTargetFile::DebugTraceTargetFile(
    const bool alwaysFlush, std::string debugTracePath, int maxSingleFileSizeInMb, int maxNumOfFiles)
    : m_alwaysFlush(alwaysFlush)
    , m_cyclicFileRecorder(true, std::move(debugTracePath), "host_manager", ".log", string(), maxSingleFileSizeInMb , maxNumOfFiles, true)
{}

void DebugTraceTargetFile::SubmitBuffer(const char *pData, size_t length)
{
    m_cyclicFileRecorder.WriteToOutputFile(pData, length, m_alwaysFlush);
}

void DebugTraceTargetFile::Flush() const
{
    m_cyclicFileRecorder.FlushToFile();
}

void DebugTraceTargetFile::Split()
{
    m_cyclicFileRecorder.SplitOutputFile();
}

OperationStatus DebugTraceTargetFile::SetTraceFilePath(std::string path)
{
    return m_cyclicFileRecorder.SetOutputFilesFolder(path);
}

std::string DebugTraceTargetFile::GetTraceFilePath() const
{
    return m_cyclicFileRecorder.GetOutputFilesFolder();
}