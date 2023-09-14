/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/StkCcUnsolSsResponse.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, StkCcUnsolSsResponse_marshall_001) {
  Marshal marshal;
  RIL_StkCcUnsolSsResponse request{
    .serviceType = SS_CF_ALL_CONDITIONAL, // = 5
      .requestType = SS_INTERROGATION,  // = 2
      .teleserviceType = SS_ALL_TELESERVICES_EXCEPT_SMS, // = 5
      .serviceClass = SS_VOICE,  // = 1
      .result = RIL_E_SUCCESS, // = 0
      .cfData = {
        .numValidIndexes = 1,
        .cfInfo = {
          { .status = 1,
            .reason = 2,
            .serviceClass = 1,
            .toa = 2,
            .number = "12345",
            .timeSeconds = 60,
          },
        },
      },
  };

  std::string s =
      "00000005"    // .serviceType = SS_CF_ALL_CONDITIONAL, // = 5
      "00000002"    // .requestType = SS_INTERROGATION,  // = 2
      "00000005"    // .teleserviceType = SS_ALL_TELESERVICES_EXCEPT_SMS, // = 5
      "00000001"    // .serviceClass = SS_VOICE,  // = 1
      "00000000"    // .result = RIL_E_SUCCESS, // = 0
      // .cfData
      "00000001"    // .numValidIndexes = 1,
      // cfInfo[0]
      "00000001"    // status = 1
      "00000002"    // reason = 2
      "00000001"    // serviceClass = 1
      "00000002"    // toa = 2
      "00000005"    // number length
      "3132333435"  // number ("12345")
      "0000003C"    // timeSeconds = 60
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);



  RIL_StkCcUnsolSsResponse request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.serviceType, request_r.serviceType);
  ASSERT_EQ(request.requestType, request_r.requestType);
  ASSERT_EQ(request.teleserviceType, request_r.teleserviceType);
  ASSERT_EQ(request.serviceClass, request_r.serviceClass);
  ASSERT_EQ(request.result, request_r.result);
}

TEST(Marshalling, StkCcUnsolSsResponse_marshall_002) {
  Marshal marshal;
  RIL_StkCcUnsolSsResponse request{
    .serviceType = SS_CLIP, // = 6
      .requestType = SS_INTERROGATION,  // = 2
      .teleserviceType = SS_ALL_TELESERVICES_EXCEPT_SMS, // = 5
      .serviceClass = SS_VOICE,  // = 1
      .result = RIL_E_SUCCESS, // = 0
      .ssInfo = {
        1, 0, 0, 0,
      },
  };

  std::string s =
      "00000006"    // .serviceType = SS_CLIP, // = 6
      "00000002"    // .requestType = SS_INTERROGATION,  // = 2
      "00000005"    // .teleserviceType = SS_ALL_TELESERVICES_EXCEPT_SMS, // = 5
      "00000001"    // .serviceClass = SS_VOICE,  // = 1
      "00000000"    // .result = RIL_E_SUCCESS, // = 0
      // ssInfo - array of 4
      "00000004"    // ssInfo length = 4
      "00000001"    // 1
      "00000000"    // 0
      "00000000"    // 0
      "00000000"    // 0
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);



  RIL_StkCcUnsolSsResponse request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.serviceType, request_r.serviceType);
  ASSERT_EQ(request.requestType, request_r.requestType);
  ASSERT_EQ(request.teleserviceType, request_r.teleserviceType);
  ASSERT_EQ(request.serviceClass, request_r.serviceClass);
  ASSERT_EQ(request.result, request_r.result);
}
