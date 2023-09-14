/******************************************************************************
  @file    HintExtHandler.cpp
  @brief   Implementation of hint extensions

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cutils/log.h>

#include "HintExtHandler.h"
#include <pthread.h>
#include "client.h"
#include "OptsData.h"
#include "PerfThreadPool.h"

#define LOG_TAG    "ANDR-PERF-HINTEXT"

using namespace std;

HintExtHandler::HintExtHandler() {
    Reset();
}

HintExtHandler::~HintExtHandler() {
    Reset();
}

bool HintExtHandler::Register(uint32_t hintId, HintExtAction preAction, HintExtAction postAction) {
    lock_guard<mutex> lock(mMutex);
    if (mNumHandlers >= MAX_HANDLERS) {
        //no more registrations
        return false;
    }

    mExtHandlers[mNumHandlers].mHintId = hintId;
    mExtHandlers[mNumHandlers].mPreAction = preAction;
    mExtHandlers[mNumHandlers].mPostAction = postAction;
    mNumHandlers++;
    return true;
}

int HintExtHandler::FetchConfigPreAction(mpctl_msg_t *pMsg) {
    int i = 0;
    int retVal = 0;
    if (NULL == pMsg) {
        return retVal;
    }

    lock_guard<mutex> lock(mMutex);
    for (i=0; i < mNumHandlers; i++) {
        if ((pMsg->hint_id == mExtHandlers[i].mHintId) && mExtHandlers[i].mPreAction) {
            retVal = mExtHandlers[i].mPreAction(pMsg);
            break;
        }
    }
    return retVal;
}

int HintExtHandler::FetchConfigPostAction(mpctl_msg_t *pMsg) {
    int i = 0;
    if (NULL == pMsg) {
        return FAILED;
    }

    lock_guard<mutex> lock(mMutex);
    for (i=0; i < mNumHandlers; i++) {
        if ((pMsg->hint_id == mExtHandlers[i].mHintId) && mExtHandlers[i].mPostAction) {
            mExtHandlers[i].mPostAction(pMsg);
            break;
        }
    }
    return SUCCESS;
}

void HintExtHandler::Reset() {
    lock_guard<mutex> lock(mMutex);
    int i = 0;
    mNumHandlers = 0;

    for (i=0; i < MAX_HANDLERS; i++) {
        mExtHandlers[i].mHintId = 0;
        mExtHandlers[i].mPreAction = NULL;
        mExtHandlers[i].mPostAction = NULL;
    }
    return;
}

//hint extension actions by modules
/**taskboost's fecth config post action
 * since perfhint attach_application used the param slot which is designed to pass timeout to pass process id
 * so for this perfhint, we also use the timeout defined in xml
 */
int TaskBoostAction::TaskBoostPostAction(mpctl_msg_t *pMsg) {
    if (NULL == pMsg) {
        return FAILED;
    }

    int size = pMsg->data;

    if (size > MAX_ARGS_PER_REQUEST)
        size = MAX_ARGS_PER_REQUEST;

    if (pMsg->hint_id == VENDOR_HINT_BOOST_RENDERTHREAD && pMsg->pl_time > 0) {
        renderThreadTidOfTopApp = pMsg->pl_time;
        for (int i = 0; i < size-1; i = i + 2) {
            if(pMsg->pl_args[i] == MPCTLV3_SCHED_ENABLE_TASK_BOOST_RENDERTHREAD)
            {
                QLOGI("renderThreadTidOfTopApp:%d, currentFPS:%d",
                       renderThreadTidOfTopApp, FpsUpdateAction::getInstance().GetFps());
                if(FpsUpdateAction::getInstance().GetFps() < FpsUpdateAction::getInstance().FPS144)
                {
                    pMsg->pl_args[i] = MPCTLV3_SCHED_DISABLE_TASK_BOOST_RENDERTHREAD;
                }
                pMsg->pl_args[i+1] = pMsg->pl_time;
                break;
            }
        }
        pMsg->pl_time = -1;
    }

    if (pMsg->hint_type == LAUNCH_TYPE_ATTACH_APPLICATION) {
        for (int i = 0; i < size-1; i = i + 2) {
            if (pMsg->pl_args[i] == MPCTLV3_SCHED_TASK_BOOST) {
                pMsg->pl_args[i+1] = pMsg->pl_time;
                break;
            }
        }
        pMsg->pl_time = -1;
    }
    return SUCCESS;
}

