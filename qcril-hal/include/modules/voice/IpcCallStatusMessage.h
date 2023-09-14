/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <qtibus/IPCMessage.h>

class IpcCallStatusMessage : public IPCMessage, public add_message_id<IpcCallStatusMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "IpcCallStatusMessage";

 private:
  uint8_t mInstanceId;
  uint8_t mTotalNumberOfCalls = 0;

 public:
  IpcCallStatusMessage()
      : IPCMessage(get_class_message_id()), mInstanceId(QCRIL_DEFAULT_INSTANCE_ID) {
    mName = MESSAGE_NAME;
  }

  IpcCallStatusMessage(uint8_t instanceId)
      : IPCMessage(get_class_message_id()), mInstanceId(instanceId) {
    mName = MESSAGE_NAME;
  }

  inline ~IpcCallStatusMessage() {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto ret = std::make_shared<IpcCallStatusMessage>(mInstanceId);
    if (ret) {
      ret->mTotalNumberOfCalls = mTotalNumberOfCalls;
      ret->setIsRemote(getIsRemote());
    }
    return ret;
  }

  void serialize(IPCOStream& os) {
    os << mInstanceId;
    os << mTotalNumberOfCalls;
  }

  void deserialize(IPCIStream& is) {
    is >> mInstanceId;
    is >> mTotalNumberOfCalls;
  }

  void setTotalNumberOfCalls(uint8_t numCalls) {
    mTotalNumberOfCalls = numCalls;
  }

  uint8_t getTotalNumberOfCalls() {
    return mTotalNumberOfCalls;
  }

  string dump() {
    std::string os = mName;
    os += "{";
    os += " .isRemote=";
    os += (getIsRemote() ? "true" : "false");
    os += " .mInstanceId=" + std::to_string(mInstanceId);
    os += " .mTotalNumberOfCalls=" + std::to_string(mTotalNumberOfCalls);
    os += "}";
    return os;
  }
};
