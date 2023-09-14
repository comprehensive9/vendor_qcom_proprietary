#ifndef QSOCKET_IDS_H
#define QSOCKET_IDS_H
/******************************************************************************
  @file    qsocket_ids.h

  @brief   List of approved QSocket IDs

  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/qmimsgs.slpi/1.0/common/api/qsocket_ids.h#2 $
*******************************************************************************/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  enum {
    QSOCKET_MIN_ID = 0x1000,
    QSOCKET_TFTP_ID = QSOCKET_MIN_ID,
    QSOCKET_DIAG_ID,
    QSOCKET_MDM_CXM_ID,
    QSOCKET_WCN_CXM_ID,
    QSOCKET_SNS_CLIENT_SVC_ID,
    QSOCKET_MAX_ID,
    QSOCKET_VENDOR_MIN_ID = 0x1800,
    QSOCKET_VENDOR_MAX_ID = 0x2000
  }qsocket_ids_enum;

#ifdef __cplusplus
}
#endif
#endif

