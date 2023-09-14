/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <type_traits>
#include <telephony/ril.h>
#include <marshal/CellInfo.h>
#include <marshal/CellIdentity.h>

template <>
Marshal::Result Marshal::write<RIL_CellInfo_v12>(const RIL_CellInfo_v12 &arg) {
    write(arg.cellInfoType);
    write(arg.registered);
    write(arg.timeStampType);
    write(arg.timeStamp);
    write(arg.connStatus);
    switch(arg.cellInfoType) {
        case RIL_CELL_INFO_TYPE_NONE:
            break;
        case RIL_CELL_INFO_TYPE_GSM:
            write(arg.CellInfo.gsm);
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            write(arg.CellInfo.cdma);
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            write(arg.CellInfo.lte);
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            write(arg.CellInfo.wcdma);
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            write(arg.CellInfo.tdscdma);
            break;
        case RIL_CELL_INFO_TYPE_NR:
            write(arg.CellInfo.nr);
            break;
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CellInfo_v12>(RIL_CellInfo_v12 &arg) const {
    read(arg.cellInfoType);
    read(arg.registered);
    read(arg.timeStampType);
    read(arg.timeStamp);
    read(arg.connStatus);
    switch(arg.cellInfoType) {
        case RIL_CELL_INFO_TYPE_NONE:
            break;
        case RIL_CELL_INFO_TYPE_GSM:
            read(arg.CellInfo.gsm);
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            read(arg.CellInfo.cdma);
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            read(arg.CellInfo.lte);
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            read(arg.CellInfo.wcdma);
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            read(arg.CellInfo.tdscdma);
            break;
        case RIL_CELL_INFO_TYPE_NR:
            read(arg.CellInfo.nr);
            break;
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CellInfo_v12>(RIL_CellInfo_v12 & /*arg*/) {
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellInfoType>(const RIL_CellInfoType &arg) {
    using inttype = std::underlying_type<RIL_CellInfoType>::type;
    return write(*reinterpret_cast<const inttype *>(&arg));
}
template <>
Marshal::Result Marshal::read<RIL_CellInfoType>(RIL_CellInfoType &arg) const {
    using inttype = std::underlying_type<RIL_CellInfoType>::type;
    return read(*reinterpret_cast<inttype *>(&arg));
}

template <>
Marshal::Result Marshal::write<RIL_TimeStampType>(const RIL_TimeStampType &arg) {
    using inttype = std::underlying_type<RIL_TimeStampType>::type;
    return write(*reinterpret_cast<const inttype *>(&arg));
}

template <>
Marshal::Result Marshal::read<RIL_TimeStampType>(RIL_TimeStampType &arg) const {
    using inttype = std::underlying_type<RIL_TimeStampType>::type;
    return read(*reinterpret_cast<inttype*>(&arg));
}

template <>
Marshal::Result Marshal::write<RIL_CellConnectionStatus>(const RIL_CellConnectionStatus &arg) {
    using inttype = std::underlying_type<RIL_CellConnectionStatus>::type;
    return write(*reinterpret_cast<const inttype *>(&arg));
}

template <>
Marshal::Result Marshal::read<RIL_CellConnectionStatus>(RIL_CellConnectionStatus &arg) const {
    using inttype = std::underlying_type<RIL_CellConnectionStatus>::type;
    return read(*reinterpret_cast<inttype *>(&arg));
}

template <>
Marshal::Result Marshal::write<RIL_CellInfoGsm_v12>(const RIL_CellInfoGsm_v12 &arg) {
    write(arg.cellIdentityGsm);
    write(arg.signalStrengthGsm);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellInfoGsm_v12>(RIL_CellInfoGsm_v12 &arg) const {
    read(arg.cellIdentityGsm);
    read(arg.signalStrengthGsm);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellInfoLte_v12>(const RIL_CellInfoLte_v12 &arg) {
    write(arg.cellIdentityLte);
    write(arg.signalStrengthLte);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellInfoLte_v12>(RIL_CellInfoLte_v12 &arg) const {
    read(arg.cellIdentityLte);
    read(arg.signalStrengthLte);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellInfoWcdma_v12>(const RIL_CellInfoWcdma_v12 &arg) {
    write(arg.cellIdentityWcdma);
    write(arg.signalStrengthWcdma);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellInfoWcdma_v12>(RIL_CellInfoWcdma_v12 &arg) const {
    read(arg.cellIdentityWcdma);
    read(arg.signalStrengthWcdma);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellInfoCdma>(const RIL_CellInfoCdma &arg) {
    write(arg.cellIdentityCdma);
    write(arg.signalStrengthCdma);
    write(arg.signalStrengthEvdo);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellInfoCdma>(RIL_CellInfoCdma &arg) const {
    read(arg.cellIdentityCdma);
    read(arg.signalStrengthCdma);
    read(arg.signalStrengthEvdo);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellInfoTdscdma>(const RIL_CellInfoTdscdma &arg) {
    write(arg.cellIdentityTdscdma);
    write(arg.signalStrengthTdscdma);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellInfoTdscdma>(RIL_CellInfoTdscdma &arg) const {
    read(arg.cellIdentityTdscdma);
    read(arg.signalStrengthTdscdma);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::write<RIL_CellInfoNr>(const RIL_CellInfoNr &arg) {
    write(arg.cellIdentityNr);
    write(arg.signalStrengthNr);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellInfoNr>(RIL_CellInfoNr &arg) const {
    read(arg.cellIdentityNr);
    read(arg.signalStrengthNr);
    return Result::SUCCESS;
}
