/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_HANDOVER
#define _QTI_MARSHALLING_IMS_HANDOVER

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_HandoverInfo>(const RIL_IMS_HandoverInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_HandoverInfo>(RIL_IMS_HandoverInfo &arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_HandoverInfo>(RIL_IMS_HandoverInfo &arg);

#endif // _QTI_MARSHALLING_IMS_HANDOVER

