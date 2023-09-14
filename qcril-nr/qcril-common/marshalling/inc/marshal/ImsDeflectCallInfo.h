/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_DEFLECTCALLINFO
#define _QTI_MARSHALLING_IMS_DEFLECTCALLINFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_DeflectCallInfo>(const RIL_IMS_DeflectCallInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_DeflectCallInfo>(RIL_IMS_DeflectCallInfo& arg) const;

#endif
