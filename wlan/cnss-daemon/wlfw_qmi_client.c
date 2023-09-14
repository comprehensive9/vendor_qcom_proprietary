/*
 * Copyright (c) 2015-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#ifdef CONFIG_RECORD_DAEMON_QMI_LOG
#include <fcntl.h>
#endif
#include <inttypes.h>

#include <cutils/properties.h>
#include <qmi_idl_lib_internal.h>
#ifdef ANDROID
#include <qmi_client_instance_defs.h>
#else
#include "qmi_client.h"
#endif
#ifdef ANDROID
/* peripheral manager */
#include <pm-service.h>
#include <mdm_detect.h>
#endif

#include "debug.h"
#include "cnss_plat.h"
#include "wlfw_qmi_client.h"

#ifdef ANDROID
#include "device_management_service_v01.h"
#endif

#ifdef CONFIG_NO_PM
typedef enum MdmType {
    MDM_TYPE_EXTERNAL = 0,
    MDM_TYPE_INTERNAL,
    SUBSYS_TYPE_SENSOR,
    SUBSYS_TYPE_SPSS,
} MdmType;
#endif

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#ifdef USE_GLIB
#undef FALSE
#endif

#define WLFW_CLIENT_ID		0x444d4f4e
#define WLFW_TIMEOUT_MS		(10000)
#define WLFW_TIMEOUT_DMS_MS	(3000)
#define DEFAULT_BDF_FILE	"bdwlan.bin"
#define REGDB_BDF_FILE		"regdb.bin"
#define BDF_FILE		"bdwlan."
#define PERSIST_XO_CAL_FILE     "/mnt/vendor/persist/wlan/xo_cal_data.bin"
#define DATA_XO_CAL_FILE	"/data/vendor/wifi/xo_cal_data.bin"
#define HOLI_TARGET_NAME	"holi"
#define TARGET_PLATFORM_PROPERTY	"ro.board.platform"
#define FALSE			1
#define WLFW_MAX_BDF_RETRY	2

#define CAL_DOWNLOAD_RETRY_MAX_TIMES	3
#define CAL_DOWNLOAD_RETRY_DELAY_US	500000

#define CNSS_WLFW_QMI_CONNECTED		BIT(0)
#define CNSS_MSA_READY			BIT(1)
#define CNSS_FW_MEM_READY		BIT(2)
#define CNSS_BDF_DOWNLOAD_DONE		BIT(3)

#define CNSS_IS_WLFW_QMI_CONNECTED(_state) ((_state) & CNSS_WLFW_QMI_CONNECTED)
#define CNSS_IS_MSA_READY(_state) ((_state) & CNSS_MSA_READY)
#define CNSS_IS_FW_MEM_READY(_state) ((_state) & CNSS_FW_MEM_READY)
#define CNSS_IS_BDF_DOWNLOAD_DONE(_state) ((_state) & CNSS_BDF_DOWNLOAD_DONE)

#define WLAN_BOARD_ID_INDEX 0x100
#define QMI_ERR_PLAT_CCPM_CLK_INIT_FAILED 0x77

#define MAX_PM_CLIENT 2

#define UMC_CHIP_ID  0x4320
#define BDWLAN_SIZE  6

const char *bdf_dir_list[] = {"/firmware/image/", "/vendor/firmware/",
				"/vendor/firmware_mnt/image/", NULL};

static int svc_running = 0;
static int dms_running = 0;

#ifdef CONFIG_RECORD_DAEMON_QMI_LOG
const char debug_file[] =  "/sys/kernel/debug/cnss/qmi_record";
#endif

struct cal_data {
	wlfw_cal_temp_id_enum_v01 cal_id;
	uint32_t total_size;
	uint8_t *data;
};

struct wlan_mac_addr {
	uint8_t mac_addr_valid;
	uint8_t mac_addr[QMI_WLFW_MAC_ADDR_SIZE_V01];
};

enum wlfw_instance_id {
	ADRASTEA_ID = 0,
	NAPIER_ID = 1,
	HAWKEYE_ID = 2,
	MOSELLE_ID = 3,
};

enum cnss_bdf_type {
	CNSS_BDF_BIN,
	CNSS_BDF_ELF,
	CNSS_BDF_REGDB = 4,
	CNSS_BDF_DUMMY = 255,
};

struct pm_data {
	void *pm_handle;
	MdmType type;
};

struct wlfw_client_data {
	qmi_idl_service_object_type svc_obj;
	qmi_client_type clnt_handler;
	qmi_service_instance instance_id;
	struct pm_data pm_client[MAX_PM_CLIENT];
	uint32_t state;
	pthread_t thread_id;
#ifdef CONFIG_READ_NV_MAC_ADDR
	pthread_t thread_id_dms;
	pthread_mutex_t dms_mutex;
	pthread_cond_t dms_cond;
#endif
	pthread_mutex_t mutex;
	bool cond_signaled;
	pthread_cond_t cond;
	pthread_cond_t cond_rsp;
	struct cnss_evt_queue evt_q;
	pthread_mutex_t cal_mutex;
	struct cal_data cal_data_array[QMI_WLFW_MAX_NUM_CAL_V01];
	wlfw_rf_chip_info_s_v01 chip_info;
	wlfw_rf_board_info_s_v01 board_info;
	wlfw_soc_info_s_v01 soc_info;
	wlfw_fw_version_info_s_v01 fw_version_info;
	struct wlan_mac_addr mac_addr;
	char foundry_name;
} gdata;


#ifdef CONFIG_NO_PM
int pm_init(MdmType type, const char *name)
{
	return 0;
}
void pm_deinit(MdmType type)
{
	return;
}
#else
int pm_vote(void *pm_handle)
{
	int ret = 0;

	if (!pm_handle) {
		wsvc_printf_err("%s: pm_handle is NULL", __func__);
		ret = -1;
		goto out;
	}

	wsvc_printf_dbg("%s: Vote for modem", __func__);

	ret = pm_client_connect(pm_handle);
	if (ret)
		wsvc_printf_err("%s: Failed to vote, ret = %d",
				__func__, ret);
out:
	return ret;
}

void pm_release_vote(void *pm_handle)
{
	if (!pm_handle) {
		wsvc_printf_err("%s: pm_handle is NULL", __func__);
		return;
	}

	wsvc_printf_dbg("%s: Release vote for modem", __func__);

	pm_client_disconnect(pm_handle);
}

void pm_event_notifier(void *client_cookie, enum pm_event event)
{
	struct pm_data *data = client_cookie;

	if (!data)
		return;

	wsvc_printf_dbg("%s: event %d received on pm[%d]",
			__func__, event, data->type);

	if (data->pm_handle)
		pm_client_event_acknowledge(data->pm_handle, event);
}

int pm_init(MdmType type, const char *name)
{
	struct dev_info devinfo;
	int i, ret;
	enum pm_event event;
	int found = 0;
	struct pm_data *data = NULL;

	wsvc_printf_dbg("%s: Starting pm[%d] service", __func__, type);

	if (type >= MAX_PM_CLIENT) {
		wsvc_printf_err("%s: Invalid MDM type %d\n", __func__, type);
		return -1;
	}

	data = &gdata.pm_client[type];
	data->type = type;

	ret = get_system_info(&devinfo);
	if (ret != RET_SUCCESS) {
		wsvc_printf_err("%s: Failed to get device info, ret = %d",
				__func__, ret);
		goto out;
	}

	wsvc_printf_dbg("%s: devinfo.num_modems = %d\n",
			__func__, devinfo.num_modems);
	for (i = 0; i < devinfo.num_modems; i++) {
		if (devinfo.mdm_list[i].type == type &&
		    (!name ||
		     strcmp(devinfo.mdm_list[i].mdm_name, name) == 0)) {
			wsvc_printf_dbg("%s: Found modem: type:%d name:%s",
					__func__, devinfo.mdm_list[i].type,
					devinfo.mdm_list[i].mdm_name);
			ret = pm_client_register(pm_event_notifier,
						 data,
						 devinfo.mdm_list[i].mdm_name,
						 "cnss-daemon",
						 &event,
						 &data->pm_handle);
			if (ret != PM_RET_SUCCESS) {
				wsvc_printf_err("%s: Failed to register pm client, ret = %d",
						__func__, ret);
				continue;
			}
			found = 1;
			break;
		}
	}

	if (found == 0) {
		ret = -1;
		goto out;
	}

	ret = pm_vote(data->pm_handle);

out:
	return ret;
}

void pm_deinit(MdmType type)
{
	void *pm_handle = NULL;

	wsvc_printf_dbg("%s: Exit pm[%d] service\n", __func__, type);

	pm_handle = gdata.pm_client[type].pm_handle;

	pm_release_vote(pm_handle);

	if (pm_handle)
		pm_client_unregister(pm_handle);

	gdata.pm_client[type].pm_handle = NULL;
}
#endif

