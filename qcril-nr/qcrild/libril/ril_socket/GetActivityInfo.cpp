/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/dms/dms_types.h>
#include <interfaces/dms/RilRequestGetModemActivityMessage.h>
#include <Marshal.h>
#include <marshal/ActivityInfo.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchGetActivityInfo(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<RilRequestGetModemActivityMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<qcril::interfaces::RilGetModemActivityResult_t> result;
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<Marshal> p = nullptr;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              result = std::static_pointer_cast<qcril::interfaces::RilGetModemActivityResult_t>(resp->data);
              if (result) {
                  p = std::make_shared<Marshal>();
                  if (p) {
                    p->write(result->respData);
                  } else if (errorCode == RIL_E_SUCCESS) {
                  errorCode = RIL_E_NO_MEMORY;
                }
              }
            }
            sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
