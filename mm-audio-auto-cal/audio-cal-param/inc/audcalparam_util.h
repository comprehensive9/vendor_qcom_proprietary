/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _AUDCALPARAM_UTIL_H_
#define _AUDCALPARAM_UTIL_H_
#include "audcalparam_api.h"
//#include "audcalparam_api_priv.h"

typedef struct{
    int32_t  acdb_dev_id;
    uint32_t app_type;
    uint32_t topo_id;
    uint32_t cal_type;
    uint32_t module_id;
    uint16_t instance_id;
    uint16_t reserved;
    uint32_t param_id;
}audcalparam_util_parser_caldata_t;



audcalparam_err_t audcalparam_util_parser_init(audcalparam_session_t **h, char* cfg_file_name);

audcalparam_err_t audcalparam_util_parser_deinit(audcalparam_session_t *h);

audcalparam_err_t  audcalparam_util_parser_get_caldata_for_cmd_inst(audcalparam_session_t *h, char* cmd_name, int idx, audcalparam_util_parser_caldata_t *caldata);


#endif
