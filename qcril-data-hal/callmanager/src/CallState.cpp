/******************************************************************************
#  Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "CallState.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "MessageCommon.h"
#include "qcril_data.h"
#include "UnSolMessages/QosInitializeMessage.h"

extern "C" {
#include "qmi_platform.h"
}

using namespace rildata;

void CallState::enter() {
}

void CallState::exit() {
}

void CallState::stopTimer(DataCallTimerType type) {
  if (timers.find(type) != timers.end()) {
    TimeKeeper::timer_id timer = timers[type];
    if (timer != TimeKeeper::no_timer) {
      TimeKeeper::getInstance().clear_timer(timer);
      timers.erase(type);
    }
  }
}

void CallState::timeoutHandler(DataCallTimerType type, void *data) {
  std::ignore = data;
  auto timeoutMsg = std::make_shared<DataCallTimerExpiredMessage>(callInfo.cid, type);
  if (timeoutMsg != nullptr) {
    timeoutMsg->broadcast();
    string logMsg = "[" + getName() + "]: Timer expired for " +
                        DataCallTimerExpiredMessage::getStringType(type);
    Log::getInstance().d(logMsg);
  }
}

void CallState::startTimer(DataCallTimerType type, TimeKeeper::millisec timeout) {
  // if timer exists, restart it with the new timeout
  auto handler = bind(&ConnectedState::timeoutHandler, this, type, std::placeholders::_1);
  stopTimer(type);
  timers[type] = TimeKeeper::getInstance().set_timer(handler, nullptr, timeout);
  string logMsg = "[" + getName() + "]: Timer started for " +
                        DataCallTimerExpiredMessage::getStringType(type) + " for " +
                        to_string(static_cast<long>(timeout)) + "ms";
  Log::getInstance().d(logMsg);
  logBuffer.addLogWithTimestamp(logMsg);
}

void CallState::sendSetupDataCallResponse(std::shared_ptr<SetupDataCallRequestMessage> msg, const SetupDataCallResponse_t& result)
{
  if (msg != nullptr) {
    auto resp = std::make_shared<SetupDataCallResponse_t>(result);
    stringstream ss;
    ss << "[" << getName() << "]: " << msg->getSerial() << "< setupDataCallResponse resp=";
    result.dump("", ss);
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  }
  else {
    Log::getInstance().d("[" + getName() + "]: Corrupted SetupDataCallRequestMessage - not able to send response");
  }
}

void CallState::sendDeactivateDataCallResponse(std::shared_ptr<DeactivateDataCallRequestMessage> msg, const ResponseError_t& result)
{
  if (msg != nullptr) {
    auto resp = std::make_shared<ResponseError_t>(result);
    stringstream ss;
    ss << "[" << getName() << "]: " << msg->getSerial() << "< deactivateDataCallResponse resp=" << (int)result << " cid=" << callInfo.cid;
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  }
  else {
    Log::getInstance().d("[" + getName() + "]: Corrupted DeactivateDataCallRequestMessage - not able to send response");
  }
}

void CallState::setupDataCallCompleted(const SetupDataCallResponse_t& result)
{
  Log::getInstance().d("[" + getName() + "]: setupDataCallCompleted = "+std::to_string((int)result.respErr));

  if(callInfo.request != nullptr) {
    Log::getInstance().d("[" + getName() + "]: found message = " + callInfo.request->get_message_name());
    std::shared_ptr<SetupDataCallRequestMessage> ms = std::static_pointer_cast<SetupDataCallRequestMessage>(callInfo.request);
    if(ms != nullptr) {
      sendSetupDataCallResponse(ms, result);

      if(result.respErr == ResponseError_t::NO_ERROR) {
          callInfo.callParamsChanged = true;
      } else {
          Log::getInstance().d("[" + getName() + "]: setupdatacallResp failed");
      }
      Log::getInstance().d("[" + getName() + "]: releasing the Wakelock");
      // release wakelock
      if(ms->getAcknowlegeRequestCb() != nullptr) {
        auto cb = *(ms->getAcknowlegeRequestCb().get());
        cb(ms->getSerial());
      }
      callInfo.request = nullptr;
    }
  }
  else {
    Log::getInstance().d("[" + getName() + "]: not found");
  }
}

void CallState::setupDataCallFailed(const ResponseError_t& respErr, const DataCallFailCause_t& cause) {
  SetupDataCallResponse_t result = {};
  result.respErr = respErr;
  result.call.suggestedRetryTime = -1;
  result.call.cause = cause;
  result.call.cid = -1;
  result.call.active = _eInactive;
  result.call.type = "";
  result.call.ifname = "";
  result.call.addresses = "";
  result.call.dnses = "";
  result.call.gateways = "";
  result.call.pcscf = "";
  result.call.mtu = 0;
  setupDataCallCompleted(result);
}

void CallState::deactivateDataCallCompleted(const ResponseError_t& result)
{
  Log::getInstance().d("[" + getName() + "]: deactivateDataCallCompleted = "+std::to_string((int)result));

  if(callInfo.request != nullptr) {
    Log::getInstance().d("[" + getName() + "]: found message = " + callInfo.request->get_message_name());
    std::shared_ptr<DeactivateDataCallRequestMessage> ms = std::static_pointer_cast<DeactivateDataCallRequestMessage>(callInfo.request);
    sendDeactivateDataCallResponse(ms, result);
    if(result != ResponseError_t::NO_ERROR)
      Log::getInstance().d("[" + getName() + "]: DeactivatedatacallResp failed");
    if( ms != nullptr)
    {
      Log::getInstance().d("[" + getName() + "]: releasing the Wakelock");
      //release wakelock
      if(ms->getAcknowlegeRequestCb() != nullptr) {
        auto cb = *(ms->getAcknowlegeRequestCb().get());
        cb(ms->getSerial());
      }
    }
    callInfo.request = nullptr;
  }
  else {
    Log::getInstance().d("[" + getName() + "]: not found");
  }
}

std::string CallState::getCallApn() {
  string apnName("");
  if (callInfo.profileInfo != nullptr) {
    if(callInfo.profileInfo->getIsEmergencyCall()) {
      apnName = callInfo.profileInfo->getEmergencyCallApn();
    }
    else {
      apnName = callInfo.profileInfo->getApn();
    }
  }
  return apnName;
}

std::unique_ptr<CallSetupProfile> CallState::createCallSetupProfile(std::shared_ptr<SetupDataCallRequestMessage> m) {
  if (m == nullptr) {
    return nullptr;
  }
  BaseProfile::params_t params;
  params.profileId = (int32_t)m->getProfileId();
  switch(m->getDataProfileInfoType())
  {
    case DataProfileInfoType_t::THREE_GPP2:
      params.radioTech = QDP_RADIOTECH_3GPP2;
      break;
    case DataProfileInfoType_t::THREE_GPP:
      params.radioTech = QDP_RADIOTECH_3GPP;
      break;
    case DataProfileInfoType_t::COMMON:
      params.radioTech = QDP_RADIOTECH_COMMON;
      break;
  }
  if (DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(callInfo.radioRAT)) {
    params.apn = "";
  } else {
    params.apn = m->getApn();
  }
  params.username = m->getUsername();
  params.password = m->getPassword();
  params.protocol = m->getProtocol();
  params.roamingProtocol = m->getRoamingProtocol();
  params.apnTypeBitmask = (int32_t)m->getSupportedApnTypesBitmap();
  params.bearerBitmask = (int32_t)m->getBearerBitmap();
  params.mtu = m->getMtu();
  params.preferred = m->getPreferred();
  params.persistent = m->getPersistent();
  params.roamingAllowed = m->getRoamingAllowed();
  params.authType = std::to_string((int)m->getAuthType());
  return std::make_unique<CallSetupProfile>(params);
}

dsi_ce_reason_t CallState::pickTemporaryCallEndReason(dsi_ce_reason_t v4Reason, dsi_ce_reason_t v6Reason) {
  DataCallFailCause_t v4RilReason = CallEndReason::getCallEndReason(v4Reason);
  if (!CallEndReason::isPermanentFailure(v4RilReason)) {
    return v4Reason;
  } else {
    return v6Reason;
  }
}

dsi_ce_reason_t CallState::getVerboseCallEndReason() {
  dsi_ce_reason_t dsiEndReason = {DSI_CE_TYPE_INVALID, WDS_VCER_MIP_HA_REASON_UNKNOWN_V01};
  std::string currentIpType = "IP";
  if (globalInfo.isRoaming) {
    currentIpType = callInfo.requestedRoamingIpType;
  } else {
    currentIpType = callInfo.requestedIpType;
  }

  // do not query for an IP type if it is already connected
  bool v4ReasonNeeded = false;
  bool v6ReasonNeeded = false;
  if (currentIpType == "IPV6" || currentIpType == "IPV4V6") {
    v6ReasonNeeded = !callInfo.v6Connected;
  }
  if (currentIpType == "IP" || currentIpType == "IPV4V6") {
    v4ReasonNeeded = !callInfo.v4Connected;
  }

  if (v4ReasonNeeded && v6ReasonNeeded) {
    dsi_ce_reason_t v4Reason = dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IP");
    dsi_ce_reason_t v6Reason = dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IPV6");
    dsiEndReason = pickTemporaryCallEndReason(v4Reason, v6Reason);
  } else if (v4ReasonNeeded) {
    dsiEndReason = dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IP");
  } else if (v6ReasonNeeded) {
    dsiEndReason = dsiGetVerboseCallEndReason(callInfo.dsiHandle, "IPV6");
  }
  return dsiEndReason;
}

bool CallState::setEmergencyProfileInfo() {
  Log::getInstance().d("[CallInfo]: setEmergencyProfileInfo");

  // getting dsi handle
  dsi_hndl_t dsiHandle = nullptr;
  if((dsiHandle = dsiGetDataSrvcHndl(CallState::dsiNetCallback, &callInfo)) == nullptr) {
    Log::getInstance().d("Unable to get dsi handle");
    return false;
  }

  // store dsiHandle into call instance
  callInfo.dsiHandle = dsiHandle;

  if (!callInfo.profileInfo->emergencyLookup()) {
    Log::getInstance().d("Failed to get emergency profile");
    return false;
  }
  if(callInfo.callBringUpCapability & BRING_UP_APN_TYPE) {
#ifndef RIL_FOR_MDM_LE
    /* set DSI bringup mode as apn-type based bringup */
    wds_apn_type_mask_v01 wdsApnTypeMask;
    wdsApnTypeMask =
      WDSModemEndPointModule::convertToApnTypeMask((int32_t)callInfo.profileInfo->getApnTypes());
    Log::getInstance().d("Bringup using apn type mask = " +std::to_string((int)wdsApnTypeMask));
    dsi_call_param_value_t bringUpMode;
    bringUpMode.buf_val = nullptr;
    bringUpMode.num_val = DSI_CALL_BRINGUP_MODE_APN_TYPE_MASK;
    if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle,
                                               DSI_CALL_INFO_CALL_BRINGUP_MODE,
                                               &bringUpMode)) {
      Log::getInstance().d("Unable to set bringup mode");
      return false;
    }
    /* set apnTypeBitmask */
    dsi_call_param_value_t apnTypeMask;
    apnTypeMask.buf_val = (char*)&wdsApnTypeMask;
    apnTypeMask.num_val = (int)sizeof(wds_apn_type_mask_v01);
    if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle,
                                               DSI_CALL_INFO_APN_TYPE_MASK,
                                               &apnTypeMask)) {
      Log::getInstance().d("Unable to set bringup apn type mask");
      return false;
    }
