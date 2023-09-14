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

class LEPeripheralMgrImpl : public PeripheralMgrImpl {
public:
    LEPeripheralMgrImpl();
    ~LEPeripheralMgrImpl();
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
    static constexpr const char* MODEM_NODE="/dev/subsys_modem";
    int mModemFD;
    int mPrimaryModemState;
    int mPrimaryModemVotingSupport;
};
