/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <type_traits>
#include <telephony/ril.h>
#include <marshal/CellIdentity.h>
#include <Marshal.h>

template <>
Marshal::Result Marshal::write<RIL_CellIdentityGsm_v12>(const RIL_CellIdentityGsm_v12 &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.lac);
    write(arg.cid);
    write(arg.arfcn);
    write(arg.bsic);
    write(arg.operatorNames);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityGsm_v12>(RIL_CellIdentityGsm_v12 &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.lac);
    read(arg.cid);
    read(arg.arfcn);
    read(arg.bsic);
    read(arg.operatorNames);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityCdma>(const RIL_CellIdentityCdma &arg) {
    write(arg.networkId);
    write(arg.systemId);
    write(arg.basestationId);
    write(arg.longitude);
    write(arg.latitude);
    write(arg.operatorNames);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityCdma>(RIL_CellIdentityCdma &arg) const {
    read(arg.networkId);
    read(arg.systemId);
    read(arg.basestationId);
    read(arg.longitude);
    read(arg.latitude);
    read(arg.operatorNames);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityLte_v12>(const RIL_CellIdentityLte_v12 &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.ci);
    write(arg.pci);
    write(arg.tac);
    write(arg.earfcn);
    write(arg.operatorNames);
    write(arg.bandwidth);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityLte_v12>(RIL_CellIdentityLte_v12 &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.ci);
    read(arg.pci);
    read(arg.tac);
    read(arg.earfcn);
    read(arg.operatorNames);
    read(arg.bandwidth);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityWcdma_v12>(const RIL_CellIdentityWcdma_v12 &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.lac);
    write(arg.cid);
    write(arg.psc);
    write(arg.uarfcn);
    write(arg.operatorNames);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityWcdma_v12>(RIL_CellIdentityWcdma_v12 &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.lac);
    read(arg.cid);
    read(arg.psc);
    read(arg.uarfcn);
    read(arg.operatorNames);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityTdscdma>(const RIL_CellIdentityTdscdma &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.lac);
    write(arg.cid);
    write(arg.cpid);
    write(arg.operatorNames);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityTdscdma>(RIL_CellIdentityTdscdma &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.lac);
    read(arg.cid);
    read(arg.cpid);
    read(arg.operatorNames);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityNr>(const RIL_CellIdentityNr &arg) {
    write(arg.mcc);
    write(arg.mnc);
    write(arg.nci);
    write(arg.pci);
    write(arg.tac);
    write(arg.nrarfcn);
    write(arg.operatorNames);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityNr>(RIL_CellIdentityNr &arg) const {
    read(arg.mcc);
    read(arg.mnc);
    read(arg.nci);
    read(arg.pci);
    read(arg.tac);
    read(arg.nrarfcn);
    read(arg.operatorNames);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CellIdentityOperatorNames>(const RIL_CellIdentityOperatorNames &arg) {
    write(arg.alphaShort);
    write(arg.alphaLong);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_CellIdentityOperatorNames>(RIL_CellIdentityOperatorNames &arg) const {
    read(arg.alphaShort);
    read(arg.alphaLong);
    return Result::SUCCESS;
}
