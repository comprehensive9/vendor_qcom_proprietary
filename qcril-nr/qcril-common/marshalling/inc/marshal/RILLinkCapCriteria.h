/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RILLINKCAPCRITERIA
#define _QTI_MARSHALLING_RILLINKCAPCRITERIA

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_LinkCapCriteria>(const RIL_LinkCapCriteria &arg);
template <>
Marshal::Result Marshal::read<RIL_LinkCapCriteria>(RIL_LinkCapCriteria &arg) const;
template <>
Marshal::Result Marshal::release<RIL_LinkCapCriteria>(RIL_LinkCapCriteria &arg);
#endif