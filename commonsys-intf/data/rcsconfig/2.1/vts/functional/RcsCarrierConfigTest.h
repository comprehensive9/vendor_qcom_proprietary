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



class RcsCarrierConfigTest : public ::testing::VtsHalHidlTargetTestBase,
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

  std::string compressFileData(bool compressData);
  sp<IImsFactory> factory = nullptr;
  sp<IRcsConfig> pService = nullptr;
  mockSetConfigListener* mListener;
  mockGetSettingsListener* mGetListener;
  mockRcsConfigListener* mRcsConfigListener;
  mockUpdateRcsSettingsListener* mUpdateSettingListener;
  mockUpdateUceStatusListener* mUceStatusListener;
};