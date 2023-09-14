/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <assert.h>
#include <string>
#include <stdlib.h>
#include <errno.h>
#ifdef ANDROID
#include <cutils/properties.h>
#endif

#include "DbState.h"

std::shared_ptr<DbState> DbState::sDbState;

std::shared_ptr<DbState> DbState::getInstance() {
    assert(sDbState != nullptr);
    return sDbState;
}

bool DbState::createDbState(bool primary) {
    if (sDbState != nullptr) return true;
    sDbState = std::make_shared<DbState>(primary);
    return (sDbState != nullptr);
}

void DbState::setDbState(DbState::State state) {
    if (!mIsPrimary)
        return;
#ifndef ANDROID
    *mAddr = static_cast<uint8_t>(state);
#else
    char value[PROPERTY_VALUE_MAX];
    snprintf(value, sizeof(value), "%d", state);
    if (property_set(DB_PROPERTY, value)) {
        // log
    }
#endif
}

DbState::State DbState::getDbState() {
#ifndef ANDROID
    if (!mIsPrimary) {
        if (mShmFd < 0) {
            mShmFd = shm_open(SHM_NAME, O_RDWR, 0600);
            if (mShmFd >= 0)
                ftruncate(mShmFd, 1);
        }
        if (mShmFd >= 0 && (!mAddr || mAddr != MAP_FAILED))
            mAddr = static_cast<uint8_t*>(
                mmap(0, 1, PROT_READ | PROT_WRITE, MAP_SHARED, mShmFd, 0));
    }
    if (mAddr && mAddr != MAP_FAILED)
        return static_cast<DbState::State>(*mAddr);
    else
        return DbState::State::UNKNOWN;
#else
    auto state = DbState::State::UNKNOWN;
    char value[PROPERTY_VALUE_MAX] = {0};
    if (property_get(DB_PROPERTY, value, "0")) {
        char *ptr;
        auto res = strtoul(value, &ptr, 0);
        if ((errno != ERANGE) && *ptr == '\0' &&
                (res <= static_cast<unsigned long>(DbState::State::READY)))
            state = static_cast<DbState::State>(res);
    }
    return state;
#endif
}

bool DbState::waitForDbState(DbState::State state, int timeout) {
    uint32_t counts = 0;
    while (true) {
        auto res = getDbState();
        if (res == state) return true;
        sleep(1);
        if (timeout > 0 && counts++ >= timeout) return false;
    }
    return true;
}

DbState::DbState(bool primary): mIsPrimary(primary) {
#ifndef ANDROID
    if (mIsPrimary) {
        mAddr = nullptr;
        mShmFd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (mShmFd < 0) {
            mShmFd = shm_open(SHM_NAME, O_RDWR, 0600);
        }
        if (mShmFd >= 0) {
            // The very first call (per power cycle)  will allocate the 1 byte memory
            // and initiliaze to 0. The later calls (after crash or other cases)
            // will have no impact of value
            ftruncate(mShmFd, 1);
            mAddr = static_cast<uint8_t*>(
                mmap(0, 1, PROT_READ | PROT_WRITE, MAP_SHARED, mShmFd, 0));
        }
    } else {
        mShmFd = -1;
        mAddr = nullptr;
    }
#else
    //mShmFd = ashmem_create_region("");
#endif
}

#if 0
int main(int argc, char* argv[]) {
    (void)DbState::createDbState(true);
    printf("initial db state is %d\n", DbState::getInstance()->getDbState());
    DbState::getInstance()->setDbState(DbState::State::READY);
    printf("modified db state is %d\n", DbState::getInstance()->getDbState());
}
#endif
