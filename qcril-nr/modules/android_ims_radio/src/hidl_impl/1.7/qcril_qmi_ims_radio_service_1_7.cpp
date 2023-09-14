/******************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/
#define TAG "RILQ"

#include <interfaces/voice/QcRilRequestCancelUssdMessage.h>
#include <interfaces/voice/QcRilRequestGetCallBarringMessage.h>
#include <interfaces/voice/QcRilRequestImsDialMessage.h>
#include <interfaces/voice/QcRilRequestSendSipDtmfMessage.h>
#include <interfaces/voice/QcRilRequestSendUssdMessage.h>
#include <interfaces/voice/QcRilRequestSetSupsServiceMessage.h>
#include <interfaces/voice/QcRilRequestQueryColpMessage.h>
#include <interfaces/ims/QcRilRequestImsSetConfigMessage.h>
#include <interfaces/ims/QcRilRequestImsQueryServiceStatusMessage.h>
#include <interfaces/ims/QcRilRequestImsQueryServiceStatusMessage.h>
#include <interfaces/ims/QcRilRequestImsSetServiceStatusMessage.h>
#include <interfaces/ims/QcRilRequestImsGetRegStateMessage.h>
#include <modules/android_ims_radio/hidl_impl/base/qcril_qmi_ims_radio_utils.h>
#include <modules/android_ims_radio/hidl_impl/1.6/qcril_qmi_ims_radio_service_1_6.h>
#include <modules/android_ims_radio/hidl_impl/1.7/qcril_qmi_ims_radio_service_1_7.h>
#include <modules/android_ims_radio/hidl_impl/1.7/qcril_qmi_ims_radio_utils_1_7.h>
#include <modules/voice/qcril_qmi_voice.h>
#include <string.h>

extern "C" {
#include "qcril_log.h"
#include "qcril_reqlist.h"
}

using ::android::hardware::Void;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace V1_7 {
namespace implementation {

void ImsRadioImpl_1_7::setCallback_nolock(
    const ::android::sp<::vendor::qti::hardware::radio::ims::V1_0::IImsRadioResponse>
        &imsRadioResponse,
    const ::android::sp<::vendor::qti::hardware::radio::ims::V1_0::IImsRadioIndication>
        &imsRadioIndication) {
  QCRIL_LOG_DEBUG("ImsRadioImpl_1_7::setCallback_nolock");

  mImsRadioIndicationCbV1_7 =
      V1_7::IImsRadioIndication::castFrom(imsRadioIndication).withDefault(nullptr);
  mImsRadioResponseCbV1_7 =
      V1_7::IImsRadioResponse::castFrom(imsRadioResponse).withDefault(nullptr);

  if (mImsRadioResponseCbV1_7 == nullptr || mImsRadioIndicationCbV1_7 == nullptr) {
    mImsRadioResponseCbV1_7 = nullptr;
    mImsRadioIndicationCbV1_7 = nullptr;
  }

  if (mBaseImsRadioImpl) {
    return mBaseImsRadioImpl->setCallback_nolock(imsRadioResponse, imsRadioIndication);
  }
}
/*
 *   @brief
 *   Registers the callback for IImsRadio using the IImsRadioCallback object
 *   being passed in by the client as a parameter
 *
 */
::android::hardware::Return<void> ImsRadioImpl_1_7::setCallback(
    const ::android::sp<::vendor::qti::hardware::radio::ims::V1_0::IImsRadioResponse>
        &imsRadioResponse,
    const ::android::sp<::vendor::qti::hardware::radio::ims::V1_0::IImsRadioIndication>
        &imsRadioIndication) {
  QCRIL_LOG_DEBUG("ImsRadioImpl_1_7::setCallback");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(ImsRadioImplBase::mCallbackLock);
    if (mImsRadioIndicationCbV1_7 != nullptr) {
      mImsRadioIndicationCbV1_7->unlinkToDeath(this);
    }
    setCallback_nolock(imsRadioResponse, imsRadioIndication);
    if (mImsRadioIndicationCbV1_7 != nullptr) {
      mImsRadioIndicationCbV1_7->linkToDeath(this, 0);
    }
  }
  notifyImsClientConnected();

  return Void();
}

