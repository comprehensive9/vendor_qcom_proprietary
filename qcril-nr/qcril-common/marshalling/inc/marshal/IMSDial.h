/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_DIAL
#define _QTI_MARSHALLING_IMS_DIAL

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>


template <>
Marshal::Result Marshal::write<RIL_IMS_MultiIdentityLineInfo>(
        const RIL_IMS_MultiIdentityLineInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_MultiIdentityLineInfo>(
        RIL_IMS_MultiIdentityLineInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_MultiIdentityLineInfo>(
        RIL_IMS_MultiIdentityLineInfo& arg);


template <>
Marshal::Result Marshal::write<RIL_IMS_CallDetails>(const RIL_IMS_CallDetails& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_CallDetails>(RIL_IMS_CallDetails& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_Dial>(const RIL_IMS_Dial& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_Dial>(RIL_IMS_Dial& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_Dial>(RIL_IMS_Dial& arg);

#endif
