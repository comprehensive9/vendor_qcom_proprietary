/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_CONFIG_INFO
#define _QTI_MARSHALLING_IMS_CONFIG_INFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ConfigInfo>(const RIL_IMS_ConfigInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_ConfigInfo>(RIL_IMS_ConfigInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_ConfigInfo>(RIL_IMS_ConfigInfo& arg);

#endif
