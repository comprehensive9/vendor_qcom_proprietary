/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>

#include <interfaces/nas/RilRequestGetVoiceRadioTechMessage.h>
#include <interfaces/nas/nas_types.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchVoiceRadioTech(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<RilRequestGetVoiceRadioTechMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          std::shared_ptr<qcril::interfaces::RilGetVoiceTechResult_t> rilVoiceRadioTechResult;
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            int voiceRadioTech = RADIO_TECH_IS95A;
            errorCode = resp->errorCode;
            rilVoiceRadioTechResult =
                std::static_pointer_cast<qcril::interfaces::RilGetVoiceTechResult_t>(resp->data);
            if (rilVoiceRadioTechResult != nullptr) {
              voiceRadioTech = rilVoiceRadioTechResult->rat;
            }
            auto p = std::make_shared<Marshal>();
            p->write(static_cast<int>(voiceRadioTech));
            sendResponse(context, errorCode, p);
          } else {
            sendResponse(context, errorCode, nullptr);
          }
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
