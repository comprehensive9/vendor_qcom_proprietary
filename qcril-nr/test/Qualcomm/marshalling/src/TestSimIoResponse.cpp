/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <QtiRilStringUtils.h>
#include <gtest/gtest.h>
#include <marshal/SimIoResponse.h>
#include <telephony/ril.h>
#include <string>

TEST(Marshalling, SIM_IO_Response_marshall_write_001) {
  Marshal marshal;
  
  RIL_SIM_IO_Response request;
  
  request.sw1 = 0x90;
  request.sw2 = 0x11;
  request.simResponse = "12345";
	
  std::string s =
		"00000090"     // sw1
		"00000011"     // sw2
		"00000005"     // simResponse
		"3132333435"   // simResponse
        ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, SIM_IO_Response_marshall_write_002) {
  Marshal marshal;
  
  RIL_SIM_IO_Response request;
  
  request.sw1 = 0x90;
  request.sw2 = 0x11;
  request.simResponse = nullptr;
	
  std::string s =
		"00000090"     // sw1
		"00000011"     // sw2
		"ffffffff"     // simResponse
        ;
		
  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}
