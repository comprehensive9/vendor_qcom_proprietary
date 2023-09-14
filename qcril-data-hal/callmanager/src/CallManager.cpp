/******************************************************************************
#  Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include <string.h>
#include "framework/Log.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "UnSolMessages/RadioDataCallListChangeIndMessage.h"
#include "UnSolMessages/IWlanDataCallListChangeIndMessage.h"
#include "UnSolMessages/BearerAllocationUpdateMessage.h"
#include "UnSolMessages/HandoverInformationIndMessage.h"
#include "UnSolMessages/DataCallTimerExpiredMessage.h"
#include "UnSolMessages/DataBearerTypeChangedMessage.h"
#include "UnSolMessages/DataAllBearerTypeChangedMessage.h"
#include "UnSolMessages/SetPreferredSystemMessage.h"
#include "UnSolMessages/DataRegistrationFailureCauseMessage.h"
#include "sync/GetCallBringUpCapabilitySyncMessage.h"
#include "event/LinkPropertiesChangedMessage.h"
#include "event/RilEventDataCallback.h"
#include "UnSolMessages/DsiInitCompletedMessage.h"
#include "UnSolMessages/NasRfBandInfoIndMessage.h"
#include "request/NasGetRfBandInfoMessage.h"

#include "DataModule.h"
#include "CallManager.h"
#include "CallInfo.h"
#include "CallState.h"
#include "qcril_data.h"
#include "DsiWrapper.h"
#include <sstream>

using namespace rildata;
using std::placeholders::_1;
using std::placeholders::_2;

unordered_map<NasActiveBand_t,FrequencyRange_t> mNR5GBandToFreqRangeMap = {
  {NasActiveBand_t::NGRAN_BAND_1,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_2,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_3,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_5,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_7,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_8,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_12,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_14,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_18,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_20,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_25,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_26,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_28,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_29,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_30,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_34,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_38,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_39,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_40,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_41,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_48,FrequencyRange_t::HIGH},
  {NasActiveBand_t::NGRAN_BAND_50,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_51,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_53,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_65,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_66,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_70,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_71,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_74,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_75,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_76,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_77,FrequencyRange_t::HIGH},
  {NasActiveBand_t::NGRAN_BAND_78,FrequencyRange_t::HIGH},
  {NasActiveBand_t::NGRAN_BAND_79,FrequencyRange_t::HIGH},
  {NasActiveBand_t::NGRAN_BAND_80,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_81,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_82,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_83,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_84,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_85,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_86,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_89,FrequencyRange_t::LOW},
  {NasActiveBand_t::NGRAN_BAND_90,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_91,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_92,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_93,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_94,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_95,FrequencyRange_t::MID},
  {NasActiveBand_t::NGRAN_BAND_257,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_258,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_259,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_260,FrequencyRange_t::UNKNOWN},
  {NasActiveBand_t::NGRAN_BAND_261,FrequencyRange_t::UNKNOWN},
};

CallManager::CallManager(LocalLogBuffer& setLogBuffer): logBuffer(setLogBuffer) {
  globalInfo.apAssistMode = false;
  globalInfo.partialRetryEnabled = true;
  mSaMode = false;
  mRadioDataAvailable = false;
  mNulBearerReasonAvailable = true;
  mRadioVoiceAvailable = false;
  globalInfo.maxPartialRetryTimeout = DEFAULT_MAX_PARTIAL_RETRY_TIMEOUT;
  mReportPhysicalChannelConfig = false;
  globalInfo.linkStateChangeReport = false;
  globalInfo.prevDefaultProfileId = QDP_PROFILE_ID_INVALID;
  globalInfo.currDefaultProfileId = QDP_PROFILE_ID_INVALID;
  toggleBearerUpdateToDataConnectionService = false;
  mPhysicalChannelInfoValid = false;
  mReportRegistrationRejectCause = false;
  dsiInitStatus = DsiInitStatus_t::RELEASED;
  mActiveBandInfo = NasActiveBand_t::NGRAN_BAND_MIN_ENUM_VAL;
#ifdef RIL_FOR_MDM_LE
  mDcListChangedCallback = [](const vector<rildata::DataCallResult_t>& dcList) -> void {
    auto msg = std::make_shared<rildata::RadioDataCallListChangeIndMessage_1_4>(dcList);
    msg->broadcast();
  };
#else
#ifdef RIL_FOR_LOW_RAM
  mDcListChangedCallback = [](const vector<rildata::DataCallResult_t>& dcList) -> void {
    auto msg = std::make_shared<rildata::RadioDataCallListChangeIndMessage_1_0>(dcList);
    msg->broadcast();
  };
#else
  mDcListChangedCallback = [](const vector<rildata::DataCallResult_t>&) -> void {
    Log::getInstance().d("[CallManager]: No valid dataCallListChanged callback");
  };
#endif //RIL_FOR_LOW_RAM
#endif//RIL_FOR_MDM_LE
  globalInfo.isRoaming = false;
  availableRadioDataService.clear();
  mPhysicalConfigStructInfo.clear();
}

CallManager::~CallManager() {
  while (!mCallInstances.empty()) {
    mCallInstances.erase(mCallInstances.begin());
  }
  #ifdef QMI_RIL_UTF
  mReportPhysicalChannelConfig = false;
  #endif
  //cleanUpAllBearerAllocation();
  availableRadioDataService.clear();
  prevDefaultProfileId = QDP_PROFILE_ID_INVALID;
  currDefaultProfileId = QDP_PROFILE_ID_INVALID;
  triggerDsiRelease();
}

void CallManager::init(bool apAssistMode, bool partialRetry, unsigned long maxPartialRetryTimeout,
                       BringUpCapability capability, bool afterBootup)
{
  Log::getInstance().d("[CallManager]: init");
  globalInfo.apAssistMode = apAssistMode;
  globalInfo.partialRetryEnabled = partialRetry;
  globalInfo.maxPartialRetryTimeout = maxPartialRetryTimeout;
  mCallBringupCapability = capability;
  memset(availableCallIds, true, sizeof(availableCallIds));
  while (!mCallInstances.empty()) {
    mCallInstances.erase(mCallInstances.begin());
  }
  wds_endpoint = ModemEndPointFactory<WDSModemEndPoint>::getInstance().buildEndPoint();
  dsd_endpoint = ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint();
  triggerDsiInit(afterBootup);
}

void CallManager::updateCurrentRoamingStatus(bool roaming) {
  Log::getInstance().d("[CallManager] roaming status updated = " + std::to_string((int)roaming));
  globalInfo.isRoaming = roaming;
}

HandoffNetworkType_t convertToHandoffNetworkType(dsd_apn_pref_sys_enum_v01 prefSys) {
  switch (prefSys) {
    case DSD_APN_PREF_SYS_WWAN_V01:
      return _eWWAN;
    case DSD_APN_PREF_SYS_WLAN_V01:
      return _eWLAN;
    case DSD_APN_PREF_SYS_IWLAN_V01:
      return _eIWLAN;
    default:
      Log::getInstance().d("[CallManager]: invalid preferred sys=" +
                           std::to_string(static_cast<int>(prefSys)));
      return _eWWAN;
  }
}

void AvailableRadioDataService_t::dump(string padding, ostream& os) const {
  os << padding << "<RadioFamily: " << radioFamily << " RadioFreq: ";
  radioFrequency.dump("", os);
  os << ">";
}

void CallManager::dump(string padding, ostream& os) const
{
  os << padding << "CallManager:" << endl;
  padding += "    ";
  os << padding << std::boolalpha << "ApAssistMode=" << globalInfo.apAssistMode << endl;
  os << padding << std::boolalpha << "PartialRetryEnabled=" << globalInfo.partialRetryEnabled << endl;
  os << padding << std::boolalpha << "SaMode=" << mSaMode << endl;
  os << padding << std::boolalpha << "RadioDataAvailable=" << mRadioDataAvailable << endl;
  os << padding << std::boolalpha << "RadioVoiceAvailable=" << mRadioVoiceAvailable << endl;
  os << padding << std::boolalpha << "NulBearerReasonAvailable=" << mNulBearerReasonAvailable << endl;
  os << padding << "MaxPartialRetryTimeout=" << globalInfo.maxPartialRetryTimeout << endl;
  os << padding << std::boolalpha << "ReportPhysicalChannelConfig=" << mReportPhysicalChannelConfig << endl;
  os << padding << std::boolalpha << "PhysicalChannelInfoValid=" << mPhysicalChannelInfoValid << endl;
  os << padding << std::boolalpha << "ReportDataRegistrationRejectCause=" << mReportRegistrationRejectCause << endl;
  os << padding << std::boolalpha << "ReportLinkActiveStateChange=" << globalInfo.linkStateChangeReport << endl;
  os << padding << "CallBringupCapability=" << (int)mCallBringupCapability << endl;
  os << padding << std::boolalpha << "ToggleBearerUpdate=" << toggleBearerUpdateToDataConnectionService << endl;
  os << endl << padding  << "RadioDCList:" << endl;
  for (const DataCallResult_t& call : radioDcList) {
    call.dump(padding + "    ", os);
    os << endl;
  }
  os << endl << padding  << "IWlanDCList:" << endl;
  for (const DataCallResult_t& call : iwlanDcList) {
    call.dump(padding + "    ", os);
    os << endl;
  }
  os << endl << padding  << "AvailableRadioDataServices: ";
  for (const AvailableRadioDataService_t& service : availableRadioDataService) {
    service.dump("", os);
  }
  os << endl << padding  << "LastPhysicalConfigStructInfo: ";
  for (const PhysicalConfigStructInfo_t& config : mPhysicalConfigStructInfo) {
    config.dump("", os);
  }
  os << endl << padding << "Calls:" << endl;
  for (const CallStateMachine& callInstance : mCallInstances) {
    callInstance.dump(padding + "    ", os);
    os << endl;
  }
}
void CallManager::setDataCallListChangedCallback(std::function<void(const std::vector<rildata::DataCallResult_t>&)> cb) {
  if (cb != nullptr) {
    mDcListChangedCallback = cb;
  }
}

void CallManager::setCleanupKeepAliveCallback(std::function<void(int)> cb) {
  mCleanupKeepAliveCb = cb;
}

/**
 * @brief find available cid
 * @details find available cid from availableCallIds array and set to not available
 * @return available cid
 */