#endif //RIL_FOR_MDM_LE
  } else {
    dsi_call_param_value_t dsi_param;
    if(callInfo.profileInfo->getUmtsProfileId() > QDP_PROFILE_ID_INVALID) {
      dsi_param.buf_val = nullptr;
      dsi_param.num_val = callInfo.profileInfo->getUmtsProfileId();
      dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &dsi_param);
    } else if(callInfo.profileInfo->getCdmaProfileId() > QDP_PROFILE_ID_INVALID) {
      dsi_param.buf_val = nullptr;
      dsi_param.num_val = callInfo.profileInfo->getCdmaProfileId();
      dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &dsi_param);
    } else {
      Log::getInstance().d("Unable to set profile id");
      return false;
    }
  }

  // set IP family
  if(!callInfo.profileInfo->getProtocol().empty()) {
    Log::getInstance().d("[qdp] protocol = " + callInfo.profileInfo->getProtocol());
    dsi_call_param_value_t ipFamily;
    ipFamily.buf_val = nullptr;
    ipFamily.num_val = (callInfo.profileInfo->getProtocol().compare(string("IP"))==0)?DSI_IP_VERSION_4:
                       (callInfo.profileInfo->getProtocol().compare(string("IPV6"))==0)?DSI_IP_VERSION_6:DSI_IP_VERSION_4_6;
    if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_IP_VERSION, &ipFamily)) {
      Log::getInstance().d("Unable to set ip version");
      return false;
    }
  }

  return true;
}

