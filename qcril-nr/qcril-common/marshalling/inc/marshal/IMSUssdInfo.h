/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_USSD_INFO
#define _QTI_MARSHALLING_IMS_USSD_INFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_UssdInfo>(const RIL_IMS_UssdInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_UssdInfo>(RIL_IMS_UssdInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_UssdInfo>(RIL_IMS_UssdInfo& arg);

#endif
