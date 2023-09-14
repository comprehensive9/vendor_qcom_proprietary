/*
 * Copyright (c) 2020-21 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __LIBSOC_HELPER_H__
#define __LIBSOC_HELPER_H__

// enums for msm cpu
typedef enum msm_cpu {
    MSM_CPU_SDM845    = 321,
    MSM_CPU_SDM670    = 336,
    MSM_CPU_SM8150    = 339,
    MSM_CPU_KONA      = 356,
    MSM_CPU_SDM710    = 360,
    MSM_CPU_LITO      = 400,
    MSM_CPU_LAHAINA   = 415,
    APQ_CPU_LAHAINA   = 439,
    MSM_CPU_SHIMA     = 450,
    MSM_CPU_SM8325    = 501,
    APQ_CPU_SM8325P   = 502,
    MSM_CPU_YUPIK     = 475,
    APQ_CPU_YUPIK     = 499,
    MSM_CPU_YUPIK_IOT = 497,
    APQ_CPU_YUPIKP_IOT = 498,
    MSM_CPU_YUPIK_LTE  = 515,
    MSM_CPU_UNKNOWN   = -1,
} msm_cpu_t;

// struct to hold soc_info
typedef struct soc_info_v0_1 {
    msm_cpu_t msm_cpu;
} soc_info_v0_1_t;

// returns soc_id as an enum; MSM_CPU_UNKNOWN if it fails or if it's not mapped
void get_soc_info(soc_info_v0_1_t *soc_info);

#endif  // __LIBSOC_HELPER_H__
