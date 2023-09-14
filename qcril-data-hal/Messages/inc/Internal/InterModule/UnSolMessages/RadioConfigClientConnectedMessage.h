/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RADIOCONFIGCLIENTCONNECTEDMESSAGE
#define RADIOCONFIGCLIENTCONNECTEDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class RadioConfigClientConnectedMessage: public UnSolicitedMessage,
                                         public add_message_id<RadioConfigClientConnectedMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "RadioConfigClientConnectedMessage";

  RadioConfigClientConnectedMessage();
  ~RadioConfigClientConnectedMessage();

  std::shared_ptr<UnSolicitedMessage> clone();
  string dump();

  #ifdef QMI_RIL_UTF
  bool mRilType = TRUE;
  void setRilType(bool val) {mRilType = val;}
  bool getRilType() {return mRilType;}
  #endif
};

} //namespace

#endif