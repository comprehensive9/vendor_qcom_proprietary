/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSCallModifyInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSCallModifyInfo_marshall_001) {
  Marshal marshal;
  RIL_IMS_CallModifyInfo request{
    .callId = 10,
      .callType = RIL_IMS_CALL_TYPE_VOICE, // = 1
      .callDomain = RIL_IMS_CALLDOMAIN_PS, // = 2
      .hasRttMode = 1,
      .rttMode = RIL_IMS_RTT_FULL, // = 1
      .hasCallModifyFailCause = 1,
      .callModifyFailCause = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_SUCCESS, // =1
  };

  std::string s =
      "0000000A"    // .callId = 10
      "00000001"    // .callType = RIL_IMS_CALL_TYPE_VOICE, // = 1
      "00000002"    // .callDomain = RIL_IMS_CALLDOMAIN_PS, // = 2
      "01"          // .hasRttMode = 1,
      "00000001"    // .rttMode = RIL_IMS_RTT_FULL, // = 1
      "01"          // .hasCallModifyFailCause = 1,
      "00000001"    // .callModifyFailCause = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_SUCCESS, // =1
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_CallModifyInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.callId, request_r.callId);
  ASSERT_EQ(request.callType, request_r.callType);
  ASSERT_EQ(request.callDomain, request_r.callDomain);
  ASSERT_EQ(request.hasRttMode, request_r.hasRttMode);
  ASSERT_EQ(request.rttMode, request_r.rttMode);
  ASSERT_EQ(request.hasCallModifyFailCause, request_r.hasCallModifyFailCause);
  ASSERT_EQ(request.callModifyFailCause, request_r.callModifyFailCause);
}
