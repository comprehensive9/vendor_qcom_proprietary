/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimSIMCloseChannelRequestMsg.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void dispatchSimCloseChannelReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Sim Close Channel request.");

    int params[1];
    Marshal::Result res = p.read(params);
    if (res == Marshal::Result::SUCCESS) {
        QCRIL_LOG_INFO("Session Id for close channel req %d",params[0]);
        auto msg = std::make_shared<UimSIMCloseChannelRequestMsg>((int32_t)params[0]);
        if (msg != nullptr) {

            GenericCallback<RIL_UIM_Errno> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_UIM_Errno> resp) -> void {

                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

                if(status == Message::Callback::Status::SUCCESS && resp) {
                    errorCode = (RIL_Errno)*resp.get();
                }
                QCRIL_LOG_INFO("Response code for close channel req %d",(RIL_Errno)errorCode);
                sendResponse(context, errorCode, nullptr);
            });
            msg->setCallback(&cb);
            msg->dispatch();
        } else {
          sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        }
    } else {
        sendResponse(context, RIL_E_INVALID_ARGUMENTS, nullptr);
    }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
