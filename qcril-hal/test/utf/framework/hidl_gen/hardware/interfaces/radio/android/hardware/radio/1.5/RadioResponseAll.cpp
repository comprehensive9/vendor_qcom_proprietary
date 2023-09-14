#define LOG_TAG "android.hardware.radio@1.5::RadioResponse"
#undef UNUSED

#include <android/hardware/radio/1.5/IRadioResponse.h>
#include <log/log.h>
#include <cutils/trace.h>
#include "ril_utf_hidl_services.h"

namespace android {
namespace hardware {
namespace radio {
namespace V1_5 {

const char* IRadioResponse::descriptor("android.hardware.radio@1.5::IRadioResponse");

// Methods from ::android::hardware::radio::V1_0::IRadioResponse follow.
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getIccCardStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::CardStatus& cardStatus)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::supplyIccPinForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t remainingRetries)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::supplyIccPukForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t remainingRetries)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::supplyIccPin2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t remainingRetries)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::supplyIccPuk2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t remainingRetries)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::changeIccPinForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t remainingRetries)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::changeIccPin2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t remainingRetries)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::supplyNetworkDepersonalizationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t remainingRetries)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCurrentCallsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::Call>& calls)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::dialResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getIMSIForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& imsi)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::hangupConnectionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::hangupWaitingOrBackgroundResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::hangupForegroundResumeBackgroundResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::switchWaitingOrHoldingAndActiveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::conferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::rejectCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getLastCallFailCauseResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::LastCallFailCauseInfo& failCauseinfo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getSignalStrengthResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SignalStrength& sigStrength)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getVoiceRegistrationStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::VoiceRegStateResult& voiceRegResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getDataRegistrationStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::DataRegStateResult& dataRegResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getOperatorResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& longName, const ::android::hardware::hidl_string& shortName, const ::android::hardware::hidl_string& numeric)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setRadioPowerResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SendSmsResult& sms)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendSMSExpectMoreResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SendSmsResult& sms)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setupDataCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SetupDataCallResult& dcResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::iccIOForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::IccIoResult& iccIo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendUssdResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::cancelPendingUssdResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getClirResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t n, int32_t m)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setClirResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCallForwardStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CallForwardInfo>& callForwardInfos)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setCallForwardResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCallWaitingResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enable, int32_t serviceClass)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setCallWaitingResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::acknowledgeLastIncomingGsmSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::acceptCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::deactivateDataCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getFacilityLockForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t response)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setFacilityLockForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t retry)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setBarringPasswordResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getNetworkSelectionModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool manual)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setNetworkSelectionModeAutomaticResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setNetworkSelectionModeManualResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getAvailableNetworksResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::OperatorInfo>& networkInfos)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::startDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::stopDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getBasebandVersionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& version)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::separateConnectionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setMuteResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getMuteResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enable)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getClipResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::ClipStatus status)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getDataCallListResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::SetupDataCallResult>& dcResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setSuppServiceNotificationsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::writeSmsToSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t index)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::deleteSmsOnSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setBandModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getAvailableBandModesResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::RadioBandMode>& bandModes)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendEnvelopeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& commandResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendTerminalResponseToSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::handleStkCallSetupRequestFromSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::explicitCallTransferResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setPreferredNetworkTypeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getPreferredNetworkTypeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::PreferredNetworkType nwType)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getNeighboringCidsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::NeighboringCell>& cells)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setLocationUpdatesResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setCdmaSubscriptionSourceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setCdmaRoamingPreferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCdmaRoamingPreferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::CdmaRoamingType type)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setTTYModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getTTYModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::TtyMode mode)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setPreferredVoicePrivacyResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getPreferredVoicePrivacyResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enable)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendCDMAFeatureCodeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendBurstDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendCdmaSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SendSmsResult& sms)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::acknowledgeLastIncomingCdmaSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getGsmBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::GsmBroadcastSmsConfigInfo>& configs)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setGsmBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setGsmBroadcastActivationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCdmaBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CdmaBroadcastSmsConfigInfo>& configs)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setCdmaBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setCdmaBroadcastActivationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCDMASubscriptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& mdn, const ::android::hardware::hidl_string& hSid, const ::android::hardware::hidl_string& hNid, const ::android::hardware::hidl_string& min, const ::android::hardware::hidl_string& prl)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::writeSmsToRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, uint32_t index)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::deleteSmsOnRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getDeviceIdentityResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& imei, const ::android::hardware::hidl_string& imeisv, const ::android::hardware::hidl_string& esn, const ::android::hardware::hidl_string& meid)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::exitEmergencyCallbackModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getSmscAddressResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& smsc)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setSmscAddressResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::reportSmsMemoryStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::reportStkServiceIsRunningResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCdmaSubscriptionSourceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::CdmaSubscriptionSource source)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::requestIsimAuthenticationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& response)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::acknowledgeIncomingGsmSmsWithPduResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendEnvelopeWithStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::IccIoResult& iccIo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getVoiceRadioTechnologyResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::RadioTechnology rat)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCellInfoListResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>& cellInfo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setCellInfoListRateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setInitialAttachApnResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getImsRegistrationStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool isRegistered, ::android::hardware::radio::V1_0::RadioTechnologyFamily ratFamily)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendImsSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SendSmsResult& sms)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::iccTransmitApduBasicChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::IccIoResult& result)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::iccOpenLogicalChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t channelId, const ::android::hardware::hidl_vec<int8_t>& selectResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::iccCloseLogicalChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::iccTransmitApduLogicalChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::IccIoResult& result)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::nvReadItemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& result)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::nvWriteItemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::nvWriteCdmaPrlResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::nvResetConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setUiccSubscriptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setDataAllowedResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getHardwareConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::HardwareConfig>& config)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::requestIccSimAuthenticationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::IccIoResult& result)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setDataProfileResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::requestShutdownResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getRadioCapabilityResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::RadioCapability& rc)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setRadioCapabilityResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::RadioCapability& rc)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::startLceServiceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::LceStatusInfo& statusInfo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::stopLceServiceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::LceStatusInfo& statusInfo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::pullLceDataResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::LceDataInfo& lceInfo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getModemActivityInfoResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::ActivityStatsInfo& activityInfo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setAllowedCarriersResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t numAllowed)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getAllowedCarriersResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool allAllowed, const ::android::hardware::radio::V1_0::CarrierRestrictions& carriers)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::sendDeviceStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setIndicationFilterResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setSimCardPowerResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::acknowledgeRequest(int32_t serial)

