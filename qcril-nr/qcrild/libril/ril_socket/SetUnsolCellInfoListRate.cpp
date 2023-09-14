/******************************************************************************
#  Copyright (c) 2020 - 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/nas/NasPhysChanConfigMessage.h>
#include <interfaces/nas/NasEnablePhysChanConfigReporting.h>
#include <interfaces/nas/RilRequestSetUnsolRespFilterMessage.h>
#include <interfaces/nas/RilRequestSetUnsolCellInfoListRateMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchSetUnsolCellInfoListRate(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("cell info list rate is empty");
      sendFailure = true;
      break;
    }
    int32_t rate;
    p.read(rate);

    auto msg = std::make_shared<RilRequestSetUnsolCellInfoListRateMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    msg->setRate(rate);

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

/*
 * RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER
 */
void dispatchSetIndicationFilter(std::shared_ptr<SocketRequestContext> ctx, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();
    int32_t int32Filter = 0;
    if (p.read(int32Filter) == Marshal::Result::SUCCESS) {
        auto msg = std::make_shared<NasEnablePhysChanConfigReporting>(
                (int32Filter & RIL_UR_PHYSICAL_CHANNEL_CONFIG) != 0, ctx,
                [ctx, int32Filter]
                (std::shared_ptr<Message> msg, Message::Callback::Status status,
                 std::shared_ptr<qcril::interfaces::NasSettingResultCode> rc) -> void {

                QCRIL_LOG_DEBUG("Callback for NasEnablePhysChanConfigReporting. rc: %d",
                    rc ? static_cast<int>(*rc):-1);
                if (*rc != qcril::interfaces::NasSettingResultCode::SUCCESS) {
                    QCRIL_LOG_ERROR("Error enabling ChanConfigReporting: %d", static_cast<int>(*rc));
                } else {
                    std::shared_ptr<NasPhysChanConfigReportingStatus> phyChanConfigReportingStatusMsg =
                        std::make_shared<NasPhysChanConfigReportingStatus>(
                            int32Filter & RIL_UR_PHYSICAL_CHANNEL_CONFIG);
                    if (phyChanConfigReportingStatusMsg) {
                        phyChanConfigReportingStatusMsg->broadcast();
                    } else {
                        QCRIL_LOG_ERROR("Failed to create message NasPhysChanConfigReportingStatus.");
                    }
                }

                int32_t filter = int32Filter &
                    (RIL_UR_SIGNAL_STRENGTH
                     | RIL_UR_FULL_NETWORK_STATE
                     | RIL_UR_DATA_CALL_DORMANCY_CHANGED
                     | RIL_UR_LINK_CAPACITY_ESTIMATE);

                auto filtermsg =
                    std::make_shared<RilRequestSetUnsolRespFilterMessage>(ctx, filter);
                if (filtermsg != nullptr) {
                    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
                        [ctx](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                                errorCode = resp->errorCode;
                            }
                            QCRIL_LOG_DEBUG("setIndicationFilterResponse: error=%d", errorCode);
                            sendResponse(ctx, errorCode, nullptr);
                        });
                    filtermsg->setCallback(&cb);
                    filtermsg->dispatch();
                } else {
                    sendResponse(ctx, RIL_E_NO_MEMORY, nullptr);
                }
        });
        if (msg == nullptr) {
            QCRIL_LOG_ERROR("NasEnablePhysChanConfigReporting msg is nullptr");
            sendResponse(ctx, RIL_E_NO_MEMORY, nullptr);
        } else {
            msg->dispatch();
        }
    } else {
        sendResponse(ctx, RIL_E_INVALID_ARGUMENTS, nullptr);
    }
    QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
