/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/nas/nas_types.h>
#include <interfaces/nas/RilRequestEnableUiccAppMessage.h>
#include <interfaces/nas/RilRequestGetUiccAppStatusMessage.h>
#include <Marshal.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchEnableSim(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  RIL_Errno err = RIL_E_SUCCESS;
  do {
    bool state;
    if(p.read(state) != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("failed to read parcel");
      sendFailure = true;
      break;
    }

    auto msg = std::make_shared<RilRequestEnableUiccAppMessage>(context, state);
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
    sendResponse(context, err, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void dispatchGetEnableSimStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<RilRequestGetUiccAppStatusMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<qcril::interfaces::RilGetUiccAppStatusResult_t> result;
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<Marshal> p = nullptr;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              bool state = false;
              errorCode = resp->errorCode;
              result = std::static_pointer_cast<qcril::interfaces::RilGetUiccAppStatusResult_t>(resp->data);
              if (result != nullptr) {
                state = result->state;
              }
              p = std::make_shared<Marshal>();
              if (p) {
                p->write(state);
              } else if (errorCode == RIL_E_SUCCESS) {
                errorCode = RIL_E_NO_MEMORY;
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
