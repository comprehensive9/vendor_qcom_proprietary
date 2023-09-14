/******************************************************************************
#  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef SETINITIALATTACHAPNREQUESTMESSAGE
#define SETINITIALATTACHAPNREQUESTMESSAGE
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"
#include "telephony/ril.h"

using namespace std;

namespace rildata {
class SetInitialAttachApnRequestMessage : public SolicitedMessage<RIL_Errno>,
                              public add_message_id<SetInitialAttachApnRequestMessage>
{
private:
    int32_t mSerial;
    shared_ptr<function<void(uint16_t)>> mAcknowlegeRequestCb;

    public:
    DataProfileInfo_t mProfile;
    static constexpr const char* MESSAGE_NAME = "SetInitialAttachApnRequestMessage";
    SetInitialAttachApnRequestMessage() = delete;
    SetInitialAttachApnRequestMessage(
        const int32_t serial,
        const DataProfileInfo_t profile,
        const shared_ptr<function<void(uint16_t)>> ackCb
        ):SolicitedMessage<RIL_Errno>(get_class_message_id()) {

      mName = MESSAGE_NAME;
      mSerial = serial;
      mProfile = profile;
      mAcknowlegeRequestCb = ackCb;
    }
    ~SetInitialAttachApnRequestMessage() = default;
    string dump() {
      std::stringstream ss;
      ss << mName << " Serial=[" << (int)getSerial() << "] ";
      mProfile.dump("", ss);
      return ss.str();
    }
    DataProfileInfo_t getProfile() {return mProfile;}
    int32_t getSerial() {return mSerial;}
};
}


#endif
