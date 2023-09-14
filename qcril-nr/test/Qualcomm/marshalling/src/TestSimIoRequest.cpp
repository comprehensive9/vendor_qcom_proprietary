/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <QtiRilStringUtils.h>
#include <gtest/gtest.h>
#include <marshal/SimIoRequest.h>
#include <telephony/ril.h>
#include <string>

TEST(Marshalling, SIM_IO_Request_marshall_write_001) {
  Marshal marshal;

  RIL_SIM_IO_v6 request;

  request.command = 4;
  request.fileid = 5;
  request.path = "12345";
  request.p1 = 1;
  request.p2 = 2;
  request.p3 = 3;
  request.data = "3456789";
  request.pin2 = "456789";
  request.aidPtr = "67890";

  std::string s =
		"00000004"     // command
		"00000005"     // fileid
        "00000005"     // path length
		"3132333435"           // path
		"00000001"     // p1
		"00000002"     // p2
		"00000003"     // p3
        "00000007"             // data length
		"33343536373839"       // data
        "00000006"             // pin2 length
		"343536373839"         // pin2
		"00000005"             // aidPtr length
		"3637383930"           // aidPtr
        ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, SIM_IO_Request_marshall_write_002) {
  Marshal marshal;

  RIL_SIM_IO_v6 request;

  request.command = 4;
  request.fileid = 5;
  request.path = nullptr;
  request.p1 = 1;
  request.p2 = 2;
  request.p3 = 3;
  request.data = nullptr;
  request.pin2 = nullptr;
  request.aidPtr = nullptr;

  std::string s =
		"00000004"     // command
		"00000005"     // fileid
		"ffffffff"     // path length
		"00000001"     // p1
		"00000002"     // p2
		"00000003"     // p3
		"ffffffff"     // data length
		"ffffffff"     // pin2 length
		"ffffffff"     // aidPtr length
        ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}
