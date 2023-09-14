/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_ANSWER
#define _QTI_MARSHALLING_IMS_ANSWER

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_Answer>(const RIL_IMS_Answer& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_Answer>(RIL_IMS_Answer& arg) const;

#endif
