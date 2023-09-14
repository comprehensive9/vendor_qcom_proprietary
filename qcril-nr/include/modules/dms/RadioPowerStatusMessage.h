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
 * Message Class: RadioPowerStatusMessage
 * Sender: NAS module
 * Receiver: Any module which needs to handle a change in telephony power state
 * Data transferred: Power state i.e Low Power Mode or Online
 * Usage: NAS module broadcasts this message to indicate a change in power state
 */

class RadioPowerStatusMessage: public UnSolicitedMessage,
    public add_message_id<RadioPowerStatusMessage>
{

    public:
        enum class RadioState{
            RADIO_LPM,
            RADIO_ONLINE
        };

    private:

        RadioState mState;

    public:

        static constexpr const char *MESSAGE_NAME =
            "com.qualcomm.qti.qcril.core.radio_power_status_message";
        RadioPowerStatusMessage() = delete;

        RadioPowerStatusMessage(int state): UnSolicitedMessage(get_class_message_id()){
            mState = static_cast<RadioState>(state);
            mName = MESSAGE_NAME;
        };

        virtual string dump() {
            return mName;
        }

        std::shared_ptr<UnSolicitedMessage> clone() {
            auto msg = std::make_shared<RadioPowerStatusMessage>(static_cast<int>(mState));
            return msg;
        }

        RadioState getRadioState() {
            return mState;
        }
};
