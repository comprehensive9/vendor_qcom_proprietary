/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/ims/ims.h>
#include <interfaces/ims/QcRilRequestImsQueryVirtualLineInfo.h>
#include <interfaces/ims/QcRilRequestImsRegisterMultiIdentityMessage.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void dispatchImsRegisterMultiIdentityLines(
        std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();
    auto msg = std::make_shared<QcRilRequestImsRegisterMultiIdentityMessage>(context);
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }

    if (!p.dataAvail()) {
        QCRIL_LOG_ERROR("Empty data");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }
    RIL_IMS_MultiIdentityLineInfo* info = nullptr;
    size_t sz = 0;
    if (p.readAndAlloc(info, sz) != Marshal::Result::SUCCESS || !sz || !info) {
        QCRIL_LOG_ERROR("Failed to read data");
        sendResponse(context, RIL_E_GENERIC_FAILURE, nullptr);
        return;
    }
    std::vector<qcril::interfaces::MultiIdentityInfo> &out_lines = msg->getLineInfo();
    for (size_t i = 0; i < sz; i++) {
        qcril::interfaces::MultiIdentityInfo out_line{};
        ril::socket::utils::convertToSocket(out_line, info[i]);
        out_lines.push_back(out_line);
        if (info[i].msisdn) delete []info[i].msisdn;
    }
    delete []info;

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                errorCode = resp->errorCode;
            }
            QCRIL_LOG_DEBUG("dispatchImsRegisterMultiIdentityLines resp: errorCode=%d", errorCode);
            sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();

    QCRIL_LOG_FUNC_RETURN();
}

void dispatchImsQueryVirtualLineInfo(
        std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();

    if (!p.dataAvail()) {
        QCRIL_LOG_ERROR("Empty data");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }
    char* msisdn = nullptr;
    if (p.read(msisdn) != Marshal::Result::SUCCESS || !msisdn) {
        QCRIL_LOG_ERROR("Failed to read data");
        sendResponse(context, RIL_E_GENERIC_FAILURE, nullptr);
        return;
    }

    auto msg = std::make_shared<QcRilRequestImsQueryVirtualLineInfo>(
            context, msisdn);
    delete []msisdn;
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<Marshal> p = nullptr;
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                errorCode = resp->errorCode;
                auto info = std::static_pointer_cast<qcril::interfaces::VirtualLineInfo>(resp->data);
                if (info) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        RIL_IMS_QueryVirtualLineInfoResponse rilResp{};
                        ril::socket::utils::convertToSocket(rilResp, *info);
                        p->write(rilResp);
                        // free the memory
                        if (rilResp.msisdn) delete []rilResp.msisdn;
                        if ((rilResp.numLines > 0) && rilResp.virtualLines) {
                            for (int i = 0; i < rilResp.numLines; i++)
                                if (rilResp.virtualLines[i]) delete []rilResp.virtualLines[i];
                            delete []rilResp.virtualLines;
                        }
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
