/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CDMA_INFORMATION_RECORDS
#define _QTI_MARSHALLING_CDMA_INFORMATION_RECORDS

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CDMA_InformationRecords>(const RIL_CDMA_InformationRecords& arg);
template <>
Marshal::Result Marshal::read<RIL_CDMA_InformationRecords>(RIL_CDMA_InformationRecords& arg) const;

#endif
