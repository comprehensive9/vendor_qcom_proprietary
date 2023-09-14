/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to set NR config message
   @Receiver: NasModule
   */

class RilRequestSetNrConfigMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestSetNrConfigMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_SET_NR_CONFIG";

    RilRequestSetNrConfigMessage() = delete;
    ~RilRequestSetNrConfigMessage() {}

    inline RilRequestSetNrConfigMessage(std::shared_ptr<MessageContext> context,
        RIL_NR_DISABLE_MODE config) : QcRilRequestMessage(get_class_message_id(), context), mConfig(config) {
      mName = MESSAGE_NAME;
    }
    inline RIL_NR_DISABLE_MODE getConfig() { return mConfig; }
private:
    RIL_NR_DISABLE_MODE mConfig;
};
