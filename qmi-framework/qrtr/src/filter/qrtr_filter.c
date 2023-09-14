/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <err.h>
#include <errno.h>
#include <linux/qrtr.h>
#include <sys/socket.h>
#include <linux/bpf_common.h>
#include <linux/bpf.h>
#include "bpf_config.h"

#include "libqrtr.h"
#include "qrtr_filter.h"

#define dprintf(x...)
#define MAX_SUPPORTED_MSG_ID 	100

/* eBPF filter level definitions */
#define SVC_LEVEL		1
#define MSG_LEVEL		2

#define LOG_BUF_SIZE 65536

char bpf_log_buf[LOG_BUF_SIZE];

struct filter_arg
{
	int svc_lvl_gid_map_fd;
	int msg_lvl_gid_map_fd;
	int svc_to_gid_prgm_jump_table;
	int msg_to_gid_prgm_jump_table;
	int node_id_table_fd;
	int msg_id_map_fd;
	int indx;
	int filter_lvl;
};

static void qrtr_set_svc_lvl_gid_filter(int svc_lvl_gid_map_fd, int svc_to_gid_jump_table_fd,
					int msg_to_gid_jump_table_fd, int msg_id_map_fd,
					int node_id_table_fd, int indx);
static void qrtr_set_msg_lvl_gid_filter(int gid_map_fd, int jump_table_fd,
					int msg_id_map_fd, int node_id_table_fd,
					int indx);
static void set_filter_program(struct filter_arg *arg);

