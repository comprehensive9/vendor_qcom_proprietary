/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CDMA_CALL_WAITING
#define _QTI_MARSHALLING_CDMA_CALL_WAITING

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CDMA_CallWaiting_v6>(const RIL_CDMA_CallWaiting_v6& arg);
template <>
Marshal::Result Marshal::read<RIL_CDMA_CallWaiting_v6>(RIL_CDMA_CallWaiting_v6& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CDMA_CallWaiting_v6>(RIL_CDMA_CallWaiting_v6& arg);

#endif
