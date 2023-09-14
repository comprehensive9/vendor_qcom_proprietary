/******************************************************************************
#  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <memory>
#include <mutex>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>
#include <Status.hpp>
#include <RequestManager.hpp>

enum class RilInstance {
    FIRST = 0,
    SECOND = 1,
    MIN = FIRST,
    MAX = SECOND,
};

// TODO: Define response and indication callbacks in different namespaces
typedef std::function<void()> VoidIndicationHandler;
typedef std::function<void(const char *)> CharStarIndicationHandler;
typedef std::function<void(int)> IntIndicationHandler;
typedef std::function<void(RIL_Errno)> DialCallback;
typedef std::function<void(RIL_Errno, const RIL_CellInfo_v12[], size_t)> CellInfoCallback;
typedef std::function<void(const RIL_SignalStrength&)> SignalStrengthIndicationHandler;
typedef std::function<void(const RIL_CellInfo_v12[], size_t)> CellInfoIndicationHandler;
typedef std::function<void(const RIL_CDMA_SMS_Message&)> CdmaNewSmsIndicationHandler;

typedef std::function<void(RIL_Errno)> VoidResponseHandler;
typedef std::function<void(RIL_Errno, const char *, size_t)> CharStarSizeResponseHandler;
typedef std::function<void(RIL_Errno, RIL_VoiceRegistrationStateResponse)> GetVoiceRegCallback;
typedef std::function<void(RIL_Errno, RIL_DataRegistrationStateResponse)> GetDataRegCallback;
typedef std::function<void(RIL_Errno)> SetNetworkTypeCallback;
typedef std::function<void(RIL_Errno, int)> GetNetworkTypeCallback;
typedef std::function<void(RIL_Errno, const size_t numCalls, const RIL_Call **)> GetCurrentCallsCallback;
typedef std::function<void(RIL_Errno)> AnswerCallback;
typedef std::function<void(RIL_Errno)> ConferenceCallback;
typedef std::function<void(RIL_Errno)> SwitchWaitingOrHoldingAndActiveCallback;
typedef std::function<void(RIL_Errno)> UdubCallback;
typedef std::function<void(RIL_Errno)> HangupWaitingOrBackgroundCallback;
typedef std::function<void(RIL_Errno)> HangupForegroundResumeBackgroundCallback;
typedef std::function<void(RIL_Errno)> SeparateConnectionCallback;
typedef std::function<void(RIL_Errno, const int *)> QueryAvailableBandModeCallback;
typedef std::function<void(RIL_Errno, const RIL_SignalStrength *)> GetSignalStrengthCallback;
typedef std::function<void(RIL_Errno)> RadioPowerCallback;
typedef std::function<void(RIL_Errno)> AllowDataCallback;
typedef std::function<void(RIL_Errno)> EnableSimCallback;
typedef std::function<void(RIL_Errno, int)> GetEnableSimStatusCallback;
typedef std::function<void(RIL_Errno)> SetLocationUpdatesCallback;
typedef std::function<void(RIL_Errno, RIL_ActivityStatsInfo)> GetActivityInfoCallback;
typedef std::function<void(RIL_Errno, int)> VoiceRadioTechCallback;
typedef std::function<void(RIL_Errno, const char **)> DeviceIdentityCallback;
typedef std::function<void(RIL_Errno, const char **)> OperatorNameCallback;
typedef std::function<void(RIL_Errno, int)> QueryNetworkSelectionModeCallback;
typedef std::function<void(RIL_Errno)> SetNetworkSelectionAutomaticCallback;
typedef std::function<void(RIL_Errno)> SetNetworkSelectionManualCallback;
typedef std::function<void(RIL_Errno, const char *)> BasebandVersionCallback;
typedef std::function<void(RIL_Errno)> SetRadioCapabilityCallback;
typedef std::function<void(RIL_Errno, const RIL_RadioCapability&)> GetRadioCapabilityCallback;
typedef std::function<void(const RIL_RadioCapability&)> RadioCapabilityIndHandler;
typedef std::function<void(RIL_Errno)> ShutDownCallback;
typedef std::function<void(RIL_Errno)> ExitEmergencyCbModeCallback;
typedef std::function<void(RIL_Errno)> SetBandModeCallback;
typedef std::function<void(RIL_Errno)> HangupCallback;
typedef std::function<void(RIL_Errno)> SendUssdCallback;
typedef std::function<void(RIL_Errno)> CancelUssdCallback;
typedef std::function<void(RIL_Errno)> DtmfCallback;
typedef std::function<void(RIL_Errno)> DtmfStartCallback;
typedef std::function<void(RIL_Errno)> DtmfStopCallback;
typedef std::function<void(RIL_Errno, const RIL_LastCallFailCauseInfo&)> LastCallFailCauseCallback;

typedef std::function<void(RIL_Errno, const RIL_SMS_Response&)> SendSmsCallback;
typedef std::function<void(RIL_Errno)> AckSmsCallback;
typedef std::function<void(RIL_Errno, const char* addr)> GetSmscAddressCallback;
typedef std::function<void(RIL_Errno)> SetSmscAddressCallback;
typedef std::function<void(RIL_Errno, const size_t numCallFwdInfos, const RIL_CallForwardInfo**)>
    QueryCallForwardStatusCallback;
typedef std::function<void(RIL_Errno)> SetCallForwardStatusCallback;
typedef std::function<void(RIL_Errno, const int enabled, const int serviceClass)>
    QueryCallWaitingCallback;
typedef std::function<void(RIL_Errno)> SetCallCallWaitingCallback;
typedef std::function<void(RIL_Errno)> ChangeBarringPasswordCallback;
typedef std::function<void(RIL_Errno, const int status)> QueryClipCallback;
typedef std::function<void(RIL_Errno)> SetSuppSvcNotificationCallback;
typedef std::function<void(const uint8_t[], size_t)> Incoming3GppSmsIndicationHandler;
typedef std::function<void(const bool)> EnableSimStatusIndicationHandler;
typedef std::function<void(const RIL_CdmaSubscriptionSource)> CdmaSubscriptionSourceChangedIndicationHandler;
typedef std::function<void(const std::vector<uint8_t>&)> BroadcastSmsIndicationHandler;
typedef std::function<void(const std::string&)> ModemRestartIndicationHandler;
typedef std::function<void(int)> RilServerReadyIndicationHandler;
typedef std::function<void(const RIL_NetworkScanResult&)> NetworkScanIndicationHandler;
typedef std::function<void(const RIL_EmergencyList&)> EmergencyListIndicationHandler;
typedef std::function<void(RIL_Errno, int32_t)> WriteSmsToSimCallback;
typedef std::function<void(RIL_Errno)> DeleteSmsOnSimCallback;
typedef std::function<void(RIL_Errno)> ReportSmsMemoryStatusCallback;
typedef std::function<void(RIL_Errno)> SetCdmaSubscriptionSourceCallback;
typedef std::function<void(RIL_Errno, RIL_CdmaSubscriptionSource)> GetCdmaSubscriptionSourceCallback;
typedef std::function<void(RIL_Errno)> SetCdmaRoamingPreferenceCallback;
typedef std::function<void(RIL_Errno, RIL_CdmaRoamingPreference)> GetCdmaRoamingPreferenceCallback;
typedef std::function<void(RIL_Errno, const RIL_GSM_BroadcastSmsConfigInfo *, size_t)> GsmGetBroadcastSmsConfigCallback;
typedef std::function<void(RIL_Errno)> SetSignalStrengthReportingCriteriaCallback;
typedef std::function<void(RIL_Errno, const char *, size_t)> OemhookRawCallback;
typedef std::function<void(const char *, size_t)> OemhookIndicationHandler;

// IMS Requests
typedef std::function<void(RIL_Errno, const RIL_IMS_Registration&)> ImsGetRegistrationStateCallback;
typedef std::function<void(RIL_Errno, const size_t, const RIL_IMS_ServiceStatusInfo**)>
    ImsQueryServiceStatusCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_SubConfigInfo&)> ImsGetImsSubConfigCallback;
typedef std::function<void(RIL_Errno)> ImsRegistrationChangeCallback;
typedef std::function<void(RIL_Errno)> ImsSetServiceStatusCallback;
typedef std::function<void(RIL_Errno)> ImsDialCallback;
typedef std::function<void(RIL_Errno)> ImsAnswerCallback;
typedef std::function<void(RIL_Errno)> ImsHangupCallback;
typedef std::function<void(RIL_Errno)> ImsModifyCallInitiateCallback;
typedef std::function<void(RIL_Errno)> ImsModifyCallConfirmCallback;
typedef std::function<void(RIL_Errno)> ImsCancelModifyCallCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsAddParticipantCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_ConfigInfo&)> ImsSetImsConfigCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_ConfigInfo&)> ImsGetImsConfigCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_QueryCallForwardStatusInfo&)>
    ImsQueryCallForwardStatusCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_SetCallForwardStatusInfo&)>
    ImsSetCallForwardStatusCallback;
typedef std::function<void(RIL_Errno)> ImsExplicitCallTransferCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_ClirInfo&)> ImsGetClirCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_ClipInfo&)> ImsQueryClipCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_ServiceClassStatus&)>
    ImsSetSuppSvcNotificationCallback;
typedef std::function<void(RIL_Errno)> ImsSetClirCallback;
typedef std::function<void(RIL_Errno)> ImsDeflectCallCallback;
typedef std::function<void(RIL_Errno)> ImsSendUiTtyModeCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_SuppSvcResponse&)> ImsSuppSvcStatusCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_ColrInfo&)> ImsGetColrCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsSetColrCallback;
typedef std::function<void(RIL_Errno)> ImsRegisterMultiIdentityLinesCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_QueryVirtualLineInfoResponse&)>
        ImsQueryVirtualLineInfoCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_SendSmsResponse&)> ImsSendSmsCallback;
typedef std::function<void(RIL_Errno)> ImsAckSmsCallback;
typedef std::function<void(RIL_Errno, RIL_IMS_SmsFormat)> ImsGetSmsFormatCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsSetCallWaitingCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_QueryCallWaitingResponse&)> ImsQueryCallWaitingCallback;

typedef std::function<void(RIL_Errno)> AckCdmaSmsCallback;
typedef std::function<void(RIL_Errno, const RIL_CDMA_BroadcastSmsConfigInfo[], size_t)> CdmaGetBroadcastSmsConfigCallback;
typedef std::function<void(RIL_Errno)> CdmaSetBroadcastSmsConfigCallback;
typedef std::function<void(RIL_Errno)> CdmaSmsBroadcastActivationCallback;
typedef std::function<void(RIL_Errno, int32_t recordNumber)> WriteCdmaSmsToSimCallback;
typedef std::function<void(RIL_Errno)> DeleteCdmaSmsOnRuimCallback;
typedef std::function<void(const int&)> RingBackToneIndicationHandler;
typedef std::function<void()> ImsNetworkStateChangeIndicationHandler;
typedef std::function<void()> VoiceNetworkStateIndicationHandler;
typedef std::function<void(int)> VoiceRadioTechIndicationHandler;
typedef std::function<void(const char*)> NitzTimeIndicationHandler;
typedef std::function<void(RIL_RadioState)> RadioStateIndicationHandler;
typedef std::function<void(const int&)> RestrictedStateChangedIndicationHandler;
typedef std::function<void(const RIL_CDMA_SignalInfoRecord *)> CallRingIndicationHandler;
typedef std::function<void(const char& mode, const std::string& message)> OnUssdIndicationHandler;
typedef std::function<void(const RIL_SuppSvcNotification& suppSvc)>
    SuppSvcNotificationIndicationHandler;
typedef std::function<void(const RIL_CDMA_CallWaiting_v6& callWaitingRecord)>
    CdmaCallWaitingIndicationHandler;
typedef std::function<void(const RIL_CDMA_OTA_ProvisionStatus& status)>
    CdmaOtaProvisionStatusIndicationHandler;
typedef std::function<void(const RIL_CDMA_InformationRecords& records)> CdmaInfoRecIndicationHandler;
typedef std::function<void(const RIL_StkCcUnsolSsResponse& ss)> OnSupplementaryServiceIndicationHandler;
typedef std::function<void(const RIL_SrvccState& state)> SrvccStatusIndicationHandler;
typedef std::function<void(RIL_Errno)> ExplicitCallTransferCallback;
typedef std::function<void(RIL_Errno)> SetMuteCallback;
typedef std::function<void(RIL_Errno, int)> GetMuteCallback;
typedef std::function<void(RIL_Errno)> SetClirCallback;
typedef std::function<void(RIL_Errno, int, int)> GetClirCallback;
typedef std::function<void(RIL_Errno)> SendRttCallback;
typedef std::function<void(RIL_Errno)> GeolocationInfoCallback;
typedef std::function<void(RIL_Errno)> SetUnsolCellInfoListRateCallback;
typedef std::function<void()> CallStateChangeIndicationHandler;
typedef std::function<void(RIL_EmergencyCbMode)> EmergencyCbModeIndHandler;
typedef std::function<void(int)> UiccSubStatusChangedHandler;
typedef std::function<void(int)> CdmaPrlChangedHandler;
typedef std::function<void(const RIL_IMS_Registration&)> ImsRegistrationStateIndicationHandler;
typedef std::function<void(const size_t, const RIL_IMS_ServiceStatusInfo**)>
    ImsServiceStatusIndicationHandler;
typedef std::function<void(const RIL_IMS_SubConfigInfo&)> ImsSubConfigChangeIndicationHandler;
typedef std::function<void(const std::string&)> ImsRttMessageIndicationHandler;
typedef std::function<void(double, double)> ImsGeolocationIndicationHandler;
typedef std::function<void(const RIL_IMS_VowifiCallQuality &)> ImsVowifiQualityIndicationHandler;
typedef std::function<void()> ImsCallRingIndicationHandler;
typedef std::function<void(const RIL_IMS_ToneOperation&)> ImsRingBackToneIndicationHandler;
typedef std::function<void(RIL_IMS_CallInfo*, size_t count)> ImsCallStateChangedIndicationHandler;
typedef std::function<void()> ImsEnterEcbmIndicationHandler;
typedef std::function<void()> ImsExitEcbmIndicationHandler;
typedef std::function<void(const RIL_IMS_CallModifyInfo&)> ImsModifyCallIndicationHandler;
typedef std::function<void(const RIL_IMS_MessageWaitingInfo&)> ImsMessageWaitingIndicationHandler;
typedef std::function<void(const RIL_IMS_SuppSvcNotification&)> ImsSuppSvcNotificationIndicationHandler;
typedef std::function<void(const RIL_IMS_StkCcUnsolSsResponse&)> ImsOnSsIndicationHandler;
typedef std::function<void(const bool)> ImsVopsChangedIndicationHandler;
typedef std::function<void(const RIL_IMS_ParticipantStatusInfo&)>
    ImsParticipantStatusInfoIndicationHandler;
typedef std::function<void(const RIL_IMS_RegistrationBlockStatus&)>
    ImsRegistrationBlockStatusIndicationHandler;
typedef std::function<void(const uint8_t&)> ImsWfcRoamingModeConfigSupportIndicationHandler;
typedef std::function<void(const RIL_IMS_UssdInfo&)> ImsUssdFailedIndicationHandler;
typedef std::function<void(const RIL_IMS_VoiceInfoType&)> ImsVoiceInfoIndicationHandler;
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsRequestConferenceCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsRequestHoldCallback;
typedef std::function<void(RIL_Errno, const RIL_IMS_SipErrorInfo&)> ImsRequestResumeCallback;
typedef std::function<void(RIL_Errno)> ImsExitECBMCallback;
typedef std::function<void(const RIL_IMS_IncomingSms&)> ImsIncomingSmsHandler;
typedef std::function<void(const RIL_IMS_SmsStatusReport&)> ImsSmsStatusReportHandler;
typedef std::function<void(const std::vector<uint8_t>&)> ImsViceInfoHandler;
typedef std::function<void(RIL_IMS_TtyModeType)> ImsTtyNotificationHandler;
typedef std::function<void(const RIL_IMS_HandoverInfo&)> ImsHandoverIndicationHandler;
typedef std::function<void(const RIL_IMS_RefreshConferenceInfo&)>
    ImsRefreshConferenceInfoIndicationHandler;
typedef std::function<void(const RIL_IMS_MultiIdentityLineInfo*, size_t)>
    MultiIdentityRegistrationStatusHandler;
typedef std::function<void(const RIL_IMS_GeoLocationDataStatus&)>
    RetrievingGeoLocationDataStatusHandler;

typedef std::function<void(RIL_Errno, const Status, const RIL_Data_Call_Response_v11*)> SetupDataCallCallback;
typedef std::function<void(RIL_Errno)> DeactivateDataCallCallback;
typedef std::function<void(const RIL_Data_Call_Response_v11[], size_t)> DataCallListChangedIndicationHandler;
typedef std::function<void(RIL_Errno)> SetInitialAttachApnCallback ;
typedef std::function<void(RIL_Errno)> SetDataProfileCallback ;
typedef std::function<void(const RIL_Errno, const RIL_Data_Call_Response_v11*, const size_t)> GetDataCallListCallback;
typedef std::function<void(const RIL_Errno)> GetRilDataDumpCallback;
typedef std::function<void(RIL_Errno, const RIL_LceStatusInfo* )> StartLceDataCallback;
typedef std::function<void(RIL_Errno, const RIL_LceStatusInfo* )> StopLceDataCallback;
typedef std::function<void(RIL_Errno, const RIL_LceDataInfo* )> PullLceDataCallback;
typedef std::function<void(RIL_Errno, const int*)> SetLinkCapFilterCallback;
typedef std::function<void(const RIL_PCO_Data)> RILPCODataChangeIndicationHandler;
typedef std::function<void(const RIL_LinkCapacityEstimate)> RILLCEDataChangeIndicationHandler;
typedef std::function<void()> CarrierInfoForImsiHandler;
typedef std::function<void(const Status, const uint32_t, const RIL_KeepaliveStatusCode)> RILDataRadioKeepAliveHandler;
typedef std::function<void(const Status, const std::vector<RIL_PhysicalChannelConfig> )> PhysicalConfigStructHandlerHandler;
typedef std::function<void(const Status, const five_g_icon_type )> RILDataNrIconChangeIndicationHandler;
typedef std::function<void(RIL_Errno, const RIL_LinkCapCriteriaResult*)> SetLinkCapRptCriteriaCallback;
typedef std::function<void(RIL_Errno, const Status, const five_g_icon_type*)> GetDataNrIconTypeCallback;
typedef std::function<void(RIL_Errno, const Status, const RIL_ResponseError*)> SetPreferredDataModemCallback;
typedef std::function<void(RIL_Errno, const Status, const RIL_ResponseError*, const uint32_t*, const RIL_KeepaliveStatus_t*)> StartKeepAliveCallback;
typedef std::function<void(RIL_Errno, const Status, const RIL_ResponseError*)> StopKeepAliveCallback;
typedef std::function<void(RIL_Errno, const Status, const int*)> RegisterDataUnsolIndicationCallback;
typedef std::function<void(const RIL_Errno, const Status, const RIL_Errno*)> CarrierInfoForImsiEncryptionCallback;

typedef std::function<void(const int&)> NewSmsOnSimIndicationHandler;
typedef std::function<void()> SimSmsStorageFullIndicationHandler;
typedef std::function<void(const char*)> NewSmsStatusReportIndicationHandler;
typedef std::function<void()> CdmaSmsRuimStorageFullIndicationHandler;
typedef std::function<void(RIL_Errno, const std::vector<RIL_OperatorInfo>&)> QueryAvailableNetworksCallback;

/* QCRIL_UIM Request Message Callbacks */

