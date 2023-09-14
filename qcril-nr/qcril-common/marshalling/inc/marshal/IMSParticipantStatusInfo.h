/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_PARTICIPANT_STATUS_INFO
#define _QTI_MARSHALLING_IMS_PARTICIPANT_STATUS_INFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ParticipantStatusInfo>(const RIL_IMS_ParticipantStatusInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_ParticipantStatusInfo>(RIL_IMS_ParticipantStatusInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_ParticipantStatusInfo>(RIL_IMS_ParticipantStatusInfo& arg);

#endif
