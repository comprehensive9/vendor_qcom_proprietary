/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <framework/GenericCallback.h>
#include <ril_socket_api.h>

#include <request/StartKeepAliveRequestMessage.h>
#include <Marshal.h>
#include <framework/Log.h>
#include <telephony/ril.h>

#ifndef RIL_FOR_MDM_LE
#include <utils/Log.h>
#else
#include <utils/Log2.h>
#endif

namespace ril {
namespace socket {
namespace api {

void dispatchStartKeepAlive(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching StartKeepAlive request.");
    using namespace rildata;
    int32_t serial = 0;
    RIL_KeepaliveRequest request = {};
    p.read(request);
    rildata::KeepaliveRequest_t data = {};
    data.type = static_cast<KeepaliveType_t>(request.type);
    int i = 0;
    string s = "";
    while(i < MAX_INADDR_LEN && request.sourceAddress[i] != '\0') {
        if (request.sourceAddress[i] == '.' || request.sourceAddress[i] == ':') {
            i++;
            try {
                data.sourceAddress.push_back(stoi(s));
            }
            catch(...){
                RLOGI("Destination Adrress::Only IPV4 address is supported");
                sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
                return;
            }
            s = "";
            continue;
        }
        s += request.sourceAddress[i];
        i++;
    }
    try {
        data.sourceAddress.push_back(stoi(s));
    }
    catch(...){
        RLOGI("Destination Adrress::Only IPV4 address is supported");
        sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
        return;
    }
    s = "";
    data.sourcePort = request.sourcePort;
    i = 0;
    while(i < MAX_INADDR_LEN && request.destinationAddress[i] != '\0') {
        if (request.destinationAddress[i] == '.' || request.destinationAddress[i] == ':') {
            i++;
            try {
                data.destinationAddress.push_back(stoi(s));
            }
            catch(...){
                RLOGI("Destination Adrress::Only IPV4 address is supported");
                sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
                return;
            }
            s = "";
            continue;
        }
        s += request.destinationAddress[i];
        i++;
    }
    try {
        data.destinationAddress.push_back(stoi(s));
    }
    catch(...){
        RLOGI("Destination Adrress::Only IPV4 address is supported");
        sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
        return;
    }
    data.destinationPort = request.destinationPort;
    data.maxKeepaliveIntervalMillis = request.maxKeepaliveIntervalMillis;
    data.cid = request.cid;
    auto msg =
        std::make_shared<StartKeepAliveRequestMessage>(
            serial,
            data,
            nullptr);
        if(msg) {
        GenericCallback<StartKeepAliveResp_t> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<StartKeepAliveResp_t> responseDataPtr) -> void {
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                int val = -1;
                if ((status == Message::Callback::Status::SUCCESS) && (responseDataPtr != nullptr)) {
                    RIL_Errno errorCode = RIL_Errno::RIL_E_SUCCESS;
                    val = static_cast<int>(responseDataPtr->error);
                    p->write(val);
                    val = static_cast<int>(responseDataPtr->handle);
                    p->write(val);
                    val = static_cast<int>(responseDataPtr->status);
                    p->write(val);
                    sendResponse(context, errorCode, p);
                }
                else {
                    sendResponse(context, errorCode, nullptr);
                }
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

}
}
}
