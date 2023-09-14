////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  nativetest.cpp
/// @brief Implementation of the test framework.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <list>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <csetjmp>
#include <stdexcept>
#include <cstdarg>
#include <chrono>
#include <regex>
#include <fstream>
#include <set>
#include "nativetest.h"

namespace haltests {
    // Initialize static members
    NativeTest* NativeTest::m_pTestObj;
    NativeTest::NativeTestContainer* NativeTest::NTFunctionContainer = NULL;
    // Constants
    static const size_t cStringSize = 1024;
    jmp_buf enb;
    sig_atomic_t errRSignal = 0;

    const std::string CAMXORFILE("/vendor/etc/camera/camxoverridesettings.txt");

    /*Camx Overrides*/
    std::map<std::string, std::string> m_camXORMap = {};

    /*Same Camx Overrides Groups*/
    const std::set<std::string> eisV2tests = {
        "VendorTagsTest.TestEISV2", "VendorTagsTest.TestEISV2NoVideo" };
    const std::set<std::string> eiseV3tests = {
        "VendorTagsTest.TestEISV3", "FlushTest.TestFlushRecordingEIS" };
    const std::set<std::string> hdrTests = {
        "FlushTest.TestFlushRecordingHDR" };
    const std::set<std::string> satTests = {
        "FlushTest.TestFlushPreviewSAT", "FlushTest.TestFlushBurstshot" };
    const std::set<std::string> swmfTests = {
        "FlushTest.TestFlushSnapshotSWMF", "StillCaptureTest.TestSnapshotSWMF" };
    const std::set<std::string> partialMetadataATests = {
        "PartialMetadataTest.TestPartialStaticSupportA" };
    const std::set<std::string> partialMetadataBTests = {
        "PartialMetadataTest.TestPartialStaticSupportB", "PartialMetadataTest.TestPartialOrderingB",
        "PartialMetadataTest.TestPartialDisjointB", "PartialMetadataTest.TestPartialEnableZSLB",
        "PartialMetadataTest.TestPartialFlushB" };
    const std::set<std::string> partialMetadataDTests = {
        "PartialMetadataTest.TestPartialStaticSupportD","PartialMetadataTest.TestPartialOrderingD",
        "PartialMetadataTest.TestPartialDisjointD", "PartialMetadataTest.TestPartialEnableZSLD",
        "PartialMetadataTest.TestPartialFlushD" };
    const std::set<std::string> partialMetadataETests = {
        "PartialMetadataTest.TestPartialStaticSupportE", "PartialMetadataTest.TestPartialOrderingE",
        "PartialMetadataTest.TestPartialDisjointE", "PartialMetadataTest.TestPartialEnableZSLE",
        "PartialMetadataTest.TestPartialFlushE" };
    const std::set<std::string> multiCameraTests = {
        "StillCaptureTest.TestPreviewBokeh", "StillCaptureTest.TestPreviewSAT",
        "FlushTest.TestFlushPreviewBokeh"};

    // Forward Declarations
    void ShutDown();
    bool ComparePriorities(NativeTest *first, NativeTest *second);
    void ReportResults(NativeTest *funcObj);
    void ReportAllResults(int totalPassed, int totalFailed);
    void RunTest(NativeTest *funcObj, int *totalPassedOut, int *totalFailedOut, bool *passedOut);
    int  catchRunErrorSignal(int(*func)());

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// EnableOverrides
    ///
    /// @brief  Enable overrides based on test
    ///
    /// @return bool
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool EnableOverrides(std::string testFullName);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// WriteCamXORMapToFile
    ///
    /// @brief  Write provided map to a file (camxoverrides)
    ///
    /// @param  writeMap    Map to write
    ///
    /// @return bool
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteCamXORMapToFile(std::map<std::string, std::string> writeMap);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ReadCamXOR
    ///
    /// @brief  Read camxoverridesettings.txt and store into a map
    ///
    /// @param  preserve    flag to indicate whether to store it to Map or not
    ///
    /// @return bool
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool ReadCamXOR(bool preserve = false);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Test helper functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void signalHandler(int signum)
    {
        errRSignal = signum;
        signal(signum, SIG_DFL);
        longjmp(enb, 1);
    }

