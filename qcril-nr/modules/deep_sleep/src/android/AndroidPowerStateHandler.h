/*
 * ============================================================
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#ifndef __RIL_ANDROIDPOWERSTATEHANDLER_H_
#define __RIL_ANDROIDPOWERSTATEHANDLER_H_

#include "android/hidl/manager/1.0/IServiceNotification.h"
#include "vendor/qti/hardware/powerstateservice/1.0/IPowerStateService.h"
#include "vendor/qti/hardware/powerstateservice/1.0/IPowerStateCallback.h"
#include "PowerStateHandler.h"

namespace ril {
namespace modules {
namespace deep_sleep {

class AndroidPowerStateHandler : public PowerStateHandler {
 private:
  class PowerStateCallbackImpl
      : public ::vendor::qti::hardware::powerstateservice::V1_0::IPowerStateCallback {
   public:
    PowerStateCallbackImpl(AndroidPowerStateHandler& owner) : mOwner(owner) {
    }
    ::android::hardware::Return<bool> notifyDeepSleepEvent(
        ::vendor::qti::hardware::powerstateservice::V1_0::state state);
    ::android::hardware::Return<bool> notifyHibernateEvent(
        ::vendor::qti::hardware::powerstateservice::V1_0::state state);

   private:
    AndroidPowerStateHandler& mOwner;
  };

  class ServiceNotificationListener : public ::android::hidl::manager::V1_0::IServiceNotification {
   public:
    ServiceNotificationListener(AndroidPowerStateHandler& m) : mOwner(m) {
    }
    ::android::hardware::Return<void> onRegistration(
        const ::android::hardware::hidl_string& fullyQualifiedName,
        const ::android::hardware::hidl_string& instanceName, bool preExisting) override;

   private:
    AndroidPowerStateHandler& mOwner;
  };

  ::android::sp<::vendor::qti::hardware::powerstateservice::V1_0::IPowerStateService> mService;

  void registerForPowerStateServiceNotification();
  void initPowerStateHal();
  bool mAcquired = false;
  EventListener* mListener = nullptr;

 public:
  bool init() override;
  void registerForEvent(EventListener* listener) override {
    mListener = listener;
  }
  bool acquireLock() override;
  bool releaseLock() override;
};

}  // namespace deep_sleep
}  // namespace modules
}  // namespace ril

#endif  // __RIL_ANDROIDPOWERSTATEHANDLER_H_
