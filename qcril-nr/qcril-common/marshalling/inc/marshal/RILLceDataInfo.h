/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RILLCEDATAINFO
#define _QTI_MARSHALLING_RILLCEDATAINFO

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_LceDataInfo>(const RIL_LceDataInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_LceDataInfo>(RIL_LceDataInfo &arg) const;

#endif
