/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestQueryCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestSetCallForwardMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsQueryCallForwardStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestQueryCallForwardMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    RIL_IMS_CallForwardInfo callFwdInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }
    Marshal::Result ret = p.read(callFwdInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }
    msg->setIsImsRequest(true);
    // Set parameters
    msg->setReason(callFwdInfo.reason);
    msg->setServiceClass(callFwdInfo.serviceClass);
    p.release(callFwdInfo);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          RIL_IMS_QueryCallForwardStatusInfo outCallFwdInfo{};
          std::shared_ptr<qcril::interfaces::GetCallForwardRespData> respData = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            respData =
                std::static_pointer_cast<qcril::interfaces::GetCallForwardRespData>(resp->data);
            ril::socket::utils::convertToSocket(outCallFwdInfo, respData);
          }
          QCRIL_LOG_DEBUG("dispatchImsQueryCallForwardStatus:resp: errorCode=%d", errorCode);
          auto p = std::make_shared<Marshal>();
          p->write(outCallFwdInfo);
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

void dispatchImsSetCallForwardStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetCallForwardMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    RIL_IMS_CallForwardInfo callForwardInfo{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }
    Marshal::Result ret = p.read(callForwardInfo);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }
    msg->setIsImsRequest(true);
    // Set parameters
    msg->setStatus(callForwardInfo.status);
    msg->setReason(callForwardInfo.reason);
    msg->setServiceClass(callForwardInfo.serviceClass);
    msg->setToa(callForwardInfo.toa);
    if (callForwardInfo.number != nullptr) {
      msg->setNumber(callForwardInfo.number);
    }
    msg->setTimeSeconds(callForwardInfo.timeSeconds);
    if (callForwardInfo.hasCallFwdTimerStart) {
      auto callFwdTimerStart = std::make_shared<qcril::interfaces::CallFwdTimerInfo>();
      convertCallFwdTimerInfo(*callFwdTimerStart, callForwardInfo.callFwdTimerStart);
      msg->setCallFwdTimerStart(callFwdTimerStart);
    }

    if (callForwardInfo.hasCallFwdTimerEnd) {
      auto callFwdTimerEnd = std::make_shared<qcril::interfaces::CallFwdTimerInfo>();
      convertCallFwdTimerInfo(*callFwdTimerEnd, callForwardInfo.callFwdTimerEnd);
      msg->setCallFwdTimerEnd(callFwdTimerEnd);
    }
    p.release(callForwardInfo);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          std::shared_ptr<qcril::interfaces::SetCallForwardRespData> respData = nullptr;
          RIL_IMS_SetCallForwardStatusInfo outCallFwdInfo{};
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            respData =
                std::static_pointer_cast<qcril::interfaces::SetCallForwardRespData>(resp->data);
            ril::socket::utils::convertToSocket(outCallFwdInfo, respData);
          }
          QCRIL_LOG_DEBUG("dispatchImsSetCallForwardStatus:resp: errorCode=%d", errorCode);
          auto p = std::make_shared<Marshal>();
          p->write(outCallFwdInfo);
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
