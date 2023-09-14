/**
 * Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_SECUREELEMENT_H
#define ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_SECUREELEMENT_H

#include <android/hardware/secure_element/1.2/ISecureElement.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "interface.h"

namespace android {
namespace hardware {
namespace secure_element {
namespace V1_2 {
namespace implementation {

using ::android::hardware::secure_element::V1_2::ISecureElement;
using ::android::hardware::secure_element::V1_0::ISecureElementHalCallback;
using ISecureElementHalCallbackV1_1 = ::android::hardware::secure_element::V1_1::ISecureElementHalCallback;
using ::android::hardware::secure_element::V1_0::LogicalChannelResponse;
using ::android::hardware::secure_element::V1_0::SecureElementStatus;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct SecureElement : public ISecureElement {
  // Methods from ::android::hardware::secure_element::V1_0::ISecureElement follow.
  Return<void> init(const sp<ISecureElementHalCallback>
                                   &clientCallback) override;
  Return<void> getAtr(getAtr_cb _hidl_cb) override;
  Return<bool> isCardPresent() override;
  Return<void> transmit(const hidl_vec<uint8_t> &data,
                        transmit_cb _hidl_cb) override;
  Return<void> openLogicalChannel(const hidl_vec<uint8_t> &aid, uint8_t p2,
                                  openLogicalChannel_cb _hidl_cb) override;
  Return<void> openBasicChannel(const hidl_vec<uint8_t> &aid, uint8_t p2,
                                openBasicChannel_cb _hidl_cb) override;
  Return<SecureElementStatus> closeChannel(uint8_t channelNumber) override;

  // Methods from ::android::hardware::secure_element::V1_1::ISecureElement follow.
  Return<void> init_1_1(const sp<ISecureElementHalCallbackV1_1>
                                   &clientCallback) override;

  // Methods from ::android::hardware::secure_element::V1_2::ISecureElement follow.
  Return<SecureElementStatus> reset() override;
};

}  // namespace implementation
}  // namespace V1_2
}  // namespace secure_element
}  // namespace hardware
}  // namespace android
#endif  // ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_SECUREELEMENT_H
