/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestImsExplicitCallTransferMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsExplicitCallTransfer(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestImsExplicitCallTransferMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    RIL_IMS_ExplicitCallTransfer ectInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }

    Marshal::Result ret = p.read(ectInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }
    // Set parameters
    msg->setCallId(ectInfo.callId);
    msg->setEctType(convertEctType(ectInfo.ectType));
    if (ectInfo.targetAddress != nullptr) {
      msg->setTargetAddress(ectInfo.targetAddress);
    }
    msg->setTargetCallId(ectInfo.targetCallId);
    msg->setIsImsRequest(true);
    p.release(ectInfo);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsExplicitCallTransfer:resp: errorCode=%d", errorCode);
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
