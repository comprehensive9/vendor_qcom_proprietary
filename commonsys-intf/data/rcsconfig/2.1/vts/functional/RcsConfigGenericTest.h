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

#define GZIP_WINDOWSIZE 15
#define GZIP_CFACTOR 9
#define GZIP_BSIZE 8096
#define GZIP_OUTPUTBUFFSIZE 32768

using ::android::sp;
using ::vendor::qti::ims::factory::V1_1::IImsFactory;
using ::vendor::qti::ims::rcsconfig::V2_1::IRcsConfig;
using ::vendor::qti::ims::rcsconfig::V2_1::IRcsConfigListener;
using ::vendor::qti::ims::rcsconfig::V2_0::IRcsServiceStatusListener;
using ::vendor::qti::ims::rcsconfig::V2_0::Container;
using ::vendor::qti::ims::rcsconfig::V2_0::ISetConfigCb;
using ::vendor::qti::ims::rcsconfig::V2_0::IGetSettingsCb;
using ::vendor::qti::ims::rcsconfig::V2_0::ISettingsChangeCb;
using ::vendor::qti::ims::rcsconfig::V2_0::RequestStatus;
using ::vendor::qti::ims::rcsconfig::V2_0::SettingsData;
using ::vendor::qti::ims::rcsconfig::V2_0::SettingsId;
using ::vendor::qti::ims::rcsconfig::V2_0::SettingsValues;
using ::vendor::qti::ims::rcsconfig::V2_0::PresenceConfigKeys;
using ::vendor::qti::ims::rcsconfig::V2_0::StandaloneMessagingConfigKeys;
using ::vendor::qti::ims::rcsconfig::V2_0::UserAgentStringKeys;
using ::vendor::qti::ims::rcsconfig::V2_0::ImsServiceEnableConfigKeys;
using ::vendor::qti::ims::rcsconfig::V2_0::KeyValuePairTypeInt;
using ::vendor::qti::ims::rcsconfig::V2_0::KeyValuePairTypeBool;
using ::vendor::qti::ims::rcsconfig::V2_0::KeyValuePairTypeString;
using ::vendor::qti::ims::rcsconfig::V2_0::UceCapabilityInfo;
using ::vendor::qti::ims::rcsconfig::V2_1::TokenType;
using ::vendor::qti::ims::rcsconfig::V2_1::TokenRequestReason;

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

//ISetConfigCb Listener callback function names
constexpr char kCallbacksetConfigResponse[] = "setConfigResponse";
constexpr char kCallbackonReconfigNeeded[] = "onReconfigNeeded";
constexpr char kCallbackGetSettingsResponse[] = "onGetSettingsResponse";
constexpr char kCallbackonUpdateRcsSettingsResponse[] = "onGetUpdatedSettings";
constexpr char kCallbackonUceStatusUpdate[] = "onUceStatusUpdate";
constexpr char kCallbackonTokenFetchRequest[] = "onTokenFetchRequest";

constexpr char slot_id_key[] = "slot_id";
constexpr char isCompressed_key[] = "isCompressed";
constexpr char xmlFileName_key[] = "xmlFile";
constexpr char AppToken_key[] = "app_token";
constexpr char PROPERTIES_FILE[] = "/data/properties.txt";

void loadProperties();


//listener args container definition
class mockSetConfigListenerArgs
{
public:
  RequestStatus status;
  int32_t requestId;
  TokenType tokenType;
  TokenRequestReason reqReason;
};

class mockGetSettingsListenerArgs
{
public:
  RequestStatus status;
  SettingsData data;
};

class mockUpdateUceStatusListenerArgs
{
public:
  UceCapabilityInfo data;
};



class mockRcsConfigListener : public IRcsConfigListener,
                              public ::testing::VtsHalHidlTargetCallbackBase<mockSetConfigListenerArgs>
{
public:
  mockRcsConfigListener() {};
  ~mockRcsConfigListener() {};

  Return<void> onReconfigNeeded()
  {
    ALOGI("%s - :onReconfigNeeded: received","mockRcsConfigListener");
    NotifyFromCallback(kCallbackonReconfigNeeded);
    return Void();
  }

  Return<void> onTokenFetchRequest(int32_t requestId, TokenType tokenType, TokenRequestReason reqReason)
  {
    ALOGI("%s - :onTokenFetchRequest: received","mockRcsConfigListener");
    mockSetConfigListenerArgs args;
    args.requestId = requestId;
    args.tokenType = tokenType;
    args.reqReason = reqReason;
    NotifyFromCallback(kCallbackonTokenFetchRequest, args);
    return Void();
  }
};

