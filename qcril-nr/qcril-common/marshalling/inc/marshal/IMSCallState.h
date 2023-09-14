/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_CALL_STATE
#define _QTI_MARSHALLING_IMS_CALL_STATE

#include <Marshal.h>
#include <telephony/ril_ims.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_SipErrorInfo>(const RIL_IMS_SipErrorInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_SipErrorInfo>(RIL_IMS_SipErrorInfo &arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_SipErrorInfo>(RIL_IMS_SipErrorInfo &arg);

template <>
Marshal::Result Marshal::write<RIL_IMS_CallFailCauseResponse>(const RIL_IMS_CallFailCauseResponse &arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_CallFailCauseResponse>(RIL_IMS_CallFailCauseResponse &arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_CallFailCauseResponse>(RIL_IMS_CallFailCauseResponse &arg);

template <>
Marshal::Result Marshal::write<RIL_IMS_VerstatInfo>(const RIL_IMS_VerstatInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_VerstatInfo>(RIL_IMS_VerstatInfo &arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_CallProgressInfo>(const RIL_IMS_CallProgressInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_CallProgressInfo>(RIL_IMS_CallProgressInfo &arg) const;

template <>
Marshal::Result Marshal::write<RIL_IMS_CallInfo>(const RIL_IMS_CallInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_CallInfo>(RIL_IMS_CallInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_CallInfo>(RIL_IMS_CallInfo &arg);

#endif // _QTI_MARSHALLING_IMS_CALL_STATE
