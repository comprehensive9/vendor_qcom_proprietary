/******************************************************************************
  Copyright (c) 2017, 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#ifndef VENDOR_QTI_HARDWARE_RADIO_UIM_LPA_V1_0_H
#define VENDOR_QTI_HARDWARE_RADIO_UIM_LPA_V1_0_H

#include <vendor/qti/hardware/radio/lpa/1.0/IUimLpa.h>
#include <vendor/qti/hardware/radio/lpa/1.0/types.h>
#include <vendor/qti/hardware/radio/lpa/1.0/IUimLpaIndication.h>
#include <vendor/qti/hardware/radio/lpa/1.0/IUimLpaResponse.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "modules/lpa/lpa_service_types.h"
#include "hidl_impl/lpa_service_base.h"
#include "modules/lpa/LpaModule.h"
#undef TAG
#define TAG "LpaService"


namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_0 {
namespace implementation {

template <typename T>
class UimLpaImpl : public T, public LpaServiceBase{
 private:
    ::android::sp<V1_0::IUimLpaResponse> mResponseCb;
    ::android::sp<V1_0::IUimLpaIndication> mIndicationCb;

    ::android::sp<V1_0::IUimLpaResponse> getResponseCallback() {
        std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
        return mResponseCb;
    }
    ::android::sp<V1_0::IUimLpaIndication> getIndicationCallback() {
        std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
        return mIndicationCb;
    }

 protected:
    void clearCallbacks_nolock() {
        QCRIL_LOG_DEBUG("V1_0::setCallback_nolock");
        mIndicationCb = nullptr;
        mResponseCb = nullptr;
    }

    virtual void clearCallbacks() {
        std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
        clearCallbacks_nolock();
    }

   void setCallback_nolock(const ::android::sp<V1_0::IUimLpaResponse>& respCb,
                          const ::android::sp<V1_0::IUimLpaIndication>& indCb) {
    QCRIL_LOG_DEBUG("V1_0::setCallback_nolock");
    if (mResponseCb != nullptr) {
      mResponseCb->unlinkToDeath(this);
    }
    mIndicationCb = indCb;
    mResponseCb = respCb;
    if (mResponseCb != nullptr) {
      mResponseCb->linkToDeath(this, 0);
    }
  }

  void uimLpaUserResponse
  (
    int32_t                         token,
    lpa_service_user_resp_type    * user_resp
  )
  {
    UimLpaProfileInfo  * profiles = NULL;
    uint8_t              p_count  = 0;

    ::android::sp<IUimLpaResponse> respCb = getResponseCallback();
    UimLpaUserResp       userResp;
    if (respCb == NULL || user_resp == NULL)
    {
      QCRIL_LOG_ERROR("UimLpaImpl::uimLpaUserResponse responseCb is null or userResp NULL");
      return;
    }

    memset(&userResp, 0x00, sizeof(userResp));

    QCRIL_LOG_INFO("UimRemoteClientImpl::uimRemoteClientEventResponse token=%d event=%d", token, user_resp->event);

    userResp.event = (UimLpaUserEventId)user_resp->event;
    userResp.result = (UimLpaResult)user_resp->result;
    userResp.eid.setToExternal(user_resp->eid, user_resp->eid_len);

    if (user_resp->no_of_profiles != 0 && user_resp->profiles != NULL)
    {
      profiles = new UimLpaProfileInfo[user_resp->no_of_profiles];

      if (profiles == NULL)
      {
        QCRIL_LOG_ERROR("Allocation failed");
        return;
      }
      for (p_count = 0; p_count < user_resp->no_of_profiles; p_count++)
      {
        profiles[p_count].state = (UimLpaProfileState)user_resp->profiles[p_count].state;
        profiles[p_count].iccid.setToExternal(
                   user_resp->profiles[p_count].iccid,
                   user_resp->profiles[p_count].iccid_len);
        profiles[p_count].profileName = user_resp->profiles[p_count].profileName;
        profiles[p_count].nickName = user_resp->profiles[p_count].nickName;
        profiles[p_count].spName = user_resp->profiles[p_count].spName;
        profiles[p_count].iconType =
           (UimLpaIconType)user_resp->profiles[p_count].iconType;
        profiles[p_count].icon.setToExternal(
           user_resp->profiles[p_count].icon,
           user_resp->profiles[p_count].icon_len);
        profiles[p_count].profileClass =
          (UimLpaProfileClassType)user_resp->profiles[p_count].profileClass;
        profiles[p_count].profilePolicy =
          (UimLpaProfilePolicyMask)user_resp->profiles[p_count].profilePolicy;
      }
    }
    userResp.profiles.setToExternal(profiles, user_resp->no_of_profiles);
    userResp.srvAddr.smdpAddress = user_resp->srvAddr.smdpAddress;
    userResp.srvAddr.smdsAddress = user_resp->srvAddr.smdsAddress;
    ::android::hardware::Return<void> ret = respCb->UimLpaUserResponse(token, userResp);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
    if (profiles != NULL)
    {
      delete[] profiles;
      profiles = NULL;
    }
    return;
  }

  /*===========================================================================

  FUNCTION:  UimLpaImpl::uimLpaHttpTxnCompletedResponse

  ===========================================================================*/
  void  uimLpaHttpTxnCompletedResponse
  (
    int32_t                      token,
    lpa_service_result_type      result
  )
  {
    ::android::sp<IUimLpaResponse> respCb = getResponseCallback();
    if (respCb == NULL)
    {
      QCRIL_LOG_ERROR("uimLpaHttpTxnCompletedResponse responseCb is null");
      return;
    }
    QCRIL_LOG_INFO("UimLpaHttpTxnCompletedResponse token=%d result=%d", token, result);
    ::android::hardware::Return<void> ret = respCb->UimLpaHttpTxnCompletedResponse(token, (UimLpaResult)result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  } /* UimLpaImpl::uimLpaHttpTxnCompletedResponse */


  public:
   static const HalServiceImplVersion& getVersion();

   bool registerService(qcril_instance_id_e_type instId) {
     this->setInstanceId(instId);
     std::string serviceName = "UimLpa" + std::to_string(instId);
     android::status_t ret = T::registerAsService(serviceName);
     Log::getInstance().d(std::string("[LpaModule]: ") + T::descriptor +
                          (ret == android::OK ? " registered" : " failed to register"));
     return (ret == android::OK);
   }

   ::android::hardware::Return<void> setCallback(
      const ::android::sp<V1_0::IUimLpaResponse>& respCb,
      const ::android::sp<V1_0::IUimLpaIndication>& indCb) {
    QCRIL_LOG_DEBUG("setCallback");
    std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    setCallback_nolock(respCb, indCb);
    return ::android::hardware::Void();
   }

   ::android::hardware::Return<void> UimLpaUserRequest
   (
     int32_t              token,
     const V1_0::UimLpaUserReq& userReq
   )
   {
     lpa_service_user_req_type   user_req;
     memset(&user_req, 0x00, sizeof(user_req));
     QCRIL_LOG_INFO("UimLpaUserRequest: token_id: 0x%x, event: %d", token, userReq.event);
     user_req.event = (lpa_service_user_event_type)userReq.event;
     user_req.activationCode = userReq.activationCode.c_str();
     user_req.confirmationCode = userReq.confirmationCode.c_str();
     user_req.nickname = userReq.nickname.c_str();
     user_req.iccid    = userReq.iccid.data();
     user_req.iccid_len = userReq.iccid.size();
     user_req.resetMask = userReq.resetMask;
     user_req.userOk = userReq.userOk;
     user_req.srvOpReq.opCode = (lpa_service_srv_addr_op_type)userReq.srvOpReq.opCode;
     user_req.srvOpReq.smdpAddress = userReq.srvOpReq.smdpAddress.c_str();
     if (this->mModule != nullptr)
     {
         QCRIL_LOG_INFO("mModule is not null");
         this->mModule->handleUimLpaUserRequest(token, &user_req);
     }
     return ::android::hardware::Void();
  } /* UimLpaImpl::UimLpaUserRequest */

  ::android::hardware::Return<void> UimLpaHttpTxnCompletedRequest
  (
    int32_t                                 token,
    UimLpaResult                            result,
    const ::android::hardware::hidl_vec<uint8_t>&                responsePayload,
    const ::android::hardware::hidl_vec<V1_0::UimLpaHttpCustomHeader>& customHeaders
  )
  {
    lpa_service_http_transaction_req_type   http_req;
    memset(&http_req, 0x00, sizeof(http_req));
    http_req.tokenId = token;
    http_req.result  = (lpa_service_result_type)result;
    http_req.payload = responsePayload.data();
    http_req.payload_len = responsePayload.size();

    QCRIL_LOG_INFO("UimLpaHttpTxnCompletedRequest: token_id: 0x%x", token);

    if (customHeaders.size() != 0 &&
        customHeaders.data() != 0) {
      http_req.customHeaders =
        new lpa_service_http_custom_header_req_type[customHeaders.size()]{};
      if (http_req.customHeaders != NULL)
      {
        uint8_t                        itr     = 0;
        const UimLpaHttpCustomHeader * headers = customHeaders.data();
        for (; itr < customHeaders.size(); itr++)
        {
          http_req.customHeaders[itr].headerName = headers[itr].headerName.c_str();
          http_req.customHeaders[itr].headerValue = headers[itr].headerValue.c_str();
        }
        http_req.no_of_headers = customHeaders.size();
      }
    }
    if (this->mModule != nullptr)
    {
      this->mModule->handleUimLpaHttpTxnCompletedRequest(token, &http_req);
    }
    if (http_req.customHeaders != NULL)
    {
      delete[] http_req.customHeaders;
      http_req.customHeaders = NULL;
    }
    return ::android::hardware::Void();
  } /* UimLpaImpl::UimLpaHttpTxnCompletedRequest */

  /*===========================================================================

  FUNCTION:  UimLpaImpl::uimLpaAddProfileProgressInd

  ===========================================================================*/
  void uimLpaAddProfileProgressInd
  (
      lpa_service_add_profile_progress_ind_type * progInd
  )
  {
    ::android::sp<IUimLpaIndication> indCb = getIndicationCallback();
    if (indCb == NULL || progInd == NULL)
    {
      QCRIL_LOG_ERROR("uimLpaAddProfileProgressInd indicationCb is null or progInd");
      return;
    }
    else
    {
        UimLpaAddProfileProgressInd  progress;
        memset(&progress, 0x00, sizeof(progress));
        progress.status              = (UimLpaAddProfileStatus)progInd->status;
        progress.cause               = (UimLpaAddProfileFailureCause)progInd->cause;
        progress.progress            = progInd->progress;
        progress.policyMask          = (UimLpaProfilePolicyMask)progInd->policyMask;
        progress.userConsentRequired = progInd->userConsentRequired;
        ::android::hardware::Return<void> ret = indCb->UimLpaAddProfileProgressIndication(progress);
        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        }
     }
  } /* UimLpaImpl::uimLpaAddProfileProgressInd */

  /*===========================================================================

  FUNCTION:  UimLpaImpl::uimLpaHttpTxnIndication

  ===========================================================================*/
  void uimLpaHttpTxnIndication
  (
      lpa_service_http_transaction_ind_type  * txnInd
  )
  {
    UimLpaHttpTransactionInd           httpTxnInd;
    uint8_t                            h_count = 0;
    UimLpaHttpCustomHeader           * headers = NULL;
    ::android::sp<IUimLpaIndication> indCb = getIndicationCallback();
    if (indCb == NULL || txnInd == NULL)
    {
       QCRIL_LOG_ERROR("uimLpaHttpTxnIndication indicationCb is null or txnInd NULL");
      return ;
    }

    memset(&httpTxnInd, 0x00, sizeof(httpTxnInd));
    httpTxnInd.tokenId = txnInd->tokenId;
    httpTxnInd.payload.setToExternal(txnInd->payload, txnInd->payload_len);

    QCRIL_LOG_INFO("uimLpaHttpTxnIndication: token_id: 0x%x, payload_len: %d",
       httpTxnInd.tokenId, txnInd->payload_len);

    if (txnInd->contentType != NULL)
    {
      httpTxnInd.contentType = txnInd->contentType;
    }
    else
    {
      std::string str = {};
      str.resize(0);
      httpTxnInd.contentType = str;
    }

    if (txnInd->no_of_headers != 0)
    {
      headers = new UimLpaHttpCustomHeader[txnInd->no_of_headers];

      if (headers == NULL)
      {
        QCRIL_LOG_ERROR("Allocation failed for headers");
      }

      if (txnInd->customHeaders != NULL)
      {
        for (h_count = 0 ; h_count < txnInd->no_of_headers; h_count++)
        {
          headers[h_count].headerName = txnInd->customHeaders[h_count].headerName;
          headers[h_count].headerValue = txnInd->customHeaders[h_count].headerValue;
        }
      }
    }
    httpTxnInd.customHeaders.setToExternal(headers, txnInd->no_of_headers);
    httpTxnInd.url = txnInd->url;
    ::android::hardware::Return<void> ret = indCb->UimLpaHttpTxnIndication(httpTxnInd);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    if (headers != NULL)
    {
      delete[] headers;
      headers = NULL;
    }
  } /* UimLpaImpl::uimLpaHttpTxnIndication */

};
template <>
const HalServiceImplVersion& UimLpaImpl<V1_0::IUimLpa>::getVersion();

} // namespace implementation
} // namespace V1_0
} // namespace lpa
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
#endif
