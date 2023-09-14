/*
 * Copyright (c) 2020-21 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cutils/properties.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "libsoc_helper.h"

#define SOC_ID_SUPPORT_PROPERTY "ro.vendor.qti.soc_id"
#define SOC_ID_SYSFS_NODE "/sys/devices/soc0/soc_id"

static soc_info_v0_1_t cpu[] = {
    [0]   = {MSM_CPU_UNKNOWN},
    [321] = {MSM_CPU_SDM845},
    [336] = {MSM_CPU_SDM670},
    [339] = {MSM_CPU_SM8150},
    [356] = {MSM_CPU_KONA},
    [360] = {MSM_CPU_SDM710},
    [400] = {MSM_CPU_LITO},
    [415] = {MSM_CPU_LAHAINA},
    [439] = {APQ_CPU_LAHAINA},
    [450] = {MSM_CPU_SHIMA},
    [501] = {MSM_CPU_SM8325},
    [502] = {APQ_CPU_SM8325P},
    [475] = {MSM_CPU_YUPIK},
    [499] = {APQ_CPU_YUPIK},
    [497] = {MSM_CPU_YUPIK_IOT},
    [498] = {APQ_CPU_YUPIKP_IOT},
    [515] = {MSM_CPU_YUPIK_LTE},
};

// internal function to get soc_id
static int get_soc_id();

// internal function to lookup sysfs
static int lookup_sysfs_node();

//internal function to check if soc_id from getprop is mapped
static int is_cpu_mapped(long);

void get_soc_info(soc_info_v0_1_t *soc_info) {
    static int soc_id = 0;

    if (!soc_id) {
        soc_id = get_soc_id();
    }
    soc_info->msm_cpu = cpu[soc_id].msm_cpu;
}

static int get_soc_id() {
    int msm_soc_id = 0;
    char soc_id_prop[PROPERTY_VALUE_MAX] = {0};
    long value = 0;
    char *end_ptr;

    if (property_get(SOC_ID_SUPPORT_PROPERTY, soc_id_prop, "0")){
        errno = 0;
        value = strtol(soc_id_prop, &end_ptr, 10);
        if ((errno == ERANGE) || (end_ptr == soc_id_prop) ||
            (value == 0) || (is_cpu_mapped(value) == -1)) {
            msm_soc_id = lookup_sysfs_node();
        } else {
            msm_soc_id = value;
        }
    } else {
        msm_soc_id = lookup_sysfs_node();
    }

    return msm_soc_id;
}

static int lookup_sysfs_node() {
    FILE *fp = NULL;
    static int sysfs_soc_id = 0;

    if (!sysfs_soc_id) {
        fp = fopen(SOC_ID_SYSFS_NODE, "r");
        if (NULL != fp) {
            if (0 == fscanf(fp, "%d", &sysfs_soc_id)) {
                sysfs_soc_id = 0;
            }
            fclose(fp);
        }
    }

    if (is_cpu_mapped((long)sysfs_soc_id) == -1) {
        sysfs_soc_id = 0;
    }

    return sysfs_soc_id;
}

static int is_cpu_mapped(long value) {
    int cpu_mapped = -1;

    if ((value == MSM_CPU_LAHAINA) || (value == MSM_CPU_SHIMA) ||
        (value == APQ_CPU_LAHAINA) || (value == MSM_CPU_SM8325) ||
        (value == APQ_CPU_SM8325P) || (value == MSM_CPU_KONA) ||
        (value == MSM_CPU_LITO) || (value == MSM_CPU_SM8150) ||
        (value == MSM_CPU_SDM710) || (value == MSM_CPU_SDM670) ||
        (value == MSM_CPU_SDM845) || (value == MSM_CPU_YUPIK) ||
        (value == APQ_CPU_YUPIK) || (value == MSM_CPU_YUPIK_IOT) ||
        (value == APQ_CPU_YUPIKP_IOT) || (value == MSM_CPU_YUPIK_LTE)) {
        cpu_mapped = 0;
    }

    return cpu_mapped;
}