typedef std::function<void(RIL_Errno, const char *)> GetImsiReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> IOReqCallback;
typedef std::function<void(RIL_Errno, const RIL_CardStatus_v6*)> GetSimStatusReqCallback;
typedef std::function<void(RIL_Errno, const int *, size_t len)> OpenChannelReqCallback;
typedef std::function<void(RIL_Errno)> CloseChannelReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> TransmitApduBasicReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> TransmitApduChannelReqCallback;
typedef std::function<void(RIL_Errno, int *)> ChangePinReqCallback;
typedef std::function<void(RIL_Errno, int *)> ChangePin2ReqCallback;
typedef std::function<void(RIL_Errno, int *)> EnterPukReqCallback;
typedef std::function<void(RIL_Errno, int *)> EnterPuk2ReqCallback;
typedef std::function<void(RIL_Errno, int *)> EnterPinReqCallback;
typedef std::function<void(RIL_Errno, int *)> EnterPin2ReqCallback;
typedef std::function<void(RIL_Errno, int32_t *)> QueryFacilityLockReqCallback;
typedef std::function<void(RIL_Errno, int *)> SetFacilityLockReqCallback;
typedef std::function<void(RIL_Errno, const char *)> IsimAuthenticationReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> StkSendEnvelopeWithStatusReqCallback;
typedef std::function<void(RIL_Errno, const RIL_SIM_IO_Response *)> AuthenticationReqCallback;
typedef std::function<void(RIL_Errno)> SetTtyModeCallBack;
typedef std::function<void(RIL_Errno, const int&)> GetTtyModeCallBack;
typedef std::function<void(RIL_Errno)> SetCdmaVoicePrefModeCallBack;
typedef std::function<void(RIL_Errno, const int&)> GetCdmaVoicePrefModeCallBack;
typedef std::function<void(RIL_Errno)> SendCdmaFlashCallBack;
typedef std::function<void(RIL_Errno)> SendCdmaBurstDtmfCallBack;
typedef std::function<void(RIL_Errno, const bool&, const RIL_RadioTechnologyFamily&)> GetImsRegStateCallBack;