/*===========================================================================*/
/*!
    @brief
    Sets the RIL provided profile id (technology specific) into the
    dsi_hndl

    @return
    true
    false
*/
/*=========================================================================*/
bool setRilProfileId
(
  const dsi_hndl_t dsiHndl,
  const int32_t rilProfile,
  const uint8_t rilTech
)
{
  int ret = false;
  int reti = true;
  dsi_call_param_value_t profile_id;
  dsi_call_param_value_t call_type;
  dsi_call_param_identifier_t profile_param_id = DSI_CALL_INFO_MIN;

  do
  {
    /* sanity checking */
    if ((RIL_DATA_PROFILE_DEFAULT == rilProfile))
    {
      Log::getInstance().d("[CallInfo]: default profile id"+ std::to_string(rilProfile)+"provided. " \
                      "no need to set in dsi_hndl");
      ret = true;
      break;
    }

    /* prepare dsi parameter with profile id */
    memset( &profile_id, 0, sizeof( dsi_call_param_value_t ) );
    profile_id.buf_val = NULL;
    profile_id.num_val = rilProfile;
    Log::getInstance().d("RIL provided PROFILE ID Number"+ std::to_string(profile_id.num_val));

    /* adjust the profile id according to ril.h */
    if (profile_id.num_val < (int)(RIL_DATA_PROFILE_OEM_BASE))
    {
      reti = true;
      switch(profile_id.num_val)
      {
      case RIL_DATA_PROFILE_TETHERED:
        Log::getInstance().d("RIL provided"+std::to_string(profile_id.num_val)+"profile id. Tethered call " \
                          "will be used");
        call_type.buf_val = NULL;
        call_type.num_val = DSI_CALL_TYPE_TETHERED;
        if (dsiSetDataCallParam(dsiHndl,
                                    DSI_CALL_INFO_CALL_TYPE,
                                    &call_type) != DSI_SUCCESS)
        {
          Log::getInstance().d("Couldn't set call_type"+ std::to_string(call_type.num_val));
          reti = false;
          break;
        }
        break;
      default:
        Log::getInstance().d("RIL provided"+std::to_string(profile_id.num_val)+ "profile id. This is currently "
                        "not supported");
      }
      if (reti != true)
      {
        break;
      }
    }
    else
    {
      /* adjust for the OEM base */
      profile_id.num_val -= RIL_DATA_PROFILE_OEM_BASE;
      Log::getInstance().d("profile_id.num_val"+std::to_string(profile_id.num_val)+"will be used (android provided"+ \
        std::to_string(profile_id.num_val + RIL_DATA_PROFILE_OEM_BASE));

      /* figure out whether this is umts or cdma profile id */
      if (rilTech == QDP_RADIOTECH_3GPP2)
      {
        profile_param_id = DSI_CALL_INFO_CDMA_PROFILE_IDX;
      }
      else if (rilTech == QDP_RADIOTECH_3GPP )
      {
        profile_param_id = DSI_CALL_INFO_UMTS_PROFILE_IDX;
      }
      else
      {
        Log::getInstance().d("RIL provided incorrect/malformed technology"+std::to_string(rilTech));
        break;
      }

      /* now set the profile id onto dsi_hndl */
      if ( ( dsiSetDataCallParam(dsiHndl,
                                     profile_param_id,
                                     &profile_id ) ) != DSI_SUCCESS )
      {
        Log::getInstance().d("unable to set profile id "+std::to_string(profile_id.num_val));
        break;
      }
    }

    ret = true;
  } while (0);

  if (true == ret)
  {
    Log::getInstance().d("setRilProfileId successful");

  }
  else
  {
    Log::getInstance().d("setRilProfileId failed");
  }

  return ret;
}

