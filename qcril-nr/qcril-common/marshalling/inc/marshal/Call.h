/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CALL
#define _QTI_MARSHALLING_CALL

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_Call>(const RIL_Call& arg);
template <>
Marshal::Result Marshal::read<RIL_Call>(RIL_Call& arg) const;
template <>
Marshal::Result Marshal::release<RIL_Call>(RIL_Call& arg);

template <>
Marshal::Result Marshal::write<RIL_CallState>(const RIL_CallState& arg);
template <>
Marshal::Result Marshal::read<RIL_CallState>(RIL_CallState& arg) const;

template <>
Marshal::Result Marshal::write<RIL_AudioQuality>(const RIL_AudioQuality& arg);
template <>
Marshal::Result Marshal::read<RIL_AudioQuality>(RIL_AudioQuality& arg) const;

#endif
