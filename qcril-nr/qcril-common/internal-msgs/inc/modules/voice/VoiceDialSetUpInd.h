/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>


class VoiceDialSetUpInd : public UnSolicitedMessage, public add_message_id<VoiceDialSetUpInd> {
  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.voice.dial_setup_ind";

    VoiceDialSetUpInd():UnSolicitedMessage(get_class_message_id())
    {
      mName = VoiceDialSetUpInd::MESSAGE_NAME;
    }

    inline ~VoiceDialSetUpInd() {}

    std::shared_ptr<UnSolicitedMessage> clone() {
      return (std::make_shared<VoiceDialSetUpInd>());
    }

    string dump() {
      return VoiceDialSetUpInd::MESSAGE_NAME;
    }
};
