/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "QC_FACE_AUTH"

#include <android/hardware/biometrics/face/1.0/types.h>
#include <android/hardware/biometrics/face/1.0/IBiometricsFace.h>
#include <android/log.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include "FaceAuth.h"

using android::sp;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::biometrics::face::V1_0::implementation::FaceAuth;
using android::hardware::biometrics::face::V1_0::IBiometricsFace;

int main() {
    ALOGI("FaceAuth HAL is being started.");

    configureRpcThreadpool(1, true /*callerWillJoin*/);

    android::sp<IBiometricsFace> face = new FaceAuth();
    const android::status_t status = face->registerAsService();

    if (status != android::OK) {
        ALOGE("Error starting the FaceAuth HAL.");
        return 1;
    }

    ALOGI("FaceAuth HAL has started successfully.");
    joinRpcThreadpool();

    ALOGI("FaceAuth HAL is terminating.");
    return 1;  // should never get here
}
