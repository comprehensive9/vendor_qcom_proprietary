/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/voice/voice.h>
#include <optional>

/*
 * Unsol message to notify SRVCC or RAT handover changes
 */
class QcRilUnsolImsHandoverMessage : public UnSolicitedMessage,
                                       public add_message_id<QcRilUnsolImsHandoverMessage> {
 private:
  std::optional<qcril::interfaces::HandoverType> mHandoverType;
  std::optional<RIL_RadioTechnology> mSourceTech;
  std::optional<RIL_RadioTechnology> mTargetTech;
  std::optional<std::string> mCauseCode;
  std::optional<std::string> mErrorCode; // Derived from mCauseCode
  std::optional<std::string> mErrorMsg;  // Derived from mCauseCode

  void deriveErrorDetails() {
      if (hasCauseCode() && !getCauseCode().empty()) {
        /* Error is reported when the handover is NOT_TRIGGERED while the device is on active
         * Wifi call and the wifi Rssi is nearing threshold roveout (-85dbm) and there is
         * no qualified LTE network to handover to. Modem sends "CD-04:No Available qualified
         * mobile network". Here it is decoded and sent as errorcode(CD-04) and errormessage
         * to telephony.
         */
        if (hasHandoverType() &&
            (getHandoverType() == qcril::interfaces::HandoverType::NOT_TRIGGERED) &&
            (getCauseCode().find(
                QcRilUnsolImsHandoverMessage::WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING) == 0)) {
          mErrorCode =
                  QcRilUnsolImsHandoverMessage::WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING;

          std::string causeCode = getCauseCode();
          std::string delim = ":";
          size_t pos = causeCode.find(delim);
          if (pos != std::string::npos) {
            std::string errorCode = causeCode.substr(0, pos);
            std::string errorMsg = causeCode.substr(pos + delim.length());
            //remove whitespaces at the beginning
            if (!errorMsg.empty()) {
              size_t start = errorMsg.find_first_not_of(" ");
              errorMsg = (start != std::string::npos) ? errorMsg.substr(start) : "";
              mErrorMsg = errorMsg;
            }
          }
        }
      }
  }

 public:
  static constexpr const char *MESSAGE_NAME = "QcRilUnsolImsHandoverMessage";
  inline static const std::string WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING = "CD-04";
  ~QcRilUnsolImsHandoverMessage() {}

  QcRilUnsolImsHandoverMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    std::shared_ptr<QcRilUnsolImsHandoverMessage> msg =
        std::make_shared<QcRilUnsolImsHandoverMessage>();
    if (msg) {
      if (mHandoverType) {
        msg->setHandoverType(*mHandoverType);
      }
      if (mSourceTech) {
        msg->setSourceTech(*mSourceTech);
      }
      if (mTargetTech) {
        msg->setTargetTech(*mTargetTech);
      }
      if (mCauseCode) {
        msg->setCauseCode(*mCauseCode);
      }
    }
    return msg;
  }

  bool hasHandoverType() {
    return mHandoverType ? true : false;
  }
  qcril::interfaces::HandoverType getHandoverType() {
    return *mHandoverType;
  }
  void setHandoverType(qcril::interfaces::HandoverType val) {
    mHandoverType = val;
    deriveErrorDetails();
  }

  bool hasSourceTech() {
    return mSourceTech ? true : false;
  }
  RIL_RadioTechnology getSourceTech() {
    return *mSourceTech;
  }
  void setSourceTech(RIL_RadioTechnology val) {
    mSourceTech = val;
  }

  bool hasTargetTech() {
    return mTargetTech ? true : false;
  }
  RIL_RadioTechnology getTargetTech() {
    return *mTargetTech;
  }
  void setTargetTech(RIL_RadioTechnology val) {
    mTargetTech = val;
  }

  bool hasCauseCode() {
    return mCauseCode ? true : false;
  }
  const std::string& getCauseCode() {
    return *mCauseCode;
  }
  void setCauseCode(const std::string& val) {
    mCauseCode = val;
    deriveErrorDetails();
  }

  inline bool hasErrorCode() {
    return (bool)mErrorCode;
  }
  inline std::string getErrorCode() {
    return mErrorCode ? *mErrorCode : "";
  }
  inline bool hasErrorMsg() {
    return (bool)mErrorMsg;
  }
  inline std::string getErrorMsg() {
    return mErrorMsg ? *mErrorMsg : "";
  }

  virtual std::string dump() {
    std::string os;
    os += mName;
    os += "{";
    os += ".mHandoverType=" + (mHandoverType ? toString(*mHandoverType) : "<invalid>");
    os += ".mSourceTech=" + (mSourceTech ? std::to_string(*mSourceTech) : "<invalid>");
    os += ".mTargetTech=" + (mTargetTech ? std::to_string(*mTargetTech) : "<invalid>");
    os += ".mCauseCode=" + (mCauseCode ? *mCauseCode : "<invalid>");
    os += "}";
    return os;
  }
};
