/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <QtiRilStringUtils.h>
#include <gtest/gtest.h>
#include <marshal/UUS_Info.h>
#include <telephony/ril.h>
#include <string>

TEST(Marshalling, UUS_Info_marshall_write_001) {
  Marshal marshal;
  RIL_UUS_Info request{ .uusType = RIL_UUS_TYPE2_REQUIRED, .uusDcs = RIL_UUS_DCS_X244, -1, nullptr };
  std::string s =
      "00000003"  // UUS Type
      "00000002"  // UUS DCS
      "ffffffff"  // Data length (-1 for null)
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, UUS_Info_marshall_write_002) {
  Marshal marshal;
  RIL_UUS_Info request{ .uusType = RIL_UUS_TYPE2_REQUIRED, .uusDcs = RIL_UUS_DCS_X244, 5, "12345" };
  std::string s =
      "00000003"    // UUS Type
      "00000002"    // UUS DCS
      "00000005"    // Data length (5)
      "3132333435"  // Data ("12345")
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}
