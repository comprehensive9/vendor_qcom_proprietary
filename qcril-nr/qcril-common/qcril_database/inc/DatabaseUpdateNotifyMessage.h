/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class DatabaseUpdateNotifyMessage : public UnSolicitedMessage,
                            public add_message_id<DatabaseUpdateNotifyMessage> {
 private:

 public:
  static constexpr const char *MESSAGE_NAME = "DATABASE_UPDATE_NOTIFYING";
  inline DatabaseUpdateNotifyMessage() :
        UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  ~DatabaseUpdateNotifyMessage() {}

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<DatabaseUpdateNotifyMessage>();
    return msg;
  }

  string dump() { return MESSAGE_NAME; }
};
