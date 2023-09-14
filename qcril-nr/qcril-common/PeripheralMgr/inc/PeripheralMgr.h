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

class PeripheralMgrImpl;
class PeripheralMgr {
public:
    ~PeripheralMgr();
    static PeripheralMgr &getInstance();
    void registerClientWithModem();
    void voteUpPrimaryModem();
    void voteUpSecondaryModem();
    void voteDownPrimaryModem();
    void voteDownSecondaryModem();
    bool isSecondaryModemPresent();
    int getPrimaryModemVotingState();
    void setPrimaryModemVotingState(int);
    int getSecondaryModemVotingState();
    const char *getEsocLinkName();
    void deregisterClientWithModem();
    bool pmVotingSupportPrimaryModem();
    bool esocVotingSupportPrimaryModem();

private:
    PeripheralMgr();
    PeripheralMgrImpl *mPmImpl;
    qtimutex::QtiRecursiveMutex mPmMutex;
};


