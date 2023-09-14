/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to query NR config message
   @Receiver: NasModule
   */

class RilRequestQueryNrConfigMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestQueryNrConfigMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_QUERY_NR_CONFIG";

    RilRequestQueryNrConfigMessage() = delete;
    ~RilRequestQueryNrConfigMessage() {}

    inline RilRequestQueryNrConfigMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
};