int DisplayEarlyWakeupAction::DisplayEarlyWakeupPreAction(mpctl_msg_t *pMsg) {
    if (NULL == pMsg) {
        return FAILED;
    }

    int hintType = pMsg->hint_type;
    if (hintType == 0xFFFF) {
        hintType = 0xFFFFFFFF;
    }
    OptsData &d = OptsData::getInstance();
    d.setEarlyWakeupDispId(hintType);

    // Just set the hint type to 0
    pMsg->hint_type = 0;

    QLOGI("drmIOCTL DisplayEarlyWakeupPreAction hintid: %d, hint_type: %d, \
            displayId: %d", pMsg->hint_id, pMsg->hint_type, d.getEarlyWakeupDispId());
    return SUCCESS;
}

FpsUpdateAction::FpsUpdateAction() {
    mCurFps = 0;
    mHandle = -1;
    mFpsHandleToRelease = -1;
    mTimer = 0;
    mFpsTimerCreated = false;
    mFpsValAfterTimerExpires = 0;

    char fpsSwitchHystTimeProperty[PROPERTY_VALUE_MAX];
    strlcpy(fpsSwitchHystTimeProperty,
            perf_get_prop("vendor.perf.fps_switch_hyst_time_secs","0").value,PROPERTY_VALUE_MAX);
    fpsSwitchHystTimeProperty[PROPERTY_VALUE_MAX-1]='\0';
    mFpsSwitchHystTime = strtod(fpsSwitchHystTimeProperty, NULL);
    QLOGI("fpsSwitchHystTime: %.2f secs", mFpsSwitchHystTime);
    if(mFpsSwitchHystTime < FPS_SWITCH_MIN_HYST_TIME_SECS) {
        QLOGE("FPS Switch Hysteresis time < %d secs, ignore it", FPS_SWITCH_MIN_HYST_TIME_SECS);
    }
}

FpsUpdateAction::~FpsUpdateAction() {
    mCurFps = 0;
    mHandle = -1;
    mFpsHandleToRelease = -1;
    mFpsSwitchHystTime = 0;
}

float FpsUpdateAction::getFpsSwitchHystTimeSecs() {
    char fpsSwitchHystTimeProperty[PROPERTY_VALUE_MAX];
    float hystTimeSec = 0.0f;

    OptsData &d = OptsData::getInstance();
    float dFpsHystTime = d.get_fps_hyst_time();
    if(dFpsHystTime != -1.0f) {
        hystTimeSec = dFpsHystTime;
    }
    else {
        hystTimeSec = mFpsSwitchHystTime;
    }
    return hystTimeSec;
}

//If fps value sent by AOSP is not standard, then standardize it as below
void FpsUpdateAction::StandardizeFps(int32_t &fps) {
    if (fps <= FPS30_MAX_RANGE) {
        fps = FPS30;
    }
    else if (fps <= FPS45_MAX_RANGE) {
        fps = FPS45;
    }
    else if (fps <= FPS60_MAX_RANGE) {
        fps = FPS60;
    }
    else if (fps <= FPS90_MAX_RANGE) {
        fps = FPS90;
    }
    else if (fps <= FPS120_MAX_RANGE) {
        fps = FPS120;
    }
    else if (fps <= FPS144_MAX_RANGE) {
        fps = FPS144;
    }
    else if (fps <= FPS180_MAX_RANGE) {
        fps = FPS180;
    }
    else if (fps <= FPS_MAX_LIMIT) {
        fps = FPS240;
    }
}

