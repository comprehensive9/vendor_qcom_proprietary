/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include "framework/legacy.h"
#include "modules/android/ril_request_info.h"
#include "modules/sms/qcril_qmi_sms_types.h"
#include <interfaces/QcRilRequestMessage.h>

/* Request to get IMS registration Message
   @Receiver: SmsModule

   Response:
    errorCode    : Valid error code
    responseData : std::shared_ptr<RilGetImsRegistrationResult_t>
*/
class RilRequestGetImsRegistrationMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestGetImsRegistrationMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_IMS_REGISTRATION_STATE";

  ~RilRequestGetImsRegistrationMessage();

  inline RilRequestGetImsRegistrationMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  string dump();
};
