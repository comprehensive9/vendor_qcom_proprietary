/**
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "SECURE-PROCESSOR-TEE"

#include <hardware/gralloc1.h>

#include "SecureProcessorTEE.h"
#include "utils.h"

namespace vendor
{
namespace qti
{
namespace hardware
{
namespace secureprocessor
{
namespace device
{
namespace V1_0
{
namespace implementation
{
using vendor::qti::hardware::secureprocessor::common::V1_0::ErrorCode;
using vendor::qti::hardware::secureprocessor::config::V1_0::ConfigType;
using vendor::qti::hardware::secureprocessor::config::V1_0::ConfigTag;

#define VALID_SESSION(id) (((id) < MAX_SESSION) && (sConfig_[(id)].active))

#define VALID_RESPONSE(status, ret) (((status) == 0) && ((ret) == 0))

#define CHECK_COND(cond, error_code)                           \
    if (!(cond)) {                                             \
        ALOGE("\n%s failed on line %d\n", __func__, __LINE__); \
        ret = (error_code);                                    \
        goto EXIT;                                             \
    }

#define CHECK_ERR(ret) \
    CHECK_COND((ret) == ErrorCode::SECURE_PROCESSOR_OK, (ret))

#define GET_CONFIG 0
#define SET_CONFIG 1
#define SESSION_CONFIG 0
#define IMAGE_CONFIG 1
#define GET_SESSION_CONFIG (SESSION_CONFIG << 1 | GET_CONFIG)  // 0
#define SET_SESSION_CONFIG (SESSION_CONFIG << 1 | SET_CONFIG)  // 1
#define GET_IMAGE_CONFIG (IMAGE_CONFIG << 1 | GET_CONFIG)      // 2
#define SET_IMAGE_CONFIG (IMAGE_CONFIG << 1 | SET_CONFIG)      // 3
#define SET_CONFIG_MASK 0x1

// Extending ConfigTag
#define SECURE_PROCESSOR_CUSTOM_CONFIG_USECASE_LICENSE \
    ((int)ConfigTag::SECURE_PROCESSOR_CUSTOM_CONFIG_START + 1)
#define SECURE_PROCESSOR_CUSTOM_CONFIG_USECASE_RESET_CAMERA \
    ((int)ConfigTag::SECURE_PROCESSOR_CUSTOM_CONFIG_START + 2)

// Extending ErrorCode
#define SECURE_PROCESSOR_LICENSE_ERROR \
    ((int)ErrorCode::SECURE_PROCESSOR_CUSTOM_STATUS + 1)

// 1024 byte license
static const uint8_t default_license[1024] = {0};

// Private Methods
ErrorCode SecureProcessorTEE::_loadTEEApp(char *appName,
                                          struct QSEECom_handle **teeHandle)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    struct QSEECom_handle *handle = nullptr;
    uint32_t location_count = 5;
    const char *location[] = {
        "/vendor/firmware_mnt/image", "/vendor/firmware",
        "/vendor/etc/firmware",       "/firmware/image",
        "/system/etc/firmware",
    };

    // Unload existing teeHandle if not NULL
    if (*teeHandle != NULL) {
        // ignore error
        _unloadTEEApp(teeHandle);
    }

    for (uint32_t i = 0; i < location_count; i++) {
        if (OK == QSEECom_start_app(&handle, location[i], appName,
                                    SHARED_BUFFER_SIZE)) {
            ALOGI("%s: App [%s] loaded successfully from [%s]\n", __func__,
                  appName, location[i]);
            ret = ErrorCode::SECURE_PROCESSOR_OK;
            break;
        }
    }

    CHECK_ERR(ret);

    *teeHandle = handle;

EXIT:
    return ret;
}

ErrorCode SecureProcessorTEE::_unloadTEEApp(struct QSEECom_handle **teeHandle)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;

    if (QSEECom_shutdown_app(teeHandle)) {
        ALOGE("%s: failed to unload app\n", __func__);
    } else {
        ALOGI("%s: unloaded app\n", __func__);
        ret = ErrorCode::SECURE_PROCESSOR_OK;
    }

    return ret;
}

ErrorCode SecureProcessorTEE::_processConfig(uint32_t sessionId,
                                             uint32_t operation,
                                             const hidl_vec<uint8_t> &inConfig)
{
    SecureProcessorCfgBuf outConfig = {0};
    return _processConfig(sessionId, operation, inConfig, outConfig);
}

ErrorCode SecureProcessorTEE::_processConfig(uint32_t sessionId,
                                             uint32_t operation,
                                             const hidl_vec<uint8_t> &inConfig,
                                             hidl_vec<uint8_t> &outConfig)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    SecureProcessorConfig *cfgOutWrapper = nullptr;
    uint32_t numEntries = 0;

    // Convert HIDL config to config wrapper class
    SecureProcessorConfig *cfgInWrapper = nullptr;
    convertFromHidl(&inConfig, &cfgInWrapper);
    CHECK_COND(cfgInWrapper != nullptr, ErrorCode::SECURE_PROCESSOR_FAIL);

    // Retrive number of config entries
    numEntries = cfgInWrapper->entryCount();
    ALOGD("%s: numEntries: %d, op: 0x%x\n", __func__, numEntries, operation);

    if ((operation & SET_CONFIG_MASK) == GET_CONFIG) {
        // Allocate the SecureProcessorConfig class for outConfig and
        // it's associated config buffer with requested number of entries
        cfgOutWrapper = new SecureProcessorConfig(numEntries);
        CHECK_COND(cfgOutWrapper != nullptr, ErrorCode::SECURE_PROCESSOR_FAIL);
    }

    // Loop over numEntries and process one entry at a time
    for (uint32_t index = 0; index < numEntries; index++) {
        auto entry = cfgInWrapper->getEntryByIndex(index);
        switch (operation) {
            case GET_IMAGE_CONFIG:
                // TBD: To be implemented based on needs
                break;
            case SET_IMAGE_CONFIG:
                ret = _HandleImageCfg(sessionId, &entry);
                break;
            case GET_SESSION_CONFIG:
                ret = _createSessionCfg(sessionId, cfgOutWrapper, &entry);
                break;
            case SET_SESSION_CONFIG:
                ret = _HandleSessionCfg(sessionId, &entry);
                break;
            default:
                ret = ErrorCode::SECURE_PROCESSOR_BAD_VAL;
                break;
        }

        CHECK_ERR(ret);
    }

    ret = ErrorCode::SECURE_PROCESSOR_OK;

EXIT:
    ALOGE("%s: ret: %d, op: 0x%x\n", __func__, ret, operation);
    // Delete cfgInWrapper object after detaching the externally set buffer
    if (cfgInWrapper != nullptr) {
        (void)cfgInWrapper->releaseAndGetBuffer();
        // Free the config wrapper class
        delete cfgInWrapper;
    }

    if ((operation & 0x1) == GET_CONFIG) {
        // Populate outConfig HIDL vector
        convertToHidl(cfgOutWrapper, &outConfig);

        // Delete cfgOutWrapper object
        delete cfgOutWrapper;
    }

    return ret;
}

/**
 * resetCamera:
 *
 * Reset camera to clear secure memory of a session that
 * has already been started.
 *
 * @param sessionId Session identifier.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Camera is reset successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Camera reset fails.
 *
 */