sp<V1_7::IImsRadioIndication> ImsRadioImpl_1_7::getIndicationCallbackV1_7() {
  std::unique_lock<qtimutex::QtiSharedMutex> lock(ImsRadioImplBase::mCallbackLock);
  return mImsRadioIndicationCbV1_7;
}

sp<V1_7::IImsRadioResponse> ImsRadioImpl_1_7::getResponseCallbackV1_7() {
  std::unique_lock<qtimutex::QtiSharedMutex> lock(ImsRadioImplBase::mCallbackLock);
  return mImsRadioResponseCbV1_7;
}

void ImsRadioImpl_1_7::serviceDied(uint64_t,
                                   const ::android::wp<::android::hidl::base::V1_0::IBase> &) {
  QCRIL_LOG_DEBUG("ImsRadioImpl_1_7::serviceDied: Client died. Cleaning up callbacks");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(ImsRadioImplBase::mCallbackLock);
  clearCallbacks();
}

ImsRadioImpl_1_7::ImsRadioImpl_1_7(qcril_instance_id_e_type instance) : ImsRadioImplBase(instance) {
  mBaseImsRadioImpl = new V1_6::implementation::ImsRadioImpl_1_6(instance);
}

ImsRadioImpl_1_7::~ImsRadioImpl_1_7() {}

void ImsRadioImpl_1_7::clearCallbacks() {
  QCRIL_LOG_ERROR("enter");
  {
    mImsRadioResponseCbV1_7 = nullptr;
    mImsRadioIndicationCbV1_7 = nullptr;
    if (mBaseImsRadioImpl) {
      mBaseImsRadioImpl->clearCallbacks();
    }
  }
  QCRIL_LOG_ERROR("exit");
}

android::status_t ImsRadioImpl_1_7::registerService() {
  std::string serviceName = "imsradio" + std::to_string(getInstanceId());
  android::status_t ret = registerAsService(serviceName);
  QCRIL_LOG_INFO("registerService: starting ImsRadioImpl_1_7 %s status %d", serviceName.c_str(),
                 ret);
  return ret;
}

