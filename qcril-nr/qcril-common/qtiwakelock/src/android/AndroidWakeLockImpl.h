/* ============================================================
 *
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#include <string>
#include "WakeLock.h"
#include "WakeLockImpl.h"

class AndroidWakeLockImpl : public WakeLockImpl {
 public:
  bool acquire(std::string mLockName,
               WakeLock::LockType type = WakeLock::LockType::PARTIAL_LOCK) override;
  bool release(std::string mLockName) override;
  ~AndroidWakeLockImpl();
};
