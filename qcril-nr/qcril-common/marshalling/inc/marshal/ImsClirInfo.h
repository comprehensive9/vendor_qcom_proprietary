/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_CLIRINFO
#define _QTI_MARSHALLING_IMS_CLIRINFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ClirInfo>(const RIL_IMS_ClirInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_ClirInfo>(RIL_IMS_ClirInfo& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_SetClirInfo>(const RIL_IMS_SetClirInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_SetClirInfo>(RIL_IMS_SetClirInfo& arg) const;

#endif
