/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_REGISTRATION_BLOCK_STATUS
#define _QTI_MARSHALLING_IMS_REGISTRATION_BLOCK_STATUS

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_RegistrationBlockStatus>(const RIL_IMS_RegistrationBlockStatus& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_RegistrationBlockStatus>(RIL_IMS_RegistrationBlockStatus& arg) const;

#endif
