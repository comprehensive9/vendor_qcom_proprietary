/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/SuppSvcNotification.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, SuppSvcNotification_marshall_001) {
  Marshal marshal;
  RIL_SuppSvcNotification request{
                    .notificationType = 1,
                    .code = 10,
                    .index = 2,
                    .type = 4,
                    .number = "123456", };

  std::string s =
      "00000001"     // notificationType = 1
      "0000000A"     // code = 10
      "00000002"     // index = 2
      "00000004"     // type = 4
      "00000006"     // number length
      "313233343536" // number ("123456")
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_SuppSvcNotification request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.notificationType, request_r.notificationType);
  ASSERT_EQ(request.code, request_r.code);
  ASSERT_EQ(request.index, request_r.index);
  ASSERT_EQ(request.type, request_r.type);
  ASSERT_STREQ(request.number, request_r.number);
}
