/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/voice/voice.h>
#include <string.h>
#include <optional>

/*
 * Unsol message to notify sip dtmf
 */
class QcRilUnsolOnSipDtmfMessage : public UnSolicitedMessage,
                                public add_message_id<QcRilUnsolOnSipDtmfMessage> {
 private:

  std::optional<std::string> mMessage;

 public:
  static constexpr const char *MESSAGE_NAME = "QcRilUnsolOnSipDtmfMessage";
  ~QcRilUnsolOnSipDtmfMessage() {}

  QcRilUnsolOnSipDtmfMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    std::shared_ptr<QcRilUnsolOnSipDtmfMessage> msg = std::make_shared<QcRilUnsolOnSipDtmfMessage>();
    if (msg) {
      if (mMessage) {
        msg->setMessage(*mMessage);
      }
    }
    return msg;
  }

  bool hasMessage() { return mMessage ? true : false; }
  const std::string &getMessage() { return *mMessage; }
  void setMessage(const std::string &val) { mMessage = val; }
  inline std::string dumpMessage() { return " mMessage = " + (mMessage ? *mMessage : "<invalid>"); }

  std::string dump() { return mName + "{" + dumpMessage() + "}"; }
};