int CallManager::getAvailableCid(void) {
  int cid=-1;
  for (int i=0 ; i<MaxConcurrentDataCalls ; i++) {
    if(availableCallIds[i] == true) {
      availableCallIds[i] = false;
      cid = i;
      break;
    }
  }

  Log::getInstance().d("[CallManager]: available cid = " + std::to_string(cid));
  return cid;
}

bool CallManager::isApnTypeInvalid(ApnTypes_t apnTypes) {
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

bool CallManager::isApnTypeEmergency(ApnTypes_t apnTypes) {
  return apnTypes == ApnTypes_t::EMERGENCY;
}

static DataCallResult_t convertToDcResult(const CallInfo& callInfo) {
  DataCallResult_t callInst = {};
  callInst.cause = callInfo.cause;
  callInst.suggestedRetryTime = -1;
  callInst.cid = callInfo.cid;
  callInst.active = callInfo.active;
  if (callInst.cause == DataCallFailCause_t::NONE) {
    callInst.type = callInfo.ipType;
    callInst.ifname = callInfo.deviceName;
    callInst.addresses = callInfo.ipAddresses;
    callInst.linkAddresses = CallState::convertAddrStringToLinkAddresses(callInfo.ipAddresses);
    callInst.dnses = callInfo.dnsAddresses;
    callInst.gateways = callInfo.gatewayAddresses;
    callInst.pcscf = callInfo.pcscfAddresses;
    callInst.mtu = callInfo.mtu;
    callInst.mtuV4 = callInfo.mtuV4;
    callInst.mtuV6 = callInfo.mtuV6;
  }
  callInst.qosSessions = callInfo.qosSessions;
  return callInst;
}

void CallManager::getRadioDataCallList(vector<DataCallResult_t> &call)
{
  Log::getInstance().d("[CallManager::getRadioDataCallList] from "+ std::to_string((int)mCallInstances.size()));

  auto matchedCalls = mFilter
      .matchCurrentNetwork(_eWWAN)
      .getResults();

  if( !globalInfo.apAssistMode ) {
    auto iwlanMatchedCalls = mFilter
        .matchCurrentNetwork(_eIWLAN)
        .getResults();

    for (auto it = iwlanMatchedCalls.begin(); it != iwlanMatchedCalls.end(); ++it) {
      matchedCalls.push_back(*it);
    }
  }

  call.resize(matchedCalls.size());

  transform(matchedCalls.begin(), matchedCalls.end(),
    call.begin(), [](CallStateMachine& callInstance) -> DataCallResult_t {
      return convertToDcResult(callInstance.getCallInfo());
  });
  stringstream ss;
  ss << "[CallManager]: getRadioDataCallList ";
  for (const DataCallResult_t& c : call) {
    c.dump("", ss);
    ss << ",";
  }
  Log::getInstance().d(ss.str());
  logBuffer.addLogWithTimestamp(ss.str());
}

void CallManager::getIWlanDataCallList(vector<DataCallResult_t> &call)
{
  Log::getInstance().d("[CallManager::getIWlanDataCallList] from "+ std::to_string((int)mCallInstances.size()));

  auto matchedCalls = mFilter
      .matchCurrentNetwork(_eIWLAN)
      .getResults();
  call.resize(matchedCalls.size());

  transform(matchedCalls.begin(), matchedCalls.end(),
    call.begin(), [](CallStateMachine& callInstance) -> DataCallResult_t {
      return convertToDcResult(callInstance.getCallInfo());
  });
  stringstream ss;
  ss << "[CallManager]: getIWlanDataCallList ";
  for (const DataCallResult_t& c : call) {
    c.dump("", ss);
    ss << ",";
  }
  Log::getInstance().d(ss.str());
  logBuffer.addLogWithTimestamp(ss.str());

}

void CallManager::dataCallListChanged() {
  Log::getInstance().d("[CallManager]: dataCallListChanged for the number of calls = " +
                      std::to_string(mCallInstances.size()));
  bool isRadioDcListChanged = false;
  bool isIWlanDcListChanged = false;
  uint16_t sizeOfradioDcList = radioDcList.size();
  uint16_t sizeOfiwlanDcList = iwlanDcList.size();
  Log::getInstance().d("current size of radioDcList = "+ std::to_string(sizeOfradioDcList));
  Log::getInstance().d("current size of iwlanDcList = "+ std::to_string(sizeOfiwlanDcList));

  radioDcList.clear();
  iwlanDcList.clear();

  auto matchedRadioCalls = mFilter
      .matchCurrentNetwork(_eWWAN)
      .matchCallStates({_eConnected, _eHandover, _eDisconnecting, _eDisconnected})
      .getResults();

  if (!globalInfo.apAssistMode) {
    auto iwlanCalls = mFilter
        .matchCurrentNetwork(_eIWLAN)
        .matchCallStates({_eConnected, _eHandover, _eDisconnecting, _eDisconnected})
        .getResults();

    for (auto it = iwlanCalls.begin(); it != iwlanCalls.end(); ++it) {
      matchedRadioCalls.push_back(*it);
    }
  }
  radioDcList.resize(matchedRadioCalls.size());

  transform(matchedRadioCalls.begin(), matchedRadioCalls.end(),
    radioDcList.begin(), [&isRadioDcListChanged](CallStateMachine& callInstance) -> DataCallResult_t {
      CallInfo& callInfo = callInstance.getCallInfo();
      if (callInfo.callParamsChanged) {
        isRadioDcListChanged = true;
      }
      return convertToDcResult(callInfo);
  });

  if (globalInfo.apAssistMode) {
    auto matchedIWlanCalls = mFilter
        .matchCurrentNetwork(_eIWLAN)
        .matchCallStates({_eConnected, _eHandover, _eDisconnecting, _eDisconnected})
        .getResults();
    iwlanDcList.resize(matchedIWlanCalls.size());

    transform(matchedIWlanCalls.begin(), matchedIWlanCalls.end(),
      iwlanDcList.begin(), [&isIWlanDcListChanged](CallStateMachine& callInstance) -> DataCallResult_t {
        CallInfo& callInfo = callInstance.getCallInfo();
        if (callInfo.callParamsChanged) {
          isIWlanDcListChanged = true;
        }
        return convertToDcResult(callInfo);
    });
  }

  for (CallStateMachine& callInstance : mCallInstances) {
    CallInfo& callInfo = callInstance.getCallInfo();
    callInfo.callParamsChanged = false;
  }

  if (sizeOfradioDcList != radioDcList.size()) {
    isRadioDcListChanged = true;
  }
  if (sizeOfiwlanDcList != iwlanDcList.size()) {
    isIWlanDcListChanged = true;
  }

  Log::getInstance().d("Radio Call list number " + std::to_string((int)radioDcList.size()));
  if(isRadioDcListChanged) {
    radioDataCallListChanged(radioDcList);
  }

  Log::getInstance().d("IWlan Call list number " + std::to_string((int)iwlanDcList.size()));
  if(isIWlanDcListChanged) {
    auto iwlanMsg = std::make_shared<rildata::IWlanDataCallListChangeIndMessage>(iwlanDcList);
    if (iwlanMsg != nullptr) {
        Log::getInstance().d("[CallManager]: " + std::to_string((int)iwlanDcList.size()) + "< iwlanDataCallListChanged " + iwlanMsg->dump());
        logBuffer.addLogWithTimestamp("[CallManager]: " + std::to_string((int)iwlanDcList.size()) + "< iwlanDataCallListChanged " + iwlanMsg->dump());
        iwlanMsg->broadcast();
    }
  }
}

void CallManager::radioDataCallListChanged(const std::vector<rildata::DataCallResult_t>& dcList) {
  stringstream ss;
  ss << "{";
  for (const DataCallResult_t& call : dcList) {
    call.dump("", ss);
    ss << ",";
  }
  ss << "}";
  Log::getInstance().d("[CallManager]: " + std::to_string((int)dcList.size()) + "< radioDataCallListChanged " + ss.str());
  logBuffer.addLogWithTimestamp("[CallManager]: " + std::to_string((int)dcList.size()) + "< radioDataCallListChanged " + ss.str());
  mDcListChangedCallback(dcList);
}

void CallManager::processQmiDsdApnPreferredSystemResultInd(const dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *ind) {
  if(ind != nullptr && globalInfo.apAssistMode) {
    Log::getInstance().d("CallManager::processQmiDsdApnPreferredSystemResultInd ENTRY" + std::to_string((int)ind->result_info.result));
    std::string apnName(ind->result_info.apn_pref_sys.apn_name);
    auto matchedCalls = mFilter
      .matchApn(apnName)
      .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      CallInfo& callInfo = callInstance.getCallInfo();
      HandoffState_t state = HandoffState_t::Failure;
      if(ind->result_info.result == DSD_AP_ASST_APN_PREF_SYS_RESULT_SUCCESS_V01) {
        state = HandoffState_t::PrefSysChangedSuccess;
      } else if (ind->result_info.result == DSD_AP_ASST_APN_PREF_SYS_RESULT_FAILURE_V01) {
        state = HandoffState_t::PrefSysChangedFailure;
      }
      auto msg = std::make_shared<HandoverInformationIndMessage>(
                          state, IpFamilyType_t::NotAvailable, callInfo.cid);
      if (msg != nullptr) {
        HandoffNetworkType_t prefNw = convertToHandoffNetworkType(ind->result_info.apn_pref_sys.pref_sys);
        msg->setPreferredNetwork(prefNw);
        handleCallEventMessage(CallEventTypeEnum::HandoverInformationInd, msg);
      }
    } else {
     Log::getInstance().d("Call not found");
    }
  }
}

