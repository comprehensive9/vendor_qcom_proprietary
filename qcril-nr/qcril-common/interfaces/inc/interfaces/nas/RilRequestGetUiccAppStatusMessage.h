/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to send a set system selection channels message.
   @Receiver: NasModule
*/

class RilRequestGetUiccAppStatusMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestGetUiccAppStatusMessage> {
public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT";

    RilRequestGetUiccAppStatusMessage() = delete;
    ~RilRequestGetUiccAppStatusMessage() {}

    inline RilRequestGetUiccAppStatusMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
};