class mockSetConfigListener : public ISetConfigCb,
                              public ::testing::VtsHalHidlTargetCallbackBase<mockSetConfigListenerArgs>
{
public:
  mockSetConfigListener() {};
  virtual ~mockSetConfigListener() {};


  Return<void> setConfigResponse(RequestStatus status)
  {
    ALOGI("%s - :onServiceReady: received","ImsCmServiceListener_ut");
    mockSetConfigListenerArgs args;
    args.status = status;
    NotifyFromCallback(kCallbacksetConfigResponse, args);
    return Void();
  }
};

class mockGetSettingsListener : public IGetSettingsCb,
                              public ::testing::VtsHalHidlTargetCallbackBase<mockGetSettingsListenerArgs>
{
public:
  mockGetSettingsListener() {};
  virtual ~mockGetSettingsListener() {};


  Return<void> onGetSettingsResponse(RequestStatus status,const SettingsData& data) {
    ALOGI("%s - :onServiceReady: received","ImsCmServiceListener_ut");
    mockGetSettingsListenerArgs args;
    args.data = data;
    args.status = status;
    NotifyFromCallback(kCallbackGetSettingsResponse, args);
    return Void();
  }
};

class mockUpdateRcsSettingsListener : public ISettingsChangeCb,
                                      public ::testing::VtsHalHidlTargetCallbackBase<mockGetSettingsListenerArgs>
{
public:
  mockUpdateRcsSettingsListener() {};
  virtual ~mockUpdateRcsSettingsListener() {};

  Return<void> onGetUpdatedSettings(const SettingsData& cbdata)
  {
    ALOGI("%s - :onGetUpdatedSettings: received","ImsCmServiceListener_ut");
    mockGetSettingsListenerArgs args;
    args.data = cbdata;
    args.status=RequestStatus::OK;
    NotifyFromCallback(kCallbackonUpdateRcsSettingsResponse, args);
    return Void();
  }
};

class mockUpdateUceStatusListener : public IRcsServiceStatusListener,
                                      public ::testing::VtsHalHidlTargetCallbackBase<mockUpdateUceStatusListenerArgs>
{
public:
  mockUpdateUceStatusListener() {};
  virtual ~mockUpdateUceStatusListener() {};

  Return<void> onUceStatusUpdate(const UceCapabilityInfo& cbdata)
  {
    ALOGI("%s - :onUceStatusUpdate: received","ImsCmServiceListener_ut");
    mockUpdateUceStatusListenerArgs args;
    args.data.isPresenceEnabled = cbdata.isPresenceEnabled;
    ALOGI("RcsConfigVTS: onUceStatusUpdate - isPresenceEnabled %d", args.data.isPresenceEnabled);
    args.data.isOptionsEnabled = cbdata.isOptionsEnabled;
    ALOGI("RcsConfigVTS: onUceStatusUpdate - isOptionsEnabled %d", args.data.isOptionsEnabled);
    //args.status=RequestStatus::OK;
    NotifyFromCallback(kCallbackonUceStatusUpdate, args);
    return Void();
  }
};


class rcsConfigVtsTestBase
{
public:
  rcsConfigVtsTestBase() {};
  ~rcsConfigVtsTestBase() {};
  static std::map<std::string, std::string> g_configValues;
  static SettingsData presenceSettings;
  static SettingsData standAloneMsgSettings;
  static SettingsData userAgentStringSettings;
  static SettingsData imsServiceEnableConfig;
};

class RcsConfigGenericTest : public ::testing::VtsHalHidlTargetTestBase,
                             public rcsConfigVtsTestBase
{
public:
  virtual void SetUp() override;
  sp<IImsFactory> factory = nullptr;
  sp<IRcsConfig> pService = nullptr;
  mockSetConfigListener* mListener;
  mockGetSettingsListener* mGetListener;
  mockRcsConfigListener* mRcsConfigListener;
  mockUpdateRcsSettingsListener* mUpdateSettingListener;
  mockUpdateUceStatusListener* mUceStatusListener;
};