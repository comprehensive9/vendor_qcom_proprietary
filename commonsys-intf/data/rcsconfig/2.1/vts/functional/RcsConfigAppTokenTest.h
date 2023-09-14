/*****************************************************************************
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#define LOG_TAG "RcsConfig_V2_VTS"
#include <VtsHalHidlTargetTestBase.h>
#include <VtsHalHidlTargetCallbackBase.h>
#include <log/log.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <zlib.h>
#include <vendor/qti/ims/factory/1.1/IImsFactory.h>
#include <vendor/qti/ims/rcsconfig/2.1/IRcsConfig.h>
#include "RcsConfigGenericTest.h"


class RcsConfigAppTokenTest : public ::testing::VtsHalHidlTargetTestBase,
                              public rcsConfigVtsTestBase
{
public:
  virtual void SetUp() override
  {
    ALOGI("Setup function");
    factory = IImsFactory::getService();
    IImsFactory::StatusCode status = IImsFactory::StatusCode::NOT_SUPPORTED;
    mRcsConfigListener = new mockRcsConfigListener();
    if(factory != nullptr)
    {
      for(auto it : g_configValues)
      {
        std::cout << "Setup g_configValues [" << it.first << "] value[" << it.second << "]\n";
        ALOGI("Setup g_configValues[%s][%s]", it.first.c_str(), it.second.c_str() );
      }
      int testI = atoi(g_configValues[slot_id_key].c_str());
      ALOGI("Setup testI[%d]", testI);
      std::cout << "Setup testI[" << testI <<"]";
      factory->createRcsConfig_1_1(atoi(g_configValues[slot_id_key].c_str()),
              mRcsConfigListener,
              [&](IImsFactory::StatusCode _status, sp<IRcsConfig> config) {
                    status = _status;
                    pService = config;
              /* end Lamda Func*/ } );

    }
    mListener = new mockSetConfigListener();
    mGetListener = new mockGetSettingsListener();
    mUpdateSettingListener = new mockUpdateRcsSettingsListener();
    mUceStatusListener = new mockUpdateUceStatusListener();
  };
  sp<IImsFactory> factory = nullptr;
  sp<IRcsConfig> pService = nullptr;
  mockSetConfigListener* mListener;
  mockGetSettingsListener* mGetListener;
  mockRcsConfigListener* mRcsConfigListener;
  mockUpdateRcsSettingsListener* mUpdateSettingListener;
  mockUpdateUceStatusListener* mUceStatusListener;
};