typedef std::function<void(RIL_Errno, const char*[5])> GetCdmaSubInfoCallback;
typedef std::function<void(RIL_Errno, RIL_AdnCountInfo *)> GetAdnRecordCallback;
typedef std::function<void(const RIL_SimRefreshResponse_v7 *)> SimRefreshIndicationHandler;

/* RilApiSession maintains state associated with an API session
 * opened by a client of RIL. */
class RilApiSession {
    public:
        RilApiSession(const std::string& socketPath);
        RilApiSession(const std::string& ipAddress, const unsigned short tcpPort);
        ~RilApiSession();
        Status initialize(const RequestManager::ErrorCallback& errorCallback);
        Status reinitialize(const RequestManager::ErrorCallback& errorCallback);
        Status deinitialize();
        Status createUnixSocket();
        Status createTcpIpSocket();
        Status registerIndicationHandler(int32_t indicationId, const RequestManager::GenericIndicationHandler& indicationHandler);

        Status dial(const RIL_Dial& dialParams, const DialCallback& cb);
        Status getCellInfo(const CellInfoCallback& cb);
        Status registerSignalStrengthIndicationHandler(const SignalStrengthIndicationHandler& indicationHandler);
        Status registerCellInfoIndicationHandler(const CellInfoIndicationHandler& indicationHandler);
        Status registerCdmaNewSmsIndicationHandler(const CdmaNewSmsIndicationHandler& indicationHandler);

