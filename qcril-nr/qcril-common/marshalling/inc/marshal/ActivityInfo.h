/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_CALL_WAITING
#define _QTI_MARSHALLING_IMS_CALL_WAITING

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_ActivityStatsInfo>(const RIL_ActivityStatsInfo& arg);

template <>
Marshal::Result Marshal::read<RIL_ActivityStatsInfo>(RIL_ActivityStatsInfo& arg) const;

#endif
