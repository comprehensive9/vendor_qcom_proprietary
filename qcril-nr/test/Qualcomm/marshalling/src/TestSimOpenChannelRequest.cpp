/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <QtiRilStringUtils.h>
#include <gtest/gtest.h>
#include <marshal/SimOpenChannelRequest.h>
#include <telephony/ril.h>
#include <string>

TEST(Marshalling, SIM_Open_Channel_Request_marshall_write_001) {
  Marshal marshal;
  RIL_OpenChannelParams request{ "123456", 5 };

  std::string s =
      "00000006"      // aidPtr length
      "313233343536"  // aidPtr
      "00000005"      // p2
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, SIM_Open_Channel_Request_marshall_write_002) {
  Marshal marshal;
  RIL_OpenChannelParams request{ nullptr, 7 };

  std::string s =
      "ffffffff"      // aidPtr length
      "00000007"      // p2
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}
