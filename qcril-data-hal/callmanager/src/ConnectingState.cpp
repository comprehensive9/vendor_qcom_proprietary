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
#include "modules/nas/NasEmbmsEnableDataConMessage.h"
#include "modules/nas/NasEmbmsDisableDataMessage.h"

using namespace std;
using namespace SM;
using namespace rildata;

// avoid macro naming conflict with Message::Callback::Status::SUCCESS
extern "C" int qcril_data_get_ifindex(const char * dev, int * ifindex);

void ConnectingState::enter(void) {
  startTimer(DataCallTimerType::SetupDataCall, (TimeKeeper::millisec)SETUP_DATA_CALL_TIMEOUT_SECONDS * 1000);
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
}

void ConnectingState::exit(void) {
  stopTimer(DataCallTimerType::SetupDataCall);
  response.call.suggestedRetryTime = -1;
  if (response.respErr == ResponseError_t::NO_ERROR &&
      response.call.cause == DataCallFailCause_t::NONE) {
    response.call.suggestedRetryTime = -1;
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

    if(((int)callInfo.profileInfo->getApnTypes()) & ((int)ApnTypes_t::IMS))
    {
      initializeQos();
    }
  }
  setupDataCallCompleted(response);
}

int ConnectingState::handleEvent(const CallEventType& event) {
  switch (event.type) {
    case CallEventTypeEnum::SetupDataCall:
    {
      Log::getInstance().d("setupdatacall with the same parameters is requested");
      shared_ptr<SetupDataCallRequestMessage> m =
        std::static_pointer_cast<SetupDataCallRequestMessage>(event.msg);
      SetupDataCallResponse_t result = {};
      result.respErr = ResponseError_t::NO_ERROR;
      result.call = {.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_1};
      result.call.suggestedRetryTime = -1;
      sendSetupDataCallResponse(m, result);
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::DeactivateDataCall:
    {
      //Telephony couldn't request for deactivate data in connectingstate as
      //it is not aware about cid value in connectingState.
      //so if there is a race condition and deactivate data is received in this state,
      //it may be for the older call. So sending success from here as
      //older call with the same cid is already disconnected.
      shared_ptr<DeactivateDataCallRequestMessage> m =
        std::static_pointer_cast<DeactivateDataCallRequestMessage>(event.msg);
      if(m != nullptr)
      {
        sendDeactivateDataCallResponse(m, ResponseError_t::NO_ERROR);
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::RilEventDataCallback:
    {
      shared_ptr<RilEventDataCallback> m =
        std::static_pointer_cast<RilEventDataCallback>(event.msg);
      if (m != nullptr) {
        EventDataType *pEvtData = m->getEventData();
        if (pEvtData != nullptr) {
          dsi_net_evt_t netEvent = pEvtData->evt;
          switch (netEvent) {
            case DSI_EVT_NET_IS_CONN:
            {
              callInfo.active = _eActivePhysicalLinkUp;
              IpFamilyType_t ipType = IpFamilyType_t::IPv4v6;
              configureParams(ipType);

              // call dsi interface to check whether iface has valid address while we are handling IS_CONN.
              if(!callInfo.isEmbmsCall && (dsiGetIPAddrCount(callInfo.dsiHandle) == 0))
              {
                callInfo.v4Connected = false;
                callInfo.v6Connected = false;
                Log::getInstance().d("iface is down whille handle IS_CONN, wait NO_NET");
                break;
              }

              if (callInfo.isEmbmsCall) {
                qcril_embms_enable_response_payload_type enableResp;
                memset(&enableResp, 0, sizeof(enableResp));
                enableResp.call_id = callInfo.cid;
                enableResp.cause = RIL_E_OEM_ERROR_4;
                enableResp.resp_code = (int32_t)EmbmsDataCallFailCause_t::ERROR_UNKNOWN;

                int if_index;
                string device_name;
                /*if callstate is errorstate then skip the below condition and send error response */
                if(dsiGetDeviceName(callInfo.dsiHandle, device_name) == DSI_SUCCESS && qcril_data_get_ifindex(callInfo.deviceName.c_str(), &if_index) == DSI_SUCCESS)
                {
                  enableResp.cause = RIL_E_SUCCESS;
                  enableResp.resp_code = (int32_t)EmbmsDataCallFailCause_t::ERROR_NONE;
                  strlcpy(enableResp.interafce_name, callInfo.deviceName.c_str(), QCRIL_EMBMS_INTERFACE_NAME_LENGTH_MAX);
                  enableResp.if_index = if_index;
                }

                auto msg = std::make_shared<NasEmbmsEnableDataConMessage>(enableResp);
                msg->dispatch();
              }
              createQmiWdsClients();
              transitionTo(_eConnected);
              break;
            }
            case DSI_EVT_NET_NO_NET:
            {
              dsi_ce_reason_t dsiReason = getVerboseCallEndReason();
              if ((dsiReason.reason_type == DSI_CE_TYPE_INTERNAL) &&
                  (dsiReason.reason_code == WDS_VCER_INTERNAL_APN_TYPE_MISMATCH_V01) &&
                  (callInfo.callBringUpCapability != BRING_UP_LEGACY)){
                Log::getInstance().d("call failed for cid = " + std::to_string(callInfo.cid)
                             + " due to APN type mismatch, fallback to legacy bringup");
                callInfo.callBringUpCapability = BRING_UP_LEGACY;
                if (callInfo.dsiHandle != nullptr) {
                  dsiRelDataSrvcHndl(callInfo.dsiHandle);
                  callInfo.dsiHandle = nullptr;
                }
                if (callInfo.profileInfo != nullptr && callInfo.profileInfo->getIsEmergencyCall()) {
                  if (setEmergencyProfileInfo()) {
                    if (DSI_SUCCESS != dsiStartDataCall(callInfo.dsiHandle)) {
                      Log::getInstance().d("Invalid apn configuration");
                      response.call.cause = CallEndReason::getCallEndReason(
                          getVerboseCallEndReason());
                      transitionTo(_eDisconnected);
                    }
                    break;
                  }
                }
                if (setProfileInfo()) {
                  if (DSI_SUCCESS != dsiStartDataCall(callInfo.dsiHandle)) {
                    Log::getInstance().d("Invalid apn configuration");
                    response.call.cause = CallEndReason::getCallEndReason(
                        getVerboseCallEndReason());
                    transitionTo(_eDisconnected);
                  }
                } else {
                  transitionTo(_eDisconnected);
                }
                return IState<CallEventType>::HANDLED;
              }
              else {
                response.call.cause = CallEndReason::getCallEndReason(dsiReason);
                transitionTo(_eDisconnected);
              }

              qcril_embms_disable_indication_payload_type disableResp;
              memset(&disableResp,0,sizeof(disableResp));

              if((dsiGetCallTech(callInfo.dsiHandle)) == DSI_EXT_TECH_EMBMS)
                {
                disableResp.call_id = callInfo.cid;
                disableResp.cause = dsiGetCallEndReason(callInfo.dsiHandle);
                disableResp.resp_code = (int32_t)EmbmsDataCallFailCause_t::ERROR_UNKNOWN;

                auto msg = std::make_shared<NasEmbmsDisableDataMessage>(disableResp);
                msg->dispatch();
              }
              break;
            }
            default:
#ifndef RIL_FOR_LOW_RAM
              if(callInfo.isEmbmsCall && callInfo.embmsInfo!= nullptr && pEvtData != nullptr)
              {
                callInfo.embmsInfo->processEvents(netEvent, &pEvtData->payload.embms_info);
                return IState<CallEventType>::HANDLED;
              }
#endif
              break;
          }
        }
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
        if (m->getType() == DataCallTimerType::SetupDataCall) {
          response.call.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_4;
          transitionTo(_eDisconnected);
        }
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::SetPreferredSystem:
    {
      std::shared_ptr<SetPreferredSystemMessage> m =
        std::static_pointer_cast<SetPreferredSystemMessage>(event.msg);
      if (m != nullptr) {
        dsiStopDataCall(callInfo.dsiHandle);
        response.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
        sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, getCallApn(), m->getPrefNetwork());
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
