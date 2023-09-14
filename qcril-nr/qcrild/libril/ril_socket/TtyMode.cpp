/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestSetTtyModeMessage.h>
#include <interfaces/voice/QcRilRequestGetTtyModeMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril::socket::api {

qcril::interfaces::TtyMode convertToTtyMode(int mode) {
  qcril::interfaces::TtyMode ttyMode = qcril::interfaces::TtyMode::UNKNOWN;
  switch (mode) {
    case 0:
      ttyMode = qcril::interfaces::TtyMode::MODE_OFF;
      break;
    case 1:
      ttyMode = qcril::interfaces::TtyMode::FULL;
      break;
    case 2:
      ttyMode = qcril::interfaces::TtyMode::HCO;
      break;
    case 3:
      ttyMode = qcril::interfaces::TtyMode::VCO;
      break;
    default:
      QCRIL_LOG_ERROR("Invlid mode");
      break;
  }
  return ttyMode;
}

int convertToRilMode(qcril::interfaces::TtyMode ttyMode) {
  int mode = -1;
  switch (ttyMode) {
    case qcril::interfaces::TtyMode::MODE_OFF:
      mode = 0;
      break;
    case qcril::interfaces::TtyMode::FULL:
      mode = 1;
      break;
    case qcril::interfaces::TtyMode::HCO:
      mode = 2;
      break;
    case qcril::interfaces::TtyMode::VCO:
      mode = 3;
      break;
    default:
      QCRIL_LOG_ERROR("Invlid TtyMode");
      break;
  }
  return mode;
}

void dispatchSetTtyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetTtyModeMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Tty input is empty");
      sendFailure = true;
      break;
    }

    int mode;
    p.read(mode);
    auto ttyMode = convertToTtyMode(mode);
    if (ttyMode == qcril::interfaces::TtyMode::UNKNOWN) {
      sendFailure = true;
      break;
    }
    msg->setTtyMode(ttyMode);
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

void dispatchGetTtyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetTtyModeMessage>(context);
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
            auto ttyModeResp = std::static_pointer_cast<qcril::interfaces::TtyModeResp>(resp->data);
            if (ttyModeResp && ttyModeResp->hasTtyMode()) {
              mode = convertToRilMode(ttyModeResp->getTtyMode());
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
