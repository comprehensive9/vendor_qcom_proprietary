/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_GSM_BROADCAST_SMS
#define _QTI_MARSHALLING_GSM_BROADCAST_SMS

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_GSM_BroadcastSmsConfigInfo>(
    const RIL_GSM_BroadcastSmsConfigInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_GSM_BroadcastSmsConfigInfo>(
    RIL_GSM_BroadcastSmsConfigInfo& arg) const;

#endif  // _QTI_MARSHALLING_GSM_BROADCAST_SMS
