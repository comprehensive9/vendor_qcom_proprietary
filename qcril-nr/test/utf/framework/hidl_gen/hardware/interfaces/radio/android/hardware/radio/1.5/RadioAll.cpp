#define LOG_TAG "android.hardware.radio@1.5::Radio"
#undef UNUSED


#include <android/hardware/radio/1.5/IRadio.h>
#include <log/log.h>
#include <cutils/trace.h>
#include "ril_utf_hidl_services.h"

namespace android {
namespace hardware {
namespace radio {
namespace V1_5 {

const char* IRadio::descriptor("android.hardware.radio@1.5::IRadio");

// Methods from ::android::hardware::radio::V1_0::IRadio follow.
// no default implementation for: ::android::hardware::Return<void> IRadio::setResponseFunctions(const ::android::sp<::android::hardware::radio::V1_0::IRadioResponse>& radioResponse, const ::android::sp<::android::hardware::radio::V1_0::IRadioIndication>& radioIndication)
// no default implementation for: ::android::hardware::Return<void> IRadio::getIccCardStatus(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::supplyIccPinForApp(int32_t serial, const ::android::hardware::hidl_string& pin, const ::android::hardware::hidl_string& aid)
// no default implementation for: ::android::hardware::Return<void> IRadio::supplyIccPukForApp(int32_t serial, const ::android::hardware::hidl_string& puk, const ::android::hardware::hidl_string& pin, const ::android::hardware::hidl_string& aid)
// no default implementation for: ::android::hardware::Return<void> IRadio::supplyIccPin2ForApp(int32_t serial, const ::android::hardware::hidl_string& pin2, const ::android::hardware::hidl_string& aid)
// no default implementation for: ::android::hardware::Return<void> IRadio::supplyIccPuk2ForApp(int32_t serial, const ::android::hardware::hidl_string& puk2, const ::android::hardware::hidl_string& pin2, const ::android::hardware::hidl_string& aid)
// no default implementation for: ::android::hardware::Return<void> IRadio::changeIccPinForApp(int32_t serial, const ::android::hardware::hidl_string& oldPin, const ::android::hardware::hidl_string& newPin, const ::android::hardware::hidl_string& aid)
// no default implementation for: ::android::hardware::Return<void> IRadio::changeIccPin2ForApp(int32_t serial, const ::android::hardware::hidl_string& oldPin2, const ::android::hardware::hidl_string& newPin2, const ::android::hardware::hidl_string& aid)
// no default implementation for: ::android::hardware::Return<void> IRadio::supplyNetworkDepersonalization(int32_t serial, const ::android::hardware::hidl_string& netPin)
// no default implementation for: ::android::hardware::Return<void> IRadio::getCurrentCalls(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::dial(int32_t serial, const ::android::hardware::radio::V1_0::Dial& dialInfo)
// no default implementation for: ::android::hardware::Return<void> IRadio::getImsiForApp(int32_t serial, const ::android::hardware::hidl_string& aid)
// no default implementation for: ::android::hardware::Return<void> IRadio::hangup(int32_t serial, int32_t gsmIndex)
// no default implementation for: ::android::hardware::Return<void> IRadio::hangupWaitingOrBackground(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::hangupForegroundResumeBackground(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::switchWaitingOrHoldingAndActive(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::conference(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::rejectCall(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getLastCallFailCause(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getSignalStrength(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getVoiceRegistrationState(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getDataRegistrationState(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getOperator(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setRadioPower(int32_t serial, bool on)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendDtmf(int32_t serial, const ::android::hardware::hidl_string& s)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendSms(int32_t serial, const ::android::hardware::radio::V1_0::GsmSmsMessage& message)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendSMSExpectMore(int32_t serial, const ::android::hardware::radio::V1_0::GsmSmsMessage& message)
// no default implementation for: ::android::hardware::Return<void> IRadio::setupDataCall(int32_t serial, ::android::hardware::radio::V1_0::RadioTechnology radioTechnology, const ::android::hardware::radio::V1_0::DataProfileInfo& dataProfileInfo, bool modemCognitive, bool roamingAllowed, bool isRoaming)
// no default implementation for: ::android::hardware::Return<void> IRadio::iccIOForApp(int32_t serial, const ::android::hardware::radio::V1_0::IccIo& iccIo)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendUssd(int32_t serial, const ::android::hardware::hidl_string& ussd)
// no default implementation for: ::android::hardware::Return<void> IRadio::cancelPendingUssd(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getClir(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setClir(int32_t serial, int32_t status)
// no default implementation for: ::android::hardware::Return<void> IRadio::getCallForwardStatus(int32_t serial, const ::android::hardware::radio::V1_0::CallForwardInfo& callInfo)
// no default implementation for: ::android::hardware::Return<void> IRadio::setCallForward(int32_t serial, const ::android::hardware::radio::V1_0::CallForwardInfo& callInfo)
// no default implementation for: ::android::hardware::Return<void> IRadio::getCallWaiting(int32_t serial, int32_t serviceClass)
// no default implementation for: ::android::hardware::Return<void> IRadio::setCallWaiting(int32_t serial, bool enable, int32_t serviceClass)
// no default implementation for: ::android::hardware::Return<void> IRadio::acknowledgeLastIncomingGsmSms(int32_t serial, bool success, ::android::hardware::radio::V1_0::SmsAcknowledgeFailCause cause)
// no default implementation for: ::android::hardware::Return<void> IRadio::acceptCall(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::deactivateDataCall(int32_t serial, int32_t cid, bool reasonRadioShutDown)
// no default implementation for: ::android::hardware::Return<void> IRadio::getFacilityLockForApp(int32_t serial, const ::android::hardware::hidl_string& facility, const ::android::hardware::hidl_string& password, int32_t serviceClass, const ::android::hardware::hidl_string& appId)
// no default implementation for: ::android::hardware::Return<void> IRadio::setFacilityLockForApp(int32_t serial, const ::android::hardware::hidl_string& facility, bool lockState, const ::android::hardware::hidl_string& password, int32_t serviceClass, const ::android::hardware::hidl_string& appId)
// no default implementation for: ::android::hardware::Return<void> IRadio::setBarringPassword(int32_t serial, const ::android::hardware::hidl_string& facility, const ::android::hardware::hidl_string& oldPassword, const ::android::hardware::hidl_string& newPassword)
// no default implementation for: ::android::hardware::Return<void> IRadio::getNetworkSelectionMode(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setNetworkSelectionModeAutomatic(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setNetworkSelectionModeManual(int32_t serial, const ::android::hardware::hidl_string& operatorNumeric)
// no default implementation for: ::android::hardware::Return<void> IRadio::getAvailableNetworks(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::startDtmf(int32_t serial, const ::android::hardware::hidl_string& s)
// no default implementation for: ::android::hardware::Return<void> IRadio::stopDtmf(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getBasebandVersion(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::separateConnection(int32_t serial, int32_t gsmIndex)
// no default implementation for: ::android::hardware::Return<void> IRadio::setMute(int32_t serial, bool enable)
// no default implementation for: ::android::hardware::Return<void> IRadio::getMute(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getClip(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getDataCallList(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setSuppServiceNotifications(int32_t serial, bool enable)
// no default implementation for: ::android::hardware::Return<void> IRadio::writeSmsToSim(int32_t serial, const ::android::hardware::radio::V1_0::SmsWriteArgs& smsWriteArgs)
// no default implementation for: ::android::hardware::Return<void> IRadio::deleteSmsOnSim(int32_t serial, int32_t index)
// no default implementation for: ::android::hardware::Return<void> IRadio::setBandMode(int32_t serial, ::android::hardware::radio::V1_0::RadioBandMode mode)
// no default implementation for: ::android::hardware::Return<void> IRadio::getAvailableBandModes(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendEnvelope(int32_t serial, const ::android::hardware::hidl_string& command)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendTerminalResponseToSim(int32_t serial, const ::android::hardware::hidl_string& commandResponse)
// no default implementation for: ::android::hardware::Return<void> IRadio::handleStkCallSetupRequestFromSim(int32_t serial, bool accept)
// no default implementation for: ::android::hardware::Return<void> IRadio::explicitCallTransfer(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setPreferredNetworkType(int32_t serial, ::android::hardware::radio::V1_0::PreferredNetworkType nwType)
// no default implementation for: ::android::hardware::Return<void> IRadio::getPreferredNetworkType(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getNeighboringCids(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setLocationUpdates(int32_t serial, bool enable)
// no default implementation for: ::android::hardware::Return<void> IRadio::setCdmaSubscriptionSource(int32_t serial, ::android::hardware::radio::V1_0::CdmaSubscriptionSource cdmaSub)
// no default implementation for: ::android::hardware::Return<void> IRadio::setCdmaRoamingPreference(int32_t serial, ::android::hardware::radio::V1_0::CdmaRoamingType type)
// no default implementation for: ::android::hardware::Return<void> IRadio::getCdmaRoamingPreference(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setTTYMode(int32_t serial, ::android::hardware::radio::V1_0::TtyMode mode)
// no default implementation for: ::android::hardware::Return<void> IRadio::getTTYMode(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setPreferredVoicePrivacy(int32_t serial, bool enable)
// no default implementation for: ::android::hardware::Return<void> IRadio::getPreferredVoicePrivacy(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendCDMAFeatureCode(int32_t serial, const ::android::hardware::hidl_string& featureCode)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendBurstDtmf(int32_t serial, const ::android::hardware::hidl_string& dtmf, int32_t on, int32_t off)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendCdmaSms(int32_t serial, const ::android::hardware::radio::V1_0::CdmaSmsMessage& sms)
// no default implementation for: ::android::hardware::Return<void> IRadio::acknowledgeLastIncomingCdmaSms(int32_t serial, const ::android::hardware::radio::V1_0::CdmaSmsAck& smsAck)
// no default implementation for: ::android::hardware::Return<void> IRadio::getGsmBroadcastConfig(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setGsmBroadcastConfig(int32_t serial, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::GsmBroadcastSmsConfigInfo>& configInfo)
// no default implementation for: ::android::hardware::Return<void> IRadio::setGsmBroadcastActivation(int32_t serial, bool activate)
// no default implementation for: ::android::hardware::Return<void> IRadio::getCdmaBroadcastConfig(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setCdmaBroadcastConfig(int32_t serial, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CdmaBroadcastSmsConfigInfo>& configInfo)
// no default implementation for: ::android::hardware::Return<void> IRadio::setCdmaBroadcastActivation(int32_t serial, bool activate)
// no default implementation for: ::android::hardware::Return<void> IRadio::getCDMASubscription(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::writeSmsToRuim(int32_t serial, const ::android::hardware::radio::V1_0::CdmaSmsWriteArgs& cdmaSms)
// no default implementation for: ::android::hardware::Return<void> IRadio::deleteSmsOnRuim(int32_t serial, int32_t index)
// no default implementation for: ::android::hardware::Return<void> IRadio::getDeviceIdentity(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::exitEmergencyCallbackMode(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getSmscAddress(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setSmscAddress(int32_t serial, const ::android::hardware::hidl_string& smsc)
// no default implementation for: ::android::hardware::Return<void> IRadio::reportSmsMemoryStatus(int32_t serial, bool available)
// no default implementation for: ::android::hardware::Return<void> IRadio::reportStkServiceIsRunning(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getCdmaSubscriptionSource(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::requestIsimAuthentication(int32_t serial, const ::android::hardware::hidl_string& challenge)
// no default implementation for: ::android::hardware::Return<void> IRadio::acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success, const ::android::hardware::hidl_string& ackPdu)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendEnvelopeWithStatus(int32_t serial, const ::android::hardware::hidl_string& contents)
// no default implementation for: ::android::hardware::Return<void> IRadio::getVoiceRadioTechnology(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getCellInfoList(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setCellInfoListRate(int32_t serial, int32_t rate)
// no default implementation for: ::android::hardware::Return<void> IRadio::setInitialAttachApn(int32_t serial, const ::android::hardware::radio::V1_0::DataProfileInfo& dataProfileInfo, bool modemCognitive, bool isRoaming)
// no default implementation for: ::android::hardware::Return<void> IRadio::getImsRegistrationState(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendImsSms(int32_t serial, const ::android::hardware::radio::V1_0::ImsSmsMessage& message)
// no default implementation for: ::android::hardware::Return<void> IRadio::iccTransmitApduBasicChannel(int32_t serial, const ::android::hardware::radio::V1_0::SimApdu& message)
// no default implementation for: ::android::hardware::Return<void> IRadio::iccOpenLogicalChannel(int32_t serial, const ::android::hardware::hidl_string& aid, int32_t p2)
// no default implementation for: ::android::hardware::Return<void> IRadio::iccCloseLogicalChannel(int32_t serial, int32_t channelId)
// no default implementation for: ::android::hardware::Return<void> IRadio::iccTransmitApduLogicalChannel(int32_t serial, const ::android::hardware::radio::V1_0::SimApdu& message)
// no default implementation for: ::android::hardware::Return<void> IRadio::nvReadItem(int32_t serial, ::android::hardware::radio::V1_0::NvItem itemId)
// no default implementation for: ::android::hardware::Return<void> IRadio::nvWriteItem(int32_t serial, const ::android::hardware::radio::V1_0::NvWriteItem& item)
// no default implementation for: ::android::hardware::Return<void> IRadio::nvWriteCdmaPrl(int32_t serial, const ::android::hardware::hidl_vec<uint8_t>& prl)
// no default implementation for: ::android::hardware::Return<void> IRadio::nvResetConfig(int32_t serial, ::android::hardware::radio::V1_0::ResetNvType resetType)
// no default implementation for: ::android::hardware::Return<void> IRadio::setUiccSubscription(int32_t serial, const ::android::hardware::radio::V1_0::SelectUiccSub& uiccSub)
// no default implementation for: ::android::hardware::Return<void> IRadio::setDataAllowed(int32_t serial, bool allow)
// no default implementation for: ::android::hardware::Return<void> IRadio::getHardwareConfig(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::requestIccSimAuthentication(int32_t serial, int32_t authContext, const ::android::hardware::hidl_string& authData, const ::android::hardware::hidl_string& aid)
// no default implementation for: ::android::hardware::Return<void> IRadio::setDataProfile(int32_t serial, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::DataProfileInfo>& profiles, bool isRoaming)
// no default implementation for: ::android::hardware::Return<void> IRadio::requestShutdown(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getRadioCapability(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setRadioCapability(int32_t serial, const ::android::hardware::radio::V1_0::RadioCapability& rc)
// no default implementation for: ::android::hardware::Return<void> IRadio::startLceService(int32_t serial, int32_t reportInterval, bool pullMode)
// no default implementation for: ::android::hardware::Return<void> IRadio::stopLceService(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::pullLceData(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getModemActivityInfo(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setAllowedCarriers(int32_t serial, bool allAllowed, const ::android::hardware::radio::V1_0::CarrierRestrictions& carriers)
// no default implementation for: ::android::hardware::Return<void> IRadio::getAllowedCarriers(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::sendDeviceState(int32_t serial, ::android::hardware::radio::V1_0::DeviceStateType deviceStateType, bool state)
// no default implementation for: ::android::hardware::Return<void> IRadio::setIndicationFilter(int32_t serial, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_0::IndicationFilter> indicationFilter)
// no default implementation for: ::android::hardware::Return<void> IRadio::setSimCardPower(int32_t serial, bool powerUp)
// no default implementation for: ::android::hardware::Return<void> IRadio::responseAcknowledgement()

// Methods from ::android::hardware::radio::V1_1::IRadio follow.
// no default implementation for: ::android::hardware::Return<void> IRadio::setCarrierInfoForImsiEncryption(int32_t serial, const ::android::hardware::radio::V1_1::ImsiEncryptionInfo& imsiEncryptionInfo)
// no default implementation for: ::android::hardware::Return<void> IRadio::setSimCardPower_1_1(int32_t serial, ::android::hardware::radio::V1_1::CardPowerState powerUp)
// no default implementation for: ::android::hardware::Return<void> IRadio::startNetworkScan(int32_t serial, const ::android::hardware::radio::V1_1::NetworkScanRequest& request)
// no default implementation for: ::android::hardware::Return<void> IRadio::stopNetworkScan(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::startKeepalive(int32_t serial, const ::android::hardware::radio::V1_1::KeepaliveRequest& keepalive)
// no default implementation for: ::android::hardware::Return<void> IRadio::stopKeepalive(int32_t serial, int32_t sessionHandle)

// Methods from ::android::hardware::radio::V1_2::IRadio follow.
// no default implementation for: ::android::hardware::Return<void> IRadio::startNetworkScan_1_2(int32_t serial, const ::android::hardware::radio::V1_2::NetworkScanRequest& request)
// no default implementation for: ::android::hardware::Return<void> IRadio::setIndicationFilter_1_2(int32_t serial, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_2::IndicationFilter> indicationFilter)
// no default implementation for: ::android::hardware::Return<void> IRadio::setSignalStrengthReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDb, const ::android::hardware::hidl_vec<int32_t>& thresholdsDbm, ::android::hardware::radio::V1_2::AccessNetwork accessNetwork)
// no default implementation for: ::android::hardware::Return<void> IRadio::setLinkCapacityReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps, int32_t hysteresisUlKbps, const ::android::hardware::hidl_vec<int32_t>& thresholdsDownlinkKbps, const ::android::hardware::hidl_vec<int32_t>& thresholdsUplinkKbps, ::android::hardware::radio::V1_2::AccessNetwork accessNetwork)
// no default implementation for: ::android::hardware::Return<void> IRadio::setupDataCall_1_2(int32_t serial, ::android::hardware::radio::V1_2::AccessNetwork accessNetwork, const ::android::hardware::radio::V1_0::DataProfileInfo& dataProfileInfo, bool modemCognitive, bool roamingAllowed, bool isRoaming, ::android::hardware::radio::V1_2::DataRequestReason reason, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& addresses, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& dnses)
// no default implementation for: ::android::hardware::Return<void> IRadio::deactivateDataCall_1_2(int32_t serial, int32_t cid, ::android::hardware::radio::V1_2::DataRequestReason reason)

// Methods from ::android::hardware::radio::V1_3::IRadio follow.
// no default implementation for: ::android::hardware::Return<void> IRadio::setSystemSelectionChannels(int32_t serial, bool specifyChannels, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::RadioAccessSpecifier>& specifiers)
// no default implementation for: ::android::hardware::Return<void> IRadio::enableModem(int32_t serial, bool on)
// no default implementation for: ::android::hardware::Return<void> IRadio::getModemStackStatus(int32_t serial)

// Methods from ::android::hardware::radio::V1_4::IRadio follow.
// no default implementation for: ::android::hardware::Return<void> IRadio::setupDataCall_1_4(int32_t serial, ::android::hardware::radio::V1_4::AccessNetwork accessNetwork, const ::android::hardware::radio::V1_4::DataProfileInfo& dataProfileInfo, bool roamingAllowed, ::android::hardware::radio::V1_2::DataRequestReason reason, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& addresses, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& dnses)
// no default implementation for: ::android::hardware::Return<void> IRadio::setInitialAttachApn_1_4(int32_t serial, const ::android::hardware::radio::V1_4::DataProfileInfo& dataProfileInfo)
// no default implementation for: ::android::hardware::Return<void> IRadio::setDataProfile_1_4(int32_t serial, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::DataProfileInfo>& profiles)
// no default implementation for: ::android::hardware::Return<void> IRadio::emergencyDial(int32_t serial, const ::android::hardware::radio::V1_0::Dial& dialInfo, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::EmergencyServiceCategory> categories, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& urns, ::android::hardware::radio::V1_4::EmergencyCallRouting routing, bool hasKnownUserIntentEmergency, bool isTesting)
// no default implementation for: ::android::hardware::Return<void> IRadio::startNetworkScan_1_4(int32_t serial, const ::android::hardware::radio::V1_2::NetworkScanRequest& request)
// no default implementation for: ::android::hardware::Return<void> IRadio::getPreferredNetworkTypeBitmap(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::setPreferredNetworkTypeBitmap(int32_t serial, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::RadioAccessFamily> networkTypeBitmap)
// no default implementation for: ::android::hardware::Return<void> IRadio::setAllowedCarriers_1_4(int32_t serial, const ::android::hardware::radio::V1_4::CarrierRestrictionsWithPriority& carriers, ::android::hardware::radio::V1_4::SimLockMultiSimPolicy multiSimPolicy)
// no default implementation for: ::android::hardware::Return<void> IRadio::getAllowedCarriers_1_4(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getSignalStrength_1_4(int32_t serial)

// Methods from ::android::hardware::radio::V1_5::IRadio follow.
// no default implementation for: ::android::hardware::Return<void> IRadio::setSignalStrengthReportingCriteria_1_5(int32_t serial, const ::android::hardware::radio::V1_5::SignalThresholdInfo& signalThresholdInfo, ::android::hardware::radio::V1_5::AccessNetwork accessNetwork)
// no default implementation for: ::android::hardware::Return<void> IRadio::enableUiccApplications(int32_t serial, bool enable)
// no default implementation for: ::android::hardware::Return<void> IRadio::areUiccApplicationsEnabled(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::canToggleUiccApplicationsEnablement(int32_t serial)
// no default implementation for: ::android::hardware::Return<void> IRadio::getCurrentCalls_1_5(int32_t serial)

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> IRadio::interfaceChain(interfaceChain_cb _hidl_cb){
    (void)_hidl_cb;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadio::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadio::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::android::hardware::radio::V1_5::IRadio::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadio::getHashChain(getHashChain_cb _hidl_cb){
    (void)_hidl_cb;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadio::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IRadio::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> IRadio::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadio::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hidl::base::V1_0::DebugInfo info = {};
    (void)_hidl_cb; 
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadio::notifySyspropsChanged(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IRadio::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadio>> IRadio::castFrom(const ::android::sp<::android::hardware::radio::V1_5::IRadio>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadio>> IRadio::castFrom(const ::android::sp<::android::hardware::radio::V1_4::IRadio>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadio>(static_cast<::android::hardware::radio::V1_5::IRadio*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadio>> IRadio::castFrom(const ::android::sp<::android::hardware::radio::V1_3::IRadio>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadio>(static_cast<::android::hardware::radio::V1_5::IRadio*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadio>> IRadio::castFrom(const ::android::sp<::android::hardware::radio::V1_2::IRadio>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadio>(static_cast<::android::hardware::radio::V1_5::IRadio*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadio>> IRadio::castFrom(const ::android::sp<::android::hardware::radio::V1_1::IRadio>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadio>(static_cast<::android::hardware::radio::V1_5::IRadio*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadio>> IRadio::castFrom(const ::android::sp<::android::hardware::radio::V1_0::IRadio>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadio>(static_cast<::android::hardware::radio::V1_5::IRadio*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_5::IRadio>> IRadio::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_5::IRadio>(static_cast<::android::hardware::radio::V1_5::IRadio*>(parent.get()));
}

static std::unordered_map<std::string, ::android::sp<::android::hardware::radio::V1_5::IRadio>> mHidlServices;

::android::sp<IRadio> IRadio::tryGetService(const std::string &serviceName, const bool getStub) {
    return mHidlServices[serviceName];
}

::android::sp<IRadio> IRadio::getService(const std::string &serviceName, const bool getStub) {
    return mHidlServices[serviceName];
}

::android::status_t IRadio::registerAsService(const std::string &serviceName) {
    mHidlServices[serviceName] = this;
    ::android::hardware::radio::V1_4::IRadio::registerAsService(serviceName);
    return ::android::OK;
}

bool IRadio::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
    return true;
}

}  // namespace V1_5
}  // namespace radio
}  // namespace hardware
}  // namespace android
