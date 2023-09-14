/******************************************************************************
#  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>
#include <interfaces/voice/voice.h>
#include <optional>

/*
 * Send a USSD message
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 */
class QcRilRequestSendUssdMessage : public QcRilRequestMessage,
                                    public add_message_id<QcRilRequestSendUssdMessage> {
 private:
  bool mIsDomainAuto = false;
  std::optional<std::string> mUssd;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilRequestSendUssdMessage";

  QcRilRequestSendUssdMessage() = delete;

  ~QcRilRequestSendUssdMessage() {
  }

  inline QcRilRequestSendUssdMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  /*
   * For IMS requests, this return true.
   * For CS requests, this returns false if USSD over IMS is
   * supported else returns true inline with legacy implementation.
   */
  bool isDomainAuto() {
    return mIsDomainAuto;
  }
  void setIsDomainAuto(bool val) {
    mIsDomainAuto = val;
  }

  bool hasUssd() {
    return mUssd ? true : false;
  }
  const std::string& getUssd() {
    return *mUssd;
  }
  void setUssd(const std::string& val) {
    mUssd = val;
  }

  virtual std::string dump() {
    std::string os;
    os += QcRilRequestMessage::dump();
    os += "{";
    os += ".mUssd=" + (mUssd ? (*mUssd) : "<invalid>");
    os += std::string(".mIsDomainAuto=") + (mIsDomainAuto ? "true" : "false");
    os += "}";
    return os;
  }
};
