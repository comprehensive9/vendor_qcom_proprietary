/*!
 * @file test_vpp_timer.c
 *
 * @cr
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Tests timer related routines
 */

#include <sys/types.h>
#include <unistd.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#include "vpp_dbg.h"

#include "vpp_timer.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

/************************************************************************
 * Local static variables
 ***********************************************************************/

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

static void vVppTimerTest_OnTimerExpiryU32Data(void *pvData)
{
    uint32_t *pu32Data = (uint32_t *)pvData;

    DVP_ASSERT_PTR_NNULL(pu32Data);
    if (pu32Data)
    {
        (*pu32Data)++;
    }
}

/************************************************************************
 * Test Functions
 ***********************************************************************/

TEST_SUITE_INIT(VppTimerSuiteInit)
{
}

TEST_SUITE_TERM(VppTimerSuiteTerm)
{
}

TEST_SETUP(VppTimerTestInit)
{
}

TEST_CLEANUP(VppTimerTestTerm)
{
}

TEST(VppTimerInitTerm)
{
    /*
     * Test that the init() and term() functions work properly.
     */

    void *pvTimer = NULL;
    uint32_t u32Data = 0;
    t_StVppTimerDuration stDuration = {5, 0};
    t_StVppTimerDuration stNanoDuration = {0, 1e8};

    t_StVppTimerCallback stOriginalCb;
    t_StVppTimerCallback stTempCb;

    stOriginalCb.pvData = &u32Data;
    stOriginalCb.pfOnTimerExpiry = vVppTimerTest_OnTimerExpiryU32Data;

    // All fields filled
    pvTimer = pvVppTimer_Init(stOriginalCb, stDuration);
    DVP_ASSERT_PTR_NNULL(pvTimer);
    vVppTimer_Term(pvTimer);
    pvTimer = NULL;

    // Callback's data being NULL
    stTempCb = stOriginalCb;
    stTempCb.pvData = NULL;
    pvTimer = pvVppTimer_Init(stTempCb, stDuration);
    DVP_ASSERT_PTR_NNULL(pvTimer);
    vVppTimer_Term(pvTimer);
    pvTimer = NULL;

    // Giving duration with seconds=0 but nanoseconds != 0
    pvTimer = pvVppTimer_Init(stOriginalCb, stNanoDuration);
    DVP_ASSERT_PTR_NNULL(pvTimer);
    vVppTimer_Term(pvTimer);
    pvTimer = NULL;
}

TEST(VppTimerInitTermInvalid)
{
    /*
     * Test invalid uses of Init() and Term().
     */

    void *pvTimer = NULL;
    uint32_t u32Data = 0;
    t_StVppTimerDuration stDuration = {5, 0};
    t_StVppTimerDuration stTempDuration = {0, 0};

    t_StVppTimerCallback stOriginalCb;
    t_StVppTimerCallback stTempCb;

    stOriginalCb.pvData = &u32Data;
    stOriginalCb.pfOnTimerExpiry = vVppTimerTest_OnTimerExpiryU32Data;

    // NULL callback function pointer
    stTempCb = stOriginalCb;
    stTempCb.pfOnTimerExpiry = NULL;
    pvTimer = pvVppTimer_Init(stTempCb, stDuration);
    DVP_ASSERT_PTR_NULL(pvTimer);

    // Duration with seconds = 0, nanoseconds = 0
    stTempDuration.u32Seconds = 0;
    stTempDuration.u32NanoSeconds = 0;
    pvTimer = pvVppTimer_Init(stOriginalCb, stTempDuration);
    DVP_ASSERT_PTR_NULL(pvTimer);

    // Duration with nanoseconds >= 1e9, since 1e9 nanoseconds is 1 second
    stTempDuration.u32Seconds = 0;
    stTempDuration.u32NanoSeconds = 1e9;
    pvTimer = pvVppTimer_Init(stOriginalCb, stTempDuration);
    DVP_ASSERT_PTR_NULL(pvTimer);

    // All fields being 0/NULL/empty
    stTempCb = stOriginalCb;
    stTempCb.pvData = NULL;
    stTempCb.pfOnTimerExpiry = NULL;
    stTempDuration.u32Seconds = 0;
    stTempDuration.u32NanoSeconds = 0;
    pvTimer = pvVppTimer_Init(stTempCb, stTempDuration);
    DVP_ASSERT_PTR_NULL(pvTimer);

    // Just making sure Term(NULL) doesn't cause a crash
    vVppTimer_Term(NULL);
}

