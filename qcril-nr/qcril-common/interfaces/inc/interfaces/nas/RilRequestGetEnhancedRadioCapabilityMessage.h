/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to Get Radio Capability
   @Receiver: NasModule
   */

class RilRequestGetEnhancedRadioCapabilityMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestGetEnhancedRadioCapabilityMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_GET_QTI_RADIO_CAPABILITY";

    RilRequestGetEnhancedRadioCapabilityMessage() = delete;
    ~RilRequestGetEnhancedRadioCapabilityMessage() {}

    inline RilRequestGetEnhancedRadioCapabilityMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
};
