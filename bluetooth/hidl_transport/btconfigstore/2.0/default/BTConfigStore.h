/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef VENDOR_QTI_HARDWARE_BTCONFIGSTORE_V2_0_IMPL_H
#define VENDOR_QTI_HARDWARE_BTCONFIGSTORE_V2_0_IMPL_H

#pragma once

#include <vendor/qti/hardware/btconfigstore/2.0/IBTConfigStore.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace btconfigstore {
namespace V2_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::vendor::qti::hardware::btconfigstore::V2_0::VendorProperty;
using ::vendor::qti::hardware::btconfigstore::V2_0::Result;
using ::vendor::qti::hardware::btconfigstore::V2_0::HostAddOnFeatures;
using ::vendor::qti::hardware::btconfigstore::V2_0::ControllerAddOnFeatures;

struct BTConfigStore : public IBTConfigStore {
    // Methods from ::vendor::qti::hardware::btconfigstore::V2_0::IBTConfigStore follow.
    Return<void> getVendorProperties(const uint32_t vPropType,
                                     getVendorProperties_cb _hidl_cb) override;
    Return<Result> setVendorProperty(const VendorProperty& vendorProp) override;
    Return<void> getHostAddOnFeatures(getHostAddOnFeatures_cb _hidl_cb) override;
    Return<void> getControllerAddOnFeatures(getControllerAddOnFeatures_cb _hidl_cb) override;

};

extern "C" IBTConfigStore* HIDL_FETCH_IBTConfigStore(const char* name);

}  // namespace implementation
}  // namespace V2_0
}  // namespace btconfigstore
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
#endif
