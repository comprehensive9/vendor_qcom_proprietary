/******************************************************************************
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#ifndef VENDOR_QTI_HARDWARE_RADIO_UIM_LPA_V1_2_H
#define VENDOR_QTI_HARDWARE_RADIO_UIM_LPA_V1_2_H

#include <vendor/qti/hardware/radio/lpa/1.2/IUimLpa.h>
#include <vendor/qti/hardware/radio/lpa/1.2/types.h>
#include <vendor/qti/hardware/radio/lpa/1.1/IUimLpaIndication.h>
#include <vendor/qti/hardware/radio/lpa/1.2/IUimLpaResponse.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "modules/lpa/lpa_service_types.h"

#include "hidl_impl/1.1/lpa_service_1_1.h"

#undef TAG
#define TAG "LpaService"

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_2 {
namespace implementation {

template <typename T>
class UimLpaImpl : public V1_1::implementation::UimLpaImpl<T> {
 private:
     ::android::sp<V1_2::IUimLpaResponse> mResponseCb_1_2 = NULL;

    ::android::sp<V1_2::IUimLpaResponse> getResponseCallback1_2() {
        std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
        return mResponseCb_1_2;
    }
 protected:
    void clearCallbacks_nolock() {
        QCRIL_LOG_DEBUG("V1_2::clearCallback_nolock");
        mResponseCb_1_2 = nullptr;
        V1_1::implementation::UimLpaImpl<T>::clearCallbacks_nolock();
    }

    virtual void clearCallbacks() {
        std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
        clearCallbacks_nolock();
    }

    void setCallback_nolock(const ::android::sp<V1_0::IUimLpaResponse>& respCb,
                          const ::android::sp<V1_0::IUimLpaIndication>& indCb) {
        QCRIL_LOG_DEBUG("V1_2::setCallback_nolock");
#ifndef QMI_RIL_UTF
        mResponseCb_1_2 = V1_2::IUimLpaResponse::castFrom(respCb).withDefault(nullptr);
        if(!mResponseCb_1_2) {
            QCRIL_LOG_ERROR("mResponseCb_1_2 is null");
        }
#endif
        V1_1::implementation::UimLpaImpl<T>::setCallback_nolock(respCb, indCb);
    }

   virtual void uimLpaUserResponse
   (
       int32_t                         token,
       lpa_service_user_resp_type    * user_resp
   )
   {
       V1_0::UimLpaProfileInfo  * profiles = NULL;
       uint8_t              p_count  = 0;

       ::android::sp<V1_2::IUimLpaResponse> respCbV1_2 = getResponseCallback1_2();

       QCRIL_LOG_INFO("UimRemoteClientImpl::uimRemoteClientEventResponse token=%d event=%d", token, user_resp->event);

       if(respCbV1_2 != NULL) {
           V1_2::UimLpaUserRespV1_2 userResp;
           if (user_resp == NULL)
           {
             QCRIL_LOG_ERROR("UimLpaImpl::uimLpaUserResponse userResp NULL");
             return;
           }

           memset(&userResp, 0x00, sizeof(userResp));

           userResp.base.event = (V1_0::UimLpaUserEventId)user_resp->event;
           userResp.base.result = (V1_0::UimLpaResult)user_resp->result;
           userResp.base.eid.setToExternal(user_resp->eid, user_resp->eid_len);

           if (user_resp->no_of_profiles != 0 && user_resp->profiles != NULL)
           {
             profiles = new V1_0::UimLpaProfileInfo[user_resp->no_of_profiles];

             if (profiles == NULL)
             {
               QCRIL_LOG_ERROR("Allocation failed");
               return;
             }

             for (p_count = 0; p_count < user_resp->no_of_profiles; p_count++)
             {
               profiles[p_count].state = (V1_0::UimLpaProfileState)user_resp->profiles[p_count].state;
               profiles[p_count].iccid.setToExternal(
                          user_resp->profiles[p_count].iccid,
                          user_resp->profiles[p_count].iccid_len);

               profiles[p_count].profileName = user_resp->profiles[p_count].profileName;
               profiles[p_count].nickName = user_resp->profiles[p_count].nickName;
               profiles[p_count].spName = user_resp->profiles[p_count].spName;
               profiles[p_count].iconType =
                  (V1_0::UimLpaIconType)user_resp->profiles[p_count].iconType;
               profiles[p_count].icon.setToExternal(
                  user_resp->profiles[p_count].icon,
                  user_resp->profiles[p_count].icon_len);
               profiles[p_count].profileClass =
                 (V1_0::UimLpaProfileClassType)user_resp->profiles[p_count].profileClass;
               profiles[p_count].profilePolicy =
                 (V1_0::UimLpaProfilePolicyMask)user_resp->profiles[p_count].profilePolicy;
             }
           }
           userResp.base.profiles.setToExternal(profiles, user_resp->no_of_profiles);
           userResp.base.srvAddr.smdpAddress = user_resp->srvAddr.smdpAddress;
           userResp.base.srvAddr.smdsAddress = user_resp->srvAddr.smdsAddress;
           userResp.euicc_info2.setToExternal(user_resp->euicc_info2,user_resp->euicc_info2_len);
          ::android::hardware::Return<void> ret = respCbV1_2->UimLpaUserResponse_1_2(token, userResp);
           if (!ret.isOk()) {
             QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
           }
           if (profiles != NULL)
           {
               delete[] profiles;
               profiles = NULL;
           }
       }
       else {
           V1_1::implementation::UimLpaImpl<T>::uimLpaUserResponse(token,user_resp);
       }
   } /* UimLpaImpl::uimLpaUserResponse */

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

    ::android::hardware::Return<void> dummy ()
    {
       return ::android::hardware::Void();
    }
  };

  template <>
  const HalServiceImplVersion& UimLpaImpl<V1_2::IUimLpa>::getVersion();

} // namespace implementation
} // namespace V1_2
} // namespace lpa
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor

#endif

