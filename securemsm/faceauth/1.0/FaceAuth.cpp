/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "QC_FACE_AUTH"
#define MAX_BUF_COUNT 4

#include "FaceAuth.h"
#include <cstdlib>
#include <inttypes.h>

namespace android::hardware::biometrics::face::V1_0::implementation {
using android::hardware::biometrics::face::V1_0::FaceError;
using android::hardware::biometrics::face::V1_0::OptionalUint64;
using vendor::qti::hardware::secureprocessor::config::V1_0::ConfigTag;

// Arbitrary value.
constexpr uint64_t kDeviceId = 123;
constexpr uint32_t kFaceId = 456;
constexpr uint64_t kAuthenticatorId = 987;
constexpr uint64_t kLockoutDuration = 555;

static bool isEnrollment;
static hidl_vec<uint8_t> token;

FaceAuth::FaceAuth() {}

static inline void camera_device_on_disconnected(void* context,
    ACameraDevice* device)
{
    ALOGD("Camera(id: %s) is diconnected.", ACameraDevice_getId(device));
    ALOGD("Context: %p", context);
}

static inline void camera_device_on_error(void* context, ACameraDevice* device,
    int error)
{
    ALOGD("Error(code: %d) on Camera(id: %s)", error,
        ACameraDevice_getId(device));
    ALOGD("Context: %p", context);
}

void FaceAuth::releaseCameraResource()
{
    camera_status_t camera_status = ACAMERA_ERROR_UNKNOWN;

    if (mCaptureRequest != NULL) {
        ACaptureRequest_free(mCaptureRequest);
        mCaptureRequest = NULL;
    }

    if (mImageReader != NULL) {
        AImageReader_delete(mImageReader);
        mImageReader = NULL;
    }

    if (mCameraOutputTarget != NULL) {
        ACameraOutputTarget_free(mCameraOutputTarget);
        mCameraOutputTarget = NULL;
    }

    if (mSessionOutput != NULL) {
        ACaptureSessionOutput_free(mSessionOutput);
        mSessionOutput = NULL;
    }

    if (mCaptureSessionOutputContainer != NULL) {
        ACaptureSessionOutputContainer_free(mCaptureSessionOutputContainer);
        mCaptureSessionOutputContainer = NULL;
    }

    if (mCameraDevice != NULL) {
        camera_status = ACameraDevice_close(mCameraDevice);

        if (camera_status != ACAMERA_OK) {
            ALOGE("Failed to close CameraDevice");
        }
        mCameraDevice = NULL;
    }
}

Return<Status> FaceAuth::setUpMSPSession()
{
    // Get MSP HAL service
    const char* TYPE_QTI_TEE = "qti-tee";
    mSecureProcessor = ISecureProcessor::getService(TYPE_QTI_TEE);
    if (mSecureProcessor == nullptr) {
        ALOGE("GetService failed for %s", TYPE_QTI_TEE);
        return Status::INTERNAL_ERROR;
    }

    // Call MSP HAL createSession() API
    ErrorCode retCode = ErrorCode::SECURE_PROCESSOR_FAIL;
    auto err = mSecureProcessor->createSession(
        [&retCode, this](ErrorCode code, uint32_t sid) {
            retCode = code;
            mSessionId = sid;
        });
    if (!err.isOk() || retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        ALOGE("createSession failed,retCode = %d,mSessionId = %d", retCode,
            mSessionId);
        return Status::INTERNAL_ERROR;
    }

    // Call MSP HAL startSession() API
    uint32_t entryLimit = 1;
    uint32_t dataLimit = 2;
    SecureProcessorConfig* configBuf = new SecureProcessorConfig(entryLimit, dataLimit);
    const int num_sensors[] = { 1 };
    uint32_t tag = (uint32_t)ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_NUM_SENSOR;
    configBuf->addEntry(tag, num_sensors, 1);

    retCode = ErrorCode::SECURE_PROCESSOR_FAIL;
    SecureProcessorCfgBuf inConfig = { 0 };
    convertToHidl(configBuf, &inConfig);
    retCode = mSecureProcessor->setConfig(mSessionId, inConfig);
    if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        ALOGE("setConfig failed,retCode = %d,mSessionId = %d", retCode, mSessionId);
        return Status::INTERNAL_ERROR;
    }

