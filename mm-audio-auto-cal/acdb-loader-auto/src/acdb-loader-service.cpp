/* Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <assert.h>

#include "acdb-loader-server.h"

#define LOG_TAG "acdb_loader_service"

using namespace audiocal;

int main(int argc, char **argv) {
    int32_t err;
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    assert(sm != 0);
    err = sm->addService(android::String16(ACDB_LOADER_SERVICE), new acdb_loader_server(), false);
    ALOGD("ACDB Loader Server is alive Err = %d", err);
    android::ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
}

