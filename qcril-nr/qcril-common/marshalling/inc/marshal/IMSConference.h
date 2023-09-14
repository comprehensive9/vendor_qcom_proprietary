/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_CONFERENCE
#define _QTI_MARSHALLING_IMS_CONFERENCE

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_RefreshConferenceInfo>(const RIL_IMS_RefreshConferenceInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_RefreshConferenceInfo>(RIL_IMS_RefreshConferenceInfo &arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_RefreshConferenceInfo>(RIL_IMS_RefreshConferenceInfo &arg);

#endif
