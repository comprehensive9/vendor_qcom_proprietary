/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef KEYMASTER_3_0_KEYMASTERHALDEVICE_H_
#define KEYMASTER_3_0_KEYMASTERHALDEVICE_H_

#include <cstdlib>
#include <map>
#include <vector>

#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include <hardware/keymaster1.h>
#include <hardware/keymaster2.h>
#include <hardware/keymaster_defs.h>

#include <KeymasterBuffer.h>
#include <KeymasterCryptoDevice.h>
#include <KeymasterUtils.h>
#include <keymaster_tags.h>

#define MSEC_TO_SEC(v) ((v) / 1000)

#define USE_NEW_APIS (1 << 0)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))
#define PROPERTY_GET_SLEEP_INTERVAL_USEC 100000
#define KM_DIGEST_SHA_2_256_SIZE (256)
#define MAX_PURPOSE_COUNT (5)
#define MAX_BLOCK_MODE_COUNT (5)
#define MAX_DIGEST_COUNT (8)
#define MAX_PADDING_COUNT (8)
#define MAX_USER_SECURE_ID_COUNT (5)

using namespace keymasterutils;

typedef enum _km_boot_state_color_t {
    GREEN,
    ORANGE,
    YELLOW,
    RED,
} km_boot_state_color_t;

typedef struct {
    uint32_t purpose[MAX_PURPOSE_COUNT];
    uint32_t purposeCount;
    bool hasAlgorithm;
    uint32_t algorithm;
    bool hasKeySize;
    uint32_t keySize;
    uint32_t blockMode[MAX_BLOCK_MODE_COUNT];
    uint32_t blockModeCount;
    uint32_t digest[MAX_DIGEST_COUNT];
    uint32_t digestCount;
    uint32_t padding[MAX_PADDING_COUNT];
    uint32_t paddingCount;
    bool callerNonce;
    bool hasMinMacLength;
    uint32_t minMacLength;
    bool hasEcCurve;
    uint32_t ecCurve;
    bool hasRsaPublicExponent;
    uint64_t rsaPublicExponent;
    bool includeUniqueId;
    bool hasBlobUsageRequirements;
    uint32_t blobUsageRequirements;
    bool hasMinSecondsBetweenOps;
    uint32_t minSecondsBetweenOps;
    bool hasMaxUsesPerBoot;
    uint32_t maxUsesPerBoot;
    bool allUsers;
    uint64_t userSecureId[MAX_USER_SECURE_ID_COUNT];
    uint32_t userSecureIdCount;
    bool noAuthRequired;
    bool hasUserAuthType;
    uint32_t userAuthType;
    bool hasAuthTimeout;
    uint32_t authTimeout;
    bool allApplications;
    bool exportable;
    bool hasOrigin;
    uint32_t origin;
    bool rollbackResistant;
    bool hasOsVersion;
    uint32_t osVersion;
    bool hasOsPatchLevel;
    uint32_t osPatchLevel;
    bool hasMacLength;
    uint32_t macLength;
} __attribute__((packed)) qtiTeeParamsAuthorizationList;

typedef struct {
    keymaster_security_level_t securityLevel;
    bool trustedUserPresence;
    bool trustedUserConfirmation;
    uint32_t vendorPatchLevel;
    bool hasVendorPatchLevel;
    uint32_t bootPatchLevel;
    bool hasBootPatchLevel;
    // Dummy not used
    bool hasAuthTimeout;
    uint64_t authTimeout;
} __attribute__((packed)) qtiTeeParams2AuthorizationList;

typedef struct {
    bool hasActiveDateTime;
    uint64_t activeDateTime;
    bool hasOriginationExpireDateTime;
    uint64_t originationExpireDateTime;
    bool hasUsageExpireDateTime;
    uint64_t usageExpireDateTime;
    bool hasUserId;
    uint32_t userId;
    bool allowWhileOnBody;
    bool hasCreationDateTime;
    uint64_t creationDateTime;
} __attribute__((packed)) qtiSwParamsAuthorizationList;

typedef struct {
    bool unlockedDeviceRequired;
} __attribute__((packed)) qtiSwParams2AuthorizationList;

namespace keymasterdevice {

typedef struct _TagToString {
    keymaster_tag_t tag;
    char tagstring[100];
} TagToString_t;

struct CertificateChain_Delete {
    void operator()(keymaster_cert_chain_t* p) {
        if (!p) return;
        for (size_t i = 0; i < p->entry_count; i++) {
            if (p->entries[i].data) free((void*)p->entries[i].data);
        }
        delete[] p->entries;
        delete p;
    }
};

struct KeymasterParam_Delete {
    void operator()(keymaster_key_param_t* p) {
        if (!p) return;
        keymaster_free_param_values(p, 1);
    }
};

struct KeyCharacteristics_Delete {
    void operator()(keymaster_key_characteristics_t* p) {
        if (!p) return;
        delete p;
    }
};

struct MallocDeleter {
    void operator()(uint8_t* p) { free(p); }
};

class KeymasterHalDevice {
  public:
    KeymasterHalDevice(keymaster_security_level_t security_level)
        : utils(new KeymasterUtils(security_level)), cryptoDevice(new KeymasterCryptoDevice) {
        level = security_level;
    }
    virtual ~KeymasterHalDevice() {}

