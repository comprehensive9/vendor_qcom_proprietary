//=================================================================================================
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  haljsonparser.cpp
// @brief Source file for nativetest json parser
//=================================================================================================

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <sstream>
#include <stdexcept>
#include <csetjmp>
#include <csignal>
#include <cstdlib>
#include <regex>
#include "haljsonparser.h"

namespace haltests
{

    // static variable declarations
    // HDR related
    bool                  HalJsonParser::sAutoHDR          = false;
    bool                  HalJsonParser::sMFHDR            = false;
    bool                  HalJsonParser::sQHDR             = false;
    bool                  HalJsonParser::sSHDR             = false;
    bool                  HalJsonParser::sRandExpPattern   = false;
    INT32                 HalJsonParser::sNumExposures     = 0;
    std::vector<uint32_t> HalJsonParser::sExpPattern       = {};

    // MFNR related
    bool HalJsonParser::sMFNR    = false;

    // MCX realted
    bool HalJsonParser::sMCBOnly = false;

    // streams related
    int                                 HalJsonParser::sNumStreams = 0;
    std::vector<int>                    HalJsonParser::sRequestTypes = {};
    std::vector<int>                    HalJsonParser::sUsageFlags = {};
    std::vector<int>                    HalJsonParser::sDataspaces = {};
    std::vector<camera3_stream_type_t>  HalJsonParser::sDirections = {};
    std::vector<int>                    HalJsonParser::sFormats = {};
    std::vector<std::string>            HalJsonParser::sResolutions = {};
    std::vector<int>                    HalJsonParser::sStreamIntervals = {};

    //session related
    int HalJsonParser::sSessionMode = 0;

    // snapshots/frames related
    int HalJsonParser::sDuration = 1000;
    int HalJsonParser::sFrames   = 30;
    int HalJsonParser::sSnaps    = 1;

    // ZSL related
    bool HalJsonParser::sZSL = false;

    //Global variables
    jmp_buf enj;
    sig_atomic_t erjSignal = 0;
    std::string jsonExt    = ".json";
    std::string folderPath = "/vendor/etc/test/";
    bool bIsAbort          = false;  // Check if a test as aborted
    bool bSuiteCommonCheck = false;  // Global bool variable to apply flag
    char cmd[MAX_PATH]     = "";     // cmd flag which iterates through j_cmdDef & verifies mutual exclusivity
    std::string suiteVal         = "";
    std::string testNameVal      = "";
    std::string paramVal         = "";
    std::string testFull         = "";
    std::unordered_set<std::string> testSuite; //TestSuite(s) Name registered in NativeHalTest
    std::unordered_set<std::string> testCase; //TestCase(s) Name registered in NativeHalTest
    std::unordered_set<std::string> testFullName; // A collection of TestSuiteName.TestCaseName
    std::string suiteCommonFlags = "";  // Flags that are applied to all the tests in JSON
    std::unordered_map<std::string, std::string> commonFlags; // Flags that are applied to only tests in a given test suite
    jsonparser::fifo_map<std::string, std::string> testMap;   // Custom ordered map

    struct StrCompare : public std::binary_function<const char*, const char*, bool> {
    public:
        bool operator() (const char* str1, const char* str2) const
        {
            return strncmp(str1, str2, strlen(str2)) < 0;
        }
    };

