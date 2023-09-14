/*===========================================================================

  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef GETDATACALLLISTRESPONSEINDMESSAGE
#define GETDATACALLLISTRESPONSEINDMESSAGE

#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

class GetDataCallListResponseIndMessage : public UnSolicitedMessage,
                                          public add_message_id<GetDataCallListResponseIndMessage> {
  private:
    std::shared_ptr<DataCallListResult_t> mDCList;
    int32_t mSerial;
    Message::Callback::Status mStatus;

  public:
    static constexpr const char *MESSAGE_NAME = "GetDataCallListResponseIndMessage";
    GetDataCallListResponseIndMessage() = delete;
    GetDataCallListResponseIndMessage(std::shared_ptr<DataCallListResult_t> dcList,
                                         int32_t serial, Message::Callback::Status status):
       UnSolicitedMessage(get_class_message_id()), mDCList(dcList), mSerial(serial), mStatus(status) {}
    ~GetDataCallListResponseIndMessage() = default;

    string dump() {
      std::stringstream ss;
      ss << MESSAGE_NAME << " {";
      for (const DataCallResult_t& call : mDCList->call) {
        call.dump("", ss);
        ss << ",";
      }
      ss << "}";
      return ss.str();
    };

    std::shared_ptr<DataCallListResult_t> getDCList() {return mDCList;}

    std::shared_ptr<UnSolicitedMessage> clone() {
      return std::make_shared<GetDataCallListResponseIndMessage>(mDCList, mSerial, mStatus);
    }

    int32_t getSerial() {return mSerial;}

    Message::Callback::Status getStatus() {return mStatus;}

};

}
#endif
