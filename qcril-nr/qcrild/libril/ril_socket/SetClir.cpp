/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestSetClirMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchSetClir(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
  do {
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("set clir is empty");
      sendFailure = true;
      errorCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    int state;
    auto msg = std::make_shared<QcRilRequestSetClirMessage>(context);
    if (p.read(state) == Marshal::Result::SUCCESS) {
        if (msg == nullptr) {
          QCRIL_LOG_ERROR("msg is nullptr");
          sendFailure = true;
          errorCode = RIL_E_NO_MEMORY;
          break;
        }
        msg->setParamN(state);
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    sendResponse(context, errorCode, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
