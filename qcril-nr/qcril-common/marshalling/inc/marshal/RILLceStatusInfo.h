/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RILLCESTATUSINFO
#define _QTI_MARSHALLING_RILLCESTATUSINFO

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_LceStatusInfo>(const RIL_LceStatusInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_LceStatusInfo>(RIL_LceStatusInfo &arg) const;

#endif