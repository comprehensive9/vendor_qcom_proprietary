/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>
#include <interfaces/voice/voice.h>
#include <optional>

/*
 * Send a Sip Info message
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 */
class QcRilRequestSendSipDtmfMessage : public QcRilRequestMessage,
                                    public add_message_id<QcRilRequestSendSipDtmfMessage> {
 private:
  std::optional<std::string> mSipInfo;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilRequestSendSipDtmfMessage";

  QcRilRequestSendSipDtmfMessage() = delete;

  ~QcRilRequestSendSipDtmfMessage() {
  }

  inline QcRilRequestSendSipDtmfMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  bool hasSipInfo() {
    return mSipInfo ? true : false;
  }
  const std::string& getSipInfo() {
    return *mSipInfo;
  }
  void setSipInfo(const std::string& val) {
    mSipInfo = val;
  }

  virtual std::string dump() {
    std::string os;
    os += QcRilRequestMessage::dump();
    os += "{";
    os += ".mSipInfo=" + (mSipInfo ? (*mSipInfo) : "<invalid>");
    os += "}";
    return os;
  }
};
