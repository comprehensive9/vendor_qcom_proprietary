/******************************************************************************
#  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef IWLANMODULE
#define IWLANMODULE
#include "framework/Module.h"
#include "IWlanImpl.h"
#include "IWlanDataRegistrationStateChangeIndMessage.h"
#include "IWlanDataCallListChangeIndMessage.h"
#include "QualifiedNetworksChangeIndMessage.h"
#include "request/SetupDataCallRequestMessage.h"
#include "request/DeactivateDataCallRequestMessage.h"

using namespace rildata;


class IWlanModule : public Module {
  public:
    IWlanModule();
    ~IWlanModule();
    void init();

  private:
      std::string readProperty(std::string name, string defaultValue);
      void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
      void handleDataRegistrationStateChange(std::shared_ptr<IWlanDataRegistrationStateChangeIndMessage> msg);
      void handleDataCallListChange(std::shared_ptr<IWlanDataCallListChangeIndMessage> msg);
      void handleQualifiedNetworksChange(std::shared_ptr<QualifiedNetworksChangeIndMessage> msg);
      void handleSetupDataCallIWlanResponseIndMessage(std::shared_ptr<SetupDataCallIWlanResponseIndMessage> msg);
      void handleDeactivateDataCallIWlanResponseIndMessage(std::shared_ptr<DeactivateDataCallIWlanResponseIndMessage> msg);
};



#endif
