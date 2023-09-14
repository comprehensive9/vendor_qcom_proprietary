//=================================================================================================
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  cmdlineparser.cpp
// @brief Source file for nativetest command line parser
//=================================================================================================

#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include "cmdlineparser.h"
#include "jsonparser.h"

// test case infrastructure
bool        CmdLineParser::m_bHelp             = false;
bool        CmdLineParser::m_bNoImageDump      = false;
UINT32      CmdLineParser::m_nFrames           = DEFAULT_FRAME_COUNT;// total number of frame requests to be sent
UINT32      CmdLineParser::m_nSnaps            = DEFAULT_SNAPS_COUNT;// number of snaps to take in a preview+snapshot test case
char        CmdLineParser::m_camList[MAX_PATH] = "[]";
int         CmdLineParser::m_nLog              = 0;
std::string CmdLineParser::m_caseName          = "";
std::string CmdLineParser::m_fullName          = "";
std::string CmdLineParser::m_suiteName         = "";
std::string CmdLineParser::m_testcaseOrJson    = ""; // JSON file name to be matched with registered test name for running
std::vector<CmdLineParser::SensorCaps> CmdLineParser::m_sensorCapsList = {}; // string with list of sensor caps for sensor mode selection
// camera features
bool        CmdLineParser::m_enableLatencyCheck = false;  // latency check disabled by default
bool        CmdLineParser::m_isTestGenMode      = false;
float       CmdLineParser::m_zoom               = 1.0;    // zoom factor
int         CmdLineParser::m_hwFdType           = -1;     // -1 means --hwfdtype flag not set; pick the best matching
int         CmdLineParser::m_nCamera            = -1;     // -1 means --camera flag not set; select all cameras for testing
int         CmdLineParser::m_sensorMode         = -1;     // -1 means --sensormode flag not set; pick the best matching
std::string CmdLineParser::m_fdTestImage        = "";
UINT32      CmdLineParser::m_numEarlyPCRs       = 0;
bool        CmdLineParser::m_bZslMode           = false;  // enable ZSL mode for preview + snapshot usecases
bool        CmdLineParser::m_bRemoveSensor      = false;
UINT32      CmdLineParser::m_nExposures         = DEFAULT_EXPOSURE_COUNT;

// camera stream resolutions and formats
char const* CmdLineParser::m_inputImage     = ""; // initialize char array to 0
uint32_t    CmdLineParser::m_inputWidth     = 0;
uint32_t    CmdLineParser::m_inputHeight    = 0;
int         CmdLineParser::m_inputFormat    = -1; // -1 by default to indicate user did not provide
int         CmdLineParser::m_inputPort      = -1; // -1 by default to indicate user did not provide
uint32_t CmdLineParser::m_outputWidth       = 0;
uint32_t CmdLineParser::m_outputHeight      = 0;
int      CmdLineParser::m_outputFormat      = -1; // -1 by default to indicate user did not provide
int      CmdLineParser::m_outputPort        = -1; // -1 by default to indicate user did not provide

uint32_t CmdLineParser::m_rdiOutputWidth    = 0;
uint32_t CmdLineParser::m_rdiOutputHeight   = 0;

int      CmdLineParser::m_snapshotFormat    = -1; // Snapshot stream format
uint32_t CmdLineParser::m_snapshotWidth     = 0;  // Snapshot stream width
uint32_t CmdLineParser::m_snapshotHeight    = 0;  // Snapshot stream height

int      CmdLineParser::m_videoFormat       = -1; // Video stream format
uint32_t CmdLineParser::m_videoWidth        = 0;  // Video stream width
uint32_t CmdLineParser::m_videoHeight       = 0;  // Video stream height

// Dual FOV (or) Display full Parameters (port cannot be modified)
int         CmdLineParser::m_dff            = -1; // Display full/dual fov stream format
uint32_t    CmdLineParser::m_dfw            = 0;  // Display full/dual fov stream width
uint32_t    CmdLineParser::m_dfh            = 0;  // Display full/dual fov stream height

// flush related runtime parameters
int         CmdLineParser::m_flushDelay         = -1;
UINT32      CmdLineParser::m_flushFrame         = 0; // Frame number to trigger flush at
int         CmdLineParser::m_flushPipelineId    = -1;  // pipeline to flush in MC tests. -1 equals session flush.

