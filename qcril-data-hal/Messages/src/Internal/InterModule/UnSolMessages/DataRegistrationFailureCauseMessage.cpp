/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "UnSolMessages/DataRegistrationFailureCauseMessage.h"

using namespace rildata;

DataRegistrationFailureCauseMessage::DataRegistrationFailureCauseMessage
(
  PLMN_t plmn, PLMN_t primaryPlmn, int32_t rejectCause, bool rejectCauseValid
) : UnSolicitedMessage(get_class_message_id()), mPlmnInfo(plmn), mPrimaryPlmnInfo(primaryPlmn), mRejectCause(rejectCause), mRejectCauseValid(rejectCauseValid)
{
  mName = MESSAGE_NAME;
}

DataRegistrationFailureCauseMessage::~DataRegistrationFailureCauseMessage()
{
}

std::shared_ptr<UnSolicitedMessage> DataRegistrationFailureCauseMessage::clone()
{
  return std::make_shared<DataRegistrationFailureCauseMessage>(mPlmnInfo, mPrimaryPlmnInfo, mRejectCause, mRejectCauseValid);
}

PLMN_t DataRegistrationFailureCauseMessage::getPLMNInfo()
{
  return mPlmnInfo;
}

PLMN_t DataRegistrationFailureCauseMessage::getPrimaryPLMNInfo()
{
  return mPrimaryPlmnInfo;
}

int32_t DataRegistrationFailureCauseMessage::getRejectCause()
{
  return mRejectCause;
}

bool DataRegistrationFailureCauseMessage::isRejectCauseValid()
{
  return mRejectCauseValid;
}

string DataRegistrationFailureCauseMessage::dump()
{
  std::stringstream ss;
  ss << MESSAGE_NAME;
  ss << " : mcc = " << mPlmnInfo.mMcc;
  ss << " : mnc = " << mPlmnInfo.mMnc;
  ss << " : primary mcc = " << mPrimaryPlmnInfo.mMcc;
  ss << " : primary mnc = " << mPrimaryPlmnInfo.mMnc;
  ss << " : reject cause = " << mRejectCause;
  ss << " : reject valid = " << mRejectCauseValid;
  return ss.str();
}

RegistrationFailureReportingStatusMessage::RegistrationFailureReportingStatusMessage
(
  bool enable
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mReportingRegistrationFailure = enable;
}

std::shared_ptr<UnSolicitedMessage> RegistrationFailureReportingStatusMessage::clone()
{
  return std::make_shared<RegistrationFailureReportingStatusMessage>(mReportingRegistrationFailure);
}

bool RegistrationFailureReportingStatusMessage::getReportingStatus()
{
  return mReportingRegistrationFailure;
}

string RegistrationFailureReportingStatusMessage::dump() {
  std::stringstream ss;
  ss << MESSAGE_NAME;
  ss << " : report = " << mReportingRegistrationFailure;
  return ss.str();
}
