/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_CELL_INFO
#define _QTI_MARSHALLING_CELL_INFO

#include <telephony/ril.h>
#include <Marshal.h>

template <>
Marshal::Result Marshal::write<RIL_CellInfo_v12>(const RIL_CellInfo_v12 &arg);
template <>
Marshal::Result Marshal::read<RIL_CellInfo_v12>(RIL_CellInfo_v12 &arg) const;
template <>
Marshal::Result Marshal::release<RIL_CellInfo_v12>(RIL_CellInfo_v12 &arg);

template <>
Marshal::Result Marshal::write<RIL_CellInfoType>(const RIL_CellInfoType &arg);
template <>
Marshal::Result Marshal::read<RIL_CellInfoType>(RIL_CellInfoType &arg) const;

template <>
Marshal::Result Marshal::write<RIL_TimeStampType>(const RIL_TimeStampType &arg);
template <>
Marshal::Result Marshal::read<RIL_TimeStampType>(RIL_TimeStampType &arg) const;

using CellConnectionStatus = enum _RIL_CellConnectionStatus;
template <>
Marshal::Result Marshal::write<CellConnectionStatus>(const CellConnectionStatus &arg);
template <>
Marshal::Result Marshal::read<CellConnectionStatus>(CellConnectionStatus &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellInfoGsm_v12>(const RIL_CellInfoGsm_v12 &arg);
template <>
Marshal::Result Marshal::read<RIL_CellInfoGsm_v12>(RIL_CellInfoGsm_v12 &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellInfoCdma>(const RIL_CellInfoCdma &arg);
template <>
Marshal::Result Marshal::read<RIL_CellInfoCdma>(RIL_CellInfoCdma &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellInfoLte_v12>(const RIL_CellInfoLte_v12 &arg);
template <>
Marshal::Result Marshal::read<RIL_CellInfoLte_v12>(RIL_CellInfoLte_v12 &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellInfoWcdma_v12>(const RIL_CellInfoWcdma_v12 &arg);
template <>
Marshal::Result Marshal::read<RIL_CellInfoWcdma_v12>(RIL_CellInfoWcdma_v12 &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellInfoTdscdma>(const RIL_CellInfoTdscdma &arg);
template <>
Marshal::Result Marshal::read<RIL_CellInfoTdscdma>(RIL_CellInfoTdscdma &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellInfoNr>(const RIL_CellInfoNr &arg);
template <>
Marshal::Result Marshal::read<RIL_CellInfoNr>(RIL_CellInfoNr &arg) const;

#endif // _QTI_MARSHALLING_CELL_INFO
