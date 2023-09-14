/*==============================================================================
*  Copyright (c) 2020 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
#pragma once

#include <log/log.h>
#include <stdlib.h>
#include <utils/Log.h>
#include <vendor/qti/hardware/wifi/wifimyftm/1.0/IWifiMyFtm.h>
#include <vendor/qti/hardware/wifi/wifimyftm/1.0/types.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace wifi {
namespace wifimyftm {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hardware::hidl_handle;
//using ::vendor::qti::wifi::wifimyftm::V1_0::Command;

struct WifiMyFtm : public IWifiMyFtm {
	// Methods from ::vendor::qti::wifi::wifimyftm::V1_0::IWifiMyFtm follow.
	Return<void> myftmCmd(const hidl_string& arg, myftmCmd_cb _hidl_cb);
	// Methods from ::android::hidl::base::V1_0::IBase follow.
	static IWifiMyFtm* getInstance(void);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace wifimyftm
}  // namespace wifi
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
