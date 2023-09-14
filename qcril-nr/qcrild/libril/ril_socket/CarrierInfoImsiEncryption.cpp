/******************************************************************************
#  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <framework/GenericCallback.h>
#include <ril_socket_api.h>

#include <request/SetCarrierInfoImsiEncryptionMessage.h>
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

void dispatchCarrierInfoImsiEncryption(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching CarrierInfoImsiEncryption request.");
    RIL_CarrierInfoForImsiEncryption carrier = {};
    RIL_PublicKeyType type = RIL_PublicKeyType::EPDG_RIL;
    if (p.read(carrier) == Marshal::Result::FAILURE || p.read(type) == Marshal::Result::FAILURE) {
        RLOGI("Message CarrierInfoImsiEncryption Could not be dispatched. Marshal Read Error");
        sendResponse(context, RIL_Errno::RIL_E_GENERIC_FAILURE, nullptr);
        return;
    }
    rildata::ImsiEncryptionInfo_t data = {};
    if (carrier.mcc) {
        data.mcc = std::string(carrier.mcc);
    }
    if (carrier.mnc) {
        data.mnc = std::string(carrier.mnc);
    }
    for (int i = 0; i < carrier.carrierKeyLength; i++) {
        data.carrierKey.push_back(carrier.carrierKey[i]);
    }
    if (carrier.keyIdentifier) {
        data.keyIdentifier = std::string(carrier.keyIdentifier);
    }
    data.expiryTime = carrier.expirationTime;
    data.keyType = static_cast<rildata::PublicKeyType_t>(type);
    p.release(carrier);
    auto msg = std::make_shared<rildata::SetCarrierInfoImsiEncryptionMessage>(data);
    if(msg) {
        GenericCallback<RIL_Errno> cb(
        [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
        std::shared_ptr<RIL_Errno> rsp) -> void {
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                int val;
                if ((status == Message::Callback::Status::SUCCESS) && (rsp != nullptr)) {
                    errorCode = RIL_Errno::RIL_E_SUCCESS;
                    RLOGI("CarrierInfoImsiEncryption RIL_Errno= %d ", *rsp);
                    val = *rsp;
                    if ( p->write(val) == Marshal::Result::FAILURE) {
                        RLOGI("CarrierInfoImsiEncryption response could not be sent to Client. Marshal Write Failed");
                        sendResponse(context, errorCode, nullptr);
                    }
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
