/******************************************************************************
#  Copyright (c) 2018-2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "framework/Log.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "UnSolMessages/SetPreferredSystemMessage.h"
#include "UnSolMessages/HandoverInformationIndMessage.h"
#include "UnSolMessages/QosDataChangedIndMessage.h"
#include "event/LinkPropertiesChangedMessage.h"
#include "modules/nas/NasEmbmsDisableDataMessage.h"
#include "CallInfo.h"
#include "CallState.h"
#include "CallManager.h"

#include "DSDModemEndPoint.h"

using namespace std;
using namespace SM;
using namespace rildata;

void ConnectedState::enter(void) {
  partialRetryCount = 0;
  dataCallListChanged();
  if (isPartiallyConnected()) {
    startPartialRetryTimerForTemporaryFailure();
  }
}

void ConnectedState::exit(void) {
  stopTimer(DataCallTimerType::PartialRetry);
  stopTimer(DataCallTimerType::PartialRetryResponse);
  partialRetryCount = 0;
}

TimeKeeper::millisec ConnectedState::getTimeoutMillis(unsigned long maxTimeoutMillis) {
  // Timeout value doubles after each retry until MAX is reached
  TimeKeeper::millisec defaultTimeoutMillis = (TimeKeeper::millisec)DEFAULT_PARTIAL_RETRY_TIMEOUT * 1000;
  TimeKeeper::millisec timeoutMillis = defaultTimeoutMillis << partialRetryCount;
  // handle integer overflow
  if (timeoutMillis <= 0 || timeoutMillis > maxTimeoutMillis) {
    timeoutMillis = maxTimeoutMillis;
  }
  return timeoutMillis;
}

bool ConnectedState::isPartiallyConnected() {
  // if dual ip call but not all IPs have connected
  return (callInfo.requestedIpType == "IPV4V6" &&
      (!callInfo.v4Connected || !callInfo.v6Connected));
}

void ConnectedState::startPartialRetryTimerForTemporaryFailure() {
  if (!globalInfo.partialRetryEnabled) {
    Log::getInstance().d("Partial retry isn't Enabled!");
    return;
  }
  DataCallFailCause_t consolidatedReason = CallEndReason::getCallEndReason(getVerboseCallEndReason());
  if (!CallEndReason::isPermanentFailure(consolidatedReason)) {
    auto timeoutMillis = getTimeoutMillis(globalInfo.maxPartialRetryTimeout * 1000);
    Log::getInstance().d("partial retry timer started for " +
                         std::to_string((unsigned long)timeoutMillis) + "ms");
    startTimer(DataCallTimerType::PartialRetry, timeoutMillis);
  } else {
    Log::getInstance().d("Skipping partial retry due to permanent CE reason");
  }
}

void ConnectedState::startPartialRetryResponseTimer() {
  TimeKeeper::millisec timeoutMillis = SETUP_DATA_CALL_TIMEOUT_SECONDS * 1000;
  startTimer(DataCallTimerType::PartialRetryResponse, timeoutMillis);
}

void ConnectedState::initiatePartialRetry() {
  dsi_call_param_value_t partialRetry;
  partialRetry.buf_val = nullptr;
  partialRetry.num_val = TRUE;
  if (DSI_SUCCESS != dsiSetDataCallParam(callInfo.dsiHandle, DSI_CALL_INFO_PARTIAL_RETRY, &partialRetry)) {
    Log::getInstance().d("Unable to set partial retry param");
    return;
  }
  // this event will only come after temporary failure
  if (DSI_SUCCESS != dsiStartDataCall(callInfo.dsiHandle)) {
    Log::getInstance().d("dsiStartDataCall failed");
    return;
  }
  startPartialRetryResponseTimer();
  partialRetryCount++;
}

int ConnectedState::handleEvent(const CallEventType& event) {
  switch (event.type) {
    case CallEventTypeEnum::SetupDataCall:
    {
      std::shared_ptr<SetupDataCallRequestMessage> m =
        std::static_pointer_cast<SetupDataCallRequestMessage>(event.msg);
      if (m != nullptr) {
        HandoffNetworkType_t NetType = (m->getAccessNetwork() == AccessNetwork_t::IWLAN)?
                            HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
        if ((m->getDataRequestReason() == DataRequestReason_t::HANDOVER) &&
          ((NetType & callInfo.currentRAT) == 0)) {
          HandoffNetworkType_t NetType = (m->getAccessNetwork() == AccessNetwork_t::IWLAN)?
            HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
          callInfo.request = m;
          // Handover scenario
          Log::getInstance().d("request reason handover with current and preferred network types different");
          callInfo.preferredRAT = NetType;
          Log::getInstance().d("Handover for Apn = ["+m->getApn()+
                          "] from ["+std::to_string((int)callInfo.currentRAT)+
                          "] to ["+std::to_string((int)callInfo.preferredRAT)+"]");
          if (callInfo.profileInfo != nullptr) {
            HandoffNetworkType_t prefSys = callInfo.preferredRAT;
            if (callInfo.pendingAck) {
              sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), prefSys);
              callInfo.pendingAck = false;
            }
          }
          transitionTo(_eHandover);
        } else {
          SetupDataCallResponse_t result = {};
          if ((m->getRequestSource()==RequestSource_t::RADIO &&
              callInfo.currentRAT==HandoffNetworkType_t::_eIWLAN) ||
              (m->getRequestSource()==RequestSource_t::IWLAN &&
              callInfo.currentRAT==HandoffNetworkType_t::_eWWAN)) {
            Log::getInstance().d("Request APN connection is already exist on another transport");
            result.respErr = ResponseError_t::NO_ERROR;
            result.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_9;
            result.call.suggestedRetryTime = -1;
          }
          else {
            //Respond with success either when request reason is NORMAL or
            //request reason is HANDOVER and prefNetwork == currentNetwork
            Log::getInstance().d("Respond with existing call because the request reason is "+
                                  std::to_string((int)m->getDataRequestReason())+
                                  ", or the requested preferred network type is "+
                                  std::to_string((int)NetType)+
                                  " while current network type is "+
                                  std::to_string((int)callInfo.currentRAT));

            result.respErr = ResponseError_t::NO_ERROR;
            result.call.cause = DataCallFailCause_t::NONE;
            result.call.suggestedRetryTime = -1;
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
          }
          sendSetupDataCallResponse(m, result);
        }
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
        callInfo.request = m;
        if (m->getDataRequestReason() == DataRequestReason_t::HANDOVER) {
          // Perform bitwise& to set network type to preferred only
          HandoffNetworkType_t target = static_cast<HandoffNetworkType_t>(
              callInfo.preferredRAT & callInfo.currentRAT);
          callInfo.currentRAT = target;
          deactivateDataCallCompleted(ResponseError_t::NO_ERROR);
          dataCallListChanged();
        } else {
          dsiStopDataCall(callInfo.dsiHandle);
          transitionTo(_eDisconnecting);
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

        IpFamilyType_t ipType = IpFamilyType_t::IPv4v6;
        switch (netEvent) {
          case DSI_EVT_NET_IS_CONN:
            stopTimer(DataCallTimerType::PartialRetryResponse);
            partialRetryCount = 0;
            callInfo.active = _eActivePhysicalLinkUp;
            configureParams(ipType);
            if (isPartiallyConnected()) {
              startPartialRetryTimerForTemporaryFailure();
            }
            break;

          case DSI_EVT_NET_PARTIAL_CONN:
            // partial retry failure case
            startPartialRetryTimerForTemporaryFailure();
            break;

          case DSI_EVT_NET_NEWMTU:
            determineIpTypeAndConfigureParams(ipType);
            break;

          case DSI_EVT_NET_NEWADDR: [[fallthrough]];
          case DSI_EVT_NET_RECONFIGURED:
            // once both IPs have connected, stop partial retry attempts
            callInfo.active = _eActivePhysicalLinkUp;
            determineIpTypeAndConfigureParams(ipType);
            if (!isPartiallyConnected()) {
              Log::getInstance().d("Dual ip connection established");
              stopTimer(DataCallTimerType::PartialRetry);
              stopTimer(DataCallTimerType::PartialRetryResponse);
              partialRetryCount = 0;
              createQmiWdsClients();
            }
            break;

          case DSI_EVT_NET_NO_NET:
            callInfo.active = _eInactive;
            callInfo.v4Connected = false;
            callInfo.v6Connected = false;
            callInfo.cause = CallEndReason::getCallEndReason(getVerboseCallEndReason());
            callInfo.callParamsChanged = true;

            if(callInfo.isEmbmsCall)
            {
              qcril_embms_disable_indication_payload_type disableResp;
              memset(&disableResp,0,sizeof(disableResp));
              disableResp.call_id = callInfo.cid;
              disableResp.cause = dsiGetCallEndReason(callInfo.dsiHandle);
              disableResp.resp_code = (int32_t)EmbmsDataCallFailCause_t::ERROR_UNKNOWN;

              auto msg = std::make_shared<NasEmbmsDisableDataMessage>(disableResp);
              msg->dispatch();
            }
            transitionTo(_eDisconnected);
            break;

          case DSI_EVT_NET_DELADDR:
            callInfo.v4HandedOver = HandOverState::Success;
            callInfo.v6HandedOver = HandOverState::Success;
            configureParams(ipType);
            if (isPartiallyConnected()) {
              startPartialRetryTimerForTemporaryFailure();
            }
            break;

          default:
#ifndef RIL_FOR_LOW_RAM
            if(callInfo.isEmbmsCall)
            {
              callInfo.embmsInfo->processEvents(netEvent, &pEvtData->payload.embms_info);
              return IState<CallEventType>::HANDLED;
            }
#endif
            break;
        }
      }
      dataCallListChanged();
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::TimerExpired:
    {
      std::shared_ptr<DataCallTimerExpiredMessage> m =
        std::static_pointer_cast<DataCallTimerExpiredMessage>(event.msg);
      if (m != nullptr) {
        switch (m->getType()) {
          case DataCallTimerType::PartialRetry:
          {
            initiatePartialRetry();
            break;
          }
          case DataCallTimerType::PartialRetryResponse:
            startPartialRetryTimerForTemporaryFailure();
            break;
          default:
            break;
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
        switch (m->getState()) {
          case HandoffState_t::Init:
          {
            if (m->hasPreferredNetwork() && m->getPreferredNetwork() != callInfo.currentRAT) {
              callInfo.preferredRAT = m->getPreferredNetwork();
              transitionTo(_eHandover);
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
    case CallEventTypeEnum::SetPreferredSystem:
    {
      std::shared_ptr<SetPreferredSystemMessage> m =
        std::static_pointer_cast<SetPreferredSystemMessage>(event.msg);
      if (m != nullptr) {
        // If RAT is same as current, or there is already handover pending for that RAT
        if (m->getPrefNetwork() == callInfo.currentRAT ||
            m->getPrefNetwork() == callInfo.preferredRAT) {
          sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), m->getPrefNetwork());
        } else {
          callInfo.pendingAck = true;
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::LinkPropertiesChanged:
    {
      std::shared_ptr<LinkPropertiesChangedMessage> m = std::static_pointer_cast<LinkPropertiesChangedMessage>(event.msg);
      if(m != nullptr) {
        IpFamilyType_t ipType = IpFamilyType_t::IPv4v6;
        switch (m->getLinkPropertiesChanged()) {
          case LinkPropertiesChangedType_t::PcscfAddressChanged:
          {
            std::string newPcscf = "";
            determineIpTypeAndConfigureParams(ipType);
            dsiGetPcscfAddresses(callInfo.dsiHandle, newPcscf, ipType);
            if (callInfo.pcscfAddresses != newPcscf) {
              callInfo.callParamsChanged = true;
              callInfo.pcscfAddresses = newPcscf;
            }
            break;
          }
          case LinkPropertiesChangedType_t::LinkActiveStateChanged:
          {
            if (m->hasActiveState()) {
              if (globalInfo.linkStateChangeReport && callInfo.active != m->getActiveState()) {
                callInfo.callParamsChanged = true;
                callInfo.active = m->getActiveState();
              }
            }
            break;
          }
          case LinkPropertiesChangedType_t::QosDataReceived:
          {
            QosParamResult_t result = {};
            callInfo.qosSessions = m->getQosSessions();
            result.qosSessions = callInfo.qosSessions;
            result.cid = m->getCid();
            auto QosDataChangedMsg = std::make_shared<QosDataChangedIndMessage>(result);
            QosDataChangedMsg->broadcast();
            break;
          }
          default:
            break;
        }
      }
      dataCallListChanged();
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
