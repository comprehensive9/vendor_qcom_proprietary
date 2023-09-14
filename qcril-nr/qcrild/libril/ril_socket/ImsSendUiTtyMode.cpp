/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestImsSetUiTtyModeMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsSendUiTtyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    RIL_IMS_TtyNotifyInfo ttyInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }

    Marshal::Result ret = p.read(ttyInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }
    auto msg = std::make_shared<QcRilRequestImsSetUiTtyModeMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

// Set parameters
    if (ttyInfo.mode != RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_INVALID) {
      msg->setTtyMode(convertTtyMode(ttyInfo.mode));
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsSendUiTtyMode:resp: errorCode=%d", errorCode);
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
