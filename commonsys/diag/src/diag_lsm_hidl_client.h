#ifndef DIAG_LSM_HIDL_CLIENT_H
#define DIAG_LSM_HIDL_CLIENT_H
#ifdef __cplusplus
extern "C" {
#endif
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Internal Header File for communication between diag lib, driver

GENERAL DESCRIPTION

Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
int diag_lsm_comm_open(void);
int diag_lsm_comm_write(int fd, unsigned char buf[], int bytes);
int diag_lsm_comm_ioctl(int fd, unsigned long request, void *buf, unsigned int len);
int diag_lsm_comm_read(void);
void diag_kill_comm_read(void);
#ifdef __cplusplus
}
#endif
#endif /* DIAG_LSM_COMM_H */