void FpsUpdateAction::switchToNewFps(sigval_t pvData) {
    FILE *fpsVal;
    char buf[NODE_MAX];
    int locFps = 0;
    int handle = -1;
    int rc = FAILED;
    bool resized = false;
    int tmpHandle = -1;
    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
    fpsVal = fopen(CURRENT_FPS_FILE, "w");
    if (fpsVal == NULL) {
        QLOGE("Could not open/create fps file, couldn't update to new FPS");
        pFpsUpdateObj.DeleteTimer();
        return;
    }
    QLOGI("Timer expired, switch to new fps: %d", pFpsUpdateObj.mFpsValAfterTimerExpires);
    {
        lock_guard<mutex> lock(pFpsUpdateObj.mMutex);
        locFps = pFpsUpdateObj.mFpsValAfterTimerExpires;
        pFpsUpdateObj.mCurFps = pFpsUpdateObj.mFpsValAfterTimerExpires;
        if (pFpsUpdateObj.mHandle > 0) {
            PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
            tmpHandle = pFpsUpdateObj.mHandle;
            do {
                rc = ptp.placeTask([=]() {
                        QLOGI("Offloading CallHintReset from %s", __func__);
                        perf_lock_rel(tmpHandle);
                    });
                if (rc == FAILED && resized == false) {
                    int new_size = ptp.resize(1);
                    QLOGW("Failed to Offload CallHintReset from %s\n ThreadPool resized to : %d",__func__ , new_size);
                    resized = true;
                } else {
                    break;
                }
            } while(true);
            if (rc == SUCCESS) {
                pFpsUpdateObj.mHandle = -1;
                QLOGI("Offloaded new thread from %s", __func__);
            } else {
                QLOGE("FAILED to offload from %s", __func__);
                QLOGE("%d FPS release Failed for Handle %d", pFpsUpdateObj.mCurFps, pFpsUpdateObj.mHandle);
            }
        }
    }

    snprintf(buf, NODE_MAX, "%d", locFps);
    fwrite(buf, sizeof(char), strlen(buf), fpsVal);
    fclose(fpsVal);
    if (rc == SUCCESS) {
        handle = perf_hint(VENDOR_HINT_FPS_IMMEDIATE_UPDATE, nullptr, 0, locFps);
        {
            lock_guard<mutex> lock(pFpsUpdateObj.mMutex);
            pFpsUpdateObj.mHandle = handle;
        }
    }
    QLOGI("Switched to %d fps successfully, saved fps hint hyst handle : %d", locFps, handle);
    pFpsUpdateObj.DeleteTimer();
}

int FpsUpdateAction::CreateTimer(int req_handle) {
    int rc = FAILED;
    struct sigevent sigEvent;
    sigEvent.sigev_notify = SIGEV_THREAD;
    sigEvent.sigev_notify_function = &FpsUpdateAction::switchToNewFps;
    sigEvent.sigev_notify_attributes = NULL;
    sigEvent.sigev_value.sival_int = req_handle;
    rc = timer_create(CLOCK_MONOTONIC, &sigEvent, &mTimer);
    if (rc != 0) {
        QLOGE("Failed to create timer");
        return rc;
    }
    QLOGI("Created fps switch timer");
    mFpsTimerCreated = true;
    return rc;
}

int FpsUpdateAction::SetTimer() {
    int rc = FAILED;
    struct itimerspec mTimeSpec;
    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
    float hystTimeSec = pFpsUpdateObj.getFpsSwitchHystTimeSecs();
    mTimeSpec.it_value.tv_sec = hystTimeSec;
    mTimeSpec.it_value.tv_nsec = 0;
    mTimeSpec.it_interval.tv_sec = 0;
    mTimeSpec.it_interval.tv_nsec = 0;
    if (mFpsTimerCreated) {
        rc = timer_settime(mTimer, 0, &mTimeSpec, NULL);
        if (rc != 0) {
            QLOGE("Failed to set timer, rc:%d", rc);
        }
        else {
            QLOGI("Fps Switch Timer %0.2f secs set", hystTimeSec);
        }
    }
    return rc;
}

int FpsUpdateAction::DeleteTimer() {
    int rc = FAILED;
    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
    pFpsUpdateObj.mFpsValAfterTimerExpires = 0;
    if (mFpsTimerCreated) {
        mFpsTimerCreated = false;
        rc = timer_delete(mTimer);
        if (rc != 0) {
            QLOGE("Failed to delete timer, rc:%d", rc);
        }
    }
    else {
        QLOGI("mFpsTimerCreated is false, so deleteTimer is NA");
    }
    return rc;
}

int FpsUpdateAction::createFpsSwitchTimer() {
    int rc = FAILED;
    int req_handle = 1;
    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();
    if (mFpsTimerCreated == false) {
        rc = pFpsUpdateObj.CreateTimer(req_handle);
    }
    if(mFpsTimerCreated == true) {
        rc = pFpsUpdateObj.SetTimer();
        if(rc != 0) {
            QLOGE("setTimer unsuccessful, hence deleting timer");
            pFpsUpdateObj.DeleteTimer();
        }
    }
    return rc;
}

