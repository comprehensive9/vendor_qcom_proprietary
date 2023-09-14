/**
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <vendor/qti/esepowermanager/1.0/IEsePowerManager.h>
#include <EsePowerManager.h>

using namespace android;

using ::vendor::qti::esepowermanager::V1_0::IEsePowerManager;
using ::android::hidl::base::V1_0::IBase;

static sp<IEsePowerManager> mHal;

static bool esepmInit() {
  if (mHal != nullptr) {
    return true;
  }

  mHal = IEsePowerManager::getService();
  return (mHal != nullptr);
}

struct notifyDeath : IBase {
  void notify() {
    ALOGD("Death notified");
  }
};

int nativeeSEPowerOn () {
  int ret = -1;
  ALOGD("nativeeSEPowerON");
  sp<notifyDeath> callback = new notifyDeath();

  if (callback == nullptr) {
    ALOGE("NULL callback");
    return ret;
  }

  if (esepmInit()) {
    return (mHal->powerOn(callback));
  } else {
    ALOGE("mHal is null");
    return ret;
  }
}

int nativeeSEPowerOff() {
  int ret = -1;
  ALOGD("nativeeSEPowerOff");

  if (esepmInit()) {
    return (mHal->powerOff() == 0);
  } else {
    ALOGE("mHal is null");
    return ret;
  }
}

int nativeeSEGetState() {
  int ret = -1;

  if (esepmInit()) {
    return (mHal->getState());
  } else {
    ALOGE("mHal is null");
    return ret;
  }
}
