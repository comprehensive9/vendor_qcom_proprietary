/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */

/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <vendor/qti/hardware/bluetooth_audio/2.1/IBluetoothAudioProvidersFactory.h>
#include "A2dpOffloadAudioProvider.h"
#include "A2dpSoftwareAudioProvider.h"
#include "BluetoothAudioProvider.h"
#include "HearingAidAudioProvider.h"

#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace bluetooth_audio {
namespace V2_1 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters;
using CodecCapabilities_2_0 =
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities;
using AudioCapabilities_2_0 =
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioCapabilities;

struct BluetoothAudioProvidersFactory : public IBluetoothAudioProvidersFactory {
 public:
  BluetoothAudioProvidersFactory() {}
    // Methods from ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioProvidersFactory follow.
    Return<void> openProvider(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType sessionType, openProvider_cb _hidl_cb) override;
    Return<void> getProviderCapabilities(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType sessionType, getProviderCapabilities_cb _hidl_cb) override;

    // Methods from ::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioProvidersFactory follow.
    Return<void> openProvider_2_1(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType sessionType, openProvider_2_1_cb _hidl_cb) override;
    Return<void> getProviderCapabilities_2_1(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType sessionType, getProviderCapabilities_2_1_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

 private:
  static A2dpSoftwareAudioProvider a2dp_software_provider_instance_;
  static A2dpOffloadAudioProvider a2dp_offload_provider_instance_;
  static HearingAidAudioProvider hearing_aid_provider_instance_;
};

extern "C" IBluetoothAudioProvidersFactory*
HIDL_FETCH_IBluetoothAudioProvidersFactory(const char* name);

}  // namespace implementation
}  // namespace V2_1
}  // namespace bluetooth_audio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
