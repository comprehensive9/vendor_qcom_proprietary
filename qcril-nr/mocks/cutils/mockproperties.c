/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 */

#include "mockproperties.h"

int property_get(const char* key, char* value, const char* default_value)
{
    return 0;
}

int8_t property_get_bool(const char* key, int8_t default_value)
{
    return 0;
}

int64_t property_get_int64(const char* key, int64_t default_value)
{
    return 0;
}

int32_t property_get_int32(const char* key, int32_t default_value)
{
    return 0;
}

int property_set(const char* key, const char* value)
{
    return 0;
}

int property_list(void (*propfn)(const char *key, const char *value, void *cookie), void *cookie)
{
  return 0;
}