int FpsUpdateAction::FpsUpdatePreAction(mpctl_msg_t *pMsg) {
    FILE *fpsVal;
    char buf[NODE_MAX];
    int locFps = 0;
    int rc = FAILED;
    bool resized = false;
    int tmpHandle = -1;

    if (NULL == pMsg) {
        QLOGE("FpsUpdatePreAction: pMsg is NULL");
        return PRE_ACTION_NO_FPS_UPDATE;
    }
    pMsg->offloadFlag = false;

    FpsUpdateAction &pFpsUpdateObj = FpsUpdateAction::getInstance();

    if (pMsg->hint_type < FPS_MIN_LIMIT || pMsg->hint_type > FPS_MAX_LIMIT) {
        QLOGE("FPS Update for values < %d & > %d  are unsupported",
                FPS_MIN_LIMIT, FPS_MAX_LIMIT);
        return PRE_ACTION_NO_FPS_UPDATE;
    }
    QLOGI("ORIGINAL hintid: 0x%x, new fps: %d", pMsg->hint_id, pMsg->hint_type);
    TargetConfig &tc = TargetConfig::getTargetConfig();
    int fps = tc.getMinFpsTuning();
    if (fps > pMsg->hint_type)
        pMsg->hint_type = fps;
    pFpsUpdateObj.StandardizeFps(pMsg->hint_type);
    QLOGI("STANDARDIZED hintid: 0x%x, curr fps: %d, new fps: %d",
           pMsg->hint_id, pFpsUpdateObj.mCurFps,pMsg->hint_type);
    float hystTimeSec = pFpsUpdateObj.getFpsSwitchHystTimeSecs();
    if (hystTimeSec >= FPS_SWITCH_MIN_HYST_TIME_SECS) {
        pFpsUpdateObj.DeleteTimer();
        if (pMsg->hint_type < pFpsUpdateObj.GetFps()) {
            pFpsUpdateObj.mFpsValAfterTimerExpires = pMsg->hint_type;
            rc = pFpsUpdateObj.createFpsSwitchTimer();
            if(rc == 0) {
                QLOGI("Created Fps Switch Timer. Do not act on this lower FPS notfn from SF right away.");
                return PRE_ACTION_NO_FPS_UPDATE;
            }
        }
    }
    if (pMsg->hint_type == pFpsUpdateObj.mCurFps) {
        QLOGI("New fps = curr fps = %d, don't do anything", pFpsUpdateObj.mCurFps);
        return PRE_ACTION_NO_FPS_UPDATE;
    }
    fpsVal = fopen(CURRENT_FPS_FILE, "w");
    if (fpsVal == NULL) {
        QLOGE("Cannot open/create fps value file");
        return PRE_ACTION_NO_FPS_UPDATE;
    }
    QLOGI("New fps rcvd from SF, %d will be updated now.", pMsg->hint_type);
    {
        lock_guard<mutex> lock(pFpsUpdateObj.mMutex);
        if (pFpsUpdateObj.mHandle > 0) {
            PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
            tmpHandle = pFpsUpdateObj.mHandle;
            do {
                rc = ptp.placeTask([=]() {
                        QLOGI("Offloading CallHintReset from %s", __func__);
                        perf_lock_rel(tmpHandle);
                     });
                if (rc == FAILED && resized == false) {
                    int new_size = ptp.resize(1);
                    QLOGW("Failed to Offload CallHintReset from %s\n ThreadPool resized to : %d",__func__ , new_size);
                    resized = true;
                } else {
                    break;
                }
            } while(true);
            if (rc == SUCCESS) {
                pFpsUpdateObj.mHandle = -1;
                QLOGI("Offloaded new thread from %s", __func__);
            } else {
                QLOGE("FAILED to offload from %s", __func__);
                QLOGE("%d FPS release Failed for Handle: %d", pFpsUpdateObj.mCurFps, pFpsUpdateObj.mHandle);
            }
        }
        pFpsUpdateObj.mCurFps = pMsg->hint_type;
        locFps = pFpsUpdateObj.mCurFps;
    }
    snprintf(buf, NODE_MAX, "%d", locFps);

    fwrite(buf, sizeof(char), strlen(buf), fpsVal);
    fclose(fpsVal);
    return SUCCESS;
}

