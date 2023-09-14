/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSAutoCallRejectionInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSAutoCallRejectionInfo_marshall_001) {
  Marshal marshal;
  RIL_IMS_AutoCallRejectionInfo request{
    .callType = RIL_IMS_CALL_TYPE_VT, // = 4,
      .cause = RIL_IMS_FAILCAUSE_SIP_REQUEST_ENTITY_TOO_LARGE, // = 0x440
      .sipErrorCode = 404,
      .number = "123456",
  };

  std::string s =
      "00000004"    // .callType = RIL_IMS_CALL_TYPE_VT, // = 4,
      "00000440"    // .cause = RIL_IMS_FAILCAUSE_SIP_REQUEST_ENTITY_TOO_LARGE, // = 0x440
      "0194"        // .sipErrorCode = 404,
      "00000006"    // .number length = 6,
      "313233343536" // .number ("123456")
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_AutoCallRejectionInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.callType, request_r.callType);
  ASSERT_EQ(request.cause, request_r.cause);
  ASSERT_EQ(request.sipErrorCode, request_r.sipErrorCode);
  ASSERT_STREQ(request.number, request_r.number);
}
