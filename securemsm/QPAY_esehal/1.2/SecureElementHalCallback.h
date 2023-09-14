/**
 * Copyright (c) 2018, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_SECUREELEMENTHALCALLBACK_H
#define ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_SECUREELEMENTHALCALLBACK_H

#include <android/hardware/secure_element/1.1/ISecureElementHalCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace android {
namespace hardware {
namespace secure_element {
namespace V1_1 {
namespace implementation {

using ::android::hardware::secure_element::V1_1::ISecureElementHalCallback;
using ::android::hidl::base::V1_0::IBase;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct SecureElementHalCallback : public ISecureElementHalCallback {
    // Methods from ::android::hardware::secure_element::V1_0::ISecureElementHalCallback follow.
    Return<void> onStateChange(bool cardPresent) override;

   // Methods from ::android::hardware::secure_element::V1_1::ISecureElementHalCallback follow.
   Return<void> onStateChange_1_1(bool cardPresent,const hidl_string& reason) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

extern "C" ISecureElementHalCallback* HIDL_FETCH_ISecureElementHalCallback(const char* name);

}  // namespace implementation
}  // namespace V1_1
}  // namespace secure_element
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_SECUREELEMENTHALCALLBACK_H
