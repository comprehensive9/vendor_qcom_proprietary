/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

#include <qmi_idl_lib_internal.h>
#ifdef ANDROID
#include <qmi_client_instance_defs.h>
#else
#include "qmi_client.h"
#endif

#include "debug.h"
#include "cnss_plat.h"
#include "cnss_plat_ipc_qmi.h"
#include "cnss_plat_ipc_service_v01.h"

#define CNSS_PLAT_IPC_QMI_EVENT "QMI_EVT"
#define MAX_EVENT_STR_SIZE 32
#define CNSS_PLAT_IPC_QMI_TIMEOUT_MS 10000
#define CNSS_PLAT_IPC_QMI_SERVER_TIMEOUT 5000

enum cnss_plat_ipc_qmi_fd {
	CNSS_PLAT_IPC_QMI_READ_FD = 0,
	CNSS_PLAT_IPC_QMI_WRITE_FD,
	CNSS_PLAT_IPC_QMI_PIPE_FD_MAX = 2
};

/**
 * struct cnss_plat_ipc_qmi_client_data: CNSS daemon QMI client connection data
 * @svc_obj: QMI service object
 * @svc_clnt_handler: QMI client service handle
 * @pipe_fd: PIPE IPC file description for passing data from QMI callback to
 *           cnss daemon main processing loop
 * @evt_q: Queue for storing incoming QMI indication messages via callback
 */
struct cnss_plat_ipc_qmi_client_data {
	qmi_idl_service_object_type svc_obj;
	qmi_client_type svc_clnt_handler;
	int pipe_fd[CNSS_PLAT_IPC_QMI_PIPE_FD_MAX];
	struct cnss_evt_queue evt_q;
};

static struct cnss_plat_ipc_qmi_client_data plat_ipc_qmi;

static int cnss_plat_ipc_qmi_access_file(char *filename)
{
	if (access(filename, R_OK | W_OK) == -1) {
		wsvc_printf_dbg("%s: No such file %s", __func__, filename);
		return -1;
	}
	return 0;
}

static uint32_t cnss_plat_ipc_qmi_find_all_cal_file(void)
{
	char fname[CNSS_MAX_FILE_PATH +
		   CNSS_PLAT_IPC_QMI_MAX_FILE_NAME_LEN_V01 + 1];
	int id;
	uint32_t cal_file_bitmask = 0;

	for (id = 0; id < QMI_WLFW_MAX_NUM_CAL_V01; id++) {
		snprintf(fname, sizeof(fname), CNSS_CAL_FILE"%02d.bin", id);
		if (cnss_plat_ipc_qmi_access_file(fname) <  0)
			continue;
		cal_file_bitmask |= (uint32_t)1 << id;
	}
	return cal_file_bitmask;
}

