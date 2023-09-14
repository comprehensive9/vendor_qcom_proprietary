/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimSIMIORequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#include <interfaces/uim/UimGetAtrRequestMsg.h>

#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

static void socketToInternal(RIL_UIM_SIM_IO &out, RIL_SIM_IO_v6 &in) {
    out.command = in.command;
    out.fileid = in.fileid;
    if(in.path) {
        out.path = in.path;
    }
    else {
        out.path = "";
    }
    out.p1 = in.p1;
    out.p2 = in.p2;
    out.p3 = in.p3;
    if(in.data) {
        out.data = in.data;
    }
    else {
        out.data = "";
    }
    if(in.pin2) {
        out.pin2 = in.pin2;
    }
    else {
        out.pin2 = "";
    }
    if(in.aidPtr) {
        out.aidPtr = in.aidPtr;
    }
    else {
        out.aidPtr = "";
    }
}

void dispatchSimIOReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching SIM IO request.");

    RIL_SIM_IO_v6 params;
    p.read(params);
    int valid_arg = 0;
    RIL_UIM_SIM_IO simIo{};
    if((params.command == RIL_UIM_CMD_UPDATE_BINARY  || params.command == RIL_UIM_CMD_UPDATE_RECORD) &&
            (params.path && params.data && params.aidPtr)) {
        valid_arg=1;
    }
    /* check for read binary or read record or get response
     *  aidPtr is mandatory and it must be non null
     */
    else if((params.command == RIL_UIM_CMD_READ_BINARY || params.command == RIL_UIM_CMD_READ_RECORD ||
                params.command == RIL_UIM_CMD_GET_RESPONSE) && (params.path && params.aidPtr)) {
        valid_arg = 1;
    }
    /* check for sim_cmd_status command
     *  file_path mandatory and it must ne non null
     */
    else if((params.command == RIL_UIM_CMD_STATUS) && (params.path)) {
        valid_arg =1;
    }
    if(valid_arg)
    {
        socketToInternal(simIo, params);
        auto msg = std::make_shared<UimSIMIORequestMsg>(qmi_ril_get_process_instance_id(), simIo);
        if (msg != nullptr) {

            GenericCallback<RIL_UIM_SIM_IO_Response> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_UIM_SIM_IO_Response> resp) -> void {

                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

                if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {

                  errorCode = static_cast<RIL_Errno>(resp->err);

                  auto p = std::make_shared<Marshal>();
                  QCRIL_LOG_INFO("sim io response errorcode  %d",errorCode);
                  QCRIL_LOG_INFO("sim io response sw1  %d",resp->sw1);
                  QCRIL_LOG_INFO("sim io  response sw2  %d",resp->sw2);
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
      }
      else {
          sendResponse(context,RIL_E_INVALID_ARGUMENTS,nullptr);
      }
      p.release(params);
}
void dispatchSimGetAtr(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching SIM GET_ATR request.");
    uint8_t slot = 0;
    p.read(slot);
    auto msg = std::make_shared<UimGetAtrRequestMsg>(slot);
    if (msg != nullptr) {
        GenericCallback<UimAtrRspParam> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<UimAtrRspParam> resp) -> void {
                RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                std::shared_ptr<Marshal> p = nullptr;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        if (p->write(resp->atr) == Marshal::Result::SUCCESS) {
                            errorCode = static_cast<RIL_Errno>(resp->err);
                        }
                    }
                }
                sendResponse(context, errorCode, p);
            }
        );
        msg->setCallback(&cb);
        msg->dispatch();
    }
}

void dispatchSimTransmitApduChannel(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL request.");
    uint8_t slot = 0;
    p.read(slot);
    auto msg = std::make_shared<UimGetAtrRequestMsg>(slot);
    if (msg != nullptr) {
        GenericCallback<UimAtrRspParam> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<UimAtrRspParam> resp) -> void {
                RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                std::shared_ptr<Marshal> p = nullptr;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        if (p->write(resp->atr) == Marshal::Result::SUCCESS) {
                            errorCode = static_cast<RIL_Errno>(resp->err);
                        }
                    }
                }
                sendResponse(context, errorCode, p);
            }
        );
        msg->setCallback(&cb);
        msg->dispatch();
    }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
