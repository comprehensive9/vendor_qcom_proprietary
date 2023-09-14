/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RILDataProfileInfo
#define _QTI_MARSHALLING_RILDataProfileInfo

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_DataProfileInfo_IRadio_1_4>(const RIL_DataProfileInfo_IRadio_1_4 &arg);
template <>
Marshal::Result Marshal::read<RIL_DataProfileInfo_IRadio_1_4>(RIL_DataProfileInfo_IRadio_1_4 &arg) const;
template <>
Marshal::Result Marshal::release<RIL_DataProfileInfo_IRadio_1_4>(RIL_DataProfileInfo_IRadio_1_4 &arg);
#endif

