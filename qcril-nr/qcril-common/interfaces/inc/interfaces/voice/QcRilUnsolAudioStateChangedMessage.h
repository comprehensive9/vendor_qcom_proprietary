/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/voice/voice.h>

/*
 * Unsol message for Audio State Change
 */
class QcRilUnsolAudioStateChangedMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilUnsolAudioStateChangedMessage> {
 private:
  std::string mParams;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilUnsolAudioStateChangedMessage";
  ~QcRilUnsolAudioStateChangedMessage() {
  }

  QcRilUnsolAudioStateChangedMessage(std::string params)
      : UnSolicitedMessage(get_class_message_id()), mParams(params) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    std::shared_ptr<QcRilUnsolAudioStateChangedMessage> msg =
        std::make_shared<QcRilUnsolAudioStateChangedMessage>(mParams);
    return msg;
  }

  std::string getAudioParams() {
    return mParams;
  }

  std::string dump() {
    return mName + "{" + mParams + "}";
  }
};
