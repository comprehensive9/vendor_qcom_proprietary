/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "DebugTraceService.h"
#include "DebugTraceTarget.h"
#include "FileSystemOsAbstraction.h"
#include "OperationStatus.h"
#include "DebugLogger.h"

#include <string>
#include <cstdio>
#include <thread>

DebugTraceService& DebugTraceService::GetInstance()
{
    static DebugTraceService instance;
    return instance;
}

DebugTraceService::DebugTraceService()
    : m_tracePath(GetDefaultTraceFilePath())
    , m_traceMaxSingleFileSizeInMb(GetDefaultMaxSingleFileSizeInMb())
    , m_traceMaxNumOfFile(GeDefaulttMaxNumOfDebugTraceFiles())
    , m_upDebugTraceTargetStdout(new DebugTraceTargetStdout())
    , m_printLocation(false)
    , m_exitOnAssert(false)
    , m_alwaysFlush(false)
{
    m_modulesSeverity.fill(LOG_SEV_INFO);
};

void DebugTraceService::InitTraceFile()
{
    m_upDebugTraceTargetFile.reset( new DebugTraceTargetFile(
        ShouldAlwaysFlush(),
        m_tracePath,
        m_traceMaxSingleFileSizeInMb,
        m_traceMaxNumOfFile));
}

void DebugTraceService::SetModuleMaxSeverity(ModuleId dtm, LogSeverity severity)
{
    m_modulesSeverity[static_cast<int>(dtm)] = severity;
}

void DebugTraceService::SetModuleMaxSeverity(LogSeverity severity)
{
    for (LogSeverity& modulesev : m_modulesSeverity)
    {
        modulesev = severity;
    }
}

LogSeverity DebugTraceService::GetModuleMaxSeverity(ModuleId dtm) const
{
    return m_modulesSeverity[static_cast<int>(dtm)];
}

const std::array<LogSeverity, static_cast<int>(DTM_MAX)> & DebugTraceService::GetAllModuleMaxSeverity()
{
    return m_modulesSeverity;
}

void DebugTraceService::SubmitBuffer(const char* pData, size_t length)
{
    m_upDebugTraceTargetStdout->SubmitBuffer(pData, length);
    if (m_upDebugTraceTargetFile)
    {
        m_upDebugTraceTargetFile->SubmitBuffer(pData, length);
    }
}
void DebugTraceService::Flush()
{
    m_upDebugTraceTargetStdout->Flush();
    if (m_upDebugTraceTargetFile)
    {
        m_upDebugTraceTargetFile->Flush();
    }
}
void DebugTraceService::Split()
{
    if (m_upDebugTraceTargetFile)
    {
        m_upDebugTraceTargetFile->Split();
    }
}

std::string DebugTraceService::GetTracePath()
{
    return m_tracePath;
}

OperationStatus DebugTraceService::SetTracePath(std::string tracePath)
{
    if (tracePath.back() != '/')
    {
        return OperationStatus(false, "Path have to end with \"/\"");
    }

    m_tracePath = tracePath;
    if (!m_upDebugTraceTargetFile)
    {
        return OperationStatus();
    }
    return m_upDebugTraceTargetFile->SetTraceFilePath(tracePath);
}

void DebugTraceService::SetTraceMaxSingleFileSize(int TraceMaxSingleFileSizeInMb)
{
    m_traceMaxSingleFileSizeInMb = TraceMaxSingleFileSizeInMb;
}

void DebugTraceService::SetTraceMaxNumOfFile(int traceMaxNumOfFile)
{
    m_traceMaxNumOfFile = traceMaxNumOfFile;
}

std::string DebugTraceService::GetDefaultTraceFilePath()
{
    return FileSystemOsAbstraction::GetOutputFilesLocation() + "DebugTrace" + FileSystemOsAbstraction::GetDirectoryDelimiter();
}

int DebugTraceService::GeDefaulttMaxNumOfDebugTraceFiles()
{
#ifdef __ANDROID__
    return 2;
#elif _OPENWRT_ // OpenWrt have to be before __linux
    return 2;
#elif __linux
    return 5;
#elif _WINDOWS //windows
    return 5;
#else //OS3
    return 5;
#endif // __linux
}

int DebugTraceService::GetDefaultMaxSingleFileSizeInMb()
{
#ifdef __ANDROID__
    return 50;
#elif _OPENWRT_ // OpenWrt have to be before __linux
    return 20;
#elif __linux
    return 50;
#elif _WINDOWS //windows
    return 50;
#else //OS3
    return 20;
#endif // __linux
}

std::ostream& DebugTraceService::GetStream()
{
    thread_local static DebugTraceStreamBuffer logStreamBuffer;
    thread_local static std::ostream stream(&logStreamBuffer);
    return stream;
}

bool DebugTraceService::StartMessage(ModuleId dtm, LogSeverity severity, const char* szFile, size_t nLine)
{

    if (!ShouldPrint(dtm, severity))
    {
        return false;
    }

    GetStream() << "[" << SeverityToString(severity) << "] ";
    GetStream() << "[" << ModuleToString(dtm) << "] ";

    if (DebugTraceService::GetInstance().ShouldPrintLocation())
    {
        GetStream() << "(" << szFile << ':' << nLine << ") ";
    }

    GetStream() << "{" << Utils::GetCurrentLocalTimeString() << "} ";
    GetStream() << "{tid:" << std::this_thread::get_id() << "} ";

    return true;
}

// *************************************************************************************************

const char * ModuleToString(ModuleId dtm)
{
    static const char* const pModuleToString[] = { "DFLT", "RDWR", "---" };

    return pModuleToString[static_cast<size_t>(dtm)];
}

const char* SeverityToString(LogSeverity sev)
{
    static const char* const pSeverityToString[] = { "ERR", "WRN", "INF", "DBG", "VRB" };

    return pSeverityToString[static_cast<size_t>(sev)];
}
