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
class RilUnsolUiccAppsStatusChangedMessage : public UnSolicitedMessage,
            public add_message_id<RilUnsolUiccAppsStatusChangedMessage> {
 public:
  static constexpr const char *MESSAGE_NAME = "RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED";
  ~RilUnsolUiccAppsStatusChangedMessage() { }

  RilUnsolUiccAppsStatusChangedMessage(bool status)
      : UnSolicitedMessage(get_class_message_id()), mStatus(status) {}

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolUiccAppsStatusChangedMessage>(mStatus);
  }

  bool getStatus() { return mStatus; }

  string dump() {
    return RilUnsolUiccAppsStatusChangedMessage::MESSAGE_NAME;
  }
 private:
  bool mStatus;
};
