/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <android/hardware/biometrics/face/1.0/IBiometricsFaceClientCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <log/log.h>

namespace android {
namespace hardware {
namespace biometrics {
namespace face {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct FaceAuthClientCallback : public V1_0::IBiometricsFaceClientCallback {
    // Methods from ::android::hardware::biometrics::face::V1_0::IBiometricsFaceClientCallback follow.
    FaceAuthClientCallback(sp <IBiometricsFaceClientCallback> callback);
    ~FaceAuthClientCallback();
    Return<void> onEnrollResult(uint64_t deviceId, uint32_t faceId, int32_t userId, uint32_t remaining) override;
    Return<void> onAuthenticated(uint64_t deviceId, uint32_t faceId, int32_t userId, const hidl_vec<uint8_t>& token) override;
    Return<void> onAcquired(uint64_t deviceId, int32_t userId, ::android::hardware::biometrics::face::V1_0::FaceAcquiredInfo acquiredInfo, int32_t vendorCode) override;
    Return<void> onError(uint64_t deviceId, int32_t userId, ::android::hardware::biometrics::face::V1_0::FaceError error, int32_t vendorCode) override;
    Return<void> onRemoved(uint64_t deviceId, const hidl_vec<uint32_t>& removed, int32_t userId) override;
    Return<void> onEnumerate(uint64_t deviceId, const hidl_vec<uint32_t>& faceIds, int32_t userId) override;
    Return<void> onLockoutChanged(uint64_t duration) override;

    public:
    sp <IBiometricsFaceClientCallback> mFaceAuthClientCallback;
};
}  // namespace implementation
}  // namespace V1_0
}  // namespace face
}  // namespace biometrics
}  // namespace hardware
}  // namespace android
