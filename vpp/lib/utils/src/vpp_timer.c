/*!
 * @file vpp_timer.c
 *
 * @cr
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vpp.h"
#include "vpp_def.h"
#define VPP_LOG_TAG     VPP_LOG_MODULE_TIMER_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_TIMER
#include "vpp_dbg.h"

#include "vpp_timer.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

#define NANOSECONDS_PER_SECOND  1e9

#define LOCK(cb)                pthread_mutex_lock(&(cb)->mutex)
#define UNLOCK(cb)              pthread_mutex_unlock(&(cb)->mutex)
#define SIGNAL(cb)              pthread_cond_signal(&(cb)->cond);
#define WAIT(cb)                pthread_cond_wait(&(cb)->cond, &(cb)->mutex);
#define TIMED_WAIT(cb, time)    pthread_cond_timedwait(&(cb)->cond, &(cb)->mutex, (time));

typedef enum {
    VPPTIMER_STATE_NULL,
    VPPTIMER_STATE_EXPIRED,
    VPPTIMER_STATE_STARTED,
} t_EVppTimerState;

typedef struct {
    t_StVppTimerCallback stCallback;
    t_StVppTimerDuration stDuration;

    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    struct timespec stExpiryTime;
    t_EVppTimerState eState;
    union {
        struct {
            uint32_t bWorkerThreadStarted:  1;
            uint32_t bWorkerThreadExit:     1;
        };
    };
} t_StVppTimer;

/************************************************************************
 * Local Functions
 ***********************************************************************/
static char *pcVppTimer_GetStateString(t_EVppTimerState eState)
{
    switch (eState)
    {
        case VPPTIMER_STATE_NULL:       return "NULL";
        case VPPTIMER_STATE_EXPIRED:    return "EXPIRED";
        case VPPTIMER_STATE_STARTED:    return "STARTED";
        default:                        return "UNKNOWN";
    }
}

static void vVppTimer_ChangeState(t_StVppTimer *pstTimer, t_EVppTimerState eState)
{
    LOGI("changing state from %s to %s",
         pcVppTimer_GetStateString(pstTimer->eState),
         pcVppTimer_GetStateString(eState));
    pstTimer->eState = eState;
}

static void vVppTimer_WaitForWorkerThreadStart(t_StVppTimer *pstTimer)
{
    VPP_RET_VOID_IF_NULL(pstTimer);

    LOCK(pstTimer);
    LOGI("waiting for worker thread to start...");
    while (!pstTimer->bWorkerThreadStarted)
    {
        WAIT(pstTimer);
    }
    LOGI("finished waiting for worker thread to start");
    UNLOCK(pstTimer);
}

static void vVppTimer_SignalWorkerThreadStart(t_StVppTimer *pstTimer)
{
    VPP_RET_VOID_IF_NULL(pstTimer);

    LOCK(pstTimer);
    LOGI("signalling worker thread is starting");
    pstTimer->bWorkerThreadStarted = VPP_TRUE;
    SIGNAL(pstTimer);
    UNLOCK(pstTimer);
}

static void vVppTimer_SignalWorkerThreadExit(t_StVppTimer *pstTimer)
{
    VPP_RET_VOID_IF_NULL(pstTimer);

    LOCK(pstTimer);
    LOGI("signalling worker thread to exit");
    pstTimer->bWorkerThreadExit = VPP_TRUE;
    SIGNAL(pstTimer);
    UNLOCK(pstTimer);
}

static void vVppTimer_RunClientCallback(t_StVppTimer *pstTimer)
{
    VPP_RET_VOID_IF_NULL(pstTimer);
    // This should never fail, but sanity check
    VPP_RET_VOID_IF_NULL(pstTimer->stCallback.pfOnTimerExpiry);

    LOGI("running client callback");
    pstTimer->stCallback.pfOnTimerExpiry(pstTimer->stCallback.pvData);
}

static uint32_t bVppTimer_WorkerThreadShouldSleep(t_StVppTimer *pstTimer)
{
    VPP_RET_IF_NULL(pstTimer, VPP_FALSE);

    if (pstTimer->bWorkerThreadExit)
    {
        LOGI("worker: wake up since thread should exit");
        return VPP_FALSE;
    }
    else if (pstTimer->eState == VPPTIMER_STATE_STARTED)
    {
        LOGI("worker: wake up since should wait for timeout");
        return VPP_FALSE;
    }

    return VPP_TRUE;
}

