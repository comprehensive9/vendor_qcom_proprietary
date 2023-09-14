/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/CallForwardInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, CallForwardInfo_marshall_001) {
  Marshal marshal;
  RIL_CallForwardInfo request{ .status = 1,
                    .reason = 1,
                    .serviceClass = 0,
                    .toa = 2,
                    .number = "12345",
                    .timeSeconds = 1, };

  std::string s =
      "00000001"    // status = 1
      "00000001"    // reason = 1
      "00000000"    // serviceClass = 0
      "00000002"    // toa = 2
      "00000005"    // number length
      "3132333435"  // number ("12345")
      "00000001"    // timeSeconds = 1
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_CallForwardInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.status, request_r.status);
  ASSERT_EQ(request.reason, request_r.reason);
  ASSERT_EQ(request.serviceClass, request_r.serviceClass);
  ASSERT_EQ(request.toa, request_r.toa);
  ASSERT_STREQ(request.number, request_r.number);
  ASSERT_EQ(request.timeSeconds, request_r.timeSeconds);
}
