/**
* Copyright (c) 2017-2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATAMODULE
#define DATAMODULE
#include "framework/PendingMessageList.h"
#include "module/IDataModule.h"
#include "DataCommon.h"
#include "AuthManager.h"
#include "LceHandler.h"
#include "ProfileHandler.h"
#include "NetworkServiceHandler.h"
#include "NetworkAvailabilityHandler.h"
#include "request/IWLANCapabilityHandshake.h"
#include "InitTracker.h"
#include "CallManager.h"
#include "PreferredDataStateMachine.h"
#include "VoiceCallModemEndPoint.h"
#include "modules/uim/UimCardStatusIndMsg.h"
#include "qtibus/IPCMessage.h"
#include "KeepAliveHandler.h"
#include "PDCModemEndPoint.h"
#ifdef USE_QCRIL_HAL
#include "modules/uim/qcril_uim_types.h"
#else
#include "interfaces/uim/qcril_uim_types.h"
#endif
#ifdef FEATURE_DATA_LQE
  #include "OTTModemEndPoint.h"
#endif /* FEATURE_DATA_LQE */
#include "MessageCommon.h"

#ifdef RIL_DATA_UTF
extern "C" {
  void dataQosInit(void* params, void (*logFn)(std::string));
  void dataQosCleanUp();
  void dataQosDeInit();
}
#endif
namespace rildata {

typedef void (*datactlInitFnPtr)(int, void (*logFn)(std::string));
typedef bool (*datactlEnableIWlanFnPtr)();
typedef bool (*datactlDisableIWlanFnPtr)();

struct dataQosInitParams {
  int cid;
  int muxId;
  int eptype;
  int epid;
  int slot_id;
};
typedef void (*dataQosInitFnPtr)(void*, void (*logFn)(std::string));
typedef void (*dataQosCleanUpFnPtr)();
typedef void (*dataQosDeInitFnPtr)();

struct datactlSymbols {
  datactlInitFnPtr datactlInit;
  datactlEnableIWlanFnPtr datactlEnableIWlan;
  datactlDisableIWlanFnPtr datactlDisableIWlan;
};

class DataModule : public IDataModule {
 public:
  DataModule();
  ~DataModule();
  PendingMessageList& getPendingMessageList();
  void dump(int fd);
  void flush();
  void init();
#ifdef QMI_RIL_UTF
  virtual void cleanup();
#endif

 private:
  #ifdef QMI_RIL_UTF
  static constexpr TimeKeeper::millisec TempDDS_SWITCH_REQUEST_TIMEOUT = 50;
  #else
  static constexpr TimeKeeper::millisec TempDDS_SWITCH_REQUEST_TIMEOUT = 1200;
  #endif
  LocalLogBuffer logBuffer{string("RIL"), 200};
  LocalLogBuffer networkAvailabilityLogBuffer{string("NAH"), 200};
  dsd_system_status_ind_msg_v01 mCachedSystemStatus = { };
  InitTracker mInitTracker;
  int32_t iwlanHandshakeMsgToken;
  bool mAuthServiceReady = false;
  PendingMessageList mMessageList;
  TimeKeeper::timer_id tempDDSSwitchRequestTimer;
  bool TempddsSwitchRequestPending = false;
  bool mLinkStatusReportEnabled = false;
  bool mLimitedIndReportEnabled = false;
  bool mInitCompleted = false;
  bool mRegistrationFailureCauseReport = false;
  bool mSimPresent = false;
  bool mIsPdcRefreshInProgress = false;

  std::set<int> attachSerialList;
  std::unique_ptr<AuthManager> auth_manager;
  std::unique_ptr<CallManager> call_manager;
  std::unique_ptr<ProfileHandler> profile_handler;
  std::unique_ptr<NetworkServiceHandler> network_service_handler;

#ifndef RIL_FOR_LOW_RAM
  std::shared_ptr<VoiceCallModemEndPoint> voiceCallEndPointSub0;
  std::shared_ptr<VoiceCallModemEndPoint> voiceCallEndPointSub1;
#endif

#if !defined(RIL_FOR_LOW_RAM) || defined(RIL_FOR_MDM_LE)
  std::unique_ptr<NetworkAvailabilityHandler> networkavailability_handler;
#endif

