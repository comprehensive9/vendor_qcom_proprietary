/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "vendor.qti.hardware.limits"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include "Limits.h"

using ::android::OK;
using ::android::status_t;

// libhwbinder:
using ::android::hardware::configureRpcThreadpool;
using ::android::hardware::joinRpcThreadpool;

// Generated HIDL files:
using ::vendor::qti::hardware::limits::V1_0::ILimits;
using ::vendor::qti::hardware::limits::implementation::Limits;

static int shutdown() {
	LOG(ERROR) << "Limits HAL Service is shutting down.";
	return 1;
}

int main(int /* argc */, char ** /* argv */) {
	status_t status;
	android::sp<ILimits> service = nullptr;

	LOG(INFO) << "Limits HAL Service 1.0 starting...";

	service = new Limits();
	if (service == nullptr) {
		LOG(ERROR) << "Error creating an instance of Limits HAL. Exiting...";
		return shutdown();
	}

	configureRpcThreadpool(1, true /* callerWillJoin */);

	status = service->registerAsService();
	if (status != OK) {
		LOG(ERROR) << "Could not register service for LimitsHAL (" << status << ")";
		return shutdown();
	}

	LOG(INFO) << "Limits HAL Service 1.0 started successfully.";
	joinRpcThreadpool();
	// We should not get past the joinRpcThreadpool().
	LOG(INFO) << "Limits HAL Service 1.0 shutting down.";
	return shutdown();
}
