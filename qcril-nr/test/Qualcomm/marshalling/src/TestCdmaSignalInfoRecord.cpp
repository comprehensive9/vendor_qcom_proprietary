/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/CdmaSignalInfoRecord.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, CDMASignalInfoRecord_marshall_write_001) {
  Marshal marshal;
  RIL_CDMA_SignalInfoRecord request{ .isPresent = 1, .signalType = 10, .alertPitch = 2, .signal = 5 };

  std::string s =
      "01"  // .isPresent = 1
      "0A"  // .signalType = 10
      "02"  // .alertPitch = 2
      "05"  // .signal = 5
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}
