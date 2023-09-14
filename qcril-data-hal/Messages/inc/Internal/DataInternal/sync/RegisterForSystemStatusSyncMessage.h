/******************************************************************************
#  Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef REGISTERFORSYSTEMSTATUSSYNCMESSAGE
#define REGISTERFORSYSTEMSTATUSSYNCMESSAGE
#include "framework/SolicitedSyncMessage.h"
#include "framework/Util.h"
#include "framework/add_message_id.h"
#include "framework/Dispatcher.h"

class RegisterForSystemStatusSyncMessage : public SolicitedSyncMessage<int>,
                                    public add_message_id<RegisterForSystemStatusSyncMessage>
{

public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.data.RegisterForSystemStatusSyncMessage";
  inline RegisterForSystemStatusSyncMessage(GenericCallback<int> *callback) :
    SolicitedSyncMessage<int>(get_class_message_id())
  {
    (void)callback;
    mName = MESSAGE_NAME;
  }
  ~RegisterForSystemStatusSyncMessage() {}

  string dump() {
    return mName;
  }
};

#endif