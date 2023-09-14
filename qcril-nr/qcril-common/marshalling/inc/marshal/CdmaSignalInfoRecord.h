/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CDMA_SIGNAL_INFO_RECORD
#define _QTI_MARSHALLING_CDMA_SIGNAL_INFO_RECORD

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CDMA_SignalInfoRecord>(const RIL_CDMA_SignalInfoRecord& arg);
template <>
Marshal::Result Marshal::read<RIL_CDMA_SignalInfoRecord>(RIL_CDMA_SignalInfoRecord& arg) const;

#endif
