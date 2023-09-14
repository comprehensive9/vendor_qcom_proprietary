/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimGetFacilityLockRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void dispatchSimQueryFacilityLockReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Sim Query Facility Lock request.");

    char** params;
    size_t sz = 0;
    p.readAndAlloc(params, sz);
    if (sz < 4) {
        QCRIL_LOG_ERROR("Not enough items");
        return;
    }
    if(params[0] && params[3]) {
        std::string facility = std::string((char*)params[0]);
    std::string password;
        if(params[1]) {
            password = std::string((char*)params[1]);
        }
        else {
            password = "";
        }
        std::string aid = std::string((char*)params[3]);
        auto msg = std::make_shared<UimGetFacilityLockRequestMsg>(password, aid, (facility == "SC" ? PIN1 : FDN));

        if (msg != nullptr) {

            GenericCallback<RIL_UIM_GetFacilityLockResponseMsg> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_UIM_GetFacilityLockResponseMsg> resp) -> void {

                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

                if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {

                    errorCode = static_cast<RIL_Errno>(resp->ril_err);

                    auto p = std::make_shared<Marshal>();
                    QCRIL_LOG_INFO("Response for query facilitry lock status %d",errorCode);
                    if(p && resp)
                    {
                        p->write(resp->lock_status);
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
