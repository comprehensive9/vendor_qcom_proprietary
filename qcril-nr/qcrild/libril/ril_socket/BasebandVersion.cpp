/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>

#include <interfaces/dms/RilRequestGetBaseBandVersionMessage.h>
#include <interfaces/dms/dms_types.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchBasebandVersion(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<RilRequestGetBaseBandVersionMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          std::shared_ptr<qcril::interfaces::RilGetBaseBandResult_t> rilGetBasebandResult;
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            rilGetBasebandResult =
                std::static_pointer_cast<qcril::interfaces::RilGetBaseBandResult_t>(resp->data);
            auto p = std::make_shared<Marshal>();
            if (p && rilGetBasebandResult) {
              p->write(rilGetBasebandResult->version);
            }
            sendResponse(context, errorCode, p);
          } else {
            sendResponse(context, errorCode, nullptr);
          }
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
