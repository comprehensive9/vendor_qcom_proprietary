/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestLastCallFailCauseMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchLastCallFailCause(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestLastCallFailCauseMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          using FailCauseInfo = qcril::interfaces::LastCallFailCauseInfo;
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          RIL_LastCallFailCauseInfo failCause;
          failCause.vendor_cause = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto qcrilFailCause = std::static_pointer_cast<FailCauseInfo>(resp->data);
            if (qcrilFailCause) {
              if (qcrilFailCause->hasCallFailCause()) {
                failCause.cause_code =
                    static_cast<RIL_LastCallFailCause>(qcrilFailCause->getCallFailCause());
              }
              if (qcrilFailCause->hasCallFailCauseDescription()) {
                auto description = qcrilFailCause->getCallFailCauseDescription();
                failCause.vendor_cause = new char[description.size() + 1]();
                if (failCause.vendor_cause) {
                  description.copy(failCause.vendor_cause, sizeof description);
                }
              }
            } else {
              errorCode = RIL_E_GENERIC_FAILURE;
            }
          }
          auto p = std::make_shared<Marshal>();
          p->write(failCause);
          sendResponse(context, errorCode, p);
          delete[] failCause.vendor_cause;
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
