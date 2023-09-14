/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag communication support

GENERAL DESCRIPTION

Implementation of communication layer between diag library and diag driver.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include <stdlib.h>
#include "comdef.h"
#include "stdio.h"
#include "stringl.h"
#include "diag_lsmi.h"
#include "./../include/diag_lsm.h"
#include "diagsvc_malloc.h"
#include "diag_lsm_event_i.h"
#include "diag_lsm_log_i.h"
#include "diag_lsm_msg_i.h"
#include "diag.h" /* For definition of diag_cmd_rsp */
#include "diag_lsm_pkt_i.h"
#include "diag_lsm_dci_i.h"
#include "diag_lsm_dci.h"
#include "diag_shared_i.h" /* For different constants */
#include "diag_lsm_comm.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "errno.h"
#include <pthread.h>
#include <stdint.h>
#include <eventi.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/select.h>
#include <ctype.h>
#include <limits.h>
#include <netdb.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define DATA_TYPE_CMD_REG 5
#define DATA_TYPE_CMD_DEREG 6
#define DATA_TYPE_QUERY_REMOTE_DEV_MASK	51

int diag_use_dev_node = 0;
unsigned char read_buffer_comm[READ_BUF_SIZE];
int diag_lsm_comm_open(void)
{
	struct sockaddr_un addr;
	int fd = -1;
	int ret;

	fd = open("/dev/diag", O_RDWR | O_CLOEXEC);
	if (fd >= 0) {
		diag_use_dev_node = 1;
	} else {
		if (errno == ENOENT) {
			diag_use_dev_node = 0;
			fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
			if (fd < 0) {
				DIAG_LOGE(" Diag_LSM_Init: Failed to create socket");
				return fd;
			}
			memset(&addr, 0, sizeof(addr));
			addr.sun_family = AF_UNIX;
			strlcpy(addr.sun_path, "\0diag", sizeof(addr.sun_path)-1);
			ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
			if (ret < 0) {
				DIAG_LOGE("diag:failed to connect to diag socket\n");
				close(fd);
				return ret;
			}
			DIAG_LOGE("diag:successfully connected to socket %d\n", fd);
		}
	}
	return fd;
}

int diag_lsm_comm_write(int fd, unsigned char buf[], int bytes, int flags)
{
	int bytes_written = 0;

	if (diag_use_dev_node)
		return write(fd, (const void*)buf, bytes);

	bytes_written = send(fd, (const void *)buf, bytes, flags);
	if (bytes_written < 0)
		return -1;

	if (*(int *)buf == DCI_DATA_TYPE)
		return DIAG_DCI_NO_ERROR;

	return 0;
}

