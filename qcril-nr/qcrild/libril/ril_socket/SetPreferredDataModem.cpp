/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <framework/GenericCallback.h>
#include <ril_socket_api.h>

#include <request/SetPreferredDataModemRequestMessage.h>
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

void dispatchSetPreferredDataModem(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching SetPreferredDataModem request.");
    using namespace rildata;
    int serial = 0;
    int modemid = -1;
    p.read(modemid);
    RLOGI("modemId: %d", modemid);
    auto msg =
        std::make_shared<SetPreferredDataModemRequestMessage>(
            serial,
            modemid,
            nullptr);
    if(msg) {
        GenericCallback<SetPreferredDataModemResponse_t> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<SetPreferredDataModemResponse_t> rsp) -> void {
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                ResponseError_t data = ResponseError_t::NOT_AVAILABLE;
                int val = -1;
                if ((status == Message::Callback::Status::SUCCESS) && (rsp != nullptr)) {
                    errorCode = RIL_Errno::RIL_E_SUCCESS;
                    RLOGI("SetPreferredDataModem invoked status %d ", status);
                    RLOGI("Internal Error code = %d", static_cast<int>(rsp->errCode));
                    data = rsp->toResponseError();
                    val = static_cast<int>(data);
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
