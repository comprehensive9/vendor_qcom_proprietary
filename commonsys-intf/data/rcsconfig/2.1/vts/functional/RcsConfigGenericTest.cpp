/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include "RcsConfigGenericTest.h"

std::map<std::string, std::string> rcsConfigVtsTestBase::g_configValues;
SettingsData rcsConfigVtsTestBase::presenceSettings;
SettingsData rcsConfigVtsTestBase::standAloneMsgSettings;
SettingsData rcsConfigVtsTestBase::userAgentStringSettings;
SettingsData rcsConfigVtsTestBase::imsServiceEnableConfig;
void loadProperties()
{
  std::string line;
  std::ifstream fileStream;
  fileStream.open(PROPERTIES_FILE, std::ifstream::in);
  while (std::getline(fileStream, line))
  {
    std::istringstream is_line(line);
    std::string key;
    if (std::getline(is_line, key, '='))
    {
      std::string value = "";
      if (key[0] == '#')
        continue;

      if (std::getline(is_line, value))
      {
        std::cout << "key [" << key << "] value[" << value << "]\n";
        ALOGI("key [%s], value[%s]", key.c_str(), value.c_str());
        rcsConfigVtsTestBase::g_configValues[key] = value;
      }
    }
  }
  fileStream.close();
}


void RcsConfigGenericTest::SetUp()
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
  else
  {
    ALOGI("Setup imsfactory is NULL");
  }

  mListener = new mockSetConfigListener();
  mGetListener = new mockGetSettingsListener();
  mUpdateSettingListener = new mockUpdateRcsSettingsListener();
  mUceStatusListener = new mockUpdateUceStatusListener();
}

TEST_F(RcsConfigGenericTest, ServiceAvailableTest) {
  IImsFactory::StatusCode status = IImsFactory::StatusCode::NOT_SUPPORTED;
  sp<IImsFactory> factory = nullptr;
  sp<IRcsConfig> service = nullptr;

  factory = IImsFactory::getService();

  if(factory != nullptr)
  {
    factory->createRcsConfig_1_1(atoi(g_configValues[slot_id_key].c_str()),
              mRcsConfigListener,
              [&](IImsFactory::StatusCode _status, sp<IRcsConfig> config) {
                    status = _status;
                    service = config;
              /* end Lamda Func*/ } );
      if(service)
      ASSERT_NE(nullptr, service.get()) << "Could not get Service";
  }
  else
  {
    ALOGI("No service object received");
    //ASSERT_NE(nullptr, service.get()) << "Could not get Service";
  }
  ALOGI("Things worked as expected");
}




