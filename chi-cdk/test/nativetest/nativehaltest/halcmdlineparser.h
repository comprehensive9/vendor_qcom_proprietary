//=================================================================================================
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  halcmdlineparser.h
// @brief Header file for NATIVECHI3 command line parser
//=================================================================================================

#ifndef __NATIVETEST_CMDPARSER__
#define __NATIVETEST_CMDPARSER__

#include <stdint.h>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "nativetestlog.h"
#include "haljsonparser.h"

#define MAX_PATH      260
#ifdef _LINUX
const int NUM_IMAGES = 30;
#else
const int NUM_IMAGES = 5;
#endif

///Buffer timeout (in milliseconds) is obtained by multiplying this by TIMEOUT_RETRIES
///Timeout waiting for buffer on device
const uint64_t BUFFER_TIMEOUT_DEVICE_MILLI = 1000;    // in milliseconds (1 sec)
const uint64_t BUFFER_TIMEOUT_DEVICE_SEC = 1;         // in seconds

//=================================================================================================
// Class Definition
//=================================================================================================
class HalCmdLineParser
{
    public:

        HalCmdLineParser() = default;
        ~HalCmdLineParser() = default;

        /// Do not allow the copy constructor or assignment operator
        HalCmdLineParser(const HalCmdLineParser&) = delete;
        HalCmdLineParser& operator= (const HalCmdLineParser&) = delete;

        static int ParseCommandLine(int argc, char** argv);
        static void PrintCommandLineUsage();
        static void resetDefaultFlags();

        static int GetLogLevel()
        {
            return g_nLog;
        }

        static int GetFlushDelay()
        {
            return g_flushDelay;
        }

        static bool PrintHelp()
        {
            return g_bHelp;
        }
        static int GetCamera()
        {
            return g_camera;
        }
        static std::string GetTestSuiteName()
        {
            return m_suiteName;
        }
        static std::string GetTestCaseName()
        {
            return m_caseName;
        }
        static std::string GetTestFullName()
        {
            return m_fullName;
        }

        static bool isQCFASize()
        {
            return g_isQCFA;
        }

        static bool NoImageDump()
        {
            return g_noImageDump;
        }

        // Returns...
        //   < 0        disable timeout
        //   otherwise  timeout override in milliseconds
        static int GetTimeoutOverride()
        {
            return g_timeout;
        }

        // static Definitions
        static int            g_nLog;
        static bool           g_bHelp;
        static bool           g_isQCFA;
        static bool           g_noImageDump;
        static int            g_flushDelay;
        static int            g_camera;      // -1 means --camera flag not set; select all cameras for testing
        static int            g_timeout;
        static char           g_camFilter[];
        static char           g_camList[];
        static int            g_randomseed;
        static int            g_platform;
        static int            g_vstats;

        static std::string    m_suiteName;
        static std::string    m_caseName;
        static std::string    m_fullName;
        static std::string    m_testcaseOrJson;

        static const char     g_camFilterPhysicalStr[];
        static const char     g_camFilterLogicalStr[];
        static const char     g_camFilterAllStr[];
};

#endif //__NATIVETEST_CMDPARSER__