static int wlfw_send_ind_register_req(uint64_t *fw_status)
{
	int rc = QMI_NO_ERR;
	wlfw_ind_register_req_msg_v01 req;
	wlfw_ind_register_resp_msg_v01 resp;

	wsvc_printf_dbg("%s", __func__);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.client_id_valid = 1;
	req.client_id = WLFW_CLIENT_ID;
	req.fw_ready_enable_valid = 1;
	req.fw_ready_enable = 1;

	if (gdata.instance_id != MOSELLE_ID) {
		req.initiate_cal_download_enable_valid = 1;
		req.initiate_cal_download_enable = 1;
		req.initiate_cal_update_enable_valid = 1;
		req.initiate_cal_update_enable = 1;
	}

	if (gdata.instance_id == ADRASTEA_ID) {
		req.msa_ready_enable_valid = 1;
		req.msa_ready_enable = 1;
		req.xo_cal_enable_valid = 1;
		req.xo_cal_enable = 1;
	}

	if (gdata.instance_id == NAPIER_ID ||
	    gdata.instance_id == HAWKEYE_ID ||
	    gdata.instance_id == MOSELLE_ID) {
		req.fw_mem_ready_enable_valid = 1;
		req.fw_mem_ready_enable = 1;
	}

	if (gdata.instance_id != MOSELLE_ID && gdata.instance_id != NAPIER_ID) {
		req.qdss_trace_save_enable_valid = 1;
		req.qdss_trace_save_enable = 1;
	}

	daemon_qmihist_record(QMI_WLFW_IND_REGISTER_REQ_V01, rc);
	rc = qmi_client_send_msg_sync(gdata.clnt_handler,
				      QMI_WLFW_IND_REGISTER_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);

	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("%s: rc %d, result %d, error %d",
				__func__,
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	if (resp.fw_status_valid)
		*fw_status = resp.fw_status;

	wsvc_printf_dbg("%s: result %d, error %d, fw_status_valid %u, fw_status 0x%" PRIx64,
			__func__,
			resp.resp.result,
			resp.resp.error,
			resp.fw_status_valid,
			resp.fw_status);

out:
	daemon_qmihist_record(QMI_WLFW_IND_REGISTER_REQ_V01, rc);
	return rc;
}

static int wlfw_send_cap_req(void)
{
	int rc = QMI_NO_ERR;
	wlfw_cap_req_msg_v01 req;
	wlfw_cap_resp_msg_v01 resp;

	wsvc_printf_dbg("%s", __func__);
	memset(&resp, 0, sizeof(resp));
	daemon_qmihist_record(QMI_WLFW_CAP_REQ_V01, rc);

	rc = qmi_client_send_msg_sync(gdata.clnt_handler,
				      QMI_WLFW_CAP_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);

	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("%s: rc %d, result %d, error %d",
				__func__,
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.error == QMI_ERR_PLAT_CCPM_CLK_INIT_FAILED) {
			wsvc_printf_err("RF card Not Present");
			rc = -EAGAIN;
			goto out;
		}
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	wsvc_printf_dbg("%s: result %d, error %d", __func__,
				resp.resp.result,
				resp.resp.error);

	/* store cap locally */
	if (resp.chip_info_valid)
		gdata.chip_info = resp.chip_info;
	if (resp.board_info_valid)
		gdata.board_info = resp.board_info;
	else
		gdata.board_info.board_id = 0xFF;
	if (resp.soc_info_valid)
		gdata.soc_info = resp.soc_info;
	if (resp.fw_version_info_valid)
		gdata.fw_version_info = resp.fw_version_info;
	/* copy foundry name first character to append in bdf file name */
	if (resp.foundry_name_valid)
		gdata.foundry_name = resp.foundry_name[0];

	wsvc_printf_info("%s: chip_id: 0x%0x, chip_family 0x%x,"
			 " board_id: 0x%0x, soc_id: 0x%0x,"
			 " fw_version: 0x%0x, fw_build_timestamp: %s",
			 __func__,
			 gdata.chip_info.chip_id,
			 gdata.chip_info.chip_family,
			 gdata.board_info.board_id,
			 gdata.soc_info.soc_id,
			 gdata.fw_version_info.fw_version,
			 gdata.fw_version_info.fw_build_timestamp);

out:
	daemon_qmihist_record(QMI_WLFW_CAP_REQ_V01, rc);
	return rc;
}

static int wlfw_send_bdf_download_req(enum cnss_bdf_type bdf_type)
{
	int i, rc = FALSE, ret;
	wlfw_bdf_download_req_msg_v01 req;
	wlfw_bdf_download_resp_msg_v01 resp;
	FILE *file = NULL;
	unsigned char *buffer, *temp;
	unsigned int file_len, remaining;
	char filename[QMI_WLFW_MAX_STR_LEN_V01];
	char foundry_specific_filename[QMI_WLFW_MAX_STR_LEN_V01];
	char filefullpath[CNSS_MAX_FILE_PATH];
	char target_name[MAX_PROPERTY_SIZE];

	wsvc_printf_dbg("%s", __func__);

	if (CNSS_BDF_BIN == bdf_type) {
		/* Below arrangement is special requirement for mannar in which even if
		 * board ID is 0x5f, 0x66 board ID's bdf should be loaded*/
		ret = property_get(TARGET_PLATFORM_PROPERTY, target_name, "");
		if (ret > MAX_PROPERTY_SIZE) {
			wsvc_printf_err("property [%s] has size [%d] that exceeds max [%d]",
					TARGET_PLATFORM_PROPERTY, ret,
					MAX_PROPERTY_SIZE);
			goto bdf_download;
		}
		if(0x5f == gdata.board_info.board_id && !strcmp(HOLI_TARGET_NAME, target_name))
			gdata.board_info.board_id = 0x66;

bdf_download:
		/* retrive bdf file name from capability */
		if (0xFF == gdata.board_info.board_id) {
			snprintf(filename, sizeof(filename),
				 "%s", DEFAULT_BDF_FILE);
		} else {
			snprintf(filename, sizeof(filename), "%s", BDF_FILE);
			if (gdata.board_info.board_id >= WLAN_BOARD_ID_INDEX) {
				snprintf(filename + strlen(filename),
					sizeof(filename) - strlen(filename),
					"%03x", gdata.board_info.board_id);
			} else {
				snprintf(filename + strlen(filename),
					sizeof(filename) - strlen(filename),
					"b%02x", gdata.board_info.board_id);
			}
		}

		if (gdata.foundry_name) {
			strlcpy(foundry_specific_filename, filename, QMI_WLFW_MAX_STR_LEN_V01);
			memmove(foundry_specific_filename + BDWLAN_SIZE + 1,
				foundry_specific_filename + BDWLAN_SIZE,
				BDWLAN_SIZE - 1);
			foundry_specific_filename[BDWLAN_SIZE] = gdata.foundry_name;
			foundry_specific_filename[QMI_WLFW_MAX_STR_LEN_V01 - 1] = '\0';
		} else if (UMC_CHIP_ID == gdata.chip_info.chip_id) {
			memmove(filename + BDWLAN_SIZE + 1, filename + BDWLAN_SIZE,
				BDWLAN_SIZE - 1);
			filename[BDWLAN_SIZE] = 'u';
			filename[QMI_WLFW_MAX_STR_LEN_V01 - 1] = '\0';
			wsvc_printf_info("%s: Updated BDF file : %s", __func__, filename);
		}

	} else {
		snprintf(filename, sizeof(filename),
			 "%s", REGDB_BDF_FILE);
	}

	wsvc_printf_info_high("%s: BDF file : %s", __func__,
			      gdata.foundry_name ? foundry_specific_filename : filename);

	if (CNSS_BDF_BIN == bdf_type && gdata.foundry_name) {
		for (i = 0; bdf_dir_list[i]; i++) {
			snprintf(filefullpath, sizeof(filefullpath),
				 "%s%s", bdf_dir_list[i], foundry_specific_filename);
			wsvc_printf_dbg("%s: Looking for BDF file: %s",
					 __func__, filefullpath);
			file = fopen(filefullpath, "rb");
			if (file != NULL) {
				wsvc_printf_dbg("%s: BDF file found: %s",
						__func__, filefullpath);
				goto start_file_download;
			}
		}
		wsvc_printf_info_high("%s: Unable to download foundry specific bdf file : %s",
				      __func__, foundry_specific_filename);
	}

	for (i = 0; bdf_dir_list[i]; i++) {
		snprintf(filefullpath, sizeof(filefullpath),
			"%s%s", bdf_dir_list[i], filename);
		wsvc_printf_dbg("%s: Looking for BDF file: %s",
			__func__, filefullpath);
		file = fopen(filefullpath, "rb");
		if (file != NULL) {
			wsvc_printf_dbg("%s: BDF file found: %s",
			__func__, filefullpath);
			break;
		}
	}
	if (!file) {
		wsvc_printf_err("%s: Failed to find BDF file %s", __func__,
			filename);
		rc = -1;
		goto end;
	}

start_file_download:
	fseek(file, 0, SEEK_END);
	file_len = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = (unsigned char *)malloc(file_len);
	if (buffer == NULL) {
		wsvc_printf_err("%s: Fail to alloc memory for BDF file",
					 __func__);
		rc = -1;
		goto memory_alloc_fail;
	}
	if (fread(buffer, 1, file_len, file) != file_len) {
		wsvc_printf_err("%s: Fail to read BDF file", __func__);
		rc = -1;
		goto read_fail;
	}
	fclose(file);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	temp = buffer;
	remaining = file_len;
	while (remaining) {
		req.valid = 1;
		/* file id should retrive from capability */
		req.file_id_valid = 1;
		req.file_id = 0;
		req.total_size_valid = 1;
		req.total_size = file_len;
		req.seg_id_valid = 1;
		req.data_valid = 1;
		req.end_valid = 1;
		req.bdf_type_valid = 1;
		req.bdf_type = bdf_type;

		if (remaining > QMI_WLFW_MAX_DATA_SIZE_V01) {
			req.data_len = QMI_WLFW_MAX_DATA_SIZE_V01;
		} else {
			req.data_len = remaining;
			req.end = 1;
		}
		memcpy(req.data, temp, req.data_len);

		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
					      QMI_WLFW_BDF_DOWNLOAD_REQ_V01,
					      &req,
					      sizeof(req),
					      &resp,
					      sizeof(resp),
					      WLFW_TIMEOUT_MS);
		if (rc != QMI_NO_ERR) {
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);
			rc = FALSE;
			goto out;
		}

		remaining -= req.data_len;
		temp += req.data_len;
		req.seg_id++;
	}

	wsvc_printf_info("%s: bdf type %d,result %d, error %d", __func__,
				bdf_type,
				resp.resp.result,
				resp.resp.error);
