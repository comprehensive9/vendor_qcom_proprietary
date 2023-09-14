/* ============================================================
 *
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#include <fstream>
#include <string>
#include "WakeLock.h"
#include "WakeLockImpl.h"

class LeWakeLockImpl : public WakeLockImpl {
 private:
  std::ofstream mLockFd;
  std::ofstream mUnlockFd;
  const std::string WAKE_LOCK_FILE = "/sys/power/wake_lock";
  const std::string WAKE_UNLOCK_FILE = "/sys/power/wake_unlock";

 public:
  LeWakeLockImpl();
  ~LeWakeLockImpl();
  bool acquire(std::string mLockName,
               WakeLock::LockType type = WakeLock::LockType::PARTIAL_LOCK) override;
  bool release(std::string mLockName) override;
};
