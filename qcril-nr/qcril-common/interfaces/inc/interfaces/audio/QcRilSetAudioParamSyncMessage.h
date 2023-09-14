/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "interfaces/audio/audio_types.h"
#include "voice_service_v02.h"

typedef struct QcRilSetAudioParams {
  uint32_t vsid;
  qcril::interfaces::AudioCallState call_state;
  call_mode_enum_v02 call_mode;
  QcRilSetAudioParams(uint32_t id, qcril::interfaces::AudioCallState state, call_mode_enum_v02 mode)
      : vsid(id), call_state(state), call_mode(mode){};
} QcRilSetAudioParams;

/*===========================================================================*/
/*!
    @brief
    Sets the Audio Parameters with AM client.

    mString       - Audio Parameters
*/
/*==========================================================================*/
class QcRilSetAudioParamSyncMessage : public SolicitedSyncMessage<RIL_Errno>,
                                      public add_message_id<QcRilSetAudioParamSyncMessage> {
 private:
  std::vector<QcRilSetAudioParams> mAudioParams;

 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.audio.set_audio_param_request";
  QcRilSetAudioParamSyncMessage() = delete;
  ~QcRilSetAudioParamSyncMessage() = default;

  QcRilSetAudioParamSyncMessage(std::vector<QcRilSetAudioParams> params)
      : SolicitedSyncMessage<RIL_Errno>(get_class_message_id()), mAudioParams(params) {
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    string dump_message = mName;
    for (auto& i : mAudioParams) {
      dump_message += "{Vsid:" + std::to_string(i.vsid) +
                      " CallState:" + qcril::interfaces::toString(i.call_state) +
                      " CallMode:" + std::to_string(i.call_mode) + "},";
    }
    return dump_message;
  }
  std::vector<QcRilSetAudioParams> getAudioParams() {
    return mAudioParams;
  }
};
