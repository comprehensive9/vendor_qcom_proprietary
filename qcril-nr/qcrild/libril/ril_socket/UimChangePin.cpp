/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimChangeSimPinRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void dispatchSimChangePinReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Sim Pin Change request.");

    char** params;
    size_t len = 0;
    p.readAndAlloc(params, len);
    if (len < 3) {
        QCRIL_LOG_ERROR("Not enough parameters");
        return;
    }
    if(params[0] && params[1] && params[2]) {
        std::string oldPin = std::string((char*)params[0]);
        std::string newPin = std::string((char*)params[1]);
        std::string aid    = std::string((char*)params[2]);
        auto msg = std::make_shared<UimChangeSimPinRequestMsg>(RIL_UIM_SIM_PIN1, newPin, oldPin, aid);
        if (msg != nullptr ) {

            GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_UIM_SIM_PIN_Response> resp) -> void {

                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

                if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {

                  errorCode = (RIL_Errno)resp->err;

                  auto p = std::make_shared<Marshal>();
                  if(p && resp)
                  {
                    p->write(resp->no_of_retries);
                  }
                  QCRIL_LOG_INFO("Change Pin reponse No of retries %d",resp->no_of_retries);
                  QCRIL_LOG_INFO("Response for Change pin request %d",errorCode);
                  sendResponse(context, errorCode, p);
                } else {
                sendResponse(context, errorCode, nullptr);
                }
            });
            msg->setCallback(&cb);
            msg->dispatch();
        } else {
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
          }
     p.release(params,len);
    }
    else {
        sendResponse(context, RIL_E_INVALID_ARGUMENTS, nullptr);
    }
}
}  // namespace api
}  // namespace socket
}  // namespace ril
