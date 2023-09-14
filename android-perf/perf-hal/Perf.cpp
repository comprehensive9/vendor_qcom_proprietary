/******************************************************************************
  @file    Perf.cpp
  @brief   Android performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <string>
#include <dlfcn.h>
#include <pthread.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include <hidl/HidlTransportSupport.h>
#include <hwbinder/IPCThreadState.h>
#include "Perf.h"
#include "PerfController.h"
#include "TargetConfig.h"
#include "PerfThreadPool.h"
#include <cutils/properties.h>
#include "MpctlUtils.h"
#include <stdio.h>
#include <stdlib.h>

enum INDEX_REFERENCE {INDEX_START = -1,
                      INDEX_CLIENT_TID = 0,
                      INDEX_CLIENT_PID = 1,
                      INDEX_REQUEST_OFFLOAD_FLAG = 2,
                      INDEX_REQUEST_TYPE = 3,
                      INDEX_REQUEST_DURATION = 4,
                      INDEX_END
                      };
/*
   INTERNAL_PRIVATE_EXTN_ARGS
   INDEX_CLIENT_TID,
   INDEX_CLIENT_PID,
   INDEX_REQUEST_OFFLOAD_FLAG
*/
#define INIT_NOT_COMPLETED (-3)

namespace vendor {
namespace qti {
namespace hardware {
namespace perf {
namespace V2_2 {
namespace implementation {

// Methods from ::vendor::qti::hardware::perf::V2_0::IPerf follow.
Return<int32_t> Perf::perfLockAcquire(int32_t pl_handle, uint32_t duration, const hidl_vec<int32_t>& boostsList, const hidl_vec<int32_t>& reserved) {
    mpctl_msg_t pMsg;
    int32_t retVal = -1;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKACQ;
    for (uint32_t i = 0; i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                    break;
            default: ALOGE("Unknown params");
                    break;
        }
    }
    uint32_t size  = boostsList.size();
    if (size > MAX_ARGS_PER_REQUEST) {
       ALOGE("Maximum number of arguments alowed exceeded");
       return retVal;
    }
    std::copy(boostsList.begin(), boostsList.end(), pMsg.pl_args);
    pMsg.data = size;
    pMsg.pl_time = duration;
    pMsg.pl_handle = pl_handle;
    if (pl_handle > 0) {
        pMsg.renewFlag = true;
    }

    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return retVal;
}

Return<void> Perf::perfLockRelease(int32_t pl_handle, const hidl_vec<int32_t>& reserved) {
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKREL;
    for (uint32_t i = 0; i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                    break;
            default: ALOGE("Unknown params");
                    break;
        }
    }
    pMsg.pl_handle = pl_handle;

    mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return Void();
}

Return<int32_t> Perf::perfHint(uint32_t hint, const hidl_string& userDataStr, int32_t userData1, int32_t userData2, const hidl_vec<int32_t>& reserved) {
    mpctl_msg_t pMsg;
    int32_t retVal = -1;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKHINTACQ;
    for (uint32_t i = 0; i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                    break;
            case INDEX_REQUEST_OFFLOAD_FLAG: pMsg.offloadFlag = reserved[i];
                    break;
            default: ALOGE("Unknown params");
                    break;
        }
    }
    pMsg.hint_id = hint;
    pMsg.pl_time = userData1;
    pMsg.hint_type = userData2;
    strlcpy(pMsg.usrdata_str, userDataStr.c_str(), MAX_MSG_APP_NAME_LEN);
    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    if (retVal != INIT_NOT_COMPLETED) {
        sendPerfCallbackOffload(hint, userDataStr, userData1, userData2, reserved);
    }
    return retVal;
}

Return<int32_t> Perf::perfProfile(int32_t pl_handle, int32_t profile, int32_t reserved) {
    mpctl_msg_t pMsg;
    int32_t retVal = -1;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCK_PROFILE;
    pMsg.client_pid = android::hardware::IPCThreadState::self()->getCallingPid();
    pMsg.client_tid = reserved;
    pMsg.pl_handle = pl_handle;
    pMsg.profile = profile;

    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return retVal;
}

Return<void> Perf::perfLockCmd(int32_t cmd, int32_t reserved) {
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = cmd;
    pMsg.client_pid = android::hardware::IPCThreadState::self()->getCallingPid();
    pMsg.client_tid = reserved;

    mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return Void();
}

Return<void> Perf::perfGetProp(const hidl_string& propName, const hidl_string& defaultVal, perfGetProp_cb _hidl_cb) {
    char prop_val[PROP_VAL_LENGTH], prop_name[PROP_VAL_LENGTH];
    char trace_buf[TRACE_BUF_SZ];
    char *retVal;
    strlcpy(prop_name, propName.c_str(), PROP_VAL_LENGTH);
    retVal = mPerfDataStore.GetProperty(prop_name, prop_val, sizeof(prop_val));

    if (retVal != NULL) {
         if (perf_debug_output) {
             snprintf(trace_buf, TRACE_BUF_SZ, "perfGetProp: Return Val from Perf.cpp is %s",retVal);
             ALOGE("%s", trace_buf);
         }
        _hidl_cb(prop_val);
    } else {
        _hidl_cb(defaultVal);
    }
    return Void();
}

Return<int32_t> Perf::perfSetProp(const hidl_string& /*propName*/, const hidl_string& /*value*/) {
    // TODO implement
    return int32_t {};
}

Return<int32_t> Perf::perfAsyncRequest(int32_t /*cmd*/, const hidl_string& /*userDataStr*/, const hidl_vec<int32_t>& /*params*/) {
    // TODO implement
    return int32_t {};
}

Return<void> Perf::perfSyncRequest(int32_t cmd, const hidl_string& /*userDataStr*/,
                                   const hidl_vec<int32_t>& /*params*/,
                                   perfSyncRequest_cb _hidl_cb) {
    std::string requestInfo = mImpl.PerfGlueLayerSyncRequest(cmd);
    _hidl_cb(requestInfo);
    return Void();
}

Perf::Perf() {
    Init();
}

Perf::~Perf() {
    mImpl.PerfGlueLayerExit();
}
void Perf::Init() {
    char trace_prop[PROPERTY_VALUE_MAX];

    //init PerfConfigStore
    mTargetConfig.InitializeTargetConfig();
    ALOGI("TargetConfig Init Complete");
    //init PerfConfigStore
    mPerfDataStore.ConfigStoreInit();

    //register mp-ctl
    mImpl.LoadPerfLib("libqti-perfd.so");

    char property[PROP_VAL_LENGTH] = "";

    perfGetProp("vendor.debug.enable.lm", "false", [&property](const auto &res) { strlcpy(property,res.c_str(),PROP_VAL_LENGTH);});
    bool enableLM = (!strncmp(property,"false",PROP_VAL_LENGTH))? false : true;
    if(enableLM) {
        ALOGE("LM enabled : Loading liblearningmodule.so");
        mImpl.LoadPerfLib("liblearningmodule.so");
    }

    perfGetProp("vendor.debug.enable.memperfd", "false", [&property](const auto &res) { strlcpy(property,res.c_str(),PROP_VAL_LENGTH);});
    bool enableMemperfd = (!strncmp(property,"false",PROP_VAL_LENGTH))? false : true;
    if(enableLM && enableMemperfd) {
        int memTotal = mTargetConfig.getRamInKB();
        if (memTotal > MEM_2GB && memTotal <= MEM_8GB) {
            mImpl.LoadPerfLib("libmemperfd.so");
        }
    }

    /* Enable systraces by adding vendor.debug.trace.perf=1 into build.prop */
    if (property_get(PROP_NAME, trace_prop, NULL) > 0) {
        if (trace_prop[0] == '1') {
            perf_debug_output = PERF_SYSTRACE = atoi(trace_prop);
        }
    }

    //init all modules
    mImpl.PerfGlueLayerInit();
}

Return<int32_t> Perf::perfCallbackRegister(const sp<IPerfCallback>& callback, int32_t clientId){
    uint32_t retVal = 0;
    if (callback == nullptr) {
        ALOGE("Invalid callback pointer PerfHAL ");
        return retVal;
    } else {
        retVal = 1;
    }

    std::lock_guard<std::mutex> _lock(perf_callback_mutex);
    if (std::any_of( callbacks.begin(), callbacks.end(), [&](const Perfcallback& c){
            return (c.clientId == clientId);
        })) {
        retVal = 0;
        ALOGE("Same callback interface registered already");
    } else {
        callbacks.emplace_back(callback, clientId);
        ALOGE("A callback has been registered to PerfHAL ");
    }
    return retVal;
}

Return<int32_t> Perf::perfCallbackDeregister(const sp<IPerfCallback>& callback, int32_t clientId){
    uint32_t retVal = 0;
    if (callback == nullptr) {
        ALOGE("Invalid callback pointer PerfHAL ");
        return retVal;
    } else {
        retVal = 1;
    }
    bool removed = false;
    std::lock_guard<std::mutex> _lock(perf_callback_mutex);
    callbacks.erase(
        std::remove_if(callbacks.begin(), callbacks.end(), [&](const Perfcallback& c) {
            if (c.clientId == clientId) {
                ALOGE("a callback has been unregistered to PerfHAL");
                removed = true;
                return true;
            }
            return false;
        }), callbacks.end());
    if (!removed) {
        ALOGE("The callback was not registered before");
    }
    return retVal;
}

void Perf::sendPerfCallback(uint32_t hint, const hidl_string& userDataStr, int32_t userData1, int32_t userData2, const hidl_vec<int32_t>& reserved) {
    std::unique_lock<std::mutex> _lock(perf_callback_mutex, std::try_to_lock);
    if (_lock.owns_lock() == false) {
        return;
    }
    for(const auto& c : callbacks){
        if(c.callback == nullptr){
            ALOGE("Invalid callback pointer");
        }else{
            Return<void> ret = c.callback->notifyCallback(hint, userDataStr, userData1, userData2, reserved);
            if(!ret.isOk()){
                ALOGE("NotifyCallback is not called");
            }
        }
    }
}

void Perf::sendPerfCallbackOffload(uint32_t hint, const hidl_string& userDataStr, int32_t userData1, int32_t userData2, const hidl_vec<int32_t>& reserved) {
    int rc = FAILED;

    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool();
    try {
        rc = ptp.placeTask([=]() {
                sendPerfCallback(hint, userDataStr, userData1, userData2, reserved);
                });
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    if(rc == FAILED) {
        QLOGE("Failed to Offload %s task", __func__);
    }
}

Return<int32_t> Perf::perfLockAcqAndRelease(int32_t pl_handle, uint32_t duration, const hidl_vec<int32_t>& boostsList, const hidl_vec<int32_t>& reserved) {
    mpctl_msg_t pMsg;
    int32_t retVal = -1;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKREL;
    for (uint32_t i = 0; i < 2 && i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                                   break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                                   break;
            default: ALOGE("Unknown params");
                     break;
        }
    }
    pMsg.pl_handle = pl_handle;

    mImpl.PerfGlueLayerSubmitRequest(&pMsg);

    memset(&pMsg, 0, sizeof(mpctl_msg_t));
    pMsg.req_type = MPCTL_CMD_PERFLOCKACQ;
    for (uint32_t i = 0; i < 2 && i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                                   break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                                   break;
            default: ALOGE("Unknown params");
                     break;
        }
    }
    uint32_t size  = boostsList.size();
    if (size > MAX_ARGS_PER_REQUEST) {
        ALOGE("Maximum number of arguments alowed exceeded");
        return retVal;
    }
    std::copy(boostsList.begin(), boostsList.end(), pMsg.pl_args);
    pMsg.data = size;
    pMsg.pl_time = duration;

    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return retVal;
}

Return<void> Perf::perfEvent(uint32_t eventId, const hidl_string& userDataStr, const hidl_vec<int32_t>& reserved){
    mpctl_msg_t pMsg;
    int32_t retVal = -1;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));
    pMsg.hint_id = eventId;
    pMsg.pl_time = -1;
    pMsg.hint_type = -1;

