/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __CNSS_PLAT_IPC_QMI_H__
#define __CNSS_PLAT_IPC_QMI_H__

#ifdef CNSS_PLAT_IPC_QMI
int cnss_plat_ipc_qmi_init(void);
void cnss_plat_ipc_qmi_deinit(void);
void cnss_plat_ipc_qmi_msg_process(void);
int cnss_plat_ipc_qmi_get_fd(void);
#else
static inline
int cnss_plat_ipc_qmi_init(void)
{
	return 0;
}

static inline
void cnss_plat_ipc_qmi_deinit(void) {}

static inline
void cnss_plat_ipc_qmi_msg_process(void) {}

static inline
int cnss_plat_ipc_qmi_get_fd(void)
{
	return -1;
}
#endif

#endif
