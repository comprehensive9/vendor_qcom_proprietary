/* ============================================================
 *
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */
#ifndef QMI_RIL_UTF
#include "LeWakeLockImpl.h"
#include <string>
#include "WakeLock.h"
#include "WakeLockImpl.h"

LeWakeLockImpl::LeWakeLockImpl() {
  mLockFd.open(WAKE_LOCK_FILE);
  mUnlockFd.open(WAKE_UNLOCK_FILE);
}

LeWakeLockImpl::~LeWakeLockImpl() {
  mLockFd.close();
  mUnlockFd.close();
}

bool LeWakeLockImpl::acquire(std::string mLockName, WakeLock::LockType type) {
  (void)type;

  if (!mLockFd.is_open()) return false;

  mLockFd << mLockName << std::endl;
  return true;
}

bool LeWakeLockImpl::release(std::string mLockName) {
  if (!mUnlockFd.is_open()) return false;

  mUnlockFd << mLockName << std::endl;
  return true;
}

WakeLockImpl* getPlatformWakeLockImpl() {
  static LeWakeLockImpl the_LeWakeLockImpl;
  return &the_LeWakeLockImpl;
}

#endif