static void *pvVppTimer_WorkerThread(void *pvTimer)
{
    uint32_t bExpired = VPP_FALSE;
    int ret;
    t_StVppTimer *pstTimer = (t_StVppTimer *)pvTimer;

    VPP_RET_IF_NULL(pstTimer, NULL);

    vVppTimer_SignalWorkerThreadStart(pstTimer);

    while (1)
    {
        LOCK(pstTimer);
        while (bVppTimer_WorkerThreadShouldSleep(pstTimer))
        {
            LOGI("worker thread sleeping...");
            WAIT(pstTimer);
            LOGI("worker thread waking up...");
        }

        if (pstTimer->bWorkerThreadExit)
        {
            UNLOCK(pstTimer);
            break;
        }

        // This should be the last thing in the while-loop, so that the worker
        // will check other things above first before waiting for the timeout
        if (pstTimer->eState == VPPTIMER_STATE_STARTED)
        {
            struct timespec stExpiryTime = pstTimer->stExpiryTime;
            LOGI("worker thread timed sleeping...");
            ret = TIMED_WAIT(pstTimer, &stExpiryTime);
            LOGI("worker thread timed waking up, ret=%d", ret);
            if (ret == ETIMEDOUT)
            {
                vVppTimer_ChangeState(pstTimer, VPPTIMER_STATE_EXPIRED);
                bExpired = VPP_TRUE;
            }
            else if (ret)
            {
                LOGE("failed running timed_wait, ret=%d", ret);
            }
        }
        UNLOCK(pstTimer);

        if (bExpired)
        {
            bExpired = VPP_FALSE;
            vVppTimer_RunClientCallback(pstTimer);
        }
    }

    LOGI("worker thread exiting...");

    return NULL;
}

static uint32_t u32VppTimer_SetTimeout(t_StVppTimer *pstTimer,
                                       t_StVppTimerDuration stDuration)
{
    int ret;

    VPP_RET_IF_NULL(pstTimer, VPP_ERR_PARAM);

    ret = clock_gettime(CLOCK_REALTIME, &pstTimer->stExpiryTime);
    if (ret)
    {
        LOGE("failed to get time, ret=%d", ret);
        return VPP_ERR;
    }

    LOGI("setting timeout, now={%" PRIu64 ", %" PRIu64 "}, duration={%u, %u}",
         (uint64_t)pstTimer->stExpiryTime.tv_sec,
         (uint64_t)pstTimer->stExpiryTime.tv_nsec,
         stDuration.u32Seconds, stDuration.u32NanoSeconds);

    pstTimer->stExpiryTime.tv_sec += stDuration.u32Seconds;
    pstTimer->stExpiryTime.tv_nsec += stDuration.u32NanoSeconds;
    if (pstTimer->stExpiryTime.tv_nsec >= NANOSECONDS_PER_SECOND)
    {
        pstTimer->stExpiryTime.tv_sec++;
        pstTimer->stExpiryTime.tv_nsec -= NANOSECONDS_PER_SECOND;
    }

    LOGD("final timeout={%" PRIu64 ", %" PRIu64 "}",
         (uint64_t)pstTimer->stExpiryTime.tv_sec,
         (uint64_t)pstTimer->stExpiryTime.tv_nsec);

    return VPP_OK;
}

static uint32_t u32VppTimer_ClearTimeout(t_StVppTimer *pstTimer)
{
    VPP_RET_IF_NULL(pstTimer, VPP_ERR_PARAM);

    memset(&pstTimer->stExpiryTime, 0, sizeof(pstTimer->stExpiryTime));

    return VPP_OK;
}

/************************************************************************
 * Global Functions
 ***********************************************************************/

