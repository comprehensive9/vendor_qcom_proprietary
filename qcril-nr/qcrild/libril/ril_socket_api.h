/******************************************************************************
#  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <telephony/ril_ims.h>
#include <telephony/ril.h>
#include <interfaces/ims/ims.h>
#include <interfaces/voice/voice.h>
#include <SocketRequestContext.h>
#include <Marshal.h>
#include <marshal/Dial.h>
#include <framework/Message.h>

int sendResponse(std::shared_ptr<SocketRequestContext> context,
                 RIL_Errno errorCode, std::shared_ptr<Marshal> p);
int sendUnsolResponse(int unsolResponse, std::shared_ptr<Marshal> p);

namespace ril {
namespace socket {
namespace api {

void handleClientDisconnectionRequest(std::shared_ptr<SocketRequestContext> context, Marshal& p);

Marshal newResponse(RIL_SOCKET_ID, uint64_t token, uint32_t errorCode);
void dispatchDeactivateDataCall(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetupDataCall(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetInitialAttachApn(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetDataProfile(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetDataCallList(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStartLce(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStopLce(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchPullLceData(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetLinkCapFilter(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetLinkCapRptCriteria(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCarrierInfoImsiEncryption(std::shared_ptr<SocketRequestContext> context, Marshal& p);

void dispatchDial(std::shared_ptr<SocketRequestContext> context, Marshal& p);

void dispatchRilRequestGsmGetBroadcastSmsConfig(
        std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchRilRequestGsmSetBroadcastSmsConfig(
        std::shared_ptr<SocketRequestContext> ctx, Marshal &p);
void dispatchRilRequestGsmSmsBroadcastActivation(
        std::shared_ptr<SocketRequestContext> ctx, Marshal &p);
void dispatchSendSms(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSmsAck(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetSmscAddress(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetSmscAddress(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchWriteSmsToSim(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchDeleteSmsOnSim(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void reportSmsMemoryStatus(std::shared_ptr<SocketRequestContext> context, const Marshal& p);
void dispatchCdmaSetSubscriptionSource(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaGetSubscriptionSource(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaSetRoamingPreference(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaGetRoamingPreference(std::shared_ptr<SocketRequestContext> context, Marshal& p);

RIL_Errno dispatchStatusToRilErrno(Message::Callback::Status status);
void dispatchGetVoiceRegState(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetDataRegState(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetIndicationFilter(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchHangup(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSendUssd(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCancelUssd(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchDtmf(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchDtmfStart(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchDtmfStop(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchLastCallFailCause(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetNetworkType(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetNetworkType(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchVoiceRadioTech(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchDeviceIdentity(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchOperatorName(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchQueryNetworkSelectionMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetNetworkSelectionAutomatic(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetNetworkSelectionManual(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetLocationUpdates(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchBasebandVersion(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchExplicitCallTransfer(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetMute(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetMute(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetClir(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetClir(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSendRttMsg(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSendGeolocationInfo(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetUnsolCellInfoListRate(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetCurrentCalls(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchAnswer(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchConference(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSwitchWaitingOrHoldingAndActive(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchUdub(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchHangupWaitingOrBackground(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchHangupForegroundResumeBackground(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSeparateConnection(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchShutDown(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchExitEmergencyCbMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetBandMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchQueryAvailableBandMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetRadioCapability(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetRadioCapability(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSendCdmaSms(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchAckCdmaSms(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaGetBroadcastSmsConfig(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaSetBroadcastSmsConfig(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaSmsBroadcastActivation(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaWriteSmsToRuim(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaDeleteSmsOnRuim(std::shared_ptr<SocketRequestContext> context, Marshal& p);

void dispatchSignalStrength(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchRadioPower(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchAllowData(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchEnableSim(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetEnableSimStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetActivityInfo(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCellInfo(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchEnterNetworkDepersonalization(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStkSendEnvelopeCommand(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStkSendTerminalResponse(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStkHandleCallSetupRequestedFromSim(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStkServiceIsRunning(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchQueryCallForwardStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetCallForward(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchQueryCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchChangeBarringPassword(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchQueryClip(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetSuppSvcNotification(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchQueryAvailableNetworks(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStartNetworkScanRequest(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStopNetworkScanRequest(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetSystemSelectionChannels(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetTtyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetTtyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetVoicePrivacyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetVoicePrivacyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaFlash(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCdmaBurstDtmf(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetImsRegistration(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void setSignalStrengthReportingCriteria(std::shared_ptr<SocketRequestContext> context, const Marshal& p);
void dispatchOemhookRaw(std::shared_ptr<SocketRequestContext> context, Marshal& p);

//---- IMS Requests: BEGIN ----
void dispatchGetImsRegistration(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsGetRegistrationState(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsRegistrationChange(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsQueryServiceStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSetServiceStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsGetImsSubConfig(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsDial(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsAnswer(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsHangup(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsDtmf(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsDtmfStart(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsDtmfStop(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsModifyCallInitiate(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsModifyCallConfirm(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsCancelModifyCall(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsAddParticipant(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSetConfig(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsGetConfig(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsQueryCallForwardStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSetCallForwardStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsExplicitCallTransfer(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsGetClir(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSetClir(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsDeflectCall(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSendUiTtyMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsQueryClip(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSetSuppSvcNotification(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSuppSvcStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsRequestConference(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsRequestHold(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsRequestResume(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsExitEmergencyCallbackMode(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsGetColr(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsSetColr(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsRegisterMultiIdentityLines(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchImsQueryVirtualLineInfo(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void imsSendSms(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void imsAckSms(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void imsGetSmsFormat(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void imsSetCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void imsQueryCallWaiting(std::shared_ptr<SocketRequestContext> context, Marshal& p);

//--- IMS Utility functions
qcril::interfaces::RegState convertRegState(const RIL_IMS_RegistrationState& in);
qcril::interfaces::RttMode convertRttMode(const RIL_IMS_RttMode in);
qcril::interfaces::CallType convertCallType(const RIL_IMS_CallType in);
qcril::interfaces::CallDomain convertCallDomain(const RIL_IMS_CallDomain in);
qcril::interfaces::Presentation convertIpPresentation(const RIL_IMS_IpPresentation in);
bool convertConfigInfo(qcril::interfaces::ConfigInfo& out, const RIL_IMS_ConfigInfo& in);
bool convertCallFwdTimerInfo(qcril::interfaces::CallFwdTimerInfo &out,
                             const RIL_IMS_CallFwdTimerInfo &in);
qcril::interfaces::EctType convertEctType(const RIL_IMS_EctType &in);
qcril::interfaces::TtyMode convertTtyMode(const RIL_IMS_TtyModeType &in);

//---- IMS Requests: END ----
void dispatchCdmaSubscription(std::shared_ptr<SocketRequestContext> context, Marshal& p);

void dispatchGetDataNrIconType(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSetPreferredDataModem(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStartKeepAlive(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchStopKeepAlive(std::shared_ptr<SocketRequestContext> context, Marshal& p);

RIL_RadioState getRadioState();

/* UIM Messages to dispatch to core module */

void dispatchSimGetImsiReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimIOReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimGetAtr(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimTransmitApduChannel(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchUpdateAdnRecord(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchGetAdnRecord(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimGetSimStatusReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimOpenChannelReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimCloseChannelReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);

void dispatchSimTransmitApduBasicReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimTransmitApduChannelReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimChangePinReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimChangePin2Req(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimEnterPukReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);

void dispatchSimEnterPuk2Req(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimEnterPinReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimEnterPin2Req(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimQueryFacilityLockReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimSetFacilityLockReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);

void dispatchSimIsimAuthenticationReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimStkSendEnvelopeWithStatusReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchSimAuthenticationReq(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchCaptureRilDataDump(std::shared_ptr<SocketRequestContext> context, Marshal& p);
void dispatchRegisterDataUnsolIndication(std::shared_ptr<SocketRequestContext> context, Marshal& p);
}
}
}
