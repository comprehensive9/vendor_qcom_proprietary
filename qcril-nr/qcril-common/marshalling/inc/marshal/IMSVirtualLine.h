/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_VIRTUAL_LINE
#define _QTI_MARSHALLING_IMS_VIRTUAL_LINE

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>


template <>
Marshal::Result Marshal::write<RIL_IMS_QueryVirtualLineInfoResponse>(
        const RIL_IMS_QueryVirtualLineInfoResponse& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_QueryVirtualLineInfoResponse>(
        RIL_IMS_QueryVirtualLineInfoResponse& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_QueryVirtualLineInfoResponse>(
        RIL_IMS_QueryVirtualLineInfoResponse& arg);

#endif
