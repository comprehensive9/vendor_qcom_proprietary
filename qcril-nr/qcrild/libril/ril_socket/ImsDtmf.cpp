
/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestImsSendDtmfMessage.h>
#include <interfaces/voice/QcRilRequestImsStartDtmfMessage.h>
#include <interfaces/voice/QcRilRequestImsStopDtmfMessage.h>

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsDtmf(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestImsSendDtmfMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("dtmf input is empty");
      sendFailure = true;
      break;
    }

    char digit;
    p.read(digit);
    msg->setDigit(digit);
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

void dispatchImsDtmfStart(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestImsStartDtmfMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("dtmf input is empty");
      sendFailure = true;
      break;
    }

    char digit;
    p.read(digit);
    msg->setDigit(digit);
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

void dispatchImsDtmfStop(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestImsStopDtmfMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

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