out:
        free(buffer);
        buffer = NULL;
        if (QMI_ERR_INVALID_ARG_V01 == resp.resp.error) {
		wsvc_printf_dbg("%s: Response failure : 0x%0x",
				__func__, resp.resp.error);
		rc = FALSE;
        }
	return rc;

read_fail:
	free(buffer);
	buffer = NULL;
memory_alloc_fail:
	fclose(file);
end:
	return rc;
}

static int wlfw_send_cal_report_req(void)
{
	int rc = QMI_NO_ERR;
	wlfw_cal_report_req_msg_v01 req;
	wlfw_cal_report_resp_msg_v01 resp;
	unsigned char *pcal = NULL;
	int i, j = 0;

	wsvc_printf_dbg("%s", __func__);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	if (gdata.instance_id == ADRASTEA_ID) {
		wsvc_printf_err("%s check for xo cal file", __func__);
		if (cnss_plat_read_file(PERSIST_XO_CAL_FILE, &pcal) > 0 ||
		    cnss_plat_read_file(DATA_XO_CAL_FILE, &pcal) > 0) {
			wsvc_printf_err("%s xo cal file found", __func__);
			req.xo_cal_data_valid = 1;
			req.xo_cal_data = *pcal;
		}
		if (pcal)
			free(pcal);
	}

	req.cal_remove_supported_valid = 1;
#ifdef CAL_DATA_REMOVE
	req.cal_remove_supported = 1;
#else
	req.cal_remove_supported = 0;
#endif

	pthread_mutex_lock(&(gdata.cal_mutex));
	for(i = 0; i < QMI_WLFW_MAX_NUM_CAL_V01; i++) {
		if (gdata.cal_data_array[i].total_size &&
		    gdata.cal_data_array[i].data) {
			req.meta_data[j] = gdata.cal_data_array[i].cal_id;
			j++;
		}
	}
	pthread_mutex_unlock(&(gdata.cal_mutex));

	req.meta_data_len = j;
	daemon_qmihist_record(QMI_WLFW_CAL_REPORT_REQ_V01, rc);
	rc = qmi_client_send_msg_sync(gdata.clnt_handler,
				      QMI_WLFW_CAL_REPORT_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);
	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("%s: rc %d, result %d, error %d",
				__func__,
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	wsvc_printf_dbg("%s: result %d, error %d", __func__,
				resp.resp.result,
				resp.resp.error);

out:
	daemon_qmihist_record(QMI_WLFW_CAL_REPORT_REQ_V01, rc);
	return rc;
}

#ifdef CONFIG_READ_NV_MAC_ADDR
static int wlfw_send_mac_addr_req(void)
{
	int rc;
	wlfw_mac_addr_req_msg_v01 req;
	wlfw_mac_addr_resp_msg_v01 resp;

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.mac_addr_valid = 1;
	memcpy(req.mac_addr, gdata.mac_addr.mac_addr,
	       QMI_WLFW_MAC_ADDR_SIZE_V01);

	wsvc_printf_mac_addr(MSG_DEBUG, "Sending MAC address to FW",
			     gdata.mac_addr.mac_addr);

	rc = qmi_client_send_msg_sync(gdata.clnt_handler,
				      QMI_WLFW_MAC_ADDR_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);
	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("Failed to send MAC address: rc %d, result %d, error %d",
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	wsvc_printf_dbg("%s: result %d, error %d", __func__,
			resp.resp.result,
			resp.resp.error);

out:
	return rc;
}
#endif

static struct cal_data *find_cal_file(wlfw_cal_temp_id_enum_v01 cal_id)
{
	int i;

	for (i = 0; i < QMI_WLFW_MAX_NUM_CAL_V01; i++) {
		if (gdata.cal_data_array[i].cal_id == cal_id) {
			if (gdata.cal_data_array[i].total_size &&
			    gdata.cal_data_array[i].data)
				return &gdata.cal_data_array[i];
		}
	}

	return NULL;
}

static void free_cal_file(void)
{
	int i;

	pthread_mutex_lock(&(gdata.cal_mutex));
	for (i = 0; i < QMI_WLFW_MAX_NUM_CAL_V01; i++) {
		gdata.cal_data_array[i].cal_id = 0;
		gdata.cal_data_array[i].total_size = 0;
		if (gdata.cal_data_array[i].data) {
			wsvc_printf_info("Start to free CAL table id: %d", i);
			free(gdata.cal_data_array[i].data);
			gdata.cal_data_array[i].data = NULL;
			wsvc_printf_info("End to free CAL table id: %d", i);
		}
	}
	pthread_mutex_unlock(&(gdata.cal_mutex));
}

static int wlfw_send_cal_download_req(void *data)
{
	int rc = QMI_NO_ERR;
	wlfw_cal_download_req_msg_v01 req;
	wlfw_cal_download_resp_msg_v01 resp;
	struct cal_data *p_cal_data = NULL;
	unsigned char *temp;
	unsigned int remaining;
	int retry = 0;

	wlfw_initiate_cal_download_ind_msg_v01 *ind = data;
	wlfw_cal_temp_id_enum_v01 cal_id = ind->cal_id;

	wsvc_printf_dbg("%s: cal_id %u", __func__, cal_id);

	pthread_mutex_lock(&(gdata.cal_mutex));
	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	p_cal_data = find_cal_file(cal_id);
	daemon_qmihist_record(QMI_WLFW_CAL_DOWNLOAD_REQ_V01, rc);
	if (p_cal_data == NULL) {
		req.total_size_valid = 1;
		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
					      QMI_WLFW_CAL_DOWNLOAD_REQ_V01,
					      &req,
					      sizeof(req),
					      &resp,
					      sizeof(resp),
					      WLFW_TIMEOUT_MS);
		if ((rc != QMI_NO_ERR) ||
		   (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);
		}
		wsvc_printf_dbg("%s: result %d, error %d", __func__,
					resp.resp.result,
					resp.resp.error);
		goto out;
	}

	temp = p_cal_data->data;
	remaining = p_cal_data->total_size;
	while (remaining) {
retry_dl:
		req.valid = 1;
		req.file_id_valid = 1;
		req.file_id = p_cal_data->cal_id;
		req.total_size_valid = 1;
		req.total_size = p_cal_data->total_size;
		req.seg_id_valid = 1;
		req.data_valid = 1;
		req.end_valid = 1;
		if (remaining > QMI_WLFW_MAX_DATA_SIZE_V01) {
			req.data_len = QMI_WLFW_MAX_DATA_SIZE_V01;
		} else {
			req.data_len = remaining;
			req.end = 1;
		}
		memcpy(req.data, temp, req.data_len);

		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
					      QMI_WLFW_CAL_DOWNLOAD_REQ_V01,
					      &req,
					      sizeof(req),
					      &resp,
					      sizeof(resp),
					      WLFW_TIMEOUT_MS);
		if ((rc != QMI_NO_ERR) ||
		    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);
			if (resp.resp.error == QMI_ERR_NO_MEMORY_V01 &&
			    retry++ < CAL_DOWNLOAD_RETRY_MAX_TIMES) {
				wsvc_printf_info("%s: Failed to download CAL seq#%d, retry#%d",
						 __func__, req.seg_id, retry);
				usleep(CAL_DOWNLOAD_RETRY_DELAY_US * retry);
				goto retry_dl;
			}

			goto out;
		}

		remaining -= req.data_len;
		temp += req.data_len;
		req.seg_id++;
		retry = 0;
	}

	wsvc_printf_dbg("%s: result %d, error %d", __func__,
				resp.resp.result,
				resp.resp.error);
out:
	pthread_mutex_unlock(&(gdata.cal_mutex));
	daemon_qmihist_record(QMI_WLFW_CAL_DOWNLOAD_REQ_V01, rc);
	return rc;
}

