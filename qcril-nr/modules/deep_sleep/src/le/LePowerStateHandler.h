/*
 * ============================================================
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#ifndef __RIL_LEPOWERSTATEHANDLER_H_
#define __RIL_LEPOWERSTATEHANDLER_H_

#include "PowerStateHandler.h"

namespace ril {
namespace modules {
namespace deep_sleep {

class LePowerStateHandler : public PowerStateHandler {
 public:
  bool init() override;
  void registerForEvent(EventListener* listener) override;
  bool acquireLock() override;
  bool releaseLock() override;
};

}  // namespace deep_sleep
}  // namespace modules
}  // namespace ril

#endif  // __RIL_LEPOWERSTATEHANDLER_H_
