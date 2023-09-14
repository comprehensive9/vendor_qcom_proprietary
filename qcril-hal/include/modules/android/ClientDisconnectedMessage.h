/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef MODULES_ANDROID_CLIENT_DISCONNECTED_MESSAGE_H
#define MODULES_ANDROID_CLIENT_DISCONNECTED_MESSAGE_H

#include <framework/UnSolicitedMessage.h>
#include <framework/legacy.h>
#include <framework/Dispatcher.h>
#include <framework/add_message_id.h>

/**
  * ClientDisconnectedMessage: Sent when an android client gets disconnected.
  * Modules can register for this message to perform actions
  * on disconnection (e.g. sending unsols)
  */
class ClientDisconnectedMessage: public UnSolicitedMessage, public add_message_id<ClientDisconnectedMessage>
{
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.android.client_disconnected";
    private:
        qcril_instance_id_e_type instance_id;
    public:
        ClientDisconnectedMessage(qcril_instance_id_e_type instance):
            UnSolicitedMessage(get_class_message_id())
        {
            this->instance_id = instance;
        }
         ~ClientDisconnectedMessage() { }
        virtual string dump() {
            return MESSAGE_NAME;
        }
        std::shared_ptr<UnSolicitedMessage> clone() override {
            return (std::make_shared<ClientDisconnectedMessage>(instance_id));
        }
};

#endif