static int wlfw_send_cal_update_req(void *data)
{
	int rc = QMI_NO_ERR;
	wlfw_cal_update_req_msg_v01 req;
	wlfw_cal_update_resp_msg_v01 resp;
	unsigned char *p_cal_data_temp, *p_cal_data = NULL;
	unsigned int remaining;
	char filename[CNSS_MAX_FILE_PATH];

	wlfw_initiate_cal_update_ind_msg_v01 *ind = data;
	wlfw_cal_temp_id_enum_v01 cal_id = ind->cal_id;
	unsigned int total_size = ind->total_size;

	wsvc_printf_dbg("%s: cal_id %u, total_size %d", __func__, ind->cal_id,
			ind->total_size);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	if (cal_id < 0 || cal_id >= QMI_WLFW_MAX_NUM_CAL_V01) {
		wsvc_printf_err("%s: Invalid Cal ID %u", __func__, cal_id);
		rc = -1;
		goto end;
	}

	p_cal_data = (unsigned char *)malloc(total_size);
	if (p_cal_data == NULL) {
		wsvc_printf_err("%s: Fail to alloc mem for cal file", __func__);
		rc = -1;
		goto end;
	}

	remaining = total_size;
	p_cal_data_temp = p_cal_data;
	daemon_qmihist_record(QMI_WLFW_CAL_UPDATE_REQ_V01, rc);

	while (remaining && resp.end == 0) {
		req.cal_id = cal_id;
		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
					      QMI_WLFW_CAL_UPDATE_REQ_V01,
					      &req,
					      sizeof(req),
					      &resp,
					      sizeof(resp),
					      WLFW_TIMEOUT_MS);
		if ((rc != QMI_NO_ERR) ||
		   (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);

			goto fail;
		}

		wsvc_printf_dbg("%s: result %d, error %d", __func__,
					resp.resp.result,
					resp.resp.error);

		if ((resp.file_id_valid == 1 && resp.file_id == cal_id)
		   && (resp.total_size_valid == 1 && resp.total_size == total_size)
		   && (resp.seg_id_valid == 1 && resp.seg_id == req.seg_id)
		   && (resp.data_valid == 1 && resp.data_len <= QMI_WLFW_MAX_DATA_SIZE_V01)) {
			memcpy(p_cal_data_temp, resp.data, resp.data_len);
		} else {
			wsvc_printf_err("%s: Unmatched cal data, "
					"Expect file_id %u, total_size %u, "
					"seg_id %u, Recv file_id_valid %u, "
					"file_id %u, total_size_valid %u, "
					"total_size %u, seg_id_valid %u, "
					"seg_id %u, data_len_valid %u, "
					"data_len %u",
					__func__,
					cal_id, total_size, req.seg_id,
					resp.file_id_valid,
					resp.file_id,
					resp.total_size_valid,
					resp.total_size,
					resp.seg_id_valid,
					resp.seg_id,
					resp.data_valid,
					resp.data_len);
			rc = -1;
			goto fail;
		}
		remaining -= resp.data_len;
		p_cal_data_temp += resp.data_len;
		req.seg_id++;
	}

	if (remaining == 0 && (resp.end_valid && resp.end)) {
		pthread_mutex_lock(&(gdata.cal_mutex));
		gdata.cal_data_array[cal_id].cal_id = cal_id;
		gdata.cal_data_array[cal_id].total_size = total_size;
		if (gdata.cal_data_array[cal_id].data)
			free(gdata.cal_data_array[cal_id].data);
		gdata.cal_data_array[cal_id].data = p_cal_data;
		snprintf(filename, sizeof(filename),
			 CNSS_CAL_FILE"%02d.bin", cal_id);
		cnss_plat_save_file(filename, p_cal_data, total_size, false);
		pthread_mutex_unlock(&(gdata.cal_mutex));
		goto end;
	} else {
		wsvc_printf_err("%s: Cal file corrupted: remaining %u, "
				"end_valid %u, end %u",	__func__,
				remaining, resp.end_valid, resp.end);
		rc = -1;
		goto fail;
	}

fail:
	free(p_cal_data);
end:
	daemon_qmihist_record(QMI_WLFW_CAL_UPDATE_REQ_V01, rc);
	return rc;
}

static int wlfw_send_qdss_trace_data_req(void *data)
{
	int rc = QMI_NO_ERR;
	wlfw_qdss_trace_data_req_msg_v01 req;
	wlfw_qdss_trace_data_resp_msg_v01 resp;
	unsigned char *p_qdss_trace_data_temp, *p_qdss_trace_data = NULL;
	unsigned int remaining;
	char filename[CNSS_MAX_FILE_PATH];

	wlfw_qdss_trace_save_ind_msg_v01 *ind = data;
	unsigned int total_size = ind->total_size;
	char *file_name = ind->file_name;

	if (ind->source != 1) {
		wsvc_printf_info("%s: Indication not for daemon", __func__);
		return -EINVAL;
	}

	wsvc_printf_dbg("%s: source %u, total_size %d, file_name %s",
			__func__, ind->source, ind->total_size, ind->file_name);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	p_qdss_trace_data = (unsigned char *)malloc(total_size);
	if (p_qdss_trace_data == NULL) {
		wsvc_printf_err("%s: Fail to alloc mem for qdss trace file",
				__func__);
		rc = -1;
		goto end;
	}

	remaining = total_size;
	p_qdss_trace_data_temp = p_qdss_trace_data;
	while (remaining && resp.end == 0) {
		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
					      QMI_WLFW_QDSS_TRACE_DATA_REQ_V01,
					      &req,
					      sizeof(req),
					      &resp,
					      sizeof(resp),
					      WLFW_TIMEOUT_MS);
		if ((rc != QMI_NO_ERR) ||
		   (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);

			goto fail;
		}

		wsvc_printf_dbg("%s: result %d, error %d", __func__,
					resp.resp.result,
					resp.resp.error);

		if ((resp.total_size_valid == 1 &&
		     resp.total_size == total_size)
		   && (resp.seg_id_valid == 1 && resp.seg_id == req.seg_id)
		   && (resp.data_valid == 1 &&
		       resp.data_len <= QMI_WLFW_MAX_DATA_SIZE_V01)) {
			memcpy(p_qdss_trace_data_temp,
			       resp.data, resp.data_len);
		} else {
			wsvc_printf_err("%s: Unmatched qdss trace data, "
					"Expect total_size %u, "
					"seg_id %u, Recv total_size_valid %u, "
					"total_size %u, seg_id_valid %u, "
					"seg_id %u, data_len_valid %u, "
					"data_len %u",
					__func__,
					total_size, req.seg_id,
					resp.total_size_valid,
					resp.total_size,
					resp.seg_id_valid,
					resp.seg_id,
					resp.data_valid,
					resp.data_len);
			rc = -1;
			goto fail;
		}

		remaining -= resp.data_len;
		p_qdss_trace_data_temp += resp.data_len;
		req.seg_id++;
	}

	if (remaining == 0 && (resp.end_valid && resp.end)) {
		if (!file_name)
			snprintf(filename, sizeof(filename),
				 CNSS_DEFAULT_QDSS_TRACE_FILE);
		else
			snprintf(filename, sizeof(filename),
				 "/data/vendor/wifi/%s", file_name);
		cnss_plat_save_file(filename, p_qdss_trace_data, total_size,
				    true);
	} else {
		wsvc_printf_err("%s: QDSS trace file corrupted: remaining %u, "
				"end_valid %u, end %u",	__func__,
				remaining, resp.end_valid, resp.end);
		rc = -1;
		goto fail;
	}

fail:
	free(p_qdss_trace_data);
end:
	return rc;
}

static int wlfw_send_qdss_trace_mode_req(wlfw_qdss_trace_mode_enum_v01 mode,
					 unsigned long long option)
{
	int rc = QMI_NO_ERR;
	wlfw_qdss_trace_mode_req_msg_v01 req;
	wlfw_qdss_trace_mode_resp_msg_v01 resp;

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.mode_valid = 1;
	req.mode = mode;
	req.option_valid = 1;
	req.option = option;

	req.hw_trc_disable_override_valid = 1;
	req.hw_trc_disable_override = cnss_plat_get_qdss_cfg_hw_trc_override();

	wsvc_printf_dbg("%s: mode %u, option %llu, hw_trc_disable_override: %u",
			__func__, mode, option, req.hw_trc_disable_override);

	rc = qmi_client_send_msg_sync(gdata.clnt_handler,
				      QMI_WLFW_QDSS_TRACE_MODE_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);
	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("Failed to send QDSS trace mode req: rc %d, result %d, error %d",
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			rc = -resp.resp.result;
	}

	return rc;
}

int wlfw_qdss_trace_start(void)
{
	return wlfw_send_qdss_trace_mode_req(QMI_WLFW_QDSS_TRACE_ON_V01, 0);
}

int wlfw_qdss_trace_stop(unsigned long long option)
{
	return wlfw_send_qdss_trace_mode_req(QMI_WLFW_QDSS_TRACE_OFF_V01,
					     option);
}

