/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <vendor/qti/esepowermanager/1.1/IEsePowerManager.h>
#include <EsePowerManager.h>
#include <string>
#include <iostream>

using namespace android;

using ::vendor::qti::esepowermanager::V1_0::IEsePowerManager;
using IEsePowerManagerV1_1 = ::vendor::qti::esepowermanager::V1_1::IEsePowerManager;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hidl::base::V1_0::IBase;

static sp<IEsePowerManager> mHal;
static sp<IEsePowerManagerV1_1> mHalV1_1;

int ret = -1;

static void esepmInit()
{
    if (mHal != nullptr) {
        return;
    }

    ALOGD("Try IEsePowerManagerV1_1::getService()");
    mHal = mHalV1_1 = IEsePowerManagerV1_1::getService();
    if(mHalV1_1 == nullptr) {
      ALOGE("Failure in getting IEsePowerManager V1.1 getService. Try IEsePowerManager::getService()");
      mHal = IEsePowerManager::getService();
      if(mHal == nullptr) {
        ALOGE("Failure in getting IEsePowerManager V1.0 getService.");
      }
    }
}

struct notifyDeath : IBase {
  void notify() { printf("notified\n"); }
};

static int nativeeSEPowerOn () {
  sp<notifyDeath> callback = new notifyDeath();
  ALOGD("nativeeSEPowerON");
  if (mHal == nullptr) {
      esepmInit();
      if (mHal == nullptr) {
        ALOGE("NULL Hal");
        return ret;
      }
  }
  return (mHal->powerOn(callback));
}

static int nativeeSEPowerOff() {

  ALOGD("nativeeSEPowerOff");
  if (mHal == nullptr) {
      esepmInit();
      if (mHal == nullptr) {
        ALOGE("NULL Hal");
        return ret;
      }
  }
  return (mHal->powerOff() == 0);
}

static int nativeeSEGetState() {
  ALOGD("nativeeSEGetState");
  if (mHal == nullptr) {
      esepmInit();
      if (mHal == nullptr) {
        ALOGE("NULL Hal");
        return ret;
      }
  }
  return (mHal->getState());
}

static int nativeKillAll() {
  ALOGD("nativeeSEPowerOff");
  if (mHal == nullptr) {
      esepmInit();
      if (mHal == nullptr) {
        ALOGE("NULL Hal");
        return ret;
      }
  }
  return (mHal->killall());
}

int main(int argc, char **argv)
{
    ALOGD("eSEPowerClient called with %d arguments.", argc);
    if (argc == 1) {
        ALOGD("service->powerOn = %d", nativeeSEPowerOn());
        ALOGD("service->powerOff = %d", nativeeSEPowerOff());
        return 0;
    }

    if (argv[1] == std::string("poweron")) {
        ALOGD("service->powerOn = %d", nativeeSEPowerOn());
        std::cin.ignore();
    } else if (argv[1] == std::string("poweroff")) {
        ALOGD("service->powerOff = %d", nativeeSEPowerOff());
    } else if (argv[1] == std::string("killall")) {
        ALOGD("service->killall = %d", nativeKillAll());
    } else if (argv[1] == std::string("getstate")) {
        ALOGD("service->getState = %d", nativeeSEGetState() );
    } else {
        ALOGD("service->powerOn = %d", nativeeSEPowerOn());
        ALOGD("service->powerOff = %d", nativeeSEPowerOff());
    }

    return 0;
}