        Status setPreferredNetworkType(int nwType, const SetNetworkTypeCallback &cb);
        Status getPreferredNetworkType(const GetNetworkTypeCallback &cb);
        Status getCurrentCalls(const GetCurrentCallsCallback& cb);
        Status answer(const AnswerCallback& cb);
        Status conference(const ConferenceCallback& cb);
        Status switchWaitingOrHoldingAndActive(const SwitchWaitingOrHoldingAndActiveCallback& cb);
        Status udub(const UdubCallback& cb);
        Status hangupWaitingOrBackground(const HangupWaitingOrBackgroundCallback& cb);
        Status hangupForegroundResumeBackground(const HangupForegroundResumeBackgroundCallback& cb);
        Status separateConnection(int callId, const SeparateConnectionCallback& cb);
        Status setRadioCapability(const RIL_RadioCapability &radioCap, const SetRadioCapabilityCallback& cb);
        Status getRadioCapability(const GetRadioCapabilityCallback& cb);
        Status shutDown(const ShutDownCallback& cb);
        Status exitEmergencyCbMode(const ExitEmergencyCbModeCallback& cb);
        Status setBandMode(const int& bandMode, const SetBandModeCallback& cb);
        Status queryAvailableBandMode(const QueryAvailableBandModeCallback& cb);
        Status getSignalStrength(const GetSignalStrengthCallback& cb);
        Status VoiceRadioTech(const VoiceRadioTechCallback &cb);
        Status getVoiceRegStatus(const GetVoiceRegCallback& cb);
        Status getDataRegStatus(const GetDataRegCallback& cb);
        Status radioPower(const bool& on, const RadioPowerCallback& cb);
        Status enableSim(const bool& on, const EnableSimCallback& cb);
        Status getEnableSimStatus(const GetEnableSimStatusCallback& cb);
        Status allowData(const bool& on, const AllowDataCallback& cb);
        Status setLocationUpdates(const bool& on, const SetLocationUpdatesCallback& cb);
        Status getActivityInfo(const GetActivityInfoCallback& cb);
        Status deviceIdentity(const DeviceIdentityCallback &cb);
        Status operatorName(const OperatorNameCallback& cb);
        Status setIndicationFilter(int32_t filter, const VoidResponseHandler &cb);
        Status QueryNetworkSelectionMode(const QueryNetworkSelectionModeCallback &cb);
        Status setNetworkSelectionAutomatic(const SetNetworkSelectionAutomaticCallback &cb);
        Status setNetworkSelectionManual(std::string mcc, std::string mnc, const SetNetworkSelectionAutomaticCallback &cb);
        Status basebandVersion(const BasebandVersionCallback &cb);
        Status hangup(const int& connid, const HangupCallback& cb);
        Status sendUssd(const std::string& ussd, const SendUssdCallback& cb);
        Status cancelUssd(const CancelUssdCallback& cb);
        Status dtmf(const char& dtmf, const DtmfCallback& cb);
        Status dtmfStart(const char& dtmf, const DtmfStartCallback& cb);
        Status dtmfStop(const DtmfStopCallback& cb);
        Status lastCallFailCause(const LastCallFailCauseCallback& cb);
        Status explicitCallTransfer(const ExplicitCallTransferCallback& cb);
        Status setMute(const bool& mute, const SetMuteCallback& cb);
        Status getMute(const GetMuteCallback& cb);
        Status setClir(const int* clir, const SetClirCallback& cb);
        Status imsSendRttMessage(const char* msg, size_t len, const SendRttCallback& cb);
        Status imsSendGeolocationInfo( const RIL_IMS_GeolocationInfo& geolocationInfo,
                                       const GeolocationInfoCallback& cb);
        Status registerImsRttMessageIndicationHandler(const
                    ImsRttMessageIndicationHandler& indicationHandler);
        Status registerImsGeolocationIndicationHandler(const
                    ImsGeolocationIndicationHandler& indicationHandler);
        Status registerImsUnsolRetrievingGeoLocationDataStatus(
            const RetrievingGeoLocationDataStatusHandler& indicationHandler);
        Status registerImsVowifiQualityIndicationHandler(const
                    ImsVowifiQualityIndicationHandler& indicationHandler);
        Status getClir(const GetClirCallback& cb);
        Status queryCallForwardStatus(const RIL_CallForwardInfo& callFwdInfo,
                                      const QueryCallForwardStatusCallback& cb);
        Status setCallForward(const RIL_CallForwardInfo& callFwdInfo,
                              const SetCallForwardStatusCallback& cb);
        Status queryCallWaiting(const int& serviceClass, const QueryCallWaitingCallback& cb);
        Status setCallWaiting(const int& enabled, const int& serviceClass,
                              const SetCallCallWaitingCallback& cb);
        Status changeBarringPassword(const std::string& facility, const std::string& oldPassword,
                                     const std::string& newPassword,
                                     const ChangeBarringPasswordCallback& cb);
        Status queryClip(const QueryClipCallback& cb);
        Status setSuppSvcNotification(const int& enabled, const SetSuppSvcNotificationCallback& cb);
        Status setUnsolCellInfoListRate(const int32_t& mRate, const SetUnsolCellInfoListRateCallback& cb);
        Status sendSms(const std::string& message, const std::string& smscAddress, bool expectMore, const SendSmsCallback& cb);
        Status ackSms(const RIL_GsmSmsAck ack, const AckSmsCallback& cb);
        Status getSmscAddress(const GetSmscAddressCallback& cb);
        Status setSmscAddress(const std::string& addr, const SetSmscAddressCallback& cb);
        Status sendCdmaSms(const RIL_CDMA_SMS_Message& smsParams, const SendSmsCallback& cb);
        Status ackCdmaSms(const RIL_CDMA_SMS_Ack& ackParams, const AckCdmaSmsCallback& cb);
        Status getCdmaSmsBroadcastConfig(const CdmaGetBroadcastSmsConfigCallback& cb);
        Status setCdmaSmsBroadcastConfig(const RIL_CDMA_BroadcastSmsConfigInfo cfgParams[], size_t size, const CdmaSetBroadcastSmsConfigCallback& cb);
        Status setCdmaSmsBroadcastActivation(const int32_t value, const CdmaSmsBroadcastActivationCallback& cb);
        Status writeCdmaSmsToRuim(const RIL_CDMA_SMS_WriteArgs arg, const WriteCdmaSmsToSimCallback& cb);
        Status deleteCdmaSmsOnRuim(int32_t index, const DeleteCdmaSmsOnRuimCallback& cb);
        Status getCdmaSubscription(const GetCdmaSubInfoCallback& cb);

        Status writeSmsToSim(const std::string& pdu, const std::string& smscAddress, const int status, const WriteSmsToSimCallback& cb);
        Status deleteSmsOnSim(int32_t recordIndex, const DeleteSmsOnSimCallback& cb);
        Status reportSmsMemoryStatus(bool storageSpaceAvailable, const ReportSmsMemoryStatusCallback& cb);
        Status setCdmaSubscriptionSource(RIL_CdmaSubscriptionSource subscriptionSource, const SetCdmaSubscriptionSourceCallback& cb);
        Status getCdmaSubscriptionSource(const GetCdmaSubscriptionSourceCallback& cb);
        Status setCdmaRoamingPreference(RIL_CdmaRoamingPreference roamingPref, const SetCdmaRoamingPreferenceCallback& cb);
        Status getCdmaRoamingPreference(const GetCdmaRoamingPreferenceCallback& cb);
        Status setSignalStrengthReportingCriteria(const RIL_SignalStrengthReportingCriteria& criteria,
                const SetSignalStrengthReportingCriteriaCallback& cb);
        Status oemhookRaw(const char *data, size_t dataLen, const OemhookRawCallback& cb);

