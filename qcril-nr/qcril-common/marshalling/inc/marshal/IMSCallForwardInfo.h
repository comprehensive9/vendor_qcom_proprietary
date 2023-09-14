/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_CALL_FORWARD_INFO
#define _QTI_MARSHALLING_IMS_CALL_FORWARD_INFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CallFwdTimerInfo>(const RIL_IMS_CallFwdTimerInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_CallFwdTimerInfo>(RIL_IMS_CallFwdTimerInfo& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_CallForwardInfo>(const RIL_IMS_CallForwardInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_CallForwardInfo>(RIL_IMS_CallForwardInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_CallForwardInfo>(RIL_IMS_CallForwardInfo& arg);

template <>
Marshal::Result Marshal::write<RIL_IMS_QueryCallForwardStatusInfo>(
    const RIL_IMS_QueryCallForwardStatusInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_QueryCallForwardStatusInfo>(
    RIL_IMS_QueryCallForwardStatusInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_QueryCallForwardStatusInfo>(
    RIL_IMS_QueryCallForwardStatusInfo& arg);

template <>
Marshal::Result Marshal::write<RIL_IMS_SetCallForwardStatusInfo>(
    const RIL_IMS_SetCallForwardStatusInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_SetCallForwardStatusInfo>(
    RIL_IMS_SetCallForwardStatusInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_SetCallForwardStatusInfo>(
    RIL_IMS_SetCallForwardStatusInfo& arg);

#endif
