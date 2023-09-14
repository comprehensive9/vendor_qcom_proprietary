/**
 * Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_HARDWARE_SECURE_ELEMENT_V1_0_INTERFACE_H
#define ANDROID_HARDWARE_SECURE_ELEMENT_V1_0_INTERFACE_H

#include <android/hardware/secure_element/1.0/ISecureElementHalCallback.h>
#include "gpqese/gpqese-apdu.h"

using ::android::hardware::secure_element::V1_0::ISecureElementHalCallback;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

Return<void> setSeCallBack(const android::sp<ISecureElementHalCallback>& clientCallback);
Return<void> initSEService();

#endif /* ANDROID_HARDWARE_SECURE_ELEMENT_V1_0_INTERFACE_H */
