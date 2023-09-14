/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

//#include "qcril_qmi_nas.h"
#include <interfaces/nas/nas_types.h>

using namespace qcril::interfaces;

class NasRegRejUnsolMessage : public UnSolicitedMessage,
                              public add_message_id<NasRegRejUnsolMessage>
{
  private:
  RilNwRegistrationRejectInfo_t mRegInfo;

  public:
    static constexpr const char *MESSAGE_NAME =
        "com.qualcomm.qti.qcril.nas.registrationinfounsol";
    NasRegRejUnsolMessage() = delete;
    ~NasRegRejUnsolMessage() = default;

    inline NasRegRejUnsolMessage(RilNwRegistrationRejectInfo_t &regInfo):
                       UnSolicitedMessage(get_class_message_id())
    {
      memcpy(&mRegInfo, &regInfo, sizeof(RilNwRegistrationRejectInfo_t));
      mName = MESSAGE_NAME;
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::static_pointer_cast<UnSolicitedMessage>(
              std::make_shared<NasRegRejUnsolMessage>(mRegInfo));
    }

    inline string dump()
    {
      std::stringstream ss;
      ss << MESSAGE_NAME;
      ss << " : causeCode_valid = " << static_cast<int>(mRegInfo.causeCode_valid);
      ss << " : causeCode = " << mRegInfo.causeCode;
      ss << " : additionalCauseCode_valid = " << static_cast<int>(mRegInfo.additionalCauseCode_valid);
      ss << " : additionalCauseCode = " << mRegInfo.additionalCauseCode;
      ss << " : domain = " << mRegInfo.domain;
      ss << " : choosenPlmn = " << mRegInfo.choosenPlmn;
      return ss.str();
    }

    inline RilNwRegistrationRejectInfo_t &getNasRegistrationInfo(void)
    {
      return mRegInfo;
    }
};
