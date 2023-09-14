/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/**
 * Message Class: SsrIndicationMessage
 * Sender: QMI client
 * Receiver: Any module which needs to handle a Sub System Restart (SSR)
 * Data transferred: A bool value indicating the status of SSR
 * Usage: QMI client broadcasts this message internally to indicate an SSR.
 */

class SsrIndicationMessage: public UnSolicitedMessage,
    public add_message_id<SsrIndicationMessage>
{
    private:

        bool mState;

    public:

        static constexpr const char *MESSAGE_NAME =
            "com.qualcomm.qti.qcril.core.ssr_indication_message";
        SsrIndicationMessage() = delete;

        SsrIndicationMessage(bool state): UnSolicitedMessage(get_class_message_id()){
            mState = state;
            mName = MESSAGE_NAME;
        };

        virtual string dump() {
            return mName;
        }

        std::shared_ptr<UnSolicitedMessage> clone() {
            auto msg = std::make_shared<SsrIndicationMessage>(mState);
            return msg;
        }

        bool getState() {
            return mState;
        }
};
