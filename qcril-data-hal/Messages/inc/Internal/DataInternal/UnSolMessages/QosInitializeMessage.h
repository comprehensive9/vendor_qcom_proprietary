/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QOS_INIT_MESSAGE
#define QOS_INIT_MESSAGE

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

namespace rildata {

/**
  * DataInitMessage: Sent by the DATA module.
  * Indicates that DATA module is ready to receive requests.
  *
  */

class QosInitializeMessage: public UnSolicitedMessage, public add_message_id<QosInitializeMessage>
{
private:
  int mCid;
  int mMuxId;
  int mEptype;
  int mEpId;
  bool mCloseQos;

public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_QOS_INIT";

  QosInitializeMessage(int cid,  int muxId,  int eptype, int epid, bool closeQos = false):
          UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mCloseQos = closeQos;
    mCid = cid;
    mMuxId = muxId;
    mEptype = eptype;
    mEpId = epid;
  }
  ~QosInitializeMessage() {};

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<QosInitializeMessage>(
        getCid(), getMuxId(), getEpType(), getEpId() );
    return msg;
  }
  string dump() {
    return std::string(MESSAGE_NAME);
  }
  int getCid() {
    return mCid;
  }
  int getMuxId() {
    return mMuxId;
  }
  int getEpType() {
    return mEptype;
  }
  int getEpId() {
    return mEpId;
  }
  bool isCloseQos() {
    return mCloseQos;
  }
};

} //namespace rildata

#endif //QOS_INIT_MESSAGE
