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

#include <vendor/qti/hardware/bluetooth_audio/2.1/IBluetoothAudioProvider.h>
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
using ::android::wp;
using ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType;
using ::android::hardware::hidl_death_recipient;
using BluetoothAudioStatus =
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType;
using AudioConfiguration_2_0 =
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration;
using BluetoothAudioPort_2_0 =
    ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort;
class BluetoothAudioDeathRecipient;

struct BluetoothAudioProvider : public IBluetoothAudioProvider {
 public:
  BluetoothAudioProvider();
  ~BluetoothAudioProvider() = default;
    virtual bool isValid(const SessionType& sessionType) = 0;
    // Methods from ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioProvider follow.
    Return<void> startSession(const sp<::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort>& hostIf,
       const ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration& audioConfig, 
       startSession_cb _hidl_cb) override;
    Return<void> streamStarted(BluetoothAudioStatus status) override;
    Return<void> streamSuspended(BluetoothAudioStatus status) override;
    Return<void> updateSessionParams(const SessionParams& params) override;
    //Return<void> streamStarted(::vendor::qti::hardware::bluetooth_audio::V2_0::Status status) override;
    //Return<void> streamSuspended(::vendor::qti::hardware::bluetooth_audio::V2_0::Status status) override;
    //Return<void> updateSessionParams(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams& sessionParams) override;
    Return<void> endSession() override;

    // Methods from ::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioProvider follow.
    Return<void> startSession_2_1(const sp<::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioPort>& hostIf,
       const ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration& audioConfig, startSession_2_1_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
  protected:
   sp<BluetoothAudioDeathRecipient> death_recipient_;
   AudioConfiguration audio_config_;
   AudioConfiguration_2_0 audio_config_2_0_;
   sp<IBluetoothAudioPort> stack_iface_;
   sp<BluetoothAudioPort_2_0> stack_iface_2_0_;
   std::mutex mutex_;
   SessionType session_type_;
   bool client_2_0;
   virtual Return<void> onSessionReady(startSession_2_1_cb _hidl_cb) = 0;
   bool isClient_2_0() { return client_2_0;}
};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" IBluetoothAudioProvider* HIDL_FETCH_IBluetoothAudioProvider(const char* name);
class BluetoothAudioDeathRecipient : public hidl_death_recipient {
 public:
  BluetoothAudioDeathRecipient(const sp<BluetoothAudioProvider> provider)
      : provider_(provider) {}

  virtual void serviceDied(
      uint64_t cookie,
      const wp<::android::hidl::base::V1_0::IBase>& who) override;

 private:
  sp<BluetoothAudioProvider> provider_;
};

}  // namespace implementation
}  // namespace V2_1
}  // namespace bluetooth_audio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
