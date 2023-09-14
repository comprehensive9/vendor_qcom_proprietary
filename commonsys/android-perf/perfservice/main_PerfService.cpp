/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <log/log.h>
#include "PerfService.h"
#include <minijail.h>
using android::SetUpMinijail;

#undef LOG_TAG
#define LOG_TAG    "PerfDaemon"

using namespace android;
const char perfServiceSeccompPolicyPath[] = "/system/system_ext/etc/seccomp_policy/perfservice.policy";

int main() {
    ALOGI("PerfDaemon is starting.");
    SetUpMinijail(perfServiceSeccompPolicyPath, "");
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();
    auto tmp = new(std::nothrow) PerfService;
    if (tmp != NULL) {
        defaultServiceManager()->addService(String16("vendor.perfservice"), tmp);
        if(defaultServiceManager()->checkService(
                    String16("vendor.perfservice"))== NULL) {
            ALOGE("adding vendor.perfservice failed!");
        } else {
            ALOGD("added vendor.perfservice to services and is now running.");
        }
    }
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
