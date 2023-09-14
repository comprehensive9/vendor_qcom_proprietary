/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestQueryCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestSetCallForwardMessage.h>
#include <marshal/CallForwardInfo.h>
#include <Marshal.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchQueryCallForwardStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestQueryCallForwardMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    RIL_CallForwardInfo callFwdInfo = {};
    p.read(callFwdInfo);

    // Set parameters
    msg->setReason(callFwdInfo.reason);
    msg->setServiceClass(callFwdInfo.serviceClass);

    p.release(callFwdInfo);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::vector<RIL_CallForwardInfo> callForwardInfos;
          std::shared_ptr<qcril::interfaces::GetCallForwardRespData> data = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            data = std::static_pointer_cast<qcril::interfaces::GetCallForwardRespData>(resp->data);
            auto cfInfoList = data->getCallForwardInfo();
            if (data && !cfInfoList.empty()) {
              uint32_t count = cfInfoList.size();
              for (uint32_t i = 0; i < count; i++) {
                RIL_CallForwardInfo callFwdInfo = {};
                auto &resp = cfInfoList[i];
                if (resp.hasStatus()) {
                  callFwdInfo.status = resp.getStatus();
                }
                if (resp.hasReason()) {
                  callFwdInfo.reason = resp.getReason();
                }
                if (resp.hasServiceClass()) {
                  callFwdInfo.serviceClass = resp.getServiceClass();
                }
                if (resp.hasToa()) {
                  callFwdInfo.toa = resp.getToa();
                }
                callFwdInfo.number = nullptr;
                if (resp.hasNumber()) {
                  auto number = resp.getNumber();
                  if (!number.empty()) {
                    callFwdInfo.number = new char[number.size() + 1]();
                    if (callFwdInfo.number) {
                      number.copy(callFwdInfo.number, number.size() + 1);
                    }
                  }
                }
                if (resp.hasTimeSeconds()) {
                  callFwdInfo.timeSeconds = resp.getTimeSeconds();
                }
                callForwardInfos.push_back(callFwdInfo);
              }
            }
          }
          auto p = std::make_shared<Marshal>();
          int32_t numCallFwdInfos = callForwardInfos.size();
          if (numCallFwdInfos) {
            if (p) {
              p->write(numCallFwdInfos);
              for (auto callFwdInfo : callForwardInfos) {
                p->write(callFwdInfo);
              }
            }
            // Free the callForwardInfos objects
            for (auto callFwdInfo : callForwardInfos) {
              delete[] callFwdInfo.number;
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

void dispatchSetCallForward(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetCallForwardMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    // Read data from marshal
    RIL_CallForwardInfo callFwdInfo = {};
    p.read(callFwdInfo);

    // Set parameters
    msg->setStatus(callFwdInfo.status);
    msg->setReason(callFwdInfo.reason);
    msg->setServiceClass(callFwdInfo.serviceClass);
    msg->setToa(callFwdInfo.toa);
    if (callFwdInfo.number) {
      msg->setNumber(callFwdInfo.number);
    }
    msg->setTimeSeconds(callFwdInfo.timeSeconds);
    p.release(callFwdInfo);
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

}  // namespace api
}  // namespace socket
}  // namespace ril
