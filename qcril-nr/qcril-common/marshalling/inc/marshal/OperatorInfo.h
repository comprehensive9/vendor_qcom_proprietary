/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_OPERATOR_INFO
#define _QTI_MARSHALLING_OPERATOR_INFO

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_OperatorInfo>(const RIL_OperatorInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_OperatorInfo>(RIL_OperatorInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_OperatorInfo>(RIL_OperatorInfo& arg);

#endif
