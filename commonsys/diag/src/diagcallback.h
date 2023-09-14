/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Internal Header File for hidl communication between diag lib, driver

GENERAL DESCRIPTION

Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#pragma once

#include <vendor/qti/diaghal/1.0/Idiagcallback.h>
#include <vendor/qti/diaghal/1.0/types.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>
#include <hidl/LegacySupport.h>

namespace vendor {
namespace qti {
namespace diaghal {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hidl::allocator::V1_0::IAllocator;
using ::android::hidl::memory::V1_0::IMemory;
using ::android::hardware::HidlMemory;

struct diagcallback : public Idiagcallback {
    // Methods from ::vendor::qti::diaghal::V1_0::Idiagcallback follow.
    Return<int32_t> send_data(const hidl_memory& buf, uint32_t len) override;
    static Idiagcallback* getInstance(void);

};

} // end of implementation
} // end of v1_0
} // end of diaghal
} // end of qti
}  // namespace vendor::qti::diaghal::v1_0::implementation
