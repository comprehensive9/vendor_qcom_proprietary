/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "AuthModemEndPoint.h"

#include <sstream>

using std::to_string;
constexpr const char *AuthModemEndPoint::NAME;

#ifdef RIL_FOR_DYNAMIC_LOADING
void AuthModemEndPoint::requestSetup(string clientToken, qcril_instance_id_e_type id,
                                      GenericCallback<string>* callback) {
  Log::getInstance().d("[" + mName + "]: requestSetup");
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>(clientToken, 0, nullptr, id, callback);
  mModule->dispatch(shared_setupMsg);
}
#else
void AuthModemEndPoint::requestSetup(string clientToken,
                                      GenericCallback<string>* callback) {
  Log::getInstance().d("[" + mName + "]: requestSetup");
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>(clientToken, 0, nullptr, callback);
  mModule->dispatch(shared_setupMsg);
}
#endif