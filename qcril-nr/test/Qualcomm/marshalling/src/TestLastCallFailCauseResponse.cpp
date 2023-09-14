/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/LastCallFailCauseResponse.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, LastCallFailCauseResponse_marshall_basic) {
  RIL_LastCallFailCauseInfo request;
  Marshal marshal;
  request.cause_code = CALL_FAIL_BUSY;
  request.vendor_cause = nullptr;

  std::string s =
      "00000011"  // Cause Code in Hex is 0x11
      "00000000"  // Vendor Cause Absent
      ;
  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, LastCallFailCauseResponse_marshall_vendor_cause) {
  RIL_LastCallFailCauseInfo request;
  Marshal marshal;

  request.cause_code = CALL_FAIL_NO_ROUTE_TO_DESTINATION;
  request.vendor_cause = (char*)"12345";
  std::string s =
      "00000003"    // Cause Code in Hex is 0x03
      "00000001"    // Vendor Cause Present
      "00000005"    // Length of Vendor Cause
      "3132333435"  // Address
      ;
  std::string expected = QtiRilStringUtils::fromHex(s);

  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_LastCallFailCauseInfo requestRead;
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(requestRead), Marshal::Result::FAILURE);
  ASSERT_EQ(request.cause_code, requestRead.cause_code);
  ASSERT_STREQ(request.vendor_cause, requestRead.vendor_cause);
}
