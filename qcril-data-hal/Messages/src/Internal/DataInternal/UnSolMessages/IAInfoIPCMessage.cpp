/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "UnSolMessages/IAInfoIPCMessage.h"

using namespace rildata;

IAInfoIPCMessage::IAInfoIPCMessage(IAInfoIPCMessageState setState):
  IPCMessage(get_class_message_id()), state(setState)
{}

IAInfoIPCMessage::~IAInfoIPCMessage() {}

void IAInfoIPCMessage::serialize(IPCOStream &os)
{
  os << static_cast<int>(state);
}

void IAInfoIPCMessage::deserialize(IPCIStream &is)
{
  std::stringstream &iss = static_cast<std::stringstream&>(is);
  Log::getInstance().d("IAInfoIPCMessage deserialize iss=[" + iss.str() + "]");
  int inState;
  iss >> inState;
  state = static_cast<IAInfoIPCMessageState>(inState);
}

IAInfoIPCMessageState IAInfoIPCMessage::getState()
{
  return state;
}

std::shared_ptr<UnSolicitedMessage> IAInfoIPCMessage::clone()
{
  auto copy = std::make_shared<IAInfoIPCMessage>(state);
  if (copy != nullptr) {
    copy->setIsRemote(getIsRemote());
  }
  return copy;
}

string IAInfoIPCMessage::dump()
{
  return std::string(MESSAGE_NAME) + " isRemote: " +
         std::to_string(getIsRemote());
}
