/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifdef CONFIG_CNSS_DP
int wlan_dp_service_start(void);
void wlan_dp_service_stop(void);
#else
static int wlan_dp_service_start(void) { return 0; }
static void wlan_dp_service_stop(void) { return; }
#endif
