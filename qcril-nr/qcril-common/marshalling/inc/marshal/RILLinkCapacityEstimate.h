/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RILLINKCAPACITYESTIMATE
#define _QTI_MARSHALLING_RILLINKCAPACITYESTIMATE

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_LinkCapacityEstimate>(const RIL_LinkCapacityEstimate &arg);
template <>
Marshal::Result Marshal::read<RIL_LinkCapacityEstimate>(RIL_LinkCapacityEstimate &arg) const;
template <>
Marshal::Result Marshal::release<RIL_LinkCapacityEstimate>(RIL_LinkCapacityEstimate &arg);
#endif