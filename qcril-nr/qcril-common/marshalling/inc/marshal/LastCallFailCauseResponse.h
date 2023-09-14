/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_LAST_CALL_FAIL_CAUSE_RESP
#define _QTI_MARSHALLING_LAST_CALL_FAIL_CAUSE_RESP

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_LastCallFailCauseInfo>(const RIL_LastCallFailCauseInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_LastCallFailCauseInfo>(RIL_LastCallFailCauseInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_LastCallFailCauseInfo>(RIL_LastCallFailCauseInfo& arg);

#endif
