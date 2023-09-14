/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>

#include <interfaces/nas/nas_types.h>
#include <interfaces/nas/RilRequestStartNetworkScanMessage.h>
#include <interfaces/nas/RilRequestStopNetworkScanMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril::socket::api {

void dispatchStartNetworkScanRequest(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  RIL_Errno err = RIL_E_GENERIC_FAILURE;
  bool sendFailure = false;

  do {
    RIL_NetworkScanRequest scanRequest = {};
    p.read(scanRequest);
    auto msg = std::make_shared<RilRequestStartNetworkScanMessage>(context, scanRequest);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      err = RIL_E_NO_MEMORY;
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

void dispatchStopNetworkScanRequest(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  RIL_Errno err = RIL_E_GENERIC_FAILURE;
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<RilRequestStopNetworkScanMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      err = RIL_E_NO_MEMORY;
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

}  // namespace ril::socket::api