int wlfw_send_qdss_trace_config_download_req(void)
{
	int rc = FALSE;
	wlfw_qdss_trace_config_download_req_msg_v01 req;
	wlfw_qdss_trace_config_download_resp_msg_v01 resp;
	FILE *file = NULL;
	unsigned char *buffer, *temp;
	unsigned int file_len, remaining;

	wsvc_printf_dbg("%s", __func__);

	file = fopen(CNSS_QDSS_TRACE_CONFIG_FILE_NEW, "rb");
	if (!file)
		file = fopen(CNSS_QDSS_TRACE_CONFIG_FILE_OLD, "rb");

	if (!file) {
		wsvc_printf_err("%s: Failed to find QDSS trace config file",
				__func__);
		rc = -1;
		goto end;
	}

	fseek(file, 0, SEEK_END);
	file_len = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = (unsigned char *)malloc(file_len);
	if (buffer == NULL) {
		wsvc_printf_err("%s: Fail to alloc memory for QDSS config file",
					 __func__);
		rc = -1;
		goto memory_alloc_fail;
	}
	if (fread(buffer, 1, file_len, file) != file_len) {
		wsvc_printf_err("%s: Fail to read QDSS config file", __func__);
		rc = -1;
		goto read_fail;
	}
	fclose(file);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	wsvc_printf_dbg("%s: size %u", __func__, file_len);

	temp = buffer;
	remaining = file_len;
	while (remaining) {
		req.total_size_valid = 1;
		req.total_size = file_len;
		req.seg_id_valid = 1;
		req.data_valid = 1;
		req.end_valid = 1;
		if (remaining > QMI_WLFW_MAX_DATA_SIZE_V01) {
			req.data_len = QMI_WLFW_MAX_DATA_SIZE_V01;
		} else {
			req.data_len = remaining;
			req.end = 1;
		}
		memcpy(req.data, temp, req.data_len);

		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
			QMI_WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_REQ_V01,
			&req, sizeof(req), &resp, sizeof(resp),
			WLFW_TIMEOUT_MS);
		if ((rc != QMI_NO_ERR) ||
		    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);
			if (resp.resp.result)
				rc = -resp.resp.result;
			goto out;
		}

		remaining -= req.data_len;
		temp += req.data_len;
		req.seg_id++;
	}

out:
	free(buffer);
	buffer = NULL;
	return rc;
read_fail:
	free(buffer);
	buffer = NULL;
memory_alloc_fail:
	fclose(file);
end:
	return rc;
}

static void wlfw_handle_initiate_cal_download_ind(void *ind_buf,
					 unsigned int ind_buf_len)
{
	qmi_client_error_type rc = QMI_NO_ERR;
	wlfw_initiate_cal_download_ind_msg_v01 decoded_ind_buf;
	struct cnss_evt *evt;

	wsvc_printf_dbg("%s", __func__);

	memset(&decoded_ind_buf, 0, sizeof(decoded_ind_buf));
	rc = qmi_client_message_decode(gdata.clnt_handler,
				       QMI_IDL_INDICATION,
				       QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01,
				       ind_buf,
				       ind_buf_len,
				       &decoded_ind_buf,
				       sizeof(decoded_ind_buf));
	daemon_qmihist_record(QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01, rc);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("%s: fail to decode msg", __func__);
		return;
	}

	evt = cnss_evt_alloc(QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01,
			     &decoded_ind_buf, sizeof(decoded_ind_buf));
	if (!evt) {
		wsvc_printf_err("%s: failed to alloc memory for evt",
				__func__);
		return;
	}
	cnss_evt_enqueue(&gdata.evt_q, evt);

	pthread_mutex_lock(&(gdata.mutex));
	gdata.cond_signaled = true;
	pthread_cond_signal(&(gdata.cond));
	pthread_mutex_unlock(&(gdata.mutex));

	return;
}

static void wlfw_handle_initiate_cal_update_ind(void *ind_buf,
				       unsigned int ind_buf_len)
{
	qmi_client_error_type rc;
	wlfw_initiate_cal_update_ind_msg_v01 decoded_ind_buf;
	struct cnss_evt *evt;

	wsvc_printf_dbg("%s", __func__);

	memset(&decoded_ind_buf, 0, sizeof(decoded_ind_buf));
	rc = qmi_client_message_decode(gdata.clnt_handler,
				       QMI_IDL_INDICATION,
				       QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01,
				       ind_buf,
				       ind_buf_len,
				       &decoded_ind_buf,
				       sizeof(decoded_ind_buf));
	daemon_qmihist_record(QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01, rc);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("%s: fail to decode msg", __func__);
		return;
	}

	evt = cnss_evt_alloc(QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01,
			     &decoded_ind_buf, sizeof(decoded_ind_buf));
	if (!evt) {
		wsvc_printf_err("%s: failed to alloc memory for evt",
				__func__);
		return;
	}
	cnss_evt_enqueue(&gdata.evt_q, evt);

	pthread_mutex_lock(&(gdata.mutex));
	gdata.cond_signaled = true;
	pthread_cond_signal(&(gdata.cond));
	pthread_mutex_unlock(&(gdata.mutex));

	return;
}

static void wlfw_handle_qdss_trace_save_ind(void *ind_buf,
					    unsigned int ind_buf_len)
{
	qmi_client_error_type rc;
	wlfw_qdss_trace_save_ind_msg_v01 decoded_ind_buf;
	struct cnss_evt *evt;

	wsvc_printf_dbg("%s", __func__);

	memset(&decoded_ind_buf, 0, sizeof(decoded_ind_buf));
	rc = qmi_client_message_decode(gdata.clnt_handler,
				       QMI_IDL_INDICATION,
				       QMI_WLFW_QDSS_TRACE_SAVE_IND_V01,
				       ind_buf,
				       ind_buf_len,
				       &decoded_ind_buf,
				       sizeof(decoded_ind_buf));
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("%s: fail to decode msg", __func__);
		return;
	}

	evt = cnss_evt_alloc(QMI_WLFW_QDSS_TRACE_SAVE_IND_V01,
			     &decoded_ind_buf, sizeof(decoded_ind_buf));
	if (!evt) {
		wsvc_printf_err("%s: failed to alloc memory for evt",
				__func__);
		return;
	}
	cnss_evt_enqueue(&gdata.evt_q, evt);

	pthread_mutex_lock(&(gdata.mutex));
	gdata.cond_signaled = true;
	pthread_cond_signal(&(gdata.cond));
	pthread_mutex_unlock(&(gdata.mutex));
}

static void wlfw_handle_xo_cal_ind(void *ind_buf, unsigned int ind_buf_len)
{
	wlfw_xo_cal_ind_msg_v01 decoded_ind_buf;
	qmi_client_error_type rc = QMI_NO_ERR;
	int ret;

	wsvc_printf_err("%s received xo cal indication", __func__);

	memset(&decoded_ind_buf, 0, sizeof(decoded_ind_buf));

	rc = qmi_client_message_decode(gdata.clnt_handler,
				       QMI_IDL_INDICATION,
				       QMI_WLFW_XO_CAL_IND_V01,
				       ind_buf,
				       ind_buf_len,
				       &decoded_ind_buf,
				       sizeof(decoded_ind_buf));
	daemon_qmihist_record(QMI_WLFW_XO_CAL_IND_V01, rc);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("%s: fail to decode xo-cal msg, rc %d", __func__, rc);
		return;
	}
	wsvc_printf_err("%s save xo cal file in persist path", __func__);
	ret = cnss_plat_save_file(PERSIST_XO_CAL_FILE, &decoded_ind_buf.xo_cal_data,
		       sizeof(decoded_ind_buf.xo_cal_data), false);
	if (ret <= 0) {
		wsvc_printf_err("%s save xo cal file in data path", __func__);
		cnss_plat_save_file(DATA_XO_CAL_FILE, &decoded_ind_buf.xo_cal_data,
			sizeof(decoded_ind_buf.xo_cal_data), false);
	}
}

