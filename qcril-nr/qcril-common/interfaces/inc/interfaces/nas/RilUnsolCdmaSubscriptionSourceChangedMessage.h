/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/*
 * Unsol message to notify voice rat change
 *
 */
class RilUnsolCdmaSubscriptionSourceChangedMessage : public UnSolicitedMessage,
            public add_message_id<RilUnsolCdmaSubscriptionSourceChangedMessage> {
 public:
  static constexpr const char *MESSAGE_NAME = "RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED";
  ~RilUnsolCdmaSubscriptionSourceChangedMessage() { }

  RilUnsolCdmaSubscriptionSourceChangedMessage(RIL_CdmaSubscriptionSource source)
      : UnSolicitedMessage(get_class_message_id()), mSource(source) {}

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolCdmaSubscriptionSourceChangedMessage>(mSource);
  }

  RIL_CdmaSubscriptionSource getSource() { return mSource; }

  string dump() {
    return RilUnsolCdmaSubscriptionSourceChangedMessage::MESSAGE_NAME;
  }
 private:
  RIL_CdmaSubscriptionSource mSource;
};
