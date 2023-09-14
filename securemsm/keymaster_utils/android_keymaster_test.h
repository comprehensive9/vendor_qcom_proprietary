/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cstdint>
#include <cstdlib>
#include "KeymasterHalDevice.h"
#ifdef OFFTARGET
extern "C" {
#include "QSEEComAPI.h"
#include "km_main.h"
#include "qtee_init.h"
}

static inline keymaster_error_t Configure(KeymasterHalDevice* device, uint32_t osVersion,
                                          uint32_t osPatchLevel, uint32_t vendorPatchLevel) {
    keymaster_key_param_set_t param;
    keymaster_key_param_t configParam[3];

    configParam[0].tag = KM_TAG_OS_VERSION;
    configParam[0].integer = osVersion;

    configParam[1].tag = KM_TAG_OS_PATCHLEVEL;
    configParam[1].integer = osPatchLevel;

    configParam[2].tag = keymaster::KM_TAG_VENDOR_PATCHLEVEL;
    configParam[2].integer = vendorPatchLevel;

    param.params = &configParam[0];
    param.length = 3;
    return device->configure(&param);
}

static inline keymaster_error_t SetRot(KeymasterHalDevice* device) {
    uint8_t rot[KM_DIGEST_SHA_2_256_SIZE / 8];
    uint8_t pubKey[KM_DIGEST_SHA_2_256_SIZE / 8];
    uint8_t vbh[KM_DIGEST_SHA_2_256_SIZE / 8];
    memset(&rot, 0, KM_DIGEST_SHA_2_256_SIZE / 8);
    memset(&vbh, 0, KM_DIGEST_SHA_2_256_SIZE / 8);
    memset(&pubKey, 0, KM_DIGEST_SHA_2_256_SIZE / 8);
    return device->SetRot(rot, vbh, true, pubKey, ORANGE, 0, 0);
}

static inline void SetUpOffTarget() {
    int error = qtee_emu_init(CFG_FILE);
    if (error) std::cerr << "qtee_emu_init failed" << std::endl;
}

static inline void SetUpOffTargetConfig(KeymasterHalDevice* device, uint32_t osVersion,
                                        uint32_t osPatchLevel, uint32_t vendorPatchLevel) {
    keymaster_error_t error = Configure(device, osVersion, osPatchLevel, vendorPatchLevel);
    if (error) std::cerr << "Configure Failed" << std::endl;

    error = SetRot(device);
    if (error) std::cerr << "SetRot Failed" << std::endl;
}

#else

static inline void SetUpOffTarget() {
    return;
}

static inline void SetUpOffTargetConfig(KeymasterHalDevice* device, uint32_t osVersion,
                                        uint32_t osPatchLevel, uint32_t vendorPatchLevel) {
    (void)device;
    (void)osVersion;
    (void)osPatchLevel;
    (void)vendorPatchLevel;
    return;
}

#endif