        Status registerUnsolCallStateChangeIndicationHandler(const CallStateChangeIndicationHandler& indicationHandler);
        Status registerRingBackToneIndicationHandler(const RingBackToneIndicationHandler& indicationHandler);
        Status registerImsNetworkStateChangeIndicationHandler(const ImsNetworkStateChangeIndicationHandler& indicationHandler);
        Status registerVoiceNetworkStateIndicationHandler(const VoiceNetworkStateIndicationHandler& indicationHandler);
        Status registerVoiceRadioTechIndicationHandler(const VoiceRadioTechIndicationHandler& indicationHandler);
        Status registerNitzTimeIndicationHandler(const NitzTimeIndicationHandler& indicationHandler);
        Status registerRadioStateIndicationHandler(const RadioStateIndicationHandler& indicationHandler);
        Status registerRestrictedStateChangedIndicationHandler(
                const RestrictedStateChangedIndicationHandler& indicationHandler);
        Status registerCallRingIndicationHandler(const CallRingIndicationHandler& indicationHandler);
        Status registerEmergencyCbModeIndHandler(const EmergencyCbModeIndHandler& indicationHandler);
        Status registerUiccSubStatusIndHandler(const UiccSubStatusChangedHandler& indicationHandler);
        Status registerCdmaPrlChangedIndHandler(const CdmaPrlChangedHandler& indicationHandler);
        Status registerRadioCapabilityChangedIndHandler(
            const RadioCapabilityIndHandler& indicationHandler);
        /**
        *  @brief Registering Indication handler for Session End indications from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void()> VoidIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerStkUnsolSessionEnd(
                const VoidIndicationHandler &indicationHandler);
        /**
        *  @brief Registering Indication handler for Proactive command indications from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(const char *)> CharStarIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerStkUnsolProactiveCommand(
                const CharStarIndicationHandler &indicationHandler);
        /**
        *  @brief Registering Indication handler for EventNotifications from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(const char *)> CharStarIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerStkEventNotify(
                const CharStarIndicationHandler &indicationHandler);
        /**
        *  @brief Registering Indication handler for Call Setup notification from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(int)> IntIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerStkCallSetup(
                const IntIndicationHandler &indicationHandler);
        Status registerOnUssdIndicationHandler(const OnUssdIndicationHandler& indicationHandler);
        Status registerSuppSvcNotificationIndicationHandler(
            const SuppSvcNotificationIndicationHandler& indicationHandler);
        Status registerCdmaCallWaitingIndicationHandler(
            const CdmaCallWaitingIndicationHandler& indicationHandler);
        Status registerCdmaOtaProvisionStatusIndicationHandler(
            const CdmaOtaProvisionStatusIndicationHandler& indicationHandler);
        Status registerCdmaInfoRecIndicationHandler(
            const CdmaInfoRecIndicationHandler& indicationHandler);
        Status registerOnSupplementaryServiceIndicationHandler(
            const OnSupplementaryServiceIndicationHandler& indicationHandler);
        Status registerSrvccStatusIndicationHandler(
            const SrvccStatusIndicationHandler& indicationHandler);
        Status registerIncoming3GppSmsIndicationHandler(
            const Incoming3GppSmsIndicationHandler& indicationHandler);
        Status registerEnableSimStatusIndicationHandler(
            const EnableSimStatusIndicationHandler& indicationHandler);
        Status registerCdmaSubscriptionSourceChangedIndicationHandler(
            const CdmaSubscriptionSourceChangedIndicationHandler& indicationHandler);
        Status registerBroadcastSmsIndicationHandler(
            const BroadcastSmsIndicationHandler& indicationHandler);
        Status registerModemRestartIndicationHandler(
            const ModemRestartIndicationHandler& indicationHandler);
        Status registerRilServerReadyIndicationHandler(
            const RilServerReadyIndicationHandler& indicationHandler);
        Status registerOemhookIndicationHandler(
            const OemhookIndicationHandler& indicationHandler);
        /**
         * @brief
         * SetupDataCall API is used to bring up data call.
         * @note This is not the preferred API for use. Use the other SetupDataCall API provided in this file.
         *
         * @param accessNetwork This is an enum of type #RIL_RadioAccessNetworks which specifies the preferred network access type.
         * @param dataProfile This is a struct of type RIL_DataProfileInfo_IRadio_1_4 which specifies the preferred data profile.
         * @param roamingAllowed This is a boolean variable which specifies if roaming is allowed or not.
         * @param reason This is an enum of type #RIL_RadioDataRequestReasons which specifies the reason for data call bringup request.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for Setupdatacall.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_Data_Call_Response_v11 *)> SetupDataCallCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * Data call bring up is SUCCESS/FAILURE would be inidcated in the setupdatacall Callback.
         *
         *
        */
        Status setupDataCall(const RIL_RadioAccessNetworks accessNetwork, const RIL_DataProfileInfo_IRadio_1_4& dataProfile,  const bool roamingAllowed, const RIL_RadioDataRequestReasons reason,const SetupDataCallCallback& cb);
        /**
         * @brief
         * SetupDataCall API is used to bring up data call.
         * @note This is the preferred API for use.
         *
         * @param callParams This is a struct of type RIL_SetUpDataCallParams with all the necessary parameters to bring up the data call.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for Setupdatacall.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_Data_Call_Response_v11 *)> SetupDataCallCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * Data call bring up is SUCCESS/FAILURE would be inidcated in the setupdatacall Callback.
         *
         *
        */
        Status setupDataCall(const RIL_SetUpDataCallParams &callParams, const SetupDataCallCallback& cb);
        /**
         * @brief
         * DeactivateDatacall API. This Api is used to tear down / deactivate a data call.
         *
         * @param cid Call id is an int32_t variable. Every call which is successfully brought up is associated with a cid which indicates call identifier..
         * @param reason reason is a boolean variable. True indicates Radio Shutdown requested and False indicates no specific reason.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for DeactivateDatacall.
         * Prototype of the callback: std::fustd::function<void(#RIL_Errno)> DeactivateDataCallCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * Data call tear down / deactivation is SUCCESS/FAILURE would be inidcated in the Deactivatedatacall Callback.
         *
         *
        */
        Status deactivateDataCall(const int32_t cid, const bool reason, const DeactivateDataCallCallback& cb);
        /**
         * @brief
         * DataCallListChanged Indication Registration API. Client needs to register for the API to receive any DataCallListChanged
         * Indication. If there in any change, ex: ip address update, MTU update, change in DNS address etc. in the data call list,
         * an indication will be received via the callback.
         * The list is the complete list of current data contexts including new contexts that have been activated.
         * A data call is only removed from this list when the client sends a deactivateDataCall request or the radio is powered off/on
         * modem/network deactivates the call.
         *
         * @note It is advisable to register for this indication before SetupDataCall request
         * @param indicationHandler This a function pointer of type DataCallListChangedIndicationHandler.
         * Prototype of the handler: std::function<void(const #RIL_Data_Call_Response_v11 [], size_t)> DataCallListChangedIndicationHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         *
        */
        Status registerDataCallListChangedIndicationHandler(const DataCallListChangedIndicationHandler& indicationHandler);
        /**
         * @brief
         * SetInitialAttachApn API is used to set the apn with which initial attach is requested by client.
         * If the Profile is present on modem with same apn, then the non matched parameters are updated.
         * If there is no match with same apn, a new profile is created on modem & a request is sent to modem to trigger a Detach and re-attach with the updated APN.
         * @note Only LTE RAT requires the Inital Attach. Its not applicable for other RATs.
         * @param dataProfile A struct of type RIL_DataProfileInfo_IRadio_1_4 which provides the profile for the intital attach.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetInitalAttach.
         * Prototype of the callback: std::function<void(#RIL_Errno)> SetInitialAttachApnCallback .
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * Initial Attach is SUCCESS/FAILURE would be inidcated in the Initial Attach Callback.
         *
         *
        */
        Status setInitialAttachApn(const RIL_DataProfileInfo_IRadio_1_4& dataProfile, const SetInitialAttachApnCallback &cb);
        /**
         * @brief
         * Set Data Profile API. This Api is used to Set Data profiles on Modem.
         * If the Profile is present on modem with same apn, then the non matched parameters are updated.
         * If there is no match with same apn, a new profile is created on modem.
         * Some situations for client invoking the API : addition of an APN, SIM enablement, powerup etc.
         * @param dataProfile An array of struct of type RIL_DataProfileInfo_IRadio_1_4 which provides the list of profiles.
         * @param size unint32_t type variable which specifies the total number of profiles.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetDataProfile.
         * Prototype of the callback: std::function<void( #RIL_Errno)> SetDataProfileCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * SetDataProfile is SUCCESS/FAILURE would be inidcated in the SetDataProfile Callback.
         *
         *
        */
        Status setDataProfile(const RIL_DataProfileInfo_IRadio_1_4* dataProfile, const uint32_t size, const SetDataProfileCallback &cb);
        /**
         * @brief
         * Get Data Call List API. This API is used to get list of active Data Calls.
         * Returns the data call list.
         * An entry is added when a setupdatacall request is issued.
         * An entry is removed when deactivatedataCall request is sent from client or when call is teared down from modem/network.
         *
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for GetDataCallList.
         * Prototype of the callback: std::function<void(const #RIL_Errno, const #RIL_Data_Call_Response_v11 *, const size_t)> GetDataCallListCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * GetDataCallList SUCCESS/FAILURE would be inidcated in the GetDataCallList Callback.
         *
         *
        */
        Status getDataCallList(const GetDataCallListCallback &cb);
        Status StartLceData(const int32_t interval, const int32_t mode, const StartLceDataCallback &cb);
        Status StopLceData(const StopLceDataCallback &cb);
        Status PullLceData(const PullLceDataCallback &cb);
        /**
         * @brief
         * SetLinkCapFilter API. Link Capacity Indication is turned on/off using this API.
         *
         * @param enable_bit enum of type #RIL_ReportFilter which specifies the enable bit
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetLinkCapFilter.
         * Prototype of the callback: std::function<void( #RIL_Errno, const int*)> SetLinkCapFilterCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * SetLinkCapFilter SUCCESS/FAILURE would be inidcated in the SetLinkCapFilter Callback.
         *
         *
        */
        Status SetLinkCapFilter(const RIL_ReportFilter enable_bit, const SetLinkCapFilterCallback &cb);
        /**
         * @brief
         * SetLinkCapRptCriteria API. Setting Link Reporting Criteria is set via this API.
         *
         * @param criteria A sturct of type RIL_LinkCapCriteria which is used to pass the link report criteria.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetLinkCapFilter.
         * Prototype of the callback: std::function<void( #RIL_Errno, const #RIL_LinkCapCriteriaResult *)> SetLinkCapRptCriteriaCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * SetLinkCapRptCriteria SUCCESS/FAILURE would be inidcated in the SetLinkCapRptCriteria Callback.
         *
         *
        */
        Status SetLinkCapRptCriteria(const RIL_LinkCapCriteria &criteria, const SetLinkCapRptCriteriaCallback &cb);
        /**
         * @brief
         * GetDataNrIconType API. This API helps in getting NR/5G icon type.
         *
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for GetDataNrIconType.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #five_g_icon_type *)> GetDataNrIconTypeCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * GetDataNrIconType SUCCESS/FAILURE would be inidcated in the GetDataNrIconTypeCallback.
         *
         *
        */
        Status GetDataNrIconType(const GetDataNrIconTypeCallback &cb);
        /**
         * @brief
         * Pco Data Change Indication Registration API. Client needs to register for the API to receive any RILPCODataChanged
         * Indication. If there in any change in the Protocol Config Option, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type RILPCODataChangeIndicationHandler.
         * Prototype of the handler: std::function<void(const #RIL_PCO_Data)> RILPCODataChangeIndicationHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         *
        */
        Status registerPcoDataChangeHandler(const RILPCODataChangeIndicationHandler &indicationHandler);
        /**
         * @brief
         * LCE Data Change Indication Registration API. Client needs to register for the API to receive any indication for change in LCE Data.
         * If there in any change in the Link Capacity, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type RILLCEDataChangeIndicationHandler.
         * Prototype of the handler: std::functistd::function<void(const #RIL_LinkCapacityEstimate)> RILLCEDataChangeIndicationHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         *
        */
        Status registerLCEDataChangeHandler(const RILLCEDataChangeIndicationHandler &indicationHandler);
        /**
         * @brief
         * Data NrIcon Change Indication Registration API. Client needs to register for the API to receive any indication for change in DataNrIcon Type.
         * If there in any change in the NR/5g icon type, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type RILDataNrIconChangeIndicationHandler.
         * Prototype of the handler: std::function<void(const Status, const #five_g_icon_type )> RILDataNrIconChangeIndicationHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         */
        Status registerDataNrIconChangeHandler(const RILDataNrIconChangeIndicationHandler &indicationHandler);
        /**
         * @brief
         * CarrierInfoForImsi Indication Registration API. Client needs to register for the API to receive any indication for change in CarrierInfo for IMSI.
         * If there in any change in the Carrier Info for IMSI, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type CarrierInfoForImsiHandler.
         * Prototype of the handler: std::function<void()> CarrierInfoForImsiHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         */
        Status registerCarrierInfoForImsiHandler(const CarrierInfoForImsiHandler &indicationHandler);
        /**
         * @brief
         * SetPreferredDataModem API. This API is used to set preferred SIM for Data Usage.
         *
         * @param modemId int32_t variable which specifies preferred SIM for Data Usage.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for SetPreferredDataModem.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_ResponseError *)> SetPreferredDataModemCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * SetPreferredDataModem SUCCESS/FAILURE would be inidcated in the SetPreferredDataModem Callback.
         *
         *
        */
        Status SetPreferredDataModem(const int32_t modemId, const SetPreferredDataModemCallback &cb);
        /**
         * @brief
         * StartKeepAlive API. This API is used to start Keep Alive. Keep Alive is a feature to
         * prevent automatic termination of connection. In specific intervals, a data packet is sent
         * to the server to keep the connection established.
         *
         * @param request A struct of type RIL_KeepaliveRequest to provide source/ destination IP/port and desired interval in ms.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for StartKeepAlive.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_ResponseError *, const uint32_t*, const #RIL_KeepaliveStatus_t *)> StartKeepAliveCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * StartKeepAlive SUCCESS/FAILURE would be inidcated in the StartKeepAlive Callback.
         *
         *
        */
        Status StartKeepAlive(const RIL_KeepaliveRequest request, const StartKeepAliveCallback &cb);
        /**
         * @brief
         * Radio Keep Alive Indication Registration API. Client needs to register for the API to receive any indication for change in Radio KeepAlive Status.
         * If there in any change in the KeepAlive Status, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type RILDataRadioKeepAliveHandler.
         * Prototype of the handler: std::function<void(const Status, const uint32_t, const #RIL_KeepaliveStatusCode)> RILDataRadioKeepAliveHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         */
        Status registerRadioKeepAliveHandler(const RILDataRadioKeepAliveHandler &indicationHandler);
        /**
         * @brief
         * Physical Channel Config Indication Registration API. Client needs to register for the API to receive any indication for change in Physical Config.
         * If there in any change in the Physical Channel Config, an indication will be received via the callback.
         *
         * @param indicationHandler This a function pointer of type PhysicalConfigStructHandlerHandler.
         * Prototype of the handler: std::function<void(const Status, const std::vector< #RIL_PhysicalChannelConfig > )> PhysicalConfigStructHandlerHandler.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         *
         */
        Status registerPhysicalConfigStructHandler(const PhysicalConfigStructHandlerHandler &indicationHandler);
        /**
         * @brief
         * StopKeepAlive API. This API is used to stop a keep alive connection.
         *
         * @param handle int32_t type variable. Keep Alive handle which needs to be stopped.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for StopKeepAlive.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_ResponseError *)> StopKeepAliveCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * StopKeepAlive SUCCESS/FAILURE would be inidcated in the StopKeepAlive Callback.
         *
         *
        */
        Status StopKeepAlive(const int32_t handle, const StopKeepAliveCallback &cb);
        /**
         * @brief
         * CarrierInfoForImsiEncryption API. This API is used to set carrier info for "IMSI" Encrytion.
         *
         * @param carrier A struct RIL_CarrierInfoForImsiEncryption for passing all information required for carrier encryption.
         * @param type an enum of type #RIL_PublicKeyType which specifies the type of public key.
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for CarrierInfoForImsiEncryption.
         * Prototype of the callback: std::function<void(const #RIL_Errno, const Status, const #RIL_Errno *)> CarrierInfoForImsiEncryptionCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * CarrierInfoForImsiEncryption SUCCESS/FAILURE would be inidcated in the CarrierInfoForImsiEncryption Callback.
         *
         *
        */
        Status CarrierInfoForImsiEncryption(const RIL_CarrierInfoForImsiEncryption carrier, const RIL_PublicKeyType type,
        const CarrierInfoForImsiEncryptionCallback &cb);
        /**
         * @brief
         * RegisterDataUnsolIndication API. This API is used to Register for UI Change Indication.
         *
         * @param Filter int32_t type variable. Filter determines whether to register/deregister(1/0) a particular Unsol Indication.
         *        1st bit - UI INFO Indication
         * @param cb This cb is a function pointer which needs to be registered from client to get back response for StopKeepAlive.
         * Prototype of the callback: std::function<void( #RIL_Errno, const Status, const #RIL_ResponseError *)> RegisterForUIChangeIndCallback.
         *
         * @retval Status SUCCESS/FAILURE
         * @note SUCCESS indicates that the request is successfully delivered to the server.
         * FAILURE indicates that the request is not successfully delivered to the server. It can be due to a failure in server communication.
         * @par
         * RegisterDataUnsolIndication SUCCESS/FAILURE would be inidcated in the RegisterDataUnsolIndication Callback.
         *
         *
        */
        Status registerDataUnsolIndication(const int32_t filter, const RegisterDataUnsolIndicationCallback &cb);
        Status captureRilDataDump(const GetRilDataDumpCallback &cb);
        Status setTtyMode(const int mode, const SetTtyModeCallBack& cb);
        Status getTtyMode(const GetTtyModeCallBack& cb);
        Status setCdmaVoicePrefMode(const int mode, const SetCdmaVoicePrefModeCallBack& cb);
        Status getCdmaVoicePrefMode(const GetCdmaVoicePrefModeCallBack& cb);
        Status sendCdmaFlash(const std::string& flash, const SendCdmaFlashCallBack& cb);
        Status sendCdmaBurstDtmf(const std::string& dtmf, int32_t on, int32_t off,
                                 const SendCdmaBurstDtmfCallBack& cb);
        Status getImsRegState(const GetImsRegStateCallBack& cb);

