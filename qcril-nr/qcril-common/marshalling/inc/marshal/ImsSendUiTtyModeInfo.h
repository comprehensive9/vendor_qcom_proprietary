/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_SENDUITTYMODEINFO
#define _QTI_MARSHALLING_IMS_SENDUITTYMODEINFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_TtyNotifyInfo>(const RIL_IMS_TtyNotifyInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_TtyNotifyInfo>(RIL_IMS_TtyNotifyInfo& arg) const;

#endif
