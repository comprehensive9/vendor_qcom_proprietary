/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <string>
#include <memory>
#include <array>
#include <ostream>

#include "DebugTraceStreamBuffer.h"

class DebugTraceService;
class DebugTraceTargetStdout;
class DebugTraceTargetFile;
class OperationStatus;


// Severity values are expected to raise from zero
enum LogSeverity
{
    LOG_SEV_ERROR = 0,     // Unexpected input/events that may cause server misbehavior
    LOG_SEV_WARNING = 1,   // Suspicious events
    LOG_SEV_INFO = 2,      // Events like command/response
    LOG_SEV_DEBUG = 3,     // Detailed functionality
    LOG_SEV_VERBOSE = 4,   // Excessive debug
    LOG_SEV_MAX,
};

// When adding new module do not forget to add the module in the next:
// TraceSetSeverityRequest::ParseModuleType()
// DebugTraceService::ModuleToString()
// update DebugTraceService constructor
enum ModuleId
{
    DFLT = 0,
    RDWR = 1,
    DTM_MAX,
};

// *************************************************************************************************

const char* ModuleToString(ModuleId dtm);
const char* SeverityToString(LogSeverity sev);

// *************************************************************************************************

class DebugTraceService
{
public:
    static DebugTraceService& GetInstance();
    DebugTraceService();

    void InitTraceFile();

    OperationStatus SetTracePath(std::string tracePath);
    void SetTraceMaxSingleFileSize(int TraceMaxSingleFileSizeInMb);
    void SetTraceMaxNumOfFile(int traceMaxNumOfFile);

    std::string GetTracePath();
    int GetTraceMaxSingleFileSize() const { return m_traceMaxSingleFileSizeInMb; };
    int GetTraceMaxNumOfFile() const { return m_traceMaxNumOfFile; };

    void SetModuleMaxSeverity(ModuleId dtm, LogSeverity severity);
    void SetModuleMaxSeverity(LogSeverity severity);
    LogSeverity GetModuleMaxSeverity(ModuleId dtm) const;
    const std::array<LogSeverity, static_cast<int>(DTM_MAX)> & GetAllModuleMaxSeverity();

    void SubmitBuffer(const char* pData, size_t length);
    void Flush();
    void Split();

    bool ShouldPrintLocation() const { return m_printLocation; }
    bool ShouldExitOnAssert() const { return m_exitOnAssert; }
    bool ShouldAlwaysFlush() const { return m_alwaysFlush; }

    bool StartMessage(ModuleId dtm, LogSeverity severity, const char* szFile, size_t nLine);
    std::ostream& GetStream();

private:
    static std::string GetDefaultTraceFilePath();
    static int GeDefaulttMaxNumOfDebugTraceFiles();
    static int GetDefaultMaxSingleFileSizeInMb();

    std::string m_tracePath;
    int m_traceMaxSingleFileSizeInMb;
    int m_traceMaxNumOfFile;

    const std::unique_ptr<DebugTraceTargetStdout> m_upDebugTraceTargetStdout;
    std::unique_ptr<DebugTraceTargetFile> m_upDebugTraceTargetFile;

    const bool m_printLocation;
    const bool m_exitOnAssert;
    const bool m_alwaysFlush; // Flush after each message

    std::array<LogSeverity, static_cast<int>(DTM_MAX) > m_modulesSeverity;
};