int main(int argc, char **argv)
{
	int service_lvl_gid_map_fd, msg_lvl_gid_map_fd;
	int svc_table_map_fd, msg_id_map_fd;
	int svc_to_gid_jump_table_fd;
	int msg_to_gid_jump_table_fd;
	int svc_lvl_acc_cntrl_pgfd;
	int broadcast_node_map_fd;
	unsigned short msg_low;
	unsigned short msg_high;
	int svc_gid_indx = 0;
	int msg_gid_indx = 0;
	int map_table_size;
	int gid_value = 1;
	long long key;
	int rc = 0;
	int sock;
	int i,j,k;

	if (!access_rule_size)
		err(1, "No Access control rule to apply");

	sock = socket(AF_QIPCRTR, SOCK_DGRAM, 0);
	if (sock < 0)
		err(1, "sock(AF_QIPCRTR)");

	/**
	 *		service ID	|	gid list
	 *		----------		--------
	 *			...		...
	 *			...		...
	 *
	 * Below section will create this map table
	 */
	/* service id based map to get gid list for service level access control */
	QRTR_BPF_MAP_CREATE(svc_table_map_fd, BPF_MAP_TYPE_HASH, sizeof(key),
					sizeof(int), access_rule_size, 0);
	if (svc_table_map_fd < 0)
		err(1, "failed to create svc_table_map_fd '%s'",strerror(errno));

	dprintf("svc_table_map_fd %d size of key %d \n\n", svc_table_map_fd,
		sizeof(key));

	/* Program array map to switch to messgae level filter from service level*/
	QRTR_BPF_MAP_CREATE(svc_to_gid_jump_table_fd, BPF_MAP_TYPE_PROG_ARRAY,
					sizeof(int), sizeof(int), access_rule_size, 0);
	if (svc_to_gid_jump_table_fd < 0)
		err(1, "failed to create svc_to_gid_jump_table_fd '%s'", strerror(errno));
	dprintf("svc_to_gid_jump_table MAP creation success %d\n",
		svc_to_gid_jump_table_fd);

	for( i = 0; i < access_rule_size; i++ ) {

		/*------------- 1st level (service level) access control -------------*/
		service_lvl_gid_map_fd = -1;
		/**
		 *			    gid 	|	   value
		 *			   (key)		   (value)
		 *			----------		------------
		 *			    gid1			 1
		 *			    gid2			 1
		 *			    ...				...
		 *
		 * Below section create & update gid map table with gid as the key & value as 1
		 * for the 1st level (service level) access control
		 */
		/* create a map to hold list of 1st level access control GID's */
		if (qmi_access_cntl_rule[i].rule1_size)
			map_table_size = qmi_access_cntl_rule[i].rule1_size;
		else
			map_table_size = 1;

		QRTR_BPF_MAP_CREATE(service_lvl_gid_map_fd, BPF_MAP_TYPE_HASH,
						sizeof(unsigned int), sizeof(int),
						map_table_size, 0);
		if (service_lvl_gid_map_fd < 0)
			err(1, "failed to create service_lvl_gid_map_fd '%s'",
				strerror(errno));

		/* update GID list in 1st level access control map */
		for ( j = 0; j < qmi_access_cntl_rule[i].rule1_size; j++ ) {
			QRTR_BPF_MAP_UPDATE(rc, service_lvl_gid_map_fd,
					&qmi_access_cntl_rule[i].rule1[j].gid,
					&gid_value, BPF_ANY);
			if (rc)
				err(1, "failed to update service_lvl_gid_map rc %d '%s'", rc,
					strerror(errno));
			dprintf(" service_lvl_gid_map %d updated with key(gid) %d value %d rc %d\n",
				service_lvl_gid_map_fd, qmi_access_cntl_rule[i].rule1[j].gid,
				gid_value, rc);
		}

		/**
		 *			   svc id 	 |	   gid fd
		 *			   (key)		   (value)
		 *			----------		------------
		 *			    svc1		   gid fd1
		 *			    svc2		   gid fd2
		 *			    ...			     ...
		 *
		 * Below section update service id as the key & value as gid fd
		 * created above for the 1st level (service level) access control
		 */
		/* update GID map fd as the value to service map */
		key = (long long) qmi_access_cntl_rule[i].instance_id << 32 |
				qmi_access_cntl_rule[i].service_id;
		svc_gid_indx = i;
		QRTR_BPF_MAP_UPDATE(rc, svc_table_map_fd, &key, &svc_gid_indx,
					BPF_ANY);
		if (rc)
			err(1, "failed to update svc_table_map rc %d '%s'", rc, strerror(errno));
		dprintf(" svc_table_map with key(svc) 0x%x updated with value(indx) %d rc %d\n",
			qmi_access_cntl_rule[i].service_id, svc_gid_indx, rc);

		/* service broadcast control to sub systems */
		if (qmi_access_cntl_rule[i].node_size)
			map_table_size = qmi_access_cntl_rule[i].node_size;
		else
			map_table_size = 1;

		QRTR_BPF_MAP_CREATE(broadcast_node_map_fd, BPF_MAP_TYPE_HASH,
					sizeof(unsigned int), sizeof(int),
					map_table_size, 0);
		if (broadcast_node_map_fd < 0)
			err(1, "failed to create broadcast_node_map_fd '%s'",
				strerror(errno));
		dprintf("broadcast_node_map_fd %d rc %d\n", broadcast_node_map_fd, rc);

		/* update node id list */
		for (j = 0; j < qmi_access_cntl_rule[i].node_size; j++) {
			if (qmi_access_cntl_rule[i].svc_broadcast_dis ==
				QMI_SRV_BROADCAST_DISABLE) {
				QRTR_BPF_MAP_UPDATE(rc, broadcast_node_map_fd,
					&qmi_access_cntl_rule[i].node[j].node_id,
					&qmi_access_cntl_rule[i].svc_broadcast_dis,
					BPF_ANY);
				if (rc)
					err(1, "failed to update broadcast_node_map_fd rc %d '%s'",
						rc, strerror(errno));
				dprintf("broadcast_node_map_fd %d updated with key(node) %d value %d rc %d\n",
					broadcast_node_map_fd, qmi_access_cntl_rule[i].node[j].node_id,
					qmi_access_cntl_rule[i].svc_broadcast_dis, rc);
			}
		}

		/*------------- 2nd level (message level) access control -------------*/

		msg_id_map_fd = -1;
		/**
		 *			   msg id 	 |	   gid fd
		 *				(key)		   (value)
		 *			----------		------------
		 *				...		        ...
		 *				...		        ...
		 *				...				...
		 *
		 * Below section create msg id map table with msg id as the key & value
		 * as gid fd for the 2nd level (message level) access control
		 */
		/* create map to hold message id's under one service */
		QRTR_BPF_MAP_CREATE(msg_id_map_fd, BPF_MAP_TYPE_HASH, sizeof(unsigned int),
				sizeof(int), MAX_SUPPORTED_MSG_ID, 0);
		if (msg_id_map_fd < 0)
			err(1, "failed to create msg_id_map_fd '%s'", strerror(errno));
		dprintf("msg_id_map_fd created fd %d\n", msg_id_map_fd);

		if (qmi_access_cntl_rule[i].rule2_size)
			map_table_size = qmi_access_cntl_rule[i].rule2_size;
		else
			map_table_size = 1;

		QRTR_BPF_MAP_CREATE(msg_to_gid_jump_table_fd, BPF_MAP_TYPE_PROG_ARRAY,
						sizeof(int), sizeof(int),
						map_table_size, 0);
		if (msg_to_gid_jump_table_fd < 0)
			err(1, "failed to create msg_to_gid_jump_table_fd '%s'", strerror(errno));
		dprintf("msg_to_gid_jump_table MAP creation success %d\n",
			msg_to_gid_jump_table_fd);

		/* update GID list in message level access control map */
		for (j = 0; j < qmi_access_cntl_rule[i].rule2_size; j++) {

			msg_lvl_gid_map_fd = -1;
			/**
			 *	    gid 	|	   value
			 *         (key)		   (value)
			 *	----------		------------
			 *	   gid1			     1
			 *	   gid2			     1
			 *	   ...			    ...
			 *
			 * Below section create & update gid map table with gid as the key & value as 1
			 * for the 2nd level (message level) access control
			 */
			/* create message level GID map */
			if (qmi_access_cntl_rule[i].rule2[j].size) {
				QRTR_BPF_MAP_CREATE(msg_lvl_gid_map_fd, BPF_MAP_TYPE_HASH,
					sizeof(unsigned int), sizeof(int),
					qmi_access_cntl_rule[i].rule2[j].size, 0);
				if (msg_lvl_gid_map_fd < 0)
					err(1, "failed to create msg_lvl_gid_map_fd '%s'",
						strerror(errno));
			}
			/* update GID list in message level map */
			for (k = 0; k < qmi_access_cntl_rule[i].rule2[j].size; k++) {
				QRTR_BPF_MAP_UPDATE(rc, msg_lvl_gid_map_fd,
						&qmi_access_cntl_rule[i].rule2[j].gid_list[k].gid,
						&gid_value, BPF_ANY);
				if (rc)
					err(1, "failed to update msg_lvl_gid_map_fd rc %d '%s'", rc,
						strerror(errno));
				dprintf(" Message level map with key(gid) %d updated with value %d rc %d\n",
					qmi_access_cntl_rule[i].rule2[j].gid_list[k].gid,
					gid_value, rc);
			}

			/**
			 *		msg id 	    |	   gid fd
			 *		(key)		   (value)
			 *		----------	------------
			 *		msgid1		    gid fd1
			 *		msgid2		    gid fd2
			 *		 ...		      ...
			 *
			 * Below section update msg id map table with msg id as the key & value
			 * as gid fd for the 2nd level (message level) access control
			 */
			msg_low = qmi_access_cntl_rule[i].rule2[j].msg_id_low;
			msg_high = qmi_access_cntl_rule[i].rule2[j].msg_id_high;

			msg_gid_indx = j;
			for (k = msg_low; k <= msg_high; k++) {
				QRTR_BPF_MAP_UPDATE(rc, msg_id_map_fd, &k, &msg_gid_indx, BPF_ANY);
				if (rc)
					err(1, "failed to update msg_id_map_fd rc %d '%s'", rc,
						strerror(errno));
				dprintf("msg_id_map updated with key(msg_id) %d value(indx) %d rc %d\n",
					k, msg_gid_indx, rc);
			}

			qrtr_set_msg_lvl_gid_filter(msg_lvl_gid_map_fd, msg_to_gid_jump_table_fd,
					msg_id_map_fd, broadcast_node_map_fd, msg_gid_indx);
		}

		qrtr_set_svc_lvl_gid_filter(service_lvl_gid_map_fd, svc_to_gid_jump_table_fd,
						msg_to_gid_jump_table_fd, msg_id_map_fd,
						broadcast_node_map_fd, svc_gid_indx);
	}


	struct bpf_insn qmi_svc_lvl_acc_cntrl_filter[] = {
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_6, .src_reg = BPF_REG_1, .off   = 0, .imm   = 0 }),

		((struct bpf_insn) {.code  = load_absolute(BPF_H),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = PKT_TYPE_OFFSET }),/* r0 = *(u16*)skb[PKT_TYPE_OFFSET], r0 = pkt_type */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 16 }),	/* convert pkt_type from BE to LE --> r0*/
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 3, .imm   = QRTR_TYPE_DATA }),/* jump to svc check in case of control pkt */

		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = EFF_GID_OFFSET }),/* r0 = *(u32*)skb[EFF_GID_OFFSET], r0 = egid */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 34, .imm   = AID_ROOT }),/* jump to success return, allow access if the process has root permission */
		/* -- get service key (inst id | svc id) from skbuff --
		 * svc_id = load_byte(skb, SERVICE_ID_OFFSET);
		 * inst_id = QMI_ANY_INSTANCE;
		 * svc_key = (long long) inst_id << 32 | svc_id;
		 */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = SERVICE_ID_OFFSET }),/* r0 = *(u32*)skb[SERVICE_ID_OFFSET], r0 = svc_id */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert svc id from BE to LE */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_7, .src_reg = BPF_REG_0, .off   = 0, .imm   = 0 }),	/* r7 = svc id */
		((struct bpf_insn) {.code  = move64_imm,
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = QMI_ANY_INSTANCE }),	/* r0 = QMI_ANY_INSTANCE */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_LSH),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* r0 = r0 << 32,  inst_id <<= 32 */
		((struct bpf_insn) {.code  = alu64_reg_opcode(BPF_OR),
		.dst_reg = BPF_REG_7, .src_reg = BPF_REG_0, .off   = 0, .imm   = 0 }),	/* r7 = r0 | r7 ( inst | svc ) */

		/* -- lookup service key on svc_table_map_fd --
		 * int *value;
		 * value = bpf_map_lookup_elem(&svc_table_map_fd, &svc_key);
		 * 	if (!value)
		 * 		return success;
		 */
		((struct bpf_insn) {.code  = store_memory(BPF_DW),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_7, .off   = -8, .imm   = 0 }),/* *(u32 *)(fp - 8) = r7 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }),	/* r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -8 }),	/* r2 = r2 - 8 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (svc_table_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (svc_table_map_fd)) >> 32 }),/* r1 = svc_table_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 15, .imm   = 0 }),/* jump to service level filter if svc key is found */

		/* -- get service key (inst id | svc id) from skbuff --
		 * svc_id = load_byte(skb, SERVICE_ID_OFFSET);
		 * inst_id = load_byte(skb, INSTANCE_ID_OFFSET);
		 * svc_key = (long long) inst_id << 32 | svc_id;
		 */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = SERVICE_ID_OFFSET }),/* r0 = *(u32*)skb[SERVICE_ID_OFFSET], r0 = svc_id */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert svc id from BE to LE */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_7, .src_reg = BPF_REG_0, .off   = 0, .imm   = 0 }),	/* r7 = svc id */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = INSTANCE_ID_OFFSET }),/* r0 = *(u32*)skb[INSTANCE_ID_OFFSET], r0 = inst_id */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert inst id from BE to LE */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_RSH),
		 .dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 8 }),/* r0 = r0 >> 8,  inst_id >>= 8 */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_LSH),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* r0 = r0 << 32,  inst_id <<= 32 */
		((struct bpf_insn) {.code  = alu64_reg_opcode(BPF_OR),
		.dst_reg = BPF_REG_7, .src_reg = BPF_REG_0, .off   = 0, .imm   = 0 }),	/* r7 = r0 | r7 ( inst | svc ) */

		/* -- lookup service key on svc_table_map_fd --
		 * int *value;
		 * value = bpf_map_lookup_elem(&svc_table_map_fd, &svc_key);
		 * 	if (!value)
		 * 		return success;
		 */
		((struct bpf_insn) {.code  = store_memory(BPF_DW),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_7, .off   = -8, .imm   = 0 }),/* *(u32 *)(fp - 8) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }),	/* r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -8 }),	/* r2 = r2 - 8 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (svc_table_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (svc_table_map_fd)) >> 32 }),/* r1 = svc_table_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 6, .imm   = 0 }),/* jump to success return if svc id is not found, grant permission */

		((struct bpf_insn) {.code  = load_memory(BPF_W),
		.dst_reg = BPF_REG_7, .src_reg = BPF_REG_0, .off   = 0, .imm   = 0 }),/* r7 = *(u32*)(r0+0) r7 = index of svc lvl gid filter program */

		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_3, .src_reg = BPF_REG_7, .off   = 0, .imm   = 0 }),	/* r3=r7, r3 = index of svc lvl gid filter program */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_1, .src_reg = BPF_REG_6, .off   = 0, .imm   = 0 }),	/* r1=r6, r1 = ctx */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_2, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (svc_to_gid_jump_table_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (svc_to_gid_jump_table_fd)) >> 32 }),/* r2 = program_jump_table */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = tail_call() }),/* bpf_tail_call(r1, r2, r3) */

		((struct bpf_insn) {.code  = move64_imm,
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 1 }),	/* success case , no restriction rule */
		((struct bpf_insn) {.code  = filter_exit,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = 0 })/* return from filter */
	};

	QRTR_BPF_LOAD_FILTER(svc_lvl_acc_cntrl_pgfd, BPF_PROG_TYPE_SOCKET_FILTER,
						qmi_svc_lvl_acc_cntrl_filter,
						sizeof(qmi_svc_lvl_acc_cntrl_filter) /
						sizeof(qmi_svc_lvl_acc_cntrl_filter[0]),
						"USER", 0, bpf_log_buf, LOG_BUF_SIZE);
	if (svc_lvl_acc_cntrl_pgfd <= 0) {
		err(1, "BPF svc level access control filter program load failed fd %d %s",
				svc_lvl_acc_cntrl_pgfd, strerror(errno));
	}
	dprintf("BPF SVC level access control filter program fd %d\n",
		svc_lvl_acc_cntrl_pgfd);

	/* Attach Service level access control filter with qrtr */
	dprintf("\n\n Attaching Service level access control filter with qrtr\n\n");
	rc = ioctl(sock, QRTR_ATTACH_BPF, &svc_lvl_acc_cntrl_pgfd);
	if (rc)
		err(1,"BPF filter attach with QRTR failed %d", rc);


	for (;;) {
		dprintf("QMI Access Control Filtration ongoing...\n\n");
		sleep(5);
	}

	rc = ioctl(sock, QRTR_DETTACH_BPF, &svc_lvl_acc_cntrl_pgfd);
	dprintf("BPF filter Detached rc %d\n", rc);

	close(sock);

	return 0;
}

