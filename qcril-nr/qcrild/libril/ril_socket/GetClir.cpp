/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>

#include <interfaces/voice/QcRilRequestGetClirMessage.h>
#include <interfaces/voice/voice.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchGetClir(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<QcRilRequestGetClirMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          std::shared_ptr<qcril::interfaces::ClirInfo> rilClirInfoResult;
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr <Marshal> p = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              rilClirInfoResult = std::static_pointer_cast<qcril::interfaces::ClirInfo>(resp->data);
              int m, n;
              if (rilClirInfoResult != nullptr) {
                  if (rilClirInfoResult->hasAction() == true
                          && rilClirInfoResult->hasPresentation() == true){
                      n = rilClirInfoResult->getAction();
                      m = rilClirInfoResult->getPresentation();
                      p = std::make_shared<Marshal>();
                      if (p != nullptr) {
                          if (p->write(m) != Marshal::Result::SUCCESS ||
                                  p->write(n) != Marshal::Result::SUCCESS){
                              p = nullptr;
                          } else {
                              errorCode = resp->errorCode;
                          }
                      } else {
                          errorCode = RIL_E_NO_MEMORY;
                      }
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
