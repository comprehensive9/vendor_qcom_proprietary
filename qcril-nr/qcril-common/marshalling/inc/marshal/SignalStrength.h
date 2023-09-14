/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_SIGNAL_STRENGTH
#define _QTI_MARSHALLING_SIGNAL_STRENGTH

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_GSM_SignalStrength_v12>(const RIL_GSM_SignalStrength_v12 &arg);
template <>
Marshal::Result Marshal::read<RIL_GSM_SignalStrength_v12>(RIL_GSM_SignalStrength_v12 &arg) const;

template <>
Marshal::Result Marshal::write<RIL_LTE_SignalStrength_v8>(const RIL_LTE_SignalStrength_v8 &arg);
template <>
Marshal::Result Marshal::read<RIL_LTE_SignalStrength_v8>(RIL_LTE_SignalStrength_v8 &arg) const;

template <>
Marshal::Result Marshal::write<RIL_SignalStrengthWcdma>(const RIL_SignalStrengthWcdma &arg);
template <>
Marshal::Result Marshal::read<RIL_SignalStrengthWcdma>(RIL_SignalStrengthWcdma &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CDMA_SignalStrength>(const RIL_CDMA_SignalStrength &arg);
template <>
Marshal::Result Marshal::read<RIL_CDMA_SignalStrength>(RIL_CDMA_SignalStrength &arg) const;

template <>
Marshal::Result Marshal::write<RIL_EVDO_SignalStrength>(const RIL_EVDO_SignalStrength &arg);
template <>
Marshal::Result Marshal::read<RIL_EVDO_SignalStrength>(RIL_EVDO_SignalStrength &arg) const;

template <>
Marshal::Result Marshal::write<RIL_TD_SCDMA_SignalStrength>(const RIL_TD_SCDMA_SignalStrength &arg);
template <>
Marshal::Result Marshal::read<RIL_TD_SCDMA_SignalStrength>(RIL_TD_SCDMA_SignalStrength &arg) const;

template <>
Marshal::Result Marshal::write<RIL_GW_SignalStrength>(const RIL_GW_SignalStrength &arg);
template <>
Marshal::Result Marshal::read<RIL_GW_SignalStrength>(RIL_GW_SignalStrength &arg) const;

template <>
Marshal::Result Marshal::write<RIL_WCDMA_SignalStrength>(const RIL_WCDMA_SignalStrength &arg);
template <>
Marshal::Result Marshal::read<RIL_WCDMA_SignalStrength>(RIL_WCDMA_SignalStrength &arg) const;

template <>
Marshal::Result Marshal::write<RIL_NR_SignalStrength>(const RIL_NR_SignalStrength &arg);
template <>
Marshal::Result Marshal::read<RIL_NR_SignalStrength>(RIL_NR_SignalStrength &arg) const;

template <>
Marshal::Result Marshal::write<RIL_SignalStrength>(const RIL_SignalStrength &arg);
template <>
Marshal::Result Marshal::read<RIL_SignalStrength>(RIL_SignalStrength &arg) const;

template <>
Marshal::Result Marshal::write<RIL_SignalStrengthReportingCriteria>(
    const RIL_SignalStrengthReportingCriteria& arg);

template <>
Marshal::Result Marshal::read<RIL_SignalStrengthReportingCriteria>(
    RIL_SignalStrengthReportingCriteria& arg) const;

#endif // _QTI_MARSHALLING_SIGNAL_STRENGTH
