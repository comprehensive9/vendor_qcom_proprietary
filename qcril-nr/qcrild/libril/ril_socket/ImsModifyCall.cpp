/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestImsModifyCallConfirmMessage.h>
#include <interfaces/voice/QcRilRequestImsModifyCallInitiateMessage.h>
#include <interfaces/voice/QcRilRequestImsCancelModifyCallMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsModifyCallInitiate(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    RIL_IMS_CallModifyInfo modifyInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }
    Marshal::Result ret = p.read(modifyInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }
    auto msg = std::make_shared<QcRilRequestImsModifyCallInitiateMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    // Set parameters
    msg->setCallId(modifyInfo.callId);
    msg->setCallType(convertCallType(modifyInfo.callType));
    msg->setCallDomain(convertCallDomain(modifyInfo.callDomain));
    if (modifyInfo.hasRttMode) {
      msg->setRttMode(convertRttMode(modifyInfo.rttMode));
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsModifyCallInitiate:resp: errorCode=%d", errorCode);
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

void dispatchImsModifyCallConfirm(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    RIL_IMS_CallModifyInfo modifyInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }
    Marshal::Result ret = p.read(modifyInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }
    auto msg = std::make_shared<QcRilRequestImsModifyCallConfirmMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    // Set parameters
    msg->setCallId(modifyInfo.callId);
    msg->setCallType(convertCallType(modifyInfo.callType));
    msg->setCallDomain(convertCallDomain(modifyInfo.callDomain));
    if (modifyInfo.hasRttMode) {
      msg->setRttMode(convertRttMode(modifyInfo.rttMode));
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsModifyCallConfirm:resp: errorCode=%d", errorCode);
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

void dispatchImsCancelModifyCall(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    uint32_t callId = 0;
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }
    Marshal::Result ret = p.read(callId);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }
    auto msg = std::make_shared<QcRilRequestImsCancelModifyCallMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    // Set parameters
    msg->setCallId(callId);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsCancelModifyCall:resp: errorCode=%d", errorCode);
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
