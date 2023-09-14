/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>

#include <interfaces/nas/RilRequestSetNetworkSelectionAutoMessage.h>
#include <interfaces/nas/RilRequestSetNetworkSelectionManualMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchSetNetworkSelectionAutomatic(std::shared_ptr<SocketRequestContext> context,
                                          Marshal& p) {
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<RilRequestSetNetworkSelectionAutoMessage>(context);
    if (msg == nullptr) {
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (false);

  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
}

void dispatchSetNetworkSelectionManual(std::shared_ptr<SocketRequestContext> context,
                                          Marshal& p) {
  bool sendFailure = false;

  do {
    std::string plmn;
    p.read(plmn);
    auto msg = std::make_shared<RilRequestSetNetworkSelectionManualMessage>(context, plmn);
    if (msg == nullptr) {
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (false);

  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