bool CallManager::getApnByCid(int cid, string& apn) {
  auto matchedCalls = mFilter
      .matchCid(cid)
      .getResults();
  if (!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    const CallInfo& callInfo = callInstance.getCallInfo();
    if (callInfo.profileInfo != nullptr) {
      apn = callInfo.profileInfo->getApn();
      return true;
    }
  }
  return false;
}

vector<QosSession_t> CallManager::getQosSessionsforCid (int cid) {
  vector<QosSession_t> sessions = {};
  auto matchedCalls = mFilter
      .matchCid(cid)
      .getResults();
  if (!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    const CallInfo& callInfo = callInstance.getCallInfo();
    return callInfo.qosSessions;
  }
  return sessions;
}

void CallManager::processQmiDsdIntentToChangeApnPrefSysInd(const dsd_intent_to_change_apn_pref_sys_ind_msg_v01& ind)
{
  Log::getInstance().d("[CallManager]: process intent to change ind");
  if(globalInfo.apAssistMode) {
    Log::getInstance().d("[CallManager]: number of pref systems = "+std::to_string((int)ind.apn_pref_sys_len));
    for (uint32_t i=0 ; i<ind.apn_pref_sys_len; i++) {
      string apnName(ind.apn_pref_sys[i].apn_pref_info.apn_name);
      HandoffNetworkType_t prefNetwork = convertToHandoffNetworkType(ind.apn_pref_sys[i].apn_pref_info.pref_sys);

      auto matchedCalls = mFilter
          .matchApn(apnName)
          .getResults();
      if (matchedCalls.empty()) {
        // call is not found for apn, send ack to modem
        Log::getInstance().d("[CallManager]: call is not found for apn = "+apnName+
                             ", ack to modem with pref sys = "+std::to_string((int)prefNetwork));
        CallState::sendSetApnPreferredSystemRequest(dsd_endpoint, apnName, prefNetwork);
      } else {
          // if rat is null bearer and attach in progress is true, QNP will not send
          // pref RAT change to the framework since there is no valid RAT. Hence
          // ack the intent immediately since framework will not do any handoff
          if ((ind.apn_pref_sys[i].apn_pref_info.pref_sys == DSD_APN_PREF_SYS_WWAN_V01) &&
              (ind.apn_pref_sys[i].rat == DSD_SYS_RAT_EX_NULL_BEARER_V01) &&
              (ind.apn_status_valid) &&
              (ind.apn_status[i] & DSD_APN_PREF_SYS_APN_STATUS_ATTACH_IN_PROGRESS_V01)){
            Log::getInstance().d("[CallManager]: rat is null bearer due to attach in progress for apn = "
                                 +apnName + " ack the intent");
            CallState::sendSetApnPreferredSystemRequest(dsd_endpoint, apnName, prefNetwork);
          } else {
            CallStateMachine& callInstance = matchedCalls.front();
            CallInfo& callInfo = callInstance.getCallInfo();
            auto msg = std::make_shared<SetPreferredSystemMessage>(callInfo.cid, prefNetwork);
            handleCallEventMessage(CallEventTypeEnum::SetPreferredSystem, msg);
          }
      }
    }
  }
}

/**
 * Toggle whether to send bearer allocation updates via HAL
 */
ResponseError_t CallManager::handleToggleBearerAllocationUpdate(bool enable)
{
  Log::getInstance().d("[CallManager]: handleToggleBearerAllocationUpdate = "+std::to_string((int)enable));
  toggleBearerUpdateToDataConnectionService = enable;
  updateAllBearerAllocations();

  return ResponseError_t::NO_ERROR;
}

/**
 * handle modem bearer type update indication
 */
void CallManager::handleDataBearerTypeUpdate(int32_t cid, rildata::BearerInfo_t bearer)
{
  Log::getInstance().d("[CallManager]: handleDataBearerTypeChanged for cid = "
      +std::to_string((int)cid)
      +", bid = "
      +std::to_string((int)bearer.bearerId));

  auto matchedCalls = mFilter
      .matchCid(cid)
      .getResults();
  if (!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    CallInfo& callInfo = callInstance.getCallInfo();
    std::vector<rildata::BearerInfo_t>::iterator i;
    for (i=callInfo.bearerCollection.bearers.begin() ; i!=callInfo.bearerCollection.bearers.end() ; ++i)
    {
      if(i->bearerId == bearer.bearerId) {
          if ((i->uplink != bearer.uplink) || (i->downlink != bearer.downlink)) {
              i->uplink = bearer.uplink;
              i->downlink = bearer.downlink;
          }
          break;
      }
    }
    if (i==callInfo.bearerCollection.bearers.end()) {
      // new bearer
      callInfo.bearerCollection.bearers.push_back(bearer);
    }
    updateAllBearerAllocations();
  }
  else {
    Log::getInstance().d("[CallManager]: no call for cid = "+std::to_string((int)cid));
  }
}

/**
 * handle modem bearer type update indication
 */
void CallManager::handleDataAllBearerTypeUpdate(rildata::AllocatedBearer_t bearer)
{
  Log::getInstance().d("[CallManager]: handleDataBearerTypeChanged for cid = "
      +std::to_string((int)bearer.cid));
  auto matchedCalls = mFilter
      .matchCid(bearer.cid)
      .getResults();
  if (!matchedCalls.empty()) {
    CallStateMachine& callInstance = matchedCalls.front();
    CallInfo& callInfo = callInstance.getCallInfo();
    callInfo.bearerCollection.bearers.clear();
    callInfo.bearerCollection.bearers.insert(callInfo.bearerCollection.bearers.end(),
        bearer.bearers.begin(), bearer.bearers.end());
    updateAllBearerAllocations();
  } else {
    Log::getInstance().d("[CallManager]: no call for cid = "+std::to_string((int)bearer.cid));
  }
}

/**
 * handle get bearers for one data connection with cid
 */
AllocatedBearerResult_t CallManager::handleGetBearerAllocation(int32_t cid)
{
  Log::getInstance().d("[CallManager]: handleGetBearerAllocation = "+std::to_string((int)cid));

  AllocatedBearerResult_t result = {};
  auto matchedCalls = mFilter
      .matchCid(cid)
      .getResults();
  if (!matchedCalls.empty()) {
    AllocatedBearer_t bearer= {};
    CallStateMachine& callInstance = matchedCalls.front();
    const CallInfo& callInfo = callInstance.getCallInfo();
    result.error = ResponseError_t::NO_ERROR;
    bearer.cid = cid;
    if (callInfo.profileInfo) {
        bearer.apn = callInfo.profileInfo->getApn();
    }
    bearer.bearers = callInfo.bearerCollection.bearers;
    result.connections.push_back(bearer);
  } else {
    result.error = ResponseError_t::CALL_NOT_AVAILABLE;
  }

  return result;
}

/**
 * handle get all bearer information request
 */
AllocatedBearerResult_t CallManager::handleGetAllBearerAllocations()
{
  Log::getInstance().d("[CallManager]: handleGetAllBearerAllocations");

  AllocatedBearerResult_t result = {};
  for (auto it = mCallInstances.begin(); it != mCallInstances.end(); it++) {
    const CallInfo& callInfo = it->getCallInfo();
    AllocatedBearer_t bearer= {};
    if (callInfo.profileInfo) {
        bearer.apn = callInfo.profileInfo->getApn();
    }
    bearer.cid = callInfo.cid;
    bearer.bearers = callInfo.bearerCollection.bearers;
    result.connections.push_back(bearer);
  }
  result.error = ResponseError_t::NO_ERROR;
  return result;
}

/**
 * Updates all bearer type info.
 *
 * If there is at least one client registered for bearer allocation updates
 * through IDataConnection HAL, the bearer list will be sent to the HAL client.
 */
void CallManager::updateAllBearerAllocations()
{
    Log::getInstance().d("[CallManager]: updateAllBearerAllocations");
    if(toggleBearerUpdateToDataConnectionService) {
        // Update to IDataConnnection HAL
        AllocatedBearerResult_t localBearers;
        localBearers.error = ResponseError_t::NO_ERROR;
        for (auto it = mCallInstances.begin(); it != mCallInstances.end(); it++) {
          const CallInfo& callInfo = it->getCallInfo();
          AllocatedBearer_t bearer= {};
          if (callInfo.profileInfo) {
               bearer.apn = callInfo.profileInfo->getApn();
          }
          bearer.cid = callInfo.cid;
          bearer.bearers = callInfo.bearerCollection.bearers;
          localBearers.connections.push_back(bearer);
        }
        std::shared_ptr<AllocatedBearerResult_t> bearers = std::make_shared<AllocatedBearerResult_t>(localBearers);
        auto msg = std::make_shared<BearerAllocationUpdateMessage>(bearers);
        msg->broadcast();
    }

    if (mReportPhysicalChannelConfig) {
      updatePhysicalChannelConfigs();
    }
}

/**
 * clean all bearer type info and update
 */
void CallManager::cleanUpAllBearerAllocation()
{
  Log::getInstance().d("[CallManager]: cleanUpAllBearerAllocation");
  for (CallStateMachine& callInstance : mCallInstances) {
    CallInfo& callInfo = callInstance.getCallInfo();
    callInfo.bearerCollection.bearers.clear();
  }
  if(mReportPhysicalChannelConfig) {
    updatePhysicalChannelConfigs();
  }
}

