/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to query available network message
   @Receiver: NasModule
   */

class RilRequestGetBarringInfoMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestGetBarringInfoMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RilRequestGetBarringInfoMessage";

    RilRequestGetBarringInfoMessage() = delete;
    ~RilRequestGetBarringInfoMessage() {}

    inline RilRequestGetBarringInfoMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
};
