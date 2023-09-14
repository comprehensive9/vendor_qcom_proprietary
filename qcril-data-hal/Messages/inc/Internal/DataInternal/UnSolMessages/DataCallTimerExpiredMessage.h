/**
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATACALLTIMEREXPIREDMESSAGE
#define DATACALLTIMEREXPIREDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class DataCallTimerExpiredMessage: public UnSolicitedMessage,
                           public add_message_id<DataCallTimerExpiredMessage> {
private:
  int mCid;
  DataCallTimerType mType;

public:
  static constexpr const char *MESSAGE_NAME = "DataCallTimerExpiredMessage";

  DataCallTimerExpiredMessage(int cid, DataCallTimerType type);
  ~DataCallTimerExpiredMessage();

  int getCid();
  DataCallTimerType getType();
  static string getStringType(DataCallTimerType);

  std::shared_ptr<UnSolicitedMessage> clone();

  string dump();
};

} //namespace
#endif