  std::function<void(const std::vector<rildata::DataCallResult_t>&)> dataCallListChangedCallback = nullptr;

  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  std::shared_ptr<WDSModemEndPoint> wds_endpoint;
  std::shared_ptr<AuthModemEndPoint> auth_endpoint;
  std::shared_ptr<PDCModemEndPoint> pdc_endpoint;
  #ifdef FEATURE_DATA_LQE
    std::shared_ptr<OTTModemEndPoint> ott_endpoint;
  #endif /* FEATURE_DATA_LQE */
  std::shared_ptr<PreferredDataInfo_t> preferred_data_state_info;

  #if (QCRIL_RIL_VERSION >= 15)
    std::shared_ptr<KeepAliveHandler> keep_alive;
  #endif

  datactlSymbols dcSymbols;
  bool loadDatactl();
  dataQosInitFnPtr mDataQosInit;
  dataQosCleanUpFnPtr mDataQosCleanup;
  dataQosDeInitFnPtr mDataQosDeInit;
  void* dataQosLibInstance;
  bool loadDataQos();
  void unloadDataQos();
  static void logFn(std::string);
  void processDeferredIntentToChange(DeferredIntentToChange_t deferIntent);

  LceHandler lceHandler;
  std::unique_ptr<PreferredDataStateMachine> preferred_data_sm;

   void handleQcrilInitMessage(std::shared_ptr<Message> msg);
   void broadcastReady();

#ifndef RIL_FOR_LOW_RAM
   void handleRilRequestSetupDataCallMessage(std::shared_ptr<Message> msg);
   void handleDataCallListMessage(std::shared_ptr<Message> msg);
   void handleRilRequestSetDataProfileMessage(std::shared_ptr<Message> msg);
   void handleRilRequestDeactivateDataCallMessage(std::shared_ptr<Message> msg);
   void handleEmbmsActivateDeactivateTmgiMessage(std::shared_ptr<Message> msg);
   void handleEmbmsActivateTmgiMessage(std::shared_ptr<Message> msg);
   void handleEmbmsContentDescUpdateMessage(std::shared_ptr<Message> msg);
   void handleEmbmsDeactivateTmgiMessage(std::shared_ptr<Message> msg);
   void handleEmbmsEnableDataReqMessage(std::shared_ptr<Message> msg);
   void handleEmbmsGetActiveTmgiMessage(std::shared_ptr<Message> msg);
   void handleEmbmsGetAvailTmgiMessage(std::shared_ptr<Message> msg);
   void handleEmbmsSendIntTmgiListMessage(std::shared_ptr<Message> msg);
   void handleDataEmbmsActiveMessage(std::shared_ptr<Message> msg);
   void handleVoiceIndMessage(std::shared_ptr<Message> msg);
   void handleTempDDSSwitchTimerExpired(void *resp);
   void handleVoiceCallOrigTimerExpired(void *resp);
   void handleVoiceCallOrigTimeoutMessage(std::shared_ptr<Message> msg);
   void handleQmiVoiceEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   void handleUimCardStatusIndMsg(std::shared_ptr<Message> msg);
   void handlePullLceDataMessage(std::shared_ptr<Message> msg);
   void handleStartLceMessage(std::shared_ptr<Message> msg);
   void handleStopLceMessage(std::shared_ptr<Message> msg);

   TimeKeeper::timer_id setTimeoutForMsg(std::shared_ptr<Message> msg, TimeKeeper::millisec maxTimeout);
   void deleteEntryInReqlist( std::shared_ptr<Message> msg );

#if (QCRIL_RIL_VERSION >= 15)
   void handleStartKeepaliveMessage(std::shared_ptr<Message> msg);
   void handleStopKeepaliveMessage(std::shared_ptr<Message> msg);
   keep_alive_start_request getApnName(const qcril_request_params_type *const params_ptr, qcril_request_return_type *const ret_ptr);
   void handleSetDataProfileMessage_v15(std::shared_ptr<Message> msg);
#endif
   void handleIWLANCapabilityHandshake(std::shared_ptr<Message> msg);
   void handleGetAllQualifiedNetworksMessage(std::shared_ptr<Message> msg);
   void handleDsdSystemStatusPerApn(std::shared_ptr<Message> msg);
   void handleIntentToChangeInd(std::shared_ptr<Message> msg);
   void handleGetIWlanDataCallListRequestMessage(std::shared_ptr<Message> msg);
   void handleGetIWlanDataRegistrationStateRequestMessage(std::shared_ptr<Message> msg);
   void handleSetApnPreferredSystemResult(std::shared_ptr<Message> msg);
   void handleNasPhysChanConfigReportingStatusMessage(std::shared_ptr<Message> msg);
   void handleNasPhysChanConfigMessage(std::shared_ptr<Message> msg);
   void handleHandoverInformationIndMessage(std::shared_ptr<Message> msg);
   void initializeIWLAN();
   void deinitializeIWLAN();
#endif //RIL_FOR_LOW_RAM

