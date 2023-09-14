/* ============================================================
 *
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */
#include "PeripheralMgr.h"
#include "PeripheralMgrImpl.h"
#include <framework/Log.h>

#define TAG "PeripheralMgr"

extern PeripheralMgrImpl* getPlatformPmModule();

PeripheralMgr::PeripheralMgr() {
    QCRIL_LOG_INFO("PeripheralMgr constructor");
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    mPmImpl = getPlatformPmModule();
};


PeripheralMgr::~PeripheralMgr() {
    //TODO free impl
}

void PeripheralMgr::registerClientWithModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    if (mPmImpl != nullptr) {
        mPmImpl->registerClientWithModem();
    }
}

void PeripheralMgr::voteUpPrimaryModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    if (mPmImpl != nullptr) {
        mPmImpl->voteUpPrimaryModem();
    }
}

void PeripheralMgr::voteUpSecondaryModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    if (mPmImpl != nullptr) {
        mPmImpl->voteUpSecondaryModem();
    }
}

void PeripheralMgr::voteDownPrimaryModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    if (mPmImpl != nullptr) {
        mPmImpl->voteDownPrimaryModem();
    }
}

void PeripheralMgr::voteDownSecondaryModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    if (mPmImpl != nullptr) {
        mPmImpl->voteDownSecondaryModem();
    }
}

bool PeripheralMgr::isSecondaryModemPresent() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    bool status = false;
    if (mPmImpl != nullptr) {
        status = mPmImpl->isSecondaryModemPresent();
    }
    return status;
}

int PeripheralMgr::getPrimaryModemVotingState() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    int state = 0;
    if (mPmImpl != nullptr) {
        state = mPmImpl->getPrimaryModemVotingState();
    }
    return state;
}

void PeripheralMgr::setPrimaryModemVotingState(int state) {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    if (mPmImpl != nullptr) {
        mPmImpl->setPrimaryModemVotingState(state);
    }
}

const char* PeripheralMgr::getEsocLinkName() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    const char* name = NULL;
    if (mPmImpl != nullptr) {
        name = mPmImpl->getEsocLinkName();
    }
    return name;
}

void PeripheralMgr::deregisterClientWithModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    if (mPmImpl != nullptr) {
        mPmImpl->deregisterClientWithModem();
    }
}

bool PeripheralMgr::pmVotingSupportPrimaryModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    bool status = false;
    if (mPmImpl != nullptr) {
        status = mPmImpl->pmVotingSupportPrimaryModem();
    }
    return status;
}

bool PeripheralMgr::esocVotingSupportPrimaryModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    bool status = false;
    if (mPmImpl != nullptr) {
        status = mPmImpl->esocVotingSupportPrimaryModem();
    }
    return status;
}

int PeripheralMgr::getSecondaryModemVotingState() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPmMutex);
    int state = 0;
    if (mPmImpl != nullptr) {
        state = mPmImpl->getSecondaryModemVotingState();
    }
    return state;
}

PeripheralMgr &PeripheralMgr::getInstance() {
    static PeripheralMgr sInstance;
    return sInstance;
}