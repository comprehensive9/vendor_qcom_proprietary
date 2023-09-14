/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <ril_socket_api.h>

#include "request/IndicationRegistrationFilterMessage.h"
#include <Marshal.h>
#include <framework/Log.h>
#include <telephony/ril.h>

#ifndef RIL_FOR_MDM_LE
#include <utils/Log.h>
#else
#include <utils/Log2.h>
#endif

namespace ril {
namespace socket {
namespace api {

void dispatchRegisterDataUnsolIndication(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    int32_t filter = p.readInt32();
    RLOGI("Filter Value: %d", filter);
    RLOGI("Dispatching IndicationRegistrationFilterMessage request.");
    using namespace rildata;
    auto msg = std::make_shared<IndicationRegistrationFilterMessage>(filter);
    if(msg) {
        GenericCallback<int> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<int> rsp) -> void {
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                int val = -1;
                if ((status == Message::Callback::Status::SUCCESS) && (rsp != nullptr)) {
                    errorCode = RIL_Errno::RIL_E_SUCCESS;
                    RLOGI("UiInfoIndRegistration invoked status %d ", status);
                    RLOGI("Response code = %d", *rsp);
                    val = *rsp;
                    p->write(val);
                    sendResponse(context, errorCode, p);
                }
                else {
                    sendResponse(context, errorCode, nullptr);
                }
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

}
}
}