bool CallManager::convertQmiDsdToAvailableRadioDataService(dsd_system_status_info_type_v01 sys,
                                                    AvailableRadioDataService_t &radioDataService)
{
  Log::getInstance().d("[CallManager]: convertQmiDsdToAvailableRadioDataService , rat = "+std::to_string((int)sys.rat_value)+
                        ", mask = "+std::to_string((uint64_t)sys.so_mask));
  bool addToAvailableService = true;
  radioDataService.radioFamily = RadioAccessFamily_t::UNKNOWN;
  radioDataService.radioFrequency.range = FrequencyRange_t::LOW;
  radioDataService.radioFrequency.channelNumber = 0;

  switch (sys.rat_value) {
    case DSD_SYS_RAT_EX_3GPP_WCDMA_V01:
    case DSD_SYS_RAT_EX_3GPP_TDSCDMA_V01:
      if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSDPAPLUS_V01) ||
        (sys.so_mask & QMI_DSD_3GPP_SO_MASK_DC_HSDPAPLUS_V01) ||
        (sys.so_mask & QMI_DSD_3GPP_SO_MASK_64_QAM_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::HSPAP;
      }
      else if(((sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSDPA_V01) && (sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSUPA_V01)) ||
             (sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::HSPA;
      }
      else if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSDPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::HSDPA;
      }
      else if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_HSUPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::HSUPA;
      }
      else if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_WCDMA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::UMTS;
      }
      else if(sys.rat_value == DSD_SYS_RAT_EX_3GPP_TDSCDMA_V01) {
        radioDataService.radioFamily = RadioAccessFamily_t::TD_SCDMA;
      }
      break;

    case DSD_SYS_RAT_EX_3GPP_GERAN_V01:
      if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_EDGE_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::EDGE;
      }
      else if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_GPRS_V01) ||
              (sys.so_mask & QMI_DSD_3GPP_SO_MASK_GSM_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::GPRS;
      }
      break;

    case DSD_SYS_RAT_EX_3GPP_LTE_V01:
      if((sys.so_mask & QMI_DSD_3GPP_SO_MASK_LTE_CA_DL_V01) ||
        (sys.so_mask & QMI_DSD_3GPP_SO_MASK_LTE_CA_UL_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::LTE_CA;
      }
      else {
        radioDataService.radioFamily = RadioAccessFamily_t::LTE;
      }
      radioDataService.radioFrequency.range = FrequencyRange_t::MID;
      break;

    case DSD_SYS_RAT_EX_3GPP2_1X_V01:
      if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_1X_IS2000_REL_A_V01) ||
        (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_1X_IS2000_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::ONE_X_RTT;
      }
      else if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_1X_IS95_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::IS95A;
      }
      break;

    case DSD_SYS_RAT_EX_3GPP2_HRPD_V01:
      if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVB_DPA_V01) ||
        (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVB_MPA_V01) ||
        (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVB_MMPA_V01) ||
        (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVB_EMPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::EVDO_B;
      }
      else if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVA_DPA_V01) ||
              (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVA_MPA_V01) ||
              (sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REVA_EMPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::EVDO_A;
      }
      else if((sys.so_mask & QMI_DSD_3GPP2_SO_MASK_HDR_REV0_DPA_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::EVDO_0;
      }
      break;

    case DSD_SYS_RAT_EX_3GPP2_EHRPD_V01:
      radioDataService.radioFamily = RadioAccessFamily_t::EHRPD;
      break;

    case DSD_SYS_RAT_EX_3GPP_5G_V01:
      radioDataService.radioFamily = RadioAccessFamily_t::NR;
      if(sys.so_mask & QMI_DSD_3GPP_SO_MASK_5G_MMWAVE_V01) {
        radioDataService.radioFrequency.range = FrequencyRange_t::MMWAVE;
      } else {
        if( sys.so_mask & QMI_DSD_3GPP_SO_MASK_5G_NSA_V01 ) {
          radioDataService.radioFrequency.range = FrequencyRange_t::UNKNOWN;
          Log::getInstance().d("Sending frequency range as UNKNOWN for 5G NSA SUB6");
        } else if ( sys.so_mask & QMI_DSD_3GPP_SO_MASK_5G_SA_V01 ) {
          if( mActiveBandInfo != NasActiveBand_t::NGRAN_BAND_MIN_ENUM_VAL ) {
            radioDataService.radioFrequency.range = mNR5GBandToFreqRangeMap[mActiveBandInfo];
            Log::getInstance().d("Sending frequency range as"+
                                 std::to_string((int)radioDataService.radioFrequency.range)+"for 5G SA SUB6");
          } else {
            //Send message to NasModule to send Nas RfBand Info
            //Not blocking the call here for receiving response from NasModule
            auto nasGetRfBandInfoMessage = std::make_shared<NasGetRfBandInfoMessage>();
            if (nasGetRfBandInfoMessage != nullptr)
            {
              GenericCallback<rildata::NasActiveBand_t> cb ([](std::shared_ptr<Message> NasGetRfBandInfoMessage,
                                             Message::Callback::Status status,
                                             std::shared_ptr<rildata::NasActiveBand_t> rsp) -> void {
                if (NasGetRfBandInfoMessage && status == Message::Callback::Status::SUCCESS && rsp)
                {
                  Log::getInstance().d("dispatching NasRfBandInfoMessage");
                  auto indMsg = std::make_shared<NasRfBandInfoMessage>(*rsp);
                  indMsg->broadcast();
                }
                else
                {
                  Log::getInstance().d(" NasGetRfBandInfoMessage response error");
                }
              });
              nasGetRfBandInfoMessage->setCallback(&cb);
              nasGetRfBandInfoMessage->dispatch();
              Log::getInstance().d("mActiveBandInfo is not updated with Band Info from modem"
                                   "Sending UNKNOWN frequency for 5G SA SUB6 Band");
              //Send UNKNOWN as there is no info in cache
              radioDataService.radioFrequency.range = FrequencyRange_t::UNKNOWN;
            }
            else
            {
              Log::getInstance().d("[CallManager]: NasGetPhyChanConfigMessage error");
            }
          }
        } else {
          Log::getInstance().d("Dsd System Status Indication received for 5G RAT"
                               " with so_mask neither as SA nor as NSA!! Sending UNKNOWN as Frequency range");
          //Send UNKNOWN as there is no info in cache
          radioDataService.radioFrequency.range = FrequencyRange_t::UNKNOWN;
        }
      }
      break;
    case DSD_SYS_RAT_EX_3GPP_WLAN_V01:
      //If ApAssistMode is legacy then IWLAN is a valid Data Radio
      if (!globalInfo.apAssistMode &&
          (sys.so_mask & QMI_DSD_3GPP_SO_MASK_S2B_V01)) {
        radioDataService.radioFamily = RadioAccessFamily_t::WLAN;
      } else {
        addToAvailableService = false;
      }
      break;

    case DSD_SYS_RAT_EX_NULL_BEARER_V01:
    default:
      addToAvailableService = false;
      break;
  }

  return addToAvailableService;
}

void CallManager::processQmiDsdSystemStatusInd(const dsd_system_status_ind_msg_v01 * ind)
{
  Log::getInstance().d("[CallManager]: processQmiDsdSystemStatusInd");

  bool radioDataAvailable = false;
  bool radioVoiceAvailable = false;
  mNulBearerReasonAvailable = true;
  if(ind != nullptr && ind->avail_sys_valid) {
    availableRadioDataService.clear();
    AvailableRadioDataService_t radioService;
    for(uint32_t i=0 ; i<ind->avail_sys_len ; i++) {
      if (convertQmiDsdToAvailableRadioDataService(ind->avail_sys[i], radioService)) {
        Log::getInstance().d("[CallManager]: availableRadioDataService , radioFamily = "+std::to_string((int)radioService.radioFamily)+
                            ", radioFrequency.range = "+std::to_string((int)radioService.radioFrequency.range));
        availableRadioDataService.push_back(radioService);
      }
    }
    if(!availableRadioDataService.empty()) {
      Log::getInstance().d("[CallManager]: Radio Data Service is available");
      radioDataAvailable = true;
    }

    if(ind->null_bearer_reason_valid &&
      (ind->null_bearer_reason & DSD_NULL_BEARER_REASON_CS_ONLY_V01)) {
      Log::getInstance().d("[CallManager]: Radio Voice Service is available");
      radioVoiceAvailable = true;
    }
    if (ind->avail_sys_valid && ind->avail_sys_len >= 1) {
      mSaMode = ((ind->avail_sys[0].so_mask & QMI_DSD_3GPP_SO_MASK_5G_SA_V01) == QMI_DSD_3GPP_SO_MASK_5G_SA_V01);
    }

    // Older modem doesn't support NULL bearer So allowing Call if NULL bearer reason is unspecified
    if(!ind->null_bearer_reason_valid)
    {
      Log::getInstance().d("Null Bearer reason is not set");
      mNulBearerReasonAvailable = false;
    }
  }

  if (radioDataAvailable != mRadioDataAvailable) {
    mRadioDataAvailable = radioDataAvailable;
    wds_endpoint->updateDataRegistrationState(mRadioDataAvailable);
    if(mRadioDataAvailable && mReportRegistrationRejectCause) {
      rildata::PLMN_t plmn = {0,0,"",""};
      auto msg = std::make_shared<rildata::DataRegistrationFailureCauseMessage>(plmn, plmn, 0, false);
      msg->broadcast();
    }
  }
  if (radioVoiceAvailable != mRadioVoiceAvailable) {
    mRadioVoiceAvailable = radioVoiceAvailable;
  }

  if (mReportPhysicalChannelConfig) {
    updatePhysicalChannelConfigs();
  }
}

