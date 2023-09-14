/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "CallCapability_V1_0_VTS"
#include <VtsHalHidlTargetTestBase.h>
#include <VtsHalHidlTargetCallbackBase.h>
#include <log/log.h>
#include <android-base/logging.h>
#include <vendor/qti/ims/factory/1.0/IImsFactory.h>
#include <vendor/qti/ims/callcapability/1.0/ICallCapabilityListener.h>
#include <vendor/qti/ims/callcapability/1.0/ICallCapabilityService.h>
#include <chrono>
#include <thread>

using ::android::sp;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::vendor::qti::ims::factory::V1_0::IImsFactory;
using ::vendor::qti::ims::callcapability::V1_0::ICallCapabilityListener;
using ::vendor::qti::ims::callcapability::V1_0::ICallCapabilityService;
using ::vendor::qti::ims::callcapability::V1_0::CallCapabilityInfo;
using ::vendor::qti::ims::callcapability::V1_0::CallCapStatusCode;
using ::vendor::qti::ims::callcapability::V1_0::CallCapabilityInfoKeys;
using ::vendor::qti::ims::callcapability::V1_0::KeyValuePairBoolType;
using ::vendor::qti::ims::callcapability::V1_0::KeyValuePairStringType;

constexpr char cbOnPeerCapabilityUpdate[] = "onPeerCapabilityUpdate";
const int slot_id = 1;

class mockCallCapabilityArgs
{
 public:
   hidl_vec<CallCapabilityInfo> cbData;
};

class mockCallCapabilityListener : public ICallCapabilityListener,
                                   public ::testing::VtsHalHidlTargetCallbackBase<mockCallCapabilityArgs>
{
 public:
  mockCallCapabilityListener() {};
  virtual ~mockCallCapabilityListener() {};

  Return<void> onPeerCapabilityUpdate(const hidl_vec<CallCapabilityInfo>& data)
  {
    LOG(INFO) << LOG_TAG << "onPeerCapabilityUpdate";
    mockCallCapabilityArgs args;
    vector<KeyValuePairBoolType> boolDataVec;
    vector<KeyValuePairStringType> stringDataVec;
    args.cbData = data;
    for(auto it : data)
    {
      LOG(INFO) << LOG_TAG << "onPeerCapabilityUpdate: Contact Number: " << it.contactNumber;
      for(auto booldata : it.boolData)
      {
        LOG(INFO) << LOG_TAG << "onPeerCapabilityUpdate: Key: " << booldata.key;
        LOG(INFO) << LOG_TAG << "onPeerCapabilityUpdate: Value: " << booldata.value;
      }

      for(auto strData: it.stringData)
      {
        LOG(INFO) << LOG_TAG << "onPeerCapabilityUpdate: Key: " << strData.key;
        LOG(INFO) << LOG_TAG << "onPeerCapabilityUpdate: Value: " << strData.value;
      }
    }
    NotifyFromCallback(cbOnPeerCapabilityUpdate, args);
    return Void();
  };
};

class CallCapabilityTestBase : public ::testing::VtsHalHidlTargetTestBase
{
 public:
  virtual void SetUp() override
  {
    ALOGI("Setup function");
    IImsFactory::StatusCode status = IImsFactory::StatusCode::NOT_SUPPORTED;
    factory = IImsFactory::getService();
    if(factory != nullptr)
    {
      factory->createCallCapabilityService(slot_id,
              [&](IImsFactory::StatusCode _status, sp<ICallCapabilityService> callcap) {
                    status = _status;
                    pService = callcap;
              /* end Lamda Func*/ } );
    }
  };
  sp<IImsFactory> factory = nullptr;
  sp<ICallCapabilityService> pService = nullptr;
  sp<mockCallCapabilityListener> mlistener;
};

TEST_F(CallCapabilityTestBase, ServiceAvailabilityTest)
{
  IImsFactory::StatusCode status = IImsFactory::StatusCode::NOT_SUPPORTED;
  factory = IImsFactory::getService();
  if(factory != nullptr)
  {
    factory->createCallCapabilityService(slot_id,
         [&](IImsFactory::StatusCode _status, sp<ICallCapabilityService> callcap) {
           status = _status;
           pService = callcap;
    /* end Lamda Func*/ } );
    if(pService)
    ASSERT_NE(nullptr, pService.get()) << "Could not get Service";
  }
  else
  {
    ALOGI("No service object received");
  }
  ALOGI("Things worked as expected");
}

TEST_F(CallCapabilityTestBase, registerServiceTest)
{
  CallCapStatusCode status = CallCapStatusCode::FAIL;
  if(pService)
   ASSERT_NE(nullptr, pService.get()) << "Could not get Service";
  if(pService != nullptr)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    sp<mockCallCapabilityListener> mlistener = new mockCallCapabilityListener();
    status = pService->registerForCallCapabilityUpdate(mlistener);
    EXPECT_EQ(CallCapStatusCode::OK, status);
  }
}

TEST_F(CallCapabilityTestBase, CallCapabilityUpdateTest)
{
  CallCapStatusCode status = CallCapStatusCode::FAIL;
  if(pService != nullptr)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    sp<mockCallCapabilityListener> listener = new mockCallCapabilityListener();
    status = pService->registerForCallCapabilityUpdate(listener);
    EXPECT_EQ(CallCapStatusCode::OK, status);

    /**
     * Note: there should be a Incoming Call when testing this usecase.
     * the test case will wait for 60 secs to receive an incoming call
     */
    auto res = listener->WaitForCallback(cbOnPeerCapabilityUpdate, std::chrono::milliseconds(60000));
    EXPECT_TRUE(res.no_timeout);
    listener = nullptr;
  }
}
