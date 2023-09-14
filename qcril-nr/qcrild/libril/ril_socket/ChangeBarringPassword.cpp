/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestSetCallBarringPasswordMessage.h>
#include <Marshal.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

qcril::interfaces::FacilityType convertFacilityType(std::string in) {
  if (in == "AO") {
    return qcril::interfaces::FacilityType::BAOC;
  } else if (in == "OI") {
    return qcril::interfaces::FacilityType::BAOIC;
  } else if (in == "OX") {
    return qcril::interfaces::FacilityType::BAOICxH;
  } else if (in == "AI") {
    return qcril::interfaces::FacilityType::BAIC;
  } else if (in == "IR") {
    return qcril::interfaces::FacilityType::BAICr;
  } else if (in == "AB") {
    return qcril::interfaces::FacilityType::BA_ALL;
  } else if (in == "AG") {
    return qcril::interfaces::FacilityType::BA_MO;
  } else if (in == "AC") {
    return qcril::interfaces::FacilityType::BA_MT;
  }
  return qcril::interfaces::FacilityType::UNKNOWN;
}

void dispatchChangeBarringPassword(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetCallBarringPasswordMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Data not available in marshal");
      sendFailure = true;
      break;
    }
    std::string facility;
    p.read(facility);
    std::string oldPassword;
    p.read(oldPassword);
    std::string newPassword;
    p.read(newPassword);
    if (!facility.empty()) {
      msg->setFacilityType(convertFacilityType(facility));
    } else {
      QCRIL_LOG_ERROR("invalid parameter: facility");
      sendFailure = true;
      break;
    }
    if (!oldPassword.empty()) {
      msg->setOldPassword(oldPassword);
    }
    if (!newPassword.empty()) {
      msg->setNewPassword(newPassword);
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
  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
