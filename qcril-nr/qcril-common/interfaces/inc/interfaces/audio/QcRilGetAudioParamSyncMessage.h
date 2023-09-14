/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "interfaces/audio/audio_types.h"

struct QcRilGetAudioParams {
  qcril::interfaces::AudioCallState call_state;
};

/*===========================================================================*/
/*!
    @brief
    Provides the Audio Parameters set with AM Service.

    mVsid       - Vsid for which Audio Parameters are requested

    Retruns Audio Paramters
*/
/*==========================================================================*/
class QcRilGetAudioParamSyncMessage : public SolicitedSyncMessage<QcRilGetAudioParams>,
                                      public add_message_id<QcRilGetAudioParamSyncMessage> {
 private:
  string mVsid;

 public:
  static constexpr const char* MESSAGE_NAME =
      "com.qualcomm.qti.qcril.audio.get_audio_param_request";
  QcRilGetAudioParamSyncMessage() = delete;
  ~QcRilGetAudioParamSyncMessage() = default;

  QcRilGetAudioParamSyncMessage(uint32_t vsid)
      : SolicitedSyncMessage<QcRilGetAudioParams>(get_class_message_id()) {
    mVsid = std::to_string(vsid);
    mName = MESSAGE_NAME;
  }
  inline string dump() {
    return mName + "{ Call Vsid :" + mVsid + "}";
  }

  string getVsid() {
    return mVsid;
  }
};