static void wlfw_qmi_ind_cb(qmi_client_type user_handle,
			    unsigned int msg_id,
			    void *ind_buf,
			    unsigned int ind_buf_len,
			    void *ind_cb_data)
{
	if ((ind_buf_len > 0) && (NULL == user_handle || NULL == ind_buf)) {
		wsvc_printf_err("%s: Invalid parameter(s) user_handle %pK:"
			"ind_buf %pK: ind_buf_len %d", __func__, user_handle,
			ind_buf, ind_buf_len);
		return;
	}
	(void)ind_cb_data;

	wsvc_printf_err("%s: Receive Ind : 0x%x", __func__, msg_id);

	switch (msg_id) {
	case QMI_WLFW_FW_READY_IND_V01:
		/* turn off Adrastea */
		daemon_qmihist_record(QMI_WLFW_FW_READY_IND_V01, 0);
		break;
	case QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01:
		wlfw_handle_initiate_cal_download_ind(ind_buf,
					     ind_buf_len);
		break;
	case QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01:
		wlfw_handle_initiate_cal_update_ind(ind_buf,
					   ind_buf_len);
		break;
	case QMI_WLFW_MSA_READY_IND_V01:
		wsvc_printf_dbg("%s: Received MSA Ready Ind", __func__);
		pthread_mutex_lock(&gdata.mutex);
		gdata.state |= CNSS_MSA_READY;
		pthread_cond_signal(&gdata.cond_rsp);
		pthread_mutex_unlock(&gdata.mutex);
		break;
	case QMI_WLFW_FW_MEM_READY_IND_V01:
		wsvc_printf_dbg("%s: Received FW memory ready indication",
				__func__);
		daemon_qmihist_record(QMI_WLFW_FW_MEM_READY_IND_V01, 0);
		pthread_mutex_lock(&gdata.mutex);
		gdata.state |= CNSS_FW_MEM_READY;
		pthread_cond_signal(&gdata.cond_rsp);
		pthread_mutex_unlock(&gdata.mutex);
		break;
	case QMI_WLFW_QDSS_TRACE_SAVE_IND_V01:
		wlfw_handle_qdss_trace_save_ind(ind_buf,
						ind_buf_len);
		break;
	case QMI_WLFW_XO_CAL_IND_V01:
		wlfw_handle_xo_cal_ind(ind_buf, ind_buf_len);
		break;
	default:
		wsvc_printf_err("%s: Unsupported Ind %u",
				__func__, msg_id);
		break;
	}
}

static void wlfw_qmi_err_cb(qmi_client_type user_handle,
			    qmi_client_error_type error,
			    void *err_cb_data)
{
	int rc;

	if (user_handle == NULL) {
		wsvc_printf_err("%s: Invalid user_handle", __func__);
		return;
	}
	(void)err_cb_data;

	wsvc_printf_info_high("%s: Server disconnect, err %d client %pK",
			 __func__, error, (void *)user_handle);

	rc = wlfw_stop(SVC_DISCONNECTED);
	if (rc != 0) {
		wsvc_printf_err("%s: Fail to stop wlfw client", __func__);
		return;
	}

	gdata.state = 0;

	rc = wlfw_start(SVC_RECONNECT);
	if (rc != 0) {
		wsvc_printf_err("%s: Fail to re-start wlfw client", __func__);
		return;
	}
}

void wlfw_handle_ind_data(struct cnss_evt *evt)
{
	wsvc_printf_dbg("%s", __func__);

	if (!evt || !evt->data) {
		wsvc_printf_err("%s: Invalid data", __func__);
		return;
	}

	if ((CNSS_IS_WLFW_QMI_CONNECTED(gdata.state)) == 0) {
		wsvc_printf_err("%s: service disconnected, "
				"ignor this ind_data", __func__);
		return;
	}

	wsvc_printf_dbg("%s: msg_id 0x%d ",__func__, evt->msg_id);
	switch (evt->msg_id) {
	case QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01:
		wlfw_send_cal_download_req(evt->data);
		break;
	case QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01:
		wlfw_send_cal_update_req(evt->data);
		break;
	case QMI_WLFW_QDSS_TRACE_SAVE_IND_V01:
		wlfw_send_qdss_trace_data_req(evt->data);
		break;
	default:
		wsvc_printf_dbg("%s: Invalid msg_id 0x%x",
				__func__, evt->msg_id);
		break;
	}
}

void wlfw_build_cal_table(void)
{
	char filename[CNSS_MAX_FILE_PATH];
	int len_read;
	unsigned char *pcal = NULL;
	int id;

	pthread_mutex_lock(&(gdata.cal_mutex));
	for (id = 0; id < QMI_WLFW_MAX_NUM_CAL_V01; id++) {
		snprintf(filename, sizeof(filename), CNSS_CAL_FILE"%02d.bin",
			 id);
		if ((len_read = cnss_plat_read_file(filename, &pcal)) <= 0) {
			wsvc_printf_err("%s: not read %s", __func__, filename);
			gdata.cal_data_array[id].cal_id = 0;
			gdata.cal_data_array[id].total_size = 0;
			if (gdata.cal_data_array[id].data) {
				free(gdata.cal_data_array[id].data);
				gdata.cal_data_array[id].data = NULL;
			}
			continue;
		}
		gdata.cal_data_array[id].cal_id = id;
		gdata.cal_data_array[id].total_size = len_read;
		/* Free old cal data if any for the ID */
		if (gdata.cal_data_array[id].data)
			free(gdata.cal_data_array[id].data);
		gdata.cal_data_array[id].data = pcal;
	}
	pthread_mutex_unlock(&(gdata.cal_mutex));
	return;
}

static int wlfw_hawkeye_init(void)
{
	int rc;
	uint64_t fw_status = 0;

	dms_running = 0;

	wlfw_build_cal_table();

	rc = wlfw_send_ind_register_req(&fw_status);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to send indication register request!");
		return rc;
	}

	if (fw_status & QMI_WLFW_ALREADY_REGISTERED_V01) {
		wsvc_printf_info("Already registered to FW, FW status: 0x%" PRIx64,
				 fw_status);

		if (fw_status & QMI_WLFW_FW_MEM_READY_V01) {
			wsvc_printf_dbg("FW memory is ready");
			gdata.state |= CNSS_FW_MEM_READY;
		}

		if (fw_status & QMI_WLFW_FW_READY_V01) {
			wsvc_printf_info("FW is already ready, skip!");
			goto out;
		}
	}

	pthread_mutex_lock(&gdata.mutex);
	if (!CNSS_IS_FW_MEM_READY(gdata.state)) {
		wsvc_printf_dbg("Wait for FW memory ready indication");
		pthread_cond_wait(&gdata.cond_rsp, &gdata.mutex);
	}
	pthread_mutex_unlock(&gdata.mutex);

	wlfw_send_cap_req();
	wlfw_send_cal_report_req();

out:
	return QMI_NO_ERR;
}

static int wlfw_moselle_init(void)
{
	int rc;
	uint64_t fw_status = 0;

	wlfw_build_cal_table();

	rc = wlfw_send_ind_register_req(&fw_status);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to send indication register request!");
		return rc;
	}

	wsvc_printf_info("FW status: 0x%" PRIx64, fw_status);

	if (fw_status & QMI_WLFW_FW_MEM_READY_V01) {
		wsvc_printf_dbg("FW memory is ready");
		gdata.state |= CNSS_FW_MEM_READY;
	}

	if (fw_status & QMI_WLFW_FW_READY_V01) {
		wsvc_printf_info("FW is already ready, skip!");
		goto out;
	}

	pthread_mutex_lock(&gdata.mutex);
	if (!CNSS_IS_FW_MEM_READY(gdata.state)) {
		wsvc_printf_dbg("Wait for FW memory ready indication");
		pthread_cond_wait(&gdata.cond_rsp, &gdata.mutex);
	}
	pthread_mutex_unlock(&gdata.mutex);

out:
#ifdef CONFIG_READ_NV_MAC_ADDR
	wsvc_printf_dbg("pthread_cond_signal dms_cond");
	pthread_mutex_lock(&(gdata.dms_mutex));
	gdata.state |= CNSS_BDF_DOWNLOAD_DONE;
	pthread_cond_signal(&(gdata.dms_cond));
	pthread_mutex_unlock(&(gdata.dms_mutex));
#endif
	return QMI_NO_ERR;
}

static int wlfw_napier_init(void)
{
	int rc;
	uint64_t fw_status = 0;

	pm_deinit(MDM_TYPE_INTERNAL);
	pm_deinit(MDM_TYPE_EXTERNAL);

	wlfw_build_cal_table();

	rc = wlfw_send_ind_register_req(&fw_status);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to send indication register request!");
		return rc;
	}

	wsvc_printf_info("FW status: 0x%" PRIx64, fw_status);

	if (fw_status & QMI_WLFW_FW_MEM_READY_V01) {
		wsvc_printf_dbg("FW memory is ready");
		gdata.state |= CNSS_FW_MEM_READY;
	}

	if (fw_status & QMI_WLFW_FW_READY_V01) {
		wsvc_printf_info("FW is already ready, skip!");
		goto out;
	}

	pthread_mutex_lock(&gdata.mutex);
	if (!CNSS_IS_FW_MEM_READY(gdata.state)) {
		wsvc_printf_dbg("Wait for FW memory ready indication");
		pthread_cond_wait(&gdata.cond_rsp, &gdata.mutex);
	}
	pthread_mutex_unlock(&gdata.mutex);

	rc = wlfw_send_cap_req();
	if (rc)
		return rc;

	wlfw_send_cal_report_req();

out:
#ifdef CONFIG_READ_NV_MAC_ADDR
	wsvc_printf_dbg("pthread_cond_signal dms_cond");
	pthread_mutex_lock(&(gdata.dms_mutex));
	gdata.state |= CNSS_BDF_DOWNLOAD_DONE;
	pthread_cond_signal(&(gdata.dms_cond));
	pthread_mutex_unlock(&(gdata.dms_mutex));
#endif
	return QMI_NO_ERR;
}