ErrorCode SecureProcessorTEE::_resetCamera(uint32_t sessionId)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_OK;
    int status = 0;
    seccam_cmd_req_t cmdRequest = {};
    seccam_cmd_rsp_t cmdResponse = {};

    CHECK_COND(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);
    CHECK_COND(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_RUNNING,
               ErrorCode::SECURE_PROCESSOR_FAIL);

    sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_RESET;

    // Send shutdown command
    cmdRequest.cmd_id = SECCAM_CMD_RESET_CAMERA;
    status = QSEECom_send_cmd(sConfig_[sessionId].teeHandle, &cmdRequest,
                              sizeof(seccam_cmd_req_t), &cmdResponse,
                              sizeof(seccam_cmd_rsp_t));
    CHECK_COND(VALID_RESPONSE(status, cmdResponse.ret),
               ErrorCode::SECURE_PROCESSOR_FAIL);

    sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_RUNNING;

EXIT:
    return ret;
}

ErrorCode SecureProcessorTEE::_HandleSessionCfg(uint32_t sessionId,
                                                ConfigEntry *entry)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    ConfigTag tag = (ConfigTag)entry->tag;
    ConfigType type = (ConfigType)entry->type;

    switch (tag) {
        case ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_NUM_SENSOR: {
            CHECK_COND(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(type == ConfigType::INT32,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            if (entry->count != 1) {
                ALOGE("%s: unexpected count[%d], expected[1] for tag[0x%x]\n",
                      __func__, entry->count, tag);
                break;
            }
            int32_t value = *(entry->data.i32);
            ALOGD("%s: TAG: 0x%x, VALUE = %d\n", __func__, tag, value);
            sConfig_[sessionId].numSensor = value;
            ret = ErrorCode::SECURE_PROCESSOR_OK;
            break;
        }
        case ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_USECASE_IDENTIFIER: {
            CHECK_COND(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(type == ConfigType::BYTE,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            uint8_t *value = entry->data.u8;
            uint32_t size = entry->count;

            CHECK_COND(value != NULL, ErrorCode::SECURE_PROCESSOR_FAIL);
            if (0 != memcmp((void *)sConfig_[sessionId].taName.c_str(),
                            (void *)value, size) ||
                (sConfig_[sessionId].teeHandle == NULL)) {
                // TA name is different or app is not loaded
                sConfig_[sessionId].taName.resize(size);
                common::V1_0::memcpy_s(
                    (void *)(sConfig_[sessionId].taName.data()),
                    sConfig_[sessionId].taName.size(), value, size);
                ALOGD("%s: TAG: 0x%x, VALUE = %s\n", __func__, tag, value);

                CHECK_ERR(ret = _loadTEEApp(
                              (char *)(sConfig_[sessionId].taName.data()),
                              &(sConfig_[sessionId].teeHandle)));
            }
            ret = ErrorCode::SECURE_PROCESSOR_OK;
            break;
        }
        case (ConfigTag)SECURE_PROCESSOR_CUSTOM_CONFIG_USECASE_LICENSE: {
            CHECK_COND(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(type == ConfigType::BYTE,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            uint8_t *value = entry->data.u8;
            uint32_t size = entry->count;

            CHECK_COND(size > 0 && size < MAX_CERT_SIZE,
                       (ErrorCode)SECURE_PROCESSOR_LICENSE_ERROR);

            sConfig_[sessionId].license_size = size;
            common::V1_0::memcpy_s((void *)(sConfig_[sessionId].license),
                                   MAX_CERT_SIZE, value, size);
            ALOGD("%s:TAG: 0x%x, VALUE = %s\n", __func__, tag, value);
            ret = ErrorCode::SECURE_PROCESSOR_OK;

            break;
        }
        case (ConfigTag)SECURE_PROCESSOR_CUSTOM_CONFIG_USECASE_RESET_CAMERA: {
            CHECK_COND(
                sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_RUNNING,
                ErrorCode::SECURE_PROCESSOR_FAIL);

            ret = _resetCamera(sessionId);
            ALOGD("%s:TAG: 0x%x, sessionid = %d, ret = %d\n", __func__, tag,
                  sessionId, ret);
            break;
        }
        default:
            ALOGE("%s: unexpected TAG: 0x%x, ignored\n", __func__, tag);
            ret = ErrorCode::SECURE_PROCESSOR_BAD_VAL;
            break;
    }

EXIT:
    return ret;
}

ErrorCode SecureProcessorTEE::_HandleImageCfg(uint32_t sessionId,
                                              ConfigEntry *entry)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_OK;
    ConfigTag tag = (ConfigTag)entry->tag;
    ConfigType type = (ConfigType)entry->type;

    switch (tag) {
        case ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_CAMERA_ID: {
            CHECK_COND(type == ConfigType::INT32,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            ALOGD("%s: TAG: 0x%x, VALUE = %d\n", __func__, tag, value);
            sConfig_[sessionId].imgCfg.cameraId = value;
            break;
        }
        case ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_NUMBER: {
            CHECK_COND(type == ConfigType::INT64,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int64_t value = *(entry->data.i64);
            ALOGD("%s: TAG: 0x%x, VALUE = %ld\n", __func__, tag, value);
            sConfig_[sessionId].imgCfg.frameNum = value;
            break;
        }
        case ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_TIMESTAMP: {
            CHECK_COND(type == ConfigType::INT64,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int64_t value = *(entry->data.i64);
            ALOGD("%s: TAG: 0x%x, VALUE = %ld\n", __func__, tag, value);
            sConfig_[sessionId].imgCfg.timeStamp = value;
            break;
        }
        case ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_WIDTH: {
            CHECK_COND(type == ConfigType::INT32,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            ALOGD("%s: TAG: 0x%x, VALUE = %d\n", __func__, tag, value);
            sConfig_[sessionId].imgCfg.buffer.width = value;
            break;
        }
        case ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_HEIGHT: {
            CHECK_COND(type == ConfigType::INT32,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            ALOGD("%s: TAG: 0x%x, VALUE = %d\n", __func__, tag, value);
            sConfig_[sessionId].imgCfg.buffer.height = value;
            break;
        }
        case ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_STRIDE: {
            CHECK_COND(type == ConfigType::INT32,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            ALOGD("%s: TAG: 0x%x, VALUE = %d\n", __func__, tag, value);
            sConfig_[sessionId].imgCfg.buffer.stride = value;
            break;
        }
        case ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_FORMAT: {
            CHECK_COND(type == ConfigType::INT32,
                       ErrorCode::SECURE_PROCESSOR_FAIL);
            CHECK_COND(entry->count == 1, ErrorCode::SECURE_PROCESSOR_BAD_VAL);
            int32_t value = *(entry->data.i32);
            ALOGD("%s: TAG: 0x%x, VALUE = %d\n", __func__, tag, value);
            sConfig_[sessionId].imgCfg.buffer.format = value;
            break;
        }
        default:
            ALOGE("%s: unexpected TAG: 0x%x, ignored\n", __func__, tag);
            ret = ErrorCode::SECURE_PROCESSOR_FAIL;
            break;
    }

EXIT:
    return ret;
}

ErrorCode SecureProcessorTEE::_createSessionCfg(
    uint32_t sessionId, SecureProcessorConfig *cfgWrapper, ConfigEntry *entry)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    ConfigTag tag = (ConfigTag)entry->tag;

    switch (tag) {
        case ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_NUM_SENSOR: {
            cfgWrapper->addEntry(entry->tag, &sConfig_[sessionId].numSensor, 1);
            ALOGD("%s: created TAG: 0x%x, VALUE = %d\n", __func__, tag,
                  sConfig_[sessionId].numSensor);
            ret = ErrorCode::SECURE_PROCESSOR_OK;
            break;
        }
        case ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_USECASE_IDENTIFIER: {
            uint8_t *value = (uint8_t *)sConfig_[sessionId].taName.data();
            cfgWrapper->addEntry(entry->tag, value,
                                 sConfig_[sessionId].taName.size());
            ALOGD("%s: created TAG: 0x%x, VALUE = %s\n", __func__, tag, value);
            ret = ErrorCode::SECURE_PROCESSOR_OK;
            break;
        }
        default:
            ALOGE("%s: unexpected TAG: 0x%x, ignored\n", __func__, tag);
            break;
    }

    return ret;
}

void SecureProcessorTEE::_resetImgCfg(uint32_t sessionId)
{
    std::memset(&sConfig_[sessionId].imgCfg, 0,
                sizeof(&sConfig_[sessionId].imgCfg));
}

ErrorCode SecureProcessorTEE::_updatePlanes(uint32_t sessionId,
                                            seccam_plane_info_t *planeData,
                                            uint32_t *num_of_in_planes)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    uint32_t yStride = 0;
    uint32_t uvStride = 0;
    uint32_t uvOffset = 0;
    uint32_t numPlanes = 1;
    ImageBuffer *buffer = &sConfig_[sessionId].imgCfg.buffer;
    bool yuvSP = isYUVSPFormat(buffer->format);

    if (!yuvSP) {
        planeData[0].row_stride = buffer->stride;
        planeData[0].offset = 0;
        planeData[0].pixel_stride = 1;
        ret = ErrorCode::SECURE_PROCESSOR_OK;
        goto EXIT;
    }

    // YUV SemiPlanar
    yStride = calcYStride(buffer->format, buffer->stride);
    uvStride = calcUVStride(buffer->format, buffer->stride);
    uvOffset = calcUVOffset(buffer->format, buffer->stride, buffer->height);
    numPlanes = 3;
    planeData[0].offset = 0;
    planeData[0].row_stride = yStride;
    planeData[0].pixel_stride = 1;
    planeData[1].offset = uvOffset;
    planeData[1].row_stride = uvStride;
    planeData[1].pixel_stride = 2;
    planeData[2].offset = uvOffset + 1;
    planeData[2].row_stride = uvStride;
    planeData[2].pixel_stride = 2;

    *num_of_in_planes = numPlanes;

    ret = ErrorCode::SECURE_PROCESSOR_OK;

EXIT:
    return ret;
}

ErrorCode SecureProcessorTEE::_processImage(uint32_t sessionId,
                                            const hidl_handle &image)
{
    int status = 0;
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    seccam_cmd_req_t cmdRequest = {};
    seccam_cmd_rsp_t cmdResponse = {};
    seccam_plane_req_data_t *planeData = &cmdRequest.plane_data[0];
    uint32_t timeTaken = 0;
    int64_t startTime = getTimeUsec();
    int64_t endTime = 0;
    int fdIndex = 0;
    struct QSEECom_ion_fd_info ion_fd_info;

    cmdRequest.cmd_id = SECCAM_CMD_PROCESS_FRAME;
    cmdRequest.buf = image->data[0];
    cmdRequest.buf_size =
        calcBufferSize(sConfig_[sessionId].imgCfg.buffer.format,
                       sConfig_[sessionId].imgCfg.buffer.width,
                       sConfig_[sessionId].imgCfg.buffer.stride,
                       sConfig_[sessionId].imgCfg.buffer.height);
    cmdRequest.frame_info.cam_id = sConfig_[sessionId].imgCfg.cameraId;
    cmdRequest.frame_info.frame_number = sConfig_[sessionId].imgCfg.frameNum;
    cmdRequest.frame_info.time_stamp = sConfig_[sessionId].imgCfg.timeStamp;

    CHECK_ERR(ret = _updatePlanes(sessionId, planeData->in_planes,
                                  &planeData->num_of_in_planes));

    // Send image frame to TEE for processing
    std::memset((void *)&ion_fd_info, 0, sizeof(struct QSEECom_ion_fd_info));

    ion_fd_info.data[0].fd = -1;
    ion_fd_info.data[1].fd = -1;
    ion_fd_info.data[2].fd = -1;
    ion_fd_info.data[3].fd = -1;

    if (cmdRequest.buf != 0) {
        ion_fd_info.data[fdIndex].fd = cmdRequest.buf;
        ion_fd_info.data[fdIndex].cmd_buf_offset =
            offsetof(seccam_cmd_req_t, buf);
    }

    status = QSEECom_send_modified_cmd(
        sConfig_[sessionId].teeHandle, &cmdRequest, sizeof(seccam_cmd_req_t),
        &cmdResponse, sizeof(seccam_cmd_rsp_t), &ion_fd_info);
    CHECK_COND(VALID_RESPONSE(status, cmdResponse.ret),
               ErrorCode::SECURE_PROCESSOR_FAIL);

    endTime = getTimeUsec();
    if (endTime >= startTime) {
        timeTaken = (int32_t)(endTime - startTime);
    } else {
        timeTaken = (int32_t)((LLONG_MAX - startTime) + endTime);
    }

    ALOGD(
        "%s: CMD_FRAME_NOTIFICATION completed with tee_status: %d, "
        "duration: %uus",
        __func__, cmdResponse.ret, timeTaken);

    ret = ErrorCode::SECURE_PROCESSOR_OK;

// Retrieve additional custom image config here from cmdResponse

EXIT:
    return ret;
}

// SecureProcessorTEE interface
SecureProcessorTEE::SecureProcessorTEE()
{
    std::memset(sConfig_, 0, sizeof(sConfig_));
}

SecureProcessorTEE::~SecureProcessorTEE()
{
    // sessionId start with 1 instead of 0
    for (uint32_t sessionId = 1; sessionId < MAX_SESSION; sessionId++) {
        if (sConfig_[sessionId].active) {
            stopSession(sessionId);
            deleteSession(sessionId);
        }
    }
}

// Methods from ::vendor::qti::hardware::secureprocessor::V1_0::ISecureProcessor
// follow.

/**
 * createSession:
 *
 * Create a new secure data processor session for image data processing.
 * It creates and returns a unique session identifier for subsequent
 * interactions to this session.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         New session created successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         New session creation failed.
 *
 * @return sessionId New session identifier.
 *
 */
Return<void> SecureProcessorTEE::createSession(createSession_cb _hidl_cb)
{
    uint32_t sessionId = 0;
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    char defaultTa[] = "seccamdemo2";

    std::lock_guard<std::mutex> lock(mLock_);

    // sessionId start with 1 instead of 0
    for (sessionId = 1; sessionId < MAX_SESSION; sessionId++) {
        if (sConfig_[sessionId].active) {
            continue;
        }

        CHECK_ERR((ret = _loadTEEApp((char *)defaultTa,
                                     &(sConfig_[sessionId].teeHandle))));

        sConfig_[sessionId].active = true;
        sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_INIT;
        sConfig_[sessionId].taName.resize(sizeof(defaultTa));
        common::V1_0::memcpy_s((void *)(sConfig_[sessionId].taName.data()),
                               sConfig_[sessionId].taName.size(), defaultTa,
                               sizeof(defaultTa));
        // Use default license
        sConfig_[sessionId].license_size = sizeof(default_license);
        common::V1_0::memcpy_s(sConfig_[sessionId].license, MAX_CERT_SIZE,
                               default_license, sizeof(default_license));
        break;
    }

EXIT:
    _hidl_cb(ret, sessionId);
    return Void();
}

/**
 * setConfig:
 *
 * Set session configuration. The configuration buffer (inConfig) contains
 * set of pairs having <tag, value> entries.
 * The configuration buffer is expected to be prepared using
 * SecureProcessorConfig common helper class.
 *
 * @param sessionId Session identifier.
 *
 * @param inConfig Input configuration buffer.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session configuration applied successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Session configuration failed to apply.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid configuration parameter passed.
 *
 */
Return<ErrorCode> SecureProcessorTEE::setConfig(
    uint32_t sessionId, const hidl_vec<uint8_t> &inConfig)
{
    std::lock_guard<std::mutex> lock(mLock_);
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    CHECK_COND(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);

    ret = _processConfig(sessionId, SET_SESSION_CONFIG, inConfig);

EXIT:
    return ret;
}

/**
 * getConfig:
 *
 * Get session configuration. The input configuration buffer (inConfig)
 * contains set of required tag entries. The output configuration buffer
 * (outConfig) is populated with set of pairs having <tag, value> entries
 * for requested configuration tags. Both input and output configuration
 * buffers are expected to be prepared using SecureProcessorConfig common
 * helper class.
 *
 * @param sessionId Session identifier.
 *
 * @param inConfig Input configuration buffer.
 *     It contains required tags to be queried.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session configuration get successful.
 *     SECURE_PROCESSOR_FAIL:
 *         Session configuration get failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid configuration parameter passed.
 *
 * @return outConfig Output configuration buffer.
 *     It contains set of <tag, value> pairs for requested tags.
 *
 */
Return<void> SecureProcessorTEE::getConfig(uint32_t sessionId,
                                           const hidl_vec<uint8_t> &inConfig,
                                           getConfig_cb _hidl_cb)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    SecureProcessorCfgBuf outConfig;

    std::lock_guard<std::mutex> lock(mLock_);
    if (!VALID_SESSION(sessionId)) {
        ALOGE("%s: unexpected sessionId[%d]\n", __func__, sessionId);
        _hidl_cb(ErrorCode::SECURE_PROCESSOR_BAD_VAL, outConfig);
        return Void();
    }

    ret = _processConfig(sessionId, GET_SESSION_CONFIG, inConfig, outConfig);

    _hidl_cb(ret, outConfig);
    return Void();
}

/**
 * startSession:
 *
 * Start requested session. This API allocates essential resources on secure
 * destination and makes them ready for secure data processing.
 * The mandatory session configs are expected to be set before calling
 * this API.
 *
 * @param sessionId Session identifier.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session started successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Session start failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid parameter passed.
 *
 */
Return<ErrorCode> SecureProcessorTEE::startSession(uint32_t sessionId)
{
    int status = 0;
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    seccam_cmd_req_t cmdRequest = {};
    seccam_cmd_rsp_t cmdResponse = {};

    std::lock_guard<std::mutex> lock(mLock_);
    CHECK_COND(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);
    CHECK_COND(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
               ErrorCode::SECURE_PROCESSOR_FAIL);
    CHECK_COND(sConfig_[sessionId].teeHandle != NULL,
               ErrorCode::SECURE_PROCESSOR_FAIL);

    // Send init command
    cmdRequest.cmd_id = SECCAM_CMD_INIT;
    status = QSEECom_send_cmd(sConfig_[sessionId].teeHandle, &cmdRequest,
                              sizeof(seccam_cmd_req_t), &cmdResponse,
                              sizeof(seccam_cmd_rsp_t));

    CHECK_COND(VALID_RESPONSE(status, cmdResponse.ret),
               ErrorCode::SECURE_PROCESSOR_FAIL);

    // Process any session specific configs as necessary
    // Set License should be first
    CHECK_COND(sConfig_[sessionId].license_size < MAX_CERT_SIZE,
               (ErrorCode)SECURE_PROCESSOR_LICENSE_ERROR);

    cmdRequest.cmd_id = SECCAM_CMD_SET_LICENSE;
    cmdRequest.cert_size = sConfig_[sessionId].license_size;
    common::V1_0::memcpy_s(cmdRequest.cert, MAX_CERT_SIZE,
                           sConfig_[sessionId].license, cmdRequest.cert_size);

    status = QSEECom_send_cmd(sConfig_[sessionId].teeHandle, &cmdRequest,
                              sizeof(seccam_cmd_req_t), &cmdResponse,
                              sizeof(seccam_cmd_rsp_t));
    CHECK_COND(VALID_RESPONSE(status, cmdResponse.ret),
               (ErrorCode)SECURE_PROCESSOR_LICENSE_ERROR);

    cmdRequest.cmd_id = SECCAM_CMD_SET_SENSORS;
    cmdRequest.num_sensors = sConfig_[sessionId].numSensor;
    status = QSEECom_send_cmd(sConfig_[sessionId].teeHandle, &cmdRequest,
                              sizeof(seccam_cmd_req_t), &cmdResponse,
                              sizeof(seccam_cmd_rsp_t));

    CHECK_COND(VALID_RESPONSE(status, cmdResponse.ret),
               ErrorCode::SECURE_PROCESSOR_FAIL);

    // Set State RUNNING
    sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_RUNNING;

    ret = ErrorCode::SECURE_PROCESSOR_OK;

EXIT:
    return ret;
}

/**
 * processImage:
 *
 * Process secure image data on selected secure destination.
 *
 * Additionally, the API allows set/get of the image specific configuration.
 * The input configuration buffer (inConfig) contains configuration data
 * associated with current image and the output configuration buffer
 * (outConfig) is expected to be populated with new configuration request to
 * be applied to current secure data capture (source) session based on
 * current image data processing on secure destination.
 *
 * The input/output configuration buffers are expected to be prepared
 * using SecureProcessorConfig common helper class.
 *
 * @param sessionId Session identifier.
 *
 * @param image Image handle for secure image data buffer.
 *
 * @param inConfig Input configuration buffer.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Image processing successful.
 *     SECURE_PROCESSOR_FAIL:
 *         Image processing failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid parameter passed.
 *     SECURE_PROCESSOR_NEED_CALIBRATE:
 *         Image processing successful. Additionally, client need to
 *         process outConfig for image source calibration.
 *
 * @return outConfig Output configuration buffer.
 *
 */
Return<void> SecureProcessorTEE::processImage(uint32_t sessionId,
                                              const hidl_handle &image,
                                              const hidl_vec<uint8_t> &inConfig,
                                              processImage_cb _hidl_cb)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_FAIL;
    SecureProcessorCfgBuf outConfig;

    std::lock_guard<std::mutex> lock(mLock_);
    if (!VALID_SESSION(sessionId)) {
        ALOGE("%s: unexpected sessionId[%d]\n", __func__, sessionId);
        ret = ErrorCode::SECURE_PROCESSOR_BAD_VAL;
        goto EXIT;
    }

    if (sConfig_[sessionId].state != SECURE_PROCESSOR_STATE_RUNNING) {
        ALOGE("%s: bad state: current = %d, expected = %d", __func__,
              sConfig_[sessionId].state, SECURE_PROCESSOR_STATE_RUNNING);
        goto EXIT;
    }

    // Reset old image config
    _resetImgCfg(sessionId);

    CHECK_ERR((ret = _processConfig(sessionId, SET_IMAGE_CONFIG, inConfig)));

    // Process image on requested secure destination w/ requested inCfg
    CHECK_ERR(ret = _processImage(sessionId, image));

// TBD: Prepare outCfg based on image data processing (if required)

EXIT:
    _hidl_cb(ret, outConfig);
    return Void();
}

/**
 * stopSession:
 *
 * Stop requested session. This API releases resources on secure destination
 * which were allocated during startSession call.
 * No secure data processing is allowed post this API call.
 * This API is expected to be called after completely stopping the
 * secure data capture requests on source.
 *
 * @param sessionId Session identifier.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session started successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Session start failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid parameter passed.
 *
 */
Return<ErrorCode> SecureProcessorTEE::stopSession(uint32_t sessionId)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_OK;
    int status = 0;
    seccam_cmd_req_t cmdRequest = {};
    seccam_cmd_rsp_t cmdResponse = {};

    std::lock_guard<std::mutex> lock(mLock_);
    CHECK_COND(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);
    CHECK_COND(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_RUNNING,
               ErrorCode::SECURE_PROCESSOR_FAIL);

    sConfig_[sessionId].state = SECURE_PROCESSOR_STATE_INIT;

    // Send shutdown command
    cmdRequest.cmd_id = SECCAM_CMD_SHUTDOWN;
    status = QSEECom_send_cmd(sConfig_[sessionId].teeHandle, &cmdRequest,
                              sizeof(seccam_cmd_req_t), &cmdResponse,
                              sizeof(seccam_cmd_rsp_t));
    CHECK_COND(VALID_RESPONSE(status, cmdResponse.ret),
               ErrorCode::SECURE_PROCESSOR_FAIL);

EXIT:
    return ret;
}

/**
 * deleteSession:
 *
 * Delete a previously allocated session.
 *
 * @param sessionId Session identifier.
 *
 * @return status Return status of this operation:
 *     SECURE_PROCESSOR_OK:
 *         Session started successfully.
 *     SECURE_PROCESSOR_FAIL:
 *         Session start failed.
 *     SECURE_PROCESSOR_BAD_VAL:
 *         Invalid parameter passed.
 *
 */
Return<ErrorCode> SecureProcessorTEE::deleteSession(uint32_t sessionId)
{
    ErrorCode ret = ErrorCode::SECURE_PROCESSOR_OK;

    std::lock_guard<std::mutex> lock(mLock_);
    CHECK_COND(VALID_SESSION(sessionId), ErrorCode::SECURE_PROCESSOR_BAD_VAL);
    CHECK_COND(sConfig_[sessionId].state == SECURE_PROCESSOR_STATE_INIT,
               ErrorCode::SECURE_PROCESSOR_FAIL);

    // unload TEEApp
    if (sConfig_[sessionId].teeHandle != NULL) {
        ret = _unloadTEEApp(&(sConfig_[sessionId].teeHandle));
    }

    std::memset(&sConfig_[sessionId], 0, sizeof(sConfig_[0]));

EXIT:
    return ret;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace device
}  // namespace secureprocessor
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
