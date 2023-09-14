/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef SETPREFERREDSYSTEMMESSAGE
#define SETPREFERREDSYSTEMMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class SetPreferredSystemMessage: public UnSolicitedMessage,
                           public add_message_id<SetPreferredSystemMessage> {

private:
  int32_t mCid;
  HandoffNetworkType_t mPrefNetwork;
public:
  static constexpr const char *MESSAGE_NAME = "SetPreferredSystemMessage";

  SetPreferredSystemMessage() = delete;
  ~SetPreferredSystemMessage() {}
  SetPreferredSystemMessage(int32_t cid, HandoffNetworkType_t prefNetwork):
    UnSolicitedMessage(get_class_message_id()), mCid(cid), mPrefNetwork(prefNetwork) {
    mName = MESSAGE_NAME;
  }

  int32_t getCid() { return mCid; }
  HandoffNetworkType_t getPrefNetwork() { return mPrefNetwork; }

  string dump(){return mName + " cid=" + std::to_string(mCid) + " network=" + std::to_string((int)mPrefNetwork);}

  std::shared_ptr<UnSolicitedMessage> clone(){
    return std::make_shared<SetPreferredSystemMessage>(mCid, mPrefNetwork);
  }
};

} //namespace

#endif