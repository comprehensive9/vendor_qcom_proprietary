////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  nativetest.h
/// @brief Declarations for the test framework.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NATIVETEST_H
#define NATIVETEST_H

#include <stdio.h>
#include <list>
#include <set>
#include "camxcdktypes.h"
#include "cmdlineparser.h"

namespace chitests {
#ifndef NULL
#define NULL 0
#endif

// Determine the compiler's environment
#if ((ULONG_MAX) == (UINT_MAX))
#define ENVIRONMENT32
#else
#define ENVIRONMENT64
#endif

// General macros
#define STRINGIFY(x) #x
#define PASTE(x, y) PASTE_(x, y)
#define PASTE_(x, y) x##y
#define NT_NEW new
#define NATIVETEST_ELEMENTS(arr) (sizeof(arr) / sizeof(arr[0]))

// Registering test macros
#define NATIVETEST_TEST_INITIALIZE(_testsuite, _testcase)\
    NATIVETEST_TEST_ORDERED(_testsuite, _testcase, cNativeTestOrderInitialize)

#define NATIVETEST_TEST_SHUTDOWN(_testsuite, _testcase)\
    NATIVETEST_TEST_ORDERED(_testsuite, _testcase, cNativeTestOrderShutDown)

#define NATIVETEST_TEST(_testsuite, _testcase)\
    NATIVETEST_TEST_ORDERED(_testsuite, _testcase, cNativeTestOrderDefault)

#define NATIVETEST_CLASS_NAME(_testsuite, _testcase)\
    _testsuite_##_testcase##Name

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// NATIVETEST_TEST_ORDERED
///
/// @brief  Main macro to construct test class.
///         Inheritance order (base to derived): NativeTest -> ChiTestCase -> _testsuite -> macro_generated_class
///
/// @param  _testsuite  Class that holds the test case
/// @param  _testcase   Name of the test case
/// @param  _testorder  Priority of this test case
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NATIVETEST_TEST_ORDERED(_testsuite, _testcase, _testorder)\
class NATIVETEST_CLASS_NAME(_testsuite, _testcase) : public _testsuite\
{\
public:\
    NATIVETEST_CLASS_NAME(_testsuite, _testcase)()\
    {\
        static char suiteName[] = #_testsuite;\
        static char caseName[] = #_testcase;\
        static char fullName[] = #_testsuite "." #_testcase;\
        this->testSuiteName = suiteName;\
        this->testCaseName = caseName;\
        this->testFullName = fullName;\
        result = 0;\
        order = (int)_testorder;\
        RegisterFunction(this);\
        SetTestObject(this);\
    };\
    ~NATIVETEST_CLASS_NAME(_testsuite, _testcase)(){};\
private:\
    char *testSuiteName;\
    char *testCaseName;\
    char *testFullName;\
    int result;\
    int order;\
    _testsuite testSuiteInstance;\
public:\
    VOID Run();\
    int GetPassed()\
    {\
        return(result == 0? 1 : 0);\
    }\
    int GetFailed()\
    {\
        return(result > 0? 1 : 0);\
    }\
    VOID SetFailed()\
    {\
        result = 1;\
    }\
    char *GetTestSuiteName()\
    {\
        return(testSuiteName);\
    }\
    char *GetTestCaseName()\
    {\
        return(testCaseName);\
    }\
    char *GetTestFullName()\
    {\
        return(testFullName);\
    }\
    int GetOrder()\
    {\
        return(order);\
    }\
}    PASTE(NATIVETEST_CLASS_NAME(_testsuite, _testcase), Instance);\
VOID NATIVETEST_CLASS_NAME(_testsuite, _testcase)::Run()

// Constants
const int cNativeTestOrderLowest     = 0;
const int cNativeTestOrderHighest    = 0x7FFFFFFE;
const int cNativeTestOrderDefault    = -1;
const int cNativeTestOrderInitialize = -2;
const int cNativeTestOrderShutDown   = -3;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Test related class/structure implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NativeTest
{
public:
    NativeTest() {};
    virtual ~NativeTest() {};

    /* NativeTest and all derived will implement */
    virtual VOID  Setup() = 0;
    virtual VOID  Teardown() = 0;

    /* Will be overridden by macro-created-class */
    virtual VOID  Run() {};
    virtual int   GetPassed() { return 0; };
    virtual int   GetFailed() { return 0; };
    virtual VOID  SetFailed() {};
    virtual char* GetTestSuiteName() { return NULL; };
    virtual char* GetTestCaseName() { return NULL; };
    virtual char* GetTestFullName() { return NULL; };
    virtual int   GetOrder() { return 0; };

    const std::set<std::string> m_NonPipelineSuites = {
        "CameraModuleTest","CameraManagerTest", "BinaryCompatibilityTest", "ChiMetadataTest"};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Test framework related class/structure implementations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef struct NativeTestContainer
    {
    public:
        NativeTestContainer(): m_pTestListInitialize(NULL), m_pTestListShutDown(NULL)
        {
            (VOID)m_TestListDefault.empty();
            (VOID)m_TestListOrder.empty();
        }

        ~NativeTestContainer()
        {
            m_pTestListInitialize = NULL;
            m_pTestListShutDown = NULL;
            (VOID)m_TestListDefault.empty();
            (VOID)m_TestListOrder.empty();
        }

        std::list<NativeTest *> m_TestListDefault;
        std::list<NativeTest *> m_TestListOrder;

        NativeTest *m_pTestListInitialize;
        NativeTest *m_pTestListShutDown;

    }   NativeTestContainer;

    static NativeTest* GetTestObject() { return m_pTestObj; };
    VOID SetTestObject(NativeTest* testObj) { m_pTestObj = testObj; };
    static NativeTestContainer* NTFunctionContainer;
private:
    static NativeTest* m_pTestObj;
};

// Extern definitions
extern VOID RegisterFunction(NativeTest *funcObj);
extern int  RunTests();
extern VOID ShutDown();
extern bool Check(NativeTest *funcObj, bool passed, const char* fileName, int lineNumber, const char* condition,
    const char* errorFmt, ...);

#define NT_ASSERT(_condition, _errorfmt, ...)\
    if (false == Check(GetTestObject(), _condition, __FILENAME__, __LINE__, #_condition, _errorfmt, ##__VA_ARGS__))\
        return
#define NT_ASSERT2(_obj, _condition, _errorfmt, ...)\
    if (false == Check(_obj, _condition, __FILENAME__, __LINE__, #_condition, _errorfmt, ##__VA_ARGS__))\
        return

#define NT_EXPECT(_condition, _errorfmt, ...)\
    Check(GetTestObject(), _condition, __FILENAME__, __LINE__, #_condition, _errorfmt, ##__VA_ARGS__)
#define NT_EXPECT2(_obj, _condition, _errorfmt, ...)\
    Check(_obj, _condition, __FILENAME__, __LINE__, #_condition, _errorfmt, ##__VA_ARGS__)

#define NT_FAIL(_errorfmt, ...)\
    if (false == Check(this, false, __FILENAME__, __LINE__, "FAIL", _errorfmt, ##__VA_ARGS__))\
        return
#define NT_FAIL2(_obj, _errorfmt, ...)\
    if (false == Check(_obj, false, __FILENAME__, __LINE__, "FAIL", _errorfmt, ##__VA_ARGS__))\
        return
} // namespace chitests
#endif // NATIVETEST_H