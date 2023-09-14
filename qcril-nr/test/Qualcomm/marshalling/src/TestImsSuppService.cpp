/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSSuppService.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSSuppService_marshall_001) {
  Marshal marshal;

  RIL_IMS_CallBarringNumbersInfo callBarringNumbersInfo[1] {
    {
    .status = RIL_IMS_SERVICE_CLASS_STATUS_ENABLED, // = 1
      .number = "12345",
    },
  };
  std::string s_callBarringNumbersInfo =
      "00000001"    // status = 404
      "00000005"    // number length
      "3132333435"  // number ("12345")
      ;

  RIL_IMS_CallBarringNumbersListInfo callBarringNumbersListInfo{
    .serviceClass = 2,
      .callBarringNumbersInfoLen = 1,
      .callBarringNumbersInfo = (RIL_IMS_CallBarringNumbersInfo *)&callBarringNumbersInfo,
  };

  std::string s_callBarringNumbersListInfo;
  s_callBarringNumbersListInfo += "00000002";    // .serviceClass = 2,
  s_callBarringNumbersListInfo += "00000001";    // .callBarringNumbersInfoLen = 1,
  s_callBarringNumbersListInfo += s_callBarringNumbersInfo;

  RIL_IMS_SuppSvcRequest request{
    .operationType = RIL_IMS_SUPP_OPERATION_DEACTIVATE, // = 1
      .facilityType = RIL_IMS_FACILITY_BA_MT,  // = 9
      .callBarringNumbersListInfo = &callBarringNumbersListInfo,
      .password = "123456",
  };

  std::string s =
      "00000001"    // .operationType = 1
      "00000009"    // .facilityType = 9
      ;
  s += "00000001";   // .callBarringNumbersListInfo length = 1
  s += s_callBarringNumbersListInfo;
  s += "00000006"    // .password length
       "313233343536"  // .password ("123456")
        ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_SuppSvcRequest request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.operationType, request_r.operationType);
  ASSERT_EQ(request.facilityType, request_r.facilityType);

  RIL_IMS_CallBarringNumbersListInfo r_callBarringNumbersListInfo =
      *request_r.callBarringNumbersListInfo;
  ASSERT_EQ(callBarringNumbersListInfo.serviceClass, r_callBarringNumbersListInfo.serviceClass);
  ASSERT_EQ(callBarringNumbersListInfo.callBarringNumbersInfoLen,
            r_callBarringNumbersListInfo.callBarringNumbersInfoLen);

  RIL_IMS_CallBarringNumbersInfo* r_callBarringNumbersInfo =
      r_callBarringNumbersListInfo.callBarringNumbersInfo;
  ASSERT_EQ(callBarringNumbersInfo[0].status, r_callBarringNumbersInfo[0].status);
  ASSERT_STREQ(callBarringNumbersInfo[0].number, r_callBarringNumbersInfo[0].number);

  ASSERT_STREQ(request.password, request_r.password);
}


TEST(Marshalling, IMSSuppService_marshall_002) {
  Marshal marshal;

  RIL_IMS_CallBarringNumbersInfo callBarringNumbersInfo[1] {
    {
    .status = RIL_IMS_SERVICE_CLASS_STATUS_ENABLED, // = 1
      .number = "12345",
    },
  };
  std::string s_callBarringNumbersInfo =
      "00000001"    // status = 404
      "00000005"    // number length
      "3132333435"  // number ("12345")
      ;

  RIL_IMS_CallBarringNumbersListInfo callBarringNumbersListInfo[2]{
    {
      .serviceClass = 2,
        .callBarringNumbersInfoLen = 1,
        .callBarringNumbersInfo = (RIL_IMS_CallBarringNumbersInfo *)&callBarringNumbersInfo,
    },
      {
        .serviceClass = 1,
        .callBarringNumbersInfoLen = 0,
      },
  };

  std::string s_callBarringNumbersListInfo;
  s_callBarringNumbersListInfo += "00000002";    // .serviceClass = 2,
  s_callBarringNumbersListInfo += "00000001";    // .callBarringNumbersInfoLen = 1,
  s_callBarringNumbersListInfo += s_callBarringNumbersInfo;
  s_callBarringNumbersListInfo += "00000001";    // .serviceClass = 1,
  s_callBarringNumbersListInfo += "FFFFFFFF";    // .callBarringNumbersInfoLen = 0,

  RIL_IMS_SipErrorInfo errorDetails{
    .errorCode = 404,
    .errorString = "12345",
  };

  std::string s_errorDetails =
      "00000194"    // errorCode = 404
      "00000005"    // errorString length
      "3132333435"  // errorString ("12345")
      ;

  RIL_IMS_SuppSvcResponse request{
    .status = RIL_IMS_SERVICE_CLASS_STATUS_ENABLED, // = 1
      .provisionStatus = RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED, // = 1
      .facilityType = RIL_IMS_FACILITY_BA_MT,  // = 9
      .callBarringNumbersListInfoLen = 2,
      .callBarringNumbersListInfo = (RIL_IMS_CallBarringNumbersListInfo *)&callBarringNumbersListInfo,
      .errorDetails = &errorDetails,
  };

  std::string s =
      "00000001"    // .status = 1
      "00000001"    // .provisionStatus = 1
      "00000009"    // .facilityType = 9
      ;
  s += "00000002";   // callBarringNumbersListInfoLen = 2,
  s += s_callBarringNumbersListInfo;
  s += "00000001";    // errorDetails length = 1
  s += s_errorDetails;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_SuppSvcResponse request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.status, request_r.status);
  ASSERT_EQ(request.provisionStatus, request_r.provisionStatus);
  ASSERT_EQ(request.facilityType, request_r.facilityType);
  ASSERT_EQ(request.callBarringNumbersListInfoLen, request_r.callBarringNumbersListInfoLen);

  RIL_IMS_CallBarringNumbersListInfo *r_callBarringNumbersListInfo =
      request_r.callBarringNumbersListInfo;
  ASSERT_EQ(callBarringNumbersListInfo[0].serviceClass,
            r_callBarringNumbersListInfo[0].serviceClass);
  ASSERT_EQ(callBarringNumbersListInfo[0].callBarringNumbersInfoLen,
            r_callBarringNumbersListInfo[0].callBarringNumbersInfoLen);
  ASSERT_EQ(callBarringNumbersListInfo[1].serviceClass,
            r_callBarringNumbersListInfo[1].serviceClass);
  ASSERT_EQ(callBarringNumbersListInfo[1].callBarringNumbersInfoLen,
            r_callBarringNumbersListInfo[1].callBarringNumbersInfoLen);

  RIL_IMS_CallBarringNumbersInfo* r_callBarringNumbersInfo =
      r_callBarringNumbersListInfo[0].callBarringNumbersInfo;
  ASSERT_EQ(callBarringNumbersInfo[0].status, r_callBarringNumbersInfo[0].status);
  ASSERT_STREQ(callBarringNumbersInfo[0].number, r_callBarringNumbersInfo[0].number);

  RIL_IMS_SipErrorInfo r_errorDetails = *request_r.errorDetails;
  ASSERT_EQ(errorDetails.errorCode, r_errorDetails.errorCode);
  ASSERT_STREQ(errorDetails.errorString, r_errorDetails.errorString);
}