static int cnss_plat_ipc_qmi_send_init_setup_req(void)
{
	cnss_plat_ipc_qmi_init_setup_req_msg_v01 req;
	cnss_plat_ipc_qmi_init_setup_resp_msg_v01 resp;
	int ret;

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.dms_mac_addr_supported = cnss_plat_get_dms_mac_addr_prov_support();
	req.qdss_hw_trace_override = cnss_plat_get_qdss_cfg_hw_trc_override();
	req.cal_file_available_bitmask = cnss_plat_ipc_qmi_find_all_cal_file();

	ret = qmi_client_send_msg_sync(plat_ipc_qmi.svc_clnt_handler,
				      CNSS_PLAT_IPC_QMI_INIT_SETUP_REQ_V01,
				      &req, sizeof(req), &resp, sizeof(resp),
				      CNSS_PLAT_IPC_QMI_TIMEOUT_MS);

	if ((ret != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("%s: ret %d, result %d, error %d",
				__func__, ret, resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			ret = -resp.resp.result;
		goto out;
	}

	wsvc_printf_info("%s: Driver Status: %lu", __func__, resp.drv_status);
out:
	return ret;
}

static int cnss_plat_ipc_qmi_download_file(char *file_name, uint32_t file_id)
{
	unsigned char *file_buf = NULL, *tmp_buf;
	char fname[CNSS_MAX_FILE_PATH +
		   CNSS_PLAT_IPC_QMI_MAX_FILE_NAME_LEN_V01 + 1];
	int file_len, ret = 0, seg_index = 0, seg_size;
	cnss_plat_ipc_qmi_file_download_req_msg_v01 *req;
	cnss_plat_ipc_qmi_file_download_resp_msg_v01 resp;

	if (!file_name)
		return -EINVAL;

	req = calloc(1, sizeof(*req));
	if (!req) {
		wsvc_printf_err("%s: No memory for file download request",
				__func__);
		return -ENOMEM;
	}

	memset(&resp, 0, sizeof(resp));

	snprintf(fname, sizeof(fname), "/data/vendor/wifi/%s", file_name);
	file_len = cnss_plat_read_file(fname, &file_buf);
	if (!file_buf) {
		wsvc_printf_err("%s: Download file Invalid: %s err: %d",
				__func__, fname, file_len);
		goto fileinv;
	}

	tmp_buf = file_buf;
	req->file_size = file_len;
	req->file_id = file_id;

	wsvc_printf_info("%s: Download file: %s ID: %d Size: %d", __func__,
			 file_name, file_id, file_len);
	while (file_len > 0) {
		seg_size = (file_len >= CNSS_PLAT_IPC_QMI_MAX_DATA_SIZE_V01 ?
			    CNSS_PLAT_IPC_QMI_MAX_DATA_SIZE_V01 : file_len);
		req->seg_index = seg_index++;
		req->seg_buf_len = seg_size;
		memcpy(req->seg_buf, tmp_buf, seg_size);
		file_len -= seg_size;
		req->end = (file_len > 0 ? 0 : 1);
		wsvc_printf_dbg("%s: File seg ID: %d Size: %d End: %u",
				__func__, req->seg_index, req->seg_buf_len,
				req->end);
		ret =
		qmi_client_send_msg_sync(plat_ipc_qmi.svc_clnt_handler,
					 CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_REQ_V01,
					 req, sizeof(*req), &resp, sizeof(resp),
					 CNSS_PLAT_IPC_QMI_TIMEOUT_MS);

		if ((ret != QMI_NO_ERR) ||
		    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
			wsvc_printf_err("%s: ret %d, result %d, error %d",
					__func__, ret, resp.resp.result,
					resp.resp.error);
			if (resp.resp.result)
				ret = -resp.resp.result;
			free(file_buf);
			goto out;
		}
		tmp_buf += seg_size;
	}
	free(file_buf);
	goto out;
fileinv:
	req->file_size = 0;
	req->file_id = file_id;
	req->seg_index = 0;
	req->seg_buf_len = 0;
	req->end = 1;
	ret = qmi_client_send_msg_sync(plat_ipc_qmi.svc_clnt_handler,
				       CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_REQ_V01,
				       req, sizeof(*req), &resp, sizeof(resp),
				       CNSS_PLAT_IPC_QMI_TIMEOUT_MS);

	if ((ret != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("%s: ret %d, result %d, error %d",
				__func__, ret, resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			ret = -resp.resp.result;
		free(file_buf);
		goto out;
	}
out:
	free(req);
	return ret;
}

static void cnss_plat_ipc_qmi_file_download_ind(struct cnss_evt *evt)
{
	cnss_plat_ipc_qmi_file_download_ind_msg_v01 *ind_msg;
	int ret;

	ind_msg = malloc(sizeof(*ind_msg));
	if (!ind_msg) {
		wsvc_printf_err("%s: No memory for file download ind decode",
				__func__);
		return;
	}

	ret = qmi_client_message_decode(plat_ipc_qmi.svc_clnt_handler,
					QMI_IDL_INDICATION,
					CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_IND_V01,
					evt->data, evt->data_len, ind_msg,
					sizeof(*ind_msg));

	if (ret < 0) {
		wsvc_printf_err("%s: Message decode failed ID: %d len: %d err: %d",
				__func__, evt->msg_id, evt->data_len, ret);
		goto out;
	}

	wsvc_printf_dbg("%s: Name: %s ID: %d\n", __func__,
			ind_msg->file_name, ind_msg->file_id);

	ret = cnss_plat_ipc_qmi_download_file(ind_msg->file_name,
					      ind_msg->file_id);
	if(ret < 0)
		wsvc_printf_err("File download failed err: %d", ret);
out:
	free(ind_msg);
}

static int cnss_plat_ipc_qmi_upload_file(char *file_name, uint32_t file_id,
					 uint32_t file_len)
{
	unsigned char *file_buf, *tmp_buf;
	char fname[CNSS_MAX_FILE_PATH +
		   CNSS_PLAT_IPC_QMI_MAX_FILE_NAME_LEN_V01 + 1];
	uint8_t end = 0;
	int ret, seg_index = 0;
	uint32_t tmp_file_size = 0;
	cnss_plat_ipc_qmi_file_upload_req_msg_v01 req;
	cnss_plat_ipc_qmi_file_upload_resp_msg_v01 *resp;

	if (!file_name)
		return -EINVAL;

	resp = calloc(1, sizeof(*resp));
	if (!resp) {
		wsvc_printf_err("%s: No memory for file upload response",
				__func__);
		ret = -ENOMEM;
		goto end;
	}

	if (file_len == 0) {
		wsvc_printf_err("%s: Upload file Invalid: %s", __func__,
				file_name);
		ret = -EINVAL;
		free(resp);
		goto end;
	}

	file_buf = (unsigned char *)malloc(file_len);
	if (!file_buf) {
		wsvc_printf_err("%s: Failed to alloc mem for file: %s",
				__func__, file_name);
		ret = -ENOMEM;
		free(resp);
		goto end;
	}

	tmp_buf = file_buf;
	tmp_file_size = file_len;
	req.file_id = file_id;

	wsvc_printf_info("%s: Upload file %s ID: %d Size: %d", __func__,
			 file_name, file_id, file_len);
	while ((file_len > 0) && (end == 0)) {
		req.seg_index = seg_index++;
		ret =
		qmi_client_send_msg_sync(plat_ipc_qmi.svc_clnt_handler,
					 CNSS_PLAT_IPC_QMI_FILE_UPLOAD_REQ_V01,
					 &req, sizeof(req), resp, sizeof(*resp),
					 CNSS_PLAT_IPC_QMI_TIMEOUT_MS);

		if ((ret != QMI_NO_ERR) ||
		    (resp->resp.result != QMI_RESULT_SUCCESS_V01)) {
			wsvc_printf_err("%s: ret %d, result %d, error %d",
					__func__, ret, resp->resp.result,
					resp->resp.error);
			if (resp->resp.result)
				ret = -resp->resp.result;
			goto out;
		}

		if ((resp->file_id == file_id) ||
		    (resp->seg_index == seg_index)) {
			wsvc_printf_dbg("%s: File seg ID: %d Size: %d End: %u",
					__func__, resp->seg_index,
					resp->seg_buf_len, resp->end);
			memcpy(tmp_buf, resp->seg_buf, resp->seg_buf_len);
		} else {
			wsvc_printf_err("%s: Unmatched file data. Expected file id:%d, seg_index:%d, Received file id:%u,seg_index:%u",
					__func__, file_id, seg_index,
					resp->file_id, resp->seg_index);
			ret = -1;
			goto out;
		}
		file_len -= resp->seg_buf_len;
		tmp_buf += resp->seg_buf_len;
		end = resp->end;
	}

	if ((file_len == 0) && (resp->end)) {
		snprintf(fname, sizeof(fname), "/data/vendor/wifi/%s",
			 file_name);
		cnss_plat_save_file(fname, file_buf, tmp_file_size, false);
	} else {
		wsvc_printf_err("%s: File corrupted. Remaining:%u End:%u",
				__func__, file_len, resp->end);
		ret = -1;
	}
out:
	free(file_buf);
	free(resp);
end:
	return ret;
}

static void cnss_plat_ipc_qmi_file_upload_ind(struct cnss_evt *evt)
{
	cnss_plat_ipc_qmi_file_upload_ind_msg_v01 *ind_msg;
	int ret;

	ind_msg = malloc(sizeof(*ind_msg));
	if (!ind_msg) {
		wsvc_printf_err("%s: No memory for file upload ind decode",
				__func__);
		return;
	}

	ret = qmi_client_message_decode(plat_ipc_qmi.svc_clnt_handler,
				       QMI_IDL_INDICATION,
				       CNSS_PLAT_IPC_QMI_FILE_UPLOAD_IND_V01,
				       evt->data,
				       evt->data_len,
				       ind_msg,
				       sizeof(*ind_msg));

	if (ret < 0) {
		wsvc_printf_err("%s: Message decode failed ID: %d len: %d err: %d",
				__func__, evt->msg_id, evt->data_len, ret);
		goto out;
	}

	wsvc_printf_dbg("%s: Name: %s ID: %d Size: %d\n", __func__,
			ind_msg->file_name, ind_msg->file_id,
			ind_msg->file_size);

	ret = cnss_plat_ipc_qmi_upload_file(ind_msg->file_name,
					    ind_msg->file_id,
					    ind_msg->file_size);
	if(ret < 0)
		wsvc_printf_err("%s: File upload failed err: %d", __func__,
				ret);
out:
	free(ind_msg);
}

/**
 * cnss_plat_ipc_qmi_msg_process(): Daemon main processing for QMI indications
 */
void cnss_plat_ipc_qmi_msg_process(void)
{
	struct cnss_evt *evt;
	char event[MAX_EVENT_STR_SIZE + 1];
	int n;

	n = read(plat_ipc_qmi.pipe_fd[0], event, MAX_EVENT_STR_SIZE);
	event[MAX_EVENT_STR_SIZE] = '\0';
	wsvc_printf_info("%s: RX %s", __func__, event);

	evt = cnss_evt_dequeue(&plat_ipc_qmi.evt_q);
	while (evt) {
		wsvc_printf_dbg("%s: Processing CNSS Platform QMI Indication ID: %d",
				__func__, evt->msg_id);
		switch (evt->msg_id) {
		case CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_IND_V01:
			cnss_plat_ipc_qmi_file_download_ind(evt);
			break;
		case CNSS_PLAT_IPC_QMI_FILE_UPLOAD_IND_V01:
			cnss_plat_ipc_qmi_file_upload_ind(evt);
			break;
		default:
			wsvc_printf_err("%s: Unhandled CNSS Platform QMI Indication ID: %d",
					__func__, evt->msg_id);
		}
		cnss_evt_free(evt);
		evt = cnss_evt_dequeue(&plat_ipc_qmi.evt_q);
	}
	wsvc_printf_dbg("%s: Processing complete", __func__);
}

/**
 * cnss_plat_ipc_qmi_trigger_process(): Trigger QMI pipe file for daemon main
 *                                       processing. It is done in QMI callback
 *                                       to switch context to daemon.
 */
void cnss_plat_ipc_qmi_trigger_process(void)
{
	write(plat_ipc_qmi.pipe_fd[CNSS_PLAT_IPC_QMI_WRITE_FD],
	      CNSS_PLAT_IPC_QMI_EVENT, strlen(CNSS_PLAT_IPC_QMI_EVENT));
}

/**
 * cnss_plat_ipc_qmi_ind_cb: QMI indication callback
 */
static void cnss_plat_ipc_qmi_ind_cb(qmi_client_type qmi_handle,
					unsigned int msg_id, void *data,
					unsigned int data_len, void *cb_data)
{
	UNUSED(cb_data);
	struct cnss_evt *evt;

	if (!qmi_handle || !data || data_len <= 0) {
		wsvc_printf_err("%s: Invalid QMI Indication", __func__);
		return;
	}

	wsvc_printf_info("%s: Msg_Id: %d Len: %d", __func__, msg_id, data_len);

	evt = cnss_evt_alloc(msg_id, data, data_len);
	if (!evt) {
		wsvc_printf_err("%s: Unable to queue QMI Indication.MSG_ID: %d",
				__func__, msg_id);
		return;
	}

	cnss_evt_enqueue(&plat_ipc_qmi.evt_q, evt);
	cnss_plat_ipc_qmi_trigger_process();
}

static void cnss_plat_ipc_qmi_err_cb(qmi_client_type user_handle,
			    qmi_client_error_type error,
			    void *err_cb_data)
{
	UNUSED(err_cb_data);
	if (!user_handle) {
		wsvc_printf_err("%s: Invalid user handle", __func__);
		return;
	}

	wsvc_printf_err("%s: CNSS Platform QMI disconnected: %d", __func__,
			error);
	cnss_plat_ipc_qmi_deinit();
}

int cnss_plat_ipc_qmi_init(void)
{
	int ret;
	qmi_client_type clnt;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	qmi_service_instance instance_id;

	if (pipe(plat_ipc_qmi.pipe_fd) < 0) {
		wsvc_printf_err("%s: Failed to create QMI IPC file descriptors: %d",
				__func__, errno);
		return -1;
	}
	plat_ipc_qmi.svc_obj = cnss_platform_get_service_object_v01( );
	ret = qmi_client_init_instance(plat_ipc_qmi.svc_obj,
					QMI_CLIENT_INSTANCE_ANY,
					cnss_plat_ipc_qmi_ind_cb,
					NULL, &os_params,
					CNSS_PLAT_IPC_QMI_SERVER_TIMEOUT,
					&clnt);

	if (ret != QMI_NO_ERR) {
		wsvc_printf_err("%s: Failed to init Platform QMI client",
				 __func__);
		return -EINVAL;
	}

	ret = qmi_client_register_error_cb(clnt, cnss_plat_ipc_qmi_err_cb,
					  NULL);
	if (ret != QMI_NO_ERR) {
		wsvc_printf_err("%s: Failed to register Platform QMI error cb",
				__func__);
		goto release_client;
	}

	plat_ipc_qmi.svc_clnt_handler = clnt;
	ret = qmi_client_get_service_instance(plat_ipc_qmi.svc_obj,
					     QMI_CLIENT_INSTANCE_ANY,
					     &info);
	if (ret != QMI_NO_ERR) {
		wsvc_printf_err("%s: Failed to get Platform QMI service instance",
				__func__);
		goto release_client;
	}

	ret = qmi_client_get_instance_id(&info, &instance_id);
	if (ret != QMI_NO_ERR) {
		wsvc_printf_err("%s: Failed to get Platform QMI instance ID",
				__func__);
		goto release_client;
	}
	wsvc_printf_info("CNSS Platform QMI service connected. Instance ID: %d",
			 instance_id);

	ret = cnss_plat_ipc_qmi_send_init_setup_req();
	return ret;

release_client:
	qmi_client_release(clnt);
	return -EINVAL;
}

void cnss_plat_ipc_qmi_deinit(void)
{
	qmi_client_release(plat_ipc_qmi.svc_clnt_handler);
	cnss_evt_free_queue(&plat_ipc_qmi.evt_q);
}

/**
 * cnss_plat_ipc_qmi_get_fd(): Get read side pipe file descriptor
 */
int cnss_plat_ipc_qmi_get_fd(void)
{
	return plat_ipc_qmi.pipe_fd[CNSS_PLAT_IPC_QMI_READ_FD];
}
