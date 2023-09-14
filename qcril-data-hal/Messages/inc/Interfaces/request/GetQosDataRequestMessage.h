/*===========================================================================
 Copyright (c) 2022 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*============================================================================*/
#ifndef GETQOSDATAREQUESTMESSAGE
#define GETQOSDATAREQUESTMESSAGE

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

class GetQosDataRequestMessage : public SolicitedMessage<QosParamResult_t>,
                           public add_message_id<GetQosDataRequestMessage> {

  private:
    int32_t mSerial;
    int32_t mCid;

  public:
    static constexpr const char *MESSAGE_NAME = "GetQosDataRequestMessage";
    GetQosDataRequestMessage():SolicitedMessage<QosParamResult_t>(get_class_message_id()) {
      mName = MESSAGE_NAME;
    }

    GetQosDataRequestMessage( const int32_t serial, const int32_t cid)
        :SolicitedMessage<QosParamResult_t>(get_class_message_id()) {
      mName = MESSAGE_NAME;
      mSerial = serial;
      mCid = cid;
    }

    ~GetQosDataRequestMessage() {
      if (mCallback) {
        delete mCallback;
        mCallback = nullptr;
      }
    }

    int32_t getSerial() {
      return mSerial;
    }

    int32_t getCid() {
      return mCid;
    }

    string dump() {
        return mName + "[serial]:" + std::to_string(mSerial) + " [cid]:" + std::to_string(mCid);
    }
};
}
#endif
