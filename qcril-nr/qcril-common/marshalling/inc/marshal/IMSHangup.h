/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_HANGUP
#define _QTI_MARSHALLING_IMS_HANGUP

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_Hangup>(const RIL_IMS_Hangup& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_Hangup>(RIL_IMS_Hangup& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_Hangup>(RIL_IMS_Hangup& arg);

#endif