bool CallState::setProfileInfo() {
  Log::getInstance().d("[CallInfo]: setProfileInfo");

  // getting dsi handle
  dsi_hndl_t dsiHandle = nullptr;
  if((dsiHandle = dsiGetDataSrvcHndl(CallState::dsiNetCallback, &callInfo)) == nullptr) {
    Log::getInstance().d("Unable to get dsi handle");
    return false;
  }
  // store dsiHandle into call instance
  callInfo.dsiHandle = dsiHandle;

  if(callInfo.profileInfo == nullptr) {
    Log::getInstance().d("ProfileInfo is nullptr");
    return false;
  }

  // look up profile using qdp
  if(!callInfo.profileInfo->getApn().empty() || DATA_IS_RIL_RADIO_TECH_3GPP(callInfo.profileInfo->getAvailableRadioRAT())) {
    //If either cdma or umts profile ids are available on profile info, this is
    //likely a retry due to fallback. No need to do qdp lookup again
    if ((callInfo.profileInfo->getCdmaProfileId() != QDP_PROFILE_ID_INVALID ||
         callInfo.profileInfo->getUmtsProfileId() != QDP_PROFILE_ID_INVALID) ||
        (callInfo.profileInfo->lookup())) {
      // set default profile number
      if(callInfo.profileInfo->getApnTypes() & (uint32_t)ApnTypes_t::DEFAULT) {
        globalInfo.prevDefaultProfileId = globalInfo.currDefaultProfileId;
        globalInfo.currDefaultProfileId = callInfo.profileInfo->getUmtsProfileId();
        if(globalInfo.apAssistMode) {
          Log::getInstance().d("Set default 3gpp profile num = " +std::to_string(globalInfo.currDefaultProfileId));
          if(callInfo.wds_endpoint) {
            callInfo.wds_endpoint->setDefaultProfileNum(callInfo.profileInfo->getUmtsProfileId());
          }
          else {
            Log::getInstance().d("wds_endpoint is nullptr ");
          }
        }
      }

      #ifdef QMI_RIL_UTF
        dsi_call_param_value_t utf_apn;
        string apn_name = callInfo.profileInfo->getApn();
        utf_apn.buf_val = (char *)(apn_name.c_str());
        utf_apn.num_val = callInfo.profileInfo->getApn().length();
        dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_APN_NAME, &utf_apn);
      #endif

      dsi_call_param_value_t bringUpMode;
      bringUpMode.buf_val = nullptr;
      bringUpMode.num_val = DSI_CALL_BRINGUP_MODE_INVALID;

      if(callInfo.callBringUpCapability & BRING_UP_APN_TYPE) {
#ifndef RIL_FOR_MDM_LE
        wds_apn_type_mask_v01 wdsApnTypeMask;
        wdsApnTypeMask =
          WDSModemEndPointModule::convertToApnTypeMask((int32_t)callInfo.profileInfo->getApnTypes());

        /*If more than one profile with the same APNtype is found
          then bring up the call using APNName*/
        if(qdp_profiles_for_apntype(wdsApnTypeMask) > 1 )
        {
          if(callInfo.callBringUpCapability & BRING_UP_APN_NAME)
          {
            Log::getInstance().d("More than one profile found with ApnType" + std::to_string((int)wdsApnTypeMask));
            Log::getInstance().d("Bringup the call by APNname " + callInfo.profileInfo->getApn());
            bringUpMode.num_val = DSI_CALL_BRINGUP_MODE_APN_NAME;

            dsi_call_param_value_t apnName;
            string apn = callInfo.profileInfo->getApn();
            apnName.buf_val = (char *)(apn.c_str());
            apnName.num_val = callInfo.profileInfo->getApn().length();
            if(DSI_SUCCESS != dsiSetDataCallParam(dsiHandle,
                                                      DSI_CALL_INFO_APN_NAME,
                                                      &apnName)) {
              Log::getInstance().d("Unable to set the apn name");
              return false;
            }
          }
        }
        else
        {
          /* set DSI bringup mode as apn-type based bringup */
          Log::getInstance().d("Bringup the call by APNType " + std::to_string((int)wdsApnTypeMask));
          bringUpMode.num_val = DSI_CALL_BRINGUP_MODE_APN_TYPE_MASK;

          dsi_call_param_value_t apnTypeMask;
          apnTypeMask.buf_val = (char*)&wdsApnTypeMask;
          apnTypeMask.num_val = (int)sizeof(wds_apn_type_mask_v01);
          if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle,
                                                   DSI_CALL_INFO_APN_TYPE_MASK,
                                                   &apnTypeMask)) {
            Log::getInstance().d("Unable to set bringup apn type mask");
            return false;
          }
        }

        if(bringUpMode.num_val != DSI_CALL_BRINGUP_MODE_INVALID)
        {
          if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle,
                                                     DSI_CALL_INFO_CALL_BRINGUP_MODE,
                                                    &bringUpMode)) {
            Log::getInstance().d("Unable to set bringup mode");
            return false;
          }
        }
