/*
 * Copyright (c) 2019,2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __CNSS_USER_H__
#define __CNSS_USER_H__
#ifndef CONFIG_CNSS_USER
static inline int cnss_user_socket_init(int sock_type)
{
	UNUSED(sock_type);
	return 0;
}
static inline void cnss_user_socket_deinit(void)
{
}
static inline int cnss_user_get_fd(void)
{
	return -1;
}
static inline void handle_cnss_user_event(int fd)
{
	UNUSED(fd);
}
#else
int cnss_user_socket_init(int sock_type);
void cnss_user_socket_deinit(void);
int cnss_user_get_fd(void);
void handle_cnss_user_event(int fd);
#endif
#endif

