/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <type_traits>
#include <telephony/ril.h>
#include <marshal/SignalStrength.h>

template <>
Marshal::Result Marshal::write<RIL_GSM_SignalStrength_v12>(const RIL_GSM_SignalStrength_v12 &arg) {
    write(arg.signalStrength);
    write(arg.bitErrorRate);
    write(arg.timingAdvance);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_GSM_SignalStrength_v12>(RIL_GSM_SignalStrength_v12 &arg) const {
    read(arg.signalStrength);
    read(arg.bitErrorRate);
    read(arg.timingAdvance);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_LTE_SignalStrength_v8>(const RIL_LTE_SignalStrength_v8 &arg) {
    write(arg.signalStrength);
    write(arg.rsrp);
    write(arg.rsrq);
    write(arg.rssnr);
    write(arg.cqi);
    write(arg.timingAdvance);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_LTE_SignalStrength_v8>(RIL_LTE_SignalStrength_v8 &arg) const {
    read(arg.signalStrength);
    read(arg.rsrp);
    read(arg.rsrq);
    read(arg.rssnr);
    read(arg.cqi);
    read(arg.timingAdvance);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SignalStrengthWcdma>(const RIL_SignalStrengthWcdma &arg) {
    write(arg.signalStrength);
    write(arg.bitErrorRate);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SignalStrengthWcdma>(RIL_SignalStrengthWcdma &arg) const {
    read(arg.signalStrength);
    read(arg.bitErrorRate);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_SignalStrength>(const RIL_CDMA_SignalStrength &arg) {
    write(arg.dbm);
    write(arg.ecio);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_SignalStrength>(RIL_CDMA_SignalStrength &arg) const {
    read(arg.dbm);
    read(arg.ecio);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_EVDO_SignalStrength>(const RIL_EVDO_SignalStrength &arg) {
    write(arg.dbm);
    write(arg.ecio);
    write(arg.signalNoiseRatio);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_EVDO_SignalStrength>(RIL_EVDO_SignalStrength &arg) const {
    read(arg.dbm);
    read(arg.ecio);
    read(arg.signalNoiseRatio);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_TD_SCDMA_SignalStrength>(const RIL_TD_SCDMA_SignalStrength &arg) {
    write(arg.rscp);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_TD_SCDMA_SignalStrength>(RIL_TD_SCDMA_SignalStrength &arg) const {
    read(arg.rscp);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_GW_SignalStrength>(const RIL_GW_SignalStrength &arg) {
    write(arg.signalStrength);
    write(arg.bitErrorRate);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_GW_SignalStrength>(RIL_GW_SignalStrength &arg) const {
    read(arg.signalStrength);
    read(arg.bitErrorRate);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_WCDMA_SignalStrength>(const RIL_WCDMA_SignalStrength &arg) {
    write(arg.valid);
    write(arg.signalStrength);
    write(arg.bitErrorRate);
    write(arg.rscp);
    write(arg.ecio);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_WCDMA_SignalStrength>(RIL_WCDMA_SignalStrength &arg) const {
    read(arg.valid);
    read(arg.signalStrength);
    read(arg.bitErrorRate);
    read(arg.rscp);
    read(arg.ecio);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_NR_SignalStrength>(const RIL_NR_SignalStrength &arg) {
    write(arg.ssRsrp);
    write(arg.ssRsrq);
    write(arg.ssSinr);
    write(arg.csiRsrp);
    write(arg.csiRsrq);
    write(arg.csiSinr);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_NR_SignalStrength>(RIL_NR_SignalStrength &arg) const {
    read(arg.ssRsrp);
    read(arg.ssRsrq);
    read(arg.ssSinr);
    read(arg.csiRsrp);
    read(arg.csiRsrq);
    read(arg.csiSinr);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SignalStrength>(const RIL_SignalStrength &arg) {
    write(arg.GW_SignalStrength);
    write(arg.WCDMA_SignalStrength);
    write(arg.CDMA_SignalStrength);
    write(arg.EVDO_SignalStrength);
    write(arg.LTE_SignalStrength);
    write(arg.TD_SCDMA_SignalStrength);
    write(arg.NR_SignalStrength);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SignalStrength>(RIL_SignalStrength &arg) const {
    read(arg.GW_SignalStrength);
    read(arg.WCDMA_SignalStrength);
    read(arg.CDMA_SignalStrength);
    read(arg.EVDO_SignalStrength);
    read(arg.LTE_SignalStrength);
    read(arg.TD_SCDMA_SignalStrength);
    read(arg.NR_SignalStrength);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SignalStrengthReportingCriteria>(
        const RIL_SignalStrengthReportingCriteria& arg) {
    WRITE_AND_CHECK(arg.accessNetwork);
    WRITE_AND_CHECK(arg.signalStrengthParameter);
    WRITE_AND_CHECK(arg.isEnabled);
    WRITE_AND_CHECK(arg.hysteresisMs);
    WRITE_AND_CHECK(arg.hysteresisDb);
    WRITE_AND_CHECK(arg.thresholds, arg.thresholdsLength);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SignalStrengthReportingCriteria>(
        RIL_SignalStrengthReportingCriteria& arg) const {
    READ_AND_CHECK(arg.accessNetwork);
    READ_AND_CHECK(arg.signalStrengthParameter);
    READ_AND_CHECK(arg.isEnabled);
    READ_AND_CHECK(arg.hysteresisMs);
    READ_AND_CHECK(arg.hysteresisDb);

    std::size_t len = 0;
    RUN_AND_CHECK(readAndAlloc(arg.thresholds, len));
    arg.thresholdsLength = len;

    return Marshal::Result::SUCCESS;
}