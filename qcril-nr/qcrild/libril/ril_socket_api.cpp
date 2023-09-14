/******************************************************************************
#  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <ril_internal.h>
#include <framework/Message.h>
#include <framework/GenericCallback.h>
#include <interfaces/voice/QcRilRequestCsDialMessage.h>
#include <interfaces/nas/nas_types.h>
#include <interfaces/dms/RilRequestRadioPowerMessage.h>
#include <interfaces/pbm/QcRilRequestGetAdnRecordMessage.h>
#ifndef RIL_FOR_MDM_LE
#include <utils/Log.h>
#else
#include <utils/Log2.h>
#endif
#include <Marshal.h>
#include <interfaces/nas/nas_types.h>
#include <interfaces/nas/RilRequestShutDownMessage.h>
#include <interfaces/nas/RilRequestGetVoiceRegistrationMessage.h>
#include <interfaces/nas/RilRequestGetDataRegistrationMessage.h>
#include <interfaces/nas/RilRequestExitEmergencyCallbackMessage.h>
#include <interfaces/nas/RilRequestCdmaSubscriptionMessage.h>
#include <interfaces/nas/RilRequestGetCdmaSubscriptionSourceMessage.h>
#include <interfaces/nas/RilRequestQueryCdmaRoamingPrefMessage.h>
#include <interfaces/sms/RilRequestSendSmsMessage.h>
#include <interfaces/sms/RilRequestAckGsmSmsMessage.h>
#include <interfaces/sms/RilRequestGetSmscAddressMessage.h>
#include <interfaces/sms/RilRequestSetSmscAddressMessage.h>
#include <interfaces/sms/RilRequestCdmaSendSmsMessage.h>
#include <interfaces/sms/RilRequestAckCdmaSmsMessage.h>
#include <interfaces/sms/RilRequestGetCdmaBroadcastConfigMessage.h>
#include <interfaces/sms/RilRequestCdmaSetBroadcastSmsConfigMessage.h>
#include <interfaces/sms/RilRequestCdmaSmsBroadcastActivateMessage.h>
#include <interfaces/sms/RilRequestCdmaWriteSmsToRuimMessage.h>
#include <interfaces/sms/RilRequestCdmaDeleteSmsOnRuimMessage.h>
#include <interfaces/sms/RilRequestWriteSmsToSimMessage.h>
#include <interfaces/sms/RilRequestDeleteSmsOnSimMessage.h>
#include <interfaces/nas/RilRequestSetCdmaSubscriptionSourceMessage.h>
#include <interfaces/nas/RilRequestSetCdmaRoamingPrefMessage.h>
#include <interfaces/pbm/pbm.h>
#include <interfaces/pbm/QcRilRequestUpdateAdnRecordMessage.h>

// UIM Headers
#include <interfaces/uim/UimGetImsiRequestMsg.h>
#include <interfaces/uim/UimSIMIORequestMsg.h>
#include <interfaces/uim/UimGetSlotStatusRequestMsg.h>
#include <interfaces/uim/UimSIMOpenChannelRequestMsg.h>
#include <interfaces/uim/UimSIMCloseChannelRequestMsg.h>
#include <interfaces/uim/UimTransmitAPDURequestMsg.h>
#include <interfaces/uim/UimChangeSimPinRequestMsg.h>
#include <interfaces/uim/UimEnterSimPukRequestMsg.h>
#include <interfaces/uim/UimGetFacilityLockRequestMsg.h>
#include <interfaces/uim/UimSetFacilityLockRequestMsg.h>
#include <interfaces/uim/UimISIMAuthenticationRequestMsg.h>
#include <interfaces/uim/UimSIMCloseChannelRequestMsg.h> // TODO - for Sim STK envelope request
#include <interfaces/uim/UimSIMAuthenticationRequestMsg.h>

#undef  TAG
#define TAG "RILQ"

#include <request/SetupDataCallRequestMessage.h>
#include <request/GetRadioDataCallListRequestMessage.h>
#include <request/SetDataProfileRequestMessage.h>
#include <request/DeactivateDataCallRequestMessage.h>
#include <request/SetInitialAttachApnRequestMessage.h>
#include <request/StartLCERequestMessage.h>
#include <request/StopLCERequestMessage.h>
#include <request/PullLCEDataRequestMessage.h>
#include <request/SetLinkCapFilterMessage.h>
#include <request/SetLinkCapRptCriteriaMessage.h>
#include <request/GetDataNrIconTypeMessage.h>
#include "request/CaptureLogBufferMessage.h"

#include <marshal/CdmaSubscription.h>

namespace ril {
namespace socket {
namespace api {

RIL_Errno dispatchStatusToRilErrno(Message::Callback::Status status) {
    RIL_Errno ret = RIL_E_INTERNAL_ERR;
    switch(status) {
        case Message::Callback::Status::CANCELLED:
            ret = RIL_E_CANCELLED;
            break;
        case Message::Callback::Status::NO_HANDLER_FOUND:
            ret = RIL_E_REQUEST_NOT_SUPPORTED;
            break;
        case Message::Callback::Status::TIMEOUT:
            ret = RIL_E_ABORTED;
            break;
        case Message::Callback::Status::SUCCESS:
            ret = RIL_E_SUCCESS;
            break;
        case Message::Callback::Status::FAILURE:
            break;
        case Message::Callback::Status::BROADCAST_NOT_ALLOWED:
            break;
    }
    return ret;
}

void handleClientDisconnectionRequest(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Handling client disconnection request.");
    if (context) {
        sendResponse(context, RIL_E_SUCCESS, nullptr);
    }
}

void dispatchCaptureRilDataDump(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching capture Dumpsys");
    auto msg = std::make_shared<rildata::CaptureLogBufferMessage>();
    if (msg != nullptr) {
        GenericCallback<RIL_Errno> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<RIL_Errno> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = *resp;
                }
                sendResponse(context, errorCode, nullptr);
            });
        msg->setCallback(&cb);
        msg->dispatch();
    }
}

void dispatchDial(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching dial request.");
    RIL_Dial params{};
    Marshal::Result result = p.read(params);
    if (result != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_Errno::RIL_E_INTERNAL_ERR, nullptr);
        return;
    }
    auto msg = std::make_shared<QcRilRequestCsDialMessage>(context);
    if (msg != nullptr) {
        RLOGI("Address: %s", params.address);
        RLOGI("CLIR: %d", params.clir);
        msg->setAddress(params.address);
        msg->setClir(params.clir);
        // TODO: pass on the rest of the dial request parameters

        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = resp->errorCode;
                }
                sendResponse(context, errorCode, nullptr);
            });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    p.release(params);
}

void dispatchGetVoiceRegState(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching get voice registration state.");
    auto msg = std::make_shared<RilRequestGetVoiceRegistrationMessage>(context);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                auto p = std::make_shared<Marshal>();
                if (p == nullptr) {
                    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
                } else {
                    RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                    if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                        errorCode = resp->errorCode;
                        auto rilRegResult = std::static_pointer_cast<
                                qcril::interfaces::RilGetVoiceRegResult_t>(resp->data);
                        p->write<RIL_VoiceRegistrationStateResponse>(rilRegResult->respData);
                    } else {
                        RIL_VoiceRegistrationStateResponse data = {};
                        p->write<RIL_VoiceRegistrationStateResponse>(data);
                    }
                    sendResponse(context, errorCode, p);
                }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchGetDataRegState(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    auto msg = std::make_shared<RilRequestGetDataRegistrationMessage>(context);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                auto p = std::make_shared<Marshal>();
                if (p == nullptr) {
                    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
                } else {
                    RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                    if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                        errorCode = resp->errorCode;
                        auto rilRegResult = std::static_pointer_cast<
                                qcril::interfaces::RilGetDataRegResult_t>(resp->data);
                        p->write<RIL_DataRegistrationStateResponse>(rilRegResult->respData);
                    } else {
                        RIL_DataRegistrationStateResponse data = {};
                        p->write<RIL_DataRegistrationStateResponse>(data);
                    }
                    sendResponse(context, errorCode, p);
                }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchShutDown(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    auto msg = std::make_shared<RilRequestShutDownMessage>(context);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = resp->errorCode;
                }
                sendResponse(context, errorCode, nullptr);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchExitEmergencyCbMode(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    auto msg = std::make_shared<RilRequestExitEmergencyCallbackMessage>(context);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = resp->errorCode;
                }
                sendResponse(context, errorCode, nullptr);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCellInfo(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching cell info request.");

    if (!context) {
        return;
    }

    android::SocketRequestInfo* pRI = android::addSocketRequestToList(
        context->getRequestToken(),
        [context] (RIL_Errno errorCode, void* response, size_t responseLen) {
            std::shared_ptr<Marshal> p;

            if (response == nullptr || responseLen == 0) {
                if (errorCode == RIL_E_SUCCESS) {
                    errorCode = RIL_E_INVALID_RESPONSE;
                }
            } else if (responseLen % sizeof(RIL_CellInfo_v12) != 0) {
                RLOGE("Size of the response buffer should be a multiple of the size of RIL_CellInfo_v12.");
                if (errorCode == RIL_E_SUCCESS) {
                    errorCode = RIL_E_INVALID_RESPONSE;
                }
            } else {
                p = std::make_shared<Marshal>();
                if (!p) {
                    RLOGE("Failed to create Marshal.");
                    if (errorCode == RIL_E_SUCCESS) {
                        errorCode = RIL_E_NO_MEMORY;
                    }
                } else {
                    // TODO: Check for marshalling errors
                    // TODO: Stop writing the array size when Marshal supports the ability to read an entire array
                    p->write(static_cast<uint64_t>(responseLen / sizeof(RIL_CellInfo_v12)));
                    p->write(reinterpret_cast<RIL_CellInfo_v12*>(response),
                             responseLen / sizeof(RIL_CellInfo_v12));
                }
            }

            sendResponse(context, errorCode, p);
        }
    );

    if (pRI == nullptr) {
        return;
    }

    RLOGI("Calling onRequest for cell info request.");

    CALL_ONREQUEST(context->getRequestId(), NULL, 0, pRI, 0);
}

void dispatchSendSms(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RIL_GsmSmsMessage sms = {.smscPdu = nullptr, .pdu = nullptr};
    bool expectMore = false;

    if (p.read(sms) != Marshal::Result::SUCCESS
            || p.read(expectMore) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_Errno::RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    auto msg = std::make_shared<RilRequestSendSmsMessage>(context,
            sms.smscPdu ? std::string(sms.smscPdu) : std::string(),
            sms.pdu ? std::string(sms.pdu) : std::string(),
            expectMore);

    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] ([[maybe_unused]] std::shared_ptr<Message> msg,
                       Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno e = RIL_Errno::RIL_E_INTERNAL_ERR;
                auto p = std::make_shared<Marshal>();
                if (p == nullptr) {
                    e = RIL_Errno::RIL_E_NO_MEMORY;
                } else {
                    RIL_SMS_Response smsResponse {};
                    smsResponse.messageRef = -1;
                    smsResponse.errorCode = -1;

                    if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                        e = resp->errorCode;
                        auto sendSmsInfo =
                            std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
                        if(sendSmsInfo) {
                            smsResponse.messageRef = sendSmsInfo->messageRef;
                            smsResponse.ackPDU = const_cast<char*>(sendSmsInfo->ackPDU.c_str());
                            smsResponse.errorCode = sendSmsInfo->errorCode;
                        }
                        p->write<RIL_SMS_Response>(smsResponse);
                    } else {
                        p->write<RIL_SMS_Response>(smsResponse);
                    }
                }
                sendResponse(context, e, p);
            });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_Errno::RIL_E_NO_MEMORY, nullptr);
    }

    if (sms.smscPdu) {
        delete[] sms.smscPdu;
        sms.smscPdu = nullptr;
    }

    if (sms.pdu) {
        delete[] sms.pdu;
        sms.pdu = nullptr;
    }
}

void dispatchSmsAck(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RIL_GsmSmsAck ack = {.result = 0, .cause = 0};
    p.read<RIL_GsmSmsAck>(ack);
    auto msg = std::make_shared<RilRequestAckGsmSmsMessage>(context, !!ack.result, ack.cause);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] ([[maybe_unused]] std::shared_ptr<Message> msg,
                       Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = resp->errorCode;
                }
                sendResponse(context, errorCode, nullptr);
            });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchGetSmscAddress(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    auto msg = std::make_shared<RilRequestGetSmscAddressMessage>(context);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] ([[maybe_unused]] std::shared_ptr<Message> msg,
                       Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                auto p = std::make_shared<Marshal>();
                if (p == nullptr) {
                    sendResponse(context, errorCode, nullptr);
                } else {
                    if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                        errorCode = resp->errorCode;
                        auto smscInfo =
                            std::static_pointer_cast<RilGetSmscAddrResult_t>(resp->data);
                        if(smscInfo) {
                          p->write(smscInfo->smscAddr);
                        }
                    } else {
                        errorCode = RIL_E_GENERIC_FAILURE;
                        p->write(std::string());
                    }
                    sendResponse(context, errorCode, p);
                }
            });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchSetSmscAddress(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    std::string addr;
    p.read(addr);
    auto msg = std::make_shared<RilRequestSetSmscAddressMessage>(context, std::move(addr));
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] ([[maybe_unused]] std::shared_ptr<Message> msg,
                       Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = resp->errorCode;
                }
                sendResponse(context, errorCode, nullptr);
            });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchWriteSmsToSim(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RIL_SMS_WriteArgs smsWriteArgs {};
    if (p.read(smsWriteArgs) == Marshal::Result::SUCCESS) {
        auto msg = std::make_shared<RilRequestWriteSmsToSimMessage>(context,
                smsWriteArgs.smsc, smsWriteArgs.pdu, smsWriteArgs.status);

        if (msg) {
            GenericCallback<QcRilRequestMessageCallbackPayload> cb(
                [context]([[maybe_unused]] std::shared_ptr<Message> msg,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                    std::shared_ptr<Marshal> p;
                    RIL_Errno e = RIL_E_INTERNAL_ERR;

                    if (resp) {
                        p = std::make_shared<Marshal>();
                        e = resp->errorCode;
                        auto writeInfo =
                            std::static_pointer_cast<RilWriteSmsToSimResult_t>(resp->data);

                        if (p && writeInfo) {
                            if (p->write(writeInfo->recordNumber) != Marshal::Result::SUCCESS) {
                                p = nullptr;
                                if (e == RIL_E_SUCCESS) {
                                    e = RIL_E_INTERNAL_ERR;
                                }
                            }
                        } else {
                            e = RIL_E_NO_MEMORY;
                        }
                    }
                    sendResponse(context, e, p);
                }
            );
            msg->setCallback(&cb);
            msg->dispatch();
        } else {
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        }
    } else {
        RLOGE("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
    }

    if (smsWriteArgs.smsc != nullptr) {
        delete[] smsWriteArgs.smsc;
    }

    if (smsWriteArgs.pdu != nullptr) {
        delete[] smsWriteArgs.pdu;
    }
}

void dispatchDeleteSmsOnSim(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    int32_t index = -1;
    if (p.read(index) != Marshal::Result::SUCCESS) {
        RLOGE("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    auto msg = std::make_shared<RilRequestDeleteSmsOnSimMessage>(context, index);
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context]([[maybe_unused]] std::shared_ptr<Message> msg,
                      Message::Callback::Status status,
                      std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno e = RIL_E_INTERNAL_ERR;
                if (resp) {
                    e = resp->errorCode;
                }
                sendResponse(context, e, nullptr);
            }
        );
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCdmaSetSubscriptionSource(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RIL_CdmaSubscriptionSource cdmaSubSource = CDMA_SUBSCRIPTION_SOURCE_INVALID;
    if (p.read(cdmaSubSource) != Marshal::Result::SUCCESS) {
        RLOGE("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    RilRequestSetCdmaSubscriptionSourceMessage::SubscriptionSource source =
            RilRequestSetCdmaSubscriptionSourceMessage::SubscriptionSource::SIM;
    switch (cdmaSubSource) {
        case RIL_CdmaSubscriptionSource::CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM:
            break;
        case RIL_CdmaSubscriptionSource::CDMA_SUBSCRIPTION_SOURCE_NV:
            source = RilRequestSetCdmaSubscriptionSourceMessage::SubscriptionSource::NV;
            break;
        default:
            RLOGE("Invalid CDMA subscription source %d", cdmaSubSource);
            sendResponse(context, RIL_E_INVALID_ARGUMENTS, nullptr);
            return;
    }

    auto msg = std::make_shared<RilRequestSetCdmaSubscriptionSourceMessage>(context, source);
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                      std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno e = RIL_E_INVALID_RESPONSE;
                if (resp) {
                    e = resp->errorCode;
                }
                sendResponse(context, e, nullptr);
            }
        );
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCdmaGetSubscriptionSource(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestGetCdmaSubscriptionSourceMessage>(context);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          RIL_CdmaSubscriptionSource cdmaSubSource = CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto getResult =
                std::static_pointer_cast<qcril::interfaces::RilGetCdmaSubscriptionSourceResult_t>(
                    resp->data);
            if (getResult) {
              cdmaSubSource = getResult->mSource;
            }
          }
          auto p = std::make_shared<Marshal>();
          if (p) {
            p->write(cdmaSubSource);
          } else {
            errorCode = RIL_E_NO_MEMORY;
          }
          sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void dispatchCdmaSetRoamingPreference(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RIL_CdmaRoamingPreference cdmaRoamingPref = CDMA_ROAMING_PREFERENCE_INVALID;
    if (p.read(cdmaRoamingPref) != Marshal::Result::SUCCESS) {
        RLOGE("Failed to read arguments of the request from parcel.");
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    auto msg = std::make_shared<RilRequestSetCdmaRoamingPrefMessage>(context, cdmaRoamingPref);
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                      std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno e = RIL_E_INVALID_RESPONSE;
                if (resp) {
                    e = resp->errorCode;
                }
                sendResponse(context, e, nullptr);
            }
        );
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCdmaGetRoamingPreference(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestQueryCdmaRoamingPrefMessage>(context);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          int prefType = 0;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto getResult =
                std::static_pointer_cast<qcril::interfaces::RilQueryCdmaRoamingPrefResult_t>(
                    resp->data);
            if (getResult) {
              prefType = getResult->mPrefType;
            }
          }
          auto p = std::make_shared<Marshal>();
          if (p) {
            p->write(prefType);
          } else {
            errorCode = RIL_E_NO_MEMORY;
          }
          sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void dispatchDeactivateDataCall(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching DeactivateDataCall request.");
    using namespace rildata;
    int serial = 0;
    int32_t cid = p.readInt32();
    bool reason = p.readBool();
    RLOGI("cid: %d", cid);
    RLOGI("reason: %d", reason);
    auto msg =
        std::make_shared<DeactivateDataCallRequestMessage>(
            serial,
            cid,
            (DataRequestReason_t)(reason),
            nullptr);
    if (msg) {
        GenericCallback<ResponseError_t> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<ResponseError_t> rsp) -> void {
                    RIL_Errno e = RIL_Errno::RIL_E_SUCCESS;
                    if (msg && rsp) {
                        if ((status != Message::Callback::Status::SUCCESS) ||
                           (*rsp != ResponseError_t::NO_ERROR)) {
                            switch(*rsp) {
                                case ResponseError_t::NOT_SUPPORTED: e = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED; break;
                                case ResponseError_t::INVALID_ARGUMENT: e = RIL_Errno::RIL_E_INVALID_ARGUMENTS; break;
                                case ResponseError_t::CALL_NOT_AVAILABLE: e = RIL_Errno::RIL_E_INVALID_CALL_ID; break;
                                default: e = RIL_Errno::RIL_E_GENERIC_FAILURE; break;
                            }
                        }
                    }
                sendResponse(context, e, nullptr);
            });
        msg->setCallback(&cb);
        msg->dispatch();
    }
}

void dispatchStartLce(std::shared_ptr<SocketRequestContext> context, Marshal& p)
{
    RLOGI("Dispatching StartLce request.");
    using namespace rildata;
    int serial = 0;
    int32_t interval = p.readInt32();
    int32_t mode = p.readInt32();
    RLOGI("interval: %d", interval);
    RLOGI("mode pull/push: %d", mode);
    auto msg =
        std::make_shared<StartLCERequestMessage>(
            serial,
            interval,
            mode,
            nullptr);
    if(msg) {
        GenericCallback<RIL_LceStatusInfo> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<RIL_LceStatusInfo> rsp) -> void {
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                RIL_LceStatusInfo data = {};
                if ((status == Message::Callback::Status::SUCCESS) && (rsp != nullptr)) {
                    errorCode = RIL_Errno::RIL_E_SUCCESS;
                    RLOGI("Pull Lce data cb invoked status %d ", status);
                    RLOGI("Lce Status = %d", rsp->lce_status);
                    RLOGI("Actual Interval in ms = %d", rsp->actual_interval_ms);
                    data.lce_status = rsp->lce_status;
                    data.actual_interval_ms = rsp->actual_interval_ms;
                    p->write(data);
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

void dispatchSetLinkCapRptCriteria(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching SetLinkCapRptCriteria request.");
    using namespace rildata;
    int32_t serial = 0;
    RIL_LinkCapCriteria criteria = {};
    LinkCapCriteria_t data = {};
    p.read(criteria);
    RLOGI("Criteria Reporting Hytersis in ms: %d\n", criteria.hysteresisMs);
    data.hysteresisMs = criteria.hysteresisMs;
    RLOGI("Criteria Reporting Speed in kbps Downlink: %d\n", criteria.hysteresisDlKbps);
    data.hysteresisDlKbps = criteria.hysteresisDlKbps;
    RLOGI("Criteria Reporting Speed in kbps Uplink: %d\n", criteria.hysteresisUlKbps);
    data.hysteresisUlKbps = criteria.hysteresisUlKbps;
    RLOGI("Criteria Length of List for DownLink: %d\n",criteria.thresholdsDownLength);
    if (criteria.thresholdsDownlinkKbps) {
        for (int i = 0; i < criteria.thresholdsDownLength; i++) {
            RLOGI("Criteria DownLink speed in kbps: %d th item: %d\n",i, criteria.thresholdsDownlinkKbps[i]);
            data.thresholdsDownlinkKbps.push_back(criteria.thresholdsDownlinkKbps[i]);
        }
        delete[]criteria.thresholdsDownlinkKbps;
        criteria.thresholdsDownlinkKbps = NULL;
    }
    RLOGI("Criteria Length of List for UpLink: %d\n",criteria.thresholdsUpLength);
    if (criteria.thresholdsUplinkKbps) {
        for (int i = 0; i < criteria.thresholdsUpLength; i++) {
            RLOGI("Criteria UpLink speed in kbps: %d th item: %d\n",i, criteria.thresholdsUplinkKbps[i]);
            data.thresholdsUplinkKbps.push_back(criteria.thresholdsUplinkKbps[i]);
        }
        delete[]criteria.thresholdsUplinkKbps;
        criteria.thresholdsUplinkKbps = NULL;
    }
    RLOGI("Criteria Selected RAN: %d\n",static_cast<uint8_t>(criteria.ran));
#ifdef HAS_QCRIL_DATA_1_5_RESPONSE_TYPES
    data.ran = static_cast<AccessNetwork_t>(criteria.ran);
#else
    data.ran = static_cast<RAN_t>(criteria.ran);
#endif
    p.release(criteria);
    auto msg =
        std::make_shared<SetLinkCapRptCriteriaMessage>(
            serial,
            data,
            nullptr);
        if(msg) {
        GenericCallback<LinkCapCriteriaResult_t> cb(
        [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
        std::shared_ptr<LinkCapCriteriaResult_t> rsp) -> void {
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
                    RLOGI("SetLinkCapRptCriteria RIL response = %d ", static_cast<uint8_t>(*rsp));
                    val = static_cast<uint8_t>(*rsp);
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

void dispatchSetLinkCapFilter(std::shared_ptr<SocketRequestContext> context, Marshal& p){
    RLOGI("Dispatching SetLinkCapFilter request.");
    using namespace rildata;
    int32_t serial = 0;
    int32_t val;
    val = p.readInt32();
    RLOGI("Enable_bit: %d", val);
    ReportFilter_t enable_bit = static_cast<ReportFilter_t>(val);
    auto msg =
        std::make_shared<SetLinkCapFilterMessage>(
            serial,
            enable_bit,
            nullptr);
        if(msg) {
        GenericCallback<int> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<int> rsp) -> void {
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
                    RLOGI("SetLinkCapFilter response = %d ", *rsp);
                    val = *rsp;
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

void dispatchGetDataNrIconType(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching GetDataNrIconType request.");
    using namespace rildata;
    int32_t serial = 0;
    auto msg =
        std::make_shared<GetDataNrIconTypeMessage>(
            serial,
            nullptr);
        if(msg) {
        GenericCallback<NrIconType_t> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<NrIconType_t> responseDataPtr) -> void {
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                int val;
                five_g_icon_type rilIconType = five_g_icon_type::FIVE_G_ICON_TYPE_INVALID;
                if ((status == Message::Callback::Status::SUCCESS) && (responseDataPtr != nullptr)) {
                    RIL_Errno errorCode = RIL_Errno::RIL_E_SUCCESS;
                    if(responseDataPtr) {
                        if(responseDataPtr->isNone()) {
                          rilIconType = FIVE_G_ICON_TYPE_NONE;
                        } else if(responseDataPtr->isBasic()) {
                          rilIconType = FIVE_G_ICON_TYPE_BASIC;
                        } else if(responseDataPtr->isUwb()) {
                          rilIconType = FIVE_G_ICON_TYPE_UWB;
                        } else {
                          rilIconType = FIVE_G_ICON_TYPE_INVALID;
                        }
                    }
                    val = static_cast<int>(rilIconType);
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

void dispatchStopLce(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching StopLce request.");
    using namespace rildata;
    int serial = 0;
    auto msg =
        std::make_shared<StopLCERequestMessage>(
            serial,
            nullptr);
    if(msg) {
        GenericCallback<RIL_LceStatusInfo> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<RIL_LceStatusInfo> rsp) -> void {
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                RIL_LceStatusInfo data = {};
                if ((status == Message::Callback::Status::SUCCESS) && (rsp != nullptr)) {
                    errorCode = RIL_Errno::RIL_E_SUCCESS;
                    RLOGI("Pull Lce data cb invoked status %d ", status);
                    RLOGI("Lce Status = %d", rsp->lce_status);
                    RLOGI("Actual Interval in ms = %d", rsp->actual_interval_ms);
                    data.lce_status = rsp->lce_status;
                    data.actual_interval_ms = rsp->actual_interval_ms;
                    p->write(data);
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

void dispatchPullLceData(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching PullLceData request.");
    using namespace rildata;
    int serial = 0;
    auto msg =
        std::make_shared<PullLCEDataRequestMessage>(
            serial,
            nullptr);
    if(msg) {
        GenericCallback<RIL_LceDataInfo> cb(
            [context]  (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<RIL_LceDataInfo> rsp) -> void {
            auto p = std::make_shared<Marshal>();
            if (p == nullptr) {
                RLOGI("[ril_socket_api]: Device has no memory");
                sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            }
            else {
                RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
                RIL_LceDataInfo data = {};
                if ((status == Message::Callback::Status::SUCCESS) && (rsp != nullptr)) {
                    errorCode = RIL_Errno::RIL_E_SUCCESS;
                    RLOGI("Pull Lce data cb invoked status %d ", status);
                    RLOGI("Last hop capacity in kbps = %d", rsp->last_hop_capacity_kbps);
                    RLOGI("Confidence level = %d", rsp->confidence_level);
                    RLOGI("Lce suspended Status= %d", rsp->lce_suspended);
                    data.last_hop_capacity_kbps = rsp->last_hop_capacity_kbps;
                    data.confidence_level = rsp->confidence_level;
                    data.lce_suspended = rsp->lce_suspended;
                    p->write(data);
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

void dispatchSetupDataCall(std::shared_ptr<SocketRequestContext> context, Marshal& parcel) {
    RLOGI("Dispatching SetUpDataCall request.");
    using namespace rildata;
    int32_t serial =0;
    RIL_SetUpDataCallParams callParams;
    memset(&callParams, 0, sizeof(RIL_SetUpDataCallParams));
    if (parcel.read(callParams) == Marshal::Result::FAILURE)
    {
        RLOGI("SetupDataCallRequestMessage could not be dispatched. Marshal Read Failed.");
        return;
    }
    AccessNetwork_t accnet = static_cast<AccessNetwork_t>(callParams.accessNetwork);
    RIL_DataProfileInfo_IRadio_1_4 dataProfileInfo = callParams.profileInfo;
    bool roamingAllowed = static_cast<bool>(callParams.roamingAllowed);
    RIL_RadioDataRequestReasons reason = callParams.reason;
    std::vector<std::string> radioAddresses;
    std::vector<std::string> radioDnses;
    int i = 0;
    string s = "";
    string apn = "";
    string protocol = "";
    string roamingProtocol = "";
    string user = "";
    string password = "";
    if (dataProfileInfo. apn) {
        apn = std::string(dataProfileInfo.apn);
    }
    if (dataProfileInfo.protocol) {
        protocol = std::string(dataProfileInfo.protocol);
    }
    if (dataProfileInfo.roamingProtocol) {
        roamingProtocol = std::string(dataProfileInfo.roamingProtocol);
    }
    if (dataProfileInfo.user) {
        user = std::string(dataProfileInfo.user);
    }
    if (dataProfileInfo.password) {
        password = std::string(dataProfileInfo.password);
    }
    if (callParams.addresses) {
        while(callParams.addresses[i] != '\0') {
            if (callParams.addresses[i] != ',') {
                s += callParams.addresses[i];
            }
            else {
                radioAddresses.push_back(s);
                s = "";
            }
            i++;
        }
        if (s != "") {
            radioAddresses.push_back(s);
        }
    }
    i = 0;
    s = "";
    if (callParams.dnses) {
        while(callParams.dnses[i] != '\0') {
            if (callParams.dnses[i] != ',') {
                s += callParams.dnses[i];
            }
            else {
                radioDnses.push_back(s);
                s = "";
            }
            i++;
        }
        if (s != "") {
            radioDnses.push_back(s);
        }
    }
    parcel.release(callParams);
    auto msg =
        std::make_shared<SetupDataCallRequestMessage>(
        serial,
        RequestSource_t::RADIO,
        accnet,
        (DataProfileId_t)dataProfileInfo.profileId,
        apn,
        protocol,
        roamingProtocol,
        (ApnAuthType_t)dataProfileInfo.authType,
        user,
        password,
        (DataProfileInfoType_t)dataProfileInfo.type,
        dataProfileInfo.maxConnsTime,
        dataProfileInfo.maxConns,
        dataProfileInfo.waitTime,
        (bool)dataProfileInfo.enabled,
        (ApnTypes_t)(dataProfileInfo.supportedTypesBitmask),
        (RadioAccessFamily_t)(dataProfileInfo.bearerBitmask),
        dataProfileInfo.mtu,
        (bool)dataProfileInfo.preferred,
        (bool)dataProfileInfo.persistent,
        roamingAllowed,
        (DataRequestReason_t)reason,
        radioAddresses,
        radioDnses,
        nullptr);
        RLOGI("Dispatching dispatchSetupDataCall request.");
    if ( msg ) {
        GenericCallback<SetupDataCallResponse_t> cb([context](std::shared_ptr<Message> msg,
            Message::Callback::Status status,
            std::shared_ptr<SetupDataCallResponse_t> rsp) -> void {
        auto p = std::make_shared<Marshal>();
        if (p == nullptr) {
            RLOGI("dispatchSetupDataCall Marshal is nullptr ");
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        } else {
            RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
            RIL_Data_Call_Response_v11 dcResult = {};
            if ( msg && (status == Message::Callback::Status::SUCCESS) && (rsp != nullptr)) {

                RLOGI("setup data call cb invoked status %d ", status);
                RLOGI("cause = %d", rsp->call.cause);
                RLOGI("suggestedRetryTime = %d", rsp->call.suggestedRetryTime);
                RLOGI("cid = %d", rsp->call.cid);
                RLOGI("active = %d", rsp->call.active);
                RLOGI("type = %s", rsp->call.type.c_str());
                RLOGI("ifname = %s", rsp->call.ifname.c_str());
                RLOGI("addresses = %s", rsp->call.addresses.c_str());
                RLOGI("dnses = %s", rsp->call.dnses.c_str());
                RLOGI("gateways = %s", rsp->call.gateways.c_str());
                RLOGI("pcscf = %s", rsp->call.pcscf.c_str());
                RLOGI("mtu = %d", rsp->call.mtu);
                errorCode = static_cast<RIL_Errno>(rsp->respErr);
                dcResult.status = static_cast<int>(rsp->call.cause);
                dcResult.suggestedRetryTime = rsp->call.suggestedRetryTime;
                dcResult.cid = rsp->call.cid;
                dcResult.active = (int)rsp->call.active;

                dcResult.type = new char[ (rsp->call.type.length())+1]{};
                if (dcResult.type) {
                    strlcpy(dcResult.type, rsp->call.type.c_str(), (rsp->call.type.length())+1);
                }

                dcResult.ifname = new char[(rsp->call.ifname.length())+1]{};
                if(dcResult.ifname) {
                    strlcpy(dcResult.ifname, rsp->call.ifname.c_str(), (rsp->call.ifname.length())+1);
                }

                dcResult.addresses = new char[(rsp->call.addresses.length())+1]{};
                if (dcResult.addresses) {
                    strlcpy(dcResult.addresses, rsp->call.addresses.c_str(), (rsp->call.addresses.length())+1);
                }

                dcResult.dnses = new char[(rsp->call.dnses.length())+1]{};
                if (dcResult.dnses) {
                    strlcpy(dcResult.dnses, rsp->call.dnses.c_str(), (rsp->call.dnses.length())+1);
                }

                dcResult.gateways = new char[(rsp->call.gateways.length())+1]{};
                if (dcResult.gateways) {
                    strlcpy(dcResult.gateways, rsp->call.gateways.c_str(), (rsp->call.gateways.length())+1);
                }

                dcResult.pcscf = new char[(rsp->call.pcscf.length())+1]{};
                if (dcResult.pcscf) {
                    strlcpy(dcResult.pcscf, rsp->call.pcscf.c_str(), (rsp->call.pcscf.length())+1);
                }
                dcResult.mtu = rsp->call.mtu;
                if (p->write(dcResult) == Marshal::Result::FAILURE) {
                    RLOGI("SetupDataCall Response could not be sent to Client. Marshal Write Failed");
                    sendResponse(context, errorCode, nullptr);
                } else {
                    sendResponse(context, errorCode, p);
                }
                p->release(dcResult);
            } else {
                switch(rsp->respErr) {
                    case ResponseError_t::NOT_SUPPORTED: errorCode = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED; break;
                    case ResponseError_t::INVALID_ARGUMENT: errorCode = RIL_Errno::RIL_E_INVALID_ARGUMENTS; break;
                    case ResponseError_t::CALL_NOT_AVAILABLE: errorCode = RIL_Errno::RIL_E_INVALID_CALL_ID; break;
                    default: errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE; break;

                }
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

void dispatchSendCdmaSms(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching sending CDMA SMS request.");
    RIL_CDMA_SMS_Message rcsm = {};
    p.read<RIL_CDMA_SMS_Message>(rcsm);
    auto msg = std::make_shared<RilRequestCdmaSendSmsMessage>(context, rcsm);
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            ([context]([[maybe_unused]] std::shared_ptr<Message> msg,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                auto p = std::make_shared<Marshal>();
                if (p == nullptr) {
                    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
                } else {
                    RIL_SMS_Response result {-1, nullptr, -1};
                    RIL_Errno e = RIL_E_GENERIC_FAILURE;
                    if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                        e = resp->errorCode;
                        auto sendSmsInfo =
                            std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
                        if(sendSmsInfo) {
                            result.messageRef = sendSmsInfo->messageRef;
                            result.ackPDU = const_cast<char*>(sendSmsInfo->ackPDU.c_str());
                            result.errorCode = sendSmsInfo->errorCode;
                        }
                    }
                    p->write<RIL_SMS_Response>(result);
                    sendResponse(context, e, p);
                }
        }));
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchAckCdmaSms(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching acking CDMA SMS request.");
    RIL_CDMA_SMS_Ack ack = {};
    p.read<RIL_CDMA_SMS_Ack>(ack);
    auto msg = std::make_shared<RilRequestAckCdmaSmsMessage>(context,
            ack.uErrorClass == RIL_CDMA_SMS_NO_ERROR, ack.uSMSCauseCode);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] ([[maybe_unused]] std::shared_ptr<Message> msg,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = resp->errorCode;
                }
                sendResponse(context, errorCode, nullptr);
            });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCdmaGetBroadcastSmsConfig(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching get CDMA broadcast sms config:");
    auto msg = std::make_shared<RilRequestGetCdmaBroadcastConfigMessage>(context);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] ([[maybe_unused]] std::shared_ptr<Message> msg,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                auto p = std::make_shared<Marshal>();
                if (p == nullptr) {
                    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
                } else {
                    RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                    if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                        errorCode = resp->errorCode;
                        auto configInfo =
                            std::static_pointer_cast<RilGetCdmaBroadcastConfigResult_t>(resp->data);
                        if(configInfo) {
                            p->write(configInfo->configList.data(), configInfo->configList.size());
                        }
                    }
                    sendResponse(context, errorCode, p);
                }
            });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCdmaSetBroadcastSmsConfig(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching set CDMA broadcast sms config:");
    size_t num = 0;
    RIL_CDMA_BroadcastSmsConfigInfo* configInfo = nullptr;

    Marshal::Result res = p.readAndAlloc<RIL_CDMA_BroadcastSmsConfigInfo>(
            configInfo, num);
    if (res != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Unmarshal failed. err: %d", (int)res);
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        return;
    }

    std::vector<RIL_CDMA_BroadcastSmsConfigInfo> configVec;
    if (num != 0) {
        configVec.reserve(num);
        for (int i = 0; i < num; i++) {
            configVec[i].service_category = configInfo[i].service_category;
            configVec[i].language = configInfo[i].language;
            configVec[i].selected = configInfo[i].selected;
        }
        delete []configInfo;
    }

    auto msg = std::make_shared<RilRequestCdmaSetBroadcastSmsConfigMessage>(context,
            std::move(configVec));
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            ([context]([[maybe_unused]] std::shared_ptr<Message> msg,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = resp->errorCode;
                }
                sendResponse(context, errorCode, nullptr);
            }));
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCdmaSmsBroadcastActivation(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching CDMA SMS broadcast activation request");
    int32_t value;
    p.read(value);
    auto msg = std::make_shared<RilRequestCdmaSmsBroadcastActivateMessage>(context, value == 0);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] ([[maybe_unused]] std::shared_ptr<Message> msg,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = resp->errorCode;
                }
                sendResponse(context, errorCode, nullptr);
            });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCdmaWriteSmsToRuim(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching CDMA write sms to RUIM request");
    RIL_CDMA_SMS_WriteArgs args;
    p.read(args);
    auto msg = std::make_shared<RilRequestCdmaWriteSmsToRuimMessage>(context,
        args.message, args.status);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] ([[maybe_unused]] std::shared_ptr<Message> msg,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                auto p = std::make_shared<Marshal>();
                if (p == nullptr) {
                    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
                } else {
                    RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                    if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                        errorCode = resp->errorCode;
                        auto writeInfo =
                            std::static_pointer_cast<RilWriteSmsToSimResult_t>(resp->data);
                        if(writeInfo) {
                            p->write(writeInfo->recordNumber);
                        }
                    }
                    sendResponse(context, errorCode, p);
                }
            });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCdmaDeleteSmsOnRuim(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching CDMA delete sms to RUIM request");
    int32_t index;
    p.read(index);
    auto msg = std::make_shared<RilRequestCdmaDeleteSmsOnRuimMessage>(context, index);
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] ([[maybe_unused]] std::shared_ptr<Message> msg,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                    errorCode = resp->errorCode;
                }
                sendResponse(context, errorCode, nullptr);
            });
            msg->setCallback(&cb);
            msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchCdmaSubscription(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatch get CDMA subscription");
    auto msg = std::make_shared<RilRequestCdmaSubscriptionMessage>(context);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                auto p = std::make_shared<Marshal>();
                if (p == nullptr) {
                    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
                } else {
                    RIL_CDMASubInfo info{};
                    RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                    if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                        errorCode = resp->errorCode;
                        auto rilResult = std::static_pointer_cast<
                                qcril::interfaces::RilGetCdmaSubscriptionResult_t>(resp->data);
                        if (rilResult) {
                            info.mdn = rilResult->mdn;
                            info.hSid = rilResult->hSid;
                            info.hNid = rilResult->hNid;
                            info.min = rilResult->min;
                            info.prl = rilResult->prl;
                        }
                    }
                    p->write<RIL_CDMASubInfo>(info);
                    sendResponse(context, errorCode, p);
                }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

void dispatchSetInitialAttachApn(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("dispatchSetInitialAttachApn enter\n");
    using namespace rildata;
    int32_t serial =0;

    RIL_DataProfileInfo_IRadio_1_4 dataProfileInfo;
    memset(&dataProfileInfo, 0, sizeof(dataProfileInfo));
    if (p.read(dataProfileInfo) == Marshal::Result::FAILURE)
    {
        RLOGI("SetInitialAttachApnRequestMessage could not be dispatched. Marshal Read Failed.");
        return;
    }
    rildata::DataProfileInfo_t profileInfo = {};
    profileInfo.profileId = (DataProfileId_t)dataProfileInfo.profileId;
    RLOGI("dataProfileInfo.profileId = %d\n", dataProfileInfo.profileId);
    if (dataProfileInfo.apn) {
        RLOGI("apn = %s\n", dataProfileInfo.apn);
        profileInfo.apn = std::string(dataProfileInfo.apn);
    }
    if (dataProfileInfo.protocol) {
        RLOGI("protocol = %s\n",  dataProfileInfo.protocol);
        profileInfo.protocol = std::string(dataProfileInfo.protocol);
    }
    if (dataProfileInfo.roamingProtocol) {
        RLOGI("dataProfileInfo.roamingProtocol = %s\n", dataProfileInfo.roamingProtocol);
        profileInfo.roamingProtocol = std::string(dataProfileInfo.roamingProtocol);
    }
    profileInfo.authType = (ApnAuthType_t)dataProfileInfo.authType;
    RLOGI("dataProfileInfo.authType = %d\n", dataProfileInfo.authType);
    if (dataProfileInfo.user) {
        RLOGI("dataProfileInfo.user = %s\n", dataProfileInfo.user);
        profileInfo.username = std::string(dataProfileInfo.user);
    }
    if (dataProfileInfo.password) {
        RLOGI("dataProfileInfo.password = %s\n", dataProfileInfo.password);
        profileInfo.password = std::string(dataProfileInfo.password);
    }
    profileInfo.dataProfileInfoType = (DataProfileInfoType_t)dataProfileInfo.type;
    RLOGI("dataProfileInfo.type=%d\n", dataProfileInfo.type);
    profileInfo.maxConnsTime = dataProfileInfo.maxConnsTime;
    RLOGI("dataProfileInfo.maxConnsTime= %d\n",dataProfileInfo.maxConnsTime);
    profileInfo. maxConns = dataProfileInfo.maxConns;
    RLOGI("dataProfileInfo.maxConns=%d\n",dataProfileInfo.maxConns);
    profileInfo.waitTime = dataProfileInfo.waitTime;
    RLOGI("dataProfileInfo.waitTime=%d\n",dataProfileInfo.waitTime);
    profileInfo.enableProfile = (bool)dataProfileInfo.enabled;
    RLOGI("dataProfileInfo.enabled=%d\n",dataProfileInfo.enabled);
    profileInfo.supportedApnTypesBitmap = (ApnTypes_t)(dataProfileInfo.supportedTypesBitmask);
    RLOGI("dataProfileInfo.supportedTypesBitmask=%x\n",dataProfileInfo.supportedTypesBitmask);
    profileInfo.bearerBitmap = (RadioAccessFamily_t)(dataProfileInfo.bearerBitmask);
    RLOGI("dataProfileInfo.bearerBitmask=%x\n",dataProfileInfo.bearerBitmask);
    profileInfo.mtu = dataProfileInfo.mtu;
    RLOGI("dataProfileInfo.mtu=%d\n",dataProfileInfo.mtu);
    profileInfo.preferred = (bool)dataProfileInfo.preferred;
    RLOGI("dataProfileInfo.preferred=%d\n",dataProfileInfo.preferred);
    profileInfo.persistent = (bool)dataProfileInfo.persistent;
    RLOGI("dataProfileInfo.persistent=%d\n",dataProfileInfo.persistent);
    p.release(dataProfileInfo);
    auto msg =
        std::make_shared<SetInitialAttachApnRequestMessage>(
        serial,
        profileInfo,
        nullptr);

    GenericCallback<RIL_Errno> cb(
        ([context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
        RLOGI("received dispatchSetInitialAttachApn cb\n");
        auto p = std::make_shared<Marshal>();
        if (p == nullptr) {
            RLOGI("dispatchSetInitialAttachApn Marshal is nullptr \n");
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        } else {
            RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
            if (msg && responseDataPtr) {
                if(status == Message::Callback::Status::SUCCESS) {
                    errorCode = *responseDataPtr;
                }
                RLOGI("dispatchSetInitialAttachApn sending response \n");
                sendResponse(context, errorCode, p);
            } else {
                RLOGI("dispatchSetInitialAttachApn:: response ptr is NULL\n");
                sendResponse(context, RIL_E_NO_MEMORY, p);
            }
        }
    }));
    RLOGI("Dispatching SetInitialAttachApnRequestMessage request.");
    msg->setCallback(&cb);
    msg->dispatch();
}
void dispatchSetDataProfile(std::shared_ptr<SocketRequestContext> context, Marshal& parcel) {
    RLOGI("Dispatching dispatchSetDataProfile request.");
    using namespace rildata;
    int32_t serial =0;
    vector<rildata::DataProfileInfo_t> p;
    rildata::DataProfileInfo_t t = {};
    int32_t len = parcel.readInt32();
    RIL_DataProfileInfo_IRadio_1_4 profile;
    for (size_t i = 0; i < len; i++) {
        t = {};
        memset(&profile, 0, sizeof(profile));
        if (parcel.read(profile) == Marshal::Result::FAILURE)
        {
            RLOGI("SetDataProfileRequestMessage could not be dispatched. Marshal Read Failed.");
            return;
        }
        t.profileId = (rildata::DataProfileId_t)profile.profileId;
        if (profile.apn) {
            t.apn = std::string(profile.apn);
        }
        if (profile.protocol) {
            t.protocol = std::string(profile.protocol);
        }
        if (profile.roamingProtocol) {
            t.roamingProtocol = std::string(profile.roamingProtocol);
        }
        t.authType = (rildata::ApnAuthType_t)profile.authType;
        if (profile.user) {
            t.username = std::string(profile.user);
        }
        if (profile.password) {
            t.password = std::string(profile.password);
        }
        t.dataProfileInfoType = (rildata::DataProfileInfoType_t)profile.type;
        t.maxConnsTime = profile.maxConnsTime;
        t.maxConns = profile.maxConns;
        t.waitTime = profile.waitTime;
        t.enableProfile = (bool)profile.enabled;
        t.supportedApnTypesBitmap = (rildata::ApnTypes_t)profile.supportedTypesBitmask;
        t.bearerBitmap = (rildata::RadioAccessFamily_t)profile.bearerBitmask;
        t.mtu = profile.mtu;
        t.preferred = (bool)profile.preferred;
        t.persistent = (bool)profile.persistent;
        p.push_back(t);
        parcel.release(profile);
    }
    auto msg = std::make_shared<rildata::SetDataProfileRequestMessage>(serial, p);
    GenericCallback<RIL_Errno> cb(
        ([context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
        auto p = std::make_shared<Marshal>();
        if (p == nullptr) {
            RLOGI("dispatchSetDataProfile Marshal is nullptr \n");
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        } else {
            RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
            if (msg && responseDataPtr) {
                if(status == Message::Callback::Status::SUCCESS) {
                    errorCode = *responseDataPtr;
                }
                RLOGI("dispatchSetDataProfile sending response \n");
                sendResponse(context, errorCode, p);
            } else {
                RLOGI("dispatchSetDataProfile:: response ptr is NULL\n");
                sendResponse(context, RIL_E_NO_MEMORY, p);
            }
        }
    }));
    msg->setCallback(&cb);
    msg->dispatch();
}

void dispatchGetDataCallList(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    using namespace rildata;
    RLOGI("Dispatching dispatchGetDataCallList request.");
    auto msg = std::make_shared<GetRadioDataCallListRequestMessage>();

    GenericCallback<DataCallListResult_t> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<DataCallListResult_t> responseDataPtr) -> void {
        auto p = std::make_shared<Marshal>();
        if (p == nullptr) {
            RLOGI("dispatchGetDataCallList Marshal is nullptr ");
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        } else {
            RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
            if (msg && responseDataPtr) {
                if ((status == Message::Callback::Status::SUCCESS) && (responseDataPtr->respErr == ResponseError_t::NO_ERROR)) {
                    QCRIL_LOG_DEBUG("Call list size = %d", responseDataPtr->call.size());
                    size_t dataCallListLen = static_cast<size_t>(responseDataPtr->call.size());
                    RLOGI("dispatchGetDataCallList write length into marshalled object");
                    p->write(dataCallListLen);
                    RIL_Data_Call_Response_v11 dataCallList = {};
                    errorCode = RIL_E_SUCCESS;
                    for (rildata::DataCallResult_t entry: responseDataPtr->call) {
                        dataCallList = {};
                        RLOGI("dispatchGetDataCallList retrieving & copying call data");
                        dataCallList.status = static_cast<int>(entry.cause);
                        dataCallList.suggestedRetryTime = entry.suggestedRetryTime;
                        dataCallList.cid = entry.cid;
                        dataCallList.active = entry.active;
                        char *type_t = new char[entry.type.length()+1]{};
                        strlcpy(type_t, entry.type.c_str(),entry.type.length()+1);
                        dataCallList.type = type_t;
                        char *ifname_t = new char[entry.ifname.length()+1]{};
                        strlcpy(ifname_t, entry.ifname.c_str(),entry.ifname.length()+1);
                        dataCallList.ifname = ifname_t;
                        char *addresses_t = new char[entry.addresses.length()+1]{};
                        strlcpy(addresses_t, entry.addresses.c_str(),entry.addresses.length()+1);
                        dataCallList.addresses = addresses_t;
                        char *dnses_t = new char[entry.dnses.length()+1]{};
                        strlcpy(dnses_t, entry.dnses.c_str(),entry.dnses.length()+1);
                        dataCallList.dnses = dnses_t;
                        char *gateways_t = new char[entry.gateways.length()+1]{};
                        strlcpy(gateways_t, entry.gateways.c_str(),entry.gateways.length()+1);
                        dataCallList.gateways = gateways_t;
                        char *pcscf_t = new char[entry.pcscf.length()+1]{};
                        strlcpy(pcscf_t, entry.pcscf.c_str(),entry.pcscf.length()+1);
                        dataCallList.pcscf = pcscf_t;
                        dataCallList.mtu = entry.mtu;
                        p->write(dataCallList);
                        p->release(dataCallList);
                    }
                } else {
                    switch (responseDataPtr->respErr) {
                        RLOGI("dispatchGetDataCallList  return status NOT Success");
                        case ResponseError_t::NOT_SUPPORTED: errorCode = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED; break;
                        case ResponseError_t::INVALID_ARGUMENT: errorCode = RIL_Errno::RIL_E_INVALID_ARGUMENTS; break;
                        case ResponseError_t::CALL_NOT_AVAILABLE: errorCode = RIL_Errno::RIL_E_INVALID_CALL_ID; break;
                        default: errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE; break;
                    }
                }
                sendResponse(context, errorCode, p);
            } else {
                RLOGI("dispatchGetDataCallList  response ptr is NULL");
                sendResponse(context, RIL_E_NO_MEMORY, p);
            }
        }
    });
    if (msg) {

        msg->setCallback(&cb);
        msg->dispatch();
    }
}

void dispatchGetAdnRecord(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching RIL_REQUEST_GET_ADN_RECORD request.");
    auto msg = std::make_shared<QcRilRequestGetAdnRecordMessage>(context);
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            ([context](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                std::shared_ptr<QcRilRequestMessageCallbackPayload> responseDataPtr) -> void {
                QCRIL_NOTUSED( status );
                std::shared_ptr<Marshal> parcel = nullptr;
                RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                if (solicitedMsg && responseDataPtr) {
                    errorCode = responseDataPtr->errorCode;
                    if(responseDataPtr->data) {
                        parcel = std::make_shared<Marshal>();
                        if (parcel) {
                            qcril::interfaces::qcril_pbm_adn_count_info adnCountInfo =
                                std::static_pointer_cast<qcril::interfaces::AdnCountInfoResp>(
                                responseDataPtr->data)->getAdnCountInfo();
                            RIL_AdnCountInfo response{
                                .max_adn_num     = adnCountInfo.max_adn_num,
                                .valid_adn_num   = adnCountInfo.valid_adn_num,
                                .max_email_num   = adnCountInfo.max_email_num,
                                .valid_email_num = adnCountInfo.valid_email_num,
                                .max_ad_num      = adnCountInfo.max_ad_num,
                                .valid_ad_num    = adnCountInfo.valid_ad_num,
                                .max_name_len    = adnCountInfo.max_name_len,
                                .max_number_len  = adnCountInfo.max_number_len,
                                .max_email_len   = adnCountInfo.max_email_len,
                                .max_anr_len     = adnCountInfo.max_anr_len,
                            };
                            parcel->write(response);
                        }
                    }
                    sendResponse(context, errorCode, parcel);
                }
        }));
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }

    QCRIL_LOG_FUNC_RETURN();
}

void dispatchUpdateAdnRecord(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    RLOGI("Dispatching RIL_REQUEST_UPDATE_ADN_RECORD request.");
    RIL_AdnRecordInfo inRecordData{};
    if (p.read(inRecordData) == Marshal::Result::SUCCESS) {
        qcril::interfaces::AdnRecordInfo recordData{
            .record_id = static_cast<int>(inRecordData.record_id),
                .name = inRecordData.name,
                .number = inRecordData.number,
                .email_elements = static_cast<int>(inRecordData.email_elements),
                .anr_elements = static_cast<int>(inRecordData.anr_elements),
        };
        delete [] inRecordData.name;
        delete [] inRecordData.number;
        for(size_t i = 0; i < inRecordData.email_elements; i++) {
            recordData.email[i] = inRecordData.email[i];
            delete [] inRecordData.email[i];
        }
        for(size_t i = 0; i < inRecordData.anr_elements; i++) {
            recordData.ad_number[i] = inRecordData.ad_number[i];
            delete [] inRecordData.ad_number[i];
        }
        auto msg = std::make_shared<QcRilRequestUpdateAdnRecordMessage>(context, recordData);
        if (msg) {
            GenericCallback<QcRilRequestMessageCallbackPayload> cb(
                                ([context](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                                                                        std::shared_ptr<QcRilRequestMessageCallbackPayload> responseDataPtr) -> void {
                                    RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                                    if (status == Message::Callback::Status::SUCCESS) {
                                        if (responseDataPtr) {
                                            errorCode = responseDataPtr->errorCode;
                                        }
                                    }
                                    sendResponse(context, errorCode, nullptr);
                                 }));
            msg->setCallback(&cb);
            msg->dispatch();
        }
    }

}

}
}
}