    static std::map<char const*, HalJsonParser::JsnParamDef, StrCompare> j_cmdDef =
    {
        /*
        * cmd     : actual cmd flag passed by user
        * HasData : if the cmd is accompanied by a value
        * type    : if the cmd is a bool/int/string type
        * data    : variable to store the value for the cmd if cmd has data
        * mutuallyExList: if cmd flag has an associated flag that is mutually exclusive
        * userSetFlag: if the corresponding flag has been set
        */
        // | cmd              | HasData | type                          | data         | mutuallyExList   | userSetFlag |
        {"flush_delay",   {true,  HalJsonParser::TYPE_INT,         &HalCmdLineParser::g_flushDelay,        {},       false}},
        {"timeout",       {true,  HalJsonParser::TYPE_INT,         &HalCmdLineParser::g_timeout,           {},       false}},
        {"no_image_dump", {false, HalJsonParser::TYPE_BOOL,        &HalCmdLineParser::g_noImageDump,       {},       false}},
        {"camera",        {true,  HalJsonParser::TYPE_INT,         &HalCmdLineParser::g_camera,       { "camlist" }, false}},
        {"camfilter",     {true,  HalJsonParser::TYPE_STRING,      &HalCmdLineParser::g_camFilter,         {},       false}},
        {"camlist",       {true,  HalJsonParser::TYPE_STRING,      &HalCmdLineParser::g_camList,       {"camera"},   false}},
        {"random_seed",   {true,  HalJsonParser::TYPE_INT,         &HalCmdLineParser::g_randomseed,        {},       false}},
        {"verify_stats",  {true,  HalJsonParser::TYPE_INT,         &HalCmdLineParser::g_vstats,            {},       false}},
        {"qcfa",          {true,  HalJsonParser::TYPE_BOOL,        &HalCmdLineParser::g_isQCFA,            {},       false}},
        // ported to NativeHALX way
        // streams
        {"data_spaces",      {true,  HalJsonParser::TYPE_INT_LIST,     &HalJsonParser::sDataspaces,      {},       false}},
        {"directions",       {true,  HalJsonParser::TYPE_DIRECTIONS,   &HalJsonParser::sDirections,      {},       false}},
        {"formats",          {true,  HalJsonParser::TYPE_INT_LIST,     &HalJsonParser::sFormats,         {},       false}},
        {"num_streams",      {true,  HalJsonParser::TYPE_INT,          &HalJsonParser::sNumStreams,      {},       false}},
        {"request_types",    {true,  HalJsonParser::TYPE_INT_LIST,     &HalJsonParser::sRequestTypes,    {},       false}},
        {"resolutions",      {true,  HalJsonParser::TYPE_STRING_LIST,  &HalJsonParser::sResolutions,     {},       false}},
        {"usage_flags",      {true,  HalJsonParser::TYPE_INT_LIST,     &HalJsonParser::sUsageFlags,      {},       false}},
        {"stream_intervals", {true,  HalJsonParser::TYPE_INT_LIST,     &HalJsonParser::sStreamIntervals, {},       false}},

        //snapshot/frames related
        {"frames",        {true,  HalJsonParser::TYPE_INT,  &HalJsonParser::sFrames,   {}, false}},

        // hdr
        {"auto_hdr",             {true, HalJsonParser::TYPE_BOOL,         &HalJsonParser::sAutoHDR,       {"hdr"},    false}},
        {"hdr",                  {true, HalJsonParser::TYPE_INLINE_PARSE, &HalJsonParser::sSHDR,        {"auto_hdr"}, false}},
        {"num_exposures",        {true, HalJsonParser::TYPE_INT,          &HalJsonParser::sNumExposures,    {},       false}},
        {"perframe_hdr_pattern", {true, HalJsonParser::TYPE_INT_LIST,     &HalJsonParser::sExpPattern,      {},       false}},
        {"rand_hdr_pattern",     {true, HalJsonParser::TYPE_BOOL,         &HalJsonParser::sRandExpPattern,  {},       false}},

        // mcx
        {"mcb_only", {true,  HalJsonParser::TYPE_BOOL, &HalJsonParser::sMCBOnly, {}, false}},

        // mfnr
        {"mfnr", {true,  HalJsonParser::TYPE_BOOL, &HalJsonParser::sMFNR, {}, false}},

        // session
        {"session_mode", {true, HalJsonParser::TYPE_INT, &HalJsonParser::sSessionMode, {}, false}},

        // zsl related
        {"zsl", {true, HalJsonParser::TYPE_BOOL, &HalJsonParser::sZSL, {}, false}},
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Test framework related class/structure implementations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void jsonSignalHandler(int signum)
    {
        erjSignal = signum;
        signal(signum, SIG_DFL);
        longjmp(enj, 1);
    }

    int catchJsonErrorSignal(int(*func)())
    {
        int result = -1;
        if (setjmp(enj) == 0) //Initially env is set to 0 and continues with normal execution
        {
            signal(SIGABRT, &jsonSignalHandler); //Registers custom SignalHandler when specified signals are called
            signal(SIGSEGV, &jsonSignalHandler);
            signal(SIGFPE,  &jsonSignalHandler);
            signal(SIGILL,  &jsonSignalHandler);
            result = (*func)();
            signal(SIGABRT, SIG_DFL); //Registers custom SignalHandler when specified signals are called
            signal(SIGSEGV, SIG_DFL);
            signal(SIGFPE,  SIG_DFL);
            signal(SIGILL,  SIG_DFL);
        }
        else
        {
            switch (erjSignal)
            {
            case SIGILL:
            {
                NT_LOG_ERROR("Illegal Instruction Called !");
                NT_LOG_ERROR("%s.%s Stopped", HalCmdLineParser::m_suiteName.c_str(), HalCmdLineParser::m_caseName.c_str());
                ShutDown();
                break;
            }
            case SIGABRT:
            {
                NT_LOG_ERROR("Abort Called !");
                NT_LOG_ERROR("%s.%s Stopped", HalCmdLineParser::m_suiteName.c_str(), HalCmdLineParser::m_caseName.c_str());
                ShutDown();
                break;
            }
            case SIGSEGV:
            {
                NT_LOG_ERROR("Segmentation Violation Called  !");
                NT_LOG_ERROR("%s.%s Stopped", HalCmdLineParser::m_suiteName.c_str(), HalCmdLineParser::m_caseName.c_str());
                ShutDown();
                break;
            }
            case SIGFPE:
            {
                NT_LOG_ERROR("Floating-Point arithmetic Exception !");
                NT_LOG_ERROR("%s.%s Stopped", HalCmdLineParser::m_suiteName.c_str(), HalCmdLineParser::m_caseName.c_str());
                ShutDown();
                break;
            }
            }
            bIsAbort = true;
        }
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   HalJsonParser::jsonFileCheck
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool HalJsonParser::jsonFileCheck(std::string fileName)
    {
        std::string filePath = folderPath + fileName + jsonExt;
        std::ifstream fileObject(filePath.c_str());
        if (fileObject.is_open())
        {
            fileObject.close();
            return true;
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   HalJsonParser::jsonRecurse
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HalJsonParser::jsonRecurse(json& jsonObj, int *level)
    {
        //flag to indicate the end of structural pairs inside a given pair (key, value)
        bool bFinalLevel  = false;
        for (auto element = jsonObj.begin(); element != jsonObj.end(); ++element)
        {
            bool bSuiteCommon = false; //Local bool flag to differentiate TestSuite or common
            if (*level == 0) //Start with base (level = 0) key, value pair
            {
                suiteVal = element.key();
                if (strcmp(suiteVal.c_str(), "Common") == 0) //Check for common flags; applied for all tests
                {
                    suiteCommonFlags  = element.value().front().dump();
                    bSuiteCommon      = true;
                    bSuiteCommonCheck = true; //Global bool flag to apply flags in executeTest
                }
            }
            if (!bSuiteCommon) //Local bool flag that avoids suite common check at every iteration
            {
                //Second level where (Test Name, flag parameters) are parsed
                if (*level == 1)
                {
                    for (auto& el : element.value().items())
                    {
                        bool common = false; //Bool flag to parse common flags inside TestSuite
                        int check   = -1;
                        testNameVal = el.key();
                        paramVal    = el.value().dump();
                        if (strcmp(testNameVal.c_str(), "Common") == 0) //Extract specific testSuite's common flags
                        {
                            commonFlags.insert({ suiteVal, paramVal });
                            common = true;
                        }
                        if (!common)
                        {
                            //Check if TestSuite and TestName is registered
                            check = testCheck(suiteVal, testNameVal);
                            if (check == 0 || check == 1)
                            {
                                testFull = suiteVal + "." + testNameVal; //TestSuite.TestName
                                testMap.insert({ testFull, paramVal });  //Insert TestSuite.TestName and Flag values
                            }
                            else
                            {
                                NT_LOG_STD("--- TESTCASE : %s.%s mismatch ---", suiteVal.c_str(), testNameVal.c_str());
                                NT_LOG_STD("--- Parsing next test ---\n");
                            }
                        }
                    }
                }
                //Recurse through Json structure after each level (key,value) is parsed
                if (!element.value().is_object())
                {
                    if (element.value().is_structured())
                    {
                        (*level)++; //increments to next (key, value) pair
                        jsonRecurse(element.value(), level);
                        bFinalLevel = true;
                    }
                }
                if (bFinalLevel)
                {
                    *level = 0;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///   HalJsonParser::jsonParsing
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int HalJsonParser::jsonParsing(std::string fName)
    {
        //Returns success when all tests on Json passes
        int returnVal = 0;
        json masterObj;
        bool bIsAbortPrint = false;
        bool bErrorInput   = false;
        bool duplicateKeyFound = false;
        //Create json file path
        std::string filePath = folderPath + fName + jsonExt;
        std::ifstream fileObject(filePath);
        jsonparser::fifo_map<std::string, std::string>::iterator testLooper;
        jsonparser::fifo_map<std::string, std::string>::reverse_iterator testReverse = testMap.rbegin();

        //check if json file opened
        if (fileObject.is_open())
        {
            //parse json file to JSON object type
            try
            {
                fileObject >> masterObj;
            }
            catch (jsonparser::detail::other_error& o)
            {
                duplicateKeyFound = true;
            }
            catch (json::exception& e)
            {
                NT_LOG_ERROR("Invalid Json Input -- %s ", e.what());
                fileObject.close();
                return 1;
            }
            int level = 0; // param to recurse key:value pairs inside Json
            jsonRecurse(masterObj, &level);
            fileObject.close();
        }

        for (testLooper = testMap.begin(); testLooper != testMap.end(); testLooper++)
        {
            std::string delimiter         = ".";
            std::string tName             = testLooper->first;
            std::string testSuiteValue    = tName.substr(0, tName.find(delimiter));
            HalCmdLineParser::m_suiteName = testSuiteValue;
            HalCmdLineParser::m_caseName  = tName.erase(0, HalCmdLineParser::m_suiteName.size() + 1);
            HalCmdLineParser::m_fullName  = HalCmdLineParser::m_suiteName + "." + HalCmdLineParser::m_caseName;

            if (duplicateKeyFound && testReverse->first == testLooper->first)
            {
                // Prevent the final test parsed by jsonRecurse() which threw duplicate flag excpetion
                NT_LOG_ERROR("Duplicate key found in Json");
                NT_LOG_STD("Stopped parsing !");
                returnVal = 1;
                break;
            }
            if (!bIsAbortPrint)
            {
                try //try - catch for common flags applied to all tests
                {
                    if (bSuiteCommonCheck)
                    {
                        executeTest(json::parse(suiteCommonFlags), 0);
                    }
                }
                catch (json::exception& e)
                {
                    NT_LOG_INFO("Invalid flag input in Common");
                    NT_LOG_INFO("%s\n", e.what());
                    bErrorInput = true;
                }
                try //try - catch for common flags applied to a given TestSuite
                {
                    if (commonFlags.count(testSuiteValue) == 1)
                    {
                        std::unordered_map<std::string, std::string>::iterator paramLooper = commonFlags.find(testSuiteValue);
                        executeTest(json::parse(paramLooper->second), 0);
                    }
                }
                catch (json::exception& er)
                {
                    NT_LOG_INFO("Invalid flag input in %s {Common} ", HalCmdLineParser::m_fullName.c_str());
                    NT_LOG_INFO("%s\n", er.what());
                    bErrorInput = true;
                }
                try //try - catch for specific TestName in a TestSuite
                {
                    executeTest(json::parse(testLooper->second), 1);
                }
                catch (json::exception& e)
                {
                    NT_LOG_ERROR("Invalid flag input in %s ",HalCmdLineParser::m_fullName.c_str());
                    NT_LOG_ERROR("%s",e.what());
                    bErrorInput = true;
                    NT_LOG_INFO("Proceeding with next test..\n");
                }
            }
            else
            {
                NT_LOG_STD("%s", HalCmdLineParser::m_fullName.c_str());
            }
            if (bIsAbort)
            {
                printf("\n");
                bIsAbort      = false;
                bIsAbortPrint = true; // Flag to print remaining test casess after an abort signal
                returnVal = 1;
                NT_LOG_STD("--- Remaining Test(s) ---");
            }
            HalCmdLineParser::resetDefaultFlags();
            std::map<char const*, HalJsonParser::JsnParamDef>::iterator it;
            //Clear userSetFlag in JsnParamDef
            for (it = j_cmdDef.begin(); it != j_cmdDef.end(); it++)
            {
                j_cmdDef.at(it->first).userSetFlag = false;
            }
        }
        if (bErrorInput) // Prints Json flag usage
        {
            HalJsonParser::PrintCommandLineUsage();
            returnVal = 1;
        }

        ShutDown();
        return returnVal;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  verifyMutuallyExclusive()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool verifyMutuallyExclusive()
    {
        for (size_t i = 0; i < j_cmdDef.at(cmd).mutuallyExList.size(); i++)
        {
            if (j_cmdDef.at(j_cmdDef.at(cmd).mutuallyExList.at(i)).userSetFlag)
            {
                NT_LOG_ERROR("%s is mutually exclusive to %s. Cannot be used together.", cmd, j_cmdDef.at(cmd).mutuallyExList.at(i));
                return true;
            }
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  HalJsonParser::executeTest
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HalJsonParser::executeTest(json jsonObj, int run)
    {
        std::string assign = "";
        strlcpy(cmd, "", MAX_PATH - 1); // reset flag iterator to check mutual exclusivity

        // Loop through flag params (defined as cmd) inside j_cmdDef
        for (auto it : jsonObj.items())
        {
            std::string jsonKey = it.key();
            const char* jsonKeyChar = jsonKey.c_str();
            if (j_cmdDef.find(jsonKeyChar) != j_cmdDef.end())
            {
                JsnParamDef jsonParam = j_cmdDef.at(jsonKeyChar);
                strlcpy(cmd, jsonKeyChar, MAX_PATH - 1);
                auto jsonFileValue = jsonObj[jsonKey];
                switch (jsonParam.nDataType)
                {
                    case TYPE_INT:
                        *reinterpret_cast<int*>(jsonParam.pvData) = jsonFileValue;
                        if (strcmp(jsonKeyChar, "frames") == 0)
                        {
                            HalJsonParser::sDuration = -1;
                        }
                        else if (strcmp(jsonKeyChar, "duration") == 0)
                        {
                            HalJsonParser::sFrames = -1;
                        }
                        break;
                    case TYPE_STRING:
                        strlcpy(reinterpret_cast<char*>(jsonParam.pvData), std::string(jsonFileValue).c_str(), MAX_PATH - 1);
                        break;
                    case TYPE_BOOL:
                        *reinterpret_cast<bool*>(jsonParam.pvData) = jsonFileValue;
                        break;
                    case TYPE_INT_LIST:
                        {
                            std::vector<int> &v = *static_cast<std::vector<int>*>(jsonParam.pvData);
                            for (auto element = jsonObj[jsonKey].begin(); element != jsonObj[jsonKey].end(); ++element)
                            {
                                v.push_back(atoi(element.value().dump().c_str()));
                            }
                            break;
                        }
                    case TYPE_STRING_LIST:
                    case TYPE_INLINE_PARSE:
                        {
                            std::vector<std::string> &v = *static_cast<std::vector<std::string>*>(jsonParam.pvData);
                            for (auto element = jsonObj[jsonKey].begin(); element != jsonObj[jsonKey].end(); ++element)
                            {
                                std::string str = element.value().dump();
                                // To remove " from the input string
                                str.erase(remove(str.begin(), str.end(), '"'), str.end());
                                if (jsonParam.nDataType == TYPE_INLINE_PARSE)
                                {
                                    if (strcmp(jsonKeyChar, "hdr") == 0)
                                    {
                                        if ((strcmp(str.c_str(), "mfhdr") == 0) || (strcmp(str.c_str(), "MFHDR") == 0))
                                        {
                                            HalJsonParser::sMFHDR = true;
                                        }
                                        else if ((strcmp(str.c_str(), "shdr") == 0) || (strcmp(str.c_str(), "SHDR") == 0))
                                        {
                                            HalJsonParser::sSHDR = true;
                                        }
                                        else if ((strcmp(str.c_str(), "qhdr") == 0) || (strcmp(str.c_str(), "QHDR") == 0))
                                        {
                                            HalJsonParser::sQHDR = true;
                                        }
                                    }
                                }
                                else
                                {
                                    v.push_back(str);
                                }
                            }
                            break;
                        }
                    case TYPE_DIRECTIONS:
                        {
                            for (auto element = jsonObj[jsonKey].begin(); element != jsonObj[jsonKey].end(); ++element)
                            {
                                std::string str = element.value().dump();
                                // To remove " from the input string
                                str.erase(remove(str.begin(), str.end(), '"'), str.end());
                                if (strcmp(str.c_str(), "OUTPUT") == 0)
                                {
                                    HalJsonParser::sDirections.push_back(CAMERA3_STREAM_OUTPUT);
                                }
                                else if (strcmp(str.c_str(), "INPUT") == 0)
                                {
                                    HalJsonParser::sDirections.push_back(CAMERA3_STREAM_INPUT);
                                }
                                else if (strcmp(str.c_str(), "BIDIRECTONAL") == 0)
                                {
                                    HalJsonParser::sDirections.push_back(CAMERA3_STREAM_BIDIRECTIONAL);
                                }
                            }
                            break;
                        }
                    default:
                        NT_LOG_ERROR("JSON has unsupported datatype [%d] for [%s]", jsonParam.nDataType, jsonParam.pvData);
                        NT_ASSERT(false, "Unsupported data type for JSON");
                        break;
                }

                if (run == 1 && verifyMutuallyExclusive())
                {
                    NT_LOG_ERROR("%s did not run due to mutually exclusive flags\n\n", HalCmdLineParser::m_fullName.c_str());
                    run = 0;
                }
                j_cmdDef.at(cmd).userSetFlag = true;
            }
            else
            {
                NT_LOG_ERROR("skipped parsing unrecognized option: [%s]", jsonKeyChar);
            }
        }
        if (run == 1)
        {
            int result = catchJsonErrorSignal(&RunTests);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  HalJsonParser::testCheck
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int HalJsonParser::testCheck(std::string tSuite, std::string tName)
    {
        std::list<NativeTest*>::const_iterator it = NativeTest::NTFunctionContainer->m_TestListDefault.begin();
        std::list<NativeTest*>::const_iterator end = NativeTest::NTFunctionContainer->m_TestListDefault.end();

        bool isPartialMatch = false;
        std::string tFull = tSuite + "." + tName;
        for (; it != end; it++)
        {
            std::string regTestSuiteName((*it)->GetTestSuiteName());
            std::string regTestCaseName((*it)->GetTestCaseName());
            std::string testFull = regTestSuiteName + "." + regTestCaseName;

            if (testFullName.count(testFull) == 0)
            {
                testFullName.insert(testFull);
            }

            if (testSuite.count(regTestSuiteName) == 0)
            {
                testSuite.insert(regTestSuiteName);
                if (testCase.count(regTestCaseName) == 0)
                {
                    testCase.insert(regTestCaseName);
                }

                // testCaseName partial check
                if (strcmp(tName.c_str(), "*") != 0)
                {
                    std::string searchCase = tName + "(.*)";
                    std::regex  searchCaseObj(searchCase);
                    if (regex_match(std::string(regTestCaseName), searchCaseObj))
                    {
                        isPartialMatch = true;
                    }
                }

            }
            else
            {
                if (testCase.count(regTestCaseName) == 0)
                {
                    testCase.insert(regTestCaseName);
                }

                // testCaseName partial check
                if (strcmp(tName.c_str(), "*") != 0)
                {
                    std::string searchCase = tName + "(.*)";
                    std::regex  searchCaseObj(searchCase);
                    if (regex_match(std::string(regTestCaseName), searchCaseObj))
                    {
                        isPartialMatch = true;
                    }
                }

                }
            }

            if (testFullName.count(tFull) == 1)
            {
                return 1; //complete match
            }
            if (testFullName.count(tFull) == 0 && testSuite.count(tSuite) == 1 && testCase.count(tName) == 1)
            {
                return -1; //mismatch
            }
            if (testSuite.count(tSuite) == 1 && (isPartialMatch || strcmp(tName.c_str(), "*") == 0) )
            {
                return 0; //partial match
            }

             return -1; //no match
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HalJsonParser::ResetDefaultFlags
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HalJsonParser::ResetDefaultFlags()
    {
        // Native HALX Migrated
        // HDR related
        HalJsonParser::sAutoHDR          = false;
        HalJsonParser::sMFHDR            = false;
        HalJsonParser::sQHDR             = false;
        HalJsonParser::sRandExpPattern   = false;
        HalJsonParser::sSHDR             = false;
        HalJsonParser::sNumExposures     = 0;
        HalJsonParser::sExpPattern       = {};

        // MFNR related
        HalJsonParser::sMFNR    = false;

        // MCX realted
        HalJsonParser::sMCBOnly = false;

        // snaps/frames realted
        HalJsonParser::sDuration = 1000;
        HalJsonParser::sFrames   = NUM_IMAGES;
        HalJsonParser::sSnaps    = 1;

        // streams related
        HalJsonParser::sNumStreams      = 1;
        HalJsonParser::sRequestTypes    = {};
        HalJsonParser::sUsageFlags      = {};
        HalJsonParser::sDataspaces      = {};
        HalJsonParser::sDirections      = {};
        HalJsonParser::sFormats         = {};
        HalJsonParser::sResolutions     = {};
        HalJsonParser::sStreamIntervals = {};

        // session related
        HalJsonParser::sSessionMode = 0;

        // ZSL related
        HalJsonParser::sZSL = false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  HalJsonParser::PrintCommandLineUsage()
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void HalJsonParser::PrintCommandLineUsage()
    {
        printf("********************************** JSON FLAG USAGE ***************************************\n");
        printf("frames                [INT]           : override default number of frames to be dumped.\n");
        printf("flush_delay           [INT]           : override flush delay (usec) for flush tests\n");
        printf("mcb_only              [BOOL]          : enable only master callback\n");
        printf("mfnr                  [BOOL]          : enable MFNR on the testcase\n");
        printf("hdr                   [STRING]        : enable MFHDR/SHDR [MFHDR, SHDR] on the testcase\n");
        printf("no_image_dump         [BOOL]          : disables result image dumping to device\n");
        printf("camera                [INT]           : override for camera selection to run test on.\n");
        printf("                                        Mutually exclusive with --camlist\n");
        printf("timeout               [INT]           : timeout in seconds while waiting for buffers/results\n");
        printf("camfilter             [STRING]        : filter which camera types to test: physical, logical, all\n");
        printf("                                        Default is physical.\n");
        printf("camlist               [STRING]        : override for multiple cameras to run test on: [id1,id2].\n");
        printf("                                        Mutually exclusive with --camera (-c) \n");
        printf("random_seed           [INT]           : to initialize the pseudo random generator\n");
        printf("snaps                 [INT]           : number of snapshots to take every 30 frames. Default is 1. \n");
        printf("qcfa                  [BOOL]          : Enables qcfa sensor mode if supported on camera \n");
        printf("num_streams           [INT]           : number of streams in the request\n");
        printf("request_types         [INT LIST]      : stream request type\n");
        printf("formats               [INT LIST]      : output format of each stream: [34, 35]. \n");
        printf("resolutions           [STRING LIST]   : output resolution of each stream: [\"1920x1440\", \"1920x1440\"]. \n");
        printf("usage_flags           [INT LIST]      : gralloc usage flag of each stream: [0, 0] \n");
        printf("data_spaces           [INT LIST]      : dataspace of each stream: [0, 0] \n");
        printf("zsl                   [BOOL]          : Enable/Disable ZSL: true/false \n");
        printf("perframe_hdr_pattern  [INT LIST]      : Exposure Count per frame \n");
        printf("rand_hdr_pattern      [BOOL]          : random HDR Exposure Count per frame \n");
        printf("*******************************************************************************************\n");
    }

}
