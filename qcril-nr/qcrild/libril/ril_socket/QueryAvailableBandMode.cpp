/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/nas/RilRequestSetBandModeMessage.h>
#include <interfaces/nas/RilRequestQueryAvailBandModeMessage.h>
#include <interfaces/nas/nas_types.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchSetBandMode(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  bool sendFailure = false;
  do {
    int bandMode;
    p.read(bandMode);
    auto msg = std::make_shared<RilRequestSetBandModeMessage>(context, bandMode);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
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
  QCRIL_LOG_FUNC_RETURN();
}

void dispatchQueryAvailableBandMode(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<RilRequestQueryAvailBandModeMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilQueryAvailBandsResult_t> rilBandResult;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            rilBandResult =
                std::static_pointer_cast<qcril::interfaces::RilQueryAvailBandsResult_t>(resp->data);
          }
          auto p = std::make_shared<Marshal>();
          uint32_t numInts = 0;
          if (p && rilBandResult) {
            numInts = rilBandResult->bandList.size();
            p->write(numInts);
            for (uint32_t i = 0; i < numInts; i++) {
              p->write(rilBandResult->bandList[i]);
            }
          }
          sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
