/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __CNSS_GENL_H__
#define __CNSS_GENL_H__
#if !defined(ANDROID) && !defined(IPQ)
static inline int cnss_genl_init(void)
{
	return 0;
}
static inline void cnss_genl_exit(void) {}
static inline int cnss_genl_recvmsgs(void)
{
	return 0;
}
static inline int cnss_genl_get_fd(void)
{
	return -1;
}
static inline int cnss_genl_send_data(uint8_t type, uint32_t instance_id,
				      uint32_t value)
{
	UNUSED(type);
	UNUSED(instance_id);
	UNUSED(value);
	return 0;
}
#else
int cnss_genl_init(void);
void cnss_genl_exit(void);
int cnss_genl_recvmsgs(void);
int cnss_genl_get_fd(void);
#endif
#endif
