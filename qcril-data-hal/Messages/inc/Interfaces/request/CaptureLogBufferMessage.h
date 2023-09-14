/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef CAPTURELOGBUFFERMESSAGE
#define CAPTURELOGBUFFERMESSAGE

#include "MessageCommon.h"

namespace rildata {

class CaptureLogBufferMessage : public SolicitedMessage<RIL_Errno>,
                          public add_message_id<CaptureLogBufferMessage> {
  public:

    static constexpr const char *MESSAGE_NAME = "CaptureLogBufferMessage";

    inline CaptureLogBufferMessage():
      SolicitedMessage<RIL_Errno>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
    }
    ~CaptureLogBufferMessage() = default;

    string dump(){return mName;}
};

}

#endif