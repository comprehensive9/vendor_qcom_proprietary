/**
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#ifndef GETDATANRICONTYPEMESSAGE
#define GETDATANRICONTYPEMESSAGE

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

class GetDataNrIconTypeMessage : public SolicitedMessage<NrIconType_t>,
                           public add_message_id<GetDataNrIconTypeMessage> {

  private:
    int32_t mSerial;
    std::shared_ptr<std::function<void(uint16_t)>> mAcknowlegeRequestCb;
  public:
    static constexpr const char *MESSAGE_NAME = "GetDataNrIconTypeMessage";
    GetDataNrIconTypeMessage():SolicitedMessage<NrIconType_t>(get_class_message_id()) {
      mName = MESSAGE_NAME;
    }

    GetDataNrIconTypeMessage( const int32_t serial,
        const std::shared_ptr<std::function<void(uint16_t)>> ackCb
    ):SolicitedMessage<NrIconType_t>(get_class_message_id()) {
      mName = MESSAGE_NAME;
      mSerial = serial;
      mAcknowlegeRequestCb = ackCb;
    }

    ~GetDataNrIconTypeMessage() {
      if (mCallback) {
        delete mCallback;
        mCallback = nullptr;
      }
    }

    int32_t getSerial() {
      return mSerial;
    }

    std::shared_ptr<std::function<void(uint16_t)>> getAcknowlegeRequestCb() {
      return mAcknowlegeRequestCb;
    }

    string dump() {
        return mName + "{ }";
    }
};
}
#endif