void ImsRadioImpl_1_7::notifyOnRTTMessage(
    std::shared_ptr<QcRilUnsolImsRttMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnRTTMessage(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnVowifiCallQuality(
    std::shared_ptr<QcRilUnsolImsVowifiCallQuality> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnVowifiCallQuality(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnVopsChanged(
    std::shared_ptr<QcRilUnsolImsVopsIndication> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnVopsChanged(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnSubConfigChanged(
    std::shared_ptr<QcRilUnsolImsSubConfigIndication> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnSubConfigChanged(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnGeoLocationChange(
    std::shared_ptr<QcRilUnsolImsGeoLocationInfo> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnGeoLocationChange(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnServiceStatusChange(
    std::shared_ptr<QcRilUnsolImsSrvStatusIndication> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnServiceStatusChange(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnSsacInfoChange(
    std::shared_ptr<QcRilUnsolImsSsacInfoIndication> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnSsacInfoChange(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnRegBlockStatusChange(
    std::shared_ptr<QcRilUnsolImsRegBlockStatusMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnRegBlockStatusChange(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnRing(std::shared_ptr<QcRilUnsolCallRingingMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnRing(msg);
  }
}
void ImsRadioImpl_1_7::notifyOnRingbackTone(std::shared_ptr<QcRilUnsolRingbackToneMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnRingbackTone(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnModifyCall(std::shared_ptr<QcRilUnsolImsModifyCallMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnModifyCall(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnTtyNotification(
    std::shared_ptr<QcRilUnsolImsTtyNotificationMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnTtyNotification(msg);
  }
}
void ImsRadioImpl_1_7::notifyOnRefreshConferenceInfo(
    std::shared_ptr<QcRilUnsolImsConferenceInfoMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnRefreshConferenceInfo(msg);
  }
}
void ImsRadioImpl_1_7::notifyOnRefreshViceInfo(std::shared_ptr<QcRilUnsolImsViceInfoMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnRefreshViceInfo(msg);
  }
}
void ImsRadioImpl_1_7::notifyOnSuppServiceNotification(
    std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnSuppServiceNotification(msg);
  }
}
void ImsRadioImpl_1_7::notifyOnParticipantStatusInfo(
    std::shared_ptr<QcRilUnsolConfParticipantStatusInfoMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnParticipantStatusInfo(msg);
  }
}
void ImsRadioImpl_1_7::notifyOnSupplementaryServiceIndication(
      std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnSupplementaryServiceIndication(msg);
  }
}

void ImsRadioImpl_1_7::notifyIncomingImsSms(std::shared_ptr<RilUnsolIncomingImsSMSMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyIncomingImsSms(msg);
  }
}

void ImsRadioImpl_1_7::notifyNewImsSmsStatusReport(
      std::shared_ptr<RilUnsolNewImsSmsStatusReportMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyNewImsSmsStatusReport(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnCallStateChanged(
    std::shared_ptr<QcRilUnsolImsCallStatusMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  sp<V1_7::IImsRadioIndication> indCb = getIndicationCallbackV1_7();
  if (!indCb) {
    if (mBaseImsRadioImpl) {
        mBaseImsRadioImpl->notifyOnCallStateChanged(msg);
    }
    return;
  }
  std::vector<qcril::interfaces::CallInfo> callInfo = msg->getCallInfo();
  if (callInfo.empty()) {
    QCRIL_LOG_ERROR("empty callInfo");
    return;
  }
  hidl_vec<V1_7::CallInfo> callListV1_7 = {};
  bool result = V1_7::utils::convertCallInfoList(callListV1_7, callInfo);
  if (!result) {
    QCRIL_LOG_ERROR("CallInfo convertion failed");
    return;
  }
  imsRadiolog("<", "onCallStateChanged_1_7: callList = " + toString(callListV1_7));
  Return<void> ret = indCb->onCallStateChanged_1_7(callListV1_7);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
  }
}

void ImsRadioImpl_1_7::notifyOnCallComposerInfoAvailable(
    std::shared_ptr<QcRilUnsolImsCallComposerInfo> msg) {
  if (mBaseImsRadioImpl) {
     mBaseImsRadioImpl->notifyOnCallComposerInfoAvailable(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnIncomingCallAutoRejected(
    std::shared_ptr<QcRilUnsolAutoCallRejectionMessage> msg) {
  mBaseImsRadioImpl->notifyOnIncomingCallAutoRejected(msg);
}

void ImsRadioImpl_1_7::notifyOnPendingMultiIdentityStatus(
    std::shared_ptr<QcRilUnsolImsPendingMultiLineStatus> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnPendingMultiIdentityStatus(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnMultiIdentityLineStatus(
    std::shared_ptr<QcRilUnsolImsMultiIdentityStatusMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnMultiIdentityLineStatus(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnVoiceInfoStatusChange(
    std::shared_ptr<QcRilUnsolImsVoiceInfo> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnVoiceInfoStatusChange(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnExitEcbmIndication(
    std::shared_ptr<QcRilUnsolImsExitEcbmIndication> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnExitEcbmIndication(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnEnterEcbmIndication(
    std::shared_ptr<QcRilUnsolImsEnterEcbmIndication> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnEnterEcbmIndication(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnRadioStateChanged(
    std::shared_ptr<QcRilUnsolImsRadioStateIndication> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnRadioStateChanged(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnMessageWaiting(
      std::shared_ptr<QcRilUnsolMessageWaitingInfoMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnMessageWaiting(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnModemSupportsWfcRoamingModeConfiguration(
    std::shared_ptr<QcRilUnsolImsWfcRoamingConfigIndication> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnModemSupportsWfcRoamingModeConfiguration(msg);
  }
}
void ImsRadioImpl_1_7::notifyOnUssdMessageFailed(std::shared_ptr<QcRilUnsolOnUssdMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnUssdMessageFailed(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnRetrievingGeoLocationDataStatus(
    std::shared_ptr<QcRilUnsolImsGeoLocationDataStatus> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnRetrievingGeoLocationDataStatus(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnUssdMessage(std::shared_ptr<QcRilUnsolOnUssdMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnUssdMessage(msg);
  }
}

void ImsRadioImpl_1_7::sendSetConfigMessageResponse(uint32_t token, uint32_t errorCode,
  std::shared_ptr<qcril::interfaces::ConfigInfo> data) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->sendSetConfigMessageResponse(token, errorCode, data);
  }
}

Return<void> ImsRadioImpl_1_7::getImsRegistrationState(int32_t token) {
  if (mBaseImsRadioImpl) {
    return mBaseImsRadioImpl->getImsRegistrationState(token);
  }
  return Void();
}

Return<void> ImsRadioImpl_1_7::queryServiceStatus(int32_t token) {
  if (mBaseImsRadioImpl) {
    return mBaseImsRadioImpl->queryServiceStatus(token);
  }
  return Void();
}

void ImsRadioImpl_1_7::notifyOnRegistrationChanged(std::shared_ptr<QcRilUnsolImsRegStateMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnRegistrationChanged(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnHandover(std::shared_ptr<QcRilUnsolImsHandoverMessage> msg) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->notifyOnHandover(msg);
  }
}

void ImsRadioImpl_1_7::notifyOnSipDtmfReceived(std::shared_ptr<QcRilUnsolOnSipDtmfMessage> msg) {
  QCRIL_LOG_ERROR("notifyOnSipDtmfReceived");
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  sp<V1_7::IImsRadioIndication> indCb = getIndicationCallbackV1_7();
  if (!indCb) {
    QCRIL_LOG_DEBUG("cannot fallback");
    return;
  }
  hidl_string SipMsg = "";
  if (msg->hasMessage()) {
    SipMsg = msg->getMessage();
  }
  Return<void> ret = indCb->onSipDtmfReceived(SipMsg);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
  }
}

Return<void> ImsRadioImpl_1_7::sendSipDtmf(int32_t token,
        const ::android::hardware::hidl_string &requestCode) {
  imsRadiolog(">", "sendSipDtmf: token = " + std::to_string(token) +
          " requestCode = " + toString(requestCode));

  bool sendFailure = false;
  uint32_t err = RIL_E_GENERIC_FAILURE;
  do {
    auto msg = std::make_shared<QcRilRequestSendSipDtmfMessage>(getContext(token));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    if (requestCode.empty()) {
      QCRIL_LOG_ERROR("Invalid parameter: requestCode");
      sendFailure = true;
      break;
    }

    msg->setSipInfo(requestCode.c_str());
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          uint32_t errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = V1_0::utils::qcril_qmi_ims_map_ril_error_to_ims_error(resp->errorCode);
          }
          sendSipDtmfResponse(token, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    sendSipDtmfResponse(token, err);
  }
  return Void();
}

void ImsRadioImpl_1_7::sendSipDtmfResponse(int32_t token, uint32_t errorCode ) {
  sp<IImsRadioResponse> respCb = getResponseCallbackV1_7();
  if (respCb != nullptr) {
    imsRadiolog("<", "sendSipDtmfResponse: token = " + std::to_string(token) + " errorCode = " +
            std::to_string(errorCode));
    Return<void> ret = respCb->sendSipDtmfResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }
}

void ImsRadioImpl_1_7::sendSuppServiceStatusResponse(int32_t token, uint32_t errorCode,
    const std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> data) {
  if (mBaseImsRadioImpl) {
    mBaseImsRadioImpl->sendSuppServiceStatusResponse(token, errorCode, data);
  }
}

Return<void> ImsRadioImpl_1_7::sendUssd(
    int32_t token, const ::android::hardware::hidl_string &ussd) {
  if (mBaseImsRadioImpl) {
    return mBaseImsRadioImpl->sendUssd(token, ussd);
  }
  return Void();
}

Return<void> ImsRadioImpl_1_7::cancelPendingUssd(int32_t token) {
  if (mBaseImsRadioImpl) {
    return mBaseImsRadioImpl->cancelPendingUssd(token);
  }
  return Void();
}

Return<void> ImsRadioImpl_1_7::callComposerDial(int32_t token,
    const ::vendor::qti::hardware::radio::ims::V1_6::DialRequest &dialRequest,
    const ::vendor::qti::hardware::radio::ims::V1_6::CallComposerInfo &callComposerInfo) {
  if (mBaseImsRadioImpl) {
    return mBaseImsRadioImpl->callComposerDial(token, dialRequest, callComposerInfo);
  }
  return Void();
}
}  // namespace implementation
}  // namespace V1_7
}  // namespace ims
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
