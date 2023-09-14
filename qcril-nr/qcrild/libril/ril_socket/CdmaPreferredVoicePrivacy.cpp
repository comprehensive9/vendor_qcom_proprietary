/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestSetPreferredVoicePrivacyMessage.h>
#include <interfaces/voice/QcRilRequestGetPreferredVoicePrivacyMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril::socket::api {

qcril::interfaces::PrivacyMode convertToPrivacyMode(int mode) {
  qcril::interfaces::PrivacyMode privacyMode = qcril::interfaces::PrivacyMode::UNKNOWN;
  switch (mode) {
    case 0:
      privacyMode = qcril::interfaces::PrivacyMode::STANDARD;
      break;
    case 1:
      privacyMode = qcril::interfaces::PrivacyMode::ENHANCED;
      break;
    default:
      QCRIL_LOG_ERROR("Invlid mode");
      break;
  }
  return privacyMode;
}

int convertToRilMode(qcril::interfaces::PrivacyMode privacyMode) {
  int mode = -1;
  switch (privacyMode) {
    case qcril::interfaces::PrivacyMode::STANDARD:
      mode = 0;
      break;
    case qcril::interfaces::PrivacyMode::ENHANCED:
      mode = 1;
      break;
    default:
      QCRIL_LOG_ERROR("Invlid Privacy Mode");
      break;
  }
  return mode;
}

void dispatchSetVoicePrivacyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetPreferredVoicePrivacyMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Privacy mode input is empty");
      sendFailure = true;
      break;
    }

    int mode;
    p.read(mode);
    auto privacyMode = convertToPrivacyMode(mode);
    if (privacyMode == qcril::interfaces::PrivacyMode::UNKNOWN) {
      sendFailure = true;
      break;
    }
    msg->setPrivacyMode(privacyMode);
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
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void dispatchGetVoicePrivacyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetPreferredVoicePrivacyMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          int mode = -1;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto privacyModeResp =
                std::static_pointer_cast<qcril::interfaces::PrivacyModeResp>(resp->data);
            if (privacyModeResp && privacyModeResp->hasPrivacyMode()) {
              mode = convertToRilMode(privacyModeResp->getPrivacyMode());
            }
          }
          auto p = std::make_shared<Marshal>();
          if (p && mode != -1) {
            p->write(mode);
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

}  // namespace ril::socket::api
