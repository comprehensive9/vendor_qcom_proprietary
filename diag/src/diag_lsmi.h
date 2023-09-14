#ifndef DIAG_LSMI_H
#define DIAG_LSMI_H

/*===========================================================================

                   Diag Mapping Layer DLL , internal declarations

DESCRIPTION
  Internal declarations for Diag Service Mapping Layer.

Copyright (c)  2007-2015, 2017-2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/08   sj      Added featurization for WM specific code & CBSP2.0
02/04/08   mad     Created File
===========================================================================*/

#include <pthread.h>

#define DIAG_INVALID_HANDLE -1
#define DIAG_MDLOG_DIR		"/sdcard/diag_logs/"
#define DIAG_MDLOG_PID_FILE	"/sdcard/diag_logs/diag_mdlog_pid"
#define DIAG_MDLOG_PID_FILE_SZ  100
#define NUM_PROC 10
#define HDLC_DISABLE	1
#define READ_BUF_SIZE 100000
#define DISK_BUF_SIZE 1024*140
#define DISK_FLUSH_THRESHOLD  1024*128

#define HDLC_CRC_LEN	2
#define NON_HDLC_VERSION	1
/* Non-HDLC Header:
 * 1 byte - Control char
 * 1 byte - Version
 * 2 bytes - Packet length
 */
#define DIAG_NON_HDLC_HEADER_SIZE	4

/*
 * Minimum command size: 1 byte
 * Minimum Non-HDLC pkt size:  6 bytes
 * Minimum HDLC pkt size: 4 bytes
 */
#define DIAG_MIN_NON_HDLC_PKT_SIZE	6
#define MIN_CMD_PKT_SIZE 4

#define FILE_LIST_NAME_SIZE 100
#define MAX_FILES_IN_FILE_LIST 100
#define std_strlprintf     snprintf
#define RENAME_CMD_LEN ((2*FILE_NAME_LEN) + 10)

/* List of processors */
#define DIAG_ALL_PROC		-1
#define DIAG_MODEM_PROC		0
#define DIAG_LPASS_PROC		1
#define DIAG_WCNSS_PROC		2
#define DIAG_SENSORS_PROC	3
#define DIAG_WDSP_PROC		4
#define DIAG_CDSP_PROC		5
#define DIAG_NPU_PROC		6
#define DIAG_NSP1_PROC		7
#define DIAG_GPDSP0_PROC	8
#define DIAG_GPDSP1_PROC	9
#define DIAG_HELIOS_M55_PROC	10
#define NUM_PERIPHERALS		11
#define DIAG_APPS_PROC		(NUM_PERIPHERALS)

#define UPD_WLAN		(NUM_PERIPHERALS + 1)
#define UPD_AUDIO		(UPD_WLAN + 1)
#define UPD_SENSORS		(UPD_AUDIO + 1)
#define UPD_CHARGER		(UPD_SENSORS + 1)
#define NUM_UPD			4

#define TOTAL_PD_COUNT	(NUM_PERIPHERALS + NUM_UPD + 1)

#define DIAGID_V2_FEATURE_COUNT		3
#define NUM_MD_SESSIONS				(NUM_PERIPHERALS + NUM_UPD + 1) /* Added 1 to account for APPS */
#define DIAG_MD_LOCAL				0
#define DIAG_MD_LOCAL_LAST			1
#define DIAG_MD_BRIDGE_BASE			DIAG_MD_LOCAL_LAST
#define DIAG_MD_MDM					(DIAG_MD_BRIDGE_BASE)
#define DIAG_MD_MDM2				(DIAG_MD_BRIDGE_BASE + 1)
#define DIAG_MD_BRIDGE_LAST			(DIAG_MD_BRIDGE_BASE + 2)
#define NUM_DIAG_MD_DEV				DIAG_MD_BRIDGE_LAST
#define DIAG_NUM_PROC				1

#define DIAG_SEC_TO_USEC(x) ((x) * 1000000ULL)

struct buffer_pool {
	int free;
	int data_ready;
	unsigned int bytes_in_buff[NUM_PROC];
	unsigned char *buffer_ptr[NUM_PROC];
	pthread_mutex_t write_mutex;
	pthread_cond_t write_cond;
	pthread_mutex_t read_mutex;
	pthread_cond_t read_cond;
};

enum status {
	NOT_READY,
	READY,
};

typedef struct
{
	int data_type;
	uint32 diagid_mask_supported;
	uint32 diagid_mask_enabled;
} query_pd_feature_mask;

typedef struct
{
	int data_type;
	int ret_val;
	uint32 diagid_mask;
} diag_hw_accel_diag_id_mask;

struct diag_feature_t {
	uint32_t feature_mask;
	uint8_t rcvd_feature_mask;
	uint8_t log_on_demand;
	uint8_t separate_cmd_rsp;
	uint8_t encode_hdlc;
	uint8_t untag_header;
	uint8_t peripheral_buffering;
	uint8_t pd_buffering;
	uint8_t mask_centralization;
	uint8_t stm_support;
	uint8_t sockets_enabled;
	uint8_t sent_feature_mask;
	uint8_t diag_id_support;
	uint8_t diagid_v2_feature_mask;
	uint8_t multi_sim_support;
};

struct diag_buffering_mode_t {
	uint8_t peripheral;
	uint8_t mode;
	uint8_t high_wm_val;
	uint8_t low_wm_val;
};

