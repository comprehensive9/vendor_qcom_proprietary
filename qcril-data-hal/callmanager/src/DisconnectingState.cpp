/******************************************************************************
#  Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "framework/Log.h"
#include "CallState.h"
#include "CallManager.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "UnSolMessages/SetPreferredSystemMessage.h"
#include "UnSolMessages/QosDataChangedIndMessage.h"
#include "DataConfig.h"

using namespace std;
using namespace SM;
using namespace rildata;

void DisconnectingState::enter(void) {
  if (callInfo.pendingAck) {
    sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), callInfo.preferredRAT);
    callInfo.pendingAck = false;
  }

  if(((int)callInfo.profileInfo->getApnTypes()) & ((int)ApnTypes_t::IMS))
  {
    deInitializeQos();
    QosParamResult_t result = {};
    result.cid = callInfo.cid;
    auto QosDataChangedMsg = std::make_shared<QosDataChangedIndMessage>(result);
    QosDataChangedMsg->broadcast();
  }

  auto m = std::static_pointer_cast<DeactivateDataCallRequestMessage>(callInfo.request);
  if(m != nullptr && m->getDataRequestReason() == DataRequestReason_t::SHUTDOWN) {
    startTimer(DataCallTimerType::DeactivateDataCall, (TimeKeeper::millisec)deactivateShutDownTimer * 1000);
  } else {
    startTimer(DataCallTimerType::DeactivateDataCall, (TimeKeeper::millisec)DEACTIVATE_DATA_CALL_TIMEOUT_SECONDS * 1000);
  }

  response = ResponseError_t::NO_ERROR;
}

void DisconnectingState::exit(void) {
  stopTimer(DataCallTimerType::DeactivateDataCall);
  deactivateDataCallCompleted(response);
}

int DisconnectingState::handleEvent(const CallEventType& event){
  switch (event.type) {
    case CallEventTypeEnum::SetupDataCall:
    {
      Log::getInstance().d("[" + getName() + "]: Setupdatacall with the same parameters is requested");
      shared_ptr<SetupDataCallRequestMessage> m =
        std::static_pointer_cast<SetupDataCallRequestMessage>(event.msg);
      SetupDataCallResponse_t result = {};
      result.respErr = ResponseError_t::NO_ERROR;
      result.call = {.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_1};
      result.call.suggestedRetryTime = -1;
      sendSetupDataCallResponse(m, result);
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::RilEventDataCallback:
    {
      shared_ptr<RilEventDataCallback> m =
        std::static_pointer_cast<RilEventDataCallback>(event.msg);
      if (m != nullptr) {
        EventDataType *pEvtData = m->getEventData();
        dsi_net_evt_t netEvent = pEvtData->evt;
        switch (netEvent) {
          case DSI_EVT_NET_NO_NET:
            transitionTo(_eDisconnected);
            break;
          default:
            break;
        }
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::DeactivateDataCall:
    {
      Log::getInstance().d("[" + getName() + "]: DeactivateDataCall with the same parameters is requested");
      shared_ptr<DeactivateDataCallRequestMessage> m =
        std::static_pointer_cast<DeactivateDataCallRequestMessage>(event.msg);
      if (m != nullptr) {
        if (m->getDataRequestReason() == DataRequestReason_t::HANDOVER) {
          sendDeactivateDataCallResponse(m, ResponseError_t::NO_ERROR);
        } else {
          sendDeactivateDataCallResponse(m, ResponseError_t::INTERNAL_ERROR);
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::SetPreferredSystem:
    {
      std::shared_ptr<SetPreferredSystemMessage> m =
        std::static_pointer_cast<SetPreferredSystemMessage>(event.msg);
      if (m != nullptr) {
        Log::getInstance().d("[" + getName() + "]: Sending Ack to Modem");
        sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), m->getPrefNetwork());
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::TimerExpired:
    {
      shared_ptr<DataCallTimerExpiredMessage> m =
        std::static_pointer_cast<DataCallTimerExpiredMessage>(event.msg);
      if (m != nullptr) {
        if (m->getType() == DataCallTimerType::DeactivateDataCall) {
          transitionTo(_eDisconnected);
          Log::getInstance().d("Deactivate data call timer expired");
        }
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::CleanUp:
    {
        transitionTo(_eIdle);
        return IState<CallEventType>::HANDLED;
    }
    default:
      return IState<CallEventType>::UNHANDLED;
  }
}

void DisconnectingState::setDeactivateShutDownTimer(void) {
  #ifdef RIL_FOR_MDM_LE
  propertySetMap("persist.vendor.radio.shutdown_deactivate_timer", PERSIST_VENDOR_RADIO_SHUTDOWN_DEACTIVATE_TIMER);
  #endif
  string args = readProperty("persist.vendor.radio.shutdown_deactivate_timer", "90");
  if(!args.empty())
  {
    int timer = stoi(args);
    if(timer != 0) {
      deactivateShutDownTimer = timer;
      Log::getInstance().d("deactivate timer is set to " + std::to_string(deactivateShutDownTimer));
    }
  }
}
