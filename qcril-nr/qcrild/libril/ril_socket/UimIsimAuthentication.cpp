/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimISIMAuthenticationRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void dispatchSimIsimAuthenticationReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching ISIM Authentication request.");

    std::string challenge;
    if (p.read(challenge) == Marshal::Result::SUCCESS) {

        auto msg = std::make_shared<UimISIMAuthenticationRequestMsg>(challenge);
        if (msg != nullptr) {

            GenericCallback<RIL_UIM_SIM_IO_Response> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_UIM_SIM_IO_Response> resp) -> void {

                RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                std::shared_ptr<Marshal> p = nullptr;

                if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = static_cast<RIL_Errno>(resp->err);
                    p = std::make_shared<Marshal>();

                    if(p && resp)
                    {
                        RIL_SIM_IO_Response simIoPtr;
                        simIoPtr.sw1 = resp->sw1;
                        simIoPtr.sw2 = resp->sw2;
                        simIoPtr.simResponse = const_cast<char*>(resp->simResponse.c_str());
                        QCRIL_LOG_INFO("Isim authentication response errorcode  %d",errorCode);
                        QCRIL_LOG_INFO("Isim authentication response sw1  %d",resp->sw1);
                        QCRIL_LOG_INFO("Isim authentication response sw2  %d",resp->sw2);

                        p->write(simIoPtr.simResponse);
                    }
                }
                sendResponse(context, errorCode, p);
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
