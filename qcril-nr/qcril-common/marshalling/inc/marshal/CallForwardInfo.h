/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CALL_FORWARD_INFO
#define _QTI_MARSHALLING_CALL_FORWARD_INFO

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CallForwardInfo>(const RIL_CallForwardInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_CallForwardInfo>(RIL_CallForwardInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CallForwardInfo>(RIL_CallForwardInfo& arg);

#endif
