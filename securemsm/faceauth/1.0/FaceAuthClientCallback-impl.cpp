/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "QC_FACE_AUTH"

#include "FaceAuthClientCallback-impl.h"
#include <inttypes.h>

namespace android {
namespace hardware {
namespace biometrics {
namespace face {
namespace V1_0 {
namespace implementation {

FaceAuthClientCallback::FaceAuthClientCallback(sp <IBiometricsFaceClientCallback> callback) {
    ALOGD("FaceAuthClientCallback constructor");
    mFaceAuthClientCallback = callback;
}

FaceAuthClientCallback::~FaceAuthClientCallback() {
    ALOGD("FaceAuthClientCallback destructor");
    mFaceAuthClientCallback = NULL;
}

// Methods from ::android::hardware::biometrics::face::V1_0::IBiometricsFaceClientCallback follow.
Return<void> FaceAuthClientCallback::onEnrollResult(uint64_t deviceId, uint32_t faceId, int32_t userId, uint32_t remaining) {
    ALOGD("FaceAuthClientCallback onEnrollResult - deviceId = 0x%" PRIx64 ", faceId = %d, userId = %d, remaining = %d", deviceId, faceId, userId, remaining);
    mFaceAuthClientCallback->onEnrollResult(deviceId, faceId, userId, remaining);
    return Void();
}

Return<void> FaceAuthClientCallback::onAuthenticated(uint64_t deviceId, uint32_t faceId, int32_t userId, const hidl_vec<uint8_t>& token) {
    ALOGD("FaceAuthClientCallback onAuthenticated - deviceId = 0x%" PRIx64 ", faceId = %d, userId = %d", deviceId, faceId, userId);
    mFaceAuthClientCallback->onAuthenticated(deviceId, faceId, userId, token);
    return Void();
}

Return<void> FaceAuthClientCallback::onAcquired(uint64_t deviceId, int32_t userId, ::android::hardware::biometrics::face::V1_0::FaceAcquiredInfo acquiredInfo, int32_t vendorCode) {
    ALOGD("FaceAuthClientCallback onAcquired - deviceId = 0x%" PRIx64 ", userId = %d, acquiredInfo = %d, vendorCode = %d", deviceId, userId, acquiredInfo, vendorCode);
    mFaceAuthClientCallback->onAcquired(deviceId, userId, acquiredInfo, vendorCode);
    return Void();
}

Return<void> FaceAuthClientCallback::onError(uint64_t deviceId, int32_t userId, ::android::hardware::biometrics::face::V1_0::FaceError error, int32_t vendorCode) {
    ALOGD("FaceAuthClientCallback onError - deviceId = 0x%" PRIx64 ", userId = %d, error = %d, vendorCode = %d", deviceId, userId, error, vendorCode);
    mFaceAuthClientCallback->onError(deviceId, userId, error, vendorCode);
    return Void();
}

Return<void> FaceAuthClientCallback::onRemoved(uint64_t deviceId, const hidl_vec<uint32_t>& removed, int32_t userId) {
    ALOGD("FaceAuthClientCallback onRemoved - deviceId = 0x%" PRIx64 ", userId = %d", deviceId, userId);
    for(int i = 0; i < removed.size(); i++ ){
        ALOGD("FaceAuthClientCallback onRemoved - removed[%d] =  %d ", i, removed[i]);
    }
    mFaceAuthClientCallback->onRemoved(deviceId, removed, userId);
    return Void();
}

Return<void> FaceAuthClientCallback::onEnumerate(uint64_t deviceId, const hidl_vec<uint32_t>& faceIds, int32_t userId) {
    ALOGD("FaceAuthClientCallback onEnumerate - deviceId = 0x%" PRIx64 ", faceIds.size() = %zu, userId = %d", deviceId, faceIds.size(), userId);
    mFaceAuthClientCallback->onEnumerate(deviceId, faceIds, userId);
    return Void();
}

Return<void> FaceAuthClientCallback::onLockoutChanged(uint64_t duration) {
    // TODO implement
    ALOGD("FaceAuthClientCallback - onLockoutChanged - duration = 0x%" PRIx64, duration);
    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace face
}  // namespace biometrics
}  // namespace hardware
}  // namespace android
