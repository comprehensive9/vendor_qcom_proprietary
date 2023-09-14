/******************************************************************************
#  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "framework/Log.h"

#include "CallState.h"
#include "CallManager.h"

#include "modules/nas/NasEmbmsEnableDataConMessage.h"
#include "EmbmsCallHandler.h"

using namespace SM;
using namespace rildata;

void IdleState::enter(void) {
}

void IdleState::exit(void) {
}

bool isApnTypeInvalid(ApnTypes_t apnTypes) {
  int eBit = static_cast<int>(ApnTypes_t::EMERGENCY);
  int apnTypesBitmap = static_cast<int>(apnTypes);
  // if emergency bit is set, apn types should be configured to ONLY emergency type
  // otherwise, qcrildata will treat it as a misconfiguration
  if (((eBit & apnTypesBitmap) == eBit) &&
      (eBit != apnTypesBitmap)) {
    return true;
  }
  return false;
}

bool isApnTypeEmergency(ApnTypes_t apnTypes) {
  return apnTypes == ApnTypes_t::EMERGENCY;
}

int IdleState::handleEvent(const CallEventType& event) {
  switch (event.type) {
    case CallEventTypeEnum::SetupDataCall:
    {
      std::shared_ptr<SetupDataCallRequestMessage> m =
        std::static_pointer_cast<SetupDataCallRequestMessage>(event.msg);
      if (m != nullptr) {
        HandoffNetworkType_t NetType = (m->getAccessNetwork() == AccessNetwork_t::IWLAN)?
                            HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
        callInfo.currentRAT = NetType;
        callInfo.request = m;
        callInfo.preferredRAT = NetType;
        callInfo.profileInfo = createCallSetupProfile(m);
        if (callInfo.profileInfo != nullptr) {
          callInfo.profileInfo->setAvailableRadioRAT(callInfo.radioRAT);
        }

        if (isApnTypeInvalid(m->getSupportedApnTypesBitmap())) {
          Log::getInstance().d("Invalid apn configuration");
          setupDataCallFailed(ResponseError_t::INVALID_ARGUMENT,
              DataCallFailCause_t::OEM_DCFAILCAUSE_4);
          transitionTo(_eDisconnected);
        }

        if (isApnTypeEmergency(m->getSupportedApnTypesBitmap())) {
          if (callInfo.profileInfo != nullptr) {
            callInfo.profileInfo->setIsEmergencyCall(true);
          }
          if (setEmergencyProfileInfo()) {
            Log::getInstance().d("Handle emergency data call request");
            if (DSI_SUCCESS != dsiStartDataCall(callInfo.dsiHandle)) {
              Log::getInstance().d("Invalid apn configuration");
              setupDataCallFailed(ResponseError_t::INVALID_ARGUMENT,
                  CallEndReason::getCallEndReason(
                    getVerboseCallEndReason()));
              transitionTo(_eDisconnected);
            } else {
              transitionTo(_eConnecting);
            }
            // release wakelock
            Log::getInstance().d("[" + getName() + "]: releasing the Wakelock");
            if(m->getAcknowlegeRequestCb() != nullptr) {
              auto cb = *(m->getAcknowlegeRequestCb().get());
              cb(m->getSerial());
            }
            return IState<CallEventType>::HANDLED;
          }
          // Fall back to normal setup data call handling
        }

        if (true == setProfileInfo()) {
          //Need to call DSD_SET_APN_INFO to update APN types for call if in
          //"legacy" mode
          if (!globalInfo.apAssistMode) {
            if(callInfo.dsd_endpoint != nullptr && callInfo.profileInfo != nullptr) {
              Message::Callback::Status status = Message::Callback::Status::FAILURE;
              status = callInfo.dsd_endpoint->setApnInfoSync(
                callInfo.profileInfo->getApn(),
                callInfo.profileInfo->getApnTypes());
              if (status != Message::Callback::Status::SUCCESS) {
                Log::getInstance().d("SET_APN_INFO request failed,"
                                " result = "+ std::to_string((int) status));
              }
              else {
                Log::getInstance().d("SET_APN_INFO request successful"
                                    ", result = "+ std::to_string((int) status));
              }
            }
            else {
              Log::getInstance().d("ERROR dsd_endpoint is NULL.");
            }
          } //end if(!mApAssistMode)


          if((callInfo.profileInfo->getApnTypes() & (uint32_t)ApnTypes_t::DEFAULT) &&
               globalInfo.prevDefaultProfileId != QDP_PROFILE_ID_INVALID &&
               globalInfo.currDefaultProfileId !=  globalInfo.prevDefaultProfileId)
          {
            if(qdp_profile_release( globalInfo.prevDefaultProfileId))
            {
               globalInfo.prevDefaultProfileId = QDP_PROFILE_ID_INVALID;
            }
          }

          //Add handover IP addresses if call is being setup for 3rd party handover
          if ((m->getDataRequestReason() == DataRequestReason_t::HANDOVER)) {
            Log::getInstance().d("3rd party handover from AP.");
#ifndef RIL_FOR_MDM_LE
            setThirdPartyHOParams(m->getAddresses());
#endif
          }
          if (DSI_SUCCESS != dsiStartDataCall(callInfo.dsiHandle)) {
            Log::getInstance().d("Invalid apn configuration");
            setupDataCallFailed(ResponseError_t::INVALID_ARGUMENT,
                CallEndReason::getCallEndReason(
                    getVerboseCallEndReason()));
            transitionTo(_eDisconnected);
            // release wakelock
            Log::getInstance().d("[" + getName() + "]: releasing the Wakelock");
            if(m->getAcknowlegeRequestCb() != nullptr) {
              auto cb = *(m->getAcknowlegeRequestCb().get());
              cb(m->getSerial());
            }
            return IState<CallEventType>::HANDLED;
          }
          transitionTo(_eConnecting);
        }
        else {
          setupDataCallFailed(ResponseError_t::INVALID_ARGUMENT,
              DataCallFailCause_t::OEM_DCFAILCAUSE_4);
          transitionTo(_eDisconnected);
        }
        // release wakelock
        Log::getInstance().d("[" + getName() + "]: releasing the Wakelock");
        if(m->getAcknowlegeRequestCb() != nullptr) {
          auto cb = *(m->getAcknowlegeRequestCb().get());
          cb(m->getSerial());
        }
      } else {
        Log::getInstance().d("Message is null");
      }
      return IState<CallEventType>::HANDLED;
    }
    case CallEventTypeEnum::EnableEmbmsCall:
    {
      if(true == setProfileInfo())
      {
        callInfo.isEmbmsCall = true;
        callInfo.embmsInfo->setDsiHandle(callInfo.dsiHandle);
        if (DSI_SUCCESS != dsi_embms_enable(callInfo.dsiHandle)) {
          Log::getInstance().d("dsi_embms_enable failed");
          qcril_embms_enable_response_payload_type enableResp;
          memset(&enableResp, 0, sizeof(enableResp));
          enableResp.call_id = callInfo.cid;
          enableResp.cause = RIL_E_GENERIC_FAILURE;
          enableResp.resp_code = (int32_t)EmbmsDataCallFailCause_t::ERROR_UNKNOWN;

          auto msg = std::make_shared<NasEmbmsEnableDataConMessage>(enableResp);
          msg->dispatch();
          transitionTo(_eDisconnected);
        } else {
          transitionTo(_eConnecting);
        }
      } else {
        Log::getInstance().d("setProfileInfo failed");
      }
      return IState<CallEventType>::HANDLED;
    }
    default:
      return IState<CallEventType>::UNHANDLED;
  }
}
