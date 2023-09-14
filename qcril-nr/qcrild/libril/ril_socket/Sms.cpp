/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <framework/Message.h>
#include <framework/GenericCallback.h>
#include <framework/Log.h>
#include <ril_socket_api.h>
#include <Marshal.h>

#include <interfaces/sms/RilRequestReportSmsMemoryStatusMessage.h>

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void reportSmsMemoryStatus(std::shared_ptr<SocketRequestContext> context, const Marshal& p) {
    bool storageSpaceAvailable = false;
    if (p.read(storageSpaceAvailable) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    auto msg = std::make_shared<RilRequestReportSmsMemoryStatusMessage>(context, storageSpaceAvailable);
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                      std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno e = RIL_E_INTERNAL_ERR;
                if (msg && status == Message::Callback::Status::SUCCESS && resp) {
                    e = resp->errorCode;
                }
                sendResponse(context, e, nullptr);
            }
        );
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

} // namespace api
} // namespace socket
} // namespace ril