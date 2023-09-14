/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "__config_key.h"
#include "framework/IxErrno.h"

#ifdef __cplusplus
#include <string>
errno_enum_type qcril_config_set(property_id_type id, const std::string& config_value);
errno_enum_type qcril_config_get(property_id_type id, std::string& config_value);
errno_enum_type qcril_config_get(property_id_type id, int& config_value);
errno_enum_type qcril_config_get(property_id_type id, bool& config_value);
errno_enum_type qcril_config_set(property_id_type id, int config_value);
errno_enum_type qcril_config_set(property_id_type id, bool config_value);
errno_enum_type qcril_config_clear_all_user_settings();
errno_enum_type qcril_db_properties_table_user_set(const char* property_name, const char* value);
#endif

#ifdef __cplusplus
extern "C" {
#endif
errno_enum_type qcril_config_set(property_id_type id, const char* config_value);
/* caller needs to call free() to dispose the memory allocated in *p_config_value
 * in case of E_SUCCESS as returned value */
errno_enum_type qcril_config_get(property_id_type id, char** p_config_value);
#ifdef __cplusplus
}
#endif
