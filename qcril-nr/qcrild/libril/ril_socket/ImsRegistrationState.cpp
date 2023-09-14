/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/sms/RilRequestGetImsRegistrationMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril::socket::api {

void dispatchGetImsRegistration(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<RilRequestGetImsRegistrationMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context]([[maybe_unused]] std::shared_ptr<Message> msg,
                  Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto regInfo = std::static_pointer_cast<RilGetImsRegistrationResult_t>(resp->data);
            auto p = std::make_shared<Marshal>();
            if (p && regInfo) {
              int registered = (regInfo->isRegistered) ? 1 : 0;
              p->write(registered);
              p->write(static_cast<int>(regInfo->ratFamily));
            }
            sendResponse(context, errorCode, p);
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

}  // namespace ril::socket::api
