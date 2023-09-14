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
#include "QtiMutex.h"

class PeripheralMgrImpl {
public:
    PeripheralMgrImpl(){};
    virtual ~PeripheralMgrImpl(){};
    virtual void registerClientWithModem() = 0;
    virtual void voteUpPrimaryModem() = 0;
    virtual void voteUpSecondaryModem() = 0;
    virtual void voteDownPrimaryModem() = 0;
    virtual void voteDownSecondaryModem() = 0;
    virtual bool isSecondaryModemPresent() = 0;
    virtual int getPrimaryModemVotingState() = 0;
    virtual void setPrimaryModemVotingState(int) = 0;
    virtual const char *getEsocLinkName() = 0;
    virtual void deregisterClientWithModem() = 0;
    virtual bool pmVotingSupportPrimaryModem() = 0;
    virtual bool esocVotingSupportPrimaryModem() = 0;
    virtual int getSecondaryModemVotingState() = 0;
};