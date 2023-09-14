/*===========================================================================

  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef QOSDATACHANGEDINDMESSAGE
#define QOSDATACHANGEDINDMESSAGE

#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

class QosDataChangedIndMessage : public UnSolicitedMessage,
                                 public add_message_id<QosDataChangedIndMessage> {
  private:
    QosParamResult_t mQosResult;

  public:
    static constexpr const char *MESSAGE_NAME = "QosDataChangedIndMessage";
    QosDataChangedIndMessage() = delete;
    QosDataChangedIndMessage(QosParamResult_t qosResult ):
       UnSolicitedMessage(get_class_message_id()), mQosResult(qosResult) {}
    ~QosDataChangedIndMessage() = default;

    string dump() {
      std::stringstream ss;
      ss << MESSAGE_NAME << " {";
      mQosResult.dump("", ss);
      ss << "}";
      return ss.str();
    };

    QosParamResult_t getQosResult() {return mQosResult;}

    std::shared_ptr<UnSolicitedMessage> clone() {
      return std::make_shared<QosDataChangedIndMessage>(mQosResult);
    }

};

}
#endif
