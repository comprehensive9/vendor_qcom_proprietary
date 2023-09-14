/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "PostCollectionFileHandler.h"
#include "Host.h"
#include "PersistentConfiguration.h"
#include "FileSystemOsAbstraction.h"
#include "ShellCommandExecutor.h"

#include <sstream>

using namespace std;

PostCollectionFileHandler::PostCollectionFileHandler():
    m_compressFiles(false),
    m_uploadFiles(false),
    m_maxNumOfLogFiles(Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxNumOfLogFiles),
    m_compressionApp(Host::GetHost().GetConfiguration().LogCollectorConfiguration.CompressionApp),
    m_verifyCompressionApp(Host::GetHost().GetConfiguration().LogCollectorConfiguration.VerifyCompressionApp),
    m_remoteIp(Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetServer),
    m_userName(Host::GetHost().GetConfiguration().LogCollectorConfiguration.UserName),
    m_remotePath(Host::GetHost().GetConfiguration().LogCollectorConfiguration.RemotePath),
    m_debugLogPrefix("LogFileHandler: ")
{
}

OperationStatus PostCollectionFileHandler::ConfigurePostCollectionSteps(bool compressFiles, bool uploadFiles)
{
    m_maxNumOfLogFiles = Host::GetHost().GetConfiguration().LogCollectorConfiguration.MaxNumOfLogFiles;
    m_remoteIp = Host::GetHost().GetConfiguration().LogCollectorConfiguration.TargetServer;
    m_userName = Host::GetHost().GetConfiguration().LogCollectorConfiguration.UserName;
    m_remotePath = Host::GetHost().GetConfiguration().LogCollectorConfiguration.RemotePath;
    OperationStatus os;
    if (compressFiles)
    {
        os = CheckCompress();
    }
    if (uploadFiles)
    {
        os = OperationStatus::Merge(os, CheckUpload());
    }
    if(os)
    {
        m_compressFiles = compressFiles;
        m_uploadFiles = uploadFiles;
    }

    return os;
}

void PostCollectionFileHandler::HandleClosedFile(const std::string& fileName)
{
    if (fileName.empty())
    {
        return;
    }
    LOG_DEBUG << "Handling closed file: " << fileName << endl;
    string newFileName = fileName;
    if (m_compressFiles)
    {
        CompressFile(newFileName);
        newFileName += ".gz";
    }
    if (m_uploadFiles)
    {
       UploadFile(newFileName);
    }

    DoFileBookkeeping(newFileName);
    LOG_DEBUG << "Handling closed file done" << endl;
}

bool PostCollectionFileHandler::IsPostStepsConfigured() const
{
    return (m_compressFiles || m_uploadFiles);
}

void PostCollectionFileHandler::DoFileBookkeeping(const string& newFileName)
{
    if (0 == m_maxNumOfLogFiles)
    {
        // no need to remove old files
        return;
    }

    lock_guard<mutex> lock(m_fileQueueMutex);
    m_fileQueue.push(newFileName);
    LOG_VERBOSE << m_debugLogPrefix << "Cleaning log files if needed. Queue size: " << m_fileQueue.size() << endl;

    if (m_fileQueue.size() > m_maxNumOfLogFiles)
    {
        const string &fileToRemove = m_fileQueue.front();
        LOG_INFO << m_debugLogPrefix << "Deleting old log file: " << fileToRemove << endl;
        const OperationStatus os = FileSystemOsAbstraction::RemoveFile(fileToRemove);
        if (!os)
        {
            LOG_ERROR << os.GetStatusMessage();
        }
        m_fileQueue.pop();
    }
}

OperationStatus PostCollectionFileHandler::CheckCompress() const
{
    if (!m_verifyCompressionApp)
    {
        LOG_DEBUG << "No compression app verification is required" << endl;
        return OperationStatus();
    }

    std::ostringstream cmd;
    cmd << "command -v " << m_compressionApp << " >/dev/null";
    LOG_DEBUG << "Verifying compression app"<< endl;

    const ShellCommandExecutor sce(cmd.str().c_str());
    if (sce.ExitStatus() != 0)
    {
        std::ostringstream osMessage;
        osMessage << "No compression application found: " << m_compressionApp;
        LOG_ERROR << osMessage.str() << endl;
        return OperationStatus(false, osMessage.str());
    }

    return OperationStatus();
}

