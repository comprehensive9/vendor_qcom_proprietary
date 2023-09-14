////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 - 2021 Qualcomm Technologies, Inc.
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
#include <stdexcept>
#include <cstdarg>
#include <chrono>
#include <regex>
#include "nativetest.h"

namespace chitests {
    // Initialize static members
    NativeTest* NativeTest::m_pTestObj;
    NativeTest::NativeTestContainer* NativeTest::NTFunctionContainer = NULL;
    // Constants
    static const size_t cStringSize = 4096;
    // Forward Declarations
    VOID ShutDown();
    bool ComparePriorities(NativeTest *first, NativeTest *second);
    VOID ReportResults(NativeTest *funcObj);
    VOID RunTest(NativeTest *funcObj, int *totalPassedOut, int *totalFailedOut, bool *passedOut);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Test helper functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID signalHandler(int signum)
    {
        static char szException[cStringSize];

        (VOID)snprintf(szException, cStringSize,"Signal: %d", signum);
        throw new std::runtime_error(szException);
    }

    bool Check(NativeTest *funcObj, bool passed, const char *fileName, int lineNumber, const char* condition,
        const char* errorFmt, ...)
    {
        // Get error format and args
        va_list errorArgs;
        va_start(errorArgs, errorFmt);
        char errOutput[cStringSize] = "\0";
        (VOID)vsnprintf(errOutput, cStringSize, errorFmt, errorArgs);
        va_end(errorArgs);

        if (!funcObj)
        {
            NT_LOG_ERROR("NativeTest aborted: Invalid Class Object: %p", funcObj);
            //return false;
            exit(-1);
        }
        else
        {
            if (!passed)
            {
                funcObj->SetFailed();
                NT_LOG_ERROR("CONDITION FAILED! %s:%d \"%s: %s\"\n    %s\n",
                    fileName, lineNumber, CmdLineParser::GetTestFullName().c_str() , condition, errOutput)
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

    VOID RegisterFunction(NativeTest *funcObj)
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

    VOID ReportResults(NativeTest *funcObj)
    {
        char szOutput[cStringSize];
        int failed = funcObj->GetFailed();
        NT_LOG_STD("\"%s\" report -> %s\n", CmdLineParser::GetTestFullName().c_str(),
            (failed == 0) ? "[PASSED]" : "[FAILED]");
    }


    VOID RunTest(NativeTest *funcObj, int *totalPassedOut, int *totalFailedOut, bool *passedOut)
    {
        if (funcObj)
        {
            if (funcObj->m_NonPipelineSuites.find(funcObj->GetTestSuiteName()) != funcObj->m_NonPipelineSuites.end())
            {
                funcObj->Setup();
            }

            if (funcObj->GetFailed() == 0)
            {
                funcObj->SetTestObject(funcObj);
                funcObj->Run();
            }

            funcObj->Teardown();

            ReportResults(funcObj);
            *totalPassedOut += funcObj->GetPassed();
            *totalFailedOut += funcObj->GetFailed();

            if (funcObj->GetFailed() != 0)
            {
                *passedOut = false;
            }
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

        std::string testSuiteName;
        std::string testCaseName;
        std::string testFullName;

        if(!JsonParser::bIsJsonDefined)
        {
            // Get test to run from user
            testSuiteName = CmdLineParser::GetTestSuiteName();
            testCaseName  = CmdLineParser::GetTestCaseName();
            testFullName  = CmdLineParser::GetTestFullName();
        }
        else
        {
            // Hardcoded to run NativeCHIX (if json picked)
            testSuiteName = "NativeCHIX";
            testCaseName  = "TestDIYPipeline";
            testFullName  = "NativeCHIX.TestDIYPipeline";
        }

        // Treat wildcard input (*) as empty string
        if (testSuiteName == "*") testSuiteName = "";
        if (testCaseName == "*") testCaseName = "";

        // Register signal handlers to trap test case crashes
        signal(SIGSEGV, signalHandler);
        signal(SIGFPE, signalHandler);
        signal(SIGILL, signalHandler);

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
        /* Execution end time*/
        // Report
        NT_LOG_STD("Final Report -> [%d PASSED] and [%d FAILED] (Execution Time: %.02fs)\n",
            totalPassed, totalFailed, executionTime);

        if (passed)
        {
            return (0);
        }
        else
        {
            return (1);
        }
    }

    VOID ShutDown()
    {
        if (NativeTest::NTFunctionContainer)
        {
            delete NativeTest::NTFunctionContainer;
            NativeTest::NTFunctionContainer = NULL;
        }
    }
}