static void qrtr_set_svc_lvl_gid_filter(int gid_map_fd, int svc_to_gid_jump_table_fd,
					int msg_to_gid_jump_table_fd, int msg_id_map_fd,
					int node_id_table_fd, int indx)
{
	struct filter_arg arg;

	arg.svc_lvl_gid_map_fd = gid_map_fd;
	arg.msg_lvl_gid_map_fd = -1;
	arg.svc_to_gid_prgm_jump_table = svc_to_gid_jump_table_fd;
	arg.msg_to_gid_prgm_jump_table = msg_to_gid_jump_table_fd;
	arg.msg_id_map_fd = msg_id_map_fd;
	arg.node_id_table_fd = node_id_table_fd;
	arg.indx = indx;
	arg.filter_lvl = SVC_LEVEL;

	set_filter_program(&arg);
}

static void qrtr_set_msg_lvl_gid_filter(int gid_map_fd, int msg_to_gid_jump_table_fd,
					int msg_id_map_fd, int node_id_table_fd,
					int indx)
{
	struct filter_arg arg;

	arg.svc_lvl_gid_map_fd = -1;
	arg.msg_lvl_gid_map_fd = gid_map_fd;
	arg.svc_to_gid_prgm_jump_table = -1;
	arg.msg_to_gid_prgm_jump_table = msg_to_gid_jump_table_fd;
	arg.msg_id_map_fd = msg_id_map_fd;
	arg.node_id_table_fd = node_id_table_fd;
	arg.indx = indx;
	arg.filter_lvl = MSG_LEVEL;

	set_filter_program(&arg);
}

