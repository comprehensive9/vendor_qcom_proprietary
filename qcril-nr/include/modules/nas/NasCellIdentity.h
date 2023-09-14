/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "telephony/ril.h"
void initialize_ril_gsm_cell_identity(RIL_CellIdentityGsm_v12& cell);
void initialize_ril_lte_cell_identity(RIL_CellIdentityLte_v12& cell);
void initialize_ril_wcdma_cell_identity(RIL_CellIdentityWcdma_v12& cell);
void initialize_ril_tdscdma_cell_identity(RIL_CellIdentityTdscdma& cell);
void initialize_ril_nr_cell_identity(RIL_CellIdentityNr& cell);