    int catchRunErrorSignal(int(*func)())
    {
        int result = -1;
        if (setjmp(enb) == 0) //Initially env is set to 0 and continues with normal execution
        {
            signal(SIGABRT, &signalHandler); //Registers custom SignalHandler when specified signals are called
            signal(SIGSEGV, &signalHandler);
            signal(SIGFPE, &signalHandler);
            signal(SIGILL, &signalHandler);
            result = (*func)();
            signal(SIGABRT, SIG_DFL); //Registers custom SignalHandler when specified signals are called
            signal(SIGSEGV, SIG_DFL);
            signal(SIGFPE, SIG_DFL);
            signal(SIGILL, SIG_DFL);
        }
        else
        {
            switch (errRSignal)
            {
                    case SIGILL:
                    {
                        NT_LOG_ERROR("Illegal Instruction Called !");
                        NT_LOG_ERROR("%s Stopped", HalCmdLineParser::m_fullName.c_str());
                        ShutDown();
                        break;
                    }
                    case SIGABRT:
                    {
                        NT_LOG_ERROR("Abort Called !");
                        NT_LOG_ERROR("%s Stopped", HalCmdLineParser::m_fullName.c_str());
                        ShutDown();
                        break;
                    }
                    case SIGSEGV:
                    {
                        NT_LOG_ERROR("Segmentation Violation Called  !");
                        NT_LOG_ERROR("%s Stopped", HalCmdLineParser::m_fullName.c_str());
                        ShutDown();
                        break;
                    }
                    case SIGFPE:
                    {
                        NT_LOG_ERROR("Floating-Point arithmetic Exception !");
                        NT_LOG_ERROR("%s Stopped", HalCmdLineParser::m_fullName.c_str());
                        ShutDown();
                        break;
                    }
            }
        }
        return result;
    }

    bool Check(NativeTest *funcObj, bool passed, const char *fileName, int lineNumber, const char* condition,
        const char* errorFmt, ...)
    {
        // Get error format and args
        va_list errorArgs;
        va_start(errorArgs, errorFmt);
        char errOutput[cStringSize] = "\0";
        (void)vsnprintf(errOutput, cStringSize, errorFmt, errorArgs);
        va_end(errorArgs);

        if (!funcObj)
        {
            NT_LOG_ERROR("NativeTest aborted: Invalid Class Object: %pK", funcObj);
            //return false;
            exit(-1);
        }
        else
        {
            if (!passed)
            {
                funcObj->SetFailed();
                NT_LOG_ERROR("CONDITION FAILED! %s:%d \"%s: %s\"\n***********\n %s\n***********\n",
                    fileName, lineNumber, funcObj->GetTestFullName(), condition, errOutput);
            }
        }

        return (passed);
    }

    bool ComparePriorities(NativeTest *first, NativeTest *second)
    {
        if (first->GetOrder() < second->GetOrder())
        {
            return (true);
        }
        else
        {
            return (false);
        }
    }

    void RegisterFunction(NativeTest *funcObj)
    {
        if (NativeTest::NTFunctionContainer == 0)
        {
            NativeTest::NTFunctionContainer = new NativeTest::NativeTestContainer();
        }

        if (funcObj->GetOrder() == cNativeTestOrderDefault)
        {
            NativeTest::NTFunctionContainer->m_TestListDefault.push_back(funcObj);
        }
        else if ((funcObj->GetOrder() >= cNativeTestOrderLowest) && (funcObj->GetOrder() <= cNativeTestOrderHighest))
        {
            NativeTest::NTFunctionContainer->m_TestListOrder.push_back(funcObj);
            NativeTest::NTFunctionContainer->m_TestListOrder.sort(ComparePriorities);
        }
        else if (funcObj->GetOrder() == cNativeTestOrderInitialize)
        {
            NativeTest::NTFunctionContainer->m_pTestListInitialize = funcObj;
        }
        else if (funcObj->GetOrder() == cNativeTestOrderShutDown)
        {
            NativeTest::NTFunctionContainer->m_pTestListShutDown = funcObj;
        }
        else
        {
            NT_LOG_ERROR("NativeTest aborted: \"%s\" was assigned to an invalid order number = %d\n",
                funcObj->GetTestFullName(), funcObj->GetOrder());
            exit(-1);
        }
    }

