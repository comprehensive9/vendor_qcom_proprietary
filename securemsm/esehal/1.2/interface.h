/**
 * Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_INTERFACE_H
#define ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_INTERFACE_H

#include <android/hardware/secure_element/1.1/ISecureElementHalCallback.h>
#include "gpqese/gpqese-apdu.h"

using ::android::hardware::secure_element::V1_0::ISecureElementHalCallback;
using ISecureElementHalCallbackV1_1 = ::android::hardware::secure_element::V1_1::ISecureElementHalCallback;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

Return<void> setSeCallBack(const android::sp<ISecureElementHalCallback>& clientCallback);
Return<void> setSeCallBackV1_1(const android::sp<ISecureElementHalCallbackV1_1>& clientCallback);
Return<void> initSEService();

#endif /* ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_INTERFACE_H */
