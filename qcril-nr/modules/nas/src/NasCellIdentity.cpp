/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/nas/NasCellIdentity.h"
#include <string.h>
#include <limits>

void initialize_ril_identity_operator_names(
        RIL_CellIdentityOperatorNames& names) {
    memset(names.alphaShort, 0, MAX_OPERATOR_NAME_LEN);
    memset(names.alphaLong, 0, MAX_OPERATOR_NAME_LEN);
}

void initialize_ril_gsm_cell_identity(RIL_CellIdentityGsm_v12& cell) {
    memset(cell.mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    // the comment in structure says, valid value is 0..65535. and INT_MAX
    // stands for unknown, which is 32767 actually. Use -1 here to keep the
    // same behavior with current code
    cell.lac = -1;
    cell.cid = -1;
    cell.arfcn = -1;
    // unknown
    cell.bsic = 0xff;
    initialize_ril_identity_operator_names(cell.operatorNames);
}

void initialize_ril_lte_cell_identity(RIL_CellIdentityLte_v12& cell) {
    memset(cell.mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);

    // set as unknown
    cell.ci = -1;
    cell.pci = -1;
    cell.tac = -1;
    cell.earfcn = -1;
    cell.bandwidth = -1;
    cell.band = -1;
    initialize_ril_identity_operator_names(cell.operatorNames);
}

void initialize_ril_wcdma_cell_identity(RIL_CellIdentityWcdma_v12& cell) {
    memset(cell.mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    // the comment in structure says, valid value is 0..65535. and INT_MAX
    // stands for unknown, which is 32767 actually. Use -1 here to keep the
    // same behavior with current code
    cell.lac = -1;
    cell.cid = -1;
    cell.psc = -1;
    cell.uarfcn = -1;
    initialize_ril_identity_operator_names(cell.operatorNames);
}

void initialize_ril_tdscdma_cell_identity(RIL_CellIdentityTdscdma& cell) {
    memset(cell.mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);

    cell.lac = -1;
    cell.cid = -1;
    cell.cpid = -1;
    cell.uarfcn = -1;
    initialize_ril_identity_operator_names(cell.operatorNames);
}

void initialize_ril_nr_cell_identity(RIL_CellIdentityNr& cell) {
    memset(cell.mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.reg_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mcc, 0, MAX_RIL_MCC_MNC_SIZE);
    memset(cell.primary_plmn_mnc, 0, MAX_RIL_MCC_MNC_SIZE);

    cell.nci = std::numeric_limits<uint64_t>::max();
    cell.pci = std::numeric_limits<uint32_t>::max();
    cell.tac = -1;
    cell.nrarfcn = -1;
    cell.bandwidth = -1;
    cell.band = -1;

    initialize_ril_identity_operator_names(cell.operatorNames);
}
