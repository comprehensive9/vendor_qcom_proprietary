/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/QcRilRequestGetColrMessage.h>
#include <interfaces/voice/QcRilRequestSetColrMessage.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsGetColr(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();
    auto msg = std::make_shared<QcRilRequestGetColrMessage>(context);
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }
    msg->setIsImsRequest(true);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<Marshal> p = nullptr;
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                errorCode = resp->errorCode;
                auto colrInfo = std::static_pointer_cast<qcril::interfaces::ColrInfo>(resp->data);
                if (colrInfo) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        RIL_IMS_ColrInfo rilColr{};
                        ril::socket::utils::convertToSocket(rilColr, colrInfo);
                        p->write(rilColr);
                        ril::socket::utils::release(rilColr);
                    } else {
                        errorCode = RIL_E_NO_MEMORY;
                    }
                }
            }
            QCRIL_LOG_DEBUG("dispatchImsGetColr resp: errorCode=%d", errorCode);
            sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();

    QCRIL_LOG_FUNC_RETURN();
}

void dispatchImsSetColr(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();
    auto msg = std::make_shared<QcRilRequestSetColrMessage>(context);
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }
    msg->setIsImsRequest(true);

    if (!p.dataAvail()) {
        QCRIL_LOG_ERROR("Empty data");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }
    RIL_IMS_ColrInfo colr{};
    p.read(colr);
    if (colr.presentation != RIL_IMS_IP_PRESENTATION_INVALID) {
        msg->setPresentation(convertIpPresentation(colr.presentation));
    }
    p.release(colr);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<Marshal> p = nullptr;
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                errorCode = resp->errorCode;
                auto errorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
                if (errorDetails) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        RIL_IMS_SipErrorInfo rilErrInfo = {};
                        ril::socket::utils::convertToSocket(rilErrInfo, *errorDetails);
                        p->write(rilErrInfo);
                        ril::socket::utils::release(rilErrInfo);
                    } else {
                        errorCode = RIL_E_NO_MEMORY;
                    }
                }
            }
            QCRIL_LOG_DEBUG("dispatchImsSetColr resp: errorCode=%d", errorCode);
            sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
