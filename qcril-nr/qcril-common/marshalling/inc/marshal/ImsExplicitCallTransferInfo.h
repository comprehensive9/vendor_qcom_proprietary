/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_EXPLICITCALLTRANSFER
#define _QTI_MARSHALLING_IMS_EXPLICITCALLTRANSFER

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ExplicitCallTransfer>(const RIL_IMS_ExplicitCallTransfer& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_ExplicitCallTransfer>(RIL_IMS_ExplicitCallTransfer& arg) const;

#endif