    retCode = ErrorCode::SECURE_PROCESSOR_FAIL;
    retCode = mSecureProcessor->startSession(mSessionId);
    if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        ALOGE("startSession failed,retCode = %d,mSessionId = %d", retCode,
            mSessionId);
        return Status::INTERNAL_ERROR;
    }

    return Status::OK;
}

void FaceAuth::destoryMSPSession()
{
    // Call MSP HAL stopSession() API
    ErrorCode retCode = ErrorCode::SECURE_PROCESSOR_FAIL;
    retCode = mSecureProcessor->stopSession(mSessionId);
    if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        ALOGE("stopSession failed,retCode = %d,mSessionId = %d", retCode,
            mSessionId);
        return;
    }

    // Call MSP HAL deleteSession() API
    retCode = ErrorCode::SECURE_PROCESSOR_FAIL;
    retCode = mSecureProcessor->deleteSession(mSessionId);
    if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        ALOGE("deleteSession failed,retCode = %d,mSessionId = %d", retCode,
            mSessionId);
        return;
    }
}

static void OnImageCallback(void* ctx, AImageReader* reader)
{
    reinterpret_cast<FaceAuth*>(ctx)->ImageCallback(reader);
}

void FaceAuth::ImageCallback(AImageReader* reader)
{
    ALOGD("FaceAuth::ImageCallback in");
    if (reader == nullptr) {
        ALOGE("FaceAuth::ImageCallback AImageReader is null");
        return;
    }

    media_status_t status;
    AImage* image = nullptr;
    status = AImageReader_acquireNextImage(reader, &image);
    if (status != AMEDIA_OK) {
        ALOGE("FaceAuth::ImageCallback Failed to get image");
        return;
    }

    mHwBuffer = nullptr;
    AImage_getHardwareBuffer(image, &mHwBuffer);
    if (mHwBuffer == nullptr) {
        ALOGE("FaceAuth::ImageCallback Failed to get hardware buffer");
        return;
    }

    // Call MSP HAL processImage() API
    SecureProcessorCfgBuf HIDLInCfg;
    SecureProcessorCfgBuf HIDLOutCfg;
    uint32_t entryLimit_image = 4;
    uint32_t dataLimit_image = 8;
    SecureProcessorConfig* imageConfigBuf = new SecureProcessorConfig(entryLimit_image, dataLimit_image);
    const native_handle_t* nativeHandle = AHardwareBuffer_getNativeHandle(mHwBuffer);
    if (nativeHandle == nullptr) {
        ALOGE("FaceAuth::ImageCallback  processImage nativeHandle is null");
        return;
    }

    AHardwareBuffer_Desc bufDesc;
    AHardwareBuffer_describe(mHwBuffer, &bufDesc);

    // Set frame buffer width
    uint32_t tag = (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_WIDTH;
    int32_t width = bufDesc.width;
    imageConfigBuf->addEntry(tag, &width, 1);

    // Set frame buffer height
    tag = (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_HEIGHT;
    int32_t height = bufDesc.height;
    imageConfigBuf->addEntry(tag, &height, 1);

    // Set frame buffer stride
    tag = (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_STRIDE;
    int32_t stride = bufDesc.stride;
    imageConfigBuf->addEntry(tag, &stride, 1);

    // Set frame buffer height
    tag = (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_FORMAT;
    int32_t format = bufDesc.format;
    imageConfigBuf->addEntry(tag, &format, 1);

    convertToHidl(imageConfigBuf, &HIDLInCfg);
    ErrorCode retCode = ErrorCode::SECURE_PROCESSOR_FAIL;
    auto ret = mSecureProcessor->processImage(
        mSessionId, nativeHandle, HIDLInCfg,
        [&retCode, &HIDLOutCfg](ErrorCode _status,
            SecureProcessorCfgBuf HIDLOutCfg_) {
            retCode = _status;
            if (retCode == ErrorCode::SECURE_PROCESSOR_OK) {
                HIDLOutCfg = HIDLOutCfg_;
            }
        });
    if (!ret.isOk() || retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        ALOGE("FaceAuth::ImageCallback  processImage failed, retCode = %d, "
              "mSessionId = %d",
            retCode, mSessionId);
        return;
    }

    AImage_delete(image);

    if (isEnrollment) {
        mClientCallback->onEnrollResult(kDeviceId, kFaceId, mUserId, 0);
    } else {
        mClientCallback->onAuthenticated(kDeviceId, kFaceId, mUserId, token);
    }

    releaseCameraResource();

    destoryMSPSession();
}

Return<Status> FaceAuth::getCameraFrame()
{
    // Get frame from VNDK camera
    ACameraIdList* cameraIdList = NULL;
    ACameraMetadata* cameraMetadata = NULL;
    const char* selectedCameraId = NULL;
    camera_status_t camera_status = ACAMERA_ERROR_UNKNOWN;
    media_status_t media_status = AMEDIA_OK;
    bool isBackCamera = false;
    ACameraManager* cameraManager = ACameraManager_create();

    camera_status = ACameraManager_getCameraIdList(cameraManager, &cameraIdList);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to get camera id list (reason: %d)", camera_status);
        return Status::INTERNAL_ERROR;
    }

    for (int i = 0; i < cameraIdList->numCameras; ++i) {
        camera_status = ACameraManager_getCameraCharacteristics(
            cameraManager, cameraIdList->cameraIds[i], &cameraMetadata);
        if (camera_status != ACAMERA_OK) {
            ALOGE("Unable to query camera characteristics for camera %s. Error = %d",
                cameraIdList->cameraIds[i], camera_status);
            return Status::INTERNAL_ERROR;
        }

        ACameraMetadata_const_entry entry;
        camera_status = ACameraMetadata_getConstEntry(cameraMetadata,
            ACAMERA_LENS_FACING, &entry);
        if (camera_status != ACAMERA_OK) {
            ALOGE("Unable to request capabilities for camera %s. Error = %d",
                cameraIdList->cameraIds[i], camera_status);
            continue;
        }

        if (entry.data.i32[0] == ACAMERA_LENS_FACING_BACK) {
            selectedCameraId = cameraIdList->cameraIds[i];
            isBackCamera = true;
            break;
        }
    }

    if (!isBackCamera) {
        ALOGE("No back camera device detected");
        return Status::INTERNAL_ERROR;
    }

    camera_status = ACameraManager_getCameraCharacteristics(
        cameraManager, selectedCameraId, &cameraMetadata);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to get camera meta data of ID:%s", selectedCameraId);
        return Status::INTERNAL_ERROR;
    }

    mDeviceStateCallbacks.onDisconnected = camera_device_on_disconnected;
    mDeviceStateCallbacks.onError = camera_device_on_error;
    camera_status = ACameraManager_openCamera(
        cameraManager, selectedCameraId, &mDeviceStateCallbacks, &mCameraDevice);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to open camera device (id: %s)", selectedCameraId);
        return Status::INTERNAL_ERROR;
    }

    camera_status = ACameraDevice_createCaptureRequest(
        mCameraDevice, TEMPLATE_PREVIEW, &mCaptureRequest);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to create preview capture request (id: %s)",
            selectedCameraId);
        return Status::INTERNAL_ERROR;
    }

    // set AE on
    ACameraMetadata_const_entry entry;
    camera_status = ACaptureRequest_getConstEntry(
        mCaptureRequest, ACAMERA_CONTROL_AE_MODE, &entry);
    ACaptureSessionOutputContainer_create(&mCaptureSessionOutputContainer);

    ACameraMetadata_free(cameraMetadata);
    ACameraManager_deleteCameraIdList(cameraIdList);
    ACameraManager_delete(cameraManager);

    uint64_t usage_bits;
    static const int GRALLOC1_PRODUCER_USAGE_PROTECTED = 1ULL << 14;
    usage_bits = GRALLOC1_PRODUCER_USAGE_PROTECTED;
    // usage_bits = AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN |
    // AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN; //for non-secure mode

    int* formatArray;
    int width, height;
    int numFormat = 1;
    formatArray = new int[numFormat];
    formatArray[0] = AIMAGE_FORMAT_YUV_420_888;
    width = 1920;
    height = 1080;
    media_status = AImageReader_newWithUsage(
        width, height, formatArray[0], usage_bits, MAX_BUF_COUNT, &mImageReader);
    if (media_status != AMEDIA_OK || mImageReader == nullptr) {
        ALOGE("Failed to create new AImageReader, ret=%d, image_reader_=%p",
            media_status, mImageReader);
        return Status::INTERNAL_ERROR;
    }

    AImageReader_ImageListener listener{
        .context = this, .onImageAvailable = OnImageCallback,
    };
    media_status = AImageReader_setImageListener(mImageReader, &listener);
    if (media_status != AMEDIA_OK) {
        ALOGE("Failed to set image listener");
        return Status::INTERNAL_ERROR;
    }

    media_status = AImageReader_getWindowNativeHandle(mImageReader, &mImgReaderAnw);
    if (media_status != AMEDIA_OK) {
        ALOGE("Failed to get image reader native window");
        return Status::INTERNAL_ERROR;
    }

    camera_status = ACaptureSessionOutput_create(mImgReaderAnw, &mSessionOutput);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to create mSessionOutput, camera_status %d", camera_status);
        return Status::INTERNAL_ERROR;
    }

    camera_status = ACaptureSessionOutputContainer_add(
        mCaptureSessionOutputContainer, mSessionOutput);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to add mSessionOutput in mCaptureSessionOutputContainer, "
              "camera_status %d",
            camera_status);
        return Status::INTERNAL_ERROR;
    }

    camera_status = ACameraOutputTarget_create(mImgReaderAnw, &mCameraOutputTarget);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to create mCameraOutputTarget, camera_status %d",
            camera_status);
        return Status::INTERNAL_ERROR;
    }

    camera_status = ACaptureRequest_addTarget(mCaptureRequest, mCameraOutputTarget);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to add mCameraOutputTarget in mCaptureRequest, camera_status "
              "%d",
            camera_status);
        return Status::INTERNAL_ERROR;
    }

    camera_status = ACameraDevice_createCaptureSession(
        mCameraDevice, mCaptureSessionOutputContainer,
        &mCaptureSessionStateCallbacks, &mCaptureSession);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to create mCaptureSession by CameraDevice, camera_status %d",
            camera_status);
        return Status::INTERNAL_ERROR;
    }

    camera_status = ACameraCaptureSession_setRepeatingRequest(
        mCaptureSession, NULL, 1, &mCaptureRequest, NULL);
    if (camera_status != ACAMERA_OK) {
        ALOGE("Failed to setRepeatingRequest by mCaptureSession, camera_status %d",
            camera_status);
        return Status::INTERNAL_ERROR;
    }

    return Status::OK;
}

