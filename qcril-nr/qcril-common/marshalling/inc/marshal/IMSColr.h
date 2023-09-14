/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_COLRINFO
#define _QTI_MARSHALLING_IMS_COLRINFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ColrInfo>(const RIL_IMS_ColrInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_ColrInfo>(RIL_IMS_ColrInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_ColrInfo>(RIL_IMS_ColrInfo& arg);

#endif
