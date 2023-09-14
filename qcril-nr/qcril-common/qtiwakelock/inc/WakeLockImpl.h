/* ============================================================
 *
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#pragma once
#include <string>
#include "WakeLock.h"

class WakeLockImpl {
 public:
  WakeLockImpl();
  virtual ~WakeLockImpl();
  virtual bool acquire(std::string mLockName,
                       WakeLock::LockType type = WakeLock::LockType::PARTIAL_LOCK) = 0;
  virtual bool release(std::string mLockName) = 0;
};