void CallManager::generateCurrentPhysicalChannelConfigs()
{
  Log::getInstance().d("[CallManager]: generateCurrentPhysicalChannelConfigs size = " +
                        std::to_string((int)mPhysicalChannelInfo.size()));

  bool fiveGAvailable = false;
  bool primaryServingValid = false;
  bool secondaryServingValid = false;
  RadioAccessFamily_t ratType = RadioAccessFamily_t::UNKNOWN;
  FrequencyRange_t frequencyRange = FrequencyRange_t::LOW;
  PhysicalConfigStructInfo_t fiveGNsaPrimaryServingInfo;
  PhysicalConfigStructInfo_t fiveGNsaSecondaryServingInfo;
  std::vector<PhysicalConfigStructInfo_t> physicalConfigStructInfo;

  for (auto radioService : availableRadioDataService) {
    if(radioService.radioFamily == RadioAccessFamily_t::NR) {
      fiveGAvailable = true;
      break;
    }
    ratType = radioService.radioFamily;
    frequencyRange = radioService.radioFrequency.range;
  }

  if(fiveGAvailable) {
    for (auto radioService : availableRadioDataService) {
      if(radioService.radioFamily == RadioAccessFamily_t::NR) {
        secondaryServingValid = true;
        fiveGNsaSecondaryServingInfo.status = CellConnectionStatus_t::SECONDARY_SERVING;
        fiveGNsaSecondaryServingInfo.rat = radioService.radioFamily;
        fiveGNsaSecondaryServingInfo.rfInfo.range = radioService.radioFrequency.range;
        fiveGNsaSecondaryServingInfo.rfInfo.channelNumber = 0;
      }
      else {
        primaryServingValid = true;
        fiveGNsaPrimaryServingInfo.status = CellConnectionStatus_t::PRIMARY_SERVING;
        fiveGNsaPrimaryServingInfo.rat = radioService.radioFamily;
        fiveGNsaPrimaryServingInfo.rfInfo.range = radioService.radioFrequency.range;
        fiveGNsaPrimaryServingInfo.rfInfo.channelNumber = 0;
      }
    }

    int cid;
    std::map<int32_t, rildata::AllocatedBearer_t>::iterator bIterator;
    for (auto cIterator=mCallInstances.begin(); cIterator!=mCallInstances.end(); ++cIterator) {
      const CallInfo& callInfo = cIterator->getCallInfo();
      cid = callInfo.cid;
      // check all allocated bearers to cid
      if (!callInfo.bearerCollection.bearers.empty()) {
        for (auto i=callInfo.bearerCollection.bearers.begin() ; i!=callInfo.bearerCollection.bearers.end() ; ++i) {
          if (i->downlink == RatType_t::RAT_4G) {
            fiveGNsaPrimaryServingInfo.contextIds.push_back(cid);
          }
          else if (i->downlink == RatType_t::RAT_5G) {
            fiveGNsaSecondaryServingInfo.contextIds.push_back(cid);
          }
          else if (i->downlink == RatType_t::RAT_SPLITED) {
            fiveGNsaPrimaryServingInfo.contextIds.push_back(cid);
            fiveGNsaSecondaryServingInfo.contextIds.push_back(cid);
          }
        }
      } else {
        fiveGNsaPrimaryServingInfo.contextIds.push_back(cid);
      }
    }

    if (primaryServingValid) {
      for (NasPhysChanInfo nasinfo : mPhysicalChannelInfo) {
        if(nasinfo.getStatus() == NasPhysChanInfo::Status::PRIMARY) {
          fiveGNsaPrimaryServingInfo.cellBandwidthDownlink = nasinfo.getBandwidth();
          fiveGNsaPrimaryServingInfo.physicalCellId = nasinfo.getPhysicalCellId();
          physicalConfigStructInfo.push_back(fiveGNsaPrimaryServingInfo);
        }
      }
    }

    if (secondaryServingValid) {
      // cellBandwidthDownlink and physicalCellId are not available for 5G NSA
      fiveGNsaSecondaryServingInfo.cellBandwidthDownlink = 0;
      fiveGNsaSecondaryServingInfo.physicalCellId = 0;
      physicalConfigStructInfo.push_back(fiveGNsaSecondaryServingInfo);
    }
  }
  // No 5G case
  else {
    for (NasPhysChanInfo nasinfo : mPhysicalChannelInfo) {
      PhysicalConfigStructInfo_t servingInfo;
      if(nasinfo.getStatus() == NasPhysChanInfo::Status::PRIMARY) {
        servingInfo.status = CellConnectionStatus_t::PRIMARY_SERVING;
      }
      else {
        servingInfo.status = CellConnectionStatus_t::SECONDARY_SERVING;
      }
      servingInfo.cellBandwidthDownlink = nasinfo.getBandwidth();
      servingInfo.rat = ratType;
      servingInfo.rfInfo.range = frequencyRange;
      servingInfo.rfInfo.channelNumber = 0;
      int cid;
      for (auto cIterator=mCallInstances.begin(); cIterator!=mCallInstances.end(); ++cIterator) {
        cid = cIterator->getCallInfo().cid;
        servingInfo.contextIds.push_back(cid);
      }
      servingInfo.physicalCellId = nasinfo.getPhysicalCellId();
      physicalConfigStructInfo.push_back(servingInfo);
    }
  }

  if (physicalConfigStructInfoChanged(physicalConfigStructInfo)) {
    mPhysicalConfigStructInfo.swap(physicalConfigStructInfo);
    auto physConfmsg = std::make_shared<rildata::PhysicalConfigStructUpdateMessage>(mPhysicalConfigStructInfo);
    if (physConfmsg != nullptr) {
        Log::getInstance().d("[CallManager]: " + physConfmsg->dump());
        logBuffer.addLogWithTimestamp("[CallManager]: " + physConfmsg->dump());
        physConfmsg->broadcast();
    }
  }
}

void CallManager::enablePhysChanConfigReporting(bool enable)
{
  Log::getInstance().d("[CallManager]: enabledPhysChanConfigReporting = "+std::to_string(enable));
  mReportPhysicalChannelConfig = enable;
  if(mReportPhysicalChannelConfig) {
    updatePhysicalChannelConfigs();
  } else {
    mPhysicalConfigStructInfo.clear();
  }
}

void CallManager::handleNasPhysChanConfigMessage(std::shared_ptr<Message> msg)
{
  std::shared_ptr<NasPhysChanConfigMessage> m = std::static_pointer_cast<NasPhysChanConfigMessage>(msg);
  if(m != nullptr) {
    std::shared_ptr<const std::vector<NasPhysChanInfo>> info = m->getInfo();
    if (info != nullptr) {
      mPhysicalChannelInfoValid = true;
      mPhysicalChannelInfo = *info;
    }
    if (mReportPhysicalChannelConfig) {
      updatePhysicalChannelConfigs();
    }
    else {
      Log::getInstance().d("[CallManager]: PhysicalChannelConfig reporting is nullptr or disabled "+
                            std::to_string((int)mReportPhysicalChannelConfig));
    }
  }
}