TEST(VppTimerRestartStop)
{
    /*
     *  Test basic, valid functionality of Restart() and Stop().
     */
    void *pvTimer = NULL;
    uint32_t u32Data = 0;
    uint32_t u32Ret;
    t_StVppTimerDuration stDuration = {1, 0};

    t_StVppTimerCallback stCallbacks;
    stCallbacks.pvData = &u32Data;
    stCallbacks.pfOnTimerExpiry = vVppTimerTest_OnTimerExpiryU32Data;
    pvTimer = pvVppTimer_Init(stCallbacks, stDuration);
    DVP_ASSERT_PTR_NNULL(pvTimer);

    // stop with no start (ie. stop timer when timer is already stopped)
    u32Ret = u32VppTimer_Stop(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Valid cases
    u32Ret = u32VppTimer_Restart(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    usleep(500000); // sleep for 1/2 second
    u32Ret = u32VppTimer_Stop(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    // Assert timer expiry hasn't gone off
    sleep(stDuration.u32Seconds + 1);
    DVP_ASSERT_EQUAL(u32Data, 0);

    // Valid timeout case
    DVP_ASSERT_EQUAL(u32Data, 0);
    u32Ret = u32VppTimer_Restart(pvTimer);
    LOGI("waiting for timer expiry");
    sleep(stDuration.u32Seconds + 1);
    LOGI("done waiting for timer expiry");
    DVP_ASSERT_NEQUAL(u32Data, 0);

    // double restart
    u32Ret = u32VppTimer_Restart(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppTimer_Restart(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppTimer_Stop(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // double stop
    u32Ret = u32VppTimer_Restart(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppTimer_Stop(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppTimer_Stop(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Stop after expiry
    u32Ret = u32VppTimer_Restart(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    sleep(stDuration.u32Seconds + 1);
    u32Ret = u32VppTimer_Stop(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    vVppTimer_Term(pvTimer);
}

TEST(VppTimerRestartStopInvalid)
{
    /*
     * Test invalid Restart() and Stop() calls.
     */
    void *pvTimer = NULL;
    uint32_t u32Data = 0;
    uint32_t u32Ret;
    t_StVppTimerDuration stDuration = {1, 0};

    t_StVppTimerCallback stCallbacks;
    stCallbacks.pvData = &u32Data;
    stCallbacks.pfOnTimerExpiry = vVppTimerTest_OnTimerExpiryU32Data;
    pvTimer = pvVppTimer_Init(stCallbacks, stDuration);
    DVP_ASSERT_PTR_NNULL(pvTimer);

    // restart(NULL)
    u32Ret = u32VppTimer_Restart(NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // stop(NULL)
    u32Ret = u32VppTimer_Restart(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppTimer_Stop(NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppTimer_Stop(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    vVppTimer_Term(pvTimer);
}

TEST(VppTimerTermNoStop)
{
    /*
     * Test that Term() works fine without a Stop() call first.
     */
    void *pvTimer = NULL;
    uint32_t u32Data = 0;
    uint32_t u32Ret;
    t_StVppTimerDuration stDuration = {1, 0};

    t_StVppTimerCallback stCallbacks;
    stCallbacks.pvData = &u32Data;
    stCallbacks.pfOnTimerExpiry = vVppTimerTest_OnTimerExpiryU32Data;
    pvTimer = pvVppTimer_Init(stCallbacks, stDuration);
    DVP_ASSERT_PTR_NNULL(pvTimer);

    u32Ret = u32VppTimer_Restart(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    vVppTimer_Term(pvTimer);
}

TEST(VppTimerRestartMultiple)
{
    /*
     * Test Restart() functionality by running Restart() multiple times to
     * prevent a timer expiry.
     */
    void *pvTimer = NULL;
    uint32_t u32Data = 0;
    uint32_t u32Ret;
    t_StVppTimerDuration stDuration = {1, 0};
    uint32_t u32HalfTimeoutInUs = (1000000 * stDuration.u32Seconds) / 2;

    t_StVppTimerCallback stCallbacks;
    stCallbacks.pvData = &u32Data;
    stCallbacks.pfOnTimerExpiry = vVppTimerTest_OnTimerExpiryU32Data;
    pvTimer = pvVppTimer_Init(stCallbacks, stDuration);
    DVP_ASSERT_PTR_NNULL(pvTimer);
    u32Ret = u32VppTimer_Restart(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    for (uint32_t i = 0; i < 10; i++)
    {
        usleep(u32HalfTimeoutInUs);
        u32Ret = u32VppTimer_Restart(pvTimer);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        DVP_ASSERT_EQUAL(u32Data, 0);
    }
    u32Ret = u32VppTimer_Stop(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    DVP_ASSERT_EQUAL(u32Data, 0);

    vVppTimer_Term(pvTimer);
}

TEST(VppTimerNanoSecondDurationCarryOver)
{
    /*
     * Test that when setting a large nanosecond duration, the carry-over works
     * correctly.
     */
    void *pvTimer = NULL;
    uint32_t u32Data = 0;
    uint32_t u32Ret;
    t_StVppTimerDuration stAlmostOneSecondDuration = {0, 999999999};

    t_StVppTimerCallback stCallbacks;
    stCallbacks.pvData = &u32Data;
    stCallbacks.pfOnTimerExpiry = vVppTimerTest_OnTimerExpiryU32Data;
    pvTimer = pvVppTimer_Init(stCallbacks, stAlmostOneSecondDuration);
    DVP_ASSERT_PTR_NNULL(pvTimer);
    u32Ret = u32VppTimer_Restart(pvTimer);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    usleep(500000); // sleep 1/2 second
    DVP_ASSERT_EQUAL(u32Data, 0);   // verify expiry hasn't gone yet
    usleep(1000000);    // sleep 1 second
    DVP_ASSERT_EQUAL(u32Data, 1);   // verify expiry has gone

    vVppTimer_Term(pvTimer);
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES VppTimerTests[] = {
    TEST_CASE(VppTimerInitTerm),
    TEST_CASE(VppTimerInitTermInvalid),
    TEST_CASE(VppTimerRestartStop),
    TEST_CASE(VppTimerRestartStopInvalid),
    TEST_CASE(VppTimerTermNoStop),
    TEST_CASE(VppTimerRestartMultiple),
    TEST_CASE(VppTimerNanoSecondDurationCarryOver),
    TEST_CASE_NULL(),
};

TEST_SUITE(VppTimerSuite,
           "VppTimerTests",
           VppTimerSuiteInit,
           VppTimerSuiteTerm,
           VppTimerTestInit,
           VppTimerTestTerm,
           VppTimerTests);
