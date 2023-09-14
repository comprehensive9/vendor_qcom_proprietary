/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_MARSHALLING_SIM_OPEN_CHANNEL_REQ
#define _QTI_MARSHALLING_SIM_OPEN_CHANNEL_REQ

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_OpenChannelParams>(const RIL_OpenChannelParams& arg);
template <>
Marshal::Result Marshal::read<RIL_OpenChannelParams>(RIL_OpenChannelParams& arg) const;
template <>
Marshal::Result Marshal::release<RIL_OpenChannelParams>(RIL_OpenChannelParams& arg);

#endif