#endif
      }
      /* profile ID based bringup */
      Log::getInstance().d("ProfileId based Bringup");
      if(bringUpMode.num_val == DSI_CALL_BRINGUP_MODE_INVALID) {
        dsi_call_param_value_t apnInfo;

        if(callInfo.profileInfo->getUmtsProfileId() > QDP_PROFILE_ID_INVALID) {
          apnInfo.buf_val = nullptr;
          apnInfo.num_val = callInfo.profileInfo->getUmtsProfileId();
          dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &apnInfo);
        }

        if(callInfo.profileInfo->getCdmaProfileId() > QDP_PROFILE_ID_INVALID) {
          apnInfo.buf_val = nullptr;
          apnInfo.num_val = callInfo.profileInfo->getCdmaProfileId();
          dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &apnInfo);
        }

        if(callInfo.profileInfo->getCdmaProfileId() == QDP_PROFILE_ID_INVALID || callInfo.profileInfo->getUmtsProfileId() == QDP_PROFILE_ID_INVALID) {
          Log::getInstance().d("[qdp] apn = " + callInfo.profileInfo->getApn() + ", length = " + std::to_string(callInfo.profileInfo->getApn().length()));
          string apnStr = callInfo.profileInfo->getApn();
          apnInfo.buf_val = (char *)apnStr.c_str();
          apnInfo.num_val = apnStr.length();
          if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_APN_NAME, &apnInfo)) {
            Log::getInstance().d("Unable to set apn name");
            return false;
          }
        } else {
            Log::getInstance().d("[qdp] umtsProfileId = "+
                                 std::to_string((int)callInfo.profileInfo->getUmtsProfileId())
                                 +", "+
                                 "[qdp] cdmaProfileId = "+
                                 std::to_string((int)callInfo.profileInfo->getCdmaProfileId()));
        }
      } /* profile-id based bringup */
    } /* profile present or created (if needed) */
  } else { /* profile doesnt have valid APN name */

    Log::getInstance().d("RIL did not provide APN, not setting any APN in start_nw_params");
    /* use the RIL profile id only if APN was not provided */
    /* set profile id in the dsi store */

    if (true != setRilProfileId( callInfo.dsiHandle,
                                 callInfo.profileInfo->getProfileId(),
                                 callInfo.profileInfo->getRadioTech()))
    {
      Log::getInstance().d("could not set ril profile id for dsiHndle");
      return false;
    }

    /* Update dsi tech pref to cdma so that route lookup returns cdma iface */
    if( DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO( callInfo.radioRAT ) )
    {
      dsi_call_param_value_t techpref_info;
      techpref_info.buf_val = NULL;
      techpref_info.num_val = DSI_RADIO_TECH_CDMA;

      Log::getInstance().d("As Radio tech is 3gpp2, Setting DSI param -DSI_CALL_INFO_TECH_PREF");

      if ( ( DSI_SUCCESS !=  dsiSetDataCallParam( dsiHandle,
                                                      DSI_CALL_INFO_TECH_PREF,
                                                      &techpref_info ) ) )
      {
        Log::getInstance().d("unable to set tech pref");
        return false;
      }
    }
  }
  // set auth preference
  if(!callInfo.profileInfo->getAuthType().empty()) {
    Log::getInstance().d("[qdp] authtype = " + callInfo.profileInfo->getAuthType());
    dsi_call_param_value_t authPref;
    authPref.buf_val = nullptr;
    authPref.num_val = stoi(callInfo.profileInfo->getAuthType());
    if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_AUTH_PREF, &authPref)) {
      Log::getInstance().d("Unable to set auth type");
      return false;
    }
  }

  // set username
  if(!callInfo.profileInfo->getUsername().empty()) {
    Log::getInstance().d("[qdp] username = " + callInfo.profileInfo->getUsername());
    dsi_call_param_value_t username;
    string userNameStr = callInfo.profileInfo->getUsername();
    username.buf_val = (char *)userNameStr.c_str();
    username.num_val = callInfo.profileInfo->getUsername().length();
    if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_USERNAME, &username)) {
      Log::getInstance().d("Unable to set username");
      return false;
    }
  }

  // set password
  if(!callInfo.profileInfo->getPassword().empty()) {
    Log::getInstance().d("[qdp] password = " + callInfo.profileInfo->getPassword());
    dsi_call_param_value_t password;
    string passwordStr = callInfo.profileInfo->getPassword();
    password.buf_val = (char *)passwordStr.c_str();
    password.num_val = callInfo.profileInfo->getPassword().length();
    if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_PASSWORD, &password)) {
      Log::getInstance().d("Unable to set password");
      return false;
    }
  }

  // set IP family
  if(!callInfo.profileInfo->getProtocol().empty()) {
    Log::getInstance().d("[qdp] protocol = " + callInfo.profileInfo->getProtocol());
    dsi_call_param_value_t ipFamily;
    ipFamily.buf_val = nullptr;

    if(callInfo.profileInfo->getProtocol().compare(string("IP"))==0) {
      ipFamily.num_val = DSI_IP_VERSION_4;
    }
    else if(callInfo.profileInfo->getProtocol().compare(string("IPV6"))==0) {
      ipFamily.num_val = DSI_IP_VERSION_6;
      /*-------------------------------------------------------------------
        Add IPv4 automatically if CLAT is enabled on this profile
      -------------------------------------------------------------------*/
      if( global_qcril_clat_supported )
      {
        if (!callInfo.profileInfo->getApn().empty() &&
             (callInfo.profileInfo->getApnTypes()
                & (uint32_t)ApnTypes_t::DEFAULT) == (uint32_t)ApnTypes_t::DEFAULT)
        {
          Log::getInstance().d("[CallInfo] Automatic IPv4 request for APN "
                               + callInfo.profileInfo->getApn());
          ipFamily.num_val = DSI_IP_VERSION_4_6;
          callInfo.requestedIpType = "IPV4V6";
        }
        else {
          if(!callInfo.wds_endpoint)
          {
            Log::getInstance().d("[CallInfo] callInfo.wds_endpoint is NULL.");
          }
          else {
            int32_t profile_type = ( callInfo.profileInfo->getRadioTech() == QDP_RADIOTECH_3GPP2 ) ?
                WDS_PROFILE_TYPE_3GPP2_V01 : WDS_PROFILE_TYPE_3GPP_V01;
            uint8_t profile_index = ( callInfo.profileInfo->getRadioTech() == QDP_RADIOTECH_3GPP2 ) ?
                callInfo.profileInfo->getCdmaProfileId() : callInfo.profileInfo->getUmtsProfileId();
            if(profile_index > QDP_PROFILE_ID_INVALID &&
                  callInfo.wds_endpoint->getWDSProfileClatCapability(profile_type, profile_index))
            {
              ipFamily.num_val = DSI_IP_VERSION_4_6;
              callInfo.requestedIpType = "IPV4V6";
            }
          }
        }
      }
    }
    else
    {
      ipFamily.num_val = DSI_IP_VERSION_4_6;
      callInfo.requestedIpType = "IPV4V6";
    }

    if ((!callInfo.profileInfo->getRoamingProtocol().empty()) && (callInfo.profileInfo->getProtocol().compare(callInfo.profileInfo->getRoamingProtocol()) != 0)) {
      Log::getInstance().d("Home Ip type is different from Roaming Ip type, use IPv4v6");
      ipFamily.num_val = DSI_IP_VERSION_4_6;
      callInfo.requestedIpType = "IPV4V6";
    }

    if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_IP_VERSION, &ipFamily)) {
      Log::getInstance().d("Unable to set ip version");
      return false;
    }
  }

