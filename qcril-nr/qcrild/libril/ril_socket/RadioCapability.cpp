/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>

#include "interfaces/nas/nas_types.h"
#include <interfaces/nas/RilRequestSetRadioCapMessage.h>
#include <interfaces/nas/RilRequestGetRadioCapMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchSetRadioCapability(std::shared_ptr<SocketRequestContext> context,
                                          Marshal& p) {
  bool sendFailure = false;
  RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

  do {
    RIL_RadioCapability radioCap;
    if (p.read(radioCap) == Marshal::Result::SUCCESS) {
        sendFailure = false;
        errorCode = RIL_E_INTERNAL_ERR;
        break;
    }
    auto msg = std::make_shared<RilRequestSetRadioCapMessage>(context, radioCap);
    if (msg == nullptr) {
      errorCode = RIL_E_NO_MEMORY;
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
    sendResponse(context, errorCode, nullptr);
  }
}

void dispatchGetRadioCapability(std::shared_ptr<SocketRequestContext> context,
                                          Marshal& p) {
  bool sendFailure = false;
  RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

  do {
    auto msg = std::make_shared<RilRequestGetRadioCapMessage>(context);
    if (msg == nullptr) {
      sendFailure = true;
      errorCode = RIL_E_NO_MEMORY;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<Marshal> p = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            p = std::make_shared<Marshal>();
            auto rilCapResult = std::static_pointer_cast<qcril::interfaces::RilRadioCapResult_t>(resp->data);
            if ( p && rilCapResult) {
                p->write(rilCapResult->respData);
            }
          }
          sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (false);

  if (sendFailure) {
    sendResponse(context, errorCode, nullptr);
  }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
