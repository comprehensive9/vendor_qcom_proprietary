/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RADIO_CAPABILITY
#define _QTI_MARSHALLING_RADIO_CAPABILITY

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_RadioCapability>(const RIL_RadioCapability &arg);
template <>
Marshal::Result Marshal::read<RIL_RadioCapability>(RIL_RadioCapability &arg) const;

#endif // _QTI_MARSHALLING_RADIO_CAPABILITY
