/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RILKEEPALIVEREQUEST
#define _QTI_MARSHALLING_RILKEEPALIVEREQUEST

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_KeepaliveRequest>(const RIL_KeepaliveRequest &arg);
template <>
Marshal::Result Marshal::read<RIL_KeepaliveRequest>(RIL_KeepaliveRequest &arg) const;

#endif