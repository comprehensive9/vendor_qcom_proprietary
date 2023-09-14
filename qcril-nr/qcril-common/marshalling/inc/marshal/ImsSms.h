/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_SMS_MESSAGE
#define _QTI_MARSHALLING_IMS_SMS_MESSAGE

#include <Marshal.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_SmsMessage>(const RIL_IMS_SmsMessage& arg);

template <>
Marshal::Result Marshal::read<RIL_IMS_SmsMessage>(RIL_IMS_SmsMessage& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_SendSmsResponse>(const RIL_IMS_SendSmsResponse& arg);

template <>
Marshal::Result Marshal::read<RIL_IMS_SendSmsResponse>(RIL_IMS_SendSmsResponse& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_SmsAck>(const RIL_IMS_SmsAck& arg);

template <>
Marshal::Result Marshal::read<RIL_IMS_SmsAck>(RIL_IMS_SmsAck& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_IncomingSms>(const RIL_IMS_IncomingSms& arg);

template <>
Marshal::Result Marshal::read<RIL_IMS_IncomingSms>(RIL_IMS_IncomingSms& arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_SmsStatusReport>(const RIL_IMS_SmsStatusReport& arg);

template <>
Marshal::Result Marshal::read<RIL_IMS_SmsStatusReport>(RIL_IMS_SmsStatusReport& arg) const;

#endif