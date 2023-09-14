/* ============================================================
 *
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */
#include "AndroidWakeLockImpl.h"
#include <string>
#include "WakeLock.h"
#include "WakeLockImpl.h"
#ifndef QMI_RIL_UTF
#include <hardware_legacy/power.h>

AndroidWakeLockImpl::~AndroidWakeLockImpl() {
}

bool AndroidWakeLockImpl::acquire(std::string mLockName, WakeLock::LockType type) {
  int lockType;
  bool rc = false;

  switch (type) {
    case WakeLock::LockType::PARTIAL_LOCK:
      lockType = PARTIAL_WAKE_LOCK;
      break;

    case WakeLock::LockType::FULL_LOCK:
      lockType = FULL_WAKE_LOCK;
      break;

    default:
      lockType = PARTIAL_WAKE_LOCK;
      break;
  }

  if (acquire_wake_lock(lockType, mLockName.c_str())) {
    rc = true;
  }

  return rc;
}

bool AndroidWakeLockImpl::release(std::string mLockName) {
  bool rc = false;

  if (release_wake_lock(mLockName.c_str())) {
    rc = true;
  }

  return rc;
}

WakeLockImpl* getPlatformWakeLockImpl() {
  static AndroidWakeLockImpl the_AndroidWakeLockImpl;
  return &the_AndroidWakeLockImpl;
}
#else

WakeLockImpl* getPlatformWakeLockImpl() {
  return nullptr;
}
#endif
