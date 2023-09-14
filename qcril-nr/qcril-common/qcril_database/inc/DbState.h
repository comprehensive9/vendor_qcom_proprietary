/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <memory>

class DbState {
public:
#ifndef ANDROID
    static constexpr const char* SHM_NAME = "/dbstate_shm";
#else
    static constexpr const char* DB_PROPERTY = "vendor.radio.db_state";
#endif
    static bool createDbState(bool primary);
    static std::shared_ptr<DbState> getInstance();
    enum class State {UNKNOWN, INIT, UPGRADING, READY};
    void setDbState(State state);
    State getDbState();
    bool waitForDbState(State state, int timeout);
    DbState(bool primary);

private:
    static std::shared_ptr<DbState> sDbState;
    //DbState(bool primary);
#ifndef ANDROID
    int mShmFd;
    uint8_t* mAddr;
#endif
    bool mIsPrimary;
};

