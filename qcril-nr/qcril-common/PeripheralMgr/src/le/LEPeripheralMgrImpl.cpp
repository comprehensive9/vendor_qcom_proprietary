/* ============================================================
 *
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#include "LEPeripheralMgrImpl.h"
#include <framework/Log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TAG "LEPeripheralMgrImpl"

LEPeripheralMgrImpl::LEPeripheralMgrImpl() {
    mModemFD = -1;
    mPrimaryModemState = 0;
    mPrimaryModemVotingSupport = false;
}

LEPeripheralMgrImpl::~LEPeripheralMgrImpl() {

}

void LEPeripheralMgrImpl::registerClientWithModem() {
    //Not required to load info . it will be done during vote-up/down
    mPrimaryModemVotingSupport = true;
}

void LEPeripheralMgrImpl::voteUpPrimaryModem() {
    QCRIL_LOG_DEBUG("Handling voteUpPrimaryModem request");

    mModemFD = open(MODEM_NODE, O_RDONLY);
    if (mModemFD < 0) {
        QCRIL_LOG_ERROR("failed to connect to modem node");
        mModemFD = -1;
        mPrimaryModemState = 0;
    } else {
        mPrimaryModemState = 1;
    }
}

void LEPeripheralMgrImpl::voteUpSecondaryModem() {
    QCRIL_LOG_DEBUG("voteUpSecondaryModem request is not supported");
}

void LEPeripheralMgrImpl::voteDownPrimaryModem() {
    QCRIL_LOG_DEBUG("Handling voteDownPrimaryModem request");
    close(mModemFD);
    mPrimaryModemState = 0;
}

void LEPeripheralMgrImpl::voteDownSecondaryModem() {
    QCRIL_LOG_DEBUG("voteDownSecondaryModem request is not supported");
}

bool LEPeripheralMgrImpl::isSecondaryModemPresent() {
    return false;
}

int LEPeripheralMgrImpl::getPrimaryModemVotingState() {
    return mPrimaryModemState;
}

void LEPeripheralMgrImpl::setPrimaryModemVotingState(int state) {
    mPrimaryModemState = state;
}

const char* LEPeripheralMgrImpl::getEsocLinkName() {
    return NULL;
}

void LEPeripheralMgrImpl::deregisterClientWithModem() {
    //Not required to de-register info . it will be done during vote-up/down
    mPrimaryModemVotingSupport = false;
}

bool LEPeripheralMgrImpl::pmVotingSupportPrimaryModem() {
    return mPrimaryModemVotingSupport;
}

bool LEPeripheralMgrImpl::esocVotingSupportPrimaryModem() {
    return false;
}

int LEPeripheralMgrImpl::getSecondaryModemVotingState() {
    return 0;
}


/*platform dependent module initialization*/
PeripheralMgrImpl* getPlatformPmModule() {
    static LEPeripheralMgrImpl mModule{};
    return &mModule;
}
