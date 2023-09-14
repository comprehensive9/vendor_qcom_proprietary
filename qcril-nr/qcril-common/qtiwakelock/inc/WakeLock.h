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
#include <unordered_map>
#include "QtiMutex.h"

// Forward declare for PIMPL
class WakeLockImpl;

class WakeLock {
  std::string mLockName;
  int mLockCount;
  static qtimutex::QtiRecursiveMutex mWakeLockMutex;
  static WakeLockImpl* mLockImpl;

 public:
  enum class LockType { PARTIAL_LOCK, FULL_LOCK };
  WakeLock() = delete;                            // Enforce wakelocks with name
  WakeLock(WakeLock const&) = delete;             // Copy construct
  WakeLock& operator=(WakeLock const&) = delete;  // Copy assign

  WakeLock(std::string lockName);
  ~WakeLock();
  bool acquire(LockType = LockType::PARTIAL_LOCK);
  bool release();
  int getLockCount();
  std::string getLockName();
  void releaseAll();
};
