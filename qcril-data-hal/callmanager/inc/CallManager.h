/*===========================================================================

  Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef CALLMANAGER
#define CALLMANAGER

#include <map>

#include "framework/Log.h"
#include "framework/Message.h"
#include "dsi_netctrl.h"
#include "CallStateMachine.h"
#include "LocalLogBuffer.h"

#ifdef USE_QCRIL_HAL
#include "modules/nas/NasPhysChanConfigMessage.h"
#else
#include "interfaces/nas/NasPhysChanConfigMessage.h"
#endif

#include "request/RilRequestSetupDataCallMessage.h"
#include "request/RilRequestDeactivateDataCallMessage.h"
#include "request/SetupDataCallRequestMessage.h"
#include "request/DeactivateDataCallRequestMessage.h"
#include "request/GetIWlanDataCallListRequestMessage.h"
#include "sync/GetCallBringUpCapabilitySyncMessage.h"
#include "UnSolMessages/PhysicalConfigStructUpdateMessage.h"
#include "UnSolMessages/NasRfBandInfoIndMessage.h"

#define MaxConcurrentDataCalls DSI_MAX_DATA_CALLS

namespace rildata {

struct AvailableRadioDataService_t {
  RadioAccessFamily_t radioFamily;
  RadioFrequencyInfo_t radioFrequency;
  void dump(std::string padding, std::ostream& os) const;
};

struct DeferredIntentToChange_t {
  dsd_apn_pref_sys_type_ex_v01 apnPrefSys;
  dsd_apn_pref_sys_apn_status_mask_v01 apnStatus;
};

static int prevDefaultProfileId = QDP_PROFILE_ID_INVALID;  // store the profile id of default type profile which needs to be deleted with the next setupdatacall
static int currDefaultProfileId = QDP_PROFILE_ID_INVALID;
/**
 * @brief CallManager class
 * @details CallManager class
 */
class CallManager {
public:
  CallManager(LocalLogBuffer& logBuffer);
  ~CallManager();

  void init(bool apAssistMode, bool partialRetry, unsigned long maxPartialRetryTimeout,
            BringUpCapability capability, bool afterBootup);
  void handleCallEventMessage(CallEventTypeEnum event, std::shared_ptr<Message> msg);
  void handleDataBearerTypeUpdate(int32_t cid, rildata::BearerInfo_t bearer);
  void handleDataAllBearerTypeUpdate(rildata::AllocatedBearer_t bearer);
  void handleNasPhysChanConfigMessage(std::shared_ptr<Message> msg);
  void setDataCallListChangedCallback(std::function<void(const std::vector<rildata::DataCallResult_t>&)> cb);

  int toggleLinkActiveStateChangeReport(bool report);

  void getRadioDataCallList(vector<DataCallResult_t> &callList);
  void getIWlanDataCallList(vector<DataCallResult_t> &callList);

  int getAvailableCid(void);
  void dataCallListChanged(void);
  void radioDataCallListChanged(const std::vector<rildata::DataCallResult_t>& dcList);
  bool getApnByCid(int cid, string& apn);
  void cleanCallInstance(int cid);

  void setProcessDeferredIntentCallback(std::function<void(DeferredIntentToChange_t)> cb);
  bool shouldDeferIntentToChange(string apn);
  void deferToProcessIntentToChange(DeferredIntentToChange_t deferIntent);
  void callStateChangedCallback(string apn, CallStateEnum state);

  ResponseError_t handleToggleBearerAllocationUpdate(bool enable);
  AllocatedBearerResult_t handleGetBearerAllocation(int32_t cid);
  AllocatedBearerResult_t handleGetAllBearerAllocations();
  void updateAllBearerAllocations();
  void cleanUpAllBearerAllocation();
  void updatePhysicalChannelConfigs();
  void enablePhysChanConfigReporting(bool enable);
  void generateCurrentPhysicalChannelConfigs();
  void generateCurrentPhysicalChannelConfigsFromBearers();
  void processQmiDsdApnPreferredSystemResultInd(const dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *ind);
  void processQmiDsdIntentToChangeApnPrefSysInd(const dsd_intent_to_change_apn_pref_sys_ind_msg_v01& ind);
  void processQmiDsdSystemStatusInd(const dsd_system_status_ind_msg_v01 * ind);
  bool notifyApAssistApnPreferredSystemChangeSuported(bool flag);
  void updateCurrentRoamingStatus();
  void updateCurrentRoamingStatus(bool isRoaming);
  static bool isApnTypeInvalid(ApnTypes_t mSupportedApnTypesBitmap);
  static bool isApnTypeEmergency(ApnTypes_t mSupportedApnTypesBitmap);
  void dump(string padding, ostream& os) const;
  bool handleGoDormantRequest(void *deviceName, size_t len);
  bool sendDormancyRequest(CallInfo& callInfo);
  void registerDataRegistrationRejectCause(bool enable);
  vector<QosSession_t> getQosSessionsforCid(int32_t cid);

#ifndef RIL_FOR_LOW_RAM
  void handleEmbmsEnableDataRequestMessage(qcril_instance_id_e_type instance_id);
  void handleEmbmsActivateTmgi(qcril_request_params_type *req);
  void handleEmbmsDeactivateTmgi(qcril_request_params_type *req);
  void handleEmbmsActivateDeactivateTmgi(qcril_request_params_type *req);
  void handleEmbmsContentDescUpdate(qcril_request_params_type *req);
  void handleEmbmsGetActiveTmgi(qcril_request_params_type *req);
  void handleEmbmsGetAvailableTmgi(qcril_request_params_type *req);
  void handleEmbmsSendInterestedList(qcril_request_params_type *req);
  bool getEmbmsCallInfo();
#endif

