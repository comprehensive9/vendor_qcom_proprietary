/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "OperationStatus.h"

#include <string>
#include <queue>
#include <mutex>

class Device;

class PostCollectionFileHandler
{
public:
    PostCollectionFileHandler();
    OperationStatus ConfigurePostCollectionSteps(bool compressFiles, bool uploadFiles);
    void HandleClosedFile(const std::string& fileName);
    bool IsPostStepsConfigured() const;

private:
    bool m_compressFiles;
    bool m_uploadFiles;

    // From config:
    uint32_t m_maxNumOfLogFiles;
    const std::string m_compressionApp;
    const bool m_verifyCompressionApp;
    std::string m_remoteIp;
    std::string m_userName;
    std::string m_remotePath;

    std::string m_debugLogPrefix;

    std::queue<std::string> m_fileQueue;
    std::mutex m_fileQueueMutex;

    OperationStatus CheckCompress() const;
    OperationStatus CheckUpload() const;
    void CompressFile(const std::string& fullFilePath) const;
    void UploadFile(const std::string& fullFilePath) const;
    void DoFileBookkeeping(const std::string& newFileName);
};