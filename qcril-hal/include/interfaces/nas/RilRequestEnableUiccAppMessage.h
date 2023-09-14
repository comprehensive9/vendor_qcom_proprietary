/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to set mode preference Message
   @Receiver: NasModule
   */

class RilRequestEnableUiccAppMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestEnableUiccAppMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_ENABLE_UICC_APPLICATIONS";

    RilRequestEnableUiccAppMessage() = delete;
    ~RilRequestEnableUiccAppMessage() {}

    inline RilRequestEnableUiccAppMessage(std::shared_ptr<MessageContext> context,
        bool enable) : QcRilRequestMessage(get_class_message_id(), context), mEnable(enable) {
      mName = MESSAGE_NAME;
    }
    inline bool getEnableUiccApp() { return mEnable; }

private:
    bool mEnable;
};
