/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */

/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HARDWARE_KEYMASTER_UTILS_H
#define HARDWARE_KEYMASTER_UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <keymaster_defs.h>
#include <keymaster.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/* Convenience functions for manipulating keymaster tag types */

static inline keymaster_tag_type_t keymaster_tag_get_type(keymaster_tag_t tag) {
    return (keymaster_tag_type_t)(tag & (0xF << 28));
}

static inline uint32_t keymaster_tag_mask_type(keymaster_tag_t tag) {
    return tag & 0x0FFFFFFF;
}

static inline bool keymaster_tag_type_repeatable(keymaster_tag_type_t type) {
    switch (type) {
    case KM_UINT_REP:
    case KM_ENUM_REP:
        return true;
    default:
        return false;
    }
}

static inline bool keymaster_tag_repeatable(keymaster_tag_t tag) {
    return keymaster_tag_type_repeatable(keymaster_tag_get_type(tag));
}

/* Convenience functions for manipulating keymaster_key_param_t structs */

static inline keymaster_key_param_t keymaster_param_enum(keymaster_tag_t tag, uint32_t value) {
    keymaster_key_param_t param;
    memset(&param, 0, sizeof(param));
    param.tag = tag;
    param.enumerated = value;
    return param;
}

static inline keymaster_key_param_t keymaster_param_int(keymaster_tag_t tag, uint32_t value) {
    keymaster_key_param_t param;
    memset(&param, 0, sizeof(param));
    param.tag = tag;
    param.integer = value;
    return param;
}

static inline keymaster_key_param_t keymaster_param_long(keymaster_tag_t tag, uint64_t value) {
    // assert(keymaster_tag_get_type(tag) == KM_LONG);
    keymaster_key_param_t param;
    memset(&param, 0, sizeof(param));
    param.tag = tag;
    param.long_integer = value;
    return param;
}

static inline keymaster_key_param_t keymaster_param_blob(keymaster_tag_t tag, const uint8_t* bytes,
                                                  size_t bytes_len) {
    keymaster_key_param_t param;
    memset(&param, 0, sizeof(param));
    param.tag = tag;
    param.blob.data = (uint8_t*)bytes;
    param.blob.data_length = bytes_len;
    return param;
}

static inline keymaster_key_param_t keymaster_param_bool(keymaster_tag_t tag) {
    keymaster_key_param_t param;
    memset(&param, 0, sizeof(param));
    param.tag = tag;
    param.boolean = true;
    return param;
}

static inline keymaster_key_param_t keymaster_param_date(keymaster_tag_t tag, uint64_t value) {
    keymaster_key_param_t param;
    memset(&param, 0, sizeof(param));
    param.tag = tag;
    param.date_time = value;
    return param;
}


static inline void keymaster_free_param_values(keymaster_key_param_t* param, size_t param_count) {
    if (!param)
        return;
    while (param_count > 0) {
        param_count--;
        switch (keymaster_tag_get_type(param->tag)) {
        case KM_BIGNUM:
        case KM_BYTES:
            km_free((void*)param->blob.data);
            param->blob.data = NULL;
            break;
        default:
            // NOP
            break;
        }
        ++param;
    }
}

static inline void keymaster_free_param_set(keymaster_key_param_set_t* set) {
    if (set) {
        keymaster_free_param_values(set->params, set->length);
        km_free(set->params);
        set->params = NULL;
        set->length = 0;
    }
}

static inline void keymaster_free_blob(keymaster_blob_t* blob) {
    if (blob) {
        km_free((void*)blob->data);
        blob->data = NULL;
        blob->data_length = 0;
    }
}

static inline void keymaster_free_key_blob(keymaster_key_blob_t* key_blob) {
    if (key_blob) {
        km_free((void*)key_blob->key_material);
        key_blob->key_material = NULL;
        key_blob->key_material_size = 0;

    }
}

static inline void keymaster_free_characteristics(keymaster_key_characteristics_t* characteristics) {
    if (characteristics) {
        keymaster_free_param_set(&characteristics->hw_enforced);
        keymaster_free_param_set(&characteristics->sw_enforced);
    }
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // HARDWARE_KEYMASTER_UTILS_H
