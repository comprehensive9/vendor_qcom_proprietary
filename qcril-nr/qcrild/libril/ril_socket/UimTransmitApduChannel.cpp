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

extern void socketToInternal(RIL_UIM_SIM_APDU &out, RIL_SIM_APDU &in);

void dispatchSimTransmitApduChannelReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Sim Transmit Channel Apdu request.");

    RIL_SIM_APDU params;
    p.read(params);
    RIL_UIM_SIM_APDU simApdu;
    socketToInternal(simApdu, params);
    auto msg = std::make_shared<UimTransmitAPDURequestMsg>(qmi_ril_get_process_instance_id(), false, true, simApdu);
    if (msg !=nullptr) {
        GenericCallback<RIL_UIM_SIM_IO_Response> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<RIL_UIM_SIM_IO_Response> resp) -> void {

            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

            if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {

              errorCode = static_cast<RIL_Errno>(resp->err);

              auto p = std::make_shared<Marshal>();
              if(p && resp)
              {
                RIL_SIM_IO_Response simIoResponse;
                simIoResponse.sw1 = resp->sw1;
                simIoResponse.sw2 = resp->sw2;
                simIoResponse.simResponse = const_cast<char*>(resp->simResponse.c_str());
                QCRIL_LOG_INFO("Statusword1 is %d",simIoResponse.sw1);
                QCRIL_LOG_INFO("statusword2 is %d",simIoResponse.sw2);
                QCRIL_LOG_INFO("error code  %d",errorCode);
                p->write(simIoResponse);
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

}  // namespace api
}  // namespace socket
}  // namespace ril
