/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestGetCallWaitingMessage.h>
#include <interfaces/voice/QcRilRequestSetCallWaitingMessage.h>
#include <Marshal.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchQueryCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetCallWaitingMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    int serviceClass;
    p.read(serviceClass);
    msg->setServiceClass(serviceClass);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                      std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          int enable = 0;
          int serviceClass = -1;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto cwInfo = std::static_pointer_cast<qcril::interfaces::CallWaitingInfo>(resp->data);
            if (cwInfo->hasStatus()) {
              enable =
                  ((cwInfo->getStatus() == qcril::interfaces::ServiceClassStatus::ENABLED) ? 1
                                                                                           : 0);
            }
            if (cwInfo->hasServiceClass()) {
              serviceClass = cwInfo->getServiceClass();
            }
          }
          auto p = std::make_shared<Marshal>();
          p->write(enable);
          p->write(serviceClass);
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

void dispatchSetCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetCallWaitingMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    int enable = 0;
    p.read(enable);
    int serviceClass = 0;
    p.read(serviceClass);
    if (enable) {
      msg->setServiceStatus(qcril::interfaces::ServiceClassStatus::ENABLED);
    } else {
      msg->setServiceStatus(qcril::interfaces::ServiceClassStatus::DISABLED);
    }
    msg->setServiceClass(serviceClass);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
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
