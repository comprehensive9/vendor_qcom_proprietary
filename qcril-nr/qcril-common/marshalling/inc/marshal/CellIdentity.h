/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_CELL_IDENTITY
#define _QTI_MARSHALLING_CELL_IDENTITY

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CellIdentityGsm_v12>(const RIL_CellIdentityGsm_v12 &arg);
template <>
Marshal::Result Marshal::read<RIL_CellIdentityGsm_v12>(RIL_CellIdentityGsm_v12 &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellIdentityCdma>(const RIL_CellIdentityCdma &arg);
template <>
Marshal::Result Marshal::read<RIL_CellIdentityCdma>(RIL_CellIdentityCdma &arg) const;
template <>
Marshal::Result Marshal::write<RIL_CellIdentityLte_v12>(const RIL_CellIdentityLte_v12 &arg);
template <>
Marshal::Result Marshal::read<RIL_CellIdentityLte_v12>(RIL_CellIdentityLte_v12 &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellIdentityWcdma_v12>(const RIL_CellIdentityWcdma_v12 &arg);
template <>
Marshal::Result Marshal::read<RIL_CellIdentityWcdma_v12>(RIL_CellIdentityWcdma_v12 &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellIdentityNr>(const RIL_CellIdentityNr &arg);
template <>
Marshal::Result Marshal::read<RIL_CellIdentityNr>(RIL_CellIdentityNr &arg) const;

template <>
Marshal::Result Marshal::write<RIL_CellIdentityOperatorNames>(const RIL_CellIdentityOperatorNames &arg);
template <>
Marshal::Result Marshal::read<RIL_CellIdentityOperatorNames>(RIL_CellIdentityOperatorNames &arg) const;
#endif // _QTI_MARSHALLING_CELL_IDENTITY
