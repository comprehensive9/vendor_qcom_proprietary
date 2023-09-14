/******************************************************************************
  @file    IOP_Stub.cpp
  @brief   Android IOP HAL stub module
           These functions called when perf opts are disabled during target bringup

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017,2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <utils/Log.h>
#include "Iop.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace iop {
namespace V2_0 {
namespace implementation {

// Methods from ::vendor::qti::hardware::iop::V2_0::IIop follow.
Return<int32_t> Iop::iopStart(int32_t pid, const hidl_string& pkg_name, const hidl_string& code_path) {
    ALOGI("IOP optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return -1;
}

Return<void> Iop::iopStop() {
    ALOGI("IOP optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return Void();
}

Return<int32_t> Iop::uxEngine_events(int32_t opcode, int32_t pid, const hidl_string& pkg_name, int32_t lat) {
    ALOGI("IOP optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return -1;
}

Return<void> Iop::uxEngine_trigger(int32_t opcode, uxEngine_trigger_cb _hidl_cb) {
    ALOGI("IOP optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    _hidl_cb("");
    return Void();
}

Iop::Iop() {
    ALOGI("IOP optimizations disabled in this build, stub function called for %s",__FUNCTION__);
}

Iop::~Iop() {
}

void Iop::LoadIopLib() {
    ALOGI("IOP optimizations disabled in this build, stub function called for %s",__FUNCTION__);
    return;
}

// Methods from ::android::hidl::base::V2_0::IBase follow.

}  // namespace implementation
}  // namespace V2_0
}  // namespace iop
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
