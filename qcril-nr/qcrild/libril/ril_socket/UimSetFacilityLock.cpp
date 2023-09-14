/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimSetFacilityLockRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>
#include<stdlib.h>
namespace ril {
namespace socket {
namespace api {

void dispatchSimSetFacilityLockReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Set Sim Facility Lock request.");

    char** params;
    size_t sz = 0;
    size_t valid_arg=0;
    int status=0;
    p.readAndAlloc(params, sz);
    if (sz < 5) {
        QCRIL_LOG_ERROR("Not enough items");
        return;
    }
    /* params[2] - password and params[4] aid mandatory for PIN */
    if(((strcmp(params[0],"SC") == 0)) && params[2] && params[4])
    {
       valid_arg = 1;
    }
    /* params[4] mandatory for FDN*/
    else if((strcmp(params[0],"SC") != 0) && params[4]) {
       valid_arg  = 1;
    }
    
    if(valid_arg) {
        std::string facility = std::string((char*)params[0]);
        status = strtol(params[1],NULL,10);
        bool lockStatus = (bool)status;
        std::string password;
        if(params[2]) {
            password = std::string((char*)params[2]);
        }
        else {
            password = "";
        }
        std::string aid = std::string((char*)params[4]);
        QCRIL_LOG_INFO("facility %s",facility.c_str());
        QCRIL_LOG_INFO("aid %s",aid.c_str());
        QCRIL_LOG_INFO("lockstatus is %d",lockStatus);
        auto msg = std::make_shared<UimSetFacilityLockRequestMsg>(password, aid,
                                    lockStatus, (facility == "SC" ? PIN1 : FDN));
        if (msg != nullptr) {

            GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_UIM_SIM_PIN_Response> resp) -> void {

                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

                if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {

                  errorCode = static_cast<RIL_Errno>(resp->err);

                  auto p = std::make_shared<Marshal>();
                  QCRIL_LOG_INFO("set facility lock reponse No of retries %d",resp->no_of_retries);
                  QCRIL_LOG_INFO("set facliity lock error code %d",errorCode);
                  if(p && resp)
                  {
                    p->write(resp->no_of_retries);
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
        p.release(params,sz);
    }
    else {
        sendResponse(context,RIL_E_INVALID_ARGUMENTS,nullptr);
    }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
