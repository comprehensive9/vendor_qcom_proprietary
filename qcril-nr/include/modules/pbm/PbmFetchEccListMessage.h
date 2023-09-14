/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <interfaces/QcRilRequestMessage.h>
#include "framework/add_message_id.h"

class PbmFetchEccListMessage : public SolicitedMessage<RIL_Errno>,
  public add_message_id<PbmFetchEccListMessage> {

  public:
    static constexpr const char *MESSAGE_NAME = "PbmFetchEccListMessage";

    PbmFetchEccListMessage():
        SolicitedMessage<RIL_Errno>(get_class_message_id())
    {
        mName = MESSAGE_NAME;
    }

    ~PbmFetchEccListMessage() {}

    string dump() {
      return mName;
    }
};
