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

#include "BluetoothAudioSession.h"
#include <vendor/qti/hardware/bluetooth_audio/2.1/IBluetoothAudioPort.h>
namespace vendor {
namespace qti {
namespace bluetooth_audio {
namespace V2_1 {
using ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType;
using ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration;
using ::vendor::qti::bluetooth_audio::V2_1::BluetoothAudioSession;
using AudioConfiguration_2_0 =
     ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration;
class BluetoothAudioSessionReport {
 public:
  // The API reports the Bluetooth stack has started the session, and will
  // inform registered bluetooth_audio outputs
  static void OnSessionStarted(const SessionType& session_type,
                               const sp<IBluetoothAudioPort> host_iface,
                               const DataMQ::Descriptor* dataMQ,
                               const AudioConfiguration& audio_config,
                               const AudioConfiguration_2_0& audio_config_2_0,
                               const sp<BluetoothAudioPort_2_0> host_iface_2_0,
                               bool hal_version) {
    std::shared_ptr<BluetoothAudioSession> session_ptr =
        BluetoothAudioSessionInstance::GetSessionInstance(session_type);
    if (session_ptr != nullptr) {
      session_ptr->OnSessionStarted(host_iface, dataMQ, audio_config,
                               audio_config_2_0, host_iface_2_0, hal_version);
    }
  }
  // The API reports the Bluetooth stack has ended the session, and will
  // inform registered bluetooth_audio outputs
  static void OnSessionEnded(const SessionType& session_type) {
    std::shared_ptr<BluetoothAudioSession> session_ptr =
        BluetoothAudioSessionInstance::GetSessionInstance(session_type);
    if (session_ptr != nullptr) {
      session_ptr->OnSessionEnded();
    }
  }
  // The API reports the Bluetooth stack has replied the result of startStream
  // or suspendStream, and will inform registered bluetooth_audio outputs
  static void ReportControlStatus(const SessionType& session_type,
                                  const bool& start_resp,
                                  const BluetoothAudioStatus& status) {
    std::shared_ptr<BluetoothAudioSession> session_ptr =
        BluetoothAudioSessionInstance::GetSessionInstance(session_type);
    if (session_ptr != nullptr) {
      session_ptr->ReportControlStatus(start_resp, status);
    }
  }
  static void OnSessionParamUpdate(const SessionType& session_type,
                                  const SessionParamType& paramType,
                                  const SessionParams& sessionParams) {
    std::shared_ptr<BluetoothAudioSession> session_ptr =
        BluetoothAudioSessionInstance::GetSessionInstance(session_type);
    if (session_ptr != nullptr) {
      session_ptr->OnSessionParamUpdate(paramType, sessionParams);
    }
  }

};
}  //V2_1
}  // namespace bluetooth_audio
}  // namespace qti
}  // namespace vendor
