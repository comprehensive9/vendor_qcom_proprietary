/******************************************************************************
  @file    service.cpp
  @brief   Android IOP HAL service

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "vendor.qti.hardware.iop@2.0-service"

#include <vendor/qti/hardware/iop/2.0/IIop.h>
#include <hidl/LegacySupport.h>
#include "Iop.h"

#include <utils/Log.h>

#include "MpctlUtils.h"
#include <client.h>

using vendor::qti::hardware::iop::V2_0::implementation::Iop;
using vendor::qti::hardware::iop::V2_0::IIop;
using android::hardware::defaultPassthroughServiceImplementation;

using android::sp;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::status_t;
using android::OK;

int main() {
    ALOGE("IIop HAL service main");

    Iop *iopObj = new(std::nothrow) Iop();

    if (iopObj != NULL) {
        android::sp<IIop> service = iopObj;

        configureRpcThreadpool(1, true /*callerWillJoin*/);

        if (service->registerAsService() != OK) {
            ALOGE("Cannot register IIop HAL service");
            return 1;
        }

        ALOGE("Registered IIop HAL service success!");
        joinRpcThreadpool();
    }
    return 0;
}
