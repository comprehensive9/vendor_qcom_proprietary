/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <framework/GenericCallback.h>
#include <ril_socket_api.h>
#include <interfaces/uim/UimEnterDePersoRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#include <Marshal.h>
#include <framework/Log.h>
#include <telephony/ril.h>

namespace ril {
namespace socket {
namespace api {

/*
 * RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION
 */
void dispatchEnterNetworkDepersonalization(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Enter Network Depersonalization request.");
    char *args[1]{};
    if (p.read<char *>(args) == Marshal::Result::SUCCESS) {
        std::string netPin = args[0];
        auto msg = std::make_shared<UimEnterDePersoRequestMsg>(netPin, RIL_UIM_PERSOSUBSTATE_SIM_NETWORK);
        if (msg) {
            GenericCallback<RIL_UIM_PersoResponse> cb(
                    [context](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_PersoResponse> responseDataPtr) -> void {
                        RIL_Errno err = RIL_E_INTERNAL_ERR;
                        std::shared_ptr<Marshal> m = nullptr;
                        if (responseDataPtr &&
                            status == Message::Callback::Status::SUCCESS) {
                            int retries = -1;
                            m = std::make_shared<Marshal>();
                            err = static_cast<RIL_Errno>(responseDataPtr->err);
                            retries = responseDataPtr->no_of_retries;
                            QCRIL_LOG_INFO("No of retries %d",retries);
                            QCRIL_LOG_INFO("Error Code is  %d",err);
                            m->write(retries);
                        }
                        sendResponse(context, err, m);
                    }
            );
            msg->setCallback(&cb);
            msg->dispatch();
        }
    } else {
        sendResponse(context, RIL_E_INVALID_ARGUMENTS, nullptr);
    }
}

}
}
}
