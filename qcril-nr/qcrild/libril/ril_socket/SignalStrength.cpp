/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <marshal/SignalStrength.h>
#include <ril_socket_api.h>
#include <ril_utils.h>

#include <interfaces/nas/RilRequestGetSignalStrengthMessage.h>
#include <interfaces/nas/nas_types.h>
#include <interfaces/nas/NasSetSignalStrengthCriteria.h>

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchSignalStrength(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<RilRequestGetSignalStrengthMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload>
    cb([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                 std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
      RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
      std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilGetSignalStrengthResult;
      if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
        errorCode = resp->errorCode;
        rilGetSignalStrengthResult =
            std::static_pointer_cast<qcril::interfaces::RilGetSignalStrengthResult_t>(resp->data);
      }
      auto p = std::make_shared<Marshal>();
      if (p && rilGetSignalStrengthResult) {
        p->write(rilGetSignalStrengthResult->respData);
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

void setSignalStrengthReportingCriteria(std::shared_ptr<SocketRequestContext> context,
        const Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();

    RIL_SignalStrengthReportingCriteria criteria {};
    if (p.read(criteria) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_Errno::RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    if (!ril::socket::utils::checkThresholdAndHysteresis(criteria.hysteresisDb,
            criteria.thresholds, criteria.thresholdsLength)) {
        sendResponse(context, RIL_Errno::RIL_E_INVALID_ARGUMENTS, nullptr);
        return;
    }

    std::vector<int32_t> thresholds;
    for (uint32_t i = 0; i < criteria.thresholdsLength; i++) {
        thresholds.push_back(criteria.thresholds[i]);
    }

    std::vector<SignalStrengthCriteriaEntry> entries;
    entries.push_back(
        {
            criteria.accessNetwork,
            criteria.signalStrengthParameter,
            criteria.hysteresisMs,
            criteria.hysteresisDb,
            std::move(thresholds),
            static_cast<bool>(criteria.isEnabled),
        }
    );

    std::shared_ptr<NasSetSignalStrengthCriteria> msg =
        std::make_shared<NasSetSignalStrengthCriteria>(context, std::move(entries));

    if (msg) {
        GenericCallback<RIL_Errno> cb(
            [context] (std::shared_ptr<Message> msg,
                       Message::Callback::Status status,
                       std::shared_ptr<RIL_Errno> rsp) -> void {
                RIL_Errno e = RIL_Errno::RIL_E_INTERNAL_ERR;
                if (status == Message::Callback::Status::SUCCESS && rsp) {
                    e = *rsp;
                }
                sendResponse(context, e, nullptr);
            }
        );
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_Errno::RIL_E_NO_MEMORY, nullptr);
    }

    if (criteria.thresholds) {
        delete[] criteria.thresholds;
        criteria.thresholds = nullptr;
        criteria.thresholdsLength = 0;
    }

    QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
