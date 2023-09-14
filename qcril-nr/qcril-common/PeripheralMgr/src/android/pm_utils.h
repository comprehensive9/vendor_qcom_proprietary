/* ============================================================
 *
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#pragma once
#include <string>
#include <unordered_map>
#include "QtiMutex.h"
extern "C" {
#include "mdm_detect.h"
}


typedef struct esoc_mdm_info {
    bool                             pm_feature_supported;
    bool                             esoc_feature_supported;
    int                                 esoc_fd;
    int                                 voting_state; // 0 - vote released; 1 - vote activated
    char                                link_name[MAX_NAME_LEN];
    char                                modem_name[MAX_NAME_LEN];
    char                                esoc_node[MAX_NAME_LEN];
    qtimutex::QtiRecursiveMutex         mdm_mutex;
    MdmType                             type;
} qcril_mdm_info;
