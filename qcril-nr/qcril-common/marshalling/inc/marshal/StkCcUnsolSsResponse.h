/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_STK_CC_UNSOL_SS_RESPONSE
#define _QTI_MARSHALLING_STK_CC_UNSOL_SS_RESPONSE

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CfData>(const RIL_CfData& arg);
template <>
Marshal::Result Marshal::read<RIL_CfData>(RIL_CfData& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CfData>(RIL_CfData& arg);

template <>
Marshal::Result Marshal::write<RIL_StkCcUnsolSsResponse>(const RIL_StkCcUnsolSsResponse& arg);
template <>
Marshal::Result Marshal::read<RIL_StkCcUnsolSsResponse>(RIL_StkCcUnsolSsResponse& arg) const;
template <>
Marshal::Result Marshal::release<RIL_StkCcUnsolSsResponse>(RIL_StkCcUnsolSsResponse& arg);

#endif
