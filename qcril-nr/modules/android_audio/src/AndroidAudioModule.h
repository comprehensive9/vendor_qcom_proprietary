/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include <interfaces/audio/QcRilGetAudioParamSyncMessage.h>
#include <interfaces/audio/QcRilSetAudioParamSyncMessage.h>
#include <src/hidl_impl/base/qcril_qmi_audio_service.h>
#include "qcril_am.h"

namespace ril {
namespace modules {

class AndroidAudioModule : public Module {
 public:
  AndroidAudioModule();
  ~AndroidAudioModule();
  void init();

 private:
  sp<vendor::qti::hardware::radio::am::V1_0::implementation::qcril_audio_impl> mAudioService;

  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void initAndroidAudio(qcril_instance_id_e_type instance_id);
  void handleQcrilGetAudioParameters(std::shared_ptr<QcRilGetAudioParamSyncMessage> msg);
  void handleQcrilSetAudioParameters(std::shared_ptr<QcRilSetAudioParamSyncMessage> msg);
  android::String8 convertRilAudioParamtersToAndroid(QcRilSetAudioParams params);
  uint32_t convertCallStateToAudioNum(qcril::interfaces::AudioCallState call_state);
  qcril::interfaces::AudioCallState convertAudioNumToCallState(uint32_t call_state);
};

}  // namespace modules
}  // namespace ril
