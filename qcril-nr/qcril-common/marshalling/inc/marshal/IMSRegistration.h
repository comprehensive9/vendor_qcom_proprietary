/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_REGISTRATION
#define _QTI_MARSHALLING_IMS_REGISTRATION

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_Registration>(const RIL_IMS_Registration& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_Registration>(RIL_IMS_Registration& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_Registration>(RIL_IMS_Registration& arg);

#endif
