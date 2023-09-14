/******************************************************************************
  Copyright (c) 2017, 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#ifndef VENDOR_QTI_HARDWARE_RADIO_UIM_LPA_V1_1_H
#define VENDOR_QTI_HARDWARE_RADIO_UIM_LPA_V1_1_H

#include <vendor/qti/hardware/radio/lpa/1.1/IUimLpa.h>
#include <vendor/qti/hardware/radio/lpa/1.1/types.h>
#include <vendor/qti/hardware/radio/lpa/1.1/IUimLpaIndication.h>
#include <vendor/qti/hardware/radio/lpa/1.0/IUimLpaResponse.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "modules/lpa/lpa_service_types.h"

#include "hidl_impl/1.0/lpa_service_1_0.h"

#undef TAG
#define TAG "LpaService"

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_1 {
namespace implementation {

template <typename T>
class UimLpaImpl : public V1_0::implementation::UimLpaImpl<T> {
 private:
     ::android::sp<V1_1::IUimLpaIndication> mIndicationCb_1_1;

     ::android::sp<V1_1::IUimLpaIndication> getIndicationCallback_1_1() {
         std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
         return mIndicationCb_1_1;
     }
 protected:
    void clearCallbacks_nolock() {
        QCRIL_LOG_DEBUG("V2_0::setCallback_nolock");
        mIndicationCb_1_1 = nullptr;
        V1_0::implementation::UimLpaImpl<T>::clearCallbacks_nolock();
    }

    virtual void clearCallbacks() {
        std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
        clearCallbacks_nolock();
    }

    void setCallback_nolock(const ::android::sp<V1_0::IUimLpaResponse>& respCb,
                          const ::android::sp<V1_0::IUimLpaIndication>& indCb) {
        QCRIL_LOG_DEBUG("V2_0::setCallback_nolock");
#ifndef QMI_RIL_UTF
        mIndicationCb_1_1= V1_1::IUimLpaIndication::castFrom(indCb).withDefault(nullptr);
#endif
        V1_0::implementation::UimLpaImpl<T>::setCallback_nolock(respCb, indCb);
    }

 public:
    static const HalServiceImplVersion& getVersion();

    ::android::hardware::Return<void> setCallback(
      const ::android::sp<V1_0::IUimLpaResponse>& respCb,
      const ::android::sp<V1_0::IUimLpaIndication>& indCb) {
      QCRIL_LOG_DEBUG("setCallback");
      std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
      setCallback_nolock(respCb, indCb);
      return ::android::hardware::Void();
    }
    ::android::hardware::Return<void> UimLpaUserRequest_1_1
    (
      int32_t              token,
      const V1_1::UimLpaUserReq& userReq
    )
    {
      lpa_service_user_req_type   user_req;

      memset(&user_req, 0x00, sizeof(user_req));
      QCRIL_LOG_INFO("UimLpaUserRequest1_1: token_id: 0x%x, event: %d", token, userReq.base.event);

      user_req.event = (lpa_service_user_event_type)userReq.base.event;
      user_req.activationCode = userReq.base.activationCode.c_str();
      user_req.confirmationCode = userReq.base.confirmationCode.c_str();
      user_req.nickname = userReq.base.nickname.c_str();
      user_req.iccid    = userReq.base.iccid.data();
      user_req.iccid_len = userReq.base.iccid.size();
      user_req.resetMask = userReq.base.resetMask;
      user_req.userOk = userReq.base.userOk;
      user_req.nok_reason = userReq.nok_reason;
      user_req.srvOpReq.opCode = (lpa_service_srv_addr_op_type)userReq.base.srvOpReq.opCode;
      user_req.srvOpReq.smdpAddress = userReq.base.srvOpReq.smdpAddress.c_str();

     if (this->mModule != nullptr)
      {
        QCRIL_LOG_INFO("mMOdule is not null");
        this->mModule->handleUimLpaUserRequest(token, &user_req);
      }

      return ::android::hardware::Void();
    } /* UimLpaImpl::UimLpaUserRequest */

    void uimLpaAddProfileProgressInd
    (
        lpa_service_add_profile_progress_ind_type * progInd
    )
    {
        ::android::sp<V1_1::IUimLpaIndication> indCb_1_1 = getIndicationCallback_1_1();

        if (progInd == NULL)
        {
          QCRIL_LOG_ERROR("uimLpaAddProfileProgressInd progInd is null");
          return;
        }

        if(indCb_1_1 != NULL)
        {
            V1_1::UimLpaAddProfileProgressIndV1_1  progress;
            memset(&progress, 0x00, sizeof(progress));
            progress.base.status              = (V1_0::UimLpaAddProfileStatus)progInd->status;
            progress.base.cause               = (V1_0::UimLpaAddProfileFailureCause)progInd->cause;
            progress.base.progress            = progInd->progress;
            progress.base.policyMask          = (V1_0::UimLpaProfilePolicyMask)progInd->policyMask;
            progress.profileName              = progInd->profile_name;
            progress.base.userConsentRequired = progInd->userConsentRequired;
            ::android::hardware::Return<void> ret = indCb_1_1->UimLpaAddProfileProgressIndication_1_1(progress);
            if (!ret.isOk()) {
              QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
            }
        }
        else
        {
             V1_0::implementation::UimLpaImpl<T>::uimLpaAddProfileProgressInd(progInd);
        }
    }
  };

  template <>
  const HalServiceImplVersion& UimLpaImpl<V1_1::IUimLpa>::getVersion();


} // namespace implementation
} // namespace V1_1
} // namespace lpa
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor

#endif

