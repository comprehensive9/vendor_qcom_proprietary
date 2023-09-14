/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __CNSS_PLAT_H__
#define __CNSS_PLAT_H__

#include <stdbool.h>
#include "wlan_firmware_service_v01.h"

#define CNSS_MAX_FILE_PATH		100
#define CNSS_ANDROID_PROPERTY_MAX 128
#define CNSS_CAL_FILE		"/data/vendor/wifi/wlfw_cal_"
#define CNSS_DEFAULT_QDSS_TRACE_FILE "/data/vendor/wifi/qdss_trace.bin"
#define CNSS_QDSS_TRACE_CONFIG_FILE_OLD "/data/vendor/wifi/qdss_trace_config.bin"
#define CNSS_QDSS_TRACE_CONFIG_FILE_NEW \
			"/vendor/firmware_mnt/image/qdss_trace_config.cfg"

struct cnss_evt {
	uint32_t msg_id;
	void *data;
	uint32_t data_len;
	struct cnss_evt *next;
};

struct cnss_evt_queue {
	pthread_mutex_t mutex;
	struct cnss_evt *head, *tail;
};

struct cnss_evt *cnss_evt_alloc(uint32_t msg_id, void *data, uint32_t data_len);
void cnss_evt_enqueue(struct cnss_evt_queue *evt_q, struct cnss_evt *evt);
struct cnss_evt *cnss_evt_dequeue(struct cnss_evt_queue *evt_q);
void cnss_evt_free_queue(struct cnss_evt_queue *evt_q);
void cnss_evt_free(struct cnss_evt *evt);

bool cnss_plat_get_dms_mac_addr_prov_support(void);
int cnss_plat_get_qdss_cfg_hw_trc_override(void);
int cnss_plat_save_file(const char *filename, unsigned char *data, uint32_t len,
			bool append);
int cnss_plat_read_file(char *filename, unsigned char **file_buf);
#endif
