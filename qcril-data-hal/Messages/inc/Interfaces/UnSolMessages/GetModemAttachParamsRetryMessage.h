/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

/********************** Class Definitions *************************/
class GetModemAttachParamsRetryMessage: public UnSolicitedMessage,
                           public add_message_id<GetModemAttachParamsRetryMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "GetModemAttachParamsRetryMessage";

  GetModemAttachParamsRetryMessage():UnSolicitedMessage(get_class_message_id()){}
  ~GetModemAttachParamsRetryMessage() {}

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::make_shared<GetModemAttachParamsRetryMessage>();
  }

  string dump()
  {
    return MESSAGE_NAME;
  }
};