static int wlfw_adrastea_init(void)
{
	int rc;
	int16_t bdf_retry = 0;
	int16_t regdb_retry = 0;
	uint64_t fw_status = 0;

	wlfw_build_cal_table();

	rc = wlfw_send_ind_register_req(&fw_status);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to send indication register request!");
		return rc;
	}

	if (fw_status & QMI_WLFW_ALREADY_REGISTERED_V01) {
		if (!(fw_status & QMI_WLFW_FW_READY_V01)) {
			wsvc_printf_err("FW in bad state 0x%" PRIx64,
							fw_status);
			goto skip_mac_addr;
		}
		goto skip_cal_report;
	}

	if (fw_status & QMI_WLFW_MSA_READY_V01) {
		wsvc_printf_dbg("MSA is ready");
		gdata.state |= CNSS_MSA_READY;
	}

	pthread_mutex_lock(&gdata.mutex);
	if (!CNSS_IS_MSA_READY(gdata.state))
	{
		wsvc_printf_dbg("Wait for MSA ready Indication from FW");
		pthread_cond_wait(&gdata.cond_rsp, &gdata.mutex);
	}
	pthread_mutex_unlock(&gdata.mutex);

	rc = wlfw_send_cap_req();
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to send capability request");
		goto skip_mac_addr;
	}

	/* Do BDF REGDB retry */
	do {
		rc = wlfw_send_bdf_download_req(CNSS_BDF_REGDB);
	} while (regdb_retry++ < WLFW_MAX_BDF_RETRY && (FALSE == rc));

	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to Download the REGDB File");
	}

	/* Do BDF retry */
	do {
		rc = wlfw_send_bdf_download_req(CNSS_BDF_BIN);
	} while (bdf_retry++ < WLFW_MAX_BDF_RETRY && (FALSE == rc));

	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to Download the BDF File");
		gdata.state = 0;
		return rc;
	}

	rc = wlfw_send_cal_report_req();
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to send cal_report_request");
		goto skip_mac_addr;
	}

skip_cal_report:
	/* Intention here is to not intersect MAC address configuration and BDF
	 * download */
#ifdef CONFIG_READ_NV_MAC_ADDR
	wsvc_printf_dbg("pthread_cond_signal dms_cond");
	pthread_mutex_lock(&(gdata.dms_mutex));
	gdata.state |= CNSS_BDF_DOWNLOAD_DONE;
	pthread_cond_signal(&(gdata.dms_cond));
	pthread_mutex_unlock(&(gdata.dms_mutex));
#endif
skip_mac_addr:
	return rc;
}

void *wlfw_service_request(void *arg)
{
	int rc;
	qmi_client_type clnt;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	struct cnss_evt *evt = NULL;
	(void)arg;

	wsvc_printf_info_high("%s: Start the pthread: %pK", __func__, arg);

	/* release any old handles for qmi client */
	if (gdata.clnt_handler) {
		qmi_client_release(gdata.clnt_handler);
		gdata.clnt_handler = NULL;
	}

	gdata.svc_obj = wlfw_get_service_object_v01();

	rc = qmi_client_init_instance(gdata.svc_obj,
					QMI_CLIENT_INSTANCE_ANY,
					wlfw_qmi_ind_cb,
					NULL,
					&os_params,
					0,
					&clnt);

	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to init client");
		return NULL;
	}

	rc = qmi_client_register_error_cb(clnt,
					  wlfw_qmi_err_cb,
					  NULL);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to register error cb");
		goto release_client;
	}

	gdata.clnt_handler = clnt;
	gdata.state |= CNSS_WLFW_QMI_CONNECTED;

	wsvc_printf_info("WLFW service connected");

	rc = qmi_client_get_service_instance(gdata.svc_obj,
					     QMI_CLIENT_INSTANCE_ANY,
					     &info);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to get service instance!");
		goto release_client;
	}

	rc = qmi_client_get_instance_id(&info, &gdata.instance_id);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to get instance ID!");
		goto release_client;
	}

	if (gdata.instance_id == NAPIER_ID) {
		rc = wlfw_napier_init();
	} else if (gdata.instance_id == ADRASTEA_ID) {
		rc = wlfw_adrastea_init();
	} else if (gdata.instance_id == HAWKEYE_ID) {
		rc = wlfw_hawkeye_init();
	} else if (gdata.instance_id == MOSELLE_ID) {
		rc = wlfw_moselle_init();
	}

	if (rc != QMI_NO_ERR)
		goto unset_state;

	while (svc_running) {
		pthread_mutex_lock(&(gdata.mutex));
		while (!gdata.cond_signaled)
			pthread_cond_wait(&gdata.cond, &gdata.mutex);

		gdata.cond_signaled = false;
		pthread_mutex_unlock(&(gdata.mutex));

		if (!svc_running)
			break;

		evt = cnss_evt_dequeue(&gdata.evt_q);
		while (evt) {
			/* handle ind */
			wlfw_handle_ind_data(evt);
			if (evt->data)
				free(evt->data);
			free(evt);
			evt = cnss_evt_dequeue(&gdata.evt_q);
		}
	}

	wsvc_printf_info("%s: Pthread exiting", __func__);

	return NULL;

release_client:
	qmi_client_release(clnt);
unset_state:
	gdata.state &= ~CNSS_WLFW_QMI_CONNECTED;
	return NULL;
}

#ifdef CONFIG_READ_NV_MAC_ADDR
int dms_get_wlan_address(qmi_client_type clnt)
{
	int rc;
	dms_get_mac_address_req_msg_v01 req;
	dms_get_mac_address_resp_msg_v01 resp;

	wsvc_printf_dbg("Sending DMS get mac address");

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.device = DMS_DEVICE_MAC_WLAN_V01;

	rc = qmi_client_send_msg_sync(clnt,
				      QMI_DMS_GET_MAC_ADDRESS_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);
	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("Send DMS get mac address failed: rc %d, result %d, error %d",
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	wsvc_printf_dbg("Read MAC address: valid %u, len %u",
			resp.mac_address_valid,
			resp.mac_address_len);

	if (resp.mac_address_valid &&
	    resp.mac_address_len == QMI_WLFW_MAC_ADDR_SIZE_V01) {
		gdata.mac_addr.mac_addr_valid = 1;
		memcpy(gdata.mac_addr.mac_addr, resp.mac_address,
		       QMI_WLFW_MAC_ADDR_SIZE_V01);
		wsvc_printf_mac_addr(MSG_DEBUG, "Read MAC address",
				     gdata.mac_addr.mac_addr);
	} else {
		gdata.mac_addr.mac_addr_valid = 0;
		memset(gdata.mac_addr.mac_addr, 0,
		       sizeof(uint8_t) * QMI_WLFW_MAC_ADDR_SIZE_V01);
	}

out:
	return rc;
}
#else
int dms_get_wlan_address(qmi_client_type clnt __attribute__((unused)))
{
	return 0;
}
#endif

#ifdef CONFIG_READ_NV_MAC_ADDR
void *dms_service_request(void *arg)
{
	int rc;
	qmi_idl_service_object_type svc_obj;
	qmi_client_type clnt;
	qmi_cci_os_signal_type os_params;
	(void)arg;

	wsvc_printf_dbg("%s: Start DMS pthread: %pK", __func__, arg);

	if (!cnss_plat_get_dms_mac_addr_prov_support()) {
		wsvc_printf_err("DMS MAC Address Provisioning not supported");
		goto out;
	}

	svc_obj = dms_get_service_object_v01();

	do {
		rc = qmi_client_init_instance(svc_obj,
					      QMI_CLIENT_INSTANCE_ANY,
					      NULL,
					      NULL,
					      &os_params,
					      WLFW_TIMEOUT_DMS_MS,
					      &clnt);
		if (rc == QMI_NO_ERR) {
			break;
		} else if (rc != QMI_TIMEOUT_ERR) {
			wsvc_printf_err("Failed to init DMS client, ret %d",
					rc);
			goto out;
		}
	} while (dms_running);

	if (rc != QMI_NO_ERR) {
		wsvc_printf_info("Exit DMS pthread for instance_id %d ,ret %d\n",
				 gdata.instance_id, rc);
		goto out;
	}

	rc = dms_get_wlan_address(clnt);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to get WLAN MAC address");
	} else if (gdata.mac_addr.mac_addr_valid) {
		int retry = 0;
		pthread_mutex_lock(&(gdata.dms_mutex));
		if (!CNSS_IS_BDF_DOWNLOAD_DONE(gdata.state) && svc_running) {
			wsvc_printf_dbg("Wait for CNSS_BDF_DOWNLOAD_DONE");
			pthread_cond_wait(&gdata.dms_cond, &gdata.dms_mutex);
		}
		pthread_mutex_unlock(&(gdata.dms_mutex));
		for (retry = 0; retry < 2 && svc_running; retry++)
		{
			rc = wlfw_send_mac_addr_req();
			if (rc == QMI_NO_ERR)
				break;
			usleep(500000);
		}
	}

	qmi_client_release(clnt);
out:
	dms_running = 0;
	return NULL;
}
#else
void *dms_service_request(void *arg __attribute__((unused)))
{
	return NULL;
}
#endif