  void setCleanupKeepAliveCallback(std::function<void(int)> cb);

  void cleanAllCalls();

  void triggerDsiInit(bool);
  void triggerDsiRelease();
  static void dsiInitCbFunc(void *userData);
  DsiInitStatus_t dsiInitStatus;
  void updateNasRfActiveBandInfo(std::shared_ptr<Message> msg);
  void handleNasRfBandInfoMessage(std::shared_ptr<Message> msg);
private:

  class Filter {
    public:
      Filter(std::list<CallStateMachine>& cl): mCallList(cl) {
        clearCriteria();
      }
      Filter& matchCid(int cid);
      Filter& matchApn(std::string apn);
      Filter& matchApnTypes(ApnTypes_t apnTypes);
      Filter& matchIpType(std::string ipType);
      Filter& matchDeviceName(std::string deviceName);
      Filter& matchEmergencySupport(bool emergencySupport);
      Filter& matchCurrentNetwork(HandoffNetworkType_t currentNetwork);
      Filter& matchCallStates(const std::set<int>& states);
      std::list<std::reference_wrapper<CallStateMachine>> getResults();
    private:
      enum Criteria : uint32_t {
        CID                = 1,
        APN                = 1 << 1,
        APN_TYPES          = 1 << 2,
        IP_TYPE            = 1 << 3,
        DEVICE_NAME        = 1 << 4,
        EMERGENCY_SUPPORT  = 1 << 5,
        CURRENT_NETWORK    = 1 << 6,
        STATES             = 1 << 7,
        CRITERIA_MAX,
      };
      uint32_t mCriteriaBitmask;
      int mCid;
      std::string mApn;
      ApnTypes_t mApnTypes;
      std::string mIpType;
      std::string mDeviceName;
      bool mEmergencySupport;
      HandoffNetworkType_t mCurrentNetwork;
      std::set<int> mStates;
      std::list<CallStateMachine>& mCallList;
      bool matchesCriteria(CallStateMachine& call);
      void clearCriteria();
  };
  LocalLogBuffer& logBuffer;
  GlobalCallInfo globalInfo{};
  bool mApAssistMode;
  bool mSaMode;
  bool mRadioDataAvailable;
  bool mRadioVoiceAvailable;
  bool mNulBearerReasonAvailable;
  bool mReportPhysicalChannelConfig;
  bool mPhysicalChannelInfoValid;
  bool mReportRegistrationRejectCause;
  BringUpCapability mCallBringupCapability;
  std::function<void(int)> mCleanupKeepAliveCb = nullptr;
  std::vector<DataCallResult_t> radioDcList;
  std::vector<DataCallResult_t> iwlanDcList;
  std::vector<rildata::AvailableRadioDataService_t> availableRadioDataService;
  std::list<CallStateMachine> mCallInstances;
  bool toggleBearerUpdateToDataConnectionService;
  bool availableCallIds[MaxConcurrentDataCalls];
  std::shared_ptr<WDSModemEndPoint> wds_endpoint;
  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  std::vector<NasPhysChanInfo> mPhysicalChannelInfo;
  std::vector<PhysicalConfigStructInfo_t> mPhysicalConfigStructInfo;
  std::function<void(const std::vector<rildata::DataCallResult_t>&)> mDcListChangedCallback;
  NasActiveBand_t mActiveBandInfo;
  std::unordered_map<string, DeferredIntentToChange_t> deferredIntents;
  std::function<void(DeferredIntentToChange_t)> mProcessDeferredIntentCb = nullptr;
  bool physicalConfigStructInfoChanged(const std::vector<PhysicalConfigStructInfo_t>& configs);
  Filter mFilter{mCallInstances};
  bool validateSetupDataCallParams(std::shared_ptr<SetupDataCallRequestMessage> m);
  bool convertQmiDsdToAvailableRadioDataService(dsd_system_status_info_type_v01 sys,
                                                AvailableRadioDataService_t &radioDataService);
  void processEventSendResponse(CallEventTypeEnum event, std::shared_ptr<Message> msg, ResponseError_t errorCode);
};

} /* namespace rildata */

#endif
