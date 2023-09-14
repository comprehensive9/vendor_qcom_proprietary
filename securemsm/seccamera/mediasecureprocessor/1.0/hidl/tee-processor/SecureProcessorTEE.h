/**
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSORTEE_H
#define VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSORTEE_H

#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <vendor/qti/hardware/secureprocessor/config/1.0/types.h>
#include <vendor/qti/hardware/secureprocessor/device/1.0/ISecureProcessor.h>
#include <mutex>
#include "QSEEComAPI.h"
#include "SecureProcessorConfig.h"
#include "SecureProcessorUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SECCAM_MAX_CAMERAS (2)
#define SECCAM_MAX_NUM_OF_PLANES (4)
#define MAX_CERT_SIZE (2048)
/* Note:
 * Following definitions are used by various secure
 * camera applications which process secure frames
 * in various secure execution environments like -
 * QTEE, guest VM etc.
 *
 * Do not modify the following defines.
 */
typedef enum {
    SECCAM_CMD_INIT = 2001,
    SECCAM_CMD_SET_LICENSE = 2002,
    SECCAM_CMD_SET_SENSORS = 2003,
    SECCAM_CMD_PROCESS_FRAME = 2004,
    SECCAM_CMD_SHUTDOWN = 2005,
    SECCAM_CMD_RESET_CAMERA = 2006,
} seccam_cmd_id_t;

#pragma pack(push, seccam_if, 1)

typedef struct seccam_plane_info_t {
    uint32_t offset;
    uint32_t row_stride;
    uint32_t pixel_stride;
} seccam_plane_info_t;

typedef struct sec_app_if_plane_req_data_t {
    uint32_t num_of_in_planes;
    seccam_plane_info_t in_planes[SECCAM_MAX_NUM_OF_PLANES];
    uint32_t num_of_out_planes;
    seccam_plane_info_t out_planes[SECCAM_MAX_NUM_OF_PLANES];
} seccam_plane_req_data_t;

// cam_id is defined as int32_t to be consistent with HIDL
// frame_number is defined as int64_t to be consistent with HIDL
typedef struct {
    int32_t cam_id;
    int64_t frame_number;
    int64_t time_stamp;
} seccam_frame_info_t;

typedef struct {
    seccam_cmd_id_t cmd_id;
    uint32_t num_sensors;  // number of protected sensors
    uint64_t buf;
    uint32_t buf_size;
    seccam_frame_info_t frame_info;  // Frame info
    uint32_t cert_size;              // cert size
    uint8_t cert[MAX_CERT_SIZE];     // cert payload offset

    seccam_plane_req_data_t plane_data[SECCAM_MAX_CAMERAS];
} seccam_cmd_req_t;

typedef struct {
    uint32_t ret;
} seccam_cmd_rsp_t;

#pragma pack(pop, seccam_if)

#ifdef __cplusplus
}
#endif

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
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_handle;
using ::android::sp;
using vendor::qti::hardware::secureprocessor::common::V1_0::ConfigEntry;
using vendor::qti::hardware::secureprocessor::common::V1_0::
    SecureProcessorConfig;
using vendor::qti::hardware::secureprocessor::common::V1_0::ErrorCode;

typedef enum SecureProcessorState {
    SECURE_PROCESSOR_STATE_NONE = 0,
    SECURE_PROCESSOR_STATE_INIT,
    SECURE_PROCESSOR_STATE_RUNNING,
    SECURE_PROCESSOR_STATE_RESET,
} SecureProcessorState_t;

struct ImageBuffer {
    int32_t size;
    int32_t width;
    int32_t height;
    int32_t stride;
    int32_t format;
};

struct ImageConfig {
    int32_t cameraId;
    int64_t frameNum;
    int64_t timeStamp;
    ImageBuffer buffer;
};

struct SessionConfig {
    bool active;
    SecureProcessorState_t state;
    int32_t numSensor;
    uint32_t license_size;
    uint8_t license[MAX_CERT_SIZE];
    std::string taName;
    ImageConfig imgCfg;
    struct QSEECom_handle *teeHandle;
};

#define MAX_SESSION 10
#define SHARED_BUFFER_SIZE 8192

static const char *TYPE_QTI_TEE = "qti-tee";

struct SecureProcessorTEE : public ISecureProcessor {
    SecureProcessorTEE();
    virtual ~SecureProcessorTEE();

    // Methods from
    // ::vendor::qti::hardware::SecureProcessor::V1_0::ISecureProcessor
    // follow.
    Return<void> createSession(createSession_cb _hidl_cb) override;
    Return<common::V1_0::ErrorCode> setConfig(
        uint32_t sessionId, const hidl_vec<uint8_t> &inConfig) override;
    Return<void> getConfig(uint32_t sessionId,
                           const hidl_vec<uint8_t> &inConfig,
                           getConfig_cb _hidl_cb) override;
    Return<common::V1_0::ErrorCode> startSession(uint32_t sessionId) override;
    Return<void> processImage(uint32_t sessionId, const hidl_handle &image,
                              const hidl_vec<uint8_t> &inConfig,
                              processImage_cb _hidl_cb) override;
    Return<common::V1_0::ErrorCode> stopSession(uint32_t sessionId) override;
    Return<common::V1_0::ErrorCode> deleteSession(uint32_t sessionId) override;
    Return<common::V1_0::ErrorCode> resetCamera(uint32_t sessionId);

    // Methods from ::android::hidl::base::V1_0::IBase follow.

    // Private variables specific to this specific implementation of
    // ISecureProcessor
   private:
    SessionConfig sConfig_[MAX_SESSION];
    mutable std::mutex mLock_;

    ErrorCode _loadTEEApp(char *appname, struct QSEECom_handle **teeHandle);
    ErrorCode _unloadTEEApp(struct QSEECom_handle **teeHandle);
    void _resetImgCfg(uint32_t sessionId);
    ErrorCode _HandleSessionCfg(uint32_t sessionId, ConfigEntry *entry);
    ErrorCode _createSessionCfg(uint32_t sessionId,
                                SecureProcessorConfig *cfgWrapper,
                                ConfigEntry *entry);
    ErrorCode _HandleImageCfg(uint32_t sessionId, ConfigEntry *entry);
    ErrorCode _processImage(uint32_t sessionId, const hidl_handle &image);
    ErrorCode _processConfig(uint32_t sessionId, uint32_t operation,
                             const hidl_vec<uint8_t> &inConfig);
    ErrorCode _processConfig(uint32_t sessionId, uint32_t operation,
                             const hidl_vec<uint8_t> &inConfig,
                             hidl_vec<uint8_t> &outConfig);
    ErrorCode _updatePlanes(uint32_t sessionId, seccam_plane_info_t *planeData,
                            uint32_t *num_of_in_planes);
    ErrorCode _resetCamera(uint32_t sessionId);
};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" ISecureProcessor* HIDL_FETCH_ISecureProcessor(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace device
}  // namespace secureprocessor
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSORTEE_H
