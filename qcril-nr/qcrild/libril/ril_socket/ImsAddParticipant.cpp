/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestImsAddParticipantMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsAddParticipant(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestImsAddParticipantMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    char *address = nullptr;
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }
    Marshal::Result ret = p.read(address);
    if (ret != Marshal::Result::SUCCESS || address == nullptr) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }
    // Set parameters
    msg->setAddress(address);
    p.release(address);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          std::shared_ptr<qcril::interfaces::SipErrorInfo> rilErrorDetails = nullptr;
          std::shared_ptr<Marshal> p;
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          RIL_IMS_SipErrorInfo errorDetails{};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            rilErrorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
            if (rilErrorDetails != nullptr) {
              ril::socket::utils::convertToSocket(errorDetails, *rilErrorDetails);
              p = std::make_shared<Marshal>();
              p->write(errorDetails);
              ril::socket::utils::release(errorDetails);
            }
          }
          QCRIL_LOG_DEBUG("dispatchImsAddParticipant:resp: errorCode=%d", errorCode);
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