struct diag_debug_all_param_t {
	uint8_t time_api;
	uint8_t timesync_enabled;
	uint8_t timestamp_switch;
	uint8_t stm_support[NUM_PERIPHERALS + 1];
	uint8_t stm_state_requested[NUM_PERIPHERALS + 1];
	uint8_t stm_state[NUM_PERIPHERALS + 1];
	uint32_t apps_feature;
	uint32_t diagid_v2_feature[DIAGID_V2_FEATURE_COUNT];
	uint32_t diagid_v2_status[DIAGID_V2_FEATURE_COUNT];
	uint32_t diag_hw_accel[DIAGID_V2_FEATURE_COUNT];

	struct diag_feature_t feature[NUM_PERIPHERALS + 1];
	int logging_mode[NUM_DIAG_MD_DEV];
	int logging_mask[NUM_DIAG_MD_DEV];
	int pd_logging_mode[NUM_UPD];
	int pd_session_clear[NUM_UPD];
	int num_pd_session;
	uint32_t md_session_mask[NUM_DIAG_MD_DEV];
	uint8_t md_session_mode[NUM_DIAG_MD_DEV];

	int real_time_mode[DIAG_NUM_PROC];
	uint16_t proc_active_mask;
	uint16_t proc_rt_vote_mask[DIAG_NUM_PROC];
	struct diag_buffering_mode_t buffering_mode[NUM_MD_SESSIONS];
	uint8_t buffering_flag[NUM_MD_SESSIONS];
	int usb_connected;
};

extern int rename_file_names;	/* Rename file name on close to current time */
extern int rename_dir_name;	/* Rename directory name to current time when ODL is halted */
extern int diag_fd;
extern int fd_md[NUM_PROC];
extern int gdwClientID;
void log_to_device(unsigned char *ptr, int logging_mode, int size, int type);
void send_mask_modem(unsigned char mask_buf[], int count_mask_bytes);
int diag_has_remote_device(uint16 *remote_mask);
int diag_register_socket_cb(int (*callback_ptr)(void *data_ptr, int socket_id), void *data_ptr);
int diag_set_socket_fd(int socket_id, int socket_fd);
int diag_send_socket_data(int id, unsigned char buf[], int num_bytes);
int diag_get_max_channels(void);
int diag_read_mask_file_list(char *mask_list_file);

/* === Functions dealing with diag wakelocks === */

/* Returns 1 if a wakelock is initialized for this process,
   0 otherwise. */
int diag_is_wakelock_init(void);

/* Opens the wakelock files and initializes the wakelock for
   the current process. It doesn't hold any wakelock. To hold
   a wakelock, call diag_wakelock_acquire. */
void diag_wakelock_init(char *wakelock_name);

/* Closes the wakelock files. It doesn't release the wakelock
   for the current process if held. */
void diag_wakelock_destroy(void);

/* Acquires a wakelock for the current process under the name
   given by diag_wakelock_init. */
void diag_wakelock_acquire(void);

/* Releases the wakelock held by the current process. */
void diag_wakelock_release(void);

/* To convert an integer/hexadecimal string to an integer */
int to_integer(char *str);

/* Request  the kernel diag to turn on/off the hdlc encoding of the data. */
int diag_hdlc_toggle(uint8 hdlc_support);
int diag_hdlc_toggle_mdm(uint8 hdlc_support, int proc);

/* === Functions dealing with qshrink4 === */

/* Creates threads to read the qshrink4 database threads. */
int create_diag_qshrink4_db_parser_thread(unsigned int peripheral_mask, unsigned int device_mask);

/* Parses the data for qshrink4 command response */
int parse_data_for_qsr4_db_file_op_rsp(uint8 *ptr, int count_received_bytes, int index);
int parse_data_for_qdss_rsp(uint8* ptr, int count_received_bytes, int index);
int parse_data_for_diag_id_rsp(uint8* ptr, int count_received_bytes, int index, int *update_count);
int parse_data_for_adpl_rsp(uint8* ptr, int count_received_bytes, int index);

/* Add qshrink4 guid information to qmdl2 header */
int add_guid_to_qshrink4_header(unsigned char * guid, int p_type, int peripheral);

/* Notify parser thread when a PD comes up */
void diag_notify_parser_thread(int type, int peripheral_mask);
void diag_set_device_mask(unsigned int device_mask);
void diag_set_peripheral_mask(unsigned int peripheral_mask);
void diag_set_upd_mask(unsigned int pd_mask);
void diag_set_qdss_mask(unsigned int qdss_mask, unsigned int diag_device_mask);
void diag_set_adpl_mask(unsigned int diag_device_mask);
void diag_set_diagid_mask(unsigned int diag_device_mask);
void diag_get_peripheral_name_from_mask(char *peripheral_name,
					unsigned int len,
					unsigned int peripheral_mask);
int diag_get_pd_name_from_mask(char *buf,
					unsigned int len,
					unsigned int pd_mask);
void get_time_string(char *buffer, int len);
void diag_kill_qshrink4_threads(void);
void diag_kill_qdss_threads(void);
void diag_kill_adpl_threads(void);
void diag_kill_diagid_threads(void);
void qdss_close_qdss_node_mdm(void);
int delete_log(int type);
int delete_qdss_log(int type);
int diag_qdss_init(void);
int diag_id_init(void);
void create_qshrink_thread(void);
void diag_reset_guid_count(int p_type, int peripheral);
int diag_adpl_init(void);
void diag_notify_qdss_thread(int peripheral_type, int peripheral_mask);
void diag_notify_adpl_thread(void);
int diag_send_cmds_to_disable_adpl(int in_ssr);
extern boolean gbRemote;
#define DIAG_LSM_PKT_EVENT_PREFIX "DIAG_SYNC_EVENT_PKT_"
#define DIAG_LSM_MASK_EVENT_PREFIX "DIAG_SYNC_EVENT_MASK_"
#endif /* DIAG_LSMI_H */