    void ReportResults(NativeTest *funcObj)
    {
        int failed = funcObj->GetFailed();
        NT_LOG_STD("\"%s\" report -> %s\n", funcObj->GetTestFullName(),(failed == 0) ? "[PASSED]" : "[FAILED]");
    }


    void RunTest(NativeTest *funcObj, int *totalPassedOut, int *totalFailedOut, bool *passedOut)
    {
        if (funcObj)
        {
            //Apply override settings
            NT_ASSERT(EnableOverrides(funcObj->GetTestFullName()), "Unable to set overrides required for the test");
            funcObj->Setup();
            // Only run if setup has not failed
            if (NativeTest::GetHalTestObject()->GetFailed() == 0)
            {
                funcObj->SetTestObject(funcObj);
                funcObj->Run();
            }
            else
            {
                funcObj->SetTestObject(funcObj);
                funcObj->SetFailed();
            }

            funcObj->Teardown();
            ReportResults(funcObj);
            *totalPassedOut += funcObj->GetPassed();
            *totalFailedOut += funcObj->GetFailed();

            if (funcObj->GetFailed() != 0)
            {
                *passedOut = false;
            }
            NT_LOG_INFO("Restoring initial override settings present before the test");
            NT_ASSERT(WriteCamXORMapToFile(m_camXORMap), "Unable to reset camxoverride settings after the test");
        }
    }

    int RunTests()
    {
        bool passed = true;
        int totalPassed = 0;
        int totalFailed = 0;
        int totalRegistered = 0;
        int registeredTests = 0;
        int matchingTestCaseCount = 0;

        // pCurrent test either holds exact match or keeps tracks of partial matches
        NativeTest* pCurrentTest = nullptr;
        char szOutput[cStringSize];

        std::list<NativeTest *>::iterator i;
        std::list<NativeTest *>::iterator end;
        NativeTest* pTest = nullptr;

        // Flush stdout and stderr by default
        setbuf(stdout, NULL);

        /* Execution start time */
        auto startTime = std::chrono::steady_clock::now();

        // Get test to run from user
        std::string testSuiteName = HalCmdLineParser::GetTestSuiteName();
        std::string testCaseName  = HalCmdLineParser::GetTestCaseName();
        std::string testFullName  = HalCmdLineParser::GetTestFullName();

        // Treat wildcard input (*) as empty string
        if (testSuiteName == "*") testSuiteName = "";
        if (testCaseName == "*") testCaseName = "";

        /***
        Print out registered tests and find number of matching tests to run
        ***/
        // Run initialize
        RunTest(NativeTest::NTFunctionContainer->m_pTestListInitialize, &totalPassed, &totalFailed, &passed);

        // Unordered / Default tests
        end = NativeTest::NTFunctionContainer->m_TestListDefault.end();
        for (i = NativeTest::NTFunctionContainer->m_TestListDefault.begin();
            i != end;
            ++i)
        {
            totalRegistered++;
            pTest = reinterpret_cast<NativeTest *>(*i);
            std::string regTestSuiteName(pTest->GetTestSuiteName());
            std::string regTestCaseName(pTest->GetTestCaseName());

            if (nullptr == pCurrentTest) //iterate over other tests only for partial match
            {
                if (0 == testSuiteName.compare(regTestSuiteName) || testSuiteName == "")
                {
                    // check if testcase is exact match
                    if (testCaseName == regTestCaseName)
                    {
                        // assign match test object
                        pCurrentTest = pTest;
                        NTLog.SetTestName(testSuiteName.c_str(), testCaseName.c_str());
                        NT_LOG_STD("Now Running with exact match : %s ", testFullName.c_str());
                        RunTest(pCurrentTest, &totalPassed, &totalFailed, &passed);
                        matchingTestCaseCount = 0;
                        break;
                    }
                    // otherwise check for partial match
                    else
                    {
                        std::string searchCase = testCaseName + "(.*)";
                        std::regex searchCaseObj(searchCase);
                        if (regex_match(regTestCaseName, searchCaseObj) || testCaseName == "")
                        {
                            matchingTestCaseCount++;
                        }
                    }
                }
            }
        }

        if ((nullptr == pCurrentTest) && (0 != matchingTestCaseCount))
        {
            NT_LOG_STD("Running %d of %d Tests: %s ", matchingTestCaseCount, NativeTest::NTFunctionContainer->m_TestListDefault.size(),
                testCaseName.c_str());
            // Run Unordered / Default list
            //iterate over other tests only for partial match
            for (i = NativeTest::NTFunctionContainer->m_TestListDefault.begin();
                i != end;
                ++i)
            {
                pTest = reinterpret_cast<NativeTest*>(*i);
                std::string regTestSuiteName(pTest->GetTestSuiteName());
                std::string regTestCaseName(pTest->GetTestCaseName());
                std::string regTestFullName(pTest->GetTestFullName());
                if (0 == testSuiteName.compare(regTestSuiteName) || testSuiteName == "")
                {
                    std::string searchCase = testCaseName + "(.*)";
                    std::regex searchCaseObj(searchCase);
                    if (regex_match(regTestCaseName, searchCaseObj) || testCaseName == "")
                    {
                        pCurrentTest = pTest;
                        NT_LOG_STD("Now Running a partially matched test : %s ", regTestFullName.c_str());
                        NTLog.SetTestName(regTestSuiteName.c_str(), regTestCaseName.c_str());
                        RunTest(pTest, &totalPassed, &totalFailed, &passed);
                    }
                }

                if ((i == end) && (nullptr == pCurrentTest))
                {
                    NT_LOG_ERROR("No tests match the input. Aborting.");
                    break;
                }
            }
        }

        // Run shutdown
        RunTest(NativeTest::NTFunctionContainer->m_pTestListShutDown, &totalPassed, &totalFailed, &passed);

        auto endTime = std::chrono::steady_clock::now();
        float executionTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

        // Report
        /* Execution end time*/
        NT_LOG_STD("Final Report -> [%d PASSED] and [%d FAILED] (Execution Time: %.02fs)\n", totalPassed, totalFailed, executionTime);

        if (passed)
        {
            return (0);
        }
        else
        {
            return (1);
        }
    }

