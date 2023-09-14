/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/nas/qcril_qmi_nas.h"
#include "framework/Log.h"
#include "framework/legacy.h"
#include "modules/nas/NasGetMccMncCallback.h"
#include <QtiMutex.h>

void qcril_uim_process_mcc_mnc_info (
    std::shared_ptr<qcril_uim_mcc_mnc_info_type> uim_mcc_mnc_info);

void NasGetMccMncCallback::onResponse(
    std::shared_ptr<Message> solicitedMsg, Status status,
    std::shared_ptr<qcril_uim_mcc_mnc_info_type> responseDataPtr) {
  mStatus = status;
  if (mStatus == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[NasGetMccMncCallback]: Callback[msg = " +
        solicitedMsg->dump() + "] executed. client data = " +
        mClientToken + " status = Message::Callback::Status::SUCCESS");
    std::shared_ptr<qcril_uim_mcc_mnc_info_type> resp =
          std::static_pointer_cast<qcril_uim_mcc_mnc_info_type>(responseDataPtr);
    if(resp) {
      qcril_uim_process_mcc_mnc_info(resp);
    } else {
      Log::getInstance().d("[NasGetMccMncCallback]: resp is NULL..not processing ");
    }
} else {
    Log::getInstance().d("[NasGetMccMncCallback]: Callback[msg = " +
        solicitedMsg->dump() + "] executed. client data = " +
        mClientToken + " FAILURE !!");
  }
}

Message::Callback::Status NasGetMccMncCallback::getStatus() {
  if (mStatus == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[NasGetMccMncCallback]: Message::Callback::Status::SUCCESS");
  } else {
    Log::getInstance().d("[NasGetMccMncCallback]: not SUCCESS");
  }
  return mStatus;
}

Message::Callback *NasGetMccMncCallback::clone() {
  NasGetMccMncCallback *clone = new NasGetMccMncCallback(mClientToken);
  return clone;
}
