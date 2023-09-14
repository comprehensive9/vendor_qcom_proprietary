/**
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*! Private */
#ifndef AUDCALPARAM_API_PRIV_H
#define AUDCALPARAM_API_PRIV_H
#include "../inc/audcalparam_api.h"
#include <linux/msm_audio_calibration.h>
#ifndef __ANDROID__
    #include <json-c/json.h>
#else
    #include <json.h>
#endif
#include "acdb-loader.h"

#ifdef _ANDROID_
#include <cutils/properties.h>
/* definitions for Android logging */
#include <log/log.h>
#include "common_log.h"
#else
#define ALOGI(...)     fprintf(stdout,__VA_ARGS__);\
                       fprintf(stdout, "\n")
#define ALOGE(...)     fprintf(stderr,__VA_ARGS__);\
                       fprintf(stderr, "\n")
#define ALOGV(...)     fprintf(stderr,__VA_ARGS__);\
                       fprintf(stderr, "\n")
#define ALOGD(...)     fprintf(stderr,__VA_ARGS__);\
                       fprintf(stderr, "\n")
#endif /* _ANDROID_ */

typedef struct _audcalparam_usecase_t{
    uint32_t acdb_dev_id;
    uint32_t app_type;
    uint32_t topo_id;
    uint32_t type;
    char * name;
    struct _audcalparam_usecase_t* next;
}audcalparam_usecase_t;

typedef struct {
    int32_t use_case_num;
    audcalparam_usecase_t* usecase_list;
}audcalparam_usecase_list_t;

typedef struct acdb_audio_cal_cfg {
    uint32_t persist;
    uint32_t snd_dev_id;
    uint32_t dev_id;
    int32_t acdb_dev_id;
    uint32_t app_type;
    uint32_t topo_id;
    uint32_t sampling_rate;
    uint32_t cal_type;
    uint32_t module_id;
    uint16_t instance_id;
    uint16_t reserved;
    uint32_t param_id;
} acdb_audio_cal_cfg_t;

typedef  acdb_audio_cal_cfg_t audcalparam_cmd_tunnel_cfg_t;

typedef struct _audcalparam_command_instance_t{
    char * inst_name;
    struct _audcalparam_command_instance_t * nxt_command_instance;
    char* use_case_name;
    uint32_t topo_id;
    uint32_t cal_type;
    uint32_t module_id;
    uint16_t inst_id;
    uint16_t reserved;
    uint32_t param_id;
    uint8_t *parambuf;
    uint32_t parambuflen;
}audcalparam_command_instance_t;

typedef struct {
    char * type_name;
    uint8_t inst_num;
    audcalparam_command_instance_t * command_instance_list;
}audcalparam_command_t;

typedef struct {
    audcalparam_command_t *cmd_bmt;
    audcalparam_command_t *cmd_delay;
    audcalparam_command_t *cmd_fnb;
    audcalparam_command_t *cmd_mute;
    audcalparam_command_t *cmd_volume_idx;
    audcalparam_command_t *cmd_gain;
    audcalparam_command_t *cmd_avc;
    audcalparam_command_t *cmd_sumx;
    audcalparam_command_t *cmd_synth;
    audcalparam_command_t *cmd_module_param;
}audcalparam_command_set_t;

typedef struct {
    audcalparam_usecase_list_t *use_cases;
    audcalparam_command_set_t *cmd_set;
    void *con_hdl;// handle to the calibration service access: either handle to the linked so-library or Unix socket to send message
}audcalparam_api_session_t;

#endif
