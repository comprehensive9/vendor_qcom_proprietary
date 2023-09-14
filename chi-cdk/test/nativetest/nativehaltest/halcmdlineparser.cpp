//=================================================================================================
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  HalCmdLineParser.cpp
// @brief Source file for nativetest command line parser
//=================================================================================================

#include <iostream>
#include <string>
#include <set>
#include "halcmdlineparser.h"

// static Definitions
int  HalCmdLineParser::g_nLog                         = 1;
bool HalCmdLineParser::g_bHelp                        = false;
bool HalCmdLineParser::g_isQCFA                       = false;
bool HalCmdLineParser::g_noImageDump                  = false;
int  HalCmdLineParser::g_flushDelay                   = -1;
int  HalCmdLineParser::g_camera                       = -1;    // -1 means --camera flag not set; select all cameras for testing

char  HalCmdLineParser::g_camFilter[MAX_PATH]         = "all";
char  HalCmdLineParser::g_camList[MAX_PATH]           = "[]";

const char HalCmdLineParser::g_camFilterPhysicalStr[] = "physical";
const char HalCmdLineParser::g_camFilterLogicalStr[]  = "logical";
const char HalCmdLineParser::g_camFilterAllStr[]      = "all";
int  HalCmdLineParser::g_randomseed                   = false;
int  HalCmdLineParser::g_platform                     = -1;
int  HalCmdLineParser::g_vstats                       = -1;

///Buffer timeout (in milliseconds) is obtained by multiplying this by TIMEOUT_RETRIES
#ifdef _LINUX
int HalCmdLineParser::g_timeout                       = BUFFER_TIMEOUT_DEVICE_MILLI; // default is 1 second
#else
int HalCmdLineParser::g_timeout                       = BUFFER_TIMEOUT_DEVICE_MILLI * 10;
#endif // _LINUX

std::string HalCmdLineParser::m_suiteName                   = "";
std::string HalCmdLineParser::m_caseName                    = "";
std::string HalCmdLineParser::m_fullName                    = "";
std::string HalCmdLineParser::m_testcaseOrJson              = ""; //JSON file name to be matched with registered test

//Comparator func for char*
struct  cstrless : public std::binary_function<const char*, const char*, bool> {
    bool operator()(const char* a,const char* b) const {
        return strncmp(a, b, strlen(b)) < 0;
    }
};
std::set<const char*, cstrless> CmdFlagCheck;               // Set used to detect duplicate flags
/**************************************************************************************************
*   HalCmdLineParser::resetDefaultFlags()
*
*   @brief
*       Reset flags to default when running multiple tests
*   @return
*       Void
 **************************************************************************************************/
void HalCmdLineParser::resetDefaultFlags()
{

    strlcpy(HalCmdLineParser::g_camList, "[]", MAX_PATH - 1);
    strlcpy(HalCmdLineParser::g_camFilter, "all", MAX_PATH - 1);

#ifdef _LINUX
    HalCmdLineParser::g_timeout = BUFFER_TIMEOUT_DEVICE_MILLI;
#else
    HalCmdLineParser::g_timeout = BUFFER_TIMEOUT_DEVICE_MILLI * 10;
#endif // _LINUX

    HalCmdLineParser::g_nLog            = 1;
    HalCmdLineParser::g_bHelp           = false;
    HalCmdLineParser::g_isQCFA          = false;
    HalCmdLineParser::g_noImageDump     = false;
    HalCmdLineParser::g_flushDelay      = -1;
    HalCmdLineParser::g_camera          = -1;
    HalCmdLineParser::g_randomseed      = false;
    HalCmdLineParser::g_platform        = -1;
    HalCmdLineParser::g_vstats          = -1;
    HalCmdLineParser::m_suiteName       = "";
    HalCmdLineParser::m_caseName        = "";
    HalCmdLineParser::m_fullName        = "";

    haltests::HalJsonParser::ResetDefaultFlags();
}