int getCid(CallEventTypeEnum event, std::shared_ptr<Message> msg) {
  int cid = -1;
  switch (event) {
    case CallEventTypeEnum::DeactivateDataCall:
    {
      auto m = std::static_pointer_cast<DeactivateDataCallRequestMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::RilEventDataCallback:
    {
      auto m = std::static_pointer_cast<RilEventDataCallback>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::TimerExpired:
    {
      auto m = std::static_pointer_cast<DataCallTimerExpiredMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::HandoverInformationInd:
    {
      auto m = std::static_pointer_cast<HandoverInformationIndMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::SetPreferredSystem:
    {
      auto m = std::static_pointer_cast<SetPreferredSystemMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    case CallEventTypeEnum::LinkPropertiesChanged:
    {
      auto m = std::static_pointer_cast<LinkPropertiesChangedMessage>(msg);
      if (m != nullptr) {
        cid = m->getCid();
      }
      break;
    }
    default:
      Log::getInstance().d("Unable to read cid from call event message");
      break;
  }
  return cid;
}

void CallManager::cleanCallInstance(int cid) {
  availableCallIds[cid] = true;
  for (auto it = mCallInstances.begin(); it != mCallInstances.end(); it++) {
    CallInfo& callInfo = it->getCallInfo();
    if (callInfo.cid == cid) {
      mCallInstances.erase(it);
      break;
    }
  }
}

void CallManager::setProcessDeferredIntentCallback(std::function<void(DeferredIntentToChange_t)> cb) {
  Log::getInstance().d("[CallManager] setProcessDeferredIntentCallback");
  mProcessDeferredIntentCb = cb;
}

bool CallManager::shouldDeferIntentToChange(string apn) {
  bool ret = true;
  list<reference_wrapper<CallStateMachine>> matchedCalls;
  matchedCalls = mFilter
      .matchApn(apn)
      .matchCallStates({_eConnecting})
      .getResults();
  if (matchedCalls.empty()) {
    ret = false;
  }
  Log::getInstance().d("[CallManager] shouldDeferIntentToChange = "+apn+", ret = "+std::to_string(ret));
  return ret;
}

void CallManager::deferToProcessIntentToChange(DeferredIntentToChange_t deferIntent) {
  string apn(deferIntent.apnPrefSys.apn_pref_info.apn_name);
  Log::getInstance().d("[CallManager] deferToProcessIntentToChange = " + apn);

  list<reference_wrapper<CallStateMachine>> matchedCalls;
  matchedCalls = mFilter
      .matchApn(apn)
      .matchCallStates({_eConnecting})
      .getResults();
  if(!matchedCalls.empty()) {
    deferredIntents[apn] = deferIntent;
    CallStateMachine& callSM = matchedCalls.front();
    callSM.setCallStateChangedCallback(std::bind(&CallManager::callStateChangedCallback, this, _1, _2));
  }
}

void CallManager::callStateChangedCallback(string apn, CallStateEnum state) {
  Log::getInstance().d("[CallManager] callStateChangedCallback = " + apn + ", state = " + to_string((int)state));
  if(state != _eConnecting) {
    auto it = deferredIntents.find(apn);
    if(it != deferredIntents.end() && mProcessDeferredIntentCb) {
      mProcessDeferredIntentCb(it->second);
      deferredIntents.erase(apn);
    }
  }
}

bool CallManager::validateSetupDataCallParams(std::shared_ptr<SetupDataCallRequestMessage> m) {
  HandoffNetworkType_t NetType = (m->getAccessNetwork() == AccessNetwork_t::IWLAN)?
                      HandoffNetworkType_t::_eIWLAN:HandoffNetworkType_t::_eWWAN;
  SetupDataCallResponse_t result = {};
  result.call.suggestedRetryTime = -1;
  // In AP-assist mode, if setupDataCall request with IWLAN on Radio,
  // it is responded with INVALID ARGUMENT
  if(globalInfo.apAssistMode &&
    (((NetType==HandoffNetworkType_t::_eWWAN)&&(m->getRequestSource()==RequestSource_t::IWLAN))||
    ((NetType==HandoffNetworkType_t::_eIWLAN)&&(m->getRequestSource()==RequestSource_t::RADIO)))) {
    Log::getInstance().d("network type mismatch to request source "+std::to_string((int)m->getRequestSource()));
    result.respErr = ResponseError_t::INVALID_ARGUMENT;
    result.call.cause = DataCallFailCause_t::ERROR_UNSPECIFIED;
    auto resp = std::make_shared<SetupDataCallResponse_t>(result);
    stringstream ss;
    ss << "[CallManager]: " << m->getSerial() << "< setupDataCallResponse resp=";
    result.dump("", ss);
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    m->sendResponse(m, Message::Callback::Status::FAILURE, resp);
    return false;
  }

  if(isApnTypeEmergency(m->getSupportedApnTypesBitmap())) {
    Log::getInstance().d("[CallManager]: Allow EIMS call");
    return true;
  }

  if (m->getRequestSource()==RequestSource_t::RADIO && !mRadioDataAvailable && !mRadioVoiceAvailable && mNulBearerReasonAvailable) {
    Log::getInstance().d("[CallManager]: Reject request because both PS/CS not available");
    result.respErr = ResponseError_t::CALL_NOT_AVAILABLE;
    result.call.cause = DataCallFailCause_t::DATA_REGISTRATION_FAIL;
    auto resp = std::make_shared<SetupDataCallResponse_t>(result);
    stringstream ss;
    ss << "[CallManager]: " << m->getSerial() << "< setupDataCallResponse resp=";
    result.dump("", ss);
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    m->sendResponse(m, Message::Callback::Status::FAILURE, resp);
    return false;
  }
  return true;
}

void CallManager::processEventSendResponse(CallEventTypeEnum event, std::shared_ptr<Message> msg, ResponseError_t errorCode) {
  switch (event) {
    case CallEventTypeEnum::SetupDataCall:
    {
      auto m = std::static_pointer_cast<SetupDataCallRequestMessage>(msg);
      if (m != nullptr) {
        SetupDataCallResponse_t result = {};
        result.respErr = errorCode;
        result.call = {.cause = DataCallFailCause_t::OEM_DCFAILCAUSE_4};
        result.call.suggestedRetryTime = -1;
        result.call.cid = -1;
        auto resp = std::make_shared<SetupDataCallResponse_t>(result);
        stringstream ss;
        ss << "[CallManager]: " << m->getSerial() << "< setupDataCallResponse resp=";
        result.dump("", ss);
        Log::getInstance().d(ss.str());
        logBuffer.addLogWithTimestamp(ss.str());
        m->sendResponse(m, Message::Callback::Status::SUCCESS, resp);
      }
      else {
        Log::getInstance().d("[CallManager]: Corrupted SetupDataCallRequestMessage - not able to send response");
      }
      break;
    }
    case CallEventTypeEnum::DeactivateDataCall:
    {
      auto m = std::static_pointer_cast<DeactivateDataCallRequestMessage>(msg);
      if (m != nullptr) {
        ResponseError_t result = errorCode;
        auto resp = std::make_shared<ResponseError_t>(result);
        Log::getInstance().d("[CallManager]: " + std::to_string(m->getSerial()) + "< deactivateDataCallResponse resp=" +
            std::to_string((int)result) + " cid=" + std::to_string(m->getCid()));
        stringstream ss;
        ss << "[CallManager]: " << m->getSerial() << "< deactivateDataCallResponse resp=" << (int)result << " cid=" << m->getCid();
        Log::getInstance().d(ss.str());
        logBuffer.addLogWithTimestamp(ss.str());
        m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
      }
      else {
        Log::getInstance().d("[CallManager]: Corrupted DeactivateDataCallRequestMessage - not able to send response");
      }
      break;
    }
    default:
      Log::getInstance().d("[CallManager]: Ignoring message " + msg->get_message_name());
      break;
  }
}

void CallManager::handleCallEventMessage(CallEventTypeEnum event, std::shared_ptr<Message> msg) {
  CallEventType callEvent = { .type = event, .msg = msg };
  Log::getInstance().d("[CallManager]: Handling " + callEvent.getEventName() + " msg = " + msg->get_message_name());
  list<reference_wrapper<CallStateMachine>> matchedCalls;
  list<reference_wrapper<CallStateMachine>> matchedDefaultCalls;
  if (event == CallEventTypeEnum::SetupDataCall) {
    // Lookup call instance by apn, apn type, and ip type. Create one if no match is found
    auto m = std::static_pointer_cast<SetupDataCallRequestMessage>(msg);
    if (m != nullptr && validateSetupDataCallParams(m)) {
      if (isApnTypeEmergency(m->getSupportedApnTypesBitmap())) {
        matchedCalls = mFilter
            .matchEmergencySupport(true)
            .getResults();
      } else {
        matchedCalls = mFilter
            .matchApn(m->getApn())
            .matchApnTypes(m->getSupportedApnTypesBitmap())
            .matchIpType(m->getProtocol())
            .getResults();
      }
      if (matchedCalls.empty()) {
        if( !(dsiInitStatus == DsiInitStatus_t::COMPLETED) ) {
          Log::getInstance().d("[CallManager]: DSI init not yet completed");
          processEventSendResponse(event, m, ResponseError_t::INTERNAL_ERROR);
          return;
        }
        int cid = getAvailableCid();
        if (cid >= 0) {
          mCallInstances.emplace_back(cid, logBuffer, globalInfo);
          matchedCalls.push_back(mCallInstances.back());
          CallInfo& callInfo = mCallInstances.back().getCallInfo();
          callInfo.callBringUpCapability = mCallBringupCapability;
          callInfo.radioRAT = RadioAccessFamily_t::UNKNOWN;
          callInfo.wds_endpoint = wds_endpoint;
          callInfo.dsd_endpoint = dsd_endpoint;
          callInfo.requestedIpType = m->getProtocol();
          callInfo.requestedRoamingIpType = m->getRoamingProtocol();
          callInfo.dataCallListChangedCallback = std::bind(&CallManager::dataCallListChanged, this);
          callInfo.cleanupKeepAliveCallback = mCleanupKeepAliveCb;
          if(!availableRadioDataService.empty())
          {
            AvailableRadioDataService_t radioData = availableRadioDataService.front();
            callInfo.radioRAT = radioData.radioFamily;
          }
        } else {
            abort();
        }
      }
    } else {
      Log::getInstance().d("[CallManager] : No valid datacall is received");
      return;
    }
  } else {
    // Lookup call instance by cid
    int cid = getCid(event, msg);
    matchedCalls = mFilter
        .matchCid(cid)
        .getResults();
  }

  if (!matchedCalls.empty()) {
    // Dispatch event to state machine
    CallStateMachine& callInstance = matchedCalls.front();
    CallInfo& callInfo = callInstance.getCallInfo();
    callInstance.processEvent(callEvent);
    int cid = callInfo.cid;
    int currentState = callInstance.getCurrentState();
    if (currentState == _eDisconnected) {
      cleanCallInstance(cid);
      dataCallListChanged();
    }
  } else {
    Log::getInstance().d("[CallManager]: No call found for = " + msg->dump());
    processEventSendResponse(event, msg, ResponseError_t::CALL_NOT_AVAILABLE);
  }
}

void CallManager::generateCurrentPhysicalChannelConfigsFromBearers()
{
    Log::getInstance().d("[CallManager]: generateCurrentPhysicalChannelConfigsFromBearers size = " +
                          std::to_string((int)mPhysicalChannelInfo.size()));
    bool fiveGAvailable = false;
    RadioAccessFamily_t ratType = RadioAccessFamily_t::UNKNOWN;
    FrequencyRange_t frequencyRange = FrequencyRange_t::LOW;
    std::vector<PhysicalConfigStructInfo_t> physicalConfigStructInfo;

    for (auto radioService : availableRadioDataService) {
      if(radioService.radioFamily == RadioAccessFamily_t::NR) {
        fiveGAvailable = true;
        break;
      }
      ratType = radioService.radioFamily;
      frequencyRange = radioService.radioFrequency.range;
    }

    if(fiveGAvailable) {
      PhysicalConfigStructInfo_t fiveGNsaPrimaryServingInfo = {};
      bool primaryServingValid = false;
      for (auto radioService : availableRadioDataService) {
        if(radioService.radioFamily == RadioAccessFamily_t::NR) {
          primaryServingValid = true;
          fiveGNsaPrimaryServingInfo.status = CellConnectionStatus_t::PRIMARY_SERVING;
          fiveGNsaPrimaryServingInfo.rat = radioService.radioFamily;
          fiveGNsaPrimaryServingInfo.rfInfo.range = radioService.radioFrequency.range;
          fiveGNsaPrimaryServingInfo.rfInfo.channelNumber = 0;
        }
      }

      for (auto cIterator=mCallInstances.begin(); cIterator!=mCallInstances.end(); ++cIterator) {
        // find cid from call list
        const CallInfo& callInfo = cIterator->getCallInfo();
        int cid = callInfo.cid;
        // check all allocated bearers to cid
        for (auto i=callInfo.bearerCollection.bearers.begin() ; i!=callInfo.bearerCollection.bearers.end() ; ++i) {
          if (i->downlink == RatType_t::RAT_5G) {
            fiveGNsaPrimaryServingInfo.contextIds.push_back(cid);
          }
        }
      }
      physicalConfigStructInfo.push_back(fiveGNsaPrimaryServingInfo);
    }
    // No 5G case
    else {
      PhysicalConfigStructInfo_t primaryServingInfo = {.status = CellConnectionStatus_t::PRIMARY_SERVING};
      PhysicalConfigStructInfo_t secondaryServingInfo = {.status = CellConnectionStatus_t::SECONDARY_SERVING};
      for (auto cIterator=mCallInstances.begin(); cIterator!=mCallInstances.end(); ++cIterator) {
        int cid = cIterator->getCallInfo().cid;
        primaryServingInfo.contextIds.push_back(cid);
        secondaryServingInfo.contextIds.push_back(cid);
      }
      physicalConfigStructInfo.push_back(primaryServingInfo);
      physicalConfigStructInfo.push_back(secondaryServingInfo);
    }

    if (physicalConfigStructInfoChanged(physicalConfigStructInfo)) {
      mPhysicalConfigStructInfo.swap(physicalConfigStructInfo);
      auto physConfmsg = std::make_shared<rildata::PhysicalConfigStructUpdateMessage>(mPhysicalConfigStructInfo);
      if (physConfmsg != nullptr) {
        Log::getInstance().d("[CallManager]: " + physConfmsg->dump());
        logBuffer.addLogWithTimestamp("[CallManager]: " + physConfmsg->dump());
        physConfmsg->broadcast();
      }
    }
}

bool CallManager::physicalConfigStructInfoChanged(const std::vector<PhysicalConfigStructInfo_t>& configs) {
  if (configs.size() != mPhysicalConfigStructInfo.size()) {
    return true;
  }
  return !std::equal(mPhysicalConfigStructInfo.begin(), mPhysicalConfigStructInfo.end(), configs.begin(),
    [](const PhysicalConfigStructInfo_t& oldConfigs, const PhysicalConfigStructInfo_t& newConfigs) {
      if (oldConfigs.status != newConfigs.status ||
          oldConfigs.cellBandwidthDownlink != newConfigs.cellBandwidthDownlink ||
          oldConfigs.rat != newConfigs.rat ||
          oldConfigs.rfInfo.range != newConfigs.rfInfo.range ||
          oldConfigs.physicalCellId != newConfigs.physicalCellId ||
          oldConfigs.contextIds.size() != newConfigs.contextIds.size()) {
        return false;
      }

      return std::equal(oldConfigs.contextIds.begin(), oldConfigs.contextIds.end(), newConfigs.contextIds.begin());
  });
}

void CallManager::updatePhysicalChannelConfigs()
{
  Log::getInstance().d("[CallManager]: updatePhysicalChannelConfigs");
  if (mSaMode) {
    generateCurrentPhysicalChannelConfigsFromBearers();
  } else {
    if (mPhysicalChannelInfoValid) {
      generateCurrentPhysicalChannelConfigs();
    } else {
      auto nasGetPhyChanConfigMsg = std::make_shared<NasGetPhyChanConfigMessage>();
      if (nasGetPhyChanConfigMsg != nullptr) {
        GenericCallback<std::vector<NasPhysChanInfo>> cb([](std::shared_ptr<Message> nasGetPhyChanConfigMsg,
                                                      Message::Callback::Status status,
                                                      std::shared_ptr<std::vector<NasPhysChanInfo>> rsp) -> void {
          if (nasGetPhyChanConfigMsg && rsp) {
            if(status == Message::Callback::Status::SUCCESS) {
              Log::getInstance().d("NasPhysChanInfo size = " + std::to_string((int)rsp->size()));
              auto indMsg = std::make_shared<NasPhysChanConfigMessage>(rsp);
              indMsg->broadcast();
            }
            else {
              Log::getInstance().d("[CallManager]: NasGetPhyChanConfigMessage response error");
            }
          }
          if(rsp == nullptr) {
            Log::getInstance().d("[CallManager]: PhysicalChannelConfig reporting is nullptr");
          }
        });
        nasGetPhyChanConfigMsg->setCallback(&cb);
        nasGetPhyChanConfigMsg->dispatch();
      }
      else {
        Log::getInstance().d("[CallManager]: NasGetPhyChanConfigMessage error");
      }
    }
  }
}

int CallManager::toggleLinkActiveStateChangeReport(bool report)
{
  Log::getInstance().d("[CallManager]: toggleLinkActiveStateChangeReport "+std::to_string((int)report));
  if (globalInfo.linkStateChangeReport != report) {
    globalInfo.linkStateChangeReport = report;
    for (auto it=mCallInstances.begin(); it!=mCallInstances.end(); ++it) {
      it->updateLinkStateChangeReport();
    }
  }

  return 0;
}

bool CallManager::handleGoDormantRequest(void *deviceName, size_t len)
{
  bool status = false;
  std::string *devName = static_cast<std::string *>(deviceName);
  std::ignore = devName;
  std::ignore = len;

  if (len > 0) {
    auto matchedCalls = mFilter
        .matchDeviceName(*devName)
        .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      status = callInstance.sendDormancyRequest();
    }
  } else {
     for (auto it=mCallInstances.begin(); it!=mCallInstances.end(); ++it) {
      status = it->sendDormancyRequest();
     }
  }
  return status;
}

void CallManager::registerDataRegistrationRejectCause(bool enable)
{
  Log::getInstance().d("[CallManager]: registerDataRegistrationRejectCause "+std::to_string(enable));
  mReportRegistrationRejectCause = enable;
}

#ifndef RIL_FOR_LOW_RAM
bool CallManager::getEmbmsCallInfo() {
  for (auto it = mCallInstances.begin(); it != mCallInstances.end(); it++) {
    CallInfo& callInfo = it->getCallInfo();
    if (callInfo.embmsInfo != nullptr) {
      Log::getInstance().d("Found an Embms call");
      return true;
    }
  }
  return false;
}

void CallManager::handleEmbmsEnableDataRequestMessage(qcril_instance_id_e_type instance_id)
{
  int cid = getAvailableCid();
  list<reference_wrapper<CallStateMachine>> matchedCalls;
  if (cid < 0) {
    Log::getInstance().d("[CallManager]: handleEmbmsEnableDataRequestMessage No available cid");
    return;
  }
  mCallInstances.emplace_back(cid, logBuffer, globalInfo);
  CallStateMachine& callInstance = mCallInstances.back();
  CallInfo& callInfo = callInstance.getCallInfo();
  callInfo.eventId = QCRIL_EVT_QMI_RIL_EMBMS_ENABLE_DATA_REQ;

  BaseProfile::params_t params;
  params.profileId = QDP_PROFILE_ID_INVALID;
  params.apn = "";
  params.protocol = "IP";
  params.roamingAllowed = TRUE;

  callInfo.profileInfo = std::make_unique<CallSetupProfile>(params);
  if (callInfo.profileInfo == nullptr) {
    Log::getInstance().d("[CallManager]:: profileInfo doesn't exist");
    mCallInstances.pop_back();
    return;
  }

  callInfo.embmsInfo = std::make_shared<EmbmsCallHandler>(logBuffer);
  callInfo.embmsInfo->setInstanceId(instance_id);

  CallEventType callEvent = { .type = CallEventTypeEnum::EnableEmbmsCall, .msg = nullptr };
  callInstance.processEvent(callEvent);
}


void CallManager::handleEmbmsActivateTmgi(qcril_request_params_type *req)
{
  Log::getInstance().d("[CallManager]: handleEmbmsActivateTmgi");
  if(req != nullptr)
  {
    embms_activate_tmgi_req_msg_v01 *activate_req = (embms_activate_tmgi_req_msg_v01 *)(req->data);
    int cid = activate_req->call_id;
    auto matchedCalls = mFilter
      .matchCid(cid)
      .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      CallInfo& callInfo = callInstance.getCallInfo();
      if (callInfo.embmsInfo != nullptr) {
        callInfo.embmsInfo->setDsiHandle(callInfo.dsiHandle);
        callInfo.embmsInfo->setInstanceId(req->instance_id);
        callInfo.embmsInfo->activateTmgi(req);
      }
    }
  }
}

void CallManager::handleEmbmsDeactivateTmgi(qcril_request_params_type *req)
{
  Log::getInstance().d("[CallManager]: handleEmbmsDeactivateTmgi");
  if(req != nullptr)
  {
    embms_deactivate_tmgi_req_msg_v01 *deactivate_req = (embms_deactivate_tmgi_req_msg_v01 *)(req->data);
    auto matchedCalls = mFilter
      .matchCid(deactivate_req->call_id)
      .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      CallInfo& callInfo = callInstance.getCallInfo();
      if (callInfo.embmsInfo != nullptr) {
        callInfo.embmsInfo->setDsiHandle(callInfo.dsiHandle);
        callInfo.embmsInfo->setInstanceId(req->instance_id);
        callInfo.embmsInfo->deactivateTmgi(req);
      }
    }
  }
}

void CallManager::handleEmbmsActivateDeactivateTmgi(qcril_request_params_type *req)
{
  Log::getInstance().d("[CallManager]: handleEmbmsActivateDeactivateTmgi");
  if(req != nullptr)
  {
    embms_activate_deactivate_tmgi_req_msg_v01 *act_deactivate_req = (embms_activate_deactivate_tmgi_req_msg_v01 *)(req->data);
    auto matchedCalls = mFilter
      .matchCid(act_deactivate_req->call_id)
      .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      CallInfo& callInfo = callInstance.getCallInfo();
      if (callInfo.embmsInfo != nullptr) {
        callInfo.embmsInfo->setDsiHandle(callInfo.dsiHandle);
        callInfo.embmsInfo->setInstanceId(req->instance_id);
        callInfo.embmsInfo->activateDeactivateTmgi(req);
      }
    }
  }
}

void CallManager::handleEmbmsContentDescUpdate(qcril_request_params_type *req)
{
  Log::getInstance().d("[CallManager]: handleEmbmsContentDescUpdate");
  if(req != nullptr)
  {
    embms_update_content_desc_req_msg_v01 *get_update_req = (embms_update_content_desc_req_msg_v01 *)(req->data);
    auto matchedCalls = mFilter
      .matchCid(get_update_req->call_id)
      .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      CallInfo& callInfo = callInstance.getCallInfo();
      if (callInfo.embmsInfo != nullptr) {
        callInfo.embmsInfo->setDsiHandle(callInfo.dsiHandle);
        callInfo.embmsInfo->setInstanceId(req->instance_id);
        callInfo.embmsInfo->contentDescUpdate(req);
      }
    }
  }
}