    void ShutDown()
    {
        if (NativeTest::NTFunctionContainer)
        {
            delete NativeTest::NTFunctionContainer;
            NativeTest::NTFunctionContainer = NULL;
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// EnableOverrides
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool EnableOverrides(std::string testFullName)
    {
        NT_LOG_INFO("Current overrides in the device are:");
        ReadCamXOR(true);

        std::map<std::string, std::string> camxorMapMirror = m_camXORMap;
        if (eisV2tests.find(testFullName) != eisV2tests.end())
        {
            camxorMapMirror["EISV2Enable"]     = "1";
            camxorMapMirror["enableICAInGrid"] = "1";
        }
        else if (eiseV3tests.find(testFullName) != eiseV3tests.end())
        {
            camxorMapMirror["EISV3Enable"]     = "1";
            camxorMapMirror["enableICAInGrid"] = "1";
        }
        else if (hdrTests.find(testFullName) != hdrTests.end())
        {
            camxorMapMirror["overrideForceSensorMode"] = "7";
        }
        else if (satTests.find(testFullName) != satTests.end())
        {
            camxorMapMirror["multiCameraEnable"]    = "TRUE";
            camxorMapMirror["multiCameraSATEnable"] = "TRUE";
            camxorMapMirror["multiCamera3ASync"]    = "MultiCamera3ASyncQTI";
        }
        else if (swmfTests.find(testFullName) != swmfTests.end())
        {
            camxorMapMirror["advanceFeatureMask"] = "0x8";
        }
        else if (multiCameraTests.find(testFullName) != multiCameraTests.end())
        {
            camxorMapMirror["multiCameraEnable"] = "TRUE";
        }
        else if (partialMetadataATests.find(testFullName) != partialMetadataATests.end())
        {
            camxorMapMirror["numMetadataResults"]   = "1";
            camxorMapMirror["enableCHIPartialData"] = "Disable";
        }
        else if (partialMetadataBTests.find(testFullName) != partialMetadataBTests.end())
        {
            camxorMapMirror["numMetadataResults"]   = "1";
            camxorMapMirror["enableCHIPartialData"] = "CHIPartialDataSeparate";
        }
        else if (partialMetadataDTests.find(testFullName) != partialMetadataDTests.end())
        {
            camxorMapMirror["numMetadataResults"]   = "2";
            camxorMapMirror["enableCHIPartialData"] = "CHIPartialDataSeparate";
        }
        else if (partialMetadataETests.find(testFullName) != partialMetadataETests.end())
        {
            camxorMapMirror["numMetadataResults"]   = "4";
            camxorMapMirror["enableCHIPartialData"] = "CHIPartialDataSeparate";
        }
        else
        {
            NT_LOG_INFO("Removing unnecessary overrides");
            camxorMapMirror.erase("EISV2Enable");               // Non-eis tests are not intended to run with this
            camxorMapMirror.erase("EISV3Enable");               // Non-eis tests are not intended to run with this
            camxorMapMirror.erase("enableICAInGrid");           // Non-eis tests are not intended to run with this
            camxorMapMirror.erase("numMetadataResults");        // Non-partial metadata tests are not intended to run with this
            camxorMapMirror.erase("enableCHIPartialData");      // Non-partial metadata tests are not intended to run with this
            camxorMapMirror.erase("overrideForceSensorMode");   // Other that flush hdr test, it is not required
            camxorMapMirror.erase("advanceFeatureMask");        // Needed only for SWMF tests
            camxorMapMirror.erase("multiCameraEnable");         // Other that multi camera tests, it is not required
            camxorMapMirror.erase("multiCameraSATEnable");      // Other that SAT tests, it is not required
            camxorMapMirror.erase("multiCamera3ASync");         // Other that SAT tests, it is not required
            camxorMapMirror.erase("multiCameraLogicalXMLFile"); // Needed for tests which uses wide sensor as primary
        }

        if (HalCmdLineParser::isQCFASize())
        {
            camxorMapMirror["exposeFullSizeForQCFA"]   = "TRUE";
            camxorMapMirror["AECGainThresholdForQCFA"] = "20.0";
        }
        else
        {
            camxorMapMirror.erase("exposeFullSizeForQCFA");   // Not required for non-qcfa tests
            camxorMapMirror.erase("AECGainThresholdForQCFA"); // Not required for non-qcfa tests
        }

        NT_LOG_INFO("Enabling below override settings:");
        return WriteCamXORMapToFile(camxorMapMirror);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ReadCamXOR
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool ReadCamXOR(bool preserve)
    {
        std::ifstream camxorFile{ CAMXORFILE };

        if (!camxorFile.is_open())
        {
            return false;
        }

        std::map<std::string, std::string> readORMap;

        while (camxorFile)
        {
            std::string setting{}, value{};
            getline(camxorFile, setting, '=') && getline(camxorFile, value);
            if (camxorFile) {
                readORMap[setting] = value;
            }
        }

        for (std::map<std::string, std::string>::iterator camxor = readORMap.begin(); camxor != readORMap.end(); ++camxor)
        {
            NT_LOG_INFO("setting %s: %s", camxor->first.c_str(), camxor->second.c_str());
        }

        if (preserve)
        {
            m_camXORMap = readORMap;
        }
        readORMap.clear();

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// WriteCamXORMapToFile
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteCamXORMapToFile(std::map<std::string, std::string> writeMap)
    {
        std::ofstream writecamxor(CAMXORFILE);

        if (!writecamxor.is_open()) {
            NT_LOG_ERROR("Unable to open file for writing camxoverrides");
            return false;
        }

        for (std::map<std::string, std::string>::iterator camxor = writeMap.begin(); camxor != writeMap.end(); ++camxor)
        {
            writecamxor << camxor->first.c_str();
            writecamxor << "=";
            writecamxor << camxor->second.c_str();
            writecamxor << "\n";
        }
        writecamxor.close();

        return ReadCamXOR();
    }
}
