/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSConfigInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSConfigInfo_marshall_001) {
  Marshal marshal;
  RIL_IMS_ConfigInfo request{
    .item = RIL_IMS_CONFIG_ITEM_SILENT_REDIAL_ENABLE, // = 7
      .hasBoolValue = 1,
      .boolValue = 0,
      .hasIntValue = 1,
      .intValue = 25,
      .stringValue = "123456",
      .hasErrorCause = 1,
      .errorCause = RIL_IMS_CONFIG_FAILURE_CAUSE_READ_FAILED, // = 3
  };

  std::string s =
      "00000007"    // .item = RIL_IMS_CONFIG_ITEM_SILENT_REDIAL_ENABLE // = 7
      "01"          // .hasBoolValue = 1
      "00"          // .boolValue = 0
      "01"          // .hasIntValue = 1,
      "00000019"    // .intValue = 25,
      "00000006"    // .stringValue length = 6,
      "313233343536" // .stringValue ("123456")
      "01"          // .hasErrorCause = 1
      "00000003"    // .errorCause = RIL_IMS_CALL_MODIFY_FAIL_CAUSE_SUCCESS, // = 3
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_ConfigInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.item, request_r.item);
  ASSERT_EQ(request.hasBoolValue, request_r.hasBoolValue);
  ASSERT_EQ(request.boolValue, request_r.boolValue);
  ASSERT_EQ(request.hasIntValue, request_r.hasIntValue);
  ASSERT_EQ(request.intValue, request_r.intValue);
  ASSERT_STREQ(request.stringValue, request_r.stringValue);
  ASSERT_EQ(request.hasErrorCause, request_r.hasErrorCause);
  ASSERT_EQ(request.errorCause, request_r.errorCause);
}