OperationStatus PostCollectionFileHandler::CheckUpload() const
{
    std::ostringstream cmd;

    // Pinging 1 packet, wait maximum 2 sec.
    cmd << "ping -c1 -w2 " <<  m_remoteIp << " > /dev/null";
    ShellCommandExecutor sce0(cmd.str().c_str());
    if (sce0.ExitStatus() != 0)
    {
        std::ostringstream osMessage;
        osMessage << "IP is invalid. Check IP and user name in configuration: " << m_userName << "@" << m_remoteIp << " shell message: " << (!sce0.Output().empty() ? sce0.Output().front() : "");
        LOG_WARNING << osMessage.str() << endl;
        return OperationStatus(false, osMessage.str());
    }
    cmd.str(string());

    // man ssh:
    //-q      Quiet mode.  Causes most warning and diagnostic messages to be suppressed.
    // BatchMode
    // If set to “yes”, passphrase / password querying will be disabled. In addition, the ServerAliveInterval option will be set to 300 seconds by
    //    default.This option is useful in scripts and other batch jobs where no user is present to supply the password, and where it is desirable to
    //    detect a broken network swiftly.The argument must be “yes” or “no”.The default is “no”.
    cmd << "ssh -o BatchMode=yes " << m_userName << "@" << m_remoteIp << " echo 0 2>&1 ; exit";
    ShellCommandExecutor sce1(cmd.str().c_str());
    if (sce1.ExitStatus() != 0)
    {
        std::ostringstream osMessage;
        osMessage << "Cannot establish SSH connection to " << m_userName << "@" << m_remoteIp << " Command output: " << (!sce1.Output().empty() ? sce1.Output().front() : "");
        LOG_WARNING << osMessage.str() << endl;
        return OperationStatus(false, osMessage.str());
    }
    cmd.str(string());
    cmd << "ssh -o BatchMode=yes " << m_userName << "@" << m_remoteIp << " ls " << m_remotePath << " 2>&1 ; exit";
    ShellCommandExecutor sce2(cmd.str().c_str());
    if (sce2.ExitStatus() != 0)
    {
        std::ostringstream osMessage;
        osMessage << "Remote path " << m_remotePath << " is invalid. Message: " << (!sce2.Output().empty() ? sce2.Output().front() : "");
        LOG_WARNING << osMessage.str() << endl;
        return OperationStatus(false, osMessage.str());
    }
    return OperationStatus();
}

void PostCollectionFileHandler::CompressFile(const std::string& fullFilePath) const
{
    if (!FileSystemOsAbstraction::DoesFileExist(fullFilePath))
    {
        LOG_ERROR << "Failed to compress the file: " << fullFilePath << "; File does not exist" << endl;
        return;
    }
    LOG_DEBUG << "Compressing file: " << fullFilePath << endl;
    std::ostringstream cmd;
    cmd << m_compressionApp << ' ' << fullFilePath;
    const ShellCommandExecutor sce(cmd.str().c_str());
    if (sce.ExitStatus() != 0)
    {
        LOG_ERROR << "Failed to compress the file: " << fullFilePath << " could not be compressed, shell message: " << (!sce.Output().empty() ? sce.Output().front() : "") << endl;
    }
}

void PostCollectionFileHandler::UploadFile(const std::string& fullFilePath) const
{
    if (!FileSystemOsAbstraction::DoesFileExist(fullFilePath))
    {
        LOG_ERROR << "Failed to Upload the file: " << fullFilePath << " does not exist" << endl;
        return;
    }
    LOG_DEBUG << "Uploading file name is: " << fullFilePath << endl;

    std::ostringstream cmd;

    // Example: scp <fullFilePath> <user_name>@<ip>:<full path on remote server>
    cmd << "scp " << fullFilePath << " " << m_userName << "@" << m_remoteIp << ":" << m_remotePath<< "/";

    const ShellCommandExecutor sce(cmd.str().c_str());
    if (sce.ExitStatus() != 0)
    {
        LOG_WARNING << "File: " << fullFilePath << " could not be uploaded shell message: " << (!sce.Output().empty() ? sce.Output().front() : "") << endl;
    }
}