CmdLineParser::switchport_t CmdLineParser::m_switchPortType = InvalidSwitch; // Default set to -1 or InvalidSwitch

///Buffer timeout (in milliseconds) is obtained by multiplying this by TIMEOUT_RETRIES
#ifdef _LINUX
int      CmdLineParser::m_timeout           = BUFFER_TIMEOUT_DEVICE_MILLI; // default is 1 second
#else
int      CmdLineParser::m_timeout           = BUFFER_TIMEOUT_DEVICE_MILLI * 10;
#endif

//Cmdline flag char* comparator func to check for match
struct  isDuplicate : public std::binary_function<const char*, const char*, bool> {
    bool operator()(const char* a, const char* b) const {
        return strcmp(a, b) < 0;
    }
};
std::set<const char*, isDuplicate> cmdFlagSet;         // Set used to detect duplicate flags

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// resetDefaultFlags
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CmdLineParser::resetDefaultFlags()
{
    // test case infrastructure
    CmdLineParser::m_nFrames          = DEFAULT_FRAME_COUNT;
    CmdLineParser::m_nSnaps           = DEFAULT_SNAPS_COUNT;
    CmdLineParser::m_bNoImageDump     = false;
    CmdLineParser::m_bZslMode         = false;
    strlcpy(CmdLineParser::m_camList, "[]", MAX_PATH - 1);
    CmdLineParser::m_nLog             = 0;
    CmdLineParser::m_caseName         = "";
    CmdLineParser::m_fullName         = "";
    CmdLineParser::m_suiteName        = "";

    // camera features
    CmdLineParser::m_enableLatencyCheck = false;  // latency check disabled by default
    CmdLineParser::m_isTestGenMode      = false;
    CmdLineParser::m_zoom               = 1.0;    // zoom factor
    CmdLineParser::m_hwFdType           = -1;     // -1 means --hwfdtype flag not set; pick the best matching
    CmdLineParser::m_nCamera            = -1;     // -1 means --camera flag not set; select all cameras for testing
    CmdLineParser::m_sensorMode         = -1;     // -1 means --sensormode flag not set; pick the best matching
    CmdLineParser::m_fdTestImage        = "";
    CmdLineParser::m_numEarlyPCRs       = 0;      // by default, 1 early PCR to be sent for respective test
    CmdLineParser::m_nExposures         = DEFAULT_EXPOSURE_COUNT;
    CmdLineParser::m_switchPortType     = InvalidSwitch;  // Default set to -1 or InvalidSwitch
    CmdLineParser::m_sensorCapsList.clear();
    CmdLineParser::m_bRemoveSensor      = false;

    // camera stream resolutions and formats
    CmdLineParser::m_inputImage        = ""; // initialize char array to 0
    CmdLineParser::m_inputWidth        = 0;
    CmdLineParser::m_inputHeight       = 0;
    CmdLineParser::m_inputFormat       = -1; // -1 by default to indicate user did not provide
    CmdLineParser::m_inputPort         = -1; // -1 by default to indicate user did not provide

    CmdLineParser::m_outputWidth       = 0;
    CmdLineParser::m_outputHeight      = 0;
    CmdLineParser::m_outputFormat      = -1; // -1 by default to indicate user did not provide
    CmdLineParser::m_outputPort        = -1; // -1 by default to indicate user did not provide

    CmdLineParser::m_rdiOutputWidth    = 0;
    CmdLineParser::m_rdiOutputHeight   = 0;

    CmdLineParser::m_snapshotFormat    = -1; // Snapshot stream format
    CmdLineParser::m_snapshotWidth     = 0;  // Snapshot stream width
    CmdLineParser::m_snapshotHeight    = 0;  // Snapshot stream height

    CmdLineParser::m_videoFormat       = -1; // Video stream format
    CmdLineParser::m_videoWidth        = 0;  // Video stream width
    CmdLineParser::m_videoHeight       = 0;  // Video stream height

    // Dual FOV (or) Display full Parameters (port cannot be modified)
    CmdLineParser::m_dff               = -1;    // Display full/dual fov stream format
    CmdLineParser::m_dfw               = 0;    // Display full/dual fov stream width
    CmdLineParser::m_dfh               = 0;    // Display full/dual fov stream height

    // flush related runtime parameters
    CmdLineParser::m_flushDelay        = -1;
    CmdLineParser::m_flushFrame        = 10; // Frame number to trigger flush at, in MC tests. Default : every 10th frame
    CmdLineParser::m_flushPipelineId   = -1; // pipeline to flush in MC tests. -1 equals session flush.

    ///Buffer timeout (in milliseconds) is obtained by multiplying this by TIMEOUT_RETRIES
#ifdef _LINUX
    CmdLineParser::m_timeout           = BUFFER_TIMEOUT_DEVICE_MILLI; // default is 1 second
#else
    CmdLineParser::m_timeout           = BUFFER_TIMEOUT_DEVICE_MILLI * 10;
#endif

    // NCX ported
    chitests::JsonParser::sBufferType   = BufferHeapIon;
    chitests::JsonParser::sbVideoFDTest = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ParseCommandLine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CmdLineParser::ParseCommandLine(int argc, char** argv)
{
    int getOptReturn;
    int foundJsonFile = 0; //Default value assume no Json input

    // Parse all short & long option flags to check for duplicates
    for (int i = 0; i < argc; i++)
    {
        if(strncmp(argv[i],"-", 1) == 0)
        {
            //Checks for duplicity using the boolean isDuplicate comparator function on the set
            if(!cmdFlagSet.insert(argv[i]).second)
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
            //long option,    DataRequirement,      flag,   short option/alias
            { "help",                  no_argument,        0,         'h' }, // --help    (-h)
            { "verbose",               required_argument,  0,         'v' }, // --verbose (-v) [val]
            { "testgen",               no_argument,        0,         'g' }, // --testgen (-tg)
            { "frames",                required_argument,  0,         'f' }, // --frames  (-f) [val]
            { "camera",                required_argument,  0,         'c' }, // --camera  (-c) [val]
            { "timeout",               required_argument,  0,         't' }, // --timeout (-t) [val]
            { "sm",                    required_argument,  0,         's' }, // --sm (-s) [val]
            { "zm",                    required_argument,  0,         'z' }, // --zm (-z)[val]

            // Run testsuite / testcase
            { "test",                  required_argument,  0,          0  }, // --test [val] Eg: [testName.testSuite]
            // Disable image dumping
            { "noimagedump",           no_argument,        0,          1  }, // --noimagedump
            { "nid",                   no_argument,        0,          2  }, // --nid
            // Input parameters for configurable tests
            { "inputimage",            required_argument,  0,          3  }, // --inputimage  [val]
            { "ii",                    required_argument,  0,          4  }, // --ii [val]
            { "inputwidth",            required_argument,  0,          5  }, // --inputwidth  [val]
            { "iw",                    required_argument,  0,          6  }, // --iw [val]
            { "inputheight",           required_argument,  0,          7  }, // --inputheight [val]
            { "ih",                    required_argument,  0,          8  }, // --ih [val]
            { "inputformat",           required_argument,  0,          9  }, // --inputformat [val]
            { "if",                    required_argument,  0,          10 }, // --if [val]
            { "inputport",             required_argument,  0,          11 }, // --inputport   [val]
            { "ip",                    required_argument,  0,          12 }, // --ip [val]
            // Output parameters for configurable tests
            { "outputwidth",           required_argument,  0,          13 }, // --outputwidth  [val]
            { "ow",                    required_argument,  0,          14 }, // --ow [val]
            { "outputheight",          required_argument,  0,          15 }, // --outputheight [val]
            { "oh",                    required_argument,  0,          16 }, // --oh [val]
            { "outputformat",          required_argument,  0,          17 }, // --outputformat [val]
            { "of",                    required_argument,  0,          18 }, // --of [val]
            // Snapshot stream
            { "sf",                    required_argument,  0,          19 }, // --sf [val] : Snapshot stream format
            { "sw",                    required_argument,  0,          20 }, // --sw [val] : Snapshot stream width
            { "sh",                    required_argument,  0,          21 }, // --sh [val] : Snapshot stream height

            // Video stream
            { "vf",                    required_argument,  0,          22 }, // --vf [val] : Video stream format
            { "vw",                    required_argument,  0,          23 }, // --vw [val] : Video stream width
            { "vh",                    required_argument,  0,          24 }, // --vh [val] : Video stream height

            { "outputport",            required_argument,  0,          25 }, // --outputport [val]
            { "op",                    required_argument,  0,          26 }, // --op [val]
            { "rdioutputwidth",        required_argument,  0,          27 }, // --rdioutputwidth [val]
            { "row",                   required_argument,  0,          28 }, // --row [val]
            { "rdioutputheight",       required_argument,  0,          29 }, // --rdioutputheight [val]
            { "roh",                   required_argument,  0,          30 }, // --roh [val]
            { "hwfdtype",              required_argument,  0,          31 }, // --hwfdtype [val]
            { "hwfd",                  required_argument,  0,          32 }, // --hwfd [val]

            { "fdtestimage",           required_argument,  0,          33 }, // --fdtestimage [val]
            { "fdti",                  required_argument,  0,          34 }, // --fdti [val]

            // Flush related parameters
            { "fd",               required_argument,  0,          36 }, // --fd [val]
            { "flushframe",       required_argument,  0,          37 }, // --flushframe [val]
            { "ff",               required_argument,  0,          38 }, // --ff [val]
            { "flushpipeline",    required_argument,  0,          39 }, // --flushpipeline [val]
            { "fpid",             required_argument,  0,          40 }, // --fpid [val]
            { "epcr",             required_argument,  0,          41 }, // --epcr [val]
            { "lc",               no_argument,        0,          42 }, // --lc

            // Display port Dual FOV parameters - so no port parameter required
            // dual fov width, dual fov height, dual fov format
            // display full width, display full height, display full format (based on context)
            { "dfw",                    required_argument,  0,         43 }, // --dfw [val]
            { "dfh",                    required_argument,  0,         44 }, // --dfh [val]
            { "dff",                    required_argument,  0,         45 }, // --dff [val]
            { "sn",                     required_argument,  0,         46 }, // --sn [val]
            { "zsl",                    no_argument,        0,         47 }, // --zsl

            // Sensor capabilities : combination of 2 or more flags below is allowed
            { "normal",            no_argument,        0,         48 }, // --normal
            { "hfr",               no_argument,        0,         49 }, // --hfr
            { "hdr",               required_argument,  0,         50 }, // --hdr [val] (supports zzhdr, ihdr, qhdr, shdr)
            { "pdaf",              no_argument,        0,         51 }, // --pdaf
            { "qcfa",              no_argument,        0,         52 }, // --qcfa
            { "depth",             no_argument,        0,         53 }, // --depth
            { "fs",                no_argument,        0,         54 }, // --fs
            { "insensorzoom",      no_argument,        0,         55 }, // --insensorzoom
            { "internal",          no_argument,        0,         56 }, // --internal
            { "rsn",               no_argument,        0,         57 }, // --rsn
            { "buffer_type",       required_argument,  0,         58 }, // --buffer_type
            { "videofd",           no_argument,        0,         59 }, // --videofd
            { "switchifefulltodisp",    no_argument,   0,         60 }, // --switchifefulltodisp
            { "switchifedisptofull",    no_argument,   0,         61 }, // --switchifedisptofull
            { "switchipedisptovideo",   no_argument,   0,         62 }, // --switchipefulltodisp
            { "switchipevideotodisp",   no_argument,   0,         63 }, // --switchipedisptofull
            { 0,                   0,                  0,          0 }  // last element is 0 to mark ending

        };

        getOptReturn = getopt_long(argc, argv,"hv:g:f:c:t:s:z:0:", cmdLineOptions, &optionIndex);
        if(getOptReturn == -1)
            break; // loop terminates when no more option characters, getopt_long() returns -1

        switch (getOptReturn)
        {
            case 'h':
                m_bHelp = true;
                break;
            case 'v':
                m_nLog = atoi(optarg);
                break;
            case 'g':
            {
                m_isTestGenMode = true;
                if(m_bRemoveSensor) // --rsn and --testgen are mutually exclusive
                {
                    m_bHelp = true;
                }
                foundJsonFile   = 0;
                break;
            }
            case 'f':
            {
                m_nFrames     = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 'c':
            {
                m_nCamera     = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 't':
            {
                m_timeout     = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 's':
            {
                m_sensorMode  = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 'z':
            {
                m_zoom = atof(optarg);
                foundJsonFile = 0;
                break;
            }
            case 0:
            {
                CmdLineParser::m_testcaseOrJson = optarg;
                // Checks if the test name has a Json file associated with it
                if (chitests::JsonParser::jsonFileCheck(CmdLineParser::m_testcaseOrJson))
                {
                    foundJsonFile = 1;
                }
                break;
            }
            case 1:
            case 2:
            {
                m_bNoImageDump = true;
                foundJsonFile  = 0;
                break;
            }
            case 3:
            case 4:
            {
                m_inputImage  = optarg;
                foundJsonFile = 0;
                break;
            }
            case 5:
            case 6:
            {
                m_inputWidth  = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 7:
            case 8:
            {
                m_inputHeight = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 9:
            case 10:
            {
                m_inputFormat = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 11:
            case 12:
            {
                m_inputPort   = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 13:
            case 14:
            {
                m_outputWidth = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 15:
            case 16:
            {
                m_outputHeight = atoi(optarg);
                foundJsonFile  = 0;
                break;
            }
            case 17:
            case 18:
            {
                m_outputFormat = atoi(optarg);
                foundJsonFile  = 0;
                break;
            }
            case 19:
            {
                m_snapshotFormat = atoi(optarg);
                foundJsonFile    = 0;
                break;
            }
            case 20:
            {
                m_snapshotWidth = atoi(optarg);
                foundJsonFile   = 0;
                break;
            }
            case 21:
            {
                m_snapshotHeight = atoi(optarg);
                foundJsonFile    = 0;
                break;
            }
            case 22:
            {
                m_videoFormat = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 23:
            {
                m_videoWidth  = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 24:
            {
                m_videoHeight = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 25:
            case 26:
            {
                m_outputPort  = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 27:
            case 28:
            {
                m_rdiOutputWidth = atoi(optarg);
                foundJsonFile    = 0;
                break;
            }
            case 29:
            case 30:
            {
                m_rdiOutputHeight = atoi(optarg);
                foundJsonFile     = 0;
                break;
            }
            case 31:
            case 32:
            {
                m_hwFdType    = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 33:
            case 34:
            {
                m_fdTestImage = optarg;
                foundJsonFile = 0;
                break;
            }
            case 36:
            {
                m_flushDelay  = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 37:
            case 38:
            {
                m_flushFrame  = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 39:
            case 40:
            {
                m_flushPipelineId = atoi(optarg);
                // todo: add this as a common pre-processor block for out of bounds check on any flag
                if(!(m_flushPipelineId >= 0 && m_flushPipelineId <= INT32_MAX))
                {
                    printf("Invalid pipelineID. Aborting test.");
                    return -1;
                }
                foundJsonFile     = 0;
                break;
            }
            case 41:
            {
                m_numEarlyPCRs = atoi(optarg);
                foundJsonFile  = 0;
                break;
            }
            case 42:
            {
                m_enableLatencyCheck = true;
                foundJsonFile   = 0;
                break;
            }
            case 43:
            {
                m_dfw = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 44:
            {
                m_dfh = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 45:
            {
                m_dff = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 46:
            {
                m_nSnaps = atoi(optarg);
                foundJsonFile = 0;
                break;
            }
            case 47:
            {
                m_bZslMode = true;
                foundJsonFile = 0;
                break;
            }
            case 48:
            {
                m_sensorCapsList.push_back(NORMAL);
                foundJsonFile = 0;
                break;
            }
            case 49:
            {
                m_sensorCapsList.push_back(HFR);
                foundJsonFile = 0;
                break;
            }
            case 50:
            {
                SensorCaps userCap = GetSensorCapEnum(optarg);
                if(userCap != MaxSensorCaps)
                {
                    m_sensorCapsList.push_back(userCap);
                }
                else
                {
                    m_bHelp = true;
                }
                foundJsonFile = 0;
                break;
            }
            case 51:
            {
                m_sensorCapsList.push_back(PDAF);
                foundJsonFile = 0;
                break;
            }
            case 52:
            {
                m_sensorCapsList.push_back(QCFA);
                foundJsonFile = 0;
                break;
            }
            case 53:
            {
                m_sensorCapsList.push_back(DEPTH);
                foundJsonFile = 0;
                break;
            }
            case 54:
            {
                m_sensorCapsList.push_back(FS);
                foundJsonFile = 0;
                break;
            }
            case 55:
            {
                m_sensorCapsList.push_back(INSENSORZOOM);
                foundJsonFile = 0;
                break;
            }
            case 56:
            {
                m_sensorCapsList.push_back(INTERNAL);
                foundJsonFile = 0;
                break;
            }
            case 57:
            {
                m_bRemoveSensor = true;
                if(m_isTestGenMode) // --rsn and --testgen are mutually exclusive
                {
                    m_bHelp = true;
                }
                foundJsonFile = 0;
                break;
            }
            case 58:
            {
                if(strcmp("EGL", optarg) == 0)
                {
                    chitests::JsonParser::sBufferType = BufferHeapEGL;
                }
                else if(strcmp("DSP", optarg) == 0)
                {
                    chitests::JsonParser::sBufferType = BufferHeapDSP;
                }
                else if(strcmp("SYS", optarg) == 0)
                {
                    chitests::JsonParser::sBufferType = BufferHeapSystem;
                }
                else if(strcmp("PSL", optarg) == 0) // pre-silicon
                {
                    chitests::JsonParser::sBufferType = 0xFFFFFFFF;
                }
                else
                {
                    chitests::JsonParser::sBufferType = BufferHeapIon;
                }
                foundJsonFile = 0;
                break;
            }
            case 59:
            {
                chitests::JsonParser::sbVideoFDTest = true;
                foundJsonFile  = 0;
                break;
            }
            case 60:
            {
                m_switchPortType = switchport_t::IfeFulltoDisp;
                foundJsonFile = 0;
                break;
            }
            case 61:
            {
                m_switchPortType = switchport_t::IfeDisptoFull;
                foundJsonFile = 0;
                break;
            }
            case 62:
            {
                m_switchPortType = switchport_t::IpeDisptoVideo;
                foundJsonFile = 0;
                break;
            }
            case 63:
            {
                m_switchPortType = switchport_t::IpeVideotoDisp;
                foundJsonFile = 0;
                break;
            }
            default:
            {
                m_bHelp = true;
                break;
            }
        }
    }

    if (foundJsonFile == 0)
    {
        std::string delimiter = ".";
        m_suiteName = CmdLineParser::m_testcaseOrJson .substr(0, CmdLineParser::m_testcaseOrJson.find(delimiter));  // extract suite name
        m_caseName  = CmdLineParser::m_testcaseOrJson .erase(0, m_suiteName.size() + 1);     // extract case name
        m_fullName  = m_suiteName + delimiter + m_caseName;         // create valid test full name [unused]
    }

    if (m_bHelp)
    {
        return -1;
    }

    return foundJsonFile;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PrintCommandLineUsage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CmdLineParser::PrintCommandLineUsage()
{
    printf("******************************** USAGE *********************************\n");
    printf(" --help    (-h)                   : print the usage instructions\n");
    printf(" --verbose (-v)        [val]      : logging levels. Insert number in range [0,7]\n");
    printf("                                    [0-entry & exit, 2-debug, 3-info, 4-performance, 5-unsupported\n");
    printf("                                     6-warning, 7-error] and above messages\n");
    printf(" --testgen (no alias)             : run test with dummy sensor node\n");
    printf(" --rsn                            : remove sensor node and enable testgen\n");
    printf(" --frames  (-f)        [val]      : override default number of frames to be dumped\n");
    printf(" --camera  (-c)        [val]      : override for camera selection to run test on\n");
    printf(" --sm      (no alias)  [val]      : override the sensor mode for given camera\n");
    printf(" --zm      (no alias)  [val]      : enable zoom factor for entire usecase \n");
    printf(" --timeout (-t)        [val]      : in milli-seconds while waiting for buffers/results (< 0 disables timeout)\n");
    printf(" --noimagedump (--nid)            : disable result image dumping to device\n");
    printf("\n");
    printf(" --test    (no alias)  [val]      : run the given testsuite / testcases(s)\n");
    printf("                                    usage: --test TestSuiteName.TestCase or TestSuiteName.*\n");
    printf("\n");
    printf(" --inputimage  (--ii)  [val]      : set the input image for offline recipe tests\n");
    printf(" --inputwidth  (--iw)  [val]      : set the input width for recipe tests\n");
    printf(" --inputheight (--ih)  [val]      : set the input height for recipe tests\n");
    printf(" --inputformat (--if)  [val]      : set the input stream format for recipe tests\n");
    printf(" --inputport   (--ip)  [val]      : set the input port on IFE/IPE/BPS for recipe tests\n");
    printf("\n");
    printf(" --outputformat (--of) [val]      : set the output format for preview streams\n");
    printf(" --outputwidth  (--ow) [val]      : set the output width  for preview streams\n");
    printf(" --outputheight (--oh) [val]      : set the output height for preview streams\n");
    printf("\n");
    printf(" --switchifefulltodisp (no alias) : switch IFEOutputPortFull to IFEOutputPortDisplayFull\n");
    printf(" --switchifedisptofull (no alias) : switch IFEOutputPortDisplayFull to IFEOutputPortFull\n");
    printf(" --switchipedisptovideo (no alias): switch IPEOutputPortDisplay to IPEOutputPortVideo\n");
    printf(" --switchipevideotodisp (no alias): switch IPEOutputPortVideo to IPEOutputPortDisplay\n");
    printf("\n");
    printf(" --dff (--dff) [val]              : set the Dual FOV/Display Full format for preview streams\n");
    printf(" --dfw  (--dfw) [val]             : set the Dual FOV/Display Full width  for preview streams\n");
    printf(" --dfh (--dfh) [val]              : set the Dual FOV/Display Full height for preview streams\n");
    printf("\n");
    printf(" --sf (no alias) [val]            : set the output format for snapshot streams\n");
    printf(" --sw (no alias) [val]            : set the output width  for snapshot streams\n");
    printf(" --sh (no alias) [val]            : set the output height for snapshot streams\n");
    printf("\n");
    printf(" --vf       (no alias) [val]      : set the output format for video streams\n");
    printf(" --vw       (no alias) [val]      : set the output width  for video streams\n");
    printf(" --vh       (no alias) [val]      : set the output height for video streams\n");
    printf("\n");
    printf(" --rdioutputwidth  (--row) [val]  : Set the RDI output width for recipe tests and few mixed pipeline tests \n");
    printf(" --rdioutputheight (--roh) [val]  : Set the RDI output height for recipe tests and few mixed pipeline tests\n");
    printf(" --outputport    (--op)    [val]  : set the output port on IFE/IPE/BPS for recipe tests\n");
    printf(" --hwfdtype      (--hwfd   [val]  : set the HW FD Type for FD HW tests\n");
    printf(" --fdtestimage   (--fdti)  [val]  : set the scene name for FD database testing\n");
    printf("\n");
    printf(" --fd                      [val]  : delay before triggering flush (microseconds)\n");
    printf(" --flushframe    (--ff)    [val]  : frame number to trigger flush at\n");
    printf(" --flushpipeline (--fpid)  [val]  : pipeline ID to trigger flush on \n");
    printf(" --epcr       (no alias)   [val]  : number of early PCRs before activating pipeline\n");
    printf(" --lc         (no alias)          : enables latency check for given test \n");
    printf(" --sn         (no alias)   [val]  : number of snaps to be taken in a test \n");
    printf(" --zsl        (no alias)          : enable ZSL mode for preview + snapshot usecase \n");
    printf("\n");
    printf(" --hdr [val = ihdr, zzhdr, qhdr, shdr] : Pick HDR sensor capability from given values\n");
    printf(" --pdaf         (no alias)        : Pick PDAF  sensor capability \n");
    printf(" --qcfa         (no alias)        : Pick QCFA  sensor capability \n");
    printf(" --fs           (no alias)        : Pick FS    sensor capability \n");
    printf(" --insensorzoom (no alias)        : Pick InSensorZoom sensor capability \n");
    printf(" --videofd      (no alias)        : Enable video database testing for FD pipelines \n");
    printf("************************************************************************\n");
    printf("More details on go/nativechiflags\n");
}
