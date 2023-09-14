/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DSIINITCOMPLETEDMESSAGE
#define DSIINITCOMPLETEDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class DsiInitCompletedMessage: public UnSolicitedMessage,
                           public add_message_id<DsiInitCompletedMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "DsiInitCompletedMessage";

  DsiInitCompletedMessage():
    UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  ~DsiInitCompletedMessage() {}

  string dump(){return mName;}

  std::shared_ptr<UnSolicitedMessage> clone(){
    return std::make_shared<DsiInitCompletedMessage>();
  }
};

} //namespace

#endif