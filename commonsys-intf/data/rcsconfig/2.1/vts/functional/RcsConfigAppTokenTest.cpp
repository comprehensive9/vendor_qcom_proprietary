/*****************************************************************************
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include "RcsConfigAppTokenTest.h"

using ::vendor::qti::ims::rcsconfig::V2_1::StatusCode;
TEST_F(RcsConfigAppTokenTest, AppTokenTest) {
  ALOGE("in vts AppTokenTest");

  // modem triggers
  auto res = mRcsConfigListener->WaitForCallback(kCallbackonTokenFetchRequest);
  EXPECT_TRUE(res.no_timeout);
  EXPECT_NE(0, res.args->requestId);
  //status
  auto res2 = pService->updateTokenFetchStatus(res.args->requestId, res.args->tokenType, StatusCode::SUCCESS );
  //App token
  if(g_configValues[AppToken_key].length() > 0) {
    ALOGE("in vts  triggering apptoken");
    pService->setAppToken(g_configValues[AppToken_key].c_str(), mListener);
    auto res1 = mListener->WaitForCallback(kCallbacksetConfigResponse);
    EXPECT_TRUE(res1.no_timeout);
    EXPECT_EQ(RequestStatus::OK, res1.args->status);
  }
}

TEST_F(RcsConfigAppTokenTest, SendAppTokenTest) {
  ALOGE("in vts SendAppTokenTest");
  pService->setAppToken("someRandom Data", mListener);
  auto res1 = mListener->WaitForCallback(kCallbacksetConfigResponse);
  EXPECT_TRUE(res1.no_timeout);
  EXPECT_EQ(RequestStatus::OK, res1.args->status);
}
