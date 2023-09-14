/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef SETINITIALATTACHCOMPLETEDMESSAGE
#define SETINITIALATTACHCOMPLETEDMESSAGE

#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"

namespace rildata {

/********************** Class Definitions *************************/
class SetInitialAttachCompletedMessage: public SolicitedSyncMessage<int>,
                           public add_message_id<SetInitialAttachCompletedMessage> {
private:
  bool mStatus;
  std::shared_ptr<SetInitialAttachApnRequestMessage> mMsg;
  bool processIACompleted;
public:
  static constexpr const char *MESSAGE_NAME = "SetInitialAttachCompletedMessage";
  inline SetInitialAttachCompletedMessage():
   SolicitedSyncMessage<int>(get_class_message_id()) {
    mName = MESSAGE_NAME;
    processIACompleted = true;
  }
  ~SetInitialAttachCompletedMessage() {};

  void setStatus (bool status) { mStatus = status;}
  bool getStatus () { return mStatus; }
  void setProcessIACompleted (bool iAcompleted) { processIACompleted = iAcompleted;}
  bool getProcessIACompleted () { return processIACompleted;}
  void setMsg(std::shared_ptr<SetInitialAttachApnRequestMessage> m) { mMsg = m;}
  std::shared_ptr<SetInitialAttachApnRequestMessage> getMsg() { return mMsg; }
  string dump() {
    return mName + " Status: " + std::to_string(mStatus);
  }
};
}

#endif