static void set_filter_program(struct filter_arg *arg)
{
	int prgm_jump_table_fd;
	int gid_filter_pgfd;
	int msg_id_map_fd;
	int gid_map_fd;
	int rc = 0;
	int key;

	if (arg->filter_lvl == SVC_LEVEL) {
		gid_map_fd = arg->svc_lvl_gid_map_fd;
		prgm_jump_table_fd = arg->svc_to_gid_prgm_jump_table;
	} else {
		gid_map_fd = arg->msg_lvl_gid_map_fd;
		prgm_jump_table_fd = arg->msg_to_gid_prgm_jump_table;
	}
	msg_id_map_fd = arg->msg_id_map_fd;

	if (gid_map_fd < 0 || prgm_jump_table_fd < 0 ||
		msg_id_map_fd < 0 || arg->node_id_table_fd < 0) {
		dprintf("%s invalid fd found gid_fd %d prgm_fd %d msg_id_fd %d node_id_fd %d\n",
			__func__, gid_map_fd, prgm_jump_table_fd, msg_id_map_fd,
			arg->node_id_table_fd);
		return;
	}

	/* filter program to handle gid filtration */
	struct bpf_insn gid_lvl_filter[] = {
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_6, .src_reg = BPF_REG_1, .off   = 0, .imm   = 0 }),

		((struct bpf_insn) {.code  = load_absolute(BPF_H),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = PKT_TYPE_OFFSET }),/* r0 = *(u16*)skb[PKT_TYPE_OFFSET], r0 = pkt_type */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 16 }),	/* convert pkt_type from BE to LE --> r0*/
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 10, .imm   = QRTR_TYPE_DATA }),/* jump to gid check in case of data pkt */

		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = BROADCAST_NODE_OFFSET }),/* r0 = *(u32 *)skb[BROADCAST_NODE_OFFSET], r0 = dest_node */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }),	/* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (arg->node_id_table_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (arg->node_id_table_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> value address */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 198, .imm   = 0 }),/* jump to success case if broadcast is not restricted to this node id */
		((struct bpf_insn) {.code  = jump_to_off,
		.dst_reg = 0, .src_reg = 0, .off   = 177, .imm   = 0 }),/* jump to failure case if broadcast is restricted to this node id */

		/* -- get gid_len field from skbuff --
		 * gid_len = load_byte(skb, GID_LEN_OFFSET);
		 */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = GID_LEN_OFFSET }),/* r0 = *(u32 *)skb[GID_LEN_OFFSET], r0 = gid_len */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_7, .src_reg = BPF_REG_0, .off   = 0, .imm   = 0 }),	/* r7 = r0, r7 = gid_len */

		/* -- compare egid from proc list against config list --
		 * egid = load_byte(skb, EFF_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &egid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = EFF_GID_OFFSET }),/* r0 = *(u32*)skb[EFF_GID_OFFSET], r0 = egid */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }),	/* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> value address */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 167, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 1st supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _1st_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 163, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _1st_GID_OFFSET }),/* r0 = *(u32*)skb[_1st_GID_OFFSET], r0 = 1st gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 156, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 2nd supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _2nd_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 152, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _2nd_GID_OFFSET }),/* r0 = *(u32*)skb[_2nd_GID_OFFSET], r0 = 2nd gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 145, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 3rd supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _3rd_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 141, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _3rd_GID_OFFSET }),/* r0 = *(u32*)skb[_3rd_GID_OFFSET], r0 = 3rd gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 134, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 4th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _4th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 130, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _4th_GID_OFFSET }),/* r0 = *(u32*)skb[_4th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 123, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 5th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _5th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 119, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _5th_GID_OFFSET }),/* r0 = *(u32*)skb[_5th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 112, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 6th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _6th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 108, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _6th_GID_OFFSET }),/* r0 = *(u32*)skb[_6th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 101, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 7th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _7th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 97, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _7th_GID_OFFSET }),/* r0 = *(u32*)skb[_7th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 90, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 8th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _8th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 86, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _8th_GID_OFFSET }),/* r0 = *(u32*)skb[_8th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 79, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 9th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _9th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 75, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _9th_GID_OFFSET }),/* r0 = *(u32*)skb[_9th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD) | BPF_K,
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 68, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 10th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _10th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 64, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _10th_GID_OFFSET }),/* r0 = *(u32*)skb[_10th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 57, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 11th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _11th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 53, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _11th_GID_OFFSET }),/* r0 = *(u32*)skb[_11th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 46, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 12th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _12th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 42, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _12th_GID_OFFSET }),/* r0 = *(u32*)skb[_12th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 35, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 13th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _13th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 31, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _13th_GID_OFFSET }),/* r0 = *(u32*)skb[_13th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 24, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 14th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _14th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 20, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _14th_GID_OFFSET }),/* r0 = *(u32*)skb[_14th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 13, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		/* -- compare 15th supplementary gid from proc list against config list --
		 * gid_len--;
		 * if (!gid_len)
		 *		return failure;
		 * sgid = load_byte(skb, _15th_GID_OFFSET);
		 * value = bpf_map_lookup_elem(&gid_fd, &sgid);
		 * if (value)
		 * 		goto filter_type;
		 */
		((struct bpf_insn) {.code  = alu_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 0, .imm   = -1 }),	/* gid_len -= 1 */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_7, .src_reg = 0, .off   = 9, .imm   = 0 }),/* jump to failure return if gid_len = 0 , all gid checked no matching gid found */
		((struct bpf_insn) {.code  = load_absolute(BPF_W),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = _15th_GID_OFFSET }),/* r0 = *(u32*)skb[_15th_GID_OFFSET], r0 = 4th gid from suppli list */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 32 }),	/* convert from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0 */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4 */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (gid_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (gid_map_fd)) >> 32 }),/* r1 = gid_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> fd(addr) of gid map */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JNE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 2, .imm   = 0 }),/* jump to filter_type: case if key(gid) is found */

		((struct bpf_insn) {.code  = move64_imm,
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 0x0 }),/* failure case where matching gid not found */
		((struct bpf_insn) {.code  = jump_to_off,
		.dst_reg = 0, .src_reg = 0, .off   = 19, .imm   = 0 }),/* jump to exit */

