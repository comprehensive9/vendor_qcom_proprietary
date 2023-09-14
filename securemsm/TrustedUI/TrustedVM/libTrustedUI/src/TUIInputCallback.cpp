/*========================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/
#include "TUIInputCallback.h"
#include "ITrustedInputCallBack.hpp"
#include "TUIDefs.h"
#include "TUIUtils.h"
#include "impl_base.hpp"
#include "memscpy.h"
#include "secure_memset.h"
#include "object.h"
#include "proxy_base.hpp"
#include <mutex>
#include <semaphore.h>
#include <stdint.h>
#include "TUILog.h"

#define LOG_TAG "VM_TUIInputCallback"

int32_t InputCBHelper::getInput_b(tsFingerData *fingers) {
  int32_t ret = NOTIFY_ERROR;
  int32_t touchResp = 0;

  TUI_CHECK(mSessionObj != nullptr);
  TUI_CHECK(fingers != nullptr);
  secure_memset(fingers, 0x0, sizeof(tsFingerData));

  {
    TUILOGV("Invoking SessionObj::getInputData");
    std::unique_lock<std::mutex> l(mInputLock);

    TUILOGV("Invoking SessionObj::getInputData locked");
    mNotifyStatus = NOTIFY_ERROR;
    touchResp = mSessionObj->getInputData(mInputTimeoutMs);
    TUILOGV("getInputData resp: %d", touchResp);
    TUI_CHECK(touchResp == 0);

    TUILOGV("Waiting on input data avialble condition");
    mInputAvailCondition.wait(l);

    if (mNotifyStatus == NOTIFY_OK) {
      TUILOGV("Input data available signaled : OK");
      *fingers = mFingers;
    } else {
      TUILOGV("Input data available signaled : %s",
            mNotifyStatus == NOTIFY_TIMEOUT
                ? "TIMEOUT"
                : mNotifyStatus == NOTIFY_ABORT ? "ABORT" : "ERROR");
    }
    ret = mNotifyStatus;
  }

end:
  return ret;
}

int32_t InputCBHelper::notifyInput(const void *pInput, size_t inputSize) {
  TUILOGV("%s++: Input Notified", __func__);
  std::unique_lock<std::mutex> l(mInputLock);
  TUILOGV("%s++: Input Notified locked", __func__);
  mNotifyStatus = NOTIFY_ERROR;

  TUI_CHECK(pInput != nullptr);
  TUI_CHECK(inputSize == sizeof(tsFingerData));

  memscpy((void *)&mFingers, sizeof(tsFingerData), pInput, inputSize);
  TUILOGD("%s: Touch Co-ordinates x:%d y:%d", __func__, mFingers.finger[0].x,
        mFingers.finger[0].y);

  mNotifyStatus = NOTIFY_OK;

end:
  TUILOGV("Input Notified sizeof(fingerdata) is : %d", sizeof(tsFingerData));
  mInputAvailCondition.notify_all();
  TUILOGV("%s--", __func__);
  return mNotifyStatus;
}

int32_t InputCBHelper::notifyTimeout() {
  TUILOGV("%s++: Input Timeout notified", __func__);
  std::unique_lock<std::mutex> l(mInputLock);
  TUILOGV("%s++: Input Timeout locked", __func__);
  mNotifyStatus = NOTIFY_TIMEOUT;
  mInputAvailCondition.notify_all();
  TUILOGV("%s--", __func__);
  return 0;
}

int32_t InputCBHelper::notifyError(int32_t err) {
  TUILOGV("%s++: Input Error notified", __func__);
  std::unique_lock<std::mutex> l(mInputLock);
  TUILOGV("%s++: Input Error notified locked", __func__);
  mNotifyStatus = NOTIFY_ERROR;
  mInputAvailCondition.notify_all();
  TUILOGV("%s--", __func__);
  return 0;
}

void InputCBHelper::abort() {
  TUILOGV("%s++: Input wait aborted", __func__);
  std::unique_lock<std::mutex> l(mInputLock);
  TUILOGV("%s++: Input wait aborted locked", __func__);
  mNotifyStatus = NOTIFY_ABORT;
  mInputAvailCondition.notify_all();
  TUILOGV("%s--", __func__);
}