// Methods from IBiometricsFace follow.
Return<void>
FaceAuth::setCallback(const sp<IBiometricsFaceClientCallback>& clientCallback,
    setCallback_cb _hidl_cb)
{
    ALOGI("FaceAuth.cpp setCallback in");
    OptionalUint64 ret;

    sp<FaceAuthClientCallback> callback(
        new FaceAuthClientCallback(clientCallback));
    mClientCallback = callback;
    ret.status = Status::OK;
    ret.value = kDeviceId;
    ALOGI("FaceAuth.cpp setCallback - calling _hidl_cb with status = %d, val = "
          "0x%" PRIx64,
        ret.status, ret.value);
    _hidl_cb(ret);
    return Void();
}

Return<Status> FaceAuth::setActiveUser(int32_t userId,
    const hidl_string& storePath)
{
    ALOGI("FaceAuth.cpp setActiveUser in");
    if (userId < 0 || storePath.empty() || std::string(storePath).find("/data") != 0) {
        return Status::ILLEGAL_ARGUMENT;
    }

    mUserId = userId;
    mClientCallback->onLockoutChanged(kLockoutDuration);
    return Status::OK;
}

Return<void> FaceAuth::generateChallenge(uint32_t challengeTimeoutSec,
    generateChallenge_cb _hidl_cb)
{
    ALOGI("FaceAuth.cpp generateChallenge in");
    (void)challengeTimeoutSec;
    OptionalUint64 ret;

    int fd = -1;
    fd = open("/dev/urandom", O_RDONLY);
    read(fd, &mRandom, sizeof(mRandom));
    close(fd);
    ret.status = Status::OK;
    ret.value = mRandom;
    ALOGI("FaceAuth.cpp generateChallenge - the challenge is 0x%" PRIx64,
        ret.value);
    _hidl_cb(ret);
    return Void();
}

