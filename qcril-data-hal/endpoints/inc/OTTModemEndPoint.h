/******************************************************************************

  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

#******************************************************************************/
#ifdef FEATURE_DATA_LQE
#ifndef OTTMODEMENDPOINT
#define OTTMODEMENDPOINT
#include "modules/qmi/ModemEndPoint.h"
#include "OTTModemEndPointModule.h"
#include "framework/Log.h"

class OTTModemEndPoint : public ModemEndPoint
{
public:
  static constexpr const char *NAME = "OTTModemEndPoint";

  OTTModemEndPoint() : ModemEndPoint(NAME) {
    mModule = new OTTModemEndPointModule("OTTModemEndPointModule", *this);
    mModule->init();
    Log::getInstance().d("[OTTModemEndPoint]: xtor");
  }

  ~OTTModemEndPoint() {
    Log::getInstance().d("[OTTModemEndPoint]: destructor");
    delete mModule;
    mModule = nullptr;
  }
#ifdef RIL_FOR_DYNAMIC_LOADING
  void requestSetup(string clientToken, qcril_instance_id_e_type id, GenericCallback<string>* cb);
#else
  void requestSetup(string clientToken, GenericCallback<string>* cb);
#endif
};
#endif /* FEATURE_DATA_LQE */

#endif