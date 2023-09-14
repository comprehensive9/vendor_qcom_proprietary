/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimTransmitAPDURequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void socketToInternal(RIL_UIM_SIM_APDU &out, RIL_SIM_APDU &in) {
    out.sessionid = in.sessionid;
    out.cla = in.cla;
    out.instruction = in.instruction;
    out.p1 = in.p1;
    out.p2 = in.p2;
    out.p3 = in.p3;
    if (in.data) {
        out.data = in.data;
    } else {
        out.data = "";
    }
}

void dispatchSimTransmitApduBasicReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Sim Transmit Basic Apdu request.");

    RIL_SIM_APDU params;
    p.read(params);
    RIL_UIM_SIM_APDU simApdu;
    socketToInternal(simApdu, params);
    auto msg = std::make_shared<UimTransmitAPDURequestMsg>(qmi_ril_get_process_instance_id(), true, true, simApdu);
    if (msg != nullptr) {

        GenericCallback<RIL_UIM_SIM_IO_Response> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<RIL_UIM_SIM_IO_Response> resp) -> void {

            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            std::shared_ptr<Marshal> p = nullptr;

            if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {

                p = std::make_shared<Marshal>();
                if(p)
                {
                    RIL_SIM_IO_Response simIoResponse;
                    simIoResponse.sw1 = resp->sw1;
                    simIoResponse.sw2 = resp->sw2;
                    simIoResponse.simResponse = const_cast<char*>(resp->simResponse.c_str());
                    errorCode = static_cast<RIL_Errno>(resp->err);
                    QCRIL_LOG_INFO("Statusword1 is %d",simIoResponse.sw1);
                    QCRIL_LOG_INFO("statusword2 is %d",simIoResponse.sw2);
                    QCRIL_LOG_INFO("error code  %d",errorCode);
                    p->write(simIoResponse);
                } else {
                    errorCode = RIL_E_NO_MEMORY;
                }
            }
            sendResponse(context, errorCode, p);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
          sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
    p.release(params);
}

}  // namespace api
}  // namespace socket
}  // namespace ril
