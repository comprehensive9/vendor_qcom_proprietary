/**
* Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef SETLINKCAPFILTERMESSAGE
#define SETLINKCAPFILTERMESSAGE
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "framework/GenericCallback.h"
#include "MessageCommon.h"

namespace rildata {

enum ReportFilter_t {
  disabled,
  enabled
};

/**
 * @brief      Support for the Link Capacity enstimation feature.
 *             This class is used to enable/disable reporting.
 */
class SetLinkCapFilterMessage : public SolicitedMessage<int>,
                                public add_message_id<SetLinkCapFilterMessage> {
private:
  int32_t mSerial;
  std::shared_ptr<std::function<void(uint16_t)>> mAcknowlegeRequestCb;
  RIL_Token t;
  ReportFilter_t params;

public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.data.RilSetLinkCapFilter";
  SetLinkCapFilterMessage(
    const int32_t serial,
    const ReportFilter_t &r,
    const std::shared_ptr<std::function<void(uint16_t)>> ackCb
  ):SolicitedMessage<int>(get_class_message_id()),params(r) {
    mName = MESSAGE_NAME;
    mSerial = serial;
    mAcknowlegeRequestCb = ackCb;
  }
  inline SetLinkCapFilterMessage(RIL_Token tok, const ReportFilter_t &r):SolicitedMessage<int>(get_class_message_id()),
        t(tok), params(r) {
    mName = MESSAGE_NAME;
  }
  ~SetLinkCapFilterMessage() {
    if (mCallback) {
      delete mCallback;
      mCallback = nullptr;
    }
  }
  string dump() {
    return "[" + mName + "]";
  }
  ReportFilter_t &getParams() {
    return params;
  }
  int32_t getSerial() {
    return mSerial;
  }
  std::shared_ptr<std::function<void(uint16_t)>> getAcknowlegeRequestCb() {
    return mAcknowlegeRequestCb;
  }
  RIL_Token getToken() {
    return t;
  }
};
} //namespace

#endif