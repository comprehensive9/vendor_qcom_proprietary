//=================================================================================================
// Copyright (c) 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  nativetestlog.h
// @brief Header file for framework logging facility
//=================================================================================================

#ifndef NATIVETESTLOG_H
#define NATIVETESTLOG_H

#include <stdio.h>
#include <vector>
#include "nativetest.h"
#include "chxutils.h"
#include "commonutils.h"
// Determine the compiler's environment
#if ((ULONG_MAX) == (UINT_MAX))
#define ENVIRONMENT32
#else
#define ENVIRONMENT64
#endif

namespace haltests {

#ifndef __FILENAME__
#define __FILENAME__ haltests::OsUtils::GetFileName(__FILE__)
#endif

#define MAX_PATH    260
#define KSTRING_LEN 1024
#define stringify( name ) # name
#define APPNAME "NATIVEHALTEST"

#ifdef _LINUX
    static const std::string sLogRootPath = "/data/vendor/camera/";
#else
    static const std::string sLogRootPath = "./";
#endif

// Maximum string length
#define NT_STRLEN_MAX 1024

// Definition for unused parameter
#define NT_UNUSED_PARAM(expr) (void)(expr)

// Definition for NT_LOG macro
#define NT_LOG(logLevel, ...)                             \
    if(logLevel >= NTLog.m_eLogLevel) {                   \
        NTLog.Log(logLevel,                               \
                  __FILENAME__,                           \
                  __FUNCTION__,                           \
                  __LINE__,                               \
                  ##__VA_ARGS__);                         \
     }

// Define logging levels

#define NT_LOG_STD( FMT, ... ) \
    NT_LOG(eStandard, FMT, ##__VA_ARGS__ );

#define NT_LOG_ENTRY() \
    NT_LOG(eEntry, "%s: %s: Line %d", __FILENAME__, __FUNCTION__, __LINE__);

#define NT_LOG_EXIT() \
    NT_LOG(eExit,  "%s: %s: Line %d", __FILENAME__, __FUNCTION__, __LINE__);

#define NT_LOG_DEBUG( FMT, ... ) \
    NT_LOG(eDebug, FMT, ##__VA_ARGS__ );

#define NT_LOG_INFO( FMT, ...) \
    NT_LOG(eInfo, FMT, ##__VA_ARGS__ );

#define NT_LOG_PERF( FMT, ... ) \
    NT_LOG(ePerformance, FMT, ##__VA_ARGS__ );

#define NT_LOG_UNSUPP( FMT, ... ) \
    NT_LOG(eUnsupported, FMT, ##__VA_ARGS__ );

#define NT_LOG_WARN( FMT, ...) \
    NT_LOG(eWarning, FMT, ##__VA_ARGS__ );

#define NT_LOG_ERROR( FMT, ... ) \
    NT_LOG(eError, FMT, ##__VA_ARGS__ );

    enum verboseSeverity
    {
        eEntry = 0, // verbose developer function entry debug messages
        eExit = 1, // verbose developer function exit debug messages
        eDebug = 2, // verbose developer debug messages
        eInfo = 3, // generic developer info messages
        ePerformance = 4, // generic developer performance info messages
        eUnsupported = 5, // typical end user unsupported messages
        eWarning = 6, // non-critical warning messages
        eError = 7, // critical error messages
        eStandard = 8, // standard decoration / separator, must always print
    };

    class NativeTestLog
    {
    public:
        verboseSeverity m_eLogLevel;

        NativeTestLog();
        ~NativeTestLog() = default;

        static void Log(verboseSeverity severity, const char* mFile, const char* mFunc, UINT ln, const char* mFormat, ...);
        static std::string GetMSGPrefixString(verboseSeverity severity);
        int SetLogLevel(verboseSeverity level);
        void SetTestName(const char* suiteName, const char* caseName);
    };

    extern NativeTestLog NTLog;
    extern const char* currentTestSuiteName;
    extern const char* currentTestCaseName;
}
#endif //NATIVETESTLOG_H