/* filter_type: */((struct bpf_insn) {.code  = move64_imm,
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = arg->filter_lvl }),/* check filter type and take next action */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 2, .imm   = SVC_LEVEL }),/* if filter type is SVC_LEVEL then check msg id level access */
		((struct bpf_insn) {.code  = move64_imm,
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 0x1 }),/* in case of MSG_LEVEL filter return success as matching gid already found */
		((struct bpf_insn) {.code  = jump_to_off,
		.dst_reg = 0, .src_reg = 0, .off   = 15, .imm   = 0 }),/* jump to exit */

		/* -- get msg id field from skbuff & lookup msg id key in msg_id_map_fd --
		 * msg_id(key) = load_byte(skb, QMI_MSG_ID_OFFSET);
		 * value = bpf_map_lookup_elem(&msg_id_map_fd, &msg_id);
		 * if (!value)
		 * 		return success;
		 */
		((struct bpf_insn) {.code  = load_absolute(BPF_H),
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = QMI_MSG_ID_OFFSET }),/* r0 = *(u16*)skb[QMI_MSG_ID_OFFSET], r0 = msg_id */
		((struct bpf_insn) {.code  = endian(BPF_FROM_BE),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 16 }),	/* convert msg_id from BE to LE --> r0*/
		((struct bpf_insn) {.code  = store_memory(BPF_W),
		.dst_reg = BPF_REG_10, .src_reg = BPF_REG_0, .off   = -4, .imm   = 0 }),/* *(u32 *)(fp - 4) = r0, push msg_id to stack */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_2, .src_reg = BPF_REG_10, .off   = 0, .imm   = 0 }), /* r2 = r10, r2 = fp */
		((struct bpf_insn) {.code  = alu64_imm_opcode(BPF_ADD),
		.dst_reg = BPF_REG_2, .src_reg = 0, .off   = 0, .imm   = -4 }),	/* r2 = r2 - 4, pointing to msg_id */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_1, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (msg_id_map_fd) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (msg_id_map_fd)) >> 32 }),/* r1 = r8, r1 = msg_id_map_fd */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = map_lookup_elem() }),/* r0 = bpf_map_lookup_elem(r1, r2) r0 -> value address */
	  /*@@*/((struct bpf_insn) {.code  = jump_imm_opcode(BPF_JEQ),
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 5, .imm   = 0 }),/* jump to success return if key(msg id) is not found */

		((struct bpf_insn) {.code  = load_memory(BPF_W),
		.dst_reg = BPF_REG_3, .src_reg = BPF_REG_0, .off   = 0, .imm   = 0 }),/* r3 = *(u32*)(r0+0) r3 = index of msg lvl gid filter program */
		((struct bpf_insn) {.code  = move64_reg,
		.dst_reg = BPF_REG_1, .src_reg = BPF_REG_6, .off   = 0, .imm   = 0 }), /* r1 = r6, r1 = ctx */
		/*$*/
		((struct bpf_insn) {.code  = load64_imm,
		.dst_reg = BPF_REG_2, .src_reg = BPF_PSEUDO_MAP_FD, .off   = 0, .imm   = (__u32) (arg->msg_to_gid_prgm_jump_table) }),
		((struct bpf_insn) {.code  = 0, /* zero is reserved opcode */
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = ((__u64) (arg->msg_to_gid_prgm_jump_table)) >> 32 }),/* r2 = program_jump_table */
		((struct bpf_insn) {.code  = func_call,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = tail_call() }),/* bpf_tail_call(r1, r2, r3) */

		((struct bpf_insn) {.code  = move64_imm,
		.dst_reg = BPF_REG_0, .src_reg = 0, .off   = 0, .imm   = 0x1 }),/* success case where matching gid is found */
		((struct bpf_insn) {.code  = filter_exit,
		.dst_reg = 0, .src_reg = 0, .off   = 0, .imm   = 0 })/* return from filter */
	};

	QRTR_BPF_LOAD_FILTER(gid_filter_pgfd, BPF_PROG_TYPE_SOCKET_FILTER, gid_lvl_filter,
						sizeof(gid_lvl_filter) / sizeof(gid_lvl_filter[0]),
						"USER", 0, bpf_log_buf, LOG_BUF_SIZE);
	if (gid_filter_pgfd <= 0) {
		err(1, " GID filter program load failed fd %d %s",
			gid_filter_pgfd, strerror(errno));
	}
	dprintf(" GID filter program fd %d \n", gid_filter_pgfd);

	key = arg->indx;
	QRTR_BPF_MAP_UPDATE(rc, prgm_jump_table_fd, &key, &gid_filter_pgfd, BPF_ANY);
	if (rc)
		err(1, "failed to update svc_to_gid_jump_table_fd rc %d '%s'",
			rc, strerror(errno));
	dprintf("prgm_jump_table_fd map with key(indx) %d updated with value %d(fd) rc %d\n",
			key, gid_filter_pgfd, rc);

}
