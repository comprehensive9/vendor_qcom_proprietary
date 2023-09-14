/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "SECURE-PROCESSOR"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <vendor/qti/hardware/secureprocessor/config/1.0/types.h>
#include <vendor/qti/hardware/secureprocessor/device/1.0/ISecureProcessor.h>
#include "SecureProcessorFactory.h"
#include "SecureProcessorTEE.h"

using vendor::qti::hardware::secureprocessor::device::V1_0::ISecureProcessor;
using vendor::qti::hardware::secureprocessor::device::V1_0::implementation::
    SecureProcessorFactory;
using vendor::qti::hardware::secureprocessor::device::V1_0::implementation::
    TYPE_QTI_TEE;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;

int main()
{
    int status;

    ALOGI("Starting qti-tee service instance");

    /*
     * Attempt to create TEE Device to communicate with Trusted App
     * use a factory method to create multiple implementation.
     */
    android::sp<ISecureProcessor> destinationTEE =
        SecureProcessorFactory::CreateSecureProcessor(
            std::string(TYPE_QTI_TEE));
    configureRpcThreadpool(1, true /* willJoinThreadpool */);

    if (destinationTEE) {
        status = destinationTEE->registerAsService(TYPE_QTI_TEE);
        if (status != 0) {
            ALOGE("Failed to register qti-tee service instance");
        }
    } else {
        ALOGE("Failed to create qti-tee service instance");
    }

    ALOGI("Started qti-tee service instance");
    joinRpcThreadpool();
    return -1;  // Should never get here.
}