        Status registerNewSmsOnSimIndicationHandler(
            const NewSmsOnSimIndicationHandler& indicationHandler);
        Status registerSimSmsStorageFullIndicationHandler(
            const SimSmsStorageFullIndicationHandler& indicationHandler);
        Status registerNewSmsStatusReportIndicationHandler(
            const NewSmsStatusReportIndicationHandler& indicationHandler);
        Status registerCdmaSmsRuimStorageFullIndicationHandler(
            const CdmaSmsRuimStorageFullIndicationHandler& indicationHandler);
        Status queryAvailableNetworks(const QueryAvailableNetworksCallback& cb);
        Status startNetworkScan(const RIL_NetworkScanRequest& req, const VoidResponseHandler& cb);
        Status stopNetworkScan(const VoidResponseHandler& cb);
        Status setSystemSelectionChannels(const RIL_SysSelChannels& req, const VoidResponseHandler& cb);
        Status registerNetworksScanIndicationHandler(
            const NetworkScanIndicationHandler& indicationHandler);
        Status registerEmergencyListIndicationHandler(
            const EmergencyListIndicationHandler& indicationHandler);

        Status GsmGetBroadcastSmsConfig(const GsmGetBroadcastSmsConfigCallback& cb);
        Status GsmSetBroadcastSMSConfig(RIL_GSM_BroadcastSmsConfigInfo *config, size_t len,
                const VoidResponseHandler &cb);
        Status GsmSmsBroadcastActivation(uint32_t disable, const VoidResponseHandler &cb);
        /**
         * @brief Indication Handler to handle Stk Service Running indication
         *
         * @param cb Callback to Handle Indication
         *           Callback prototype(typedef std::function<void(#RIL_Errno)> VoidResponseHandler)
         *
         *  @retval SUCCESS/FAILURE
         *  @note   SUCCESS If request is Successfully placed into the RIL.
         *          FAILURE if request was not placed into the RIL.
         */
        Status ReportStkServiceIsRunning(const VoidResponseHandler &cb);
       /**
        *  @brief Request to get ATR from card
        *
        *  @param slot  physical slot-id of the card
        *
        *  @param cb Callback to handle the response for request
        *            callback prototype(typedef std::function<void(#RIL_Errno, const char *, size_t)> CharStarSizeResponseHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status GetAtr(uint8_t slot, const CharStarSizeResponseHandler &cb);
        /**
        *  @brief Handling Call Setup Request from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno)> VoidResponseHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status StkHandleCallSetupRequestedFromSim(int32_t accept, const VoidResponseHandler &cb);
        /**
        *  @brief Sending Envelope command to Upper layers
        *
        *  @param command Conatins Envelope Data
        *
        *  @param size  Length of the Envelope command
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const char *, size_t)> CharStarSizeResponseHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status StkSendEnvelopeCommand(const char *command, size_t size, const CharStarSizeResponseHandler &cb);
        /**
        *  @brief Sending Terminal Response to Card
        *
        *  @param terminalResponse Conatins Terminal Response data
        *
        *  @param size  Length of the Terminal Response
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno)> VoidResponseHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status StkSendTerminalResponse(const char *terminalResponse, size_t size, const VoidResponseHandler &cb);
        // IMS APIs
        Status imsGetRegistrationState(const ImsGetRegistrationStateCallback& cb);
        Status imsQueryServiceStatus(const ImsQueryServiceStatusCallback& cb);
        Status imsRegistrationChange(const RIL_IMS_RegistrationState& state,
                                     const ImsRegistrationChangeCallback& cb);
        Status imsSetServiceStatus(const size_t, const RIL_IMS_ServiceStatusInfo **,
                                   const ImsSetServiceStatusCallback& cb);
        Status imsGetImsSubConfig(const ImsGetImsSubConfigCallback& cb);
        Status imsDial(const RIL_IMS_Dial& dial, const ImsDialCallback& cb);
        Status imsAnswer(const RIL_IMS_Answer& answer, const ImsAnswerCallback& cb);
        Status imsHangup(const RIL_IMS_Hangup& hangup, const ImsHangupCallback& cb);
        Status imsDtmf(char digit, const VoidResponseHandler &cb);
        Status imsDtmfStart(char digit, const VoidResponseHandler &cb);
        Status imsDtmfStop(const VoidResponseHandler &cb);
        Status imsModifyCallInitiate(const RIL_IMS_CallModifyInfo& modifyInfo,
                                     const ImsModifyCallInitiateCallback& cb);
        Status imsModifyCallConfirm(const RIL_IMS_CallModifyInfo& modifyInfo,
                                    const ImsModifyCallConfirmCallback& cb);
        Status imsCancelModifyCall(const uint32_t& callId, const ImsCancelModifyCallCallback& cb);
        Status imsAddParticipant(const char* address, const ImsAddParticipantCallback& cb);
        Status imsSetImsConfig(const RIL_IMS_ConfigInfo& config, const ImsSetImsConfigCallback& cb);
        Status imsGetImsConfig(const RIL_IMS_ConfigInfo& config, const ImsGetImsConfigCallback& cb);
        Status imsQueryCallForwardStatus(const RIL_IMS_CallForwardInfo& cfInfo,
                                         const ImsQueryCallForwardStatusCallback& cb);
        Status imsSetCallForwardStatus(const RIL_IMS_CallForwardInfo& cfInfo,
                                       const ImsSetCallForwardStatusCallback& cb);
        Status imsExplicitCallTransfer(const RIL_IMS_ExplicitCallTransfer& explicitCall,
                                       const ImsExplicitCallTransferCallback& cb);
        Status imsGetClir(const ImsGetClirCallback& cb);
        Status imsSetClir(const RIL_IMS_SetClirInfo& clirInfo,
                          const ImsSetClirCallback& cb);
        Status imsQueryClip(const ImsQueryClipCallback& cb);
        Status imsSetSuppSvcNotification(const RIL_IMS_ServiceClassStatus& serviceStatus,
                                         const ImsSetSuppSvcNotificationCallback& cb);
        Status imsDeflectCall(const RIL_IMS_DeflectCallInfo& deflectCall,
                              const ImsDeflectCallCallback& cb);
        Status imsSendUiTtyMode(const RIL_IMS_TtyNotifyInfo& ttyInfo,
                                const ImsSendUiTtyModeCallback& cb);
        Status imsSuppSvcStatus(const RIL_IMS_SuppSvcRequest& suppSvc,
                                const ImsSuppSvcStatusCallback& cb);
        Status imsRequestConference(const ImsRequestConferenceCallback& cb);
        Status imsRequestHoldCall(uint32_t callId, const ImsRequestHoldCallback& cb);
        Status imsRequestResumeCall(uint32_t callId, const ImsRequestResumeCallback& cb);
        Status imsRequestExitEmergencyCallbackMode(const ImsExitECBMCallback& cb);
        Status imsSetColr(const RIL_IMS_ColrInfo& colr, const ImsSetColrCallback& cb);
        Status imsGetColr(const ImsGetColrCallback& cb);
        Status imsRegisterMultiIdentityLines(const RIL_IMS_MultiIdentityLineInfo lineInfo[],
                size_t size, const ImsRegisterMultiIdentityLinesCallback& cb);
        Status imsQueryVirtualLineInfo(const char* msisdn,
                const ImsQueryVirtualLineInfoCallback& cb);
        Status imsSendSms(const RIL_IMS_SmsMessage& smsMessage, const ImsSendSmsCallback& cb);
        Status imsAckSms(const RIL_IMS_SmsAck& smsAck, const ImsAckSmsCallback& cb);
        Status imsGetSmsFormat(const ImsGetSmsFormatCallback& cb);
        Status imsSetCallWaiting(const RIL_IMS_CallWaitingSettings& callWaitingSettings, const ImsSetCallWaitingCallback& cb);
        Status imsQueryCallWaiting(uint32_t serviceClass, const ImsQueryCallWaitingCallback& cb);

        Status registerImsUnsolRegistrationStateIndicationHandler(
            const ImsRegistrationStateIndicationHandler& indicationHandler);
        Status registerImsUnsolServiceStatusIndicationHandler(
            const ImsServiceStatusIndicationHandler& indicationHandler);
        Status registerImsUnsolSubConfigChangeIndicationHandler(
            const ImsSubConfigChangeIndicationHandler& indicationHandler);
        Status registerImsUnsolCallRingIndicationHandler(
            const ImsCallRingIndicationHandler& indicationHandler);
        Status registerImsUnsolRingBackToneIndicationHandler(
            const ImsRingBackToneIndicationHandler& indicationHandler);
        Status registerImsUnsolCallStateChangedIndicationHandler(
            const ImsCallStateChangedIndicationHandler& indicationHandler);
        Status registerImsUnsolEnterEcbmIndicationHandler(
            const ImsEnterEcbmIndicationHandler& indicationHandler);
        Status registerImsUnsolExitEcbmIndicationHandler(
            const ImsExitEcbmIndicationHandler& indicationHandler);
        Status registerImsUnsolModifyCallIndicationHandler(
            const ImsModifyCallIndicationHandler& indicationHandler);
        Status registerImsUnsolMessageWaitingIndicationHandler(
            const ImsMessageWaitingIndicationHandler& indicationHandler);
        Status registerImsUnsolSuppSvcNotificationIndicationHandler(
            const ImsSuppSvcNotificationIndicationHandler& indicationHandler);
        Status registerImsUnsolOnSsIndicationHandler(
            const ImsOnSsIndicationHandler& indicationHandler);
        Status registerImsUnsolVopsChangedIndicationHandler(
            const ImsVopsChangedIndicationHandler& indicationHandler);
        Status registerImsUnsolParticipantStatusInfoIndicationHandler(
            const ImsParticipantStatusInfoIndicationHandler& indicationHandler);
        Status registerImsUnsolRegistrationBlockStatusIndicationHandler(
            const ImsRegistrationBlockStatusIndicationHandler& indicationHandler);
        Status registerImsUnsolWfcRoamingModeConfigSupportIndicationHandler(
            const ImsWfcRoamingModeConfigSupportIndicationHandler& indicationHandler);
        Status registerImsUnsolUssdFailedIndicationHandler(
            const ImsUssdFailedIndicationHandler& indicationHandler);
        Status registerImsUnsolVoiceInfoIndicationHandler(
            const ImsVoiceInfoIndicationHandler& indicationHandler);
        Status registerImsIncomingSmsHandler(const ImsIncomingSmsHandler& handler);
        Status registerImsSmsStatusReportHandler(const ImsSmsStatusReportHandler& handler);
        Status registerImsViceInfoHandler(const ImsViceInfoHandler& handler);
        Status registerImsTtyNotificationHandler(const ImsTtyNotificationHandler& handler);
        Status registerImsUnsolHandoverIndicationHandler(
            const ImsHandoverIndicationHandler& indicationHandler);
        Status registerImsUnsolRefreshConferenceInfo(
            const ImsRefreshConferenceInfoIndicationHandler& indicationHandler);
        Status registerImsUnsolMultiIdentityPendingInfo(
            const VoidIndicationHandler &indicationHandler);
        Status registerImsUnsolMultiIdentityRegistrationStatus(
            const MultiIdentityRegistrationStatusHandler indicationHandler);

        /* QCRIL_UIM Request Message APIs */
        /**
        *  @brief Request IMSI value from the card
        *
        *  @param aid  Aid related to Specific application in the card.
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(callback typedef std::function<void(#RIL_Errno, const char *)> GetImsiReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simGetImsiReq(const char *aid, const GetImsiReqCallback& cb);
        /**
        *  @brief Perform SIMIO Operations in card
        *
        *  @param RIL_SIM_IO_v6  Structure to hold below APDU parameters
        *                        Command.fileid,path,P1,P2,P3,
        *                        Pin2,data,aidPtr
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> IOReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simIOReq(const RIL_SIM_IO_v6& reqParams, const IOReqCallback& cb);
        /**
        *  @brief GetSimStatus Request to retrieve card info from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const char *)> GetImsiReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simGetSimStatusReq(const GetSimStatusReqCallback& cb);
        /**
        *  @brief simOpenChannelReq to open a logical channel from card
        *
        *  @param RIL_OpenChannelParams Structure to hold Open channel parameter AID info
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype (typedef std::function<void(#RIL_Errno, const int *, size_t len)> OpenChannelReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simOpenChannelReq(const RIL_OpenChannelParams& reqParams, const OpenChannelReqCallback& cb);
        /**
        *  @brief CloseChannel Request to clsoe already opened Logical channel
        *
        *  @param SessionId  Logical Session id
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype (typedef std::function<void(#RIL_Errno)> CloseChannelReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simCloseChannelReq(const int sessionId, const CloseChannelReqCallback& cb);
        /**
        *  @brief TransmitApduBasic  Request to Send a stream of bytes to card
        *
        *  @param RIL_SIM_APDU  Structure to hold APDU parameters
        *                       Session-id,cla,instruction,p1,p2,p3,data
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> TransmitApduBasicReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simTransmitApduBasicReq(const RIL_SIM_APDU& reqParams, const TransmitApduBasicReqCallback& cb);
        /**
        *  @brief TransmitApduChannel  Request to Send a stream of bytes to card
        *
        *  @param RIL_SIM_APDU  Structure to hold APDU parameters
        *                       Session-id,cla,instruction,p1,p2,p3,data
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> TransmitApduChannelReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simTransmitApduChannelReq(const RIL_SIM_APDU& reqParams, const TransmitApduChannelReqCallback& cb);
        /**
        *  @brief ChangePin Request to change pin in the card.
        *
        *  @param reqParams  Array of pointer to hold Old pin,Newpin and AID
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, int *)> ChangePinReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simChangePinReq(const char ** reqParams, const ChangePinReqCallback& cb);
        /**
        *  @brief ChangePin2 Request to change pin in the card.
        *
        *  @param reqParams  Pointer to hold Old pin,Newpin and AID info
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, int *)> ChangePin2ReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simChangePin2Req(const char ** reqParams, const ChangePin2ReqCallback& cb);
        /**
        *  @brief EnterPuk Request to Enter Puk code to unblock the card.
        *
        *  @param reqParams  Pointer to hold Puk code, Newpin, AID
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPukReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simEnterPukReq(const char ** reqParams, const EnterPukReqCallback& cb);
        /**
        *  @brief EnterPuk2 Request to Enter Puk code to unblock the card.
        *
        *  @param reqParams  Pointer to hold Puk code,Newpin and AID
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPuk2ReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simEnterPuk2Req(const char ** reqParams, const EnterPuk2ReqCallback& cb);
        /**
        *  @brief EnterPin Request to Verify PIN in the card.
        *
        *  @param reqParams  Pointer to hold Pin and AID info
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPinReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *          SUCCESS If request is Successfully placed into the RIL.
        *          Response will be notified using Callbacks.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simEnterPinReq(const char ** reqParams, const EnterPinReqCallback& cb);
        /**
        *  @brief EnterPin2 Request to Verify Pin2 in the card.
        *
        *  @param reqParams  Pointer to hold Pin2,AID info
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPin2ReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simEnterPin2Req(const char ** reqParams, const EnterPin2ReqCallback& cb);
        /**
        *  @brief QueryFacilitylock  Request to query FDN or PIN from card..
        *
        *  @param reqParams  Pointer to hold Password,AID,and Facility
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback Prototype (typedef std::function<void(#RIL_Errno, int32_t *)> QueryFacilityLockReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simQueryFacilityLockReq(const char ** reqParams, const QueryFacilityLockReqCallback& cb);
        /**
        *  @brief SetFacilitylock  Request to Set Pin or FDN to the card
        *
        *  @param reqParams  Pointer to hold Password,AID,lockstatus and Facility info
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback prototype(typedef std::function<void(#RIL_Errno, int *)> SetFacilityLockReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simSetFacilityLockReq(const char ** reqParams, const SetFacilityLockReqCallback& cb);
        /**
        *  @brief IsimAuthenticate  Request to Perform Authentication Procedure with card
        *
        *  @param reqParams  Pointer to hold the Card Challenge Data
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, const char *)> IsimAuthenticationReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simIsimAuthenticationReq(const char * reqParams, const IsimAuthenticationReqCallback& cb);
        /**
        *  @brief simStkSendEnvelopeWithStatus Request to send Envelope command to the card
        *
        *  @param reqParams  Pointer to hold Envelope data
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> StkSendEnvelopeWithStatusReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *          SUCCESS If request is Successfully placed into the RIL.
        *          Response will be notified using Callbacks.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simStkSendEnvelopeWithStatusReq(const char * reqParams, const StkSendEnvelopeWithStatusReqCallback& cb);
        /**
        *  @brief simAuthenticate  Request to Perform Authentication Procedure with card
        *
        *  @param reqParams  Pointer to hold the Card Challenge Data
        *
        *  @param cb   Callback to get the response of the request.
        *              Callback prototype(typedef std::function<void(#RIL_Errno, const #RIL_SIM_IO_Response *)> AuthenticationReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status simAuthenticationReq(const RIL_SimAuthentication &reqParams, const AuthenticationReqCallback& cb);
        /**
        *  @brief EnterNetworkDepersonalization  to send Depersonalization Request to card
        *
        *  @param reqParams  Pointer to hold the NetPin Value
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(#RIL_Errno, int *)> EnterPinReqCallback)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status EnterNetworkDepersonalization(const char **reqParams, const EnterPinReqCallback &cb);
        Status GetAdnRecord(const GetAdnRecordCallback &cb);
        Status UpdateAdnRecord(const RIL_AdnRecordInfo *record, const VoidResponseHandler &cb);
        Status registerAdnInitDone(
            const VoidIndicationHandler indicationHandler);
        /**
        *  @brief Registering Indication handler for card_status_change indications
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void()> VoidIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerSimStatusChanged(
            const VoidIndicationHandler &indicationHandler);
        Status registerStkCcAlphaNotify(
            const CharStarIndicationHandler &indicationHandler);
        /**
        *  @brief Registering Indication handler for Refresh indications from card
        *
        *  @param cb   Callback to get the response of the request.
        *              callback prototype(typedef std::function<void(const #RIL_SimRefreshResponse_v7 *)> SimRefreshIndicationHandler)
        *
        *  @retval SUCCESS/FAILURE
        *  @note   SUCCESS If request is Successfully placed into the RIL.
        *          FAILURE if request was not placed into the RIL.
        */
        Status registerSimRefresh(
            const SimRefreshIndicationHandler &indicationHandler);

    private:
        const std::string socketPath;
        const std::string ipAddress;
        const unsigned short tcpPort = 50000;
        int socketFd = -1;
        std::mutex socketFdMutex;
        std::mutex reqMgrMutex;

#ifdef QCRIL_ENABLE_AFL
    public:
        std::unique_ptr<RequestManager> reqMgr;
#else
    private:
        std::unique_ptr<RequestManager> reqMgr;
#endif
};

