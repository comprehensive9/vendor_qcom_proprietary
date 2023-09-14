/*
 * Copyright (c) 2019 - 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "trustedui@1.0-1-service-qti"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include "TrustedUIFactory.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;

using vendor::qti::hardware::trustedui::V1_2::implementation::TrustedUIFactory;
using vendor::qti::hardware::trustedui::V1_2::implementation::TrustedInputFactory;

int main() {
#ifdef ARCH_ARM_32
    android::hardware::ProcessState::initWithMmapSize((size_t)(2*PAGE_SIZE));
#endif
    configureRpcThreadpool(1, true /*willJoinThreadpool*/);

    /* =====================For TZ Implementation ===========================*/
    ALOGD("TrustedUI service starting.");
    do{
        auto trustedui = TrustedUIFactory::GetInstance("qtee-tz");
        if (trustedui == nullptr) {
            ALOGE("TrustedUIFactory::GetInstance: qtee-tz null.");
            break;
        }
        auto status = trustedui->registerAsService();
        if (status != android::OK) {
            ALOGE("Could not register service for TrustedUI 1.0-1, status = 0x%x",
                  status);
        }
        ALOGD("Registered TrustedUI HAL service success");
        /* Start ITrustedInput Service*/
        auto trustedInputDevice = TrustedInputFactory::GetInstance("qtee-tz");
        if (trustedInputDevice == nullptr) {
            ALOGE("TrustedInputFactory::GetInstance: qtee-tz null.");
            break;
        }

        ALOGD("TrustedInput service starting.");
        status = trustedInputDevice->registerAsService();
        if (status != android::OK) {
            ALOGE("Could not register service for TrustedInput 1.0-1, status = 0x%x",
                    status);
        }
        ALOGD("Registered TrustedInput service success");
    }while(0);

    /* =====================For VM Implementation ===========================*/
    ALOGD("TrustedUI-VM service starting.");
    do{
        auto trusteduiVM = TrustedUIFactory::GetInstance("qtee-vm");
        if (trusteduiVM == nullptr) {
            ALOGE("TrustedUIFactory::GetInstance: qtee-vm null.");
            break;
        }
        auto status = trusteduiVM->registerAsService("qtee-vm");
        if (status != android::OK) {
            ALOGE("Could not register service for TrustedUI 1.0, status = 0x%x",
                  status);
        }
        ALOGD("Registered TrustedUI VM service success");

        /* Start ITrustedInput Service*/
        auto trustedInputDevice = TrustedInputFactory::GetInstance("qtee-vm");
        if (trustedInputDevice == nullptr) {
            ALOGE("TrustedInputFactory::GetInstance: qtee-vm null.");
            break;
        }

        ALOGD("TrustedInput-VM service starting.");
        status = trustedInputDevice->registerAsService("qtee-vm");
        if (status != android::OK) {
            ALOGE("Could not register service for TrustedInput 1.0-1, status = 0x%x",
                    status);
        }
        ALOGD("Registered TrustedInput service success");
    }while(0);

    joinRpcThreadpool();
    return -1;
}
