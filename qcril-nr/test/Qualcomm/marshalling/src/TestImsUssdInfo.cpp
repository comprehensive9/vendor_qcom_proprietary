/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSUssdInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSUssdInfo_marshall_001) {
  Marshal marshal;

  RIL_IMS_SipErrorInfo errorDetails{
    .errorCode = 404,
    .errorString = "12345",
  };

  std::string s_errorDetails =
      "00000194"    // errorCode = 404
      "00000005"    // errorString length
      "3132333435"  // errorString ("12345")
      ;

  RIL_IMS_UssdInfo request{
    .mode = RIL_USSD_REQUEST,  // = 1
      .errorDetails = &errorDetails,
  };

  std::string s;
  s += "00000001";    // .mode = RIL_USSD_REQUEST,  // = 1
  s += "00000001";    // errorDetails length = 1
  s += s_errorDetails;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_UssdInfo request_r{};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.mode, request_r.mode);

  RIL_IMS_SipErrorInfo r_errorDetails = *request_r.errorDetails;
  ASSERT_EQ(errorDetails.errorCode, r_errorDetails.errorCode);
  ASSERT_STREQ(errorDetails.errorString, r_errorDetails.errorString);
}