    uint32_t get_keymaster_version();
    uint32_t get_keymaster_api_minor_version();
    keymaster_error_t get_date_support(bool* supported);
    keymaster_error_t configure(const keymaster_key_param_set_t* params);
    keymaster_error_t generate_key(const keymaster_key_param_set_t* params,
                                   keymaster_key_blob_t* key_blob,
                                   keymaster_key_characteristics_t* characteristics);
    keymaster_error_t get_key_characteristics(const keymaster_key_blob_t* key_blob,
                                              const keymaster_blob_t* client_id,
                                              const keymaster_blob_t* app_data,
                                              keymaster_key_characteristics_t* characteristics);
    keymaster_error_t import_key(const keymaster_key_param_set_t* params,
                                 keymaster_key_format_t key_format,
                                 const keymaster_blob_t* key_data, keymaster_key_blob_t* key_blob,
                                 keymaster_key_characteristics_t* characteristics);
    keymaster_error_t export_key(keymaster_key_format_t export_format,
                                 const keymaster_key_blob_t* key_to_export,
                                 const keymaster_blob_t* client_id,
                                 const keymaster_blob_t* app_data, keymaster_blob_t* export_data);
    keymaster_error_t attest_key(const keymaster_key_blob_t* key_to_attest,
                                 const keymaster_key_param_set_t* attest_params,
                                 keymaster_cert_chain_t* cert_chain);
    keymaster_error_t upgrade_key(const keymaster_key_blob_t* key_to_upgrade,
                                  const keymaster_key_param_set_t* upgrade_params,
                                  keymaster_key_blob_t* upgraded_key);
    keymaster_error_t begin_operation(keymaster_purpose_t purpose, const keymaster_key_blob_t* key,
                                      const keymaster_key_param_set_t* in_params,
                                      keymaster_key_param_set_t* out_params,
                                      keymaster_operation_handle_t* operation_handle);
    keymaster_error_t update_operation(keymaster_operation_handle_t operation_handle,
                                       const keymaster_key_param_set_t* in_params,
                                       const keymaster_blob_t* input, size_t* input_consumed,
                                       keymaster_key_param_set_t* out_params,
                                       keymaster_blob_t* output);
    keymaster_error_t finish_operation(keymaster_operation_handle_t operation_handle,
                                       const keymaster_key_param_set_t* in_params,
                                       const keymaster_blob_t* input,
                                       const keymaster_blob_t* signature,
                                       keymaster_key_param_set_t* out_params,
                                       keymaster_blob_t* output);
    keymaster_error_t abort_operation(keymaster_operation_handle_t operation_handle);
    keymaster_error_t delete_key(const keymaster_key_blob_t* key);
    keymaster_error_t delete_all_keys(void);
    keymaster_error_t destroy_device_ids(void);
    keymaster_error_t import_wrapped_key(const keymaster_key_param_set_t* in_params,
                                         const uint64_t passwordSid, const uint64_t biometricSid,
                                         const std::vector<uint8_t> wrappedKeyData,
                                         const std::vector<uint8_t> wrappingData,
                                         const std::vector<uint8_t> maskingKey,
                                         keymaster_key_blob_t* keyBlob,
                                         keymaster_key_characteristics_t* characteristics);
    keymaster_error_t get_hmac_sharing_parameters(hmac_sharing_params_t* params);
    keymaster_error_t compute_shared_hmac(std::vector<hmac_sharing_params_t> params,
                                          std::vector<uint8_t>& sharingCheck);
    keymaster_error_t verifyAuthorization(uint64_t challenge,
                                          verification_token_t* verification_token);
    keymaster_error_t SetRot(uint8_t rot[KM_DIGEST_SHA_2_256_SIZE / 8],
                             uint8_t vbh[KM_DIGEST_SHA_2_256_SIZE / 8], bool isUnlocked,
                             uint8_t pubKey[KM_DIGEST_SHA_2_256_SIZE / 8],
                             km_boot_state_color_t vbColor, uint32_t osVersion,
                             uint32_t osPatchLevel);
    keymaster_error_t device_ids_provision(const keymaster_key_param_set_t* params);
    keymaster_error_t set_early_boot_ended(void);
    keymaster_error_t set_device_locked(bool passwordOnly);

  protected:
    keymaster_security_level_t level;
    keymaster_error_t params_serialize(std::unique_ptr<KeymasterBuffer>& buffer,
                                       keymaster_key_param_t* a, size_t params_count);
    std::unique_ptr<KeymasterUtils> utils;

  private:
    char* TagToString(keymaster_tag_t tag);
    void PrintParams(keymaster_key_param_t* params, size_t params_count);

    // keymaster_error_t getCharacteristics(qti_keyBlob_t *keyblob, keymaster_key_characteristics_t*
    // characteristics);
    keymaster_error_t generate_key_common(const keymaster_key_param_set_t* params,
                                          keymaster_key_blob_t* key_blob);

