//=================================================================================================
// Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  nativetestlog.cpp
// @brief Implementation of logging methods
//=================================================================================================

#include "nativetestlog.h"
#include <stdarg.h>
#include "log/log.h"

namespace chitests {

    NativeTestLog NTLog;
    const char* currentTestSuiteName;
    const char* currentTestCaseName;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Constructor for ChiFeature2Log
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    NativeTestLog::NativeTestLog()
    {
        // Set default log level to debug and above messages
        m_eLogLevel = eDebug;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Helper function for logging
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string NativeTestLog::GetMSGPrefixString(
        verboseSeverity severity)  ///< [in] severity level
    {
        switch (severity)
        {
        case eDebug:        return "[DEBUG] ";
        case eInfo:         return "[ INFO] ";
        case ePerformance:  return "[ PERF] ";
        case eUnsupported:  return "[UNSUP] ";
        case eWarning:      return "[ WARN] ";
        case eError:        return "[ERROR] ";
        case eStandard:     return "[=====] ";
        default:
            return "[UNKWN]";
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Sets the logging level
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int NativeTestLog::SetLogLevel(
        verboseSeverity level)
    {
        switch (level)
        {
        case eDebug:        m_eLogLevel = eDebug;
            break;
        case eInfo:         m_eLogLevel = eInfo;
            break;
        case ePerformance:  m_eLogLevel = ePerformance;
            break;
        case eUnsupported:  m_eLogLevel = eUnsupported;
            break;
        case eWarning:      m_eLogLevel = eWarning;
            break;
        case eError:        m_eLogLevel = eError;
            break;
        default:
            return -1;
        }
        return 0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Sets the logging level
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void NativeTestLog::SetTestName(const char* suiteName, const char* caseName)
    {
        currentTestSuiteName = suiteName;
        currentTestCaseName = caseName;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Print the message to console and logcat
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void NativeTestLog::Log(verboseSeverity severity, const char* fileName, const char* functionName,
        UINT lineNumber, const char* message, ...)
    {
        std::string fullMessage;

        // Extract message to print from variable argument inputs
        char printStatement[NT_STRLEN_MAX];
        va_list args;
        va_start(args, message);
        vsnprintf(printStatement, NT_STRLEN_MAX - 1, message, args);
        va_end(args);

        std::string srcFile = std::string(fileName) + " : ";
        std::string srcFunc = std::string(functionName) + "() : ";
        std::string srcLine = std::to_string(lineNumber) + " : ";

        fullMessage = GetMSGPrefixString(severity) + srcFile + srcLine + printStatement;
        // print to console
        fprintf(stdout, "%s\n", fullMessage.c_str());

        fullMessage = GetMSGPrefixString(severity) + srcFile + srcLine + srcFunc + printStatement;
        //print to logcat
        ALOGE("[NATIVECHITEST] : %s\n", fullMessage.c_str());
    }
}