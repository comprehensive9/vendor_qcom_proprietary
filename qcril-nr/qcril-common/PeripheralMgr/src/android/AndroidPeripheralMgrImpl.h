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
#include "PeripheralMgrImpl.h"
#include "PeripheralMgr.h"
#include "pm_utils.h"

class AndroidPeripheralMgrImpl : public PeripheralMgrImpl {
public:
    AndroidPeripheralMgrImpl();
    ~AndroidPeripheralMgrImpl();
    void registerClientWithModem();
    void voteUpPrimaryModem();
    void voteUpSecondaryModem();
    void voteDownPrimaryModem();
    void voteDownSecondaryModem();
    bool isSecondaryModemPresent();
    int getPrimaryModemVotingState();
    void setPrimaryModemVotingState(int);
    const char *getEsocLinkName();
    void deregisterClientWithModem();
    bool pmVotingSupportPrimaryModem();
    bool esocVotingSupportPrimaryModem();
    int getSecondaryModemVotingState();

private:
    qcril_mdm_info mPrimaryModemInfo;
    qcril_mdm_info mSecModemInfo;
    void loadEsocInfo();
    void copyPrimaryModemInfo(struct mdm_info &src);
    void copySecondaryModemInfo(struct mdm_info &src);
    const char * getPrimaryModemName();
    const char * getSecondaryModemName();
    void voteModemUsingEsoc(qcril_mdm_info &);
};
