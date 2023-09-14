/******************************************************************************
#  Copyright (c) 2017 , 2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include "UnSolMessages/DataRegistrationStateMessage.h"
#include "UnSolMessages/DataNrIconTypeIndMessage.h"
#include "UnSolMessages/QosDataChangedIndMessage.h"
#include "interfaces/nas/Nas5gConnectionIndMessage.h"
#include "interfaces/nas/Nas5gStatusIndMessage.h"
#include "interfaces/nas/NasEndcDcnrIndMessage.h"
#include "interfaces/nas/Nas5gSignalStrengthIndMessage.h"
#include "interfaces/nas/Nas5gConfigInfoIndMessage.h"
#include "interfaces/nas/NasUpperLayerIndInfoIndMessage.h"

class QtiRadio : public Module {
  public:
    QtiRadio();
    ~QtiRadio();
    void init();

  private:
      void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
      void handleNas5gConnectionIndMessage(
              std::shared_ptr<Nas5gConnectionIndMessage> msg);
      void handleNas5gStatusIndMessage(
              std::shared_ptr<Nas5gStatusIndMessage> msg);
      void handleNasEndcDcnrIndMessage(
              std::shared_ptr<NasEndcDcnrIndMessage> msg);
      void handleNas5gSignalStrengthIndMessage(
              std::shared_ptr<Nas5gSignalStrengthIndMessage> msg);
      void handleNas5gConfigInfoIndMessage(
              std::shared_ptr<Nas5gConfigInfoIndMessage> msg);
      void handleNasUpperLayerIndInfoIndMessage(
              std::shared_ptr<NasUpperLayerIndInfoIndMessage> msg);
      void handleQosDataChangedIndMessage(
              std::shared_ptr<rildata::QosDataChangedIndMessage> msg);
#ifndef QMI_RIL_UTF
      void handleDataNrIconTypeIndMessage(
              std::shared_ptr<rildata::DataNrIconTypeIndMessage> msg);
#endif
};


