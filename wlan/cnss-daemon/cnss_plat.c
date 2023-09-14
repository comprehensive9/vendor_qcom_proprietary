/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>

#include "debug.h"
#include "cnss_plat.h"

#ifdef ANDROID
/* peripheral manager */
#include <pm-service.h>
#include <mdm_detect.h>

#define MODEM_BASEBAND_PROPERTY   "ro.baseband"
#if defined(__BIONIC_FORTIFY)
#define MODEM_BASEBAND_PROPERTY_SIZE  PROP_VALUE_MAX
#else
#define MODEM_BASEBAND_PROPERTY_SIZE  10
#endif
#define MODEM_BASEBAND_VALUE_APQ  "apq"
#define MODEM_BASEBAND_VALUE_SDA  "sda"
#define MODEM_BASEBAND_VALUE_QCS  "qcs"
#endif

#define CNSS_PLAT_MAX_FILE_LEN (8 * 1024 * 1024)

#ifdef ANDROID
static bool cnss_plat_check_modem_compatability(struct dev_info
						*mdm_detect_info)
{
	char args[MODEM_BASEBAND_PROPERTY_SIZE] = {0};
	int ret = 0;

	/* Get the hardware property */
	ret = property_get(MODEM_BASEBAND_PROPERTY, args, "");
	if (ret > MODEM_BASEBAND_PROPERTY_SIZE) {
		wsvc_printf_err("property [%s] has size [%d] that exceeds max [%d]",
				MODEM_BASEBAND_PROPERTY, ret,
				MODEM_BASEBAND_PROPERTY_SIZE);
		return 0;
	}
	wsvc_printf_err("ro.baseband : [%s]", args);

	/* This will check for the type of hardware, and if the
	   hardware type needs external modem, it will check if the
	   modem type is external*/
	if((!strncmp(MODEM_BASEBAND_VALUE_APQ, args, 3)) ||
	    (!strncmp(MODEM_BASEBAND_VALUE_SDA, args, 3)) ||
	    !strncmp(MODEM_BASEBAND_VALUE_QCS, args, 3)) {
		for (ret = 0; ret < mdm_detect_info->num_modems; ret++) {
			if (mdm_detect_info->mdm_list[ret].type ==
			    MDM_TYPE_EXTERNAL) {
				wsvc_printf_err("Hardware supports external modem");
				return 1;
			}
		}
		wsvc_printf_err("Hardware does not support external modem");
		return 0;
	}
	return 1;
}


bool cnss_plat_get_dms_mac_addr_prov_support(void)
{
	struct dev_info mdm_detect_info;
	int ret;

	ret = get_system_info(&mdm_detect_info);
	if (ret > 0) {
		wsvc_printf_err("Failed to get system info: %d", ret);
		return false;
	}

	if (mdm_detect_info.num_modems == 0) {
		wsvc_printf_err("No Modem support for this target");
		return false;
	}

	if (!cnss_plat_check_modem_compatability(&mdm_detect_info)) {
		wsvc_printf_err("Target does not have external modem");
		return false;
	}
	return true;
}

int cnss_plat_get_qdss_cfg_hw_trc_override(void)
{
	char hw_trc_disable_override[CNSS_ANDROID_PROPERTY_MAX + 1],
	     default_value[32];
	int tmp;

	snprintf(default_value, 32, "%d", QMI_PARAM_INVALID_V01);
	property_get("persist.vendor.cnss-daemon.hw_trc_disable_override",
		     hw_trc_disable_override, default_value);
	hw_trc_disable_override[CNSS_ANDROID_PROPERTY_MAX] = '\0';
	tmp = atoi(hw_trc_disable_override);

	return (tmp > QMI_PARAM_DISABLE_V01 ? QMI_PARAM_DISABLE_V01 :
		 (tmp < 0 ? QMI_PARAM_INVALID_V01 : tmp));
}
#else
int cnss_plat_get_qdss_cfg_hw_trc_override(void)
{
	return 0;
}

int cnss_plat_get_qdss_cfg()
{
	return QMI_PARAM_INVALID_V01;
}

bool cnss_plat_get_dms_mac_addr_prov_support(void)
{
	return false;
}
#endif

