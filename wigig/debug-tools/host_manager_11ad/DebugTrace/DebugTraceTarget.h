/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "CyclicFileRecorder.h"
#include "OperationStatus.h"
#include <vector>
#include <memory>

// *************************************************************************************************

// Prints log messages to stdout
class DebugTraceTargetStdout
{
public:
    void SubmitBuffer(const char* pData, size_t length);
    void Flush() const;
};

// *************************************************************************************************

// Prints log messages to file
class DebugTraceTargetFile
{
public:
    DebugTraceTargetFile(bool bAlwaysFlush, std::string debugTracePath, int maxSingleFileSizeInMb, int maxNumOfFiles);

    void SubmitBuffer(const char* pData, size_t length);
    void Flush() const;
    void Split();
    OperationStatus SetTraceFilePath(std::string path);
    std::string GetTraceFilePath() const;

private:
    const bool m_alwaysFlush;
    CyclicFileRecorder m_cyclicFileRecorder;
};