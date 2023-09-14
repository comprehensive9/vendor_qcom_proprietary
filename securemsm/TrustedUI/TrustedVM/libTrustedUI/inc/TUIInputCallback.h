/*========================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/
#ifndef __TUIINPUT_CALLBACK_H__
#define __TUIINPUT_CALLBACK_H__

#include <stdint.h>
#include <semaphore.h>
#include <mutex>
#include <condition_variable>

#include "object.h"
#include "impl_base.hpp"
#include "proxy_base.hpp"
#include "ITrustedInputCallBack.hpp"
#include "ITUISystemSession.hpp"
#include "ITrustedInputCallBack_invoke.hpp"
#include "TUILog.h"
#include "TUIDefs.h"
#include "TouchApiDefs.h"

/**
 * @brief This class provides a simplified access to touch-data and implements the
 *        TUIInputCallback
 */
class InputCBHelper : public TrustedInputCallBackImplBase {
 public:
    enum NOTIFY_STATUS : int32_t {
        NOTIFY_TIMEOUT = 1,
        NOTIFY_OK = 0,
        NOTIFY_ERROR = -1,
        NOTIFY_ABORT = -2,
    };

    InputCBHelper(int32_t timeoutMs)
        : mInputTimeoutMs(timeoutMs) {
    }

    virtual ~InputCBHelper() {
        TUILOGV("~InputCBHelper() Destructor");
    };

    /*
     * -------------------------------------------------------------------------------
     * Implementations of ITrustedInputCallBack
     * -------------------------------------------------------------------------------
     */
    int32_t notifyInput(const void* inputData_ptr, size_t inputData_len) override;

    int32_t notifyTimeout() override;

    int32_t notifyError(int32_t error) override;

    /*
     * ----------------------------------------------------------------------------------------
     * App helpers
     * ----------------------------------------------------------------------------------------
     */
    void setSessionObj(std::shared_ptr<ITUISystemSession> sessionObj) {
        mSessionObj = sessionObj;
    }
    void releaseSessionObj() {
        mSessionObj = nullptr;
    }

    /*
     * @brief Get touch-data. This method blocks until data is available or abort() is called.
     * @return NOTIFY_OK if input was read successfully
     * @return NOTIFY_TIMEOUT/ERROR if timeout/error
     * @return NOTIFY_ABORT if unblocked via abort()
     */
    int32_t getInput_b(tsFingerData *fingers);

    /*
     * @brief Abort the blocked getInput_b() call esp. when terminating the session
     */
    void abort();

 private:
    std::shared_ptr<ITUISystemSession> mSessionObj = nullptr;
    int32_t mInputTimeoutMs = -1;

    std::mutex mInputLock;
    std::condition_variable mInputAvailCondition;

    struct tsFingerData mFingers = {0};

    NOTIFY_STATUS mNotifyStatus = NOTIFY_ERROR;
};
#endif

