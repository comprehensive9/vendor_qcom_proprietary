/*
 * ============================================================
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#include "LePowerStateHandler.h"

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace modules {
namespace deep_sleep {

bool LePowerStateHandler::acquireLock() {
  return true;
}

bool LePowerStateHandler::releaseLock() {
  return true;
}

bool LePowerStateHandler::init() {
  return true;
}

void registerForEvent(EventListener* /*listener*/) {
}

PowerStateHandler* getPlatformPowerStateHandler() {
  static LePowerStateHandler the_LePowerStateHandler;
  return &the_LePowerStateHandler;
}

}  // namespace deep_sleep
}  // namespace modules
}  // namespace ril
