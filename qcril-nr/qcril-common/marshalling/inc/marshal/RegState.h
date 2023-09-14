/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_REG_STATE
#define _QTI_MARSHALLING_REG_STATE

#include <Marshal.h>
#include <telephony/ril.h>
#include <marshal/CellIdentity.h>

template <>
Marshal::Result Marshal::write<RIL_CellIdentity_v16>(const RIL_CellIdentity_v16& arg);
template <>
Marshal::Result Marshal::read<RIL_CellIdentity_v16>(RIL_CellIdentity_v16& arg) const;

template <>
Marshal::Result Marshal::write<LteVopsInfo>(const LteVopsInfo& arg);
template <>
Marshal::Result Marshal::read<LteVopsInfo>(LteVopsInfo& arg) const;

template <>
Marshal::Result Marshal::write<NrIndicators>(const NrIndicators& arg);
template <>
Marshal::Result Marshal::read<NrIndicators>(NrIndicators& arg) const;

template <>
Marshal::Result Marshal::write<RIL_VoiceRegistrationStateResponse>(
    const RIL_VoiceRegistrationStateResponse& arg);
template <>
Marshal::Result Marshal::read<RIL_VoiceRegistrationStateResponse>(
    RIL_VoiceRegistrationStateResponse& arg) const;

template <>
Marshal::Result Marshal::write<RIL_DataRegistrationStateResponse>(
    const RIL_DataRegistrationStateResponse& arg);
template <>
Marshal::Result Marshal::read<RIL_DataRegistrationStateResponse>(
    RIL_DataRegistrationStateResponse& arg) const;

#endif
