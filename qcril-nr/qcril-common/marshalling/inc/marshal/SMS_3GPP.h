/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_SMS_3GPP
#define _QTI_MARSHALLING_SMS_3GPP

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_GsmSmsMessage>(const RIL_GsmSmsMessage &arg);
template <>
Marshal::Result Marshal::read<RIL_GsmSmsMessage>(RIL_GsmSmsMessage &arg) const;

template<>
Marshal::Result Marshal::write<RIL_GsmSmsAck>(const RIL_GsmSmsAck &arg);
template <>
Marshal::Result Marshal::read<RIL_GsmSmsAck>(RIL_GsmSmsAck &arg) const;

#endif
