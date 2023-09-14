/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_MESSAGE_WAITING_INFO
#define _QTI_MARSHALLING_IMS_MESSAGE_WAITING_INFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_MessageSummary>(const RIL_IMS_MessageSummary& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_MessageSummary>(RIL_IMS_MessageSummary& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_MessageDetails>(const RIL_IMS_MessageDetails& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_MessageDetails>(RIL_IMS_MessageDetails& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_MessageWaitingInfo>(const RIL_IMS_MessageWaitingInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_MessageWaitingInfo>(RIL_IMS_MessageWaitingInfo& arg) const;

#endif
