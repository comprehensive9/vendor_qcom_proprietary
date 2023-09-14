/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestGetClirMessage.h>
#include <interfaces/voice/QcRilRequestSetClirMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsGetClir(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetClirMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    msg->setIsImsRequest(true);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          RIL_IMS_ClirInfo clirInfo{};
          std::shared_ptr<qcril::interfaces::ClirInfo> respData = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            respData = std::static_pointer_cast<qcril::interfaces::ClirInfo>(resp->data);
            ril::socket::utils::convertToSocket(clirInfo, respData);
          }
          QCRIL_LOG_DEBUG("dispatchImsGetClir:resp: errorCode=%d", errorCode);
          auto p = std::make_shared<Marshal>();
          p->write(clirInfo);
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

void dispatchImsSetClir(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    RIL_IMS_SetClirInfo clirInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }

    Marshal::Result ret = p.read(clirInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }
    auto msg = std::make_shared<QcRilRequestSetClirMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    msg->setParamN(clirInfo.action);
    msg->setIsImsRequest(true);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsSetClir:resp: errorCode=%d", errorCode);
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
