/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <limits>
#include <telephony/ril_ims.h>
#include <Marshal.h>
#include <ril_socket_api.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/QcRilRequestSetCallWaitingMessage.h>
#include <interfaces/voice/QcRilRequestGetCallWaitingMessage.h>

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void imsSetCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RIL_IMS_CallWaitingSettings callWaitingSettings {};

    if (p.read(callWaitingSettings) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_Errno::RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    auto msg = std::make_shared<QcRilRequestSetCallWaitingMessage>(context);
    if (!msg) {
        sendResponse(context, RIL_Errno::RIL_E_NO_MEMORY, nullptr);
        return;
    }

    msg->setIsImsRequest(true);

    msg->setServiceStatus(callWaitingSettings.enabled ?
            qcril::interfaces::ServiceClassStatus::ENABLED :
            qcril::interfaces::ServiceClassStatus::DISABLED);

    if (callWaitingSettings.serviceClass != std::numeric_limits<uint32_t>::max()) {
        msg->setServiceClass(callWaitingSettings.serviceClass);
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<Marshal> parcel;
            RIL_Errno err = RIL_Errno::RIL_E_INTERNAL_ERR;

            if (status == Message::Callback::Status::SUCCESS && resp) {
                err = resp->errorCode;
                std::shared_ptr<qcril::interfaces::SipErrorInfo> sipErrorInfo =
                        std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
                if (sipErrorInfo && (sipErrorInfo->hasErrorCode() || sipErrorInfo->hasErrorString())) {
                    parcel = std::make_shared<Marshal>();
                    if (parcel) {
                        RIL_IMS_SipErrorInfo sipError {};

                        if (sipErrorInfo->hasErrorCode()) {
                            sipError.errorCode = sipErrorInfo->getErrorCode();
                        } else {
                            sipError.errorCode = std::numeric_limits<uint32_t>::max();
                        }

                        std::string errorString;
                        if (sipErrorInfo->hasErrorString()) {
                            errorString = sipErrorInfo->getErrorString();
                            sipError.errorString = const_cast<char*>(errorString.c_str());
                        } else {
                            sipError.errorString = nullptr;
                        }

                        if (parcel->write(sipError) != Marshal::Result::SUCCESS) {
                            parcel = nullptr;
                            err = RIL_Errno::RIL_E_INTERNAL_ERR;
                        }
                    } else {
                        err = RIL_Errno::RIL_E_NO_MEMORY;
                    }
                }
            }

            sendResponse(context, err, parcel);
        }
    );
    msg->setCallback(&cb);
    msg->dispatch();
}

void imsQueryCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    uint32_t serviceClass = std::numeric_limits<uint32_t>::max();

    if (p.read(serviceClass) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_Errno::RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    auto msg = std::make_shared<QcRilRequestGetCallWaitingMessage>(context);
    if (!msg) {
        sendResponse(context, RIL_Errno::RIL_E_NO_MEMORY, nullptr);
        return;
    }

    msg->setIsImsRequest(true);

    if (serviceClass != std::numeric_limits<uint32_t>::max()) {
        msg->setServiceClass(serviceClass);
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<Marshal> parcel;
            RIL_Errno err = RIL_Errno::RIL_E_INTERNAL_ERR;

            if (status == Message::Callback::Status::SUCCESS && resp) {
                err = resp->errorCode;

                if (resp->data) {
                    std::shared_ptr<qcril::interfaces::CallWaitingInfo> callWaitingInfo =
                            std::static_pointer_cast<qcril::interfaces::CallWaitingInfo>(resp->data);

                    if (
                            callWaitingInfo->hasStatus() ||
                            callWaitingInfo->hasServiceClass() ||
                            (
                                callWaitingInfo->hasErrorDetails() &&
                                callWaitingInfo->getErrorDetails() &&
                                (
                                    callWaitingInfo->getErrorDetails()->hasErrorCode() ||
                                    callWaitingInfo->getErrorDetails()->hasErrorString()
                                )
                            )
                    ) {
                        parcel = std::make_shared<Marshal>();
                        if (parcel) {
                            RIL_IMS_QueryCallWaitingResponse queryCallWaitingResponse {};

                            queryCallWaitingResponse.callWaitingSettings.enabled = callWaitingInfo->hasStatus()
                                    && callWaitingInfo->getStatus() == qcril::interfaces::ServiceClassStatus::ENABLED;

                            if (callWaitingInfo->hasServiceClass()) {
                                queryCallWaitingResponse.callWaitingSettings.serviceClass = callWaitingInfo->getServiceClass();
                            } else {
                                queryCallWaitingResponse.callWaitingSettings.serviceClass = std::numeric_limits<uint32_t>::max();
                            }

                            std::string sipErrorString;

                            if (callWaitingInfo->hasErrorDetails()) {
                                std::shared_ptr<qcril::interfaces::SipErrorInfo> sipErrorInfo = callWaitingInfo->getErrorDetails();
                                if (sipErrorInfo) {
                                    if (sipErrorInfo->hasErrorCode()) {
                                        queryCallWaitingResponse.sipError.errorCode = sipErrorInfo->getErrorCode();
                                    } else {
                                        queryCallWaitingResponse.sipError.errorCode = std::numeric_limits<uint32_t>::max();
                                    }

                                    if (sipErrorInfo->hasErrorString()) {
                                        sipErrorString = sipErrorInfo->getErrorString();
                                        queryCallWaitingResponse.sipError.errorString = const_cast<char*>(sipErrorString.c_str());
                                    } else {
                                        queryCallWaitingResponse.sipError.errorString = nullptr;
                                    }
                                }
                            } else {
                              queryCallWaitingResponse.sipError.errorCode = std::numeric_limits<uint32_t>::max();
                              queryCallWaitingResponse.sipError.errorString = nullptr;
                            }

                            if (parcel->write(queryCallWaitingResponse) != Marshal::Result::SUCCESS) {
                                parcel = nullptr;
                                err = RIL_Errno::RIL_E_INTERNAL_ERR;
                            }
                        } else {
                            err = RIL_Errno::RIL_E_NO_MEMORY;
                        }
                    }
                }
            }

            sendResponse(context, err, parcel);
        }
    );
    msg->setCallback(&cb);
    msg->dispatch();
}

}
}
}
