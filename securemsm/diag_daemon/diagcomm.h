/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    DIAGCOMM Daemon Header File

Description
 Global Data declarations of the diagcommd component.

# Copyright (c) 2021 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <sys/types.h>
#include <diagpkt.h>
#include <diag_lsm.h>

/** adb log */
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "DIAGD: "
#endif

#define LOG_NDDEBUG 0 //Define to enable LOGD

#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#define LOG_NDEBUG  0 //Define to enable LOGV
#endif

#include <syslog.h>
#define LOGI(...) syslog(LOG_NOTICE, "INFO:" __VA_ARGS__)
#define LOGV(...) syslog(LOG_NOTICE,"VERB:" __VA_ARGS__)
#define LOGD(...) syslog(LOG_DEBUG,"DBG:" __VA_ARGS__)
#define LOGE(...) syslog(LOG_ERR,"ERR:" __VA_ARGS__)
#define LOGW(...) syslog(LOG_WARNING,"WRN:" __VA_ARGS__)

#define EXTRACT_BYTE(x, n)	((unsigned long long)((uint8_t *)&x)[n])
#define CPU_TO_FDT16(x, n) ((EXTRACT_BYTE(x, n+1) << 8) | EXTRACT_BYTE(x, n))

#define DIAG_CMD_C 0x0100
#define SECBOOT_TOOL_TZ_SUCCESS 0xFFFB0000

// Payload Type
#define DIAG_NO_RSP_BUFF 0x001
#define DIAG_RQT_RSP_BUFF 0x002
#define DIAG_QUERY_RSP_BUFF_SIZE 0x003

/* Error codes in HLOS */
typedef enum
{
	SECBOOT_TOOL_HLOS_SUCCESS = 0xFFFA0000,
	SECBOOT_TOOL_HLOS_ERR_INVALID_REQ_PKT,
	SECBOOT_TOOL_HLOS_ERR_INVALID_REQ_PKT_SIZE,
	SECBOOT_TOOL_HLOS_ERR_INVALID_RSP_PAYLOAD_SIZE,
	SECBOOT_TOOL_HLOS_ERR_INVALID_RSP_PAYLOAD_TYPE,
	SECBOOT_TOOL_HLOS_ERR_INVALID_RSP_PKT_PARAMS,
	SECBOOT_TOOL_HLOS_ERR_SYSCALL_FAILED,
	SECBOOT_TOOL_HLOS_ERR_DIAG_ALLOC_FAILED,
} fuse_diag_err_code;

/* Diag Header */
typedef struct __attribute__((__packed__))
{
	diagpkt_cmd_code_type         cmd_code;
	diagpkt_subsys_id_type        subsys_id;
	diagpkt_subsys_cmd_code_type  subsys_cmd_code;
} diagpkt_subsys_header;


/* Request Packet Header */
typedef struct __attribute__((__packed__))
{
	uint32_t op_id;
	uint32_t packet_size;
	struct{
		uint32_t  payload_size     : 16;
		uint32_t  type_of_payload  : 16;
	}rsp_payload;
} diag_reqpkt_header;


/* Requet Packet */
typedef struct __attribute__((__packed__))
{
	diagpkt_subsys_header  diag_hdr;
	diag_reqpkt_header header;
	void * payload;
} diag_reqpkt_struct;


/* Request Packet to Query size */
typedef struct __attribute__((__packed__))
{
	diag_reqpkt_header header;
	uint32_t query_size;
} diag_reqpkt_size;


/* Response Packet Header */
typedef struct __attribute__((__packed__))
{
	uint32_t op_id;
	uint32_t packet_size;
	fuse_diag_err_code rsp_code;
} diag_rsp_pkt_header_struct;


/* Response Packet */
typedef struct __attribute__((__packed__))
{
	diag_rsp_pkt_header_struct header;
	void * payload_rsp;
} diag_rsp_pkt_struct;


diag_rsp_pkt_struct * fuse_dispatch( void * request, uint16_t len);

bool inline hlos_success(fuse_diag_err_code val)
{
	if(val == SECBOOT_TOOL_TZ_SUCCESS)
		return true;
	else
		return false;
}

static inline uint16_t fdt16_to_cpu(uint16_t x, uint16_t n)
{
	return (uint16_t)CPU_TO_FDT16(x,n);
}