void *pvVppTimer_Init(t_StVppTimerCallback stCallback,
                      t_StVppTimerDuration stDuration)
{
    int ret;
    t_StVppTimer *pstTimer;

    if (!stCallback.pfOnTimerExpiry ||
        (stDuration.u32Seconds == 0 && stDuration.u32NanoSeconds == 0) ||
        stDuration.u32NanoSeconds >= NANOSECONDS_PER_SECOND)
    {
        LOGE("error params, callback=%p, seconds=%u, nanoseconds=%u",
             stCallback.pfOnTimerExpiry, stDuration.u32Seconds,
             stDuration.u32NanoSeconds);
        goto error_params;
    }

    pstTimer = calloc(1, sizeof(t_StVppTimer));
    if (!pstTimer)
    {
        LOGE("failed to allocate timer cb");
        goto error_calloc;
    }
    pstTimer->stCallback = stCallback;
    pstTimer->stDuration = stDuration;

    ret = pthread_mutex_init(&pstTimer->mutex, NULL);
    if (ret)
    {
        LOGE("failed to init mutex, ret=%d", ret);
        goto error_mutex_init;
    }

    ret = pthread_cond_init(&pstTimer->cond, NULL);
    if (ret)
    {
        LOGE("failed to init cond, ret=%d", ret);
        goto error_cond_init;
    }

    ret = pthread_create(&pstTimer->thread, NULL, pvVppTimer_WorkerThread,
                         pstTimer);
    if (ret)
    {
        LOGE("failed to create timer thread, ret=%d", ret);
        goto error_thread_create;
    }

    vVppTimer_WaitForWorkerThreadStart(pstTimer);

    vVppTimer_ChangeState(pstTimer, VPPTIMER_STATE_EXPIRED);

    return pstTimer;

error_thread_create:
    ret = pthread_cond_destroy(&pstTimer->cond);
    LOGE_IF(ret, "failed to destroy cond, ret=%d", ret);

error_cond_init:
    ret = pthread_mutex_destroy(&pstTimer->mutex);
    LOGE_IF(ret, "failed to destroy mutex, ret=%d", ret);

error_mutex_init:
    free(pstTimer);
    pstTimer = NULL;

error_calloc:
error_params:
    return NULL;
}

void vVppTimer_Term(void *pvTimer)
{
    int ret;
    uint32_t u32Ret;
    t_StVppTimer *pstTimer = (t_StVppTimer *)pvTimer;

    VPP_RET_VOID_IF_NULL(pstTimer);

    vVppTimer_SignalWorkerThreadExit(pstTimer);
    ret = pthread_join(pstTimer->thread, NULL);
    LOGE_IF(ret, "failed to join worker thread, ret=%d", ret);

    u32Ret = u32VppTimer_ClearTimeout(pstTimer);
    LOGE_IF(u32Ret != VPP_OK, "failed to clear timeout, ret=%u", u32Ret);

    ret = pthread_cond_destroy(&pstTimer->cond);
    LOGE_IF(ret, "failed to destroy cond, ret=%d", ret);

    ret = pthread_mutex_destroy(&pstTimer->mutex);
    LOGE_IF(ret, "failed to destroy mutex, ret=%d", ret);

    vVppTimer_ChangeState(pstTimer, VPPTIMER_STATE_NULL);

    free(pstTimer);
}

uint32_t u32VppTimer_Restart(void *pvTimer)
{
    uint32_t u32Ret = VPP_OK;
    t_StVppTimer *pstTimer = (t_StVppTimer *)pvTimer;

    VPP_RET_IF_NULL(pstTimer, VPP_ERR_PARAM);

    LOCK(pstTimer);

    u32Ret = u32VppTimer_SetTimeout(pstTimer, pstTimer->stDuration);
    if (u32Ret != VPP_OK)
    {
        LOGE("failed to set timeout, ret=%u", u32Ret);
        goto func_end;
    }

    vVppTimer_ChangeState(pstTimer, VPPTIMER_STATE_STARTED);
    SIGNAL(pstTimer);

func_end:
    UNLOCK(pstTimer);
    return u32Ret;
}

uint32_t u32VppTimer_Stop(void *pvTimer)
{
    uint32_t u32Ret = VPP_OK;
    t_StVppTimer *pstTimer = (t_StVppTimer *)pvTimer;

    VPP_RET_IF_NULL(pstTimer, VPP_ERR_PARAM);

    LOCK(pstTimer);

    if (pstTimer->eState == VPPTIMER_STATE_EXPIRED)
    {
        LOGD("timer already stopped");
    }
    else if (pstTimer->eState == VPPTIMER_STATE_STARTED)
    {
        LOGD("stopping timer");
        u32Ret = u32VppTimer_ClearTimeout(pstTimer);
        if (u32Ret != VPP_OK)
        {
            LOGE("failed to clear timeout, ret=%u", u32Ret);
        }
        else
        {
            vVppTimer_ChangeState(pstTimer, VPPTIMER_STATE_EXPIRED);
        }
        SIGNAL(pstTimer);
    }

    UNLOCK(pstTimer);
    return u32Ret;
}
