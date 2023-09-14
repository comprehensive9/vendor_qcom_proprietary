/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <Marshal.h>
#include <marshal/Call.h>
#include <interfaces/voice/QcRilRequestGetCurrentCallsMessage.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

RIL_CallState convertCallState(qcril::interfaces::CallState in) {
  switch (in) {
    case qcril::interfaces::CallState::ACTIVE:
      return RIL_CALL_ACTIVE;
    case qcril::interfaces::CallState::HOLDING:
      return RIL_CALL_HOLDING;
    case qcril::interfaces::CallState::DIALING:
      return RIL_CALL_DIALING;
    case qcril::interfaces::CallState::ALERTING:
      return RIL_CALL_ALERTING;
    case qcril::interfaces::CallState::INCOMING:
      return RIL_CALL_INCOMING;
    case qcril::interfaces::CallState::WAITING:
      return RIL_CALL_WAITING;
    case qcril::interfaces::CallState::END:
      return RIL_CALL_END;
    default:
      return RIL_CALL_ACTIVE;
  }
  return RIL_CALL_ACTIVE;
}

void dispatchGetCurrentCalls(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  do {
    auto msg = std::make_shared<QcRilRequestGetCurrentCallsMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      break;
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::vector<RIL_Call> calls;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto currentCalls =
                std::static_pointer_cast<qcril::interfaces::GetCurrentCallsRespData>(resp->data);
            if (currentCalls) {
              auto callList = currentCalls->getCallInfoList();
              if (!callList.empty()) {
                for (uint32_t i = 0; i < callList.size(); i++) {
                  RIL_Call call = {};
                  call.state = convertCallState(callList[i].getCallState());
                  call.index = callList[i].getIndex();
                  call.toa = callList[i].getToa();
                  call.isMpty = callList[i].getIsMpty();
                  call.isMT = callList[i].getIsMt();
                  call.als = callList[i].getAls();
                  call.isVoice = callList[i].getIsVoice();
                  call.isVoicePrivacy = callList[i].getIsVoicePrivacy();
                  call.number = nullptr;
                  if (callList[i].hasNumber()) {
                    auto number = callList[i].getNumber();
                    if (!number.empty()) {
                      call.number = new char[number.size() + 1]();
                      if (call.number) {
                        number.copy(call.number, number.size() + 1);
                      }
                    }
                  }
                  call.numberPresentation = callList[i].getNumberPresentation();
                  call.name = nullptr;
                  if (callList[i].hasName()) {
                    auto name = callList[i].getName();
                    if (!name.empty()) {
                      call.name = new char[name.size() + 1]();
                      if (call.name) {
                        name.copy(call.name, name.size() + 1);
                      }
                    }
                  }
                  call.namePresentation = callList[i].getNamePresentation();
                  call.uusInfo = nullptr;
                  if (callList[i].hasUusInfo()) {
                    auto uusInfo = callList[i].getUusInfo();
                    if (uusInfo != nullptr) {
                      call.uusInfo = new RIL_UUS_Info();
                      if (call.uusInfo) {
                        call.uusInfo->uusType = uusInfo->getType();
                        call.uusInfo->uusDcs = uusInfo->getDcs();
                        auto uusData = uusInfo->getData();
                        call.uusInfo->uusLength = uusData.size();
                        if (!uusData.empty()) {
                          call.uusInfo->uusData = new char[call.uusInfo->uusLength]();
                          if (call.uusInfo->uusData) {
                            uusData.copy(call.uusInfo->uusData, call.uusInfo->uusLength);
                          }
                        }
                      }
                    }
                  }
                  call.audioQuality = callList[i].getAudioQuality();

                  calls.push_back(call);
                }
              }
            }
          }
          auto p = std::make_shared<Marshal>();
          int32_t numCalls = calls.size();
          if (numCalls) {
            if (p) {
              p->write(numCalls);
              for (auto call : calls) {
                p->write(call);
              }
            }
            // Free the calls objects
            for (auto call : calls) {
              delete[] call.number;
              delete[] call.name;
              if (call.uusInfo) {
                delete[] call.uusInfo->uusData;
              }
              delete call.uusInfo;
            }
          }
          sendResponse(context, errorCode, p);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
