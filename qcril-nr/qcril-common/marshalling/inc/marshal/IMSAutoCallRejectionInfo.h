/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_AUTO_CALL_REJECTION_INFO
#define _QTI_MARSHALLING_IMS_AUTO_CALL_REJECTION_INFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_AutoCallRejectionInfo>(
    const RIL_IMS_AutoCallRejectionInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_AutoCallRejectionInfo>(
    RIL_IMS_AutoCallRejectionInfo& arg) const;

#endif
