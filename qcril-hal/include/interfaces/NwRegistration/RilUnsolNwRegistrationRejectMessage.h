/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once

#include "telephony/ril.h"
#include <interfaces/nas/nas_types.h>

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

using namespace qcril::interfaces;

class RilUnsolNwRegistrationRejectMessage: public UnSolicitedMessage,
                           public add_message_id<RilUnsolNwRegistrationRejectMessage> {

private:
  RilNwRegistrationRejectInfo_t mRegRejInfo;
public:
  static constexpr const char *MESSAGE_NAME = "RilUnsolNwRegistrationRejectMessage";

  RilUnsolNwRegistrationRejectMessage() = delete;
  ~RilUnsolNwRegistrationRejectMessage(){};
  RilUnsolNwRegistrationRejectMessage(RilNwRegistrationRejectInfo_t const &regRejInfo);

  std::shared_ptr<UnSolicitedMessage> clone();
  const RilNwRegistrationRejectInfo_t &getNwRegistrationRejectInfo();
  string dump();
};

inline RilUnsolNwRegistrationRejectMessage::RilUnsolNwRegistrationRejectMessage
(
  const RilNwRegistrationRejectInfo_t &regRejInfo
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  memcpy(&mRegRejInfo, &regRejInfo, sizeof(mRegRejInfo));
}

inline const RilNwRegistrationRejectInfo_t&
RilUnsolNwRegistrationRejectMessage::getNwRegistrationRejectInfo()
{
  return mRegRejInfo;
}

inline std::shared_ptr<UnSolicitedMessage> RilUnsolNwRegistrationRejectMessage::clone()
{
  return std::make_shared<RilUnsolNwRegistrationRejectMessage>(mRegRejInfo);
}

inline string RilUnsolNwRegistrationRejectMessage::dump()
{
  std::stringstream ss;
  ss << mName;
  ss << " : causeCode_valid = " << static_cast<int>(mRegRejInfo.causeCode_valid);
  ss << " : causeCode = " << mRegRejInfo.causeCode;
  ss << " : additionalCauseCode_valid = " << static_cast<int>(mRegRejInfo.additionalCauseCode_valid);
  ss << " : additionalCauseCode = " << mRegRejInfo.additionalCauseCode;
  ss << " : domain = " << mRegRejInfo.domain;
  ss << " : choosenPlmn = " << mRegRejInfo.choosenPlmn;
   return ss.str();

  return ss.str();
}
