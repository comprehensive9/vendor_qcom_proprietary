/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/gstk/GstkSendEnvelopeRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

// TODO - Find the right UIM module message not found in ril_service.cpp
void dispatchSimStkSendEnvelopeWithStatusReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Sim Stk Send Envelope Status request.");
    std::string command;
    if (p.read(command) == Marshal::Result::SUCCESS) { 
        auto msg = std::make_shared<GstkSendEnvelopeRequestMsg>(static_cast<uint32_t>(context->getRequestToken()),command);
        if (msg != nullptr) {

            GenericCallback<RIL_GSTK_EnvelopeResponse> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_GSTK_EnvelopeResponse> resp) -> void {
                    (void) msg;
                    RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                    std::shared_ptr<Marshal> p = nullptr;
                    if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                        p = std::make_shared<Marshal>();
                        if (p) {
                            RIL_SIM_IO_Response rsp{};
                            rsp.simResponse = const_cast<char *>(resp->rsp.c_str());
                            if (p->write(rsp) == Marshal::Result::SUCCESS) {
                                errorCode = static_cast<RIL_Errno>(resp->err);
                            }
                        } else {
                            errorCode = RIL_E_NO_MEMORY;
                        }
                    }
                    sendResponse(context, errorCode, p);
                });
            msg->setCallback(&cb);
            msg->dispatch();
        }
    } else {
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
    }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
