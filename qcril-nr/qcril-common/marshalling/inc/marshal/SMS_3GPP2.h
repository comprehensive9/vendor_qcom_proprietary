/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_SMS_3GPP2
#define _QTI_MARSHALLING_SMS_3GPP2

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CDMA_SMS_Message>(const RIL_CDMA_SMS_Message &arg);
template <>
Marshal::Result Marshal::read<RIL_CDMA_SMS_Message>(RIL_CDMA_SMS_Message &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CDMA_SMS_Ack>(const RIL_CDMA_SMS_Ack &arg);
template <>
Marshal::Result Marshal::read<RIL_CDMA_SMS_Ack>(RIL_CDMA_SMS_Ack &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CDMA_BroadcastSmsConfigInfo>(const RIL_CDMA_BroadcastSmsConfigInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_CDMA_BroadcastSmsConfigInfo>(RIL_CDMA_BroadcastSmsConfigInfo &arg) const;

template <>
Marshal::Result Marshal::readAndAlloc<RIL_CDMA_BroadcastSmsConfigInfo>(
        RIL_CDMA_BroadcastSmsConfigInfo* &arg, size_t &sz) const;

template<>
Marshal::Result Marshal::write<RIL_CDMA_SMS_WriteArgs>(const RIL_CDMA_SMS_WriteArgs &arg);
template<>
Marshal::Result Marshal::read<RIL_CDMA_SMS_WriteArgs>(RIL_CDMA_SMS_WriteArgs& arg) const;

#endif