int FpsUpdateAction::FpsUpdatePostAction(mpctl_msg_t *pMsg) {
    if (NULL == pMsg) {
        return FAILED;
    }
    int size = pMsg->data;
    if (size > MAX_ARGS_PER_REQUEST)
        size = MAX_ARGS_PER_REQUEST;

    if(pMsg->hint_type >= FPS144) {
        for (int i = 0; i < size-1; i = i + 2) {
            if (pMsg->pl_args[i] == MPCTLV3_SCHED_ENABLE_TASK_BOOST_RENDERTHREAD) {
                QLOGI("FPS from FW:%d, apply task boost on top-app renderThreadTid:%d",
                       pMsg->hint_type, renderThreadTidOfTopApp);
                pMsg->pl_args[i+1] = renderThreadTidOfTopApp;
            }
        }
    }
    else if(pMsg->hint_type < FPS144) {
        for (int i = 0; i < size-1; i = i + 2) {
            if (pMsg->pl_args[i] == MPCTLV3_SCHED_DISABLE_TASK_BOOST_RENDERTHREAD) {
                QLOGI("FPS from FW:%d, disable task boost on top-app renderThreadTid:%d",
                       pMsg->hint_type, renderThreadTidOfTopApp);
                pMsg->pl_args[i+1] = renderThreadTidOfTopApp;
            }
        }
    }
    return SUCCESS;
}

void FpsUpdateAction::SetFps() {
    FILE *fpsVal;
    int fps = 0, handle = -1;
    fpsVal = fopen(CURRENT_FPS_FILE, "r");
    if (fpsVal != NULL) {
        fscanf(fpsVal,"%d", &fps);
        fclose(fpsVal);
    }

    handle = perf_hint(VENDOR_HINT_FPS_UPDATE, nullptr, 0, fps);
}

void FpsUpdateAction::saveFpsHintHandle(int handle, uint32_t hint_id) {
    if(hint_id == VENDOR_HINT_FPS_UPDATE) {
        {
            lock_guard<mutex> lock(mMutex);
            mHandle = handle;
        }
        QLOGI("Saved new fps update handle = %d", handle);
    }
}

//CPUFreqPostAction updates the freq val in the hint
//from perfboostconfig to a specified value
int CPUFreqAction::CPUFreqPostAction(mpctl_msg_t *pMsg) {
    if (NULL == pMsg) {
        return FAILED;
    }
    int size = pMsg->data;
    if ((size <= 0) || (size > MAX_ARGS_PER_REQUEST) || (size & 1))
        return FAILED;

    //interested in only vertical and horizontal scroll
    if ((pMsg->hint_type == 1) || (pMsg->hint_type == 2)) {
        for (int i = 0; i < size-1; i = i + 2) {
            if (pMsg->pl_args[i] == MPCTLV3_MIN_FREQ_CLUSTER_BIG_CORE_0 ||
                pMsg->pl_args[i] == MPCTLV3_MIN_FREQ_CLUSTER_PRIME_CORE_0) {
                //setting freq value to minimum in case feature is enabled
                pMsg->pl_args[i+1] = 0;
            }
        }
        return SUCCESS;
    }
    return FAILED;
}

int LargeComp::LargeCompPreAction(mpctl_msg_t *pMsg) {
    OptsData &d = OptsData::getInstance();
    FILE *pFile = NULL;
    char buf[NODE_MAX]= "";
    if (NULL == pMsg) {
        return FAILED;
    }
    d.hwcPid = pMsg->pl_time;
    pMsg->pl_time = 0;
    pMsg->offloadFlag = false;
    pFile = fopen(HWC_PID_FILE, "w");
    if (pFile == NULL) {
        QLOGE("Cannot open/create HWC pid file");
        return FAILED;
    }
    snprintf(buf, NODE_MAX, "%d", d.hwcPid);

    fwrite(buf, sizeof(char), strlen(buf), pFile);
    fclose(pFile);
    QLOGI("HWC pid %d", d.hwcPid);
    return SUCCESS;
}

//Storing pid for Sf and HWC from the hint
int StorePID::StorePIDPreAction(mpctl_msg_t *pMsg) {
    OptsData &d = OptsData::getInstance();
    FILE *pFile = NULL;
    char buf[NODE_MAX]= "";
    if (NULL == pMsg) {
        return FAILED;
    }
    if(pMsg->hint_type == 1) {
        d.sfPid = pMsg->pl_time;
        pFile = fopen(SF_PID_FILE, "w");
        if (pFile == NULL) {
            QLOGE("Cannot open/create SF pid file");
            return FAILED;
        }
        snprintf(buf, NODE_MAX, "%d", d.sfPid);

        fwrite(buf, sizeof(char), strlen(buf), pFile);
        fclose(pFile);
        QLOGI("SF pid %d", d.sfPid);
    }
    return PRE_ACTION_SF_HWC_VAL;
}
