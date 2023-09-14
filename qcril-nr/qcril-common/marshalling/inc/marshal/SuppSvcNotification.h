/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_SUPP_SVC_NOTIFICATION
#define _QTI_MARSHALLING_SUPP_SVC_NOTIFICATION

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SuppSvcNotification>(const RIL_SuppSvcNotification& arg);
template <>
Marshal::Result Marshal::read<RIL_SuppSvcNotification>(RIL_SuppSvcNotification& arg) const;
template <>
Marshal::Result Marshal::release<RIL_SuppSvcNotification>(RIL_SuppSvcNotification& arg);

#endif
