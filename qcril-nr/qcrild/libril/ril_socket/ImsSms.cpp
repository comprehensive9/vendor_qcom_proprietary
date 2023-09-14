/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <thread>
#include <ril_utils.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>
#include <telephony/ril_cdma_sms.h>
#include <Marshal.h>
#include <marshal/ImsSms.h>
#include <ril_socket_api.h>
#include <interfaces/sms/qcril_qmi_sms_types.h>
#include <modules/sms/qcril_qmi_sms.h>
#include <interfaces/sms/RilRequestImsSendSmsMessage.h>
#include <interfaces/sms/RilRequestAckImsSmsMessage.h>

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

void imsSendSms(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RIL_IMS_SmsMessage imsSms {};

    if (p.read(imsSms) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_Errno::RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    RIL_Errno err = RIL_Errno::RIL_E_SUCCESS;
    RIL_RadioTechnologyFamily tech = (imsSms.format == RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP) ?
            RIL_RadioTechnologyFamily::RADIO_TECH_3GPP : RIL_RadioTechnologyFamily::RADIO_TECH_3GPP2;

    auto msg = std::make_shared<RilRequestImsSendSmsMessage>(context, imsSms.messageRef, tech, imsSms.shallRetry);
    if (msg) {
        if (tech == RIL_RadioTechnologyFamily::RADIO_TECH_3GPP) {
            std::string payload;
            if (ril::socket::utils::convertBytesToHexString(imsSms.pdu, imsSms.pduLength, payload)) {
                std::string smsc(imsSms.smsc, imsSms.smscLength);
                msg->setGsmPayload(smsc, payload);
            } else {
                err = RIL_Errno::RIL_E_INVALID_ARGUMENTS;
            }
        } else {
            RIL_CDMA_SMS_Message cdmaMsg;
            if (ril::socket::utils::smsConvertPayloadToCdmaFormat(imsSms.pdu, imsSms.pduLength, cdmaMsg)) {
                msg->setCdmaPayload(cdmaMsg);
            } else {
                err = RIL_Errno::RIL_E_INVALID_ARGUMENTS;
            }
        }

        if (err == RIL_Errno::RIL_E_SUCCESS) {
            GenericCallback<QcRilRequestMessageCallbackPayload> cb(
                [context, tech] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> callbackData) -> void {
                    RIL_Errno e = RIL_Errno::RIL_E_INTERNAL_ERR;
                    std::shared_ptr<Marshal> parcel;
                    if (msg && callbackData) {
                        e = callbackData->errorCode;
                        if (callbackData->data) {
                            std::shared_ptr<RilSendSmsResult_t> sendSmsResult =
                                    std::static_pointer_cast<RilSendSmsResult_t>(callbackData->data);
                            parcel = std::make_shared<Marshal>();
                            if (parcel) {
                                RIL_IMS_SendSmsResponse resp {};
                                resp.messageRef = sendSmsResult->messageRef;
                                resp.status = ril::socket::utils::getImsSmsSendStatus(
                                    callbackData->errorCode,
                                    tech,
                                    sendSmsResult->bLteOnlyReg ? *sendSmsResult->bLteOnlyReg : false
                                );

                                if (parcel->write(resp) != Marshal::Result::SUCCESS) {
                                    parcel = nullptr;
                                    if (e == RIL_Errno::RIL_E_SUCCESS) {
                                        e = RIL_Errno::RIL_E_INTERNAL_ERR;
                                    }
                                }
                            } else if (e == RIL_Errno::RIL_E_SUCCESS) {
                                e = RIL_Errno::RIL_E_NO_MEMORY;
                            }
                        }
                    }
                    sendResponse(context, e, parcel);
                }
            );
            msg->setCallback(&cb);
            msg->dispatch();
        }
    } else {
        err = RIL_Errno::RIL_E_NO_MEMORY;
    }

    if (err != RIL_Errno::RIL_E_SUCCESS) {
        sendResponse(context, err, nullptr);
    }

    if (imsSms.smsc) {
        delete[] imsSms.smsc;
        imsSms.smsc = nullptr;
        imsSms.smscLength = 0;
    }

    if (imsSms.pdu) {
        delete[] imsSms.pdu;
        imsSms.pdu = nullptr;
        imsSms.pduLength = 0;
    }
}

void imsAckSms(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RIL_IMS_SmsAck smsAck {};

    if (p.read(smsAck) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_Errno::RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    auto msg = std::make_shared<RilRequestAckImsSmsMessage>(context, smsAck.messageRef,
            ril::socket::utils::convertImsSmsDeliveryStatus(smsAck.deliveryStatus));
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                    std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                if (resp) {
                    sendResponse(context, resp->errorCode, nullptr);
                } else {
                    sendResponse(context, RIL_Errno::RIL_E_INTERNAL_ERR, nullptr);
                }
            }
        );
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_Errno::RIL_E_NO_MEMORY, nullptr);
    }
}

void imsGetSmsFormat(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RIL_Errno err = RIL_Errno::RIL_E_SUCCESS;

    std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
    if (parcel) {
        RIL_IMS_SmsFormat smsFormat = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_UNKNOWN;

        switch (qcril_qmi_sms_get_ims_sms_format()) {
            case qmi_ril_sms_format_type::QMI_RIL_SMS_FORMAT_3GPP:
                smsFormat = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP;
                break;
            case qmi_ril_sms_format_type::QMI_RIL_SMS_FORMAT_3GPP2:
                smsFormat = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP2;
                break;
            default:
                break;
        }

        if (parcel->write(smsFormat) != Marshal::Result::SUCCESS) {
            parcel = nullptr;
            err = RIL_Errno::RIL_E_INTERNAL_ERR;
        }
    } else {
        err = RIL_Errno::RIL_E_NO_MEMORY;
    }

    sendResponse(context, err, parcel);
}

}
}
}
