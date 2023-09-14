/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <framework/GenericCallback.h>
#include <ril_socket_api.h>
#include <interfaces/gstk/GstkSendEnvelopeRequestMsg.h>
#include <interfaces/gstk/GstkSendTerminalResponseRequestMsg.h>
#include <interfaces/gstk/GstkSendSetupCallResponseRequestMsg.h>
#include <interfaces/gstk/GstkReportSTKIsRunningMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#include <Marshal.h>
#include <framework/Log.h>
#include <telephony/ril.h>

namespace ril {
namespace socket {
namespace api {

/*
 * RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND
 */
void dispatchStkSendEnvelopeCommand(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
	QCRIL_LOG_INFO("Disaptching StkSendEnvelopeCommand");
    char *args;
    size_t sz = 0;
    if (p.readAndAlloc(args,sz) == Marshal::Result::SUCCESS && args) {
        std::string command = args;
        delete[] args;
        auto msg = std::make_shared<GstkSendEnvelopeRequestMsg>(static_cast<uint32_t>(context->getRequestToken()), command);
        if (msg) {
            GenericCallback<RIL_GSTK_EnvelopeResponse> cb(
                    [context](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<RIL_GSTK_EnvelopeResponse> responseDataPtr) -> void {
                        RIL_Errno err = RIL_E_INTERNAL_ERR;
                        std::shared_ptr<Marshal> m = nullptr;
                        if (responseDataPtr &&
                            status == Message::Callback::Status::SUCCESS) {
                            m = std::make_shared<Marshal>();
                            if (m) {
                                err = static_cast<RIL_Errno>(responseDataPtr->err);
                                m->write(responseDataPtr->rsp);
                            } else {
                                err = RIL_E_NO_MEMORY;
                            }
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

/*
 * RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE
 */
void dispatchStkSendTerminalResponse(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
	 QCRIL_LOG_INFO("Disaptching StkSendTerminalResponse");
    std::string terminalResponse{};
    if (p.read(terminalResponse) == Marshal::Result::SUCCESS) {
        auto msg = std::make_shared<GstkSendTerminalResponseRequestMsg>(static_cast<uint32_t>(context->getRequestToken()), terminalResponse);
        if (msg) {
            GenericCallback<RIL_GSTK_Errno> cb(
                    [context](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<RIL_GSTK_Errno> responseDataPtr) -> void {
                        RIL_Errno err = RIL_E_INTERNAL_ERR;
                        std::shared_ptr<Marshal> m = nullptr;
                        if (responseDataPtr &&
                            status == Message::Callback::Status::SUCCESS) {
                            err = static_cast<RIL_Errno>(*responseDataPtr);
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

/*
 * RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM
 */
void dispatchStkHandleCallSetupRequestedFromSim(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
	QCRIL_LOG_INFO("Disaptching StkHandleCallSetupRequestedFromSim");
    int args[1];
    if (p.read(args) == Marshal::Result::SUCCESS) {
        bool accept = args[0] != 0;
        auto msg = std::make_shared<GstkSendSetupCallResponseRequestMsg>(accept);
        if (msg) {
            GenericCallback<RIL_GSTK_Errno> cb(
                    [context](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<RIL_GSTK_Errno> responseDataPtr) -> void {
                        RIL_Errno err = RIL_E_INTERNAL_ERR;
                        std::shared_ptr<Marshal> m = nullptr;
                        if (responseDataPtr &&
                            status == Message::Callback::Status::SUCCESS) {
                            err = static_cast<RIL_Errno>(*responseDataPtr);
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

/*
 * RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING
 */
void dispatchStkServiceIsRunning(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
	 QCRIL_LOG_INFO("Disaptching Stk Service is running");
    auto msg = std::make_shared<GstkReportSTKIsRunningMsg>();
    if (msg) {
        GenericCallback<RIL_GSTK_Errno> cb(
                [context](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                    std::shared_ptr<RIL_GSTK_Errno> responseDataPtr) -> void {
                    RIL_Errno err = RIL_E_INTERNAL_ERR;
                    std::shared_ptr<Marshal> m = nullptr;
                    if (responseDataPtr &&
                        status == Message::Callback::Status::SUCCESS) {
                        err = static_cast<RIL_Errno>(*responseDataPtr);
                    }
                    sendResponse(context, err, m);
                }
        );
        msg->setCallback(&cb);
        msg->dispatch();
    }
}

}
}
}
