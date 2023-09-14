/******************************************************************************
#  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "MessageCommon.h"
#include "framework/Log.h"
#include "CallState.h"
#include "CallManager.h"
#include "UnSolMessages/SetPreferredSystemMessage.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "UnSolMessages/HandoverInformationIndMessage.h"

using namespace std;
using namespace SM;
using namespace rildata;

void HandoverState::enter(void) {
  if (callInfo.pendingAck) {
    callInfo.pendingAck = false;
  }
  startTimer(DataCallTimerType::Handover, (TimeKeeper::millisec)HANDOVER_TIMEOUT_SECONDS * 1000);
  response = {};
  response.respErr = ResponseError_t::NO_ERROR;
  response.call.suggestedRetryTime = -1;
  response.call.cause = DataCallFailCause_t::NONE;
  response.call.cid = -1;
  response.call.active = _eInactive;
  response.call.type = "";
  response.call.ifname = "";
  response.call.addresses = "";
  response.call.dnses = "";
  response.call.gateways = "";
  response.call.pcscf = "";
  response.call.mtu = 0;
  response.call.mtuV4 = 0;
  response.call.mtuV6 = 0;
  if( callInfo.v4Connected )
    callInfo.v4HandedOver = HandOverState::Pending;
  if( callInfo.v6Connected )
    callInfo.v6HandedOver = HandOverState::Pending;
  if( callInfo.v4Connected && callInfo.v6Connected )
    callInfo.dualIpHandover = true;
}

void HandoverState::exit(void) {
  stopTimer(DataCallTimerType::Handover);
  response.call.suggestedRetryTime = -1;
  if (response.call.cause == DataCallFailCause_t::NONE) {
    response.call.cid = callInfo.cid;
    response.call.active = callInfo.active;
    response.call.type = callInfo.ipType;
    response.call.ifname = callInfo.deviceName;
    response.call.addresses = callInfo.ipAddresses;
    response.call.linkAddresses = convertAddrStringToLinkAddresses(callInfo.ipAddresses);
    response.call.dnses = callInfo.dnsAddresses;
    response.call.gateways = callInfo.gatewayAddresses;
    response.call.pcscf = callInfo.pcscfAddresses;
    response.call.mtu = callInfo.mtu;
    response.call.mtuV4 = callInfo.mtuV4;
    response.call.mtuV6 = callInfo.mtuV6;
  }
  setupDataCallCompleted(response);
}

int HandoverState::handleEvent(const CallEventType& event) {
  IpFamilyType_t ipType = IpFamilyType_t::IPv4v6;
  switch (event.type) {
    case CallEventTypeEnum::SetupDataCall:
    {
      std::shared_ptr<SetupDataCallRequestMessage> m =
        std::static_pointer_cast<SetupDataCallRequestMessage>(event.msg);
      if (m != nullptr) {
        HandoffNetworkType_t NetType = (m->getAccessNetwork() == AccessNetwork_t::IWLAN)?
                             HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
        SetupDataCallResponse_t result = {};
        result.respErr = ResponseError_t::NO_ERROR;
        result.call.suggestedRetryTime = -1;
        if (m->getDataRequestReason() == DataRequestReason_t::HANDOVER) {
          if (NetType == callInfo.currentRAT) {
            result.call.cause = DataCallFailCause_t::NONE;
            result.call.cid = callInfo.cid;
            result.call.active = callInfo.active;
            result.call.type = callInfo.ipType;
            result.call.ifname = callInfo.deviceName;
            result.call.addresses = callInfo.ipAddresses;
            result.call.linkAddresses = convertAddrStringToLinkAddresses(callInfo.ipAddresses);
            result.call.dnses = callInfo.dnsAddresses;
            result.call.gateways = callInfo.gatewayAddresses;
            result.call.pcscf = callInfo.pcscfAddresses;
            result.call.mtu = callInfo.mtu;
            result.call.mtuV4 = callInfo.mtuV4;
            result.call.mtuV6 = callInfo.mtuV6;

          } else {
            /*store the request when there is any delay from the framework in sending
              setupData handoff when modem has already started the HO*/
            if(callInfo.request == nullptr) {
              callInfo.request = m;
              return IState<CallEventType>::HANDLED;
            } else {
              result.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_9;
            }
          }
        } else if (m->getDataRequestReason() == DataRequestReason_t::NORMAL) {
          result.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_1;
        }
        sendSetupDataCallResponse(m, result);
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::DeactivateDataCall:
    {
      std::shared_ptr<DeactivateDataCallRequestMessage> m =
        std::static_pointer_cast<DeactivateDataCallRequestMessage>(event.msg);
      if (m != nullptr) {
        if (m->getDataRequestReason() == DataRequestReason_t::NORMAL) {
          dsiStopDataCall(callInfo.dsiHandle);
          response.respErr = ResponseError_t::INTERNAL_ERROR;
          response.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_4;
          transitionTo(_eDisconnecting);
          callInfo.request = m;
        } else {
          sendDeactivateDataCallResponse(m, ResponseError_t::NO_ERROR);
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::HandoverInformationInd:
    {
      std::shared_ptr<HandoverInformationIndMessage> m =
        std::static_pointer_cast<HandoverInformationIndMessage>(event.msg);
      if (m != nullptr) {
        dsi_ce_reason_t dsiReason = {
          .reason_type = static_cast<dsi_ce_reason_type_e>(CallEndReason::AP_ASSIST_HANDOVER),
          .reason_code = static_cast<int>(ApAssistHandoverFailCause_t::HANDOFF_FAILURE)
        };
        switch (m->getState()) {
          case HandoffState_t::Failure:
          {
            if(callInfo.dualIpHandover) {
              if (m->getIpType() == IpFamilyType_t::IPv4) {
                callInfo.v4HandedOver = HandOverState::Failure;
              } else if (m->getIpType() == IpFamilyType_t::IPv6) {
                callInfo.v6HandedOver = HandOverState::Failure;
              }
            }
            if( DUAL_IP_HO_BOTH_IP_HO_FAILURE || !callInfo.dualIpHandover ) {
              //If HO fails on both IP types, we send failure reason & set SRAT & move to Connected State
              if (m->hasFailReason())
              {
                auto reason = m->getFailReason();
                dsiReason = {
                    .reason_type = static_cast<dsi_ce_reason_type_e>(reason.failure_reason_type),
                    .reason_code = reason.failure_reason};
              }
              callInfo.preferredRAT = callInfo.currentRAT;
              response.call.cause = CallEndReason::getCallEndReason(dsiReason);
              configureParams(ipType);
              transitionTo(_eConnected);
            }
            else if ( DUAL_IP_HO_ONLY_ONE_IP_HO_SUCCESS )
            {
              //If HO fails on only one of the IP type, then we send success & set TRAT & move to Connected State
              // Perform bitwise| to set network type to both current and preferred
              setCurrentRATAndConfigureParams(ipType);
              transitionTo(_eConnected);
            }
            break;
          }
          case HandoffState_t::PrefSysChangedFailure:
          {
            dsiReason.reason_code = static_cast<int>(ApAssistHandoverFailCause_t::APN_PREF_SYSTEM_CHANGE_REQUEST_FAILURE);
            callInfo.preferredRAT = callInfo.currentRAT;
            response.call.cause = CallEndReason::getCallEndReason(dsiReason);
            configureParams(ipType);
            transitionTo(_eConnected);
            break;
          }
          case HandoffState_t::Success:
          {
            if (m->getIpType() == IpFamilyType_t::IPv4) {
              callInfo.v4HandedOver = HandOverState::Success;
            } else if (m->getIpType() == IpFamilyType_t::IPv6) {
              callInfo.v6HandedOver = HandOverState::Success;
            }
            if(!callInfo.dualIpHandover || DUAL_IP_HO_BOTH_IP_HO_SUCCESS) {
              //If HO is successful on both IP types, we send success & set TRAT & move to Connected State
              // Perform bitwise| to set network type to both current and preferred
              HandoffNetworkType_t target = static_cast<HandoffNetworkType_t>(
                  callInfo.preferredRAT | callInfo.currentRAT);
              callInfo.currentRAT = target;
              configureParams(ipType);
              transitionTo(_eConnected);
            } else if (DUAL_IP_HO_ONLY_ONE_IP_HO_SUCCESS) {
              //If HO fails on only one of the IP type, then we send success & set TRAT & move to Connected State
              // Perform bitwise| to set network type to both current and preferred
              setCurrentRATAndConfigureParams(ipType);
              transitionTo(_eConnected);
            }
            break;
          }
          default:
            break;
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::TimerExpired:
    {
      std::shared_ptr<DataCallTimerExpiredMessage> m =
        std::static_pointer_cast<DataCallTimerExpiredMessage>(event.msg);
      if (m != nullptr) {
        switch (m->getType()) {
          case DataCallTimerType::Handover:
          {
            response.call.cause = DataCallFailCause_t::NONE;
            HandoffNetworkType_t target = static_cast<HandoffNetworkType_t>(
                  callInfo.preferredRAT | callInfo.currentRAT);

            if( callInfo.dualIpHandover && (callInfo.v6HandedOver == HandOverState::Success) ) {
              callInfo.currentRAT = target;
              ipType = IpFamilyType_t::IPv6;
            }
            else if ( callInfo.dualIpHandover && (callInfo.v4HandedOver == HandOverState::Success) ) {
              callInfo.currentRAT = target;
              ipType = IpFamilyType_t::IPv4;
            } else {
              callInfo.preferredRAT = callInfo.currentRAT;
              response.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
            }

            configureParams(ipType);
            transitionTo(_eConnected);
            break;
          }
          default:
            break;
        }
      } else {
        Log::getInstance().d("Message is null");
      }
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
            callInfo.active = _eInactive;
            callInfo.v4Connected = false;
            callInfo.v6Connected = false;
            response.call.cause = CallEndReason::getCallEndReason(getVerboseCallEndReason());
            callInfo.cause = response.call.cause;
            transitionTo(_eDisconnected);
            dataCallListChanged();
            break;
          case DSI_EVT_NET_DELADDR:
            determineIpTypeAndConfigureParams(ipType);
            if ( callInfo.dualIpHandover ) {
              if( !( callInfo.v4Connected ) ) {
                callInfo.v4HandedOver = HandOverState::Failure;
              } else if ( !( callInfo.v6Connected ) ) {
                callInfo.v6HandedOver = HandOverState::Failure;
              }
            }
            if (DUAL_IP_HO_ONLY_ONE_IP_HO_SUCCESS) {
              //If HO fails on only one of the IP type, then we send success & set TRAT & move to Connected State
              // Perform bitwise| to set network type to both current and preferred
              setCurrentRATAndConfigureParams(ipType);
              transitionTo(_eConnected);
            }
            break;
          default:
            break;
        }
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::SetPreferredSystem:
    {
      std::shared_ptr<SetPreferredSystemMessage> m =
        std::static_pointer_cast<SetPreferredSystemMessage>(event.msg);
      if (m != nullptr) {
        sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), m->getPrefNetwork());
        // If there is already handover pending for the other RAT
        if (m->getPrefNetwork() == callInfo.currentRAT) {
            callInfo.preferredRAT = callInfo.currentRAT;
            response.call.cause = DataCallFailCause_t::HANDOFF_PREFERENCE_CHANGED;
            configureParams(ipType);
            transitionTo(_eConnected);
        }
      } else {
        Log::getInstance().d("Message is null");
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
