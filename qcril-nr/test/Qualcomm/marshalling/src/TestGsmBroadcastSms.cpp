/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <telephony/ril.h>
#include <marshal/GsmBroadcastSms.h>
#include <string>
#include <QtiRilStringUtils.h>

TEST(Marshalling, GsmBroadcastSms) {
  RIL_GSM_BroadcastSmsConfigInfo request{};
  RIL_GSM_BroadcastSmsConfigInfo out{};
  Marshal marshal;
  request.fromServiceId = 0x0001;
  request.toServiceId = 0xffff;
  request.fromCodeScheme = 0x00;
  request.toCodeScheme = 0xff;
  request.selected = 1;

  std::string s =
      "00000001"  // fromServiceId
      "0000ffff"  // toServiceId
      "00000000"  // fromCodeScheme
      "000000ff"  // toCodeScheme
      "01"        // selected
      ;
  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(out), Marshal::Result::FAILURE);
  ASSERT_EQ(out.fromServiceId, request.fromServiceId);
  ASSERT_EQ(out.toServiceId, request.toServiceId);
  ASSERT_EQ(out.fromCodeScheme, request.fromCodeScheme);
  ASSERT_EQ(out.toCodeScheme, request.toCodeScheme);
  ASSERT_EQ(out.selected, request.selected);
}
