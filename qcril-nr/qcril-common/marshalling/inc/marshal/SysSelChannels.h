/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_SYS_SELECTION_CHANNELS
#define _QTI_MARSHALLING_SYS_SELECTION_CHANNELS

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SysSelChannels>(
    const RIL_SysSelChannels& arg);
template <>
Marshal::Result Marshal::read<RIL_SysSelChannels>(
    RIL_SysSelChannels& arg) const;

#endif
