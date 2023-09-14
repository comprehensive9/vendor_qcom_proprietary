/******************************************************************************
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#ifndef VENDOR_QTI_HARDWARE_ACTIVITYEVENTINFOCALLBACK_V1_2_H
#define VENDOR_QTI_HARDWARE_ACTIVITYEVENTINFOCALLBACK_V1_2_H

#include <vendor/qti/hardware/servicetracker/1.2/IActivityEventInfoCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace servicetracker {
namespace V1_2 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct ActivityEventInfoCallback : public IActivityEventInfoCallback {
    // Methods from ::vendor::qti::hardware::servicetracker::V1_2::IActivityEventInfoCallback follow.
    Return<void> notifyActivityEvent(::vendor::qti::hardware::servicetracker::V1_2::ActivityStates aState,
                                     const ::vendor::qti::hardware::servicetracker::V1_2::ActivityDetails& aDetails,
                                     const ::vendor::qti::hardware::servicetracker::V1_2::ActivityStats& aStats,
                                     bool early_notify, int32_t userata) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" IActivityEventInfoCallback* HIDL_FETCH_IActivityEventInfoCallback(const char* name);

}  // namespace implementation
}  // namespace V1_2
}  // namespace servicetracker
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
#endif /*VENDOR_QTI_HARDWARE_ACTIVITYEVENTINFOCALLBACK_V1_2_H */
