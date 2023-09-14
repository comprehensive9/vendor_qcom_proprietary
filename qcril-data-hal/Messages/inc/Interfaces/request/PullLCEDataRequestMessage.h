/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef PULLLCEDATAREQUESTMESSAGE
#define PULLLCEDATAREQUESTMESSAGE
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"
#include "telephony/ril.h"


namespace rildata {

class PullLCEDataRequestMessage : public SolicitedMessage<RIL_LceDataInfo>,
                              public add_message_id<PullLCEDataRequestMessage>
{
private:
  int32_t mSerial;
  std::shared_ptr<std::function<void(uint16_t)>> mAcknowlegeRequestCb;
public:
  static constexpr const char* MESSAGE_NAME = "PullLCEDataRequestMessage";
  PullLCEDataRequestMessage(
        const int32_t serial,
        const std::shared_ptr<std::function<void(uint16_t)>> ackCb
        ):SolicitedMessage<RIL_LceDataInfo>(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mSerial = serial;
    mAcknowlegeRequestCb = ackCb;
  }
  ~PullLCEDataRequestMessage() = default;
  string dump() {
    return mName;
  }
  int32_t getSerial() {
    return mSerial;
  }
  std::shared_ptr<std::function<void(uint16_t)>> getAcknowlegeRequestCb() {
    return mAcknowlegeRequestCb;
  }
};

}


#endif