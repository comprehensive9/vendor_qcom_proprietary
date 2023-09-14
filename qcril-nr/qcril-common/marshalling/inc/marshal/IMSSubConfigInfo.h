/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_SUBCONFIGINFO
#define _QTI_MARSHALLING_IMS_SUBCONFIGINFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_SubConfigInfo>(const RIL_IMS_SubConfigInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_SubConfigInfo>(RIL_IMS_SubConfigInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_SubConfigInfo>(RIL_IMS_SubConfigInfo& arg);

#endif
