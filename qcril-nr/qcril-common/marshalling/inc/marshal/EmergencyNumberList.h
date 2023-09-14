/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_EMERGENCY_NUMBERT_LIST
#define _QTI_MARSHALLING_EMERGENCY_NUMBERT_LIST

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_EmergencyNumberInfo>(
    const RIL_EmergencyNumberInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_EmergencyNumberInfo>(
    RIL_EmergencyNumberInfo& arg) const;

template <>
Marshal::Result Marshal::write<RIL_EmergencyList>(
    const RIL_EmergencyList& arg);
template <>
Marshal::Result Marshal::read<RIL_EmergencyList>(
    RIL_EmergencyList& arg) const;

#endif