    pMsg.req_type = MPCTL_CMD_PERFLOCKHINTACQ;
    for (uint32_t i = 0; i < reserved.size(); i++) {
        switch (i) {
            case INDEX_CLIENT_TID: pMsg.client_tid = reserved[i];
                    break;
            case INDEX_CLIENT_PID: pMsg.client_pid = reserved[i];
                    break;
            case INDEX_REQUEST_OFFLOAD_FLAG: pMsg.offloadFlag = reserved[i];
                    break;
            case INDEX_REQUEST_TYPE: pMsg.hint_type = reserved[i];
                    break;
            case INDEX_REQUEST_DURATION: pMsg.pl_time = reserved[i];
                    break;
            default: ALOGE("Unknown params");
                    break;
        }
    }
    strlcpy(pMsg.usrdata_str, userDataStr.c_str(), MAX_MSG_APP_NAME_LEN);
    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    sendPerfCallbackOffload(pMsg.hint_id, userDataStr, pMsg.pl_time, pMsg.hint_type, reserved);
    return Void();
}

Return<int32_t> Perf::perfGetFeedback(uint32_t featureId, const hidl_string& userDataStr, const hidl_vec<int32_t>& reserved){
//for backward compatibility
    return perfHint(featureId, userDataStr, 0, 0, reserved);
}

}  // namespace implementation
}  // namespace V2_2
}  // namespace perf
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
