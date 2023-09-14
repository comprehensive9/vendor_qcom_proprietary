/******************************************************************************
  @file    Perf_Stub.cpp
  @brief   Android performance HAL stub module
           These functions called when perf opts are disabled during target bringup

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017,2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <hidl/HidlTransportSupport.h>
#include <hwbinder/IPCThreadState.h>
#include "Perf.h"

enum INDEX_REFERENCE {INDEX_CLIENT_TID=0, INDEX_CLIENT_PID =1};

namespace vendor {
namespace qti {
namespace hardware {
namespace perf {
namespace V2_2 {
namespace implementation {

// Methods from ::vendor::qti::hardware::perf::V2_0::IPerf follow.
Return<int32_t> Perf::perfLockAcquire(int32_t pl_handle, uint32_t duration, const hidl_vec<int32_t>& boostsList, const hidl_vec<int32_t>& reserved) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return -1;
}

Return<void> Perf::perfLockRelease(int32_t pl_handle, const hidl_vec<int32_t>& reserved) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return Void();
}

Return<int32_t> Perf::perfHint(uint32_t hint, const hidl_string& userDataStr, int32_t userData1, int32_t userData2, const hidl_vec<int32_t>& reserved) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return -1;
}

Return<int32_t> Perf::perfProfile(int32_t pl_handle, int32_t profile, int32_t reserved) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return -1;
}

Return<void> Perf::perfLockCmd(int32_t cmd, int32_t reserved) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return Void();
}

Return<void> Perf::perfGetProp(const hidl_string& propName, const hidl_string& defaultVal, perfGetProp_cb _hidl_cb) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
     _hidl_cb(defaultVal); /* return the defaultval*/
    return Void();
}

Return<int32_t> Perf::perfSetProp(const hidl_string& /*propName*/, const hidl_string& /*value*/) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return int32_t {};
}

Return<int32_t> Perf::perfAsyncRequest(int32_t /*cmd*/, const hidl_string& /*userDataStr*/, const hidl_vec<int32_t>& /*params*/) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return int32_t {};
}

Return<void> Perf::perfSyncRequest(int32_t cmd, const hidl_string& /*userDataStr*/,
                                   const hidl_vec<int32_t>& /*params*/,
                                   perfSyncRequest_cb _hidl_cb) {
    std::string res;
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    _hidl_cb(res);
    return Void();
}

Perf::Perf() {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
}

Perf::~Perf() {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
}

Return<int32_t> Perf::perfCallbackRegister(const sp<IPerfCallback>& callback, int32_t clientId){
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return 0;
}

Return<int32_t> Perf::perfCallbackDeregister(const sp<IPerfCallback>& callback, int32_t clientId){
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return 0;
}

void Perf::sendPerfCallback(uint32_t hint, const hidl_string& userDataStr, int32_t userData1, int32_t userData2, const hidl_vec<int32_t>& reserved) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
}

void Perf::sendPerfCallbackOffload(uint32_t hint, const hidl_string& userDataStr, int32_t userData1, int32_t userData2, const hidl_vec<int32_t>& reserved) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
}

Return<int32_t> Perf::perfLockAcqAndRelease(int32_t pl_handle, uint32_t duration, const hidl_vec<int32_t>& boostsList, const hidl_vec<int32_t>& reserved) {
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return -1;
}

Return<void> Perf::perfEvent(uint32_t eventId, const hidl_string& userDataStr, const hidl_vec<int32_t>& reserved){
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return Void();
}

Return<int32_t> Perf::perfGetFeedback(uint32_t featureId, const hidl_string& userDataStr, const hidl_vec<int32_t>& reserved){
    ALOGI("Perf optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return int32_t {};
}

}  // namespace implementation
}  // namespace V2_2
}  // namespace perf
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
