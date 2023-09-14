/******************************************************************************
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#ifndef _QCRIL_QMI_LPA_SERVICE_BASE_H_
#define _QCRIL_QMI_LPA_SERVICE_BASE_H_

#include <QtiMutex.h>
#include <framework/legacy.h>
#include <hidl/HidlSupport.h>

#include "HalServiceImplFactory.h"
#include "modules/lpa/lpa_service_types.h"
#include "modules/lpa/LpaModule.h"

class LpaModule;
class LpaServiceBase : public ::android::hardware::hidl_death_recipient {
 private:
  qcril_instance_id_e_type mInstanceId;

 protected:
  qtimutex::QtiSharedMutex mCallbackLock;

  /**
   * Clean up the callback handlers.
   */
  virtual void clearCallbacks() = 0;

  /**
   * Handler function for hidl death notification.
   * From hidl_death_recipient
   */
  void serviceDied(uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase>&);

  // Utils APIs


  /**
   * Set instance id
   */
  void setInstanceId(qcril_instance_id_e_type instId);

  /**
   * Returns the instance id
   */
  qcril_instance_id_e_type getInstanceId();

 public:

  LpaServiceBase() = default;

  virtual ~LpaServiceBase() = default;

  LpaModule *mModule;
  /**
   * Register the latest version of the service.
   */
  virtual bool registerService(qcril_instance_id_e_type instId) = 0;

  // Indicaiton APIs

  // Indicaiton APIs
  // V1_0
  virtual void uimLpaAddProfileProgressInd(lpa_service_add_profile_progress_ind_type * /*progInd*/) {
  }

  virtual void uimLpaHttpTxnIndication(lpa_service_http_transaction_ind_type * /*txnInd*/) {
  }
  // Response APIs

  virtual void uimLpaHttpTxnCompletedResponse (int32_t /*token*/,lpa_service_result_type /*result*/) {
  }
  virtual void uimLpaUserResponse (int32_t /*token*/,lpa_service_user_resp_type * /*user_resp*/) {
  }


};
#endif  // _QCRIL_QMI_QTI_RADIO_SERVICE_BASE_H_