#ifndef RIL_FOR_MDM_LE
  // set disallow roaming state
  dsi_call_param_value_t disallowRoaming;
  disallowRoaming.buf_val = nullptr;
  disallowRoaming.num_val = callInfo.profileInfo->isRoamingAllowed() ? 0 : 1;
  if (DSI_SUCCESS != dsiSetDataCallParam(dsiHandle, DSI_CALL_INFO_DISALLOW_ROAMING, &disallowRoaming)) {
    Log::getInstance().d("Unable to set disallow roaming");
    return false;
  }
#endif

  return true;
}

/**
 * @brief static function for DSI callback
 * @details static function requires to use call instance
 *
 * @param userData is CallState * type
 */
void CallState::dsiNetCallback(
  dsi_hndl_t dsiHandle,
  void *userData,
  dsi_net_evt_t netEvent,
  dsi_evt_payload_t *payload
)
{
  std::ignore = dsiHandle;

  Log::getInstance().d("dsi net evt = " + std::to_string(netEvent));

  EventDataType callbackEvent;
  memset(&callbackEvent, 0, sizeof(EventDataType));

  callbackEvent.evt = netEvent;
  callbackEvent.data = (void *)userData;
  callbackEvent.data_len = sizeof(CallInfo *);
  callbackEvent.self = (void *)&callbackEvent;
  memset(&callbackEvent.payload, '\0', sizeof(dsi_evt_payload_t));

  if(payload != nullptr)
  {
    memcpy(&callbackEvent.payload.embms_info, &(payload->embms_info), sizeof(dsi_embms_tmgi_info_type));
  }

  std::shared_ptr<RilEventDataCallback> msg = std::make_shared<RilEventDataCallback>(callbackEvent);
  if(msg) {
    msg->setCid(((CallInfo*)userData)->cid);
    msg->broadcast();
  }
}

void CallState::compareCallParams(const CallInfo& callInst) {
  if(callInfo.v4Connected != callInst.v4Connected) {
    Log::getInstance().d("compareCallParams - v4Connected state is changed");
    callInfo.v4Connected = callInst.v4Connected;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.v6Connected != callInst.v6Connected) {
    Log::getInstance().d("compareCallParams - v6Connected state is changed");
    callInfo.v6Connected = callInst.v6Connected;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.deviceName.compare(callInst.deviceName) != 0) {
    Log::getInstance().d("compareCallParams - deviceName is changed");
    callInfo.deviceName = callInst.deviceName;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.ipAddresses.compare(callInst.ipAddresses) != 0) {
    Log::getInstance().d("compareCallParams - ipAddresses is changed");
    callInfo.ipAddresses = callInst.ipAddresses;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.gatewayAddresses.compare(callInst.gatewayAddresses) != 0) {
    Log::getInstance().d("compareCallParams - gatewayAddresses is changed");
    callInfo.gatewayAddresses = callInst.gatewayAddresses;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.dnsAddresses.compare(callInst.dnsAddresses) != 0) {
    Log::getInstance().d("compareCallParams - dnsAddresses is changed");
    callInfo.dnsAddresses = callInst.dnsAddresses;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.pcscfAddresses.compare(callInst.pcscfAddresses) != 0) {
    Log::getInstance().d("compareCallParams - pcscfAddresses is changed");
    callInfo.pcscfAddresses = callInst.pcscfAddresses;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.ipType.compare(callInst.ipType) != 0) {
    Log::getInstance().d("compareCallParams - ipType is changed");
    callInfo.ipType = callInst.ipType;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.mtu != callInst.mtu) {
    Log::getInstance().d("compareCallParams - MTU is changed");
    callInfo.mtu = callInst.mtu;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.mtuV4 != callInst.mtuV4) {
    Log::getInstance().d("compareCallParams - v4 MTU is changed");
    callInfo.mtuV4 = callInst.mtuV4;
    callInfo.callParamsChanged = true;
  }
  if(callInfo.mtuV6 != callInst.mtuV6) {
    Log::getInstance().d("compareCallParams - v6 MTU is changed");
    callInfo.mtuV6 = callInst.mtuV6;
    callInfo.callParamsChanged = true;
  }
}

