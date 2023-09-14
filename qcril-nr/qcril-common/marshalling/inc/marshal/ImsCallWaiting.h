/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_CALL_WAITING
#define _QTI_MARSHALLING_IMS_CALL_WAITING

#include <Marshal.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CallWaitingSettings>(const RIL_IMS_CallWaitingSettings& arg);

template <>
Marshal::Result Marshal::read<RIL_IMS_CallWaitingSettings>(RIL_IMS_CallWaitingSettings& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_QueryCallWaitingResponse>(const RIL_IMS_QueryCallWaitingResponse& arg);

template <>
Marshal::Result Marshal::read<RIL_IMS_QueryCallWaitingResponse>(RIL_IMS_QueryCallWaitingResponse& arg) const;

template <>
Marshal::Result Marshal::release<RIL_IMS_QueryCallWaitingResponse>(RIL_IMS_QueryCallWaitingResponse& arg);

#endif