    keymaster_error_t legacy_get_date_support(bool* supported);
    keymaster_error_t legacy_configure(const keymaster_key_param_set_t* params);
    keymaster_error_t legacy_generate_key(const keymaster_key_param_set_t* params,
                                          keymaster_key_blob_t* key_blob,
                                          keymaster_key_characteristics_t* characteristics);
    keymaster_error_t legacy_get_key_characteristics(
            const keymaster_key_blob_t* key_blob, const keymaster_blob_t* client_id,
            const keymaster_blob_t* app_data, keymaster_key_characteristics_t* characteristics);
    keymaster_error_t legacy_import_key(const keymaster_key_param_set_t* params,
                                        keymaster_key_format_t key_format,
                                        const keymaster_blob_t* key_data,
                                        keymaster_key_blob_t* key_blob,
                                        keymaster_key_characteristics_t* characteristics);
    keymaster_error_t legacy_export_key(keymaster_key_format_t export_format,
                                        const keymaster_key_blob_t* key_to_export,
                                        const keymaster_blob_t* client_id,
                                        const keymaster_blob_t* app_data,
                                        keymaster_blob_t* export_data);
    keymaster_error_t legacy_attest_key(const keymaster_key_blob_t* key_to_attest,
                                        const keymaster_key_param_set_t* attest_params,
                                        keymaster_cert_chain_t* cert_chain);
    keymaster_error_t legacy_upgrade_key(const keymaster_key_blob_t* key_to_upgrade,
                                         const keymaster_key_param_set_t* upgrade_params,
                                         keymaster_key_blob_t* upgraded_key);
    keymaster_error_t legacy_begin_operation(keymaster_purpose_t purpose,
                                             const keymaster_key_blob_t* key,
                                             const keymaster_key_param_set_t* in_params,
                                             keymaster_key_param_set_t* out_params,
                                             keymaster_operation_handle_t* operation_handle);
    keymaster_error_t legacy_update_operation(keymaster_operation_handle_t operation_handle,
                                              const keymaster_key_param_set_t* in_params,
                                              const keymaster_blob_t* input, size_t* input_consumed,
                                              keymaster_key_param_set_t* out_params,
                                              keymaster_blob_t* output);
    keymaster_error_t legacy_finish_operation(keymaster_operation_handle_t operation_handle,
                                              const keymaster_key_param_set_t* in_params,
                                              const keymaster_blob_t* input,
                                              const keymaster_blob_t* signature,
                                              keymaster_key_param_set_t* out_params,
                                              keymaster_blob_t* output);
    keymaster_error_t legacy_abort_operation(keymaster_operation_handle_t operation_handle);
    keymaster_error_t legacy_delete_key(const keymaster_key_blob_t* key);
    keymaster_error_t legacy_delete_all_keys(void);
    keymaster_error_t legacy_destroy_device_ids(void);
    keymaster_error_t legacy_import_wrapped_key(
            const keymaster_key_param_set_t* in_params, const uint64_t passwordSid,
            const uint64_t biometricSid, const std::vector<uint8_t> wrappedKeyData,
            const std::vector<uint8_t> wrappingData, const std::vector<uint8_t> maskingKey,
            keymaster_key_blob_t* keyBlob, keymaster_key_characteristics_t* characteristics);
    keymaster_error_t legacy_get_hmac_sharing_parameters(hmac_sharing_params_t* params);
    keymaster_error_t legacy_compute_shared_hmac(std::vector<hmac_sharing_params_t> params,
                                                 std::vector<uint8_t>& sharingCheck);
    keymaster_error_t LegacyVerifyAuthorization(uint64_t challenge,
                                                verification_token_t* verification_token);
    keymaster_error_t LegacySetRot(uint8_t rot[KM_DIGEST_SHA_2_256_SIZE / 8],
                                   uint8_t vbh[KM_DIGEST_SHA_2_256_SIZE / 8], bool isUnlocked,
                                   uint8_t pubKey[KM_DIGEST_SHA_2_256_SIZE / 8],
                                   km_boot_state_color_t vbColor, uint32_t osVersion,
                                   uint32_t osPatchLevel);
    keymaster_error_t legacy_device_ids_provision(const keymaster_key_param_set_t* params);
    keymaster_error_t parseTeeParams(qtiTeeParamsAuthorizationList* teeParams,
                                     qtiTeeParams2AuthorizationList* teeParams2,
                                     size_t teeParamsCount,
                                     keymaster_key_characteristics_t* characteristics);
    keymaster_error_t parseSwParams(qtiSwParamsAuthorizationList* swParams,
                                    qtiSwParams2AuthorizationList* swParams2, size_t swParamsCount,
                                    keymaster_key_characteristics_t* characteristics);
    keymaster_error_t getCharacteristicsLegacy(const uint8_t* keyblob,
                                               keymaster_key_characteristics_t* characteristics);

    std::unique_ptr<KeymasterCryptoDevice> cryptoDevice;
    std::map<keymaster_operation_handle_t, keymaster_purpose_t> kmmap;
};

}  // namespace keymasterdevice

#endif /* KEYMASTER_3_0_KEYMASTERHALDEVICE_H_ */