Return<Status> FaceAuth::enroll(const hidl_vec<uint8_t>& hat,
    uint32_t timeoutSec,
    const hidl_vec<Feature>& disabledFeatures)
{
    ALOGI("FaceAuth.cpp enroll in");
    (void)disabledFeatures;
    token = hat;
    ALOGI("FaceAuth.cpp enroll - google timeout = %d", timeoutSec);

    if (setUpMSPSession() != Status::OK) {
        ALOGE("Failed to run setUpMSPSession()");
        return Status::INTERNAL_ERROR;
    }

    /**
      Enrollment bussiness process
      1.verify token
      2.get the secure camera frame
      3.create a user and save it in SFS
      4.return the enrollment result
    */

    /**
     * The face enrollment logic should have been done using a front camera. But
     * this demo uses Android Settings APK for testing, while in Google's default
     * Settings APK, the front camera has been opened before this code is called,
     * so the front camera cannot be used here. For testing only, the back camera
     * is opened here. In commercial products, please open the camera you need in
     * combination with the actual needs.
     */
    if (getCameraFrame() != Status::OK) {
        ALOGE("Failed to run getCameraFrame()");
        return Status::INTERNAL_ERROR;
    }

    isEnrollment = true;

    return Status::OK;
}

Return<Status> FaceAuth::revokeChallenge()
{
    ALOGI("FaceAuth.cpp revokeChallenge in");
    return Status::OK;
}

