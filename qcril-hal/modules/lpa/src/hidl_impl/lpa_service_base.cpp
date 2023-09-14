/******************************************************************************
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
 ******************************************************************************/

#include "framework/Log.h"
#include "lpa_service_base.h"

#undef TAG
#define TAG "LpaServiceBase"

/**
 * Handler function for hidl death notification.
 * From hidl_death_recipient
 */
void LpaServiceBase::serviceDied(uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase>&) {
  QCRIL_LOG_DEBUG("serviceDied");
  clearCallbacks();
}

// Utils APIs

/**
 * Set instance id
 */
void LpaServiceBase::setInstanceId(qcril_instance_id_e_type instId) {
  mInstanceId = instId;
}

/**
 * Returns the instance id
 */
qcril_instance_id_e_type LpaServiceBase::getInstanceId() {
  return mInstanceId;
}

