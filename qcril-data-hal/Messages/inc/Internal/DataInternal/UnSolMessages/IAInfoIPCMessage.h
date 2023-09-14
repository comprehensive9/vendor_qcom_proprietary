/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "framework/Message.h"
#include "qtibus/IPCMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

enum class IAInfoIPCMessageState : int {
    IACompleted = 0,
    IAStartRequest,
    IAStartResponse,
};

/********************** Class Definitions *************************/
class IAInfoIPCMessage: public IPCMessage,
                           public add_message_id<IAInfoIPCMessage> {

private:
  IAInfoIPCMessageState state;

public:
  static constexpr const char *MESSAGE_NAME = "IAInfoIPCMessage";

  IAInfoIPCMessage(IAInfoIPCMessageState setState);
  ~IAInfoIPCMessage();

  void serialize(IPCOStream &os) override;
  void deserialize(IPCIStream &is) override;

  IAInfoIPCMessageState getState();

  std::shared_ptr<UnSolicitedMessage> clone();
  string dump();
};

} //namespace