void CallState::configureParams(const IpFamilyType_t &ipType) {

  CallInfo tempCallinfo = {};
  dsiGetDeviceName(callInfo.dsiHandle, tempCallinfo.deviceName);
  dsiGetAddresses(callInfo.dsiHandle, tempCallinfo.v4Connected, tempCallinfo.v6Connected,
                  tempCallinfo.ipAddresses, tempCallinfo.gatewayAddresses, tempCallinfo.dnsAddresses, ipType);
  if(tempCallinfo.v4Connected && tempCallinfo.v6Connected) {
    tempCallinfo.ipType = "IPV4V6";
  }
  else if (tempCallinfo.v6Connected) {
    tempCallinfo.ipType = "IPV6";
  }
  else if (tempCallinfo.v4Connected) {
    tempCallinfo.ipType = "IP";
  }
  dsiGetPcscfAddresses(callInfo.dsiHandle, tempCallinfo.pcscfAddresses, ipType);
  tempCallinfo.mtu = dsiGetMtu(callInfo.dsiHandle);

  dsi_v4_v6_mtu_t v4_v6_mtu;
  dsiGetMtuByFamily(callInfo.dsiHandle, v4_v6_mtu);
  tempCallinfo.mtuV4 = v4_v6_mtu.v4_mtu;
  tempCallinfo.mtuV6 = v4_v6_mtu.v6_mtu;

  compareCallParams(tempCallinfo);
}

void CallState::createQmiWdsClients() {
  Log::getInstance().d("createQmiWdsClients");

  int ep_type = -1;
  int epid = -1;
  int mux_id = -1;
  qmi_linux_get_conn_id_by_name_ex(callInfo.deviceName.c_str(), &ep_type, &epid, &mux_id);
  QmiSetupRequestCallback qmiWdsSetupCallback("CallInfo-Token");

  if(callInfo.v4Connected && callInfo.wds_v4_call_endpoint == nullptr) {
    callInfo.wds_v4_call_endpoint = std::make_unique<WDSCallModemEndPoint>(
                                                                "WDSCallv4ModemEndPoint_" + std::to_string(callInfo.cid),
                                                                callInfo.cid,
                                                                callInfo.profileInfo->getApn(),
                                                                callInfo.deviceName,
                                                                IpFamilyType_t::IPv4);
    callInfo.wds_v4_call_endpoint->setBindMuxDataPortParams(ep_type, epid, mux_id);
#ifdef RIL_FOR_DYNAMIC_LOADING
    callInfo.wds_v4_call_endpoint->requestSetup("CallInfo-v4-token", static_cast<qcril_instance_id_e_type>(global_instance_id),
    &qmiWdsSetupCallback);
#else
    callInfo.wds_v4_call_endpoint->requestSetup("CallInfo-v4-token", &qmiWdsSetupCallback);
#endif
    callInfo.wds_v4_call_endpoint->setLinkStateChangeReport(globalInfo.linkStateChangeReport);
  }

  if(callInfo.v6Connected && callInfo.wds_v6_call_endpoint == nullptr) {
    callInfo.wds_v6_call_endpoint = std::make_unique<WDSCallModemEndPoint>(
                                                                "WDSCallv6ModemEndPoint_" + std::to_string(callInfo.cid),
                                                                callInfo.cid,
                                                                callInfo.profileInfo->getApn(),
                                                                callInfo.deviceName,
                                                                IpFamilyType_t::IPv6);
    callInfo.wds_v6_call_endpoint->setBindMuxDataPortParams(ep_type, epid, mux_id);
#ifdef RIL_FOR_DYNAMIC_LOADING
    callInfo.wds_v6_call_endpoint->requestSetup("CallInfo-v6-token", static_cast<qcril_instance_id_e_type>(global_instance_id),
    &qmiWdsSetupCallback);
#else
    callInfo.wds_v6_call_endpoint->requestSetup("CallInfo-v6-token", &qmiWdsSetupCallback);
#endif
    callInfo.wds_v6_call_endpoint->setLinkStateChangeReport(globalInfo.linkStateChangeReport);
  }
}

void CallState::initializeQos()
{
  int ep_type = -1;
  int epid = -1;
  int mux_id = -1;
  qmi_linux_get_conn_id_by_name_ex(callInfo.deviceName.c_str(), &ep_type, &epid, &mux_id);
  auto m = std::make_shared<QosInitializeMessage>(callInfo.cid, mux_id,ep_type, epid);
  m->broadcast();
  Log::getInstance().d("[" + getName() + "]: initializeQos done");
}

void CallState::deInitializeQos()
{
  auto m = std::make_shared<QosInitializeMessage>(0,0,0,0,true);
  m->broadcast();
  Log::getInstance().d("[" + getName() + "]: deInitializeQos done");
}

void CallState::dataCallListChanged() {
  if (callInfo.dataCallListChangedCallback == nullptr) {
    Log::getInstance().d("[" + getName() + "]: dataCallListChangedCallback not found");
  } else {
    callInfo.dataCallListChangedCallback();
  }
}

void CallState::cleanupKeepAlive(int cid) {
  if (callInfo.cleanupKeepAliveCallback == nullptr) {
    Log::getInstance().d("[" + getName() + "]:cleanupKeepAliveCallback not found");
  } else {
    callInfo.cleanupKeepAliveCallback(cid);
  }
}

vector<LinkAddress_t> CallState::convertAddrStringToLinkAddresses(const string &addr) {
  string tmpString;
  char delimiter = 0x20;
  vector<LinkAddress_t> linkAddressesVector;
  stringstream ssAddresses(addr);
  while(getline(ssAddresses, tmpString, delimiter)) {
    LinkAddress_t linkAddress;
    linkAddress.address = tmpString;
    linkAddressesVector.push_back(linkAddress);
  }
  return linkAddressesVector;
}

