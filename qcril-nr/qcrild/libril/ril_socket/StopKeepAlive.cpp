/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <framework/GenericCallback.h>
#include <ril_socket_api.h>

#include <request/StopKeepAliveRequestMessage.h>
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

void dispatchStopKeepAlive(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching StopKeepAlive request.");
    using namespace rildata;
    int serial = 0;
    int32_t handle = p.readInt32();
    RLOGI("handle: %d", handle);
    auto msg =
        std::make_shared<StopKeepAliveRequestMessage>(
            serial,
            handle,
            nullptr);
    if(msg) {
        GenericCallback<ResponseError_t> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<ResponseError_t> rsp) -> void {
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
                    RLOGI("StopKeepAlive invoked status %d ", status);
                    RLOGI("Error code = %d", static_cast<int>(*rsp));
                    val = static_cast<int>(*rsp);
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