int cnss_plat_save_file(const char *filename, unsigned char *data, uint32_t len,
			bool append)
{
	long flen;
	size_t len_written = 0;
	FILE *fp = NULL;

	if (!filename || !data)
		return -1;

	if (append)
		fp = fopen(filename, "ab");
	else
		fp = fopen(filename, "wb");

	if (fp == NULL) {
		wsvc_printf_err("%s: Failed to open file %s", __func__,
				filename);
		return -1;
	}
	flen = ftell(fp);
	if (flen < 0) {
		wsvc_printf_err("%s: Invalid file size", __func__);
		fclose(fp);
		return -1;
	}

	if ((flen > LONG_MAX - len) || (flen + len > CNSS_PLAT_MAX_FILE_LEN)) {
		wsvc_printf_err("%s: Unexpected file len %ld", __func__,
				flen + len);
		fclose(fp);
		return -1;
	}

	len_written = fwrite(data, 1, len, fp);
	if (len_written != len) {
		wsvc_printf_err("%s: Invalid write:%zu Data len:%d", __func__,
				len_written, len);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	wsvc_printf_dbg("%s: Saved data to: %s", __func__, filename);

	return len_written;
}

int cnss_plat_read_file(char *filename, unsigned char **file_buf)
{
	unsigned char *fbuf;
	int len;
	int len_read = 0;
	FILE *fp = NULL;

	if (!filename) {
		wsvc_printf_err("%s: Filename is null", __func__);
		return -1;
	}

	if (access(filename, F_OK) == -1) {
		wsvc_printf_info("%s: No such file %s", __func__, filename);
		return -errno;
	}

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		wsvc_printf_err("%s: Failed to open file %s", __func__,
				filename);
		return -errno;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (len <= 0 || len > CNSS_PLAT_MAX_FILE_LEN) {
		wsvc_printf_err("%s: Unexpected file len %d", __func__, len);
		fclose(fp);
		return -EINVAL;
	}

	fbuf = malloc(len);
	if (fbuf == NULL) {
		wsvc_printf_err("%s: Failed to alloc mem", __func__);
		fclose(fp);
		return -ENOMEM;
	}

	len_read = fread(fbuf, 1, len, fp);
	if (len_read != len) {
		wsvc_printf_err("%s: invalid read %d: %d", __func__,
			len_read, len);
		fclose(fp);
		if (fbuf)
		    free(fbuf);
		return -errno;
	}
	fclose(fp);

	*file_buf = fbuf;
	return len_read;
}

struct cnss_evt *cnss_evt_alloc(uint32_t msg_id, void *data, uint32_t data_len)
{
	struct cnss_evt *evt = NULL;

	evt = malloc(sizeof(*evt));
	if (!evt)
		goto end;

	if (!data) {
		evt->data_len = 0;
		goto no_data;
	}

	evt->data = malloc(data_len);
	if (!evt->data) {
		free(evt);
		evt = NULL;
		goto end;
	}
	evt->data_len = data_len;
	memcpy(evt->data, data, data_len);
no_data:
	evt->msg_id = msg_id;
	evt->next = NULL;
end:
	return evt;
}

void cnss_evt_enqueue(struct cnss_evt_queue *evt_q, struct cnss_evt *evt)
{
	if (!evt)
		return;

	pthread_mutex_lock(&evt_q->mutex);
	if (!evt_q->head) {
		evt_q->head = evt;
		evt_q->tail = evt;
	} else {
		(evt_q->tail)->next = evt;
		evt_q->tail = evt;
	}
	pthread_mutex_unlock(&evt_q->mutex);
}

struct cnss_evt *cnss_evt_dequeue(struct cnss_evt_queue *evt_q)
{
	struct cnss_evt *evt;

	pthread_mutex_lock(&evt_q->mutex);
	if (!evt_q->head) {
		evt = NULL;
	} else if ((evt_q->head)->next == NULL) {
		evt = evt_q->head;
		evt_q->head = NULL;
		evt_q->tail = NULL;
	} else {
		evt = evt_q->head;
		evt_q->head = (evt_q->head)->next;
	}
	pthread_mutex_unlock(&evt_q->mutex);
	return evt;
}

void cnss_evt_free_queue(struct cnss_evt_queue *evt_q)
{
	struct cnss_evt *evt;

	pthread_mutex_lock(&evt_q->mutex);
	while (evt_q->head) {
		evt = evt_q->head;
		evt_q->head = (evt_q->head)->next;
		if (evt->data)
			free(evt->data);
		free(evt);
	}
	evt_q->head = NULL;
	evt_q->tail = NULL;
	pthread_mutex_unlock(&evt_q->mutex);
}

void cnss_evt_free(struct cnss_evt *evt)
{
	if (!evt)
		return;

	if (evt->data)
		free(evt->data);
	free(evt);
}