   void handleGetDataCallProfileMessage(std::shared_ptr<Message> msg);
   void handleGoDormantMessage(std::shared_ptr<Message> msg);
   void handleLastDataCallFailCauseMessage(std::shared_ptr<Message> msg);
   void handleProcessScreenStateChangeMessage(std::shared_ptr<Message> msg);
   void handleProcessStackSwitchMessage(std::shared_ptr<Message> msg);
   void handleSetApnInfoMessage(std::shared_ptr<Message> msg);
   void handleSetIsDataEnabledMessage(std::shared_ptr<Message> msg);
   void handleSetIsDataRoamingEnabledMessage(std::shared_ptr<Message> msg);
   void handleSetLteAttachProfileMessage(std::shared_ptr<Message> msg);
   void handleSetQualityMeasurementMessage(std::shared_ptr<Message> msg);
   void handleToggleDormancyIndMessage(std::shared_ptr<Message> msg);
   void handleToggleLimitedSysIndMessage(std::shared_ptr<Message> msg);
   void handleUpdateMtuMessage(std::shared_ptr<Message> msg);
   void handleGetDdsSubIdMessage(std::shared_ptr<Message> msg);
   void handleDataRequestDDSSwitchMessage(std::shared_ptr<Message> msg);
   void handleRilRequestSetInitialAttachApn(std::shared_ptr<Message> msg);
   void handleSetLteAttachPdnListActionResult(std::shared_ptr<Message> msg);
   void handleLinkPropertiesChangedMessage(std::shared_ptr<Message> msg);

#if (QCRIL_RIL_VERSION >= 15)
   void handleSetLteAttachProfileMessage_v15(std::shared_ptr<Message> msg);
   void handleSetDataProfileRequestMessage(std::shared_ptr<Message> msg);
   void handleRilRequestSetCarrierInfoImsiEncryptionMessage(std::shared_ptr<Message> msg);
   void handleQmiAuthServiceIndMessage(std::shared_ptr<Message> msg);
   void handleQmiAuthEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   void handleQmiDsdEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   void handleSetLinkCapFilterMessage(std::shared_ptr<Message> msg);
   void handleSetLinkCapRptCriteriaMessage(std::shared_ptr<Message> msg);
   void handleStartKeepAliveRequestMessage(std::shared_ptr<Message> msg);
   void handleStopKeepAliveRequestMessage(std::shared_ptr<Message> msg);
   void handleKeepAliveIndMessage(std::shared_ptr<Message> msg);
   void cleanupKeepAlive(int cid_val);

   #ifdef FEATURE_DATA_LQE
     void handleQmiOttEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   #endif /* FEATURE_DATA_LQE */
#endif //(QCRIL_RIL_VERSION >= 15)

