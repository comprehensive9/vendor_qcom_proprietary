#ifndef DIAG_LSM_COMM_H
#define DIAG_LSM_COMM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Internal Header File for communication between diag lib, driver

GENERAL DESCRIPTION

Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
int diag_lsm_comm_open(void);
int diag_lsm_comm_write(int fd, unsigned char buf[], int bytes, int flags);
int diag_lsm_comm_ioctl(int fd, unsigned long request, void *buf, unsigned int len);

#endif /* DIAG_LSM_COMM_H */
