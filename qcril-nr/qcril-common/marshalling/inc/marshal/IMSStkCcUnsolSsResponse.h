/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_STK_CC_UNSOL_SS_RESPONSE
#define _QTI_MARSHALLING_IMS_STK_CC_UNSOL_SS_RESPONSE

#include <Marshal.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CfData>(const RIL_IMS_CfData& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_CfData>(RIL_IMS_CfData& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_StkCcUnsolSsResponse>(const RIL_IMS_StkCcUnsolSsResponse& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_StkCcUnsolSsResponse>(RIL_IMS_StkCcUnsolSsResponse& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_StkCcUnsolSsResponse>(RIL_IMS_StkCcUnsolSsResponse& arg);

#endif
