/*
 * Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#include "debug.h"
#include "cnss_genl_msg.h"
#include "cnss_plat.h"

struct cnss_nl_data {
	struct nl_sock *sock;
	int family_id;
	int mcgrp_id;
	struct cnss_evt_queue evt_q;
};

static struct cnss_nl_data cnss_nl;

static void cnss_genl_save_file(char *file_name, uint32_t total_size)
{
	FILE *fp = NULL;
	char filename[CNSS_MAX_FILE_PATH];
	int count = 0, wr_size = 0;
	struct cnss_evt *evt;

	if (strcmp(file_name, "default") == 0)
		snprintf(filename, sizeof(filename),
			 CNSS_DEFAULT_QDSS_TRACE_FILE);
	else
		snprintf(filename, sizeof(filename),
			 "/data/vendor/wifi/%s", file_name);
	fp = fopen(filename, "ab");
	if (fp == NULL) {
		wsvc_printf_err("%s: Failed to open file %s", __func__,
				filename);
		cnss_evt_free_queue(&cnss_nl.evt_q);
		return;
	}

	wsvc_printf_err("%s: Storing QDSS data to file: %s: total_size %u",
			__func__,  filename, total_size);

	evt = cnss_evt_dequeue(&cnss_nl.evt_q);
	while (evt) {
		wsvc_printf_err("%s: seg_id: %d data_len %u", __func__,
				count++, evt->data_len);
		wr_size += evt->data_len;

		fwrite(evt->data, 1, evt->data_len, fp);
		evt = cnss_evt_dequeue(&cnss_nl.evt_q);
	}
	cnss_evt_free_queue(&cnss_nl.evt_q);
	if (wr_size != total_size)
		wsvc_printf_err("%s: Total size mismatch. Check seq_id",
				 __func__);
	fclose(fp);
}

static int cnss_genl_recv_msg(struct nl_msg *nl_msg, void *data)
{
	UNUSED(data);
	struct nlmsghdr *nlh = nlmsg_hdr(nl_msg);
	struct genlmsghdr *gnlh = nlmsg_data(nlh);
	struct nlattr *attrs[CNSS_GENL_ATTR_MAX + 1];
	int ret = 0;
	unsigned char type = 0;
	char *file_name = NULL;
	unsigned int total_size = 0;
	unsigned int seg_id = 0;
	unsigned char end = 0;
	unsigned int data_len = 0;
	void *msg_buf = NULL;
	struct cnss_evt *evt;

	if (gnlh->cmd != CNSS_GENL_CMD_MSG)
		return NL_SKIP;

	wsvc_printf_dbg("Received CNSS_GENL_CMD_MSG");

	ret = genlmsg_parse(nlh, 0, attrs, CNSS_GENL_ATTR_MAX, NULL);
	if (ret < 0) {
		wsvc_printf_err("RX NLMSG: Parse fail %d", ret);
		return 0;
	}

	type = nla_get_u8(attrs[CNSS_GENL_ATTR_MSG_TYPE]);
	file_name = nla_get_string(attrs[CNSS_GENL_ATTR_MSG_FILE_NAME]);
	total_size = nla_get_u32(attrs[CNSS_GENL_ATTR_MSG_TOTAL_SIZE]);
	seg_id = nla_get_u32(attrs[CNSS_GENL_ATTR_MSG_SEG_ID]);
	end = nla_get_u8(attrs[CNSS_GENL_ATTR_MSG_END]);
	data_len = nla_get_u32(attrs[CNSS_GENL_ATTR_MSG_DATA_LEN]);
	msg_buf = nla_data(attrs[CNSS_GENL_ATTR_MSG_DATA]);

	wsvc_printf_err("RX NLMSG: type %u, file_name %s, total_size %u, seg_id %u, end %u, data_len %u",
			type, file_name, total_size, seg_id, end, data_len);

	if (type != CNSS_GENL_MSG_TYPE_QDSS ||
	    data_len > CNSS_GENL_DATA_LEN_MAX) {
		wsvc_printf_err("%s: Invalid CNSS_GENL_CMD_MSG\n", __func__);
		return 0;
	}

	evt = cnss_evt_alloc(CNSS_GENL_MSG_TYPE_QDSS, msg_buf, data_len);
	if (!evt) {
		wsvc_printf_err("%s: No memory for evt", __func__);
		return 0;
	}
	cnss_evt_enqueue(&cnss_nl.evt_q, evt);

	if (end)
		cnss_genl_save_file(file_name, total_size);

	return 0;
}

int cnss_genl_recvmsgs(void)
{
	int ret = 0;

	if (!cnss_nl.sock)
		return -EINVAL;

	ret = nl_recvmsgs_default(cnss_nl.sock);
	if (ret < 0)
		wsvc_printf_err("NL msg recv error %d", ret);

	return ret;
}

int cnss_genl_init(void)
{
	struct nl_sock *sock;
	int ret = 0;

	sock = nl_socket_alloc();
	if (!sock) {
		wsvc_printf_err("NL socket alloc fail");
		return -EINVAL;
	}

	ret = genl_connect(sock);
	if (ret < 0) {
		wsvc_printf_err("GENL socket connect fail");
		goto free_socket;
	}

	ret = nl_socket_set_buffer_size(sock, CNSS_GENL_BUF_SIZE, 0);
	if (ret < 0)
		wsvc_printf_err("Could not set NL RX buffer size %d",
				ret);

	cnss_nl.family_id = genl_ctrl_resolve(sock, CNSS_GENL_FAMILY_NAME);
	if (cnss_nl.family_id < 0) {
		ret = cnss_nl.family_id;
		wsvc_printf_err("Couldn't resolve family id");
		goto close_socket;
	}

	cnss_nl.mcgrp_id = genl_ctrl_resolve_grp(sock, CNSS_GENL_FAMILY_NAME,
					 CNSS_GENL_MCAST_GROUP_NAME);

	wsvc_printf_err("NL group_id %d, cnss_nl.family_id %d",
			cnss_nl.mcgrp_id, cnss_nl.family_id);

	nl_socket_disable_seq_check(sock);
	ret = nl_socket_modify_cb(sock, NL_CB_MSG_IN,
				  NL_CB_CUSTOM, cnss_genl_recv_msg, NULL);
	if (ret < 0) {
		wsvc_printf_err("Couldn't modify NL cb, ret %d", ret);
		goto close_socket;
	}

	ret = nl_socket_add_membership(sock, cnss_nl.mcgrp_id);
	if (ret < 0) {
		wsvc_printf_err("Couldn't add membership to group %d, ret %d",
				cnss_nl.mcgrp_id, ret);
		goto close_socket;
	}
	cnss_nl.sock = sock;

	return ret;

close_socket:
	nl_close(sock);
free_socket:
	nl_socket_free(sock);
	return ret;
}

int cnss_genl_get_fd(void)
{
	if (!cnss_nl.sock)
		return -1;

	return nl_socket_get_fd(cnss_nl.sock);
}

void cnss_genl_exit(void)
{
	if (!cnss_nl.sock)
		return;

	nl_close(cnss_nl.sock);
	nl_socket_free(cnss_nl.sock);
	cnss_nl.sock = NULL;
}
