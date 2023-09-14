/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_UUS_INFO
#define _QTI_MARSHALLING_UUS_INFO

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_UUS_Info>(const RIL_UUS_Info& arg);
template <>
Marshal::Result Marshal::read<RIL_UUS_Info>(RIL_UUS_Info& arg) const;
template <>
Marshal::Result Marshal::release<RIL_UUS_Info>(RIL_UUS_Info &arg);

#endif
