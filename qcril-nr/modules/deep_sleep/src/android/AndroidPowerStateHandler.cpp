/*
 * ============================================================
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#include "AndroidPowerStateHandler.h"
#include "framework/Log.h"
#include "vendor/qti/hardware/powerstateservice/1.0/IPowerStateService.h"

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace modules {
namespace deep_sleep {

using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;
using namespace ::vendor::qti::hardware::powerstateservice;

static bool convertState(PowerStateHandler::EventListener::State& out, V1_0::state in) {
  bool result = true;
  switch (in) {
    case V1_0::state::DEEP_SLEEP_ENTER:
      out = PowerStateHandler::EventListener::State::ENTER;
      break;
    case V1_0::state::DEEP_SLEEP_EXIT:
      out = PowerStateHandler::EventListener::State::EXIT;
      break;
    default:
      result = false;
      break;
  }
  return result;
}

Return<bool> AndroidPowerStateHandler::PowerStateCallbackImpl::notifyDeepSleepEvent(
    V1_0::state state) {
  QCRIL_LOG_DEBUG("notifyDeepSleepEvent: state = %d", state);
  bool result = true;
  EventListener::State eventState;
  if (mOwner.mListener && convertState(eventState, state)) {
    result = mOwner.mListener->notifyDeepSleepEvent(eventState);
  }
  QCRIL_LOG_DEBUG("notifyDeepSleepEvent: result = %d", result);
  return result;
}

Return<bool> AndroidPowerStateHandler::PowerStateCallbackImpl::notifyHibernateEvent(
    V1_0::state /*state*/) {
  return true;
}

void AndroidPowerStateHandler::initPowerStateHal() {
  QCRIL_LOG_FUNC_ENTRY();
  if (!mService) {
    mService = V1_0::IPowerStateService::getService();
    if (mService) {
      QCRIL_LOG_DEBUG("V1_0::IPowerStateService::getService SUCCESS");
      ::android::sp<AndroidPowerStateHandler::PowerStateCallbackImpl> callback =
          new AndroidPowerStateHandler::PowerStateCallbackImpl(*this);
      auto ret = mService->registerDeepSleepClient(callback, "QCRIL");
      if (!ret) {
        QCRIL_LOG_ERROR("V1_0::IPowerStateService FAILED to registerDeepSleepClient");
      }
      if (mAcquired) {
        mAcquired = false;
        acquireLock();
      }
    } else {
      QCRIL_LOG_ERROR("V1_0::IPowerStateService::getService FAILED: received null");
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

Return<void> AndroidPowerStateHandler::ServiceNotificationListener::onRegistration(
    const hidl_string& fullyQualifiedName, const hidl_string& instanceName, bool preExisting) {
  QCRIL_LOG_DEBUG("PowerStateService started: %s %s %d", fullyQualifiedName.c_str(),
                  instanceName.c_str(), preExisting);
  mOwner.initPowerStateHal();
  return Void();
}

void AndroidPowerStateHandler::registerForPowerStateServiceNotification() {
  QCRIL_LOG_FUNC_ENTRY();
  ::android::sp<AndroidPowerStateHandler::ServiceNotificationListener> listener =
      new AndroidPowerStateHandler::ServiceNotificationListener(*this);
  auto ret = V1_0::IPowerStateService::registerForNotifications("default", listener);
  if (!ret) {
    QCRIL_LOG_DEBUG("Failed to V1_0::IPowerStateService::registerForNotifications");
  }
  QCRIL_LOG_FUNC_RETURN();
}

bool AndroidPowerStateHandler::acquireLock() {
  QCRIL_LOG_DEBUG("mAcquired = %d", mAcquired);
  if (!mAcquired) {
    if (mService) {
      QCRIL_LOG_DEBUG("calling IPowerStateService::acquireLPMWakelock");
      auto ret = mService->acquireLPMWakelock(LOCK_NAME);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to acquireLPMWakelock. Exception : %s", ret.description().c_str());
      }
    }
  }
  mAcquired = true;
  return true;
}

bool AndroidPowerStateHandler::releaseLock() {
  QCRIL_LOG_DEBUG("mAcquired = %d", mAcquired);
  if (mAcquired) {
    if (mService) {
      QCRIL_LOG_DEBUG("calling IPowerStateService::releaseLPMWakelock");
      auto ret = mService->releaseLPMWakelock(LOCK_NAME);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to releaseLPMWakelock. Exception : %s", ret.description().c_str());
      }
    }
  }
  mAcquired = false;
  return true;
}

bool AndroidPowerStateHandler::init() {
  registerForPowerStateServiceNotification();
  return true;
}

PowerStateHandler* getPlatformPowerStateHandler() {
  static AndroidPowerStateHandler the_AndroidPowerStateHandler;
  return &the_AndroidPowerStateHandler;
}

}  // namespace deep_sleep
}  // namespace modules
}  // namespace ril
