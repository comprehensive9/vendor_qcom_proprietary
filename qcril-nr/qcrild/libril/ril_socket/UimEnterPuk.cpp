/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimEnterSimPukRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void dispatchSimEnterPukReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Enter Sim PUK request.");

    char** params;
    size_t sz = 0;
    p.readAndAlloc(params, sz);
    if (sz < 3) {
        QCRIL_LOG_ERROR("Not enough items");
        return;
    }
    if(params[0] && params[1] && params[2]) {
        std::string puk    = std::string((char*)params[0]);
        std::string newPin = std::string((char*)params[1]);
        std::string aid    = std::string((char*)params[2]);
        auto msg = std::make_shared<UimEnterSimPukRequestMsg>(RIL_UIM_SIM_PUK1, puk, newPin, aid);

        if (msg != nullptr) {

            GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_UIM_SIM_PIN_Response> resp) -> void {

                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

                if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {

                  errorCode = static_cast<RIL_Errno>(resp->err);

                  auto p = std::make_shared<Marshal>();
                  if(p && resp)
                  {
                    p->write(resp->no_of_retries);
                  }
                  QCRIL_LOG_INFO("Enter Puk reponse No of retries %d",resp->no_of_retries);
                  QCRIL_LOG_INFO("Response for Enter puk request %d",errorCode);
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
          p.release(params,sz);
      }
      else {
          sendResponse(context,RIL_E_INVALID_ARGUMENTS,nullptr);
      }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
