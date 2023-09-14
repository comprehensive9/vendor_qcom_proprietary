/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

struct PLMN_t {
  uint16_t mcc;
  uint16_t mnc;
  std::string mMcc;
  std::string mMnc;
};

/********************** Class Definitions *************************/
class DataRegistrationFailureCauseMessage: public UnSolicitedMessage,
                                    public add_message_id<DataRegistrationFailureCauseMessage> {
private:
  PLMN_t mPlmnInfo;
  PLMN_t mPrimaryPlmnInfo;
  int32_t mRejectCause;
  bool mRejectCauseValid;

public:
  static constexpr const char *MESSAGE_NAME = "DataRegistrationFailureCauseMessage";

  DataRegistrationFailureCauseMessage() = delete;
  ~DataRegistrationFailureCauseMessage();
  DataRegistrationFailureCauseMessage(PLMN_t plmn, PLMN_t primaryPlmn, int32_t rejectCause, bool rejectCauseValid);

  std::shared_ptr<UnSolicitedMessage> clone();

  PLMN_t getPLMNInfo();
  PLMN_t getPrimaryPLMNInfo();
  int32_t getRejectCause();
  bool isRejectCauseValid();
  string dump();
};

class RegistrationFailureReportingStatusMessage : public UnSolicitedMessage,
                                                  public add_message_id<RegistrationFailureReportingStatusMessage>
{
private:
  bool mReportingRegistrationFailure;

public:
  static constexpr const char *MESSAGE_NAME = "RegistrationFailureReportingStatusMessage";
  RegistrationFailureReportingStatusMessage() = delete;
  ~RegistrationFailureReportingStatusMessage() {};
  RegistrationFailureReportingStatusMessage(bool enable);

  std::shared_ptr<UnSolicitedMessage> clone();

  bool getReportingStatus();
  string dump();
};

} // namespace
