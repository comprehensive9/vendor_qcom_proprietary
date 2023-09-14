/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/nas/nas_types.h>

/*
 * Unsol message to report network scan result
 *
 */
class RilUnsolCellBarringMessage : public UnSolicitedMessage,
            public add_message_id<RilUnsolCellBarringMessage> {
 private:
  qcril::interfaces::RilCellBarringInfo_t mCellBarringInfo;

 public:
  static constexpr const char *MESSAGE_NAME = "RilUnsolCellBarringMessage";
  ~RilUnsolCellBarringMessage() { }


  template<typename T>
  RilUnsolCellBarringMessage(T info)
      : UnSolicitedMessage(get_class_message_id()),
        mCellBarringInfo(std::forward<T>(info)) { }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolCellBarringMessage>( mCellBarringInfo);
  }

  const qcril::interfaces::RilCellBarringInfo_t &getCellBarringInfo() { return mCellBarringInfo; }

  string dump() {
    return RilUnsolCellBarringMessage::MESSAGE_NAME;
  }

};
