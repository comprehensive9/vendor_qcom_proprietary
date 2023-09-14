/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/ims/QcRilRequestImsSendRttMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsSendRttMsg(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();
    char* rttMessage;
    std::size_t rttMessageLen;

    if (p.readAndAlloc(rttMessage, rttMessageLen) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Send Rtt Message is empty");
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        QCRIL_LOG_FUNC_RETURN();
        return;
    }

    auto msg = std::make_shared<QcRilRequestImsSendRttMessage>(context,
            std::string(rttMessage, rttMessageLen));
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        delete[] rttMessage;
        rttMessage = nullptr;
        QCRIL_LOG_FUNC_RETURN();
        return;
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              sendResponse(context, resp->errorCode, nullptr);
          } else {
              sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
          }
    });

    msg->setCallback(&cb);
    msg->dispatch();
    delete[] rttMessage;
    rttMessage = nullptr;
    QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
