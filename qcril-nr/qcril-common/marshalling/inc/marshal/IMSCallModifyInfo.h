/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_CALL_MODIFY_INFO
#define _QTI_MARSHALLING_IMS_CALL_MODIFY_INFO

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CallModifyInfo>(const RIL_IMS_CallModifyInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_CallModifyInfo>(RIL_IMS_CallModifyInfo& arg) const;

#endif
