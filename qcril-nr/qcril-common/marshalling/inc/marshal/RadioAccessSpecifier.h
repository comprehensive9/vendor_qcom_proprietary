/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RADIO_ACCESS_SPECIFIER
#define _QTI_MARSHALLING_RADIO_ACCESS_SPECIFIER

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_RadioAccessSpecifier>(
    const RIL_RadioAccessSpecifier& arg);
template <>
Marshal::Result Marshal::read<RIL_RadioAccessSpecifier>(
    RIL_RadioAccessSpecifier& arg) const;

#endif
