/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <android/hardware/biometrics/face/1.0/IBiometricsFace.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <random>
#include <log/log.h>
#include <hardware/hw_auth_token.h>
#include "FaceAuthClientCallback-impl.h"

#include <vendor/qti/hardware/secureprocessor/config/1.0/types.h>
#include <vendor/qti/hardware/secureprocessor/device/1.0/ISecureProcessor.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/RefBase.h>
#include "SecureProcessorConfig.h"
#include "SecureProcessorUtils.h"

#include <NdkCameraDevice.h>
#include <NdkCameraManager.h>
#include <NdkImageReader.h>
#include <ui/GraphicBuffer.h>
#include <media/NdkImage.h>
#include <vndk/hardware_buffer.h>

namespace android::hardware::biometrics::face::V1_0::implementation {

using ::android::sp;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::biometrics::face::V1_0::Feature;
using ::android::hardware::biometrics::face::V1_0::IBiometricsFaceClientCallback;
using ::android::hardware::biometrics::face::V1_0::Status;
using vendor::qti::hardware::secureprocessor::device::V1_0::ISecureProcessor;
using vendor::qti::hardware::secureprocessor::common::V1_0::ErrorCode;
using vendor::qti::hardware::secureprocessor::config::V1_0::ConfigTag;
using vendor::qti::hardware::secureprocessor::config::V1_0::ConfigType;
using vendor::qti::hardware::secureprocessor::common::V1_0::SecureProcessorConfig;
using vendor::qti::hardware::secureprocessor::common::V1_0::SecureProcessorCfgBuf;

class FaceAuth : public V1_0::IBiometricsFace {
  public:
    FaceAuth();

    // Methods from ::android::hardware::biometrics::face::V1_0::IBiometricsFace follow.
    Return<void> setCallback(const sp<IBiometricsFaceClientCallback>& clientCallback,
                             setCallback_cb _hidl_cb) override;

    Return<Status> setActiveUser(int32_t userId, const hidl_string& storePath) override;

    Return<void> generateChallenge(uint32_t challengeTimeoutSec,
                                   generateChallenge_cb _hidl_cb) override;

    Return<Status> enroll(const hidl_vec<uint8_t>& hat, uint32_t timeoutSec,
                          const hidl_vec<Feature>& disabledFeatures) override;

    Return<Status> revokeChallenge() override;

    Return<Status> setFeature(Feature feature, bool enabled, const hidl_vec<uint8_t>& hat,
                              uint32_t faceId) override;

    Return<void> getFeature(Feature feature, uint32_t faceId, getFeature_cb _hidl_cb) override;

    Return<void> getAuthenticatorId(getAuthenticatorId_cb _hidl_cb) override;

    Return<Status> cancel() override;

    Return<Status> enumerate() override;

    Return<Status> remove(uint32_t faceId) override;

    Return<Status> authenticate(uint64_t operationId) override;

    Return<Status> userActivity() override;

    Return<Status> resetLockout(const hidl_vec<uint8_t>& hat) override;

  private:
    int64_t mRandom;
    int32_t mUserId;

  public:
    sp<FaceAuthClientCallback> mClientCallback;
    sp<ISecureProcessor> mSecureProcessor;
    uint32_t mSessionId;
    void ImageCallback(AImageReader *reader);
    void releaseCameraResource();
    void destoryMSPSession();
    Return<Status> setUpMSPSession();
    Return<Status> getCameraFrame();

  protected:
    ACameraDevice *mCameraDevice;
    ACaptureRequest *mCaptureRequest;
    ACaptureSessionOutputContainer *mCaptureSessionOutputContainer;
    ACameraCaptureSession *mCaptureSession;

    ACameraDevice_StateCallbacks mDeviceStateCallbacks;
    ACameraCaptureSession_stateCallbacks mCaptureSessionStateCallbacks;

    AImageReader *mImageReader;
    AHardwareBuffer *mHwBuffer;
    ACameraOutputTarget *mCameraOutputTarget;
    ACaptureSessionOutput *mSessionOutput;
    native_handle_t *mImgReaderAnw;
};

}  // namespace android::hardware::biometrics::face::V1_0::implementation
