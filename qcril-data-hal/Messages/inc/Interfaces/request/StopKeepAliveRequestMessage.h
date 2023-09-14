/******************************************************************************
#  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef STOPKEEPALIVEREQUESTMESSAGE
#define STOPKEEPALIVEREQUESTMESSAGE

#include "MessageCommon.h"

using namespace std;

namespace rildata {

class StopKeepAliveRequestMessage : public SolicitedMessage<ResponseError_t>,
                          public add_message_id<StopKeepAliveRequestMessage> {
  private:
    int32_t mSessionHandle;
    int32_t mSerial;
    std::shared_ptr<std::function<void(uint16_t)>> mAcknowlegeRequestCb;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.data.StopKeepAliveRequestMessage";
    StopKeepAliveRequestMessage() = delete;
    StopKeepAliveRequestMessage(
      const int32_t serial,
      const int32_t sessionHandle):SolicitedMessage<ResponseError_t>(get_class_message_id()) {

      mName = MESSAGE_NAME;
      mSerial = serial;
      mSessionHandle = sessionHandle;

    }
    StopKeepAliveRequestMessage(
      const int32_t serial,
      const int32_t sessionHandle,
      const std::shared_ptr<std::function<void(uint16_t)>> ackCb
      ):SolicitedMessage<ResponseError_t>(get_class_message_id()) {
        mName = MESSAGE_NAME;
        mSerial = serial;
        mAcknowlegeRequestCb = ackCb;
        mSessionHandle = sessionHandle;
    }
    ~StopKeepAliveRequestMessage() = default;

    string dump(){return mName;}
    int32_t getSerial() {return mSerial;}
    int32_t* getHandle() {return &mSessionHandle;}
    std::shared_ptr<std::function<void(uint16_t)>> getAcknowlegeRequestCb() {
        return mAcknowlegeRequestCb;
    }
};

}

#endif