Return<Status> FaceAuth::setFeature(Feature /* feature */, bool /* enabled */,
    const hidl_vec<uint8_t>& /* hat */,
    uint32_t /* faceId */)
{
    ALOGI("FaceAuth.cpp setFeature in");
    return Status::ILLEGAL_ARGUMENT;
}

Return<void> FaceAuth::getFeature(Feature /* feature */, uint32_t /* faceId */,
    getFeature_cb _hidl_cb)
{
    ALOGI("FaceAuth.cpp getFeature in");
    _hidl_cb({ Status::ILLEGAL_ARGUMENT, false });
    return Void();
}

Return<void> FaceAuth::getAuthenticatorId(getAuthenticatorId_cb _hidl_cb)
{
    ALOGI("FaceAuth.cpp getAuthenticatorId in");
    _hidl_cb({ Status::OK, kAuthenticatorId });
    return Void();
}

Return<Status> FaceAuth::cancel()
{
    ALOGI("FaceAuth.cpp cancel in");
    mClientCallback->onError(kDeviceId, mUserId, FaceError::CANCELED,
        0 /* vendorCode */);
    return Status::OK;
}

Return<Status> FaceAuth::enumerate()
{
    ALOGI("FaceAuth.cpp enumerate in");
    mClientCallback->onEnumerate(kDeviceId, {}, mUserId);
    return Status::OK;
}

Return<Status> FaceAuth::remove(uint32_t /* faceId */)
{
    ALOGI("FaceAuth.cpp remove in");

    /**
      Removal business process
      1.remove user
      2.get database file path
      3.remove database in SFS
    */

    setUpMSPSession();

    uint32_t numFaceIds = 1;
    hidl_vec<uint32_t> faceIds;
    faceIds.resize(numFaceIds);
    faceIds[0] = kFaceId;
    mClientCallback->onRemoved(kDeviceId, faceIds, mUserId);

    destoryMSPSession();

    return Status::OK;
}

Return<Status> FaceAuth::authenticate(uint64_t operationId)
{
    ALOGI("FaceAuth.cpp authenticate in, operationId = 0x%" PRIx64, operationId);

    /**
      Authentication bussiness process
      1.get operation_id from the request message
      2.get hw auth token from gatekeeper
      3.copy hat into local token
      4.verify token
      5.get the secure camera frame
      6.match a user face
      7.return the authenticate result
    */

    if (setUpMSPSession() != Status::OK) {
        ALOGE("Failed to run setUpMSPSession()");
        return Status::INTERNAL_ERROR;
    }

    if (getCameraFrame() != Status::OK) {
        ALOGE("Failed to run getCameraFrame()");
        return Status::INTERNAL_ERROR;
    }

    isEnrollment = false;

    return Status::OK;
}

Return<Status> FaceAuth::userActivity()
{
    ALOGD("FaceAuth.cpp userActivity in");
    return Status::OK;
}

Return<Status> FaceAuth::resetLockout(const hidl_vec<uint8_t>& /* hat */)
{
    ALOGD("FaceAuth.cpp resetLockout in");
    return Status::OK;
}

} // namespace anrdroid::hardware::biometrics::face::V1_0::implementation
