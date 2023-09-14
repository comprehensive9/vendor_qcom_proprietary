/* ============================================================
 *
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */
#include "WakeLock.h"
#include <string>
#include "QtiMutex.h"
#include "WakeLockImpl.h"

// Forward declare for platform based impl provider
extern WakeLockImpl* getPlatformWakeLockImpl();

WakeLock::WakeLock(std::string lockName) {
  mLockName = lockName;
  mLockCount = 0;
  if (!mLockImpl) {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mWakeLockMutex);
    mLockImpl = getPlatformWakeLockImpl();
  }
}

WakeLock::~WakeLock() {
  releaseAll();
}

bool WakeLock::acquire(WakeLock::LockType type) {
  bool rc = false;
  if (!mLockImpl) return rc;

  std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mWakeLockMutex);
  rc = mLockImpl->acquire(getLockName(), type);
  if (rc) {
    mLockCount++;
  }

  return rc;
}

bool WakeLock::release() {
  bool rc = false;
  if (!mLockImpl) return rc;

  std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mWakeLockMutex);
  rc = mLockImpl->release(getLockName());
  if (rc) {
    mLockCount--;
  }

  return rc;
}

int WakeLock::getLockCount() {
  std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mWakeLockMutex);
  return mLockCount;
}

void WakeLock::releaseAll() {
  if (!mLockImpl) return;

  std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mWakeLockMutex);

  while (mLockCount--) {
    release();
  }
}

std::string WakeLock::getLockName() {
  return mLockName;
}

qtimutex::QtiRecursiveMutex WakeLock::mWakeLockMutex;
WakeLockImpl* WakeLock::mLockImpl = nullptr;