void CallManager::handleEmbmsGetActiveTmgi(qcril_request_params_type *req)
{
  Log::getInstance().d("[CallManager]: handleEmbmsGetActiveTmgi");
  if(req != nullptr)
  {
    embms_get_active_tmgi_req_msg_v01 *get_active_req = (embms_get_active_tmgi_req_msg_v01 *)(req->data);
    auto matchedCalls = mFilter
      .matchCid(get_active_req->call_id)
      .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      CallInfo& callInfo = callInstance.getCallInfo();
      if (callInfo.embmsInfo != nullptr) {
        callInfo.embmsInfo->setDsiHandle(callInfo.dsiHandle);
        callInfo.embmsInfo->setInstanceId(req->instance_id);
        callInfo.embmsInfo->getActiveTmgi(req);
      }
    }
  }
}

void CallManager::handleEmbmsGetAvailableTmgi(qcril_request_params_type *req)
{
  Log::getInstance().d("[CallManager]: handleEmbmsGetAvailableTmgi");
  if(req != nullptr)
  {
    embms_get_available_tmgi_req_msg_v01 *get_available_req = (embms_get_available_tmgi_req_msg_v01 *)(req->data);
    auto matchedCalls = mFilter
      .matchCid(get_available_req->call_id)
      .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      CallInfo& callInfo = callInstance.getCallInfo();
      if (callInfo.embmsInfo != nullptr) {
        callInfo.embmsInfo->setDsiHandle(callInfo.dsiHandle);
        callInfo.embmsInfo->setInstanceId(req->instance_id);
        callInfo.embmsInfo->getAvailableTmgi(req);
      }
    }
  }
}