TEST_F(RcsConfigGenericTest, GetUceStatusTest) {

  if(pService != nullptr)
  {
    auto result = pService->getUceStatus(mUceStatusListener);
    auto res = mUceStatusListener->WaitForCallback(kCallbackonUceStatusUpdate);
    EXPECT_TRUE(res.no_timeout);
    // EXPECT_TRUE(res.args->data.isPresenceEnabled);
    // EXPECT_TRUE(res.args->data.isOptionsEnabled);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, GetSettingsDataPresenceTest) {

  if(pService != nullptr)
  {
    auto result = pService->getSettingsValue(SettingsId::PRESENCE_CONFIG, mGetListener);
    auto res = mGetListener->WaitForCallback(kCallbackGetSettingsResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);
    EXPECT_EQ(SettingsId::PRESENCE_CONFIG, res.args->data.settingsId);
    presenceSettings = res.args->data;
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, GetSettingsDataStandaloneTest) {

  if(pService != nullptr)
  {
    auto result = pService->getSettingsValue(SettingsId::STANDALONE_MESSAGING_CONFIG, mGetListener);
    auto res = mGetListener->WaitForCallback(kCallbackGetSettingsResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);
    EXPECT_EQ(SettingsId::STANDALONE_MESSAGING_CONFIG, res.args->data.settingsId);
    standAloneMsgSettings = res.args->data;
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, GetSettingsDataUserAgentTest) {

  if(pService != nullptr)
  {
    auto result = pService->getSettingsValue(SettingsId::USER_AGENT_STRING, mGetListener);
    auto res = mGetListener->WaitForCallback(kCallbackGetSettingsResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);
    EXPECT_EQ(SettingsId::USER_AGENT_STRING, res.args->data.settingsId);
    userAgentStringSettings = res.args->data;
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, GetSettingsDataImsServiceTest) {

  if(pService != nullptr)
  {
    auto result = pService->getSettingsValue(SettingsId::IMS_SERVICE_ENABLE_CONFIG, mGetListener);
    auto res = mGetListener->WaitForCallback(kCallbackGetSettingsResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);
    EXPECT_EQ(SettingsId::IMS_SERVICE_ENABLE_CONFIG, res.args->data.settingsId);
    imsServiceEnableConfig = res.args->data;
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, RegisterForImsServiceEnableIndications) {
  ALOGE("in vts register for indications");

  if(pService != nullptr)
  {
    ALOGE("calling RegisterForIndications");
    uint32_t result = pService->registerForSettingsChange(SettingsId::IMS_SERVICE_ENABLE_CONFIG, mUpdateSettingListener);
   // auto res = mUpdateSettingListener->WaitForCallback(kCallbackonUpdateRcsSettingsResponse);
   // EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK,(RequestStatus)result);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, RegisterForPresenceIndications) {
  ALOGE("in vts RegisterForPresenceIndications");

  if(pService != nullptr)
  {
    ALOGE("calling RegisterForPresenceIndications");
    uint32_t result = pService->registerForSettingsChange(SettingsId::PRESENCE_CONFIG, mUpdateSettingListener);
    EXPECT_EQ(RequestStatus::OK,(RequestStatus)result);
   // auto res = mUpdateSettingListener->WaitForCallback(kCallbackonUpdateRcsSettingsResponse);
   // EXPECT_TRUE(res.no_timeout);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, RegisterForUserAgentIndications) {
  ALOGE("in vts RegisterForUserAgentIndications");

  if(pService != nullptr)
  {
    ALOGE("calling RegisterForUserAgentIndications");
    uint32_t result = pService->registerForSettingsChange(SettingsId::USER_AGENT_STRING, mUpdateSettingListener);
    EXPECT_EQ(RequestStatus::OK,(RequestStatus)result);
    //auto res = mUpdateSettingListener->WaitForCallback(kCallbackonUpdateRcsSettingsResponse);
    //EXPECT_TRUE(res.no_timeout);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, RegisterForRcsSmConfigIndications) {
  ALOGE("in vts RegisterForRcsSmConfigIndications");

  if(pService != nullptr)
  {
    ALOGE("calling RegisterForRcsSmConfigIndications");
    uint32_t result = pService->registerForSettingsChange(SettingsId::STANDALONE_MESSAGING_CONFIG, mUpdateSettingListener);
    EXPECT_EQ(RequestStatus::OK,(RequestStatus)result);
    //auto res = mUpdateSettingListener->WaitForCallback(kCallbackonUpdateRcsSettingsResponse);
    //EXPECT_TRUE(res.no_timeout);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, SetSettingsDataPresenceConfigTest) {
  ALOGE("in vts SetSettingsDataPresenceConfigTest rcs settings");
  SettingsData data;

  data = presenceSettings;
  data.settingsId = SettingsId::PRESENCE_CONFIG;

  if(pService != nullptr)
  {
    ALOGE("calling set rcs settings");
    auto result = pService->setSettingsValue(data, mListener);
    auto res = mListener->WaitForCallback(kCallbacksetConfigResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);

  }
  else
  {
     ALOGE("This functionality is not supported");

  }

}

TEST_F(RcsConfigGenericTest, SetSettingsDataStandaloneMessagingTest) {
  ALOGE("in vts SetSettingsDataStandaloneMessagingTest rcs settings");
  SettingsData data;

  //settingsValues.stringData = stringDataVec;
  data = standAloneMsgSettings;
  data.settingsId = SettingsId::STANDALONE_MESSAGING_CONFIG;

  if(pService != nullptr)
  {
    ALOGE("calling set rcs settings");
  auto result = pService->setSettingsValue(data, mListener);
    //sleep(5);
  auto res = mListener->WaitForCallback(kCallbacksetConfigResponse);
  EXPECT_TRUE(res.no_timeout);
  EXPECT_EQ(RequestStatus::OK, res.args->status);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

/** This Test is not Required as it overrids Modem Config
TEST_F(RcsConfigGenericTest, SetSettingsDataUserAgentTestEmptyValTest) {
  ALOGE("in vts SetSettingsDataUserAgentTestEmptyValTest rcs settings");
  SettingsData data;
  SettingsValues settingsValues;

  data.settingsValues = settingsValues;
  data.settingsId = SettingsId::USER_AGENT_STRING;

  if(pService != nullptr)
  {
    ALOGE("calling set rcs settings");
    auto result = pService->setSettingsValue(data, mListener);
    auto res = mListener->WaitForCallback(kCallbacksetConfigResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::FAIL, res.args->status);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

} */

TEST_F(RcsConfigGenericTest, SetSettingsDataUserAgentTest) {
  ALOGE("in vts SetSettingsDataUserAgentTest rcs settings");
  SettingsData data;
  data =  userAgentStringSettings;
  data.settingsId = SettingsId::USER_AGENT_STRING;

  if(pService != nullptr)
  {
    ALOGE("calling set rcs settings");
  auto result = pService->setSettingsValue(data, mListener);
  auto res = mListener->WaitForCallback(kCallbacksetConfigResponse);
  EXPECT_TRUE(res.no_timeout);
  EXPECT_EQ(RequestStatus::OK, res.args->status);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, SetSettingsDataImsServiceEnableTest) {
  ALOGE("in vts set rcs settings.. sleeping for sometime");
  //sleep(15);
  SettingsData data;
  data = imsServiceEnableConfig;
  data.settingsId = SettingsId::IMS_SERVICE_ENABLE_CONFIG;

  if(pService != nullptr)
  {
    ALOGE("calling set rcs settings");
    auto result = pService->setSettingsValue(data, mListener);
    auto res = mListener->WaitForCallback(kCallbacksetConfigResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}


TEST_F(RcsConfigGenericTest, GetSettingsDataPresenceTest1) {

  if(pService != nullptr)
  {
    auto result = pService->getSettingsValue(SettingsId::PRESENCE_CONFIG, mGetListener);
    auto res = mGetListener->WaitForCallback(kCallbackGetSettingsResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);
    EXPECT_EQ(SettingsId::PRESENCE_CONFIG, res.args->data.settingsId);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, GetSettingsDataStandaloneTest1) {

  if(pService != nullptr)
  {
    auto result = pService->getSettingsValue(SettingsId::STANDALONE_MESSAGING_CONFIG, mGetListener);
    auto res = mGetListener->WaitForCallback(kCallbackGetSettingsResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);
    EXPECT_EQ(SettingsId::STANDALONE_MESSAGING_CONFIG, res.args->data.settingsId);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, GetSettingsDataUserAgentTest1) {

  if(pService != nullptr)
  {
    auto result = pService->getSettingsValue(SettingsId::USER_AGENT_STRING, mGetListener);
    auto res = mGetListener->WaitForCallback(kCallbackGetSettingsResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);
    EXPECT_EQ(SettingsId::USER_AGENT_STRING, res.args->data.settingsId);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, GetSettingsDataImsServiceTest1) {

  if(pService != nullptr)
  {
    auto result = pService->getSettingsValue(SettingsId::IMS_SERVICE_ENABLE_CONFIG, mGetListener);
    auto res = mGetListener->WaitForCallback(kCallbackGetSettingsResponse);
  EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);
    EXPECT_EQ(SettingsId::IMS_SERVICE_ENABLE_CONFIG, res.args->data.settingsId);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, DeRegisterForPresenceIndications) {
  ALOGE("in vts DeRegisterForPresenceIndications");

  if(pService != nullptr)
  {
    ALOGE("calling DeRegisterForPresenceIndications");
    uint32_t result = pService->deregisterForSettingsChange(SettingsId::PRESENCE_CONFIG);
    EXPECT_EQ(RequestStatus::OK,(RequestStatus)result);
  }
  else
  {
     ALOGE("This functionality is not supported");
  }

}

TEST_F(RcsConfigGenericTest, SetSettingsDataPresenceConfigTestAfterDeregister) {
  ALOGE("in vts SetSettingsDataPresenceConfigTestAfterDeregister rcs settings");
  SettingsData data;
  SettingsValues settingsValues;

  vector<KeyValuePairTypeInt> intDataVec;
  vector<KeyValuePairTypeBool> boolDataVec;
  vector<KeyValuePairTypeString> stringDataVec;

  intDataVec.push_back(KeyValuePairTypeInt());
  intDataVec[0].key = (uint32_t)PresenceConfigKeys::PUBLISH_TIMER_KEY;
  intDataVec[0].value = 100;

  intDataVec.push_back(KeyValuePairTypeInt());
  intDataVec[1].key = (uint32_t)PresenceConfigKeys::AVAILABILITY_CACHE_EXPIRY_KEY;
  intDataVec[1].value = 50;

  intDataVec.push_back(KeyValuePairTypeInt());
  intDataVec[2].key = (uint32_t)PresenceConfigKeys::PUBLISH_EXTENDED_TIMER_KEY;
  intDataVec[2].value = 50;

  intDataVec.push_back(KeyValuePairTypeInt());
  intDataVec[3].key = (uint32_t)PresenceConfigKeys::PUBLISH_SRC_THROTTLE_TIMER_KEY;
  intDataVec[3].value = 50;

  intDataVec.push_back(KeyValuePairTypeInt());
  intDataVec[4].key = (uint32_t)PresenceConfigKeys::PUBLISH_ERROR_RECOVERY_TIMER_KEY;
  intDataVec[4].value = 50;

  intDataVec.push_back(KeyValuePairTypeInt());
  intDataVec[5].key = (uint32_t)PresenceConfigKeys::LIST_SUBSCRIPTION_EXPIRY_KEY;
  intDataVec[5].value = 50;

  intDataVec.push_back(KeyValuePairTypeInt());
  intDataVec[6].key = (uint32_t)PresenceConfigKeys::CAPABILITES_CACHE_EXPIRY_KEY;
  intDataVec[6].value = 50;

  intDataVec.push_back(KeyValuePairTypeInt());
  intDataVec[7].key = (uint32_t)PresenceConfigKeys::CAPABILITY_POLL_INTERVAL_KEY;
  intDataVec[7].value = 50;

  intDataVec.push_back(KeyValuePairTypeInt());
  intDataVec[8].key = (uint32_t)PresenceConfigKeys::MAX_ENTIES_IN_LIST_SUBSCRIBE_KEY;
  intDataVec[8].value = 50;

  boolDataVec.push_back(KeyValuePairTypeBool());
  boolDataVec[0].key = (uint32_t)PresenceConfigKeys::CAPABILITY_DISCOVERY_ENABLED_KEY;
  boolDataVec[0].value = false;

  boolDataVec.push_back(KeyValuePairTypeBool());
  boolDataVec[1].key = (uint32_t)PresenceConfigKeys::GZIP_ENABLED_KEY;
  boolDataVec[1].value = true;

  stringDataVec.push_back(KeyValuePairTypeString());
  stringDataVec[0].key = (uint32_t)PresenceConfigKeys::USER_AGENT_KEY;
  stringDataVec[0].value = "Presence_config";

  settingsValues.intData = intDataVec;
  settingsValues.boolData = boolDataVec;
  settingsValues.stringData = stringDataVec;
  data.settingsValues = settingsValues;
  data.settingsId = SettingsId::PRESENCE_CONFIG;

  if(pService != nullptr)
  {
    ALOGE("calling set rcs settings");
    auto result = pService->setSettingsValue(data, mListener);
    auto res = mListener->WaitForCallback(kCallbacksetConfigResponse);
    EXPECT_TRUE(res.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res.args->status);

  }
  else
  {
     ALOGE("This functionality is not supported");

  }

}

int main(int argc, char** argv)
{
  //loading properties
  cout << "Please double check you have a carrier config xml file and a properties.txt file";
  cout << " in the same location as this test binary" << endl;
  loadProperties();
  ::testing::InitGoogleTest(&argc, argv);
  int status = RUN_ALL_TESTS();
  ALOGE("Test result with status=%d", status);
  return status;
}
