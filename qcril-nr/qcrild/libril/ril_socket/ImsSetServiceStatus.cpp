/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/QcRilRequestImsSetServiceStatusMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsSetServiceStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  std::vector<RIL_IMS_ServiceStatusInfo> ssInfoList;
  do {
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }
    int32_t numSsInfo = 0;
    p.read(numSsInfo);
    for (int i = 0; i < numSsInfo; i++) {
      RIL_IMS_ServiceStatusInfo ss = {};
      p.read(ss);
      ssInfoList.push_back(ss);
    }
    auto msg = std::make_shared<QcRilRequestImsSetServiceStatusMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }

    for (const auto& ssInfo : ssInfoList) {
      auto& callType = ssInfo.callType;
      auto& networkMode = ssInfo.accTechStatus.networkMode;
      auto& status = ssInfo.accTechStatus.status;
      if ((callType == RIL_IMS_CALL_TYPE_VOICE) &&
          (networkMode == RADIO_TECH_LTE)) {
        msg->setVolteEnabled(status != RIL_IMS_STATUS_DISABLED);
      }
      if ((callType == RIL_IMS_CALL_TYPE_VOICE) &&
          ((networkMode == RADIO_TECH_IWLAN) ||
           (networkMode == RADIO_TECH_WIFI))) {
        msg->setWifiCallingEnabled(status != RIL_IMS_STATUS_DISABLED);
      }
      if (callType == RIL_IMS_CALL_TYPE_VT) {
        msg->setVideoTelephonyEnabled(status != RIL_IMS_STATUS_DISABLED);
      }
      if (callType == RIL_IMS_CALL_TYPE_UT) {
        msg->setUTEnabled(status != RIL_IMS_STATUS_DISABLED);
      }
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsSetServiceStatus:resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
  for (auto &ss : ssInfoList) {
    p.release(ss);
  }

  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