void CallManager::handleEmbmsSendInterestedList(qcril_request_params_type *req)
{
  Log::getInstance().d("[CallManager]: handleEmbmsSendInterestedList");
  if(req != nullptr)
  {
    embms_get_interested_tmgi_list_resp_msg_v01 *interest_list_resp = (embms_get_interested_tmgi_list_resp_msg_v01 *)(req->data);
    auto matchedCalls = mFilter
      .matchCid(interest_list_resp->call_id)
      .getResults();
    if (!matchedCalls.empty()) {
      CallStateMachine& callInstance = matchedCalls.front();
      CallInfo& callInfo = callInstance.getCallInfo();
      if (callInfo.embmsInfo != nullptr) {
        callInfo.embmsInfo->setDsiHandle(callInfo.dsiHandle);
        callInfo.embmsInfo->setInstanceId(req->instance_id);
        callInfo.embmsInfo->sendInterestedList(req);
      }
    }
  }
}
#endif// RIL_FOR_LOW_RAM

CallManager::Filter& CallManager::Filter::matchCid(int cid) {
  mCriteriaBitmask |= Criteria::CID;
  mCid = cid;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchApn(std::string apn) {
  mCriteriaBitmask |= Criteria::APN;
  mApn = apn;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchApnTypes(ApnTypes_t apnTypes) {
  mCriteriaBitmask |= Criteria::APN_TYPES;
  mApnTypes = apnTypes;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchIpType(std::string ipType) {
  mCriteriaBitmask |= Criteria::IP_TYPE;
  mIpType = ipType;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchDeviceName(std::string deviceName) {
  mCriteriaBitmask |= Criteria::DEVICE_NAME;
  mDeviceName = deviceName;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchEmergencySupport(bool emergencySupport) {
  mCriteriaBitmask |= Criteria::EMERGENCY_SUPPORT;
  mEmergencySupport = emergencySupport;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchCurrentNetwork(HandoffNetworkType_t currentNetwork) {
  mCriteriaBitmask |= Criteria::CURRENT_NETWORK;
  mCurrentNetwork = currentNetwork;
  return *this;
}

CallManager::Filter& CallManager::Filter::matchCallStates(const std::set<int>& states) {
  mCriteriaBitmask |= Criteria::STATES;
  mStates = states;
  return *this;
}

bool CallManager::Filter::matchesCriteria(CallStateMachine& call) {
  const CallInfo& callInfo = call.getCallInfo();
  for (uint32_t matchType = 1; matchType < Criteria::CRITERIA_MAX; matchType <<= 1) {
    if ((matchType & mCriteriaBitmask) == matchType) {
      switch (matchType) {
        case Criteria::CID:
          if(mCid != callInfo.cid) {
            return false;
          }
          break;
        case Criteria::APN:
          if (callInfo.profileInfo == nullptr ||
              mApn.length() != callInfo.profileInfo->getApn().length()) {
            return false;
          }
          if(!std::equal(mApn.begin(), mApn.end(), callInfo.profileInfo->getApn().begin(), []
              (auto a, auto b){return std::tolower(a)==std::tolower(b);})) {
            return false;
          }
          break;
        case Criteria::APN_TYPES:
          if (callInfo.profileInfo == nullptr ||
              (static_cast<uint32_t>(mApnTypes) != callInfo.profileInfo->getApnTypes())) {
            return false;
          }
          break;
        case Criteria::IP_TYPE:
          if (callInfo.profileInfo == nullptr ||
              mIpType != callInfo.profileInfo->getProtocol()) {
            return false;
          }
          break;
        case Criteria::DEVICE_NAME:
          if (mDeviceName.length() != callInfo.deviceName.length()) {
            return false;
          }
          if(!std::equal(mDeviceName.begin(), mDeviceName.end(), callInfo.deviceName.begin(), []
              (auto a, auto b){return std::tolower(a)==std::tolower(b);})) {
            return false;
          }
          break;
        case Criteria::EMERGENCY_SUPPORT:
          if (callInfo.profileInfo == nullptr ||
              mEmergencySupport != callInfo.profileInfo->getIsEmergencyCall()) {
            return false;
          }
          break;
        case Criteria::CURRENT_NETWORK:
          if (mCurrentNetwork != (mCurrentNetwork & callInfo.currentRAT)) {
            return false;
          }
          break;
        case Criteria::STATES:
          if (mStates.find(call.getCurrentState()) == mStates.end()) {
            return false;
          }
          break;
        default:
          Log::getInstance().d("[CallManager::Filter]: invalid criteria");
          return false;
      }
    }
  }
  return true;
}

list<reference_wrapper<CallStateMachine>> CallManager::Filter::getResults() {
  std::list<reference_wrapper<CallStateMachine>> filtered(mCallList.begin(), mCallList.end());
  if (mCriteriaBitmask == 0) {
    return filtered;
  }

  filtered.remove_if([this](CallStateMachine& call) -> bool {
    return !matchesCriteria(call);
  });
  clearCriteria();
  return filtered;
}

void CallManager::Filter::clearCriteria() {
  mCriteriaBitmask = 0;
  mCid = 0;
  mApn = "";
  mApnTypes = ApnTypes_t::NONE;
  mIpType = "";
  mDeviceName = "";
  mEmergencySupport = false;
  mCurrentNetwork = HandoffNetworkType_t::_eWWAN;
}

void CallManager::dsiInitCbFunc(void *userData)
{
  (void)userData;
  Log::getInstance().d("Received DSI Init callback");
  auto msg = std::make_shared<DsiInitCompletedMessage>();
  if( msg != NULL ) {
    msg->broadcast();
  }
}

void CallManager::triggerDsiInit(bool afterBootup)
{
  if( dsiInitStatus == DsiInitStatus_t::RELEASED )
  {
    if( dsiInit(!afterBootup, dsiInitCbFunc) == DSI_SUCCESS ) {
      dsiInitStatus = DsiInitStatus_t::STARTED;
      Log::getInstance().d("[CallManager]: DSI Init Successful");
    } else {
      Log::getInstance().d("[CallManager]: DSI Init Failed");
    }
  } else {
    Log::getInstance().d("[CallManager]: DSI Init is already Started/Progress/Completed");
  }
}

void CallManager::triggerDsiRelease()
{
  if( (dsiInitStatus == DsiInitStatus_t::COMPLETED) || (dsiInitStatus == DsiInitStatus_t::PENDING_RELEASE) )
  {
    if( dsiRelease() == DSI_SUCCESS ) {
      Log::getInstance().d("[CallManager]: DSI Release Successful");
      dsiInitStatus = DsiInitStatus_t::RELEASED;
    } else {
      Log::getInstance().d("[CallManager]: DSI Release Failed");
      dsiInitStatus = DsiInitStatus_t::PENDING_RELEASE;
    }
  } else {
    dsiInitStatus = DsiInitStatus_t::PENDING_RELEASE;
    Log::getInstance().d("[CallManager]: DSI Init not yet completed. Ignore DSI release");
  }
}

void CallManager::cleanAllCalls()
{
  Log::getInstance().d("[CallManager]: cleanAllCalls");
  for (auto it = mCallInstances.begin(); it != mCallInstances.end();) {
    auto it_save = it;
    it++;
    CallEventType callEvent = { .type = CallEventTypeEnum::CleanUp, .msg = nullptr };
    it_save->processEvent(callEvent);
    CallInfo& callInfo = it_save->getCallInfo();
    availableCallIds[callInfo.cid] = true;
    mCallInstances.erase(it_save);
  }
}

void CallManager::updateNasRfActiveBandInfo(std::shared_ptr<Message> msg)
{
   std::shared_ptr<NasRfBandInfoIndMessage> m = std::static_pointer_cast<NasRfBandInfoIndMessage>(msg);
  if(m != nullptr) {
    mActiveBandInfo = static_cast<rildata::NasActiveBand_t>(m->getNasRFActiveBand());
    Log::getInstance().d("[CallManager]: Received NAS RF Active Band Info"+std::to_string((int32_t)mActiveBandInfo));
  } else {
    Log::getInstance().d("[CallManager]: NasPhysChanConfigMessage message is NULL");
  }
}

void CallManager::handleNasRfBandInfoMessage(std::shared_ptr<Message> msg)
{
   std::shared_ptr<NasRfBandInfoMessage> m = std::static_pointer_cast<NasRfBandInfoMessage>(msg);
  if(m != nullptr) {
    mActiveBandInfo = static_cast<NasActiveBand_t>(m->getRfBandInfo());
    Log::getInstance().d("[CallManager]: NasRfBandInfoMessage Key"
                         +std::to_string((int32_t)mActiveBandInfo) +"not present in mNR5GBandToFreqRangeMap ");
  } else {
    Log::getInstance().d("[CallManager]: NasRfBandInfoMessage message is NULL");
  }
}
