/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestGetClipMessage.h>
#include <interfaces/voice/voice.h>
#include <Marshal.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchQueryClip(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetClipMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          int clip = 2; // for "unknown, e.g. no network etc"
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto clipInfo = std::static_pointer_cast<qcril::interfaces::ClipInfo>(resp->data);
            if (clipInfo && clipInfo->hasClipStatus()) {
              if (clipInfo->getClipStatus() == qcril::interfaces::ClipStatus::NOT_PROVISIONED) {
                clip = 0; // for "CLIP not provisioned"
              } else if (clipInfo->getClipStatus() == qcril::interfaces::ClipStatus::PROVISIONED) {
                clip = 1; // for "CLIP provisioned"
              }
            }
          }
          auto p = std::make_shared<Marshal>();
          p->write(clip);
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
