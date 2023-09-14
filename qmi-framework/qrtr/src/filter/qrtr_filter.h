/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QRTR_FILTER_H
#define _QRTR_FILTER_H

#include <linux/bpf_common.h>
#include <sys/syscall.h>
#include <linux/bpf.h>
#include <unistd.h>

/* BPF op-code wrapper definitions */
#define alu64_reg_opcode(x)		BPF_ALU64 | BPF_OP(x) | BPF_X
#define alu_reg_opcode(x)		BPF_ALU | BPF_OP(x) | BPF_X
#define alu64_imm_opcode(x)		BPF_ALU64 | BPF_OP(x) | BPF_K
#define alu_imm_opcode(x)		BPF_ALU | BPF_OP(x) | BPF_K
#define endian(x)				BPF_ALU | BPF_END | BPF_SRC(x)
#define move64_reg				BPF_ALU64 | BPF_MOV | BPF_X
#define move32_reg				BPF_ALU | BPF_MOV | BPF_X
#define move64_imm				BPF_ALU64 | BPF_MOV | BPF_K
#define move32_imm				BPF_ALU | BPF_MOV | BPF_K
#define load64_imm				BPF_LD | BPF_DW | BPF_IMM
#define load_absolute(x)		BPF_LD | BPF_SIZE(x) | BPF_ABS
#define load_indirect(x)		BPF_LD | BPF_SIZE(x) | BPF_IND
#define load_memory(x)			BPF_LDX | BPF_SIZE(x) | BPF_MEM
#define store_memory(x)			BPF_STX | BPF_SIZE(x) | BPF_MEM
#define jump_reg_opcode(x)		BPF_JMP | BPF_OP(x) | BPF_X
#define jump_imm_opcode(x)		BPF_JMP | BPF_OP(x) | BPF_K
#define jump_to_off				BPF_JMP | BPF_JA
#define func_call				BPF_JMP | BPF_CALL
#define filter_exit				BPF_JMP | BPF_EXIT

/* BPF function declaration */
#define map_lookup_elem()		1
#define tail_call()				12

#define to_u64(x)	(__u64)(x)

/* create new bpf map table */
#define QRTR_BPF_MAP_CREATE(fd, type, k_size, v_size, max_e, flags)	\
{							\
	union bpf_attr b_attr;	\
	memset(&b_attr, '\0', sizeof(b_attr));	\
	b_attr.map_type = type;	\
    b_attr.key_size = k_size;	\
    b_attr.value_size = v_size;	\
    b_attr.max_entries = max_e;	\
    b_attr.map_flags = flags;	\
	fd = syscall(SYS_bpf, BPF_MAP_CREATE, &b_attr, sizeof(b_attr));	\
}

/* lookup element from bpf map table using key */
#define QRTR_BPF_MAP_LOOKUP(ret, fd, key, value)	\
{							\
	union bpf_attr b_attr;	\
	bzero(&b_attr, sizeof(b_attr));	\
    b_attr.map_fd = fd;	\
    b_attr.key = to_u64(key);	\
    b_attr.value = to_u64(value);	\
	ret = syscall(SYS_bpf, BPF_MAP_LOOKUP_ELEM, &b_attr, sizeof(b_attr));	\
}

/* update element on bpf map table with key */
#define QRTR_BPF_MAP_UPDATE(ret, fd, k, val, flag)	\
{							\
	union bpf_attr b_attr;	\
	bzero(&b_attr, sizeof(b_attr));	\
    b_attr.map_fd = fd;	\
    b_attr.key = to_u64(k);	\
    b_attr.value = to_u64(val);	\
	b_attr.flags = flag;	\
	ret = syscall(SYS_bpf, BPF_MAP_UPDATE_ELEM, &b_attr, sizeof(b_attr));	\
}

/* delete element from bpf map table with key */
#define QRTR_BPF_MAP_DELETE(ret, fd, key)	\
{							\
	union bpf_attr b_attr;	\
	bzero(&b_attr, sizeof(b_attr));	\
    b_attr.map_fd = fd;	\
    b_attr.key = ptr_to_u64(key);	\
	ret = syscall(SYS_bpf, BPF_MAP_DELETE_ELEM, &b_attr, sizeof(b_attr));	\
}

/* load bpf program to kernel */
#define QRTR_BPF_LOAD_FILTER(fd, type, ins, ins_c, lic, ver, log, log_s)	\
{							\
	union bpf_attr b_attr;	\
	bzero(&b_attr, sizeof(b_attr));	\
    b_attr.prog_type = type;	\
    b_attr.insn_cnt = (__u32)ins_c;	\
    b_attr.insns = to_u64(ins);	\
    b_attr.license = to_u64(lic);	\
    b_attr.log_buf = to_u64(log);	\
    b_attr.log_size = log_s;	\
    b_attr.log_level = 1;	\
    b_attr.kern_version = ver;	\
    log[0] = 0;	\
	fd = syscall(SYS_bpf, BPF_PROG_LOAD, &b_attr, sizeof(b_attr));	\
}

#endif /* _QRTR_FILTER_H */
