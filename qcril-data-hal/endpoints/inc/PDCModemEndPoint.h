/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef PDCMODEMENDPOINT
#define PDCMODEMENDPOINT
#include "modules/qmi/ModemEndPoint.h"
#include "PDCModemEndPointModule.h"
#include "framework/Log.h"

#define QCRIL_DATA_QMI_TIMEOUT 10000

namespace rildata {

class PDCModemEndPoint : public ModemEndPoint
{
public:
  static constexpr const char *NAME = "PDCModemEndPoint";
  PDCModemEndPoint() : ModemEndPoint(NAME)
  {
    mModule = new PDCModemEndPointModule("PDCModemEndPointModule", *this);
    mModule->init();
    Log::getInstance().d("[PDCModemEndPoint]: xtor");
  }
  ~PDCModemEndPoint()
  {
    Log::getInstance().d("[PDCModemEndPoint]: destructor");
    delete mModule;
    mModule = nullptr;
  }
  #ifdef RIL_FOR_DYNAMIC_LOADING
  void requestSetup(string clientToken, qcril_instance_id_e_type id, GenericCallback<string>* cb);
  #else
  void requestSetup(string clientToken, GenericCallback<string>* cb);
  #endif
  void registerForPDCIndication();
};

}

#endif
