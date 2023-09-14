/*
 * ============================================================
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#ifndef __RIL_DEEPSLEEPWAKELOCKIMPL_H_
#define __RIL_DEEPSLEEPWAKELOCKIMPL_H_

#include <string>

namespace ril {
namespace modules {
namespace deep_sleep {

class PowerStateHandler {
 public:
  class EventListener {
   public:
    enum class State {
      ENTER,
      EXIT,
    };
    EventListener();
    virtual ~EventListener();
    virtual bool notifyDeepSleepEvent(State state) = 0;
  };

  static constexpr const char* LOCK_NAME = "QCRIL";
  PowerStateHandler();
  virtual ~PowerStateHandler();
  virtual bool init() = 0;
  virtual void registerForEvent(EventListener* listener) = 0;
  virtual bool acquireLock() = 0;
  virtual bool releaseLock() = 0;
};

extern PowerStateHandler* getPlatformPowerStateHandler();

}  // namespace deep_sleep
}  // namespace modules
}  // namespace ril

#endif  // __RIL_DEEPSLEEPWAKELOCKIMPL_H_