#ifdef CONFIG_READ_NV_MAC_ADDR
int pthread_initialize_dms(void)
{
	int rc = 0;

	rc = pthread_cond_init(&(gdata.dms_cond), NULL);
	if (rc != 0) {
		wsvc_printf_err("Failed to init dms_cond, ret %d", rc);
		goto err_fail_dms_cond_rsp;
	}
	rc = pthread_mutex_init(&(gdata.dms_mutex), NULL);
	if (rc != 0) {
		wsvc_printf_err("Failed to init dms_mutex, ret %d", rc);
		goto err_fail_dms_mutex_create;
	}
	rc = pthread_create(&(gdata.thread_id_dms), NULL,
			    dms_service_request, NULL);
	if (rc != 0) {
		wsvc_printf_err("Failed to create DMS thread, ret %d", rc);
		goto err_fail_thread_create;
	}
	dms_running = 1;
	return rc;

err_fail_thread_create:
	rc = pthread_mutex_destroy(&(gdata.dms_mutex));
	if (rc != 0)
		wsvc_printf_err("Failed to destroy dms_mutex, ret %d", rc);

err_fail_dms_mutex_create:
	rc = pthread_cond_destroy(&(gdata.dms_cond));
	if (rc != 0)
		wsvc_printf_err("Failed to destroy dms_cond, ret %d", rc);
err_fail_dms_cond_rsp:
	return rc;
}
#else
int pthread_initialize_dms(void)
{
	dms_running = 0;
	return 0;
}
#endif

#ifdef CAL_DATA_REMOVE
int remove_cal_data(void)
{
	int ret = 0;
	char filename[CNSS_MAX_FILE_PATH];
	int id;

	for (id = 0; id < QMI_WLFW_MAX_NUM_CAL_V01; id++) {

		snprintf(filename, sizeof(filename), CNSS_CAL_FILE"%02d.bin",
			 id);

		if (access(filename, F_OK) == 0) {
			ret = remove(filename);
			if (ret) {
				wsvc_printf_err("Not able to delete cal file %s",
						filename);
				break;
			}
		} else {
			wsvc_printf_info("%s cal file does not exists",
					 filename);
		}
	}

	return ret;
}
#else
int remove_cal_data(void)
{
	return 0;
}
#endif

int wlfw_start(enum wlfw_svc_flag flag)
{
	int rc = 0;

	wsvc_printf_info_high("%s: Starting", __func__);

	if (flag == SVC_START) {
		pm_init(MDM_TYPE_INTERNAL, NULL);
		pm_init(MDM_TYPE_EXTERNAL, "SDXPRAIRIE");
		if (remove_cal_data())
			 wsvc_printf_err("Unable to clear cal data");
	}

	rc = pthread_mutex_init(&(gdata.cal_mutex), NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to init cal_mutex, ret %d", rc);
		goto out;
	}

	rc = pthread_mutex_init(&(gdata.mutex), NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to init mutex, ret %d", rc);
		goto err_fail_cal_mutex;;
	}

#ifdef CONFIG_RECORD_DAEMON_QMI_LOG
	rc = pthread_mutex_init(&(qmi_record_mutex), NULL);
	if (rc != 0) {
		wsvc_printf_err("Failed to init qmi_record_mutex, ret %d", rc);
		goto err_fail_mutex;
	}
#endif
	gdata.cond_signaled = false;
	rc = pthread_cond_init(&(gdata.cond), NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to init cond, ret %d", rc);
		goto err_fail_cond;
	}
	rc = pthread_cond_init(&(gdata.cond_rsp), NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to init cond_rsp, ret %d", rc);
		goto err_fail_cond_rsp;
	}

	rc = pthread_initialize_dms();
	if (rc != 0)
		goto err_fail_thread_create;

	rc = pthread_create(&(gdata.thread_id), NULL,
			    wlfw_service_request, NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to create thread, ret %d", rc);
		goto err_fail_thread_create;
	}

#ifdef CONFIG_RECORD_DAEMON_QMI_LOG
	daemon_debuglog_file = open(debug_file, O_RDWR);
	if (daemon_debuglog_file < 0) {
		wsvc_printf_err("%s:Unable to open daemon_qmilog_file");
	}
#endif

	svc_running = 1;
	wsvc_printf_dbg("%s: Start done: %d", __func__, rc);

	return rc;
err_fail_thread_create:
	rc = pthread_cond_destroy(&(gdata.cond_rsp));
	if (0 != rc) {
		wsvc_printf_err("Failed to destroy cond_rsp, ret %d", rc);
	}
err_fail_cond_rsp:
	rc = pthread_cond_destroy(&(gdata.cond));
	if (0 != rc) {
		wsvc_printf_err("Failed to destroy cond, ret %d", rc);
	}
err_fail_cond:
#ifdef CONFIG_RECORD_DAEMON_QMI_LOG
	rc = pthread_mutex_destroy(&(qmi_record_mutex));
	if (rc != 0) {
		wsvc_printf_err("Failed to destroy qmi_record_mutex, ret %d", rc);
	}
err_fail_mutex:
#endif
	rc = pthread_mutex_destroy(&(gdata.mutex));
        if (0 != rc) {
		wsvc_printf_err("Failed to destroy mutex, ret %d", rc);
	}
err_fail_cal_mutex:
	rc = pthread_mutex_destroy(&(gdata.cal_mutex));
        if (0 != rc) {
		wsvc_printf_err("Failed to destroy cal_mutex, ret %d", rc);
	}
out:
	return rc;
}

#ifdef CONFIG_READ_NV_MAC_ADDR
void pthread_signal_and_join_dms(void)
{
	int rc = 0;

	wsvc_printf_dbg("Signal DMS client");
	pthread_mutex_lock(&(gdata.dms_mutex));
	pthread_cond_signal(&(gdata.dms_cond));
	pthread_mutex_unlock(&(gdata.dms_mutex));
	wsvc_printf_dbg("Join DMS thread");
	pthread_join(gdata.thread_id_dms, NULL);
	if (rc != 0) {
		wsvc_printf_err("Failed to join dms mutex, ret %d", rc);
	}
}
void pthread_cleanup_dms(void)
{
	int rc = 0;

	wsvc_printf_dbg("DMS thread cleanup");
	rc = pthread_cond_destroy(&(gdata.dms_cond));
	if (rc != 0) {
		wsvc_printf_err("Failed to destroy dms_cond, ret %d", rc);
	}
	rc = pthread_mutex_destroy(&(gdata.dms_mutex));
	if (rc != 0) {
		wsvc_printf_err("Failed to destroy dms_mutex, ret %d", rc);
	}
}
#else
void pthread_signal_and_join_dms(void)
{
}

void pthread_cleanup_dms(void)
{
}
#endif

int wlfw_stop(enum wlfw_svc_flag flag)
{
	int rc = 0;

	if(!svc_running) {
		wsvc_printf_err("wlfw thread not started");
		return -EALREADY;
	}

	wsvc_printf_dbg("%s: Stopping: %d", __func__, flag);

	svc_running = 0;
	dms_running = 0;
	pthread_mutex_lock(&(gdata.mutex));
	pthread_cond_signal(&(gdata.cond_rsp));

	gdata.cond_signaled = true;
	pthread_cond_signal(&(gdata.cond));
	pthread_mutex_unlock(&(gdata.mutex));

	rc = pthread_join(gdata.thread_id, NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to join mutex, ret %d", rc);
	}
	pthread_signal_and_join_dms();

	wsvc_printf_info_high("DMS thread joined succesfully\n");

	if (gdata.clnt_handler && (SVC_EXIT == flag)) {
		qmi_client_release(gdata.clnt_handler);
		gdata.clnt_handler = NULL;
	}

	cnss_evt_free_queue(&gdata.evt_q);

	pthread_cleanup_dms();

	rc = pthread_cond_destroy(&(gdata.cond_rsp));
	if (0 != rc) {
		wsvc_printf_err("Failed to destroy cond_rsp, ret %d", rc);
	}
	rc = pthread_cond_destroy(&(gdata.cond));
	if (0 != rc) {
		wsvc_printf_err("Failed to destroy cond, ret %d", rc);
	}

#ifdef CONFIG_RECORD_DAEMON_QMI_LOG
	rc = pthread_mutex_destroy(&(qmi_record_mutex));
	if (rc != 0) {
		wsvc_printf_err("Failed to destroy qmi_record_mutex, ret %d", rc);
	}
	if (daemon_debuglog_file >= 0)
		close(daemon_debuglog_file);
#endif
	rc = pthread_mutex_destroy(&(gdata.mutex));
	if (0 != rc) {
		wsvc_printf_err("Failed to destroy mutex, ret %d", rc);
	}

	if (flag == SVC_EXIT) {
		/* Free and clear cached CAL data */
		free_cal_file();
		if (gdata.instance_id == ADRASTEA_ID) {
			pm_deinit(MDM_TYPE_INTERNAL);
			pm_deinit(MDM_TYPE_EXTERNAL);
		}
	}

	rc = pthread_mutex_destroy(&(gdata.cal_mutex));
	if (0 != rc) {
		wsvc_printf_err("Failed to destroy cal_mutex, ret %d", rc);
	}

	wsvc_printf_info_high("%s: Stop done: %d", __func__, rc);

	return rc;
}