dsd_apn_pref_sys_enum_v01 convertToDsdPrefSys(HandoffNetworkType_t network) {
  switch (network) {
    case _eWWAN:
      return DSD_APN_PREF_SYS_WWAN_V01;
    case _eWLAN:
      return DSD_APN_PREF_SYS_WLAN_V01;
    case _eIWLAN:
      return DSD_APN_PREF_SYS_IWLAN_V01;
    default:
      Log::getInstance().d("invalid preferred sys=" +
                           std::to_string(static_cast<int>(network)));
      return DSD_APN_PREF_SYS_WWAN_V01;
  }
}

bool CallState::sendSetApnPreferredSystemRequest(std::shared_ptr<DSDModemEndPoint> dsd_endpoint, string apnName, HandoffNetworkType_t prefNetwork) {
  int rc;
  bool ret = true;
  dsd_set_apn_preferred_system_req_msg_v01 req;
  dsd_set_apn_preferred_system_resp_msg_v01 resp;
  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));
  dsd_apn_pref_sys_enum_v01 prefSys = convertToDsdPrefSys(prefNetwork);
  Log::getInstance().d("sendSetApnPreferredSystemRequest for apn ="+apnName+
                       ", prefSys ="+std::to_string((int)prefSys));

  strlcpy(req.apn_pref_sys.apn_name, apnName.c_str(), QMI_DSD_MAX_APN_LEN_V01+1);
  req.apn_pref_sys.pref_sys = prefSys;
  req.is_ap_asst_valid = true;
  req.is_ap_asst = true;

  if (dsd_endpoint != nullptr) {
    rc = dsd_endpoint->sendRawSync(
      QMI_DSD_SET_APN_PREFERRED_SYSTEM_REQ_V01,
      &req, sizeof(req),
      &resp, sizeof(resp));
  } else {
    Log::getInstance().d("Failed to get dsd endpoint");
    rc = false;
  }

  if (rc!=QMI_NO_ERR) {
    Log::getInstance().d("Failed to set APN preferred system change = " + std::to_string(rc)
    + " resp.result = " + std::to_string(resp.resp.result)
    + " resp.error = " + std::to_string(resp.resp.error));
    ret = false;
  }

  return ret;
}

void CallState::determineIpTypeAndConfigureParams(IpFamilyType_t &ipType)
{
  if(DUAL_IP_HO_ONLY_IPV6_HO_SUCCESS) {
    ipType = IpFamilyType_t::IPv6;
  } else if (DUAL_IP_HO_ONLY_IPV4_HO_SUCCESS) {
    ipType = IpFamilyType_t::IPv4;
  }
  configureParams(ipType);
}

void CallState::setCurrentRATAndConfigureParams(IpFamilyType_t &ipType)
{
  HandoffNetworkType_t target = static_cast<HandoffNetworkType_t>(callInfo.preferredRAT | callInfo.currentRAT);
  callInfo.currentRAT = target;
  if (callInfo.v6HandedOver == HandOverState::Success)
    ipType = IpFamilyType_t::IPv6;
  else
    ipType = IpFamilyType_t::IPv4;

  configureParams(ipType);
}

#ifndef RIL_FOR_MDM_LE
void CallState::setThirdPartyHOParams(std::vector<std::string> addr)
{
  callInfo.thirdPartyHOAddr = addr;
  if (callInfo.dsiHandle) {
    dsi_call_param_value_t apnInfo;
    memset(&apnInfo,0,sizeof(apnInfo));
    dsi_wds_handoff_ctxt_t hoParams;
    memset(&hoParams,0,sizeof(hoParams));
    bool ipV4Found=false, ipV6Found= false;
    for (unsigned long i=0 ; i<addr.size(); i++) {
      std::string addrNoPrefix = addr[i].substr(0,addr[i].find("/"));
      //Do simple check for '.' to decide if ipv4 or ipv6 addr
      if (addrNoPrefix.find(".")!=std::string::npos) {
        Log::getInstance().d("found IPV4 HO addr" + addrNoPrefix);
        ipV4Found = true;
        strlcpy(hoParams.ipv4_addr,addrNoPrefix.c_str(),sizeof(hoParams.ipv4_addr));
      }
      else
      {
        Log::getInstance().d("found IPV6 HO addr" + addrNoPrefix);
        ipV6Found = true;
        strlcpy(hoParams.ipv6_addr,addrNoPrefix.c_str(),sizeof(hoParams.ipv6_addr));
      }
    }

    if (ipV4Found && ipV6Found) {
      hoParams.bearer_ip_type = DSI_BEARER_IP_TYPE_V4_V6;
    }
    else if (ipV4Found) {
      hoParams.bearer_ip_type = DSI_BEARER_IP_TYPE_V4;
    }
    else {
      hoParams.bearer_ip_type = DSI_BEARER_IP_TYPE_V6;
    }
    apnInfo.num_val = sizeof(hoParams);
    apnInfo.buf_val = (char*)&hoParams;
    if (DSI_SUCCESS != dsi_set_data_call_param(callInfo.dsiHandle, DSI_CALL_INFO_HANDOFF_CONTEXT, &apnInfo)) {
      Log::getInstance().d("Unable to set handoff context");
    } else {
      callInfo.thirdPartyHOParamsSet=true;
    }
  }
  else {
    Log::getInstance().d("dsi handle is null, not setting handoff context");
  }
}
#endif
