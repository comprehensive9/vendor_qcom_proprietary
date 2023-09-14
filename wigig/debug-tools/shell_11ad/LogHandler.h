/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <chrono>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctime>

#ifdef _WINDOWS
    #pragma warning(push)
    #pragma warning(disable: 4702) // suppress false unreachable code warning
#endif

// Severity values are expected to raise from zero
enum LogSeverity
{
    LOG_SEV_ERROR = 0,   // Unexpected input/events that may cause server misbehavior
    LOG_SEV_WARNING = 1,   // Tcp messages
    LOG_SEV_INFO = 2,   // Stream stats
    LOG_SEV_DEBUG = 3,   // Message to send
    LOG_SEV_VERBOSE = 4,   // Excessive debug
};

class WallTimeTag {};
inline std::ostream& operator<<(std::ostream& os, const WallTimeTag& wallTimeTag)
{
    (void)wallTimeTag;
    const auto wallTime = std::chrono::system_clock::now();
    const auto wallTimeMsec = std::chrono::duration_cast<std::chrono::milliseconds>(wallTime.time_since_epoch()) % 1000;

    const auto wallTimeSystem = std::chrono::system_clock::to_time_t(wallTime);

    // 32 bytes is enough for the used time format: 2020-07-19 18:44:41
    char timeBuf[32];
    if (std::strftime(timeBuf, sizeof(timeBuf), "%F %T", std::localtime(&wallTimeSystem)))
    {
        os << timeBuf << '.' << std::setfill('0') << std::setw(3) << wallTimeMsec.count();
    }

    return os;

    // std::put_time is not supported by all GCC compilers that do not fully support C++11.
    // May be used when using GCC 5.x on all platforms.
    //return os << std::put_time(std::localtime(&wallTimeSystem), "%a %b %d %Y %T")
    //    << '.' << std::setfill('0') << std::setw(3) << wallTimeMsec.count();
}

#define TRACE_WITH_PREFIX(SEV)                                          \
    g_LogConfig.ShouldPrint(SEV) && std::cout << "["<< g_LogConfig.SeverityToString(SEV) << "] {" << WallTimeTag() << "} "

#define LOG_ERROR   TRACE_WITH_PREFIX(LOG_SEV_ERROR)
#define LOG_WARNING TRACE_WITH_PREFIX(LOG_SEV_WARNING)
#define LOG_INFO    TRACE_WITH_PREFIX(LOG_SEV_INFO)
#define LOG_DEBUG   TRACE_WITH_PREFIX(LOG_SEV_DEBUG)
#define LOG_VERBOSE TRACE_WITH_PREFIX(LOG_SEV_VERBOSE)



// Decoupling from system assert allows to print an error message when
// assert is disabled.
#define LOG_ASSERT(CONDITION)                                           \
    do {                                                                \
        if (!(CONDITION)) {                                             \
            LOG_ERROR << "ASSERTION FAILURE: " << #CONDITION            \
                << " at " << __FILE__ << ':' << __LINE__                \
                << std::endl;                                           \
                if (g_LogConfig.ShouldExitOnAssert()) exit(1);          \
        }                                                               \
    } while (false)



class LogConfig
{
public:
    LogConfig(): m_MaxSeverity(LOG_SEV_ERROR) {}

    bool ShouldPrint(LogSeverity sev) const { return sev <= m_MaxSeverity; }

    void SetMaxSeverity(int traceLevel)
    {
        if (traceLevel > LOG_SEV_VERBOSE)
        {
            fprintf(stderr, "Invalid trace level, setting %d\n", LOG_SEV_VERBOSE);
            m_MaxSeverity = LOG_SEV_VERBOSE;
        }
        else
        {
            m_MaxSeverity = static_cast<LogSeverity>(traceLevel);
        }
    }

    static const char* SeverityToString(LogSeverity sev)
    {
        static const char* const pSeverityToString[] = { "ERR", "WRN", "INF", "DBG", "VRB" };

        const size_t index = static_cast<size_t>(sev);
        if (index >= sizeof(pSeverityToString) / sizeof(pSeverityToString[0]))
        {
            return "---";
        }

        return pSeverityToString[index];
    }

private:
    LogSeverity m_MaxSeverity;
};

extern LogConfig g_LogConfig;
extern std::string  g_ApplicationName;
