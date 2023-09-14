/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/*
 * Notify UIM that it is Manual provisioning for both Single sim and multi sim if HIDL version is greater than or equal to 1.5
 *
 */
class RilManualProvisioningMessage
    : public UnSolicitedMessage,
      public add_message_id<RilManualProvisioningMessage> {

    public:
        static constexpr const char *MESSAGE_NAME = "RilManualProvisioningMessage";

        RilManualProvisioningMessage(): UnSolicitedMessage(get_class_message_id()) {
            mName = MESSAGE_NAME;
        }

        ~RilManualProvisioningMessage() {}

        std::shared_ptr<UnSolicitedMessage> clone() {
            return nullptr;
        }

        string dump() {
            return RilManualProvisioningMessage::MESSAGE_NAME;
        }
};