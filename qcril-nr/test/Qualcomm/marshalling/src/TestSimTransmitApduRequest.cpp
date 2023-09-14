/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <QtiRilStringUtils.h>
#include <gtest/gtest.h>
#include <marshal/SimTransmitApduRequest.h>
#include <telephony/ril.h>
#include <string>

TEST(Marshalling, SIM_Transmit_Apdu_Request_marshall_write_001) {
  Marshal marshal;

  RIL_SIM_APDU request;

  request.sessionid = 4;
  request.cla = 5;
  request.instruction = 6;
  request.p1 = 1;
  request.p2 = 2;
  request.p3 = 3;
  request.data = nullptr;

  std::string s =
		"00000004"     // session id
		"00000005"     // cla
		"00000006"     // instruction
		"00000001"     // p1
		"00000002"     // p2
		"00000003"     // p3
		"ffffffff"     // data length
        ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}
