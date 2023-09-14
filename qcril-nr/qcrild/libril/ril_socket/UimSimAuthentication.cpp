/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimSIMAuthenticationRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void dispatchSimAuthenticationReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Sim Authentication request.");

    RIL_SimAuthentication params;
    p.read(params);
    if(params.authData && params.aid ) {
       int32_t authContext = params.authContext;
       std::string authData = std::string(params.authData);
       std::string aid = std::string(params.aid);
       auto msg = std::make_shared<UimSIMAuthenticationRequestMsg>(authContext, authData, aid);
       if (msg != nullptr) {
           GenericCallback<RIL_UIM_SIM_IO_Response> cb(
               [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
               std::shared_ptr<RIL_UIM_SIM_IO_Response> resp) -> void {

               RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

               if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {

                 errorCode = static_cast<RIL_Errno>(resp->err);

                 auto p = std::make_shared<Marshal>();
                 QCRIL_LOG_INFO("sim authentication response errorcode  %d",errorCode);
                 QCRIL_LOG_INFO("sim authentication response sw1  %d",resp->sw1);
                 QCRIL_LOG_INFO("sim authentication response sw2  %d",resp->sw2);
                 if(p && resp)
                 {
                   RIL_SIM_IO_Response simIoPtr;
                   simIoPtr.sw1 = resp->sw1;
                   simIoPtr.sw2 = resp->sw2;
                   simIoPtr.simResponse = const_cast<char*>(resp->simResponse.c_str());
                   p->write(simIoPtr);
                 }
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
        p.release(params);
    }
    else {
        sendResponse(context,RIL_E_INVALID_ARGUMENTS,nullptr);
    }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