   void handleNasSrvDomainPrefInd(std::shared_ptr<Message> msg);
   void handleDataNasPsAttachDetachResponse(std::shared_ptr<Message> m);
   void handleDetachAttachIndTimeoutMessage(std::shared_ptr<Message> m);
   void handleNasRequestDataShutdown(std::shared_ptr<Message> msg);
   void handleDataAllBearerTypeUpdate(std::shared_ptr<Message> msg);
   void handleDataBearerTypeUpdate(std::shared_ptr<Message> msg);
   void handleToggleBearerAllocationUpdate(std::shared_ptr<Message> msg);
   void handleGetBearerAllocation(std::shared_ptr<Message> msg);
   void handleGetAllBearerAllocations(std::shared_ptr<Message> msg);
   void handleQmiWdsEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   void handleDataConnectionStateChangedMessage(std::shared_ptr<Message> msg);
   void handleDsdSystemStatusInd(std::shared_ptr<Message> msg);
   void handleSetPreferredDataModem(std::shared_ptr<Message> msg);
   void handleCurrentDDSIndMessage(std::shared_ptr<Message> msg);
   void handleCurrentRoamingStatusChangedMessage(std::shared_ptr<Message> msg);
   void handleDDSSwitchResultIndMessage(std::shared_ptr<Message> msg);
   void handleRadioConfigClientConnectedMessage(std::shared_ptr<Message> msg);
   void handleDDSSwitchTimeoutMessage(std::shared_ptr<Message> msg);
   void handleDDSSwitchIPCMessage(std::shared_ptr<Message> msg);
   void handleIAInfoIPCMessage(std::shared_ptr<Message> msg);
   std::shared_ptr<IPCMessage> constructDDSSwitchIPCMessage(IPCIStream &is);
   std::shared_ptr<IPCMessage> constructIAInfoIPCMessage(IPCIStream &is);
   RIL_Errno map_internalerr_from_reqlist_new_to_ril_err(IxErrnoType error);
   void performDataModuleInitialization();

   void handleRilEventDataCallback(std::shared_ptr<Message> msg);
   void handleSetupDataCallRequestMessage(std::shared_ptr<Message> msg);
   void handleDeactivateDataCallRequestMessage(std::shared_ptr<Message> msg);
   void handleGetRadioDataCallListRequestMessage(std::shared_ptr<Message> msg);
   void handleSetRadioClientCapMessage(std::shared_ptr<Message> msg);
   void handleSetRadioClientCapUnSolMessage(std::shared_ptr<Message> msg);
   void handleDataCallTimerExpiredMessage(std::shared_ptr<Message> msg);
   void handleSetInitialAttachApn(std::shared_ptr<Message> msg);
   void handleSetInitialAttachCompleted(std::shared_ptr<Message> msg);
   void processInitialAttachRequestMessage(std::shared_ptr<Message> msg);
   void handleSetCarrierInfoImsiEncryptionMessage(std::shared_ptr<Message> msg);
   void handleRegistrationFailureReportingStatusMessage(std::shared_ptr<Message> msg);
   void handleStartLCERequestMessage(std::shared_ptr<Message> msg);
   void handleStopLCERequestMessage(std::shared_ptr<Message> msg);
   void handlePullLCEDataRequestMessage(std::shared_ptr<Message> msg);
   void handleDsiInitCompletedMessage(std::shared_ptr<Message> msg);
   void handleNasRfBandInfoIndMessage(std::shared_ptr<Message> msg);
   void handleNasRfBandInfoMessage(std::shared_ptr<Message> msg);
   void handleGetModemAttachParamsRetryMessage(std::shared_ptr<Message> msg);
   void handleGetQosDataRequestMessage(std::shared_ptr<Message> msg);
   void handleUimCardAppStatusIndMsg(std::shared_ptr<Message> msg);
   int retrieveUIMCardStatus( std::shared_ptr<RIL_UIM_CardStatus> ril_card_status, string &aid_buffer, RIL_UIM_AppType *app_type);
   int retrieveUIMAppStatusFromAppInfo(RIL_UIM_AppStatus *application, string &aid_buffer, RIL_UIM_AppType *app_type);
   void qcrilDataUimEventAppStatusUpdate ( const qcril_request_params_type *const params_ptr, qcril_request_return_type *const ret_ptr);
   void handleClientDisconnectedMessage(std::shared_ptr<Message> msg);
   void handleClientConnectedMessage(std::shared_ptr<Message> msg);
   void handlePDCRefreshIndication(std::shared_ptr<Message> msg);
   void handleQmiPDCEndpointStatusIndMessage(std::shared_ptr<Message> msg);
   void handleQosInitMessage(std::shared_ptr<Message> msg);
#ifdef RIL_FOR_MDM_LE
   void handleCaptureLogBufferMessage(std::shared_ptr<Message> msg);
   void handleIndicationRegistrationFilterMessage(std::shared_ptr<Message> msg);
#endif
};
void qcrilDataprocessMccMncInfo
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

DataModule& getDataModule();

} //namespace

#endif
