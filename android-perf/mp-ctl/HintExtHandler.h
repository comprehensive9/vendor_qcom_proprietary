/******************************************************************************
  @file    HintExtHandler.h
  @brief   Declaration of hint extensions

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __HINT_EXT_HANDLER__H_
#define __HINT_EXT_HANDLER__H_

#include <mutex>
#include "PerfController.h"

#define MAX_HANDLERS 12
#define FPS_SWITCH_MIN_HYST_TIME_SECS 1

typedef int (*HintExtAction)(mpctl_msg_t *);

typedef struct HintActionInfo {
    uint32_t mHintId;
    HintExtAction mPreAction;
    HintExtAction mPostAction;
} HintActionInfo;

class HintExtHandler {
    private:
        std::mutex mMutex;
        int mNumHandlers;
        HintActionInfo mExtHandlers[MAX_HANDLERS];

    private:
        HintExtHandler();
        HintExtHandler(HintExtHandler const&);
        HintExtHandler& operator=(HintExtHandler const&);

    public:
        ~HintExtHandler();
        static HintExtHandler &getInstance() {
            static HintExtHandler mHintExtHandler;
            return mHintExtHandler;
        }

        void Reset();
        bool Register(uint32_t hintId, HintExtAction preAction, HintExtAction postAction);
        int FetchConfigPreAction(mpctl_msg_t *pMsg);
        int FetchConfigPostAction(mpctl_msg_t *pMsg);
};

class DisplayEarlyWakeupAction {
    public:
        static int DisplayEarlyWakeupPreAction(mpctl_msg_t *pMsg);
};

//pre/post actions of modules
class TaskBoostAction  {
    public:
        static int TaskBoostPostAction(mpctl_msg_t *pMsg);
};

#define CURRENT_FPS_FILE "/data/vendor/perfd/current_fps"
class FpsUpdateAction {
    private:
        int mCurFps;
        int mHandle;
        int mFpsHandleToRelease;
        std::mutex mMutex;
        float mFpsSwitchHystTime;
        timer_t mTimer;
        bool mFpsTimerCreated;
        int mFpsValAfterTimerExpires;
    private:
        FpsUpdateAction();
        FpsUpdateAction(FpsUpdateAction const&);
        FpsUpdateAction& operator=(FpsUpdateAction const&);
        ~FpsUpdateAction();

        void StandardizeFps(int32_t &fps);

    public:
        static FpsUpdateAction& getInstance() {
            static FpsUpdateAction mInstance;
            return mInstance;
        }
        static int FpsUpdatePreAction(mpctl_msg_t *pMsg);
        static int FpsUpdatePostAction(mpctl_msg_t *pMsg);
        float getFpsSwitchHystTimeSecs();
        void SetFps();
        int GetFps() {
            return mCurFps;
        }
        int createFpsSwitchTimer();
        static void switchToNewFps(sigval_t pvData);
        int CreateTimer(int req_handle);
        int SetTimer();
        int DeleteTimer();
        void saveFpsHintHandle(int handle, uint32_t hint_id);
        enum fps {
            FPS_MIN_LIMIT = 10,
            FPS30 = 30,
            FPS30_MAX_RANGE = 37,
            FPS45 = 45,
            FPS45_MAX_RANGE = 52,
            FPS60 = 60,
            FPS60_MAX_RANGE = 75,
            FPS90 = 90,
            FPS90_MAX_RANGE = 105,
            FPS120 = 120,
            FPS120_MAX_RANGE = 132,
            FPS144 = 144,
            FPS144_MAX_RANGE = 162,
            FPS180 = 180,
            FPS180_MAX_RANGE = 210,
            FPS240 = 240,
            FPS_MAX_LIMIT = 260
        };
};

class CPUFreqAction {
    public:
        static int CPUFreqPostAction(mpctl_msg_t *pMsg);
};

#define HWC_PID_FILE "/data/vendor/perfd/hwc_pid"
#define SF_PID_FILE "/data/vendor/perfd/sf_pid"
class LargeComp {
    public:
        static int LargeCompPreAction(mpctl_msg_t *pMsg);
};
class StorePID {
    public:
        static int StorePIDPreAction(mpctl_msg_t *pMsg);
};
#endif /*__HINT_EXT_HANDLER__H_*/
