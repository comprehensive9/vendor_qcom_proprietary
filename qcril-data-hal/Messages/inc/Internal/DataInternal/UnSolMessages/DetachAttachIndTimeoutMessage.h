/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

/********************** Class Definitions *************************/
class DetachAttachIndTimeoutMessage: public UnSolicitedMessage,
                           public add_message_id<DetachAttachIndTimeoutMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "DetachAttachIndTimeoutMessage";

  DetachAttachIndTimeoutMessage():UnSolicitedMessage(get_class_message_id()){}
  DetachAttachIndTimeoutMessage(sigval val):UnSolicitedMessage(get_class_message_id())
  {
    memcpy(&mSig, &val, sizeof(mSig));
  }
  ~DetachAttachIndTimeoutMessage() {}

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::make_shared<DetachAttachIndTimeoutMessage>();
  }

  string dump()
  {
    return MESSAGE_NAME;
  }

  union sigval getSigVal()
  {
    return mSig;
  }

  private:
    union sigval mSig;
};

