/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <framework/Log.h>
#include <ril_socket_api.h>
#include <SocketRequestContext.h>
#include <framework/Message.h>
#include <framework/GenericCallback.h>
#include <marshal/GsmBroadcastSms.h>
#include <interfaces/sms/RilRequestGetGsmBroadcastConfigMessage.h>
#include <interfaces/sms/RilRequestGsmSetBroadcastSmsConfigMessage.h>
#include <interfaces/sms/RilRequestGsmSmsBroadcastActivateMessage.h>
#include <telephony/ril.h>
#define TAG "GsmBroadcastSms"

namespace ril {
namespace socket {
namespace api {

void dispatchRilRequestGsmGetBroadcastSmsConfig(std::shared_ptr<SocketRequestContext> ctx,
                                                Marshal& p) {
  QCRIL_LOG_DEBUG("Dispatching RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG");
  auto msg = std::make_shared<RilRequestGetGsmBroadcastConfigMessage>(ctx);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [ctx]([[maybe_unused]] std::shared_ptr<Message> msg,
              Message::Callback::Status status,
              std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) {
          RIL_Errno err = RIL_E_INTERNAL_ERR;
          std::shared_ptr<Marshal> m;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            err = resp->errorCode;
            auto broadcastConfigResp =
                std::static_pointer_cast<RilGetGsmBroadcastConfigResult_t>(resp->data);
            m = std::make_shared<Marshal>();
            if(m && broadcastConfigResp) {
              Marshal::Result r =
                  m->write(broadcastConfigResp->configList.data(), broadcastConfigResp->configList.size());
              if (r != Marshal::Result::SUCCESS) {
                QCRIL_LOG_ERROR("Failure while marshalling");
                err = RIL_E_INTERNAL_ERR;
              }
            }
          } else {
            err = dispatchStatusToRilErrno(status);
          }
          sendResponse(ctx, err, m);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  }
};

void dispatchRilRequestGsmSetBroadcastSmsConfig(std::shared_ptr<SocketRequestContext> ctx,
                                                Marshal& p) {
  QCRIL_LOG_DEBUG("Dispatching RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG");
  size_t num = 0;
  RIL_GSM_BroadcastSmsConfigInfo* configInfo = nullptr;
  size_t rdSize = 0;

  Marshal::Result res = p.read(configInfo, rdSize);
  if (res != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Writing to Marshal failed. err: %d", (int)res);
    sendResponse(ctx, RIL_E_INTERNAL_ERR, nullptr);
    return;
  }
  num = rdSize / sizeof(RIL_GSM_BroadcastSmsConfigInfo);

  std::vector<RIL_GSM_BroadcastSmsConfigInfo> configVec(num);
  for (int i = 0; i < num; i++) {
    configVec[i].fromServiceId = configInfo[i].fromServiceId;
    configVec[i].toServiceId = configInfo[i].toServiceId;
    configVec[i].fromCodeScheme = configInfo[i].fromCodeScheme;
    configVec[i].toCodeScheme = configInfo[i].toCodeScheme;
    configVec[i].selected = configInfo[i].selected;
  }

  auto msg = std::make_shared<RilRequestGsmSetBroadcastSmsConfigMessage>(ctx, std::move(configVec));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([ctx]([[maybe_unused]] std::shared_ptr<Message> msg,
               Message::Callback::Status status,
               std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno err = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            err = resp->errorCode;
          } else {
            err = dispatchStatusToRilErrno(status);
          }
          sendResponse(ctx, err, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponse(ctx, RIL_E_NO_MEMORY, nullptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}

void dispatchRilRequestGsmSmsBroadcastActivation(std::shared_ptr<SocketRequestContext> ctx,
                                                 Marshal& p) {
  QCRIL_LOG_DEBUG("Dispatching RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION");
  bool activate = false;
  uint32_t in;
  Marshal::Result res = p.read(in);
  activate = (in == 0);
  if (res == Marshal::Result::SUCCESS) {
    auto msg = std::make_shared<RilRequestGsmSmsBroadcastActivateMessage>(ctx, activate);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([ctx]([[maybe_unused]] std::shared_ptr<Message> msg,
                 Message::Callback::Status status,
                 std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno err = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              err = resp->errorCode;
            } else {
              err = dispatchStatusToRilErrno(status);
            }
            sendResponse(ctx, err, nullptr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      QCRIL_LOG_ERROR("Unable to create message");
      sendResponse(ctx, RIL_E_NO_MEMORY, nullptr);
    }
  } else {
    QCRIL_LOG_ERROR("Unable to read from Marshal");
    sendResponse(ctx, RIL_E_INTERNAL_ERR, nullptr);
  }

}

}  // namespace api
}  // namespace socket
}  // namespace ril
