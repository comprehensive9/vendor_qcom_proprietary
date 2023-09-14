/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <loc_pla.h>
#include <log_util.h>
#include <GardenUtil.h>

using std::mutex;
using std::unique_lock;
using std::cv_status;
using namespace std::chrono;

namespace garden {

bool Waitable::wait(int timeoutInSec) {
    unique_lock<mutex> lck(mLock);
    auto now = chrono::system_clock::now();

    bool res = true;

    // A notify did not ocurred before this wait call, so wait.
    if (mCounter <= 0) {
        res = (mCv.wait_until(lck, now + chrono::seconds(timeoutInSec)) == cv_status::no_timeout);
    }

    mCounter--;
    return res;
}

void Waitable::notify() {
    unique_lock<mutex> lck(mLock);
    mCounter++;
    mCv.notify_all();
}


uint64_t getRealTimeNsec()
{
    struct timespec ts;

    ts.tv_sec = ts.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

uint64_t getUpTimeSec()
{
    struct timespec ts;

    ts.tv_sec = ts.tv_nsec = 0;
    clock_gettime(CLOCK_BOOTTIME, &ts);
    return ts.tv_sec + (ts.tv_nsec / 1000000000LL);
}

} // namespace garden