// Methods from ::android::hardware::radio::V1_1::IRadioResponse follow.
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setCarrierInfoForImsiEncryptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setSimCardPowerResponse_1_1(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::startNetworkScanResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::stopNetworkScanResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::startKeepaliveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_1::KeepaliveStatus& status)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::stopKeepaliveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)

// Methods from ::android::hardware::radio::V1_2::IRadioResponse follow.
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCellInfoListResponse_1_2(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>& cellInfo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getIccCardStatusResponse_1_2(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_2::CardStatus& cardStatus)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setSignalStrengthReportingCriteriaResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setLinkCapacityReportingCriteriaResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCurrentCallsResponse_1_2(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::Call>& calls)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getSignalStrengthResponse_1_2(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_2::SignalStrength& signalStrength)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getVoiceRegistrationStateResponse_1_2(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_2::VoiceRegStateResult& voiceRegResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getDataRegistrationStateResponse_1_2(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_2::DataRegStateResult& dataRegResponse)

// Methods from ::android::hardware::radio::V1_3::IRadioResponse follow.
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setSystemSelectionChannelsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::enableModemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getModemStackStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool isEnabled)

// Methods from ::android::hardware::radio::V1_4::IRadioResponse follow.
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::emergencyDialResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::startNetworkScanResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCellInfoListResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>& cellInfo)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getDataRegistrationStateResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_4::DataRegStateResult& dataRegResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getIccCardStatusResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_4::CardStatus& cardStatus)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getPreferredNetworkTypeBitmapResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::RadioAccessFamily> networkTypeBitmap)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setPreferredNetworkTypeBitmapResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getDataCallListResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::SetupDataCallResult>& dcResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setupDataCallResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_4::SetupDataCallResult& dcResponse)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setAllowedCarriersResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getAllowedCarriersResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_4::CarrierRestrictionsWithPriority& carriers, ::android::hardware::radio::V1_4::SimLockMultiSimPolicy multiSimPolicy)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getSignalStrengthResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_4::SignalStrength& signalStrength)

// Methods from ::android::hardware::radio::V1_5::IRadioResponse follow.
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::setSignalStrengthReportingCriteriaResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::enableUiccApplicationsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::areUiccApplicationsEnabledResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enabled)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::canToggleUiccApplicationsEnablementResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool canToggle)
// no default implementation for: ::android::hardware::Return<void> IRadioResponse::getCurrentCallsResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::Call>& calls)

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> IRadioResponse::interfaceChain(interfaceChain_cb _hidl_cb){
    (void)_hidl_cb;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioResponse::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioResponse::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::android::hardware::radio::V1_5::IRadioResponse::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioResponse::getHashChain(getHashChain_cb _hidl_cb){
    (void)_hidl_cb;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioResponse::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IRadioResponse::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> IRadioResponse::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioResponse::getDebugInfo(getDebugInfo_cb _hidl_cb){
    (void)_hidl_cb;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioResponse::notifySyspropsChanged(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IRadioResponse::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadioResponse>> IRadioResponse::castFrom(const ::android::sp<::android::hardware::radio::V1_5::IRadioResponse>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadioResponse>> IRadioResponse::castFrom(const ::android::sp<::android::hardware::radio::V1_4::IRadioResponse>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadioResponse>(static_cast<::android::hardware::radio::V1_5::IRadioResponse*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadioResponse>> IRadioResponse::castFrom(const ::android::sp<::android::hardware::radio::V1_3::IRadioResponse>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadioResponse>(static_cast<::android::hardware::radio::V1_5::IRadioResponse*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadioResponse>> IRadioResponse::castFrom(const ::android::sp<::android::hardware::radio::V1_2::IRadioResponse>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadioResponse>(static_cast<::android::hardware::radio::V1_5::IRadioResponse*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadioResponse>> IRadioResponse::castFrom(const ::android::sp<::android::hardware::radio::V1_1::IRadioResponse>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadioResponse>(static_cast<::android::hardware::radio::V1_5::IRadioResponse*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadioResponse>> IRadioResponse::castFrom(const ::android::sp<::android::hardware::radio::V1_0::IRadioResponse>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadioResponse>(static_cast<::android::hardware::radio::V1_5::IRadioResponse*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadioResponse>> IRadioResponse::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadioResponse>(static_cast<::android::hardware::radio::V1_5::IRadioResponse*>(parent.get()));
}

::android::sp<IRadioResponse> IRadioResponse::tryGetService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::sp<IRadioResponse> IRadioResponse::getService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::status_t IRadioResponse::registerAsService(const std::string &serviceName) {
    return ::android::OK;
}

bool IRadioResponse::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
    return true;
}

}  // namespace V1_5
}  // namespace radio
}  // namespace hardware
}  // namespace android
