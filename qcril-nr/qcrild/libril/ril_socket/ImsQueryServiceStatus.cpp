/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/QcRilRequestImsQueryServiceStatusMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsQueryServiceStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestImsQueryServiceStatusMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<Marshal> p;
          std::vector<RIL_IMS_ServiceStatusInfo> serviceStatusInfo;
          std::shared_ptr<qcril::interfaces::ServiceStatusInfoList> respData = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            respData =
                std::static_pointer_cast<qcril::interfaces::ServiceStatusInfoList>(resp->data);
            if (respData) {
              auto ssInfo = respData->getServiceStatusInfo();
              ril::socket::utils::convertToSocket(serviceStatusInfo, ssInfo);
              p = std::make_shared<Marshal>();
              int32_t numSsInfo = serviceStatusInfo.size();
              QCRIL_LOG_DEBUG("dispatchImsQueryServiceStatus: numSsInfo =%d", numSsInfo);
              if (numSsInfo) {
                if (p) {
                  p->write(numSsInfo);
                  for (auto ssInfo : serviceStatusInfo) {
                    QCRIL_LOG_DEBUG("dispatchImsQueryServiceStatus: ssInfo =0x%x", ssInfo);
                    p->write(ssInfo);
                  }
                }
              }
              ril::socket::utils::release(serviceStatusInfo);
            }
          }
          QCRIL_LOG_DEBUG("dispatchImsQueryServiceStatus:resp: errorCode=%d", errorCode);
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
