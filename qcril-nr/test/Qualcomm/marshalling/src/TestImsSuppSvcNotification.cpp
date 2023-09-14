/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSSuppSvcNotification.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSSuppSvcNotification_marshall_001) {
  Marshal marshal;
  RIL_IMS_SuppSvcNotification request{
    .notificationType = 1,
      .code = 1,
      .index = 0,
      .type = 1,
      .number = "123456",
      .connId = 1,
      .historyInfo = "123456",
      .hasHoldTone = 1,
      .holdTone = 1,
  };

  std::string s =
      "00000001"    // .notificationType = 1
      "00000001"    // .code = 1
      "00000000"    // .index = 0
      "00000001"    // .type = 1,
      "00000006"    // .number length = 6,
      "313233343536" // .number ("123456")
      "00000001"    // .connId = 1
      "00000006"    // .historyInfo length = 6,
      "313233343536" // .historyInfo ("123456")
      "01"          // .hasHoldTone = 1
      "01"          // .holdTone = 1,
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_SuppSvcNotification request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.notificationType, request_r.notificationType);
  ASSERT_EQ(request.code, request_r.code);
  ASSERT_EQ(request.index, request_r.index);
  ASSERT_EQ(request.type, request_r.type);
  ASSERT_STREQ(request.number, request_r.number);
  ASSERT_EQ(request.connId, request_r.connId);
  ASSERT_STREQ(request.historyInfo, request_r.historyInfo);
  ASSERT_EQ(request.hasHoldTone, request_r.hasHoldTone);
  ASSERT_EQ(request.holdTone, request_r.holdTone);
}
