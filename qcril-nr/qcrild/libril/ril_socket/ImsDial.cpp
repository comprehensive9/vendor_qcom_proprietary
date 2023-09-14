/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/QcRilRequestImsDialMessage.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

static std::string getExtra(std::string key, char** extras, uint32_t length) {
    if (key.empty()) return "";
    if (!extras) return "";

    std::string value;
    for (size_t i = 0; i < length; i++) {
        if (extras[i] == nullptr) continue;
        std::string extra = extras[i];
        if (extra.find(key) != std::string::npos &&
                extra.find("=") != std::string::npos) {
            value = extra.substr(extra.find("=") + 1);
        }
    }
    return value;
}

void dispatchImsDial(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  RIL_IMS_Dial dial{};
  do {
    auto msg = std::make_shared<QcRilRequestImsDialMessage>(context);
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
    }

    if (!p.dataAvail()) {
        QCRIL_LOG_ERROR("Empty data");
        sendFailure = true;
        break;
    }
    p.read<RIL_IMS_Dial>(dial);

    // Set parameters
    if (dial.address != nullptr) {
        msg->setAddress(dial.address);
    } else if (!dial.hasIsConferenceUri || !dial.isConferenceUri) {
         QCRIL_LOG_ERROR("msg is nullptr");
         sendFailure = true;
         break;
    }
    if (dial.clirMode != INT32_MAX) {
      msg->setClir(dial.clirMode);
    }
    if (dial.hasCallDetails) {
      msg->setCallType(convertCallType(dial.callDetails.callType));
      msg->setCallDomain(convertCallDomain(dial.callDetails.callDomain));
      msg->setRttMode(convertRttMode(dial.callDetails.rttMode));

      if (dial.callDetails.extrasLength) {
        std::string displayText = getExtra("DisplayText",
                dial.callDetails.extras, dial.callDetails.extrasLength);
        if (!displayText.empty()) {
          msg->setDisplayText(displayText);
        }
        std::string retryCallFailReason = getExtra("RetryCallFailReason",
                dial.callDetails.extras, dial.callDetails.extrasLength);
        if (!retryCallFailReason.empty()) {
          msg->setRetryCallFailReason(static_cast<qcril::interfaces::CallFailCause>
                (std::stoi(retryCallFailReason)));
        }
        std::string retryCallFailMode = getExtra("RetryCallFailRadioTech",
                dial.callDetails.extras, dial.callDetails.extrasLength);
        if (!retryCallFailMode.empty()) {
          msg->setRetryCallFailMode(static_cast<RIL_RadioTechnology>(
                std::stoi(retryCallFailMode)));
        }
      }
    }

    if (dial.hasIsConferenceUri) {
      msg->setIsConferenceUri(dial.isConferenceUri);
    }
    if (dial.hasIsCallPull) {
      msg->setIsCallPull(dial.isCallPull);
    }
    if (dial.hasIsEncrypted) {
      msg->setIsEncrypted(dial.isEncrypted);
    }
    if (dial.multiLineInfo.msisdn != nullptr) {
      msg->setOriginatingNumber(dial.multiLineInfo.msisdn);
      msg->setIsSecondary(
          (dial.multiLineInfo.lineType == RIL_IMS_LINE_TYPE_SECONDARY));
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          QCRIL_LOG_DEBUG("dispatchImsDial resp: errorCode=%d", errorCode);
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
  p.release(dial);

  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
