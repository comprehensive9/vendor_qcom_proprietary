/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_QTI_VENDOR_LIMITS_H
#define ANDROID_QTI_VENDOR_LIMITS_H


#include <vendor/qti/hardware/limits/1.0/ILimits.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "LimitsEstimate.h"

namespace vendor::qti::hardware::limits::implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hardware::thermal::V1_0::ThermalStatus;
using ::android::hardware::thermal::V1_0::ThermalStatusCode;

struct Limits : public V1_0::ILimits {
	Limits();
	~Limits() = default;

    // Methods from ::vendor::qti::hardware::limits::V1_0::ILimits follow.
    Return<void> getThermalHeadRoom(getThermalHeadRoom_cb _hidl_cb) override;
    Return<void> stopHeadRoomCalculation() override;
    Return<void> startHeadRoomCalculation() override;

private:
	LimitsEstimate le;

};

}  // namespace vendor::qti::hardware::limits::implementation

#endif  // ANDROID_QTI_VENDOR_LIMITS_H
