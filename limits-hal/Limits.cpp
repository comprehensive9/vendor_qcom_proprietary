/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android-base/logging.h>
#include "Limits.h"
#include "LimitsEstimate.h"

namespace vendor::qti::hardware::limits::implementation {

Limits::Limits(): le()
{}

// Methods from ::vendor::qti::hardware::limits::V1_0::ILimits follow.
Return<void> Limits::getThermalHeadRoom(getThermalHeadRoom_cb _hidl_cb) {
	float hr = 0;
	ThermalStatus status;
	status.code = ThermalStatusCode::SUCCESS;

	hr = le.getHeadRoom();
	if (hr < 0.0) {
		status.code = ThermalStatusCode::FAILURE;
		status.debugMessage = "Error getting Head Room.";
		LOG(ERROR) << "Error getting Head Room.";
	}

	_hidl_cb(status, hr);

    return Void();
}

Return<void> Limits::startHeadRoomCalculation() {

	le.startEstimate();

    return Void();
}

Return<void> Limits::stopHeadRoomCalculation() {

	le.stopEstimate();

    return Void();
}


}  // namespace vendor::qti::hardware::limits::implementation