int diag_lsm_comm_ioctl(int fd, unsigned long request, void *buf, unsigned int len)
{
	unsigned char* ptr = NULL;
	int data_type, ret = 0;
	int i = 0, num_bytes,num_bytes_read;
	query_pd_feature_mask *params;

	if (diag_use_dev_node) {
		ret = ioctl(fd, request, buf, len, NULL, 0, NULL, NULL);
		return ret;
	} else {
		switch(request) {
		case DIAG_IOCTL_COMMAND_REG:
		{
			diag_cmd_reg_tbl_t *reg_tbl = buf;
			struct diag_cmd_tbl {
			int data_type;
			int count;
			diag_cmd_reg_entry_t entries[0];
			}__packed;
			struct diag_cmd_tbl *tbl ;
			num_bytes = (sizeof(diag_cmd_reg_entry_t) * reg_tbl->count)+ sizeof(data_type) + sizeof(reg_tbl->count);
			tbl = (struct diag_cmd_tbl*)malloc(num_bytes + 4);
			if (!tbl)
				return -ENOMEM;
			tbl->data_type = DATA_TYPE_CMD_REG;
			tbl->count = reg_tbl->count;
			for (i = 0; i < reg_tbl->count; i++) {
				tbl->entries[i].cmd_code = reg_tbl->entries[i].cmd_code;
				tbl->entries[i].subsys_id = reg_tbl->entries[i].subsys_id;
				tbl->entries[i].cmd_code_lo = reg_tbl->entries[i].cmd_code_lo;
				tbl->entries[i].cmd_code_hi = reg_tbl->entries[i].cmd_code_hi;
			}
			ret = write(fd, tbl, num_bytes);
			free(tbl);
			break;
		}
		case DIAG_IOCTL_COMMAND_DEREG:
		{
			data_type = DATA_TYPE_CMD_DEREG;
			ret = write(fd, &data_type, sizeof(data_type));
			break;
		}
		case DIAG_IOCTL_LSM_DEINIT:
		{
			data_type = DIAG_IOCTL_LSM_DEINIT;
			ret = write(fd, &data_type, sizeof(data_type));
			if (ret >= 0)
				return 1;
			else
				return ret;
			break;
		}
		case DIAG_IOCTL_GET_DELAYED_RSP_ID:
		{
			uint16 delayed_rsp_id;
			data_type = DIAG_IOCTL_GET_DELAYED_RSP_ID;
			ptr = malloc(sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			ret = write(fd, ptr, sizeof(data_type));
			free(ptr);
			num_bytes_read = read(fd, (void*)read_buffer_comm,
						READ_BUF_SIZE);
			delayed_rsp_id = read_buffer_comm[4];
			*(uint16 *)buf = delayed_rsp_id;
			break;
		}
		case DIAG_IOCTL_QUERY_PD_FEATUREMASK:
		{
			diag_hw_accel_query_sub_payload_rsp_t *query_params;
			if (!buf)
				return -EINVAL;
			query_params = (diag_hw_accel_query_sub_payload_rsp_t*)buf;
			data_type = DIAG_IOCTL_QUERY_PD_FEATUREMASK;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			num_bytes_read = read(fd, (void*)read_buffer_comm,
						READ_BUF_SIZE);
			if (num_bytes_read < 0)
				return num_bytes_read;
			params = (query_pd_feature_mask *)read_buffer_comm;
			query_params->diagid_mask_supported = params->diagid_mask_supported;
			query_params->diagid_mask_enabled = params->diagid_mask_enabled;
			break;
		}
		case DIAG_IOCTL_PASSTHRU_CONTROL:
		{
			diag_hw_accel_diag_id_mask *params;
			diag_hw_accel_cmd_req_t *req_params;
			if (!buf)
				return -EINVAL;
			req_params = (diag_hw_accel_cmd_req_t*)buf;
			data_type = DIAG_IOCTL_PASSTHRU_CONTROL;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			num_bytes_read = read(fd, (void*)read_buffer_comm,
								  READ_BUF_SIZE);
			if (num_bytes_read < 0)
				return num_bytes_read;
			params = (diag_hw_accel_diag_id_mask *)read_buffer_comm;
			req_params->op_req.diagid_mask = params->diagid_mask;
			if (params->ret_val < 0)
				ret = params->ret_val;
			break;
		}
		case DIAG_IOCTL_QUERY_CON_ALL:
		{
			data_type = DIAG_IOCTL_QUERY_CON_ALL;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			ret = wait_for_query_con_rsp(buf);
			break;
		}
		case DIAG_IOCTL_QUERY_MD_PID:
		{
			data_type = DIAG_IOCTL_QUERY_MD_PID;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			wait_for_query_md_pid_rsp(buf);
			break;
		}
		case DIAG_IOCTL_SWITCH_LOGGING:
		{
			struct logging_mode{
				int data_type;
				struct diag_logging_mode_param_t params;
				int pid;
			};
			struct logging_mode *param;
			data_type = DIAG_IOCTL_SWITCH_LOGGING;
			param = malloc(sizeof(struct logging_mode));
			if (!param)
				return -ENOMEM;
			param->data_type = data_type;
			memcpy(&param->params, buf, len);
			param->pid = getpid();
			ret = write(fd, param, sizeof(struct logging_mode));
			DIAG_LOGE("diag: wrote switch log pkt ret:%d\n", ret);
			if (ret < 0) {
				free(param);
				return ret;
			}
			free(param);
			ret = wait_for_switch_logging_rsp();
			return ret;
		}
		case DIAG_IOCTL_REGISTER_CALLBACK:
			data_type = DIAG_IOCTL_REGISTER_CALLBACK;
			ptr = malloc(sizeof(int) + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, sizeof(int));
			ret = write(fd, ptr, sizeof(int) + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			ret = wait_for_query_register_callback();
			return ret;
			break;
		case DIAG_IOCTL_REMOTE_DEV:
			data_type = DATA_TYPE_QUERY_REMOTE_DEV_MASK;
			ptr = malloc(sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			ret = write(fd, ptr, sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			wait_for_query_remote_device_mask(buf);
			break;
		case DIAG_IOCTL_VOTE_REAL_TIME:
			data_type = DIAG_IOCTL_VOTE_REAL_TIME;
			len = sizeof(struct real_time_vote_t);
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			break;
		case DIAG_IOCTL_GET_REAL_TIME:
			data_type = DIAG_IOCTL_GET_REAL_TIME;
			len = sizeof(struct real_time_query_t);
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			wait_for_query_real_time(buf);
			break;
		case DIAG_IOCTL_BUFFERING_DRAIN_IMMEDIATE:
			data_type = DIAG_IOCTL_BUFFERING_DRAIN_IMMEDIATE;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			break;
		case DIAG_IOCTL_CONFIG_BUFFERING_TX_MODE:
			data_type = DIAG_IOCTL_CONFIG_BUFFERING_TX_MODE;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			break;
		case DIAG_IOCTL_QUERY_REG_TABLE:
			data_type = DIAG_IOCTL_QUERY_REG_TABLE;
			ptr = malloc(sizeof(data_type));
			if (!ptr) {
				printf("In %s: malloc failed\n", __func__);
				return -ENOMEM;
			}
			memcpy(ptr, &data_type, sizeof(data_type));
			ret = write(fd, ptr, sizeof(data_type));
			free(ptr);
			wait_for_query_reg_table_mask();
			break;
		case DIAG_IOCTL_QUERY_DEBUG_ALL:
			data_type = DIAG_IOCTL_QUERY_DEBUG_ALL;
			ptr = malloc(sizeof(data_type));
			if (!ptr) {
				printf("In %s: malloc failed for DIAG_IOCTL_QUERY_DEBUG_ALL\n", __func__);
				return -ENOMEM;
			}
			memcpy(ptr, &data_type, sizeof(data_type));
			ret = write(fd, ptr, sizeof(data_type));
			free(ptr);
			wait_for_query_debug_all_mask();
			break;
		case DIAG_IOCTL_DCI_REG:
			data_type = DIAG_IOCTL_DCI_REG;
			len = sizeof(struct diag_dci_reg_tbl_t);
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			return wait_for_dci_reg_client_id();
		case DIAG_IOCTL_DCI_SUPPORT:
			data_type = DIAG_IOCTL_DCI_SUPPORT;
			len = sizeof(struct diag_dci_peripheral_list_t);
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			wait_for_dci_support(buf);
			return DIAG_DCI_NO_ERROR;
		case DIAG_IOCTL_DCI_HEALTH_STATS:
			data_type = DIAG_IOCTL_DCI_HEALTH_STATS;
			len = sizeof(struct diag_dci_health_stats_proc);
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, len);
			ret = write(fd, ptr, len + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			wait_for_dci_health_stats(buf);
			return DIAG_DCI_NO_ERROR;
		case DIAG_IOCTL_DCI_LOG_STATUS:
			data_type = DIAG_IOCTL_DCI_LOG_STATUS;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, sizeof(struct diag_log_event_stats));
			ret = write(fd, ptr, sizeof(struct diag_log_event_stats) + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			wait_for_dci_log_status(buf);
			return DIAG_DCI_NO_ERROR;
			break;
		case DIAG_IOCTL_DCI_EVENT_STATUS:
			data_type = DIAG_IOCTL_DCI_EVENT_STATUS;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, sizeof(struct diag_log_event_stats));
			ret = write(fd, ptr, sizeof(struct diag_log_event_stats) + sizeof(data_type));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			wait_for_dci_event_status(buf);
			return DIAG_DCI_NO_ERROR;
			break;
		case DIAG_IOCTL_DCI_CLEAR_LOGS:
			data_type = DIAG_IOCTL_DCI_CLEAR_LOGS;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, sizeof(int));
			ret = write(fd, ptr, sizeof(data_type) + sizeof(int));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			return DIAG_DCI_NO_ERROR;
			break;
		case DIAG_IOCTL_DCI_CLEAR_EVENTS:
			data_type = DIAG_IOCTL_DCI_CLEAR_EVENTS;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, sizeof(int));
			ret = write(fd, ptr, sizeof(data_type) + sizeof(int));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			return DIAG_DCI_NO_ERROR;
			break;
		case DIAG_IOCTL_DCI_DEINIT:
			data_type = DIAG_IOCTL_DCI_DEINIT;
			ptr = malloc(len + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, sizeof(int));
			ret = write(fd, ptr, sizeof(data_type) + sizeof(int));
			if (ret < 0) {
				free(ptr);
				return ret;
			}
			free(ptr);
			return DIAG_DCI_NO_ERROR;
			break;
		case DIAG_IOCTL_UPDATE_DYN_DBG_MASK:
			data_type = DIAG_IOCTL_UPDATE_DYN_DBG_MASK;
			ptr = malloc(sizeof(int) + sizeof(data_type));
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, &data_type, sizeof(data_type));
			memcpy(ptr + sizeof(data_type), buf, sizeof(int));
			ret = write(fd, ptr, sizeof(int) + sizeof(data_type));
			free(ptr);
			break;
		default:
			break;
		}
		if (ret >= 0)
			return 0;
		else
			return ret;

    }
}
