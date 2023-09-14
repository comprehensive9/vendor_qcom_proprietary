/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef STARTLCEREQUESTMESSAGE
#define STARTLCEREQUESTMESSAGE
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"
#include "telephony/ril.h"


namespace rildata {

class StartLCERequestMessage : public SolicitedMessage<RIL_LceStatusInfo>,
                              public add_message_id<StartLCERequestMessage>
{
private:
  int32_t mSerial;
  std::shared_ptr<std::function<void(uint16_t)>> mAcknowlegeRequestCb;
public:
  int32_t mInterval;
  int32_t mMode;
  static constexpr const char* MESSAGE_NAME = "StartLCERequestMessage";
  StartLCERequestMessage( const int32_t serial,int32_t interval,int32_t mode,
        const std::shared_ptr<std::function<void(uint16_t)>> ackCb
  ):SolicitedMessage<RIL_LceStatusInfo>(get_class_message_id()), mInterval(interval), mMode(mode) {
    mName = MESSAGE_NAME;
    mSerial = serial;
    mAcknowlegeRequestCb = ackCb;
  }
  ~StartLCERequestMessage() = default;
  string dump(){
    return mName;
  }
  int32_t getSerial() {
    return mSerial;
  }
  int32_t getLCEinterval() {
    return mInterval;
  }
  int32_t getLCEmode() {
    return mMode;
  }
  std::shared_ptr<std::function<void(uint16_t)>> getAcknowlegeRequestCb() {
    return mAcknowlegeRequestCb;
  }
};
}

#endif