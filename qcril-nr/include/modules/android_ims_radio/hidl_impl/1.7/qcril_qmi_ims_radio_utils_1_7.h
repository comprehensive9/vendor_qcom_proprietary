/*===========================================================================
   Copyright (c) 2020 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once

#include <hidl/HidlSupport.h>
#include <modules/android_ims_radio/hidl_impl/base/qcril_qmi_ims_radio_utils.h>
#include <modules/android_ims_radio/hidl_impl/1.6/qcril_qmi_ims_radio_utils_1_6.h>
#include <vendor/qti/hardware/radio/ims/1.7/types.h>

using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_vec;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace V1_7 {
namespace utils {

void convertCrsType(const qcril::interfaces::CrsType &in,
        hidl_bitfield<V1_7::CrsType>& crsType);
bool convertCallInfoList(hidl_vec<V1_7::CallInfo> &out,
                         const std::vector<qcril::interfaces::CallInfo> &in);
bool convertCallInfo(V1_7::CallInfo &out, const qcril::interfaces::CallInfo &in);
}  // namespace utils
}  // namespace V1_7
}  // namespace ims
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