/**************************************************************************************************
*   ParseCommandLine
*
*   @brief
*       Parse the command line using GNU Getopt feature
*   @return
*       0 (runs test) and 1 (runs json), -1 fail
**************************************************************************************************/
int HalCmdLineParser::ParseCommandLine(
    int argc,       ///< [in] the total number of arguments
    char** argv)    ///< [in] array of argument values
{
    int getOptReturn;
    int foundJsonFile     = 0;  //Default value assume no Json input
    bool isFrameDuration  = false;
    bool isListCamera     = false;
    bool isTestFlagParsed = false;

    // Parse all short & long option flags to check for duplicates
    for (int i = 0; i < argc; i++)
    {
        if(strncmp(argv[i],"-", 1) == 0)
        {
            if(!CmdFlagCheck.insert(argv[i]).second)
            {
                printf("Duplicate flags found for %s, Exiting !\n", argv[i]);
                return -1;
            }
        }
    }

    // Parse all command line flag options with '-' and '--'
    while (true)
    {
        int optionIndex = 0;

        static struct option cmdLineOptions[] =
        {
            //long option,     DataRequirement,     flag,   short option/alias
            { "help",             no_argument,        0,         'h'  }, // --help    (-h)
            { "v",                required_argument,  0,         'v'  }, // --v (-v) [val]
            { "frames",           required_argument,  0,         'f'  }, // --frames  (-f) [val]
            { "camera",           required_argument,  0,         'c'  }, // --camera  (-c) [val]
            { "timeout",          required_argument,  0,         't'  }, // --timeout (-t) [val]
            { "p",                no_argument,        0,         'p' },  // --p (-p)
            // Run testsuite / testcase
            { "gtest_filter",     required_argument,  0,           0  }, // --gtest_filter [val]
            { "test",             required_argument,  0,           1  }, // --test [val]
            { "mcbonly",          no_argument,        0,           2  }, // --mcbonly
            // Disable image dumping
            { "nid",              no_argument,        0,           3  }, // --nid
            { "flushdelay",       required_argument,  0,           4  }, // --flushdelay [val]
            { "camlist",          required_argument,  0,           5  }, // --camlist [val]
            { "camfilter",        required_argument,  0,           6  }, // --camfilter [val]
            { "randomseed",       required_argument,  0,           7  }, // --randomseed [val]
            { "duration",         required_argument,  0,           8  }, // --duration [val]
            { "snaps",            required_argument,  0,           9  }, // --snaps [val]
            { "platform",         required_argument,  0,          10  }, // --platform [val]
            { "mfnr",             no_argument,        0,          11  }, // --mfnr [val]
            { "hdr",              required_argument,  0,          12  }, // --hdr [val]
            { "qcfa",             no_argument,        0,          13  }, // --qcfa
            { 0,                    0,                0,            0  }  // last element is 0 to mark ending
        };

        getOptReturn = getopt_long(argc, argv, "hpv:f:c:t:0:1:234:5:6:7:8:9:10:1112:13", cmdLineOptions, &optionIndex);
        if(getOptReturn == -1)
            break; // loop terminates when no more option characters, getopt_long() returns -1


        switch (getOptReturn)
        {
        case 'h':
        case '?':
            g_bHelp = true;
            break;
        case 'v':
            g_nLog  = atoi(optarg);
            break;
        case 'f':
        {

            if (isFrameDuration)
            {
                printf("--frames (-f) is mutually exclusive to --duration !\n");
                return -1;
            }
            haltests::HalJsonParser::sFrames = atoi(optarg);
            isFrameDuration = true;
            haltests::HalJsonParser::sDuration = -1;
            foundJsonFile   = 0;
            break;
        }
        case 'c':
        {
            if (isListCamera)
            {
                printf("--camera (-c) is mutually exclusive to --camlist !\n");
                return -1;
            }
            g_camera      = atoi(optarg);
            isListCamera  = true;
            foundJsonFile = 0;
            break;
        }
        case 't':
        {
            g_timeout     = atoi(optarg);
            foundJsonFile = 0;
            break;
        }
        case 'p':
        {
            foundJsonFile = 0;
            break;
        }
        case 0:
        case 1:
        {
            if (!isTestFlagParsed)
            {
                m_testcaseOrJson = optarg;
                isTestFlagParsed = true;
                // Checks if the test name has a Json file associated with it
                if (haltests::HalJsonParser::jsonFileCheck(m_testcaseOrJson))
                {
                    foundJsonFile = 1;
                }
            }
            else
            {
                return 2;
            }
            break;
        }
        case 2:
        {
            haltests::HalJsonParser::sMCBOnly = true;
            foundJsonFile  = 0;
            break;
        }
        case 3:
        {
            g_noImageDump = true;
            foundJsonFile = 0;
            break;
        }
        case 4:
        {
            g_flushDelay  = atoi(optarg);
            foundJsonFile = 0;
            break;
        }
        case 5:
        {
            if (isListCamera)
            {
                printf("--camlist is mutually exclusive to  --camera (-c) !\n");
                return -1;
            }
            memcpy(g_camList, optarg, MAX_PATH);
            isListCamera     = true;
            foundJsonFile    = 0;
            break;
        }
        case 6:
        {
            memcpy(g_camFilter, optarg, MAX_PATH);
            foundJsonFile = 0;
            break;
        }
        case 7:
        {
            g_randomseed  = atoi(optarg);
            foundJsonFile = 0;
            break;
        }
        case 8:
        {
            if (isFrameDuration)
            {
                printf("--duration is mutually exclusive to --frames (-f)  !\n");
                return -1;
            }
            haltests::HalJsonParser::sDuration = atoi(optarg);
            isFrameDuration = true;
            haltests::HalJsonParser::sFrames = -1;
            foundJsonFile   = 0;
            break;
        }
        case 9:
        {
            haltests::HalJsonParser::sSnaps = atoi(optarg);
            foundJsonFile = 0;
            break;
        }
        case 10:
        {
            g_platform    = atoi(optarg);
            foundJsonFile = 0;
            break;
        }
        case 11:
        {
            haltests::HalJsonParser::sMFNR = true;
            foundJsonFile = 0;
            break;
        }
        case 12:
        {
            static char hdrType[5];
            memcpy(hdrType, optarg, 5);
            if (strcmp(hdrType, "mfhdr") == 0 || strcmp(hdrType, "MFHDR") == 0)
            {
                haltests::HalJsonParser::sMFHDR = true;
            }
            else if (strcmp(hdrType, "shdr") == 0 || strcmp(hdrType, "SHDR") == 0)
            {
                haltests::HalJsonParser::sSHDR  = true;
            }
            foundJsonFile = 0;
            break;
        }
        case 13:
        {
            g_isQCFA      = true;
            foundJsonFile = 0;
            break;
        }
        default:
            g_bHelp = true;
            break;
        }
    }

    // Extract Test suite name & case name to override json when cmd line params are present
    if (foundJsonFile == 0)
    {
        std::string delimiter = ".";
        // extract suite name
        m_suiteName = HalCmdLineParser::m_testcaseOrJson.substr(0, HalCmdLineParser::m_testcaseOrJson.find(delimiter));
        // extract case name
        m_caseName = HalCmdLineParser::m_testcaseOrJson.erase(0, m_suiteName.size() + 1);
        // create valid test full name
        m_fullName = m_suiteName + delimiter + m_caseName;
    }

    if (g_bHelp)
    {
        return -1;
    }

    return foundJsonFile;
}
/**************************************************************************************************
*   PrintCommandLineUsage
*
*   @brief
*       Print the usage instructions to stdout
*       Usage instructions are printed when a commandline error is detected or --help (-h) is used
**************************************************************************************************/
void HalCmdLineParser::PrintCommandLineUsage()
{
    printf("******************************** USAGE *********************************\n");
    printf(" --gtest_filter (no alias) [val] : run the given testsuite / testcases(s)\n");
    printf(" --test         (no alias) [val] : run the given testsuite / testcases(s)\n");
    printf("  usage :- \n");
    printf("              --gtest_filter TestSuiteName.TestCase or TestSuiteName.*\n");
    printf("              --test TestSuiteName.TestCase or TestSuiteName.*\n");
    printf(" --frames                        : override default number of frames to be dumped.\n");
    printf("                                   Mutually exclusive with --duration\n");
    printf(" --flushdelay                    : override flush delay (usec) for flush tests\n");
    printf(" --mcbonly                       : enable only master callback\n");
    printf(" --mfnr                          : enable MFNR on the testcase\n");
    printf(" --hdr                           : enable MFHDR/SHDR [MFHDR, SHDR] on the testcase\n");
    printf(" --nid                           : disables result image dumping to device\n");
    printf(" -c                              : override for camera selection to run test on.\n");
    printf("                                   Mutually exclusive with --camlist\n");
    printf(" --timeout                       : timeout in seconds while waiting for buffers/results\n");
    printf(" --camfilter                     : filter which camera types to test: physical, logical, all\n");
    printf("                                   Default is physical.\n");
    printf(" --camlist                       : override for multiple cameras to run test on: [id1,id2].\n");
    printf("                                   Mutually exclusive with --c\n");
    printf(" --platform                      : platform/target device that is being tested. Insert number in range [0,4]\n");
    printf("                                   [0-kona non triple camera/2 ifes, 1-kona non triple camera/3 ifes, \n");
    printf("                                   2 - kona triple camera / 2 ifes, 3 - kona triple camera / 3 ifes, \n");
    printf("                                   4 - kona HMD / 6 ifes]\n");
    printf(" --duration                      : test duration in milliseconds. Default is 1000 milliseconds.\n");
    printf("                                   Mutually exclusive with --frames\n");
    printf(" --randomseed                    : to initialize the pseudo random generator\n");
    printf(" --snaps                         : number of snapshots to take every 30 frames. Default is 1. \n");
    printf(" --qcfa                          : Enables qcfa sensor mode if supported on camera \n");
    printf(" --p                             : print app logs on stdout\n");
    printf(" --v                             : logging levels. Insert number in range [0,3]\n");
    printf("                                   [0-debug, 1-info, 2-warning, 3-error] and above messages\n");
    printf(" --h                             : print the usage instructions\n");
    printf("************************************************************************\n");
}
