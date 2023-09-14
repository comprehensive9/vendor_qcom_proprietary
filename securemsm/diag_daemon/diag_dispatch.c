/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        DiagCommD Dispatch Function

GENERAL DESCRIPTION
  Implementation of APIs to be called by call back function which
validates and forward the diag request packet to secure world and return
the response back to the caller.

EXTERNALIZED FUNCTIONS
  None

Copyright (c) 2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <comdef.h>
#include <diagcmd.h>
#include <QSEEComAPI.h>
#include <diagcomm.h>

/* Return on allocation failure */
diag_rsp_pkt_struct rsp_fail = {.header.rsp_code = SECBOOT_TOOL_HLOS_ERR_DIAG_ALLOC_FAILED},
	rsp_fail_syscall = {.header.rsp_code = SECBOOT_TOOL_HLOS_ERR_SYSCALL_FAILED};

/* Function to get response packet size */
fuse_diag_err_code get_rsp_pkt_size (size_t *rsp_pkt_size, diag_reqpkt_header *req, size_t len)
{
	size_t rsp_size = 0,ret = 0;
	diag_reqpkt_size rsp_pkt;

	switch (req->rsp_payload.type_of_payload) {

	case DIAG_RQT_RSP_BUFF :
		*rsp_pkt_size = req->rsp_payload.payload_size;
		if (*rsp_pkt_size == 0) {
			*rsp_pkt_size = sizeof(diag_rsp_pkt_header_struct);
			return SECBOOT_TOOL_HLOS_ERR_INVALID_RSP_PAYLOAD_SIZE;
		}
		break;
	case DIAG_QUERY_RSP_BUFF_SIZE :
		ret = QSEECom_send_service_cmd (req, len,
			&rsp_pkt, sizeof(diag_reqpkt_size), QSEECOM_DIAG_FUSE_REQ_RSP);
		if(ret)
			return SECBOOT_TOOL_HLOS_ERR_SYSCALL_FAILED;

		*rsp_pkt_size = rsp_pkt.query_size;
		if (*rsp_pkt_size == 0) {
			*rsp_pkt_size =  sizeof(diag_rsp_pkt_header_struct);
			return SECBOOT_TOOL_HLOS_ERR_INVALID_RSP_PAYLOAD_SIZE;
		}
		else
			*rsp_pkt_size = fdt16_to_cpu(*rsp_pkt_size,0);
		break;
	default :
		*rsp_pkt_size = 0;
	}
	*rsp_pkt_size += sizeof(diag_rsp_pkt_header_struct);
	return 0;
}


diag_rsp_pkt_struct * fuse_dispatch (void * req, uint16_t length)
{
	diag_reqpkt_struct *req_pkt = (diag_reqpkt_struct *)req;
	diag_rsp_pkt_struct *rsp_pkt = NULL;
	diag_reqpkt_header *send_req = NULL;
	size_t ret = 0, rsp_pkt_size = 0;
	size_t payload_type = 0;
	fuse_diag_err_code res = 0;
	enum QSEECom_command_id cmd_id = 0;


	if (req_pkt == NULL || req_pkt->header.packet_size == 0 ||
		req_pkt->header.packet_size < sizeof(diag_reqpkt_header))
	{
		rsp_pkt = (diag_rsp_pkt_struct *)diagpkt_subsys_alloc(DIAG_SUBSYS_SEC,
			DIAG_CMD_C, sizeof(diag_rsp_pkt_header_struct));

		if (rsp_pkt == NULL) {
			LOGE("diagpkt_subsys_alloc Failed to allocate resouce \n");
			return &rsp_fail;
		}

		if (req_pkt == NULL ) {
			LOGE("Invalid Request Packet\n");
			rsp_pkt->header.rsp_code = SECBOOT_TOOL_HLOS_ERR_INVALID_REQ_PKT;
			return rsp_pkt;
		}
		else {
			LOGE("Invalid Request Packet Size\n");
			rsp_pkt->header.rsp_code = SECBOOT_TOOL_HLOS_ERR_INVALID_REQ_PKT_SIZE ;
			return rsp_pkt;
		}
	}

	send_req = &req_pkt->header;
	payload_type = req_pkt->header.rsp_payload.type_of_payload ;

	if (payload_type != DIAG_NO_RSP_BUFF) {
		res = get_rsp_pkt_size (&rsp_pkt_size, send_req, length);

		rsp_pkt = ((diag_rsp_pkt_struct *)diagpkt_subsys_alloc(DIAG_SUBSYS_SEC,
			DIAG_CMD_C, rsp_pkt_size ));

		if (rsp_pkt == NULL) {
			LOGE("diagpkt_subsys_alloc Failed to allocate resouce \n");
			return &rsp_fail;
		}
		if (res != 0) {
			LOGE("Failed to get the response packet size %x",res);
			rsp_pkt->header.rsp_code = res;
			return rsp_pkt;
		}
	}

	switch (payload_type)
	{
		case DIAG_NO_RSP_BUFF :
			rsp_pkt_size = 0;
			cmd_id = QSEECOM_DIAG_FUSE_REQ;
			break;

		case DIAG_RQT_RSP_BUFF :
			cmd_id = QSEECOM_DIAG_FUSE_REQ_RSP;
			break;

		case DIAG_QUERY_RSP_BUFF_SIZE :
			req_pkt->header.rsp_payload.type_of_payload  = DIAG_RQT_RSP_BUFF;
			req_pkt->header.rsp_payload.payload_size = rsp_pkt_size;
			cmd_id = QSEECOM_DIAG_FUSE_REQ_RSP;
			break;

		default :
			rsp_pkt->header.rsp_code = SECBOOT_TOOL_HLOS_ERR_INVALID_RSP_PAYLOAD_TYPE;
			LOGE("Invalid response payload type \n");
			return rsp_pkt;
	}

	ret = QSEECom_send_service_cmd (send_req, length,
		rsp_pkt, rsp_pkt_size, cmd_id);

	if (ret)
	{
		LOGE("QSEECom_send_service_cmd Failed\n", ret);
		if (rsp_pkt) {
			rsp_pkt->header.rsp_code = SECBOOT_TOOL_HLOS_ERR_SYSCALL_FAILED;
			return rsp_pkt;
		}
		else {
                        return &rsp_fail_syscall;
		}
	}

	if (rsp_pkt && hlos_success(rsp_pkt->header.rsp_code)) {
		if (rsp_pkt->header.op_id == send_req->op_id &&
			rsp_pkt->header.packet_size == send_req->packet_size) {
			rsp_pkt->header.rsp_code = SECBOOT_TOOL_HLOS_SUCCESS;
			LOGD("QSEECom_send_service_cmd SUCCESS\n");
		}
		else {
			LOGE("Invalid Response Packet Parameters\n");
			rsp_pkt->header.rsp_code = SECBOOT_TOOL_HLOS_ERR_INVALID_RSP_PKT_PARAMS;
		}
	}

	return rsp_pkt;
}
