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
#include <marshal/RegState.h>

template <>
Marshal::Result Marshal::write<RIL_CellIdentity_v16>(const RIL_CellIdentity_v16& arg) {
  auto data = static_cast<uint32_t>(arg.cellInfoType);
  if (data > RIL_CELL_INFO_TYPE_TD_SCDMA) data = static_cast<uint32_t>(RIL_CELL_INFO_TYPE_NONE);
  write(data);
  switch (arg.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
      write<RIL_CellIdentityGsm_v12>(arg.cellIdentityGsm);
      break;
    case RIL_CELL_INFO_TYPE_CDMA:
      write<RIL_CellIdentityCdma>(arg.cellIdentityCdma);
      break;
    case RIL_CELL_INFO_TYPE_LTE:
      write<RIL_CellIdentityLte_v12>(arg.cellIdentityLte);
      break;
    case RIL_CELL_INFO_TYPE_WCDMA:
      write<RIL_CellIdentityWcdma_v12>(arg.cellIdentityWcdma);
      break;
#if 0
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        write<RIL_CellInfoTdscdma>(arg.cellIdentityTdscdma);
        break;
#endif
    default:
      break;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CellIdentity_v16>(RIL_CellIdentity_v16& arg) const {
  uint32_t data;
  read(data);
  if (data > RIL_CELL_INFO_TYPE_TD_SCDMA) data = static_cast<uint32_t>(RIL_CELL_INFO_TYPE_NONE);
  arg.cellInfoType = static_cast<RIL_CellInfoType>(data);
  switch (arg.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
      read<RIL_CellIdentityGsm_v12>(arg.cellIdentityGsm);
      break;
    case RIL_CELL_INFO_TYPE_CDMA:
      read<RIL_CellIdentityCdma>(arg.cellIdentityCdma);
      break;
    case RIL_CELL_INFO_TYPE_LTE:
      read<RIL_CellIdentityLte_v12>(arg.cellIdentityLte);
      break;
    case RIL_CELL_INFO_TYPE_WCDMA:
      read<RIL_CellIdentityWcdma_v12>(arg.cellIdentityWcdma);
      break;
#if 0
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        read<RIL_CellInfoTdscdma>(arg.cellIdentityTdscdma);
        break;
#endif
    default:
      break;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<LteVopsInfo>(const LteVopsInfo& arg) {
  write(arg.isVopsSupported);
  write(arg.isEmcBearerSupported);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<LteVopsInfo>(LteVopsInfo& arg) const {
  read(arg.isVopsSupported);
  read(arg.isEmcBearerSupported);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<NrIndicators>(const NrIndicators& arg) {
  write(arg.isEndcAvailable);
  write(arg.isDcNrRestricted);
  write(arg.plmnInfoListR15Available);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<NrIndicators>(NrIndicators& arg) const {
  read(arg.isEndcAvailable);
  read(arg.isDcNrRestricted);
  read(arg.plmnInfoListR15Available);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_VoiceRegistrationStateResponse>(
    const RIL_VoiceRegistrationStateResponse& arg) {
  write(static_cast<uint16_t>(arg.regState));
  write(static_cast<uint16_t>(arg.rat));
  write(arg.cssSupported);
  write(arg.roamingIndicator);
  write(arg.systemIsInPrl);
  write(arg.defaultRoamingIndicator);
  write(arg.reasonForDenial);
  write<RIL_CellIdentity_v16>(arg.cellIdentity);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_VoiceRegistrationStateResponse>(
    RIL_VoiceRegistrationStateResponse& arg) const {
  uint16_t data;
  read(data);
  arg.regState = static_cast<RIL_RegState>(data);
  read(data);
  arg.rat = static_cast<RIL_RadioTechnology>(data);
  read(arg.cssSupported);
  read(arg.roamingIndicator);
  read(arg.systemIsInPrl);
  read(arg.defaultRoamingIndicator);
  read(arg.reasonForDenial);
  read<RIL_CellIdentity_v16>(arg.cellIdentity);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_DataRegistrationStateResponse>(
    const RIL_DataRegistrationStateResponse& arg) {
  write(static_cast<uint16_t>(arg.regState));
  write(static_cast<uint16_t>(arg.rat));
  write(arg.reasonDataDenied);
  write(arg.maxDataCalls);
  write<RIL_CellIdentity_v16>(arg.cellIdentity);
  write(arg.lteVopsInfoValid);
  if (arg.lteVopsInfoValid) write<LteVopsInfo>(arg.lteVopsInfo);
  write(arg.nrIndicatorsValid);
  if (arg.nrIndicatorsValid) write<NrIndicators>(arg.nrIndicators);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_DataRegistrationStateResponse>(
    RIL_DataRegistrationStateResponse& arg) const {
  uint16_t data;
  read(data);
  arg.regState = static_cast<RIL_RegState>(data);
  read(data);
  arg.rat = static_cast<RIL_RadioTechnology>(data);
  read(arg.reasonDataDenied);
  read(arg.maxDataCalls);
  read<RIL_CellIdentity_v16>(arg.cellIdentity);
  read(arg.lteVopsInfoValid);
  if (arg.lteVopsInfoValid) read<LteVopsInfo>(arg.lteVopsInfo);
  read(arg.nrIndicatorsValid);
  if (arg.nrIndicatorsValid) read<NrIndicators>(arg.nrIndicators);
  return Result::SUCCESS;
}

#endif
