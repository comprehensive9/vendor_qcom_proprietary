/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimSIMOpenChannelRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#include <marshal/SimOpenChannelRequest.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void dispatchSimOpenChannelReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Sim Open Channel request.");

    RIL_OpenChannelParams params;
    p.read(params);
    if(params.aidPtr) {
        QCRIL_LOG_INFO("aid ptr for open channel %s ",params.aidPtr);
        QCRIL_LOG_INFO("p2 for open channel %d ",params.p2);
        auto msg = std::make_shared<UimSIMOpenChannelRequestMsg>(params.aidPtr, params.p2);
        if (msg != nullptr) {

            GenericCallback<RIL_UIM_OpenChannelResponse> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_UIM_OpenChannelResponse> resp) -> void {

                RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                std::shared_ptr<Marshal> p = nullptr;

                if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        std::vector<int> sent{};
                        sent.push_back(resp->channel_id);
                        for(int sr: resp->selectResponse) {
                            sent.push_back(sr);
                        }
                        if (p->write(sent) == Marshal::Result::SUCCESS) {
                            errorCode = static_cast<RIL_Errno>(resp->err);
                        }
                    } else {
                        errorCode = RIL_E_NO_MEMORY;
                    }
                } else {
                    errorCode = RIL_E_INVALID_RESPONSE;
                }
                QCRIL_LOG_INFO("Response code for Open channel Req is %d",(RIL_Errno)errorCode);
                sendResponse(context, errorCode, p);
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
