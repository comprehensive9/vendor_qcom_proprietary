/******************************************************************************

                        N E T M G R _ X F R M . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_xfrm.h
  @brief   Network Manager XFRM Netlink module header file

  DESCRIPTION
  Header file for XFRM netlink module.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __NETMGR_XFRM_H__
#define __NETMGR_XFRM_H__

#include "netmgr_defs.h"
#include "wireless_data_service_v01.h"

int netmgr_xfrm_add_states
(
  wds_get_ipsec_static_sa_config_resp_msg_v01 *sa_config
);

int netmgr_xfrm_remove_states
(
  netmgr_xfrm_state_id_t *state
);

int netmgr_xfrm_add_policies
(
  netmgr_xfrm_state_id_t *sa_config,
  char *policy_saddr,
  char *policy_daddr,
  char *policy_proto,
  int is_tx
);

int netmgr_xfrm_remove_policies
(
  char *policy_saddr,
  char *policy_daddr,
  char *policy_proto,
  int is_tx
);

int netmgr_xfrm_add_policy_ex
(
  uint8_t dir
);

int netmgr_xfrm_remove_policy_ex
(
  uint8_t dir
);

#endif
