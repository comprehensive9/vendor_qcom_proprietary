/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSCallForwardInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSCallForwardInfo_marshall_001) {
  Marshal marshal;
  RIL_IMS_CallForwardInfo request{
    .status = 1,
    .reason = 1,
    .serviceClass = 0,
    .toa = 2,
    .number = "12345",
    .timeSeconds = 1,
    .hasCallFwdTimerStart = 1,
    .callFwdTimerStart = {
      .year = 2020,
      .month = 1,
      .day = 2,
      .hour = 1,
      .minute = 0,
      .second = 0,
      .timezone = 0,
    },
    .hasCallFwdTimerEnd = 1,
    .callFwdTimerEnd = {
      .year = 2020,
      .month = 3,
      .day = 21,
      .hour = 1,
      .minute = 0,
      .second = 0,
      .timezone = 0,
    },
    };

  std::string s =
      "00000001"    // status = 1
      "00000001"    // reason = 1
      "00000000"    // serviceClass = 0
      "00000002"    // toa = 2
      "00000005"    // number length
      "3132333435"  // number ("12345")
      "00000001"    // timeSeconds = 1
      "01"          // .hasCallFwdTimerStart = 1
      "000007E4"    //  .year = 2020,
      "00000001"    //  .month = 1,
      "00000002"    //  .day = 2,
      "00000001"    //  .hour = 1,
      "00000000"    //  .minute = 0,
      "00000000"    //  .second = 0,
      "00000000"    //  .timezone = 0,
      "01"          // .hasCallFwdTimerEnd = 1
      "000007E4"    //  .year = 2020,
      "00000003"    //  .month = 3,
      "00000015"    //  .day = 21,
      "00000001"    //  .hour = 1,
      "00000000"    //  .minute = 0,
      "00000000"    //  .second = 0,
      "00000000"    //  .timezone = 0,
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_CallForwardInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.status, request_r.status);
  ASSERT_EQ(request.reason, request_r.reason);
  ASSERT_EQ(request.serviceClass, request_r.serviceClass);
  ASSERT_EQ(request.toa, request_r.toa);
  ASSERT_STREQ(request.number, request_r.number);
  ASSERT_EQ(request.timeSeconds, request_r.timeSeconds);
  ASSERT_EQ(request.hasCallFwdTimerStart, request_r.hasCallFwdTimerStart);
  ASSERT_EQ(request.callFwdTimerStart.year, request_r.callFwdTimerStart.year);
  ASSERT_EQ(request.callFwdTimerStart.month, request_r.callFwdTimerStart.month);
  ASSERT_EQ(request.callFwdTimerStart.day, request_r.callFwdTimerStart.day);
  ASSERT_EQ(request.callFwdTimerStart.hour, request_r.callFwdTimerStart.hour);
  ASSERT_EQ(request.callFwdTimerStart.minute, request_r.callFwdTimerStart.minute);
  ASSERT_EQ(request.callFwdTimerStart.second, request_r.callFwdTimerStart.second);
  ASSERT_EQ(request.callFwdTimerStart.timezone, request_r.callFwdTimerStart.timezone);
  ASSERT_EQ(request.hasCallFwdTimerEnd, request_r.hasCallFwdTimerEnd);
  ASSERT_EQ(request.callFwdTimerEnd.year, request_r.callFwdTimerEnd.year);
  ASSERT_EQ(request.callFwdTimerEnd.month, request_r.callFwdTimerEnd.month);
  ASSERT_EQ(request.callFwdTimerEnd.day, request_r.callFwdTimerEnd.day);
  ASSERT_EQ(request.callFwdTimerEnd.hour, request_r.callFwdTimerEnd.hour);
  ASSERT_EQ(request.callFwdTimerEnd.minute, request_r.callFwdTimerEnd.minute);
  ASSERT_EQ(request.callFwdTimerEnd.second, request_r.callFwdTimerEnd.second);
  ASSERT_EQ(request.callFwdTimerEnd.timezone, request_r.callFwdTimerEnd.timezone);
}

TEST(Marshalling, IMSQueryCallForwardStatusInfo_marshall_001) {
  Marshal marshal;
  RIL_IMS_CallForwardInfo callForwardInfo[2] {
    { .status = 1,
      .reason = 1,
      .serviceClass = 0,
      .toa = 2,
      .number = "12345",
      .timeSeconds = 1,
      .hasCallFwdTimerStart = 1,
      .callFwdTimerStart = {
        .year = 2020,
        .month = 1,
        .day = 2,
        .hour = 1,
        .minute = 0,
        .second = 0,
        .timezone = 0,
      },
      .hasCallFwdTimerEnd = 1,
      .callFwdTimerEnd = {
        .year = 2020,
        .month = 3,
        .day = 21,
        .hour = 1,
        .minute = 0,
        .second = 0,
        .timezone = 0,
      },
    },
      { .status = 1,
        .reason = 2,
        .serviceClass = 1,
        .toa = 2,
        .number = "12345",
        .timeSeconds = 60,
        .hasCallFwdTimerStart = 0,
        .hasCallFwdTimerEnd = 0,
      }
  };

  std::string s_callForwardInfo =
    // callForwardInfo[0]
      "00000001"    // status = 1
      "00000001"    // reason = 1
      "00000000"    // serviceClass = 0
      "00000002"    // toa = 2
      "00000005"    // number length
      "3132333435"  // number ("12345")
      "00000001"    // timeSeconds = 1
      "01"          // .hasCallFwdTimerStart = 1
      "000007E4"    //  .year = 2020,
      "00000001"    //  .month = 1,
      "00000002"    //  .day = 2,
      "00000001"    //  .hour = 1,
      "00000000"    //  .minute = 0,
      "00000000"    //  .second = 0,
      "00000000"    //  .timezone = 0,
      "01"          // .hasCallFwdTimerEnd = 1
      "000007E4"    //  .year = 2020,
      "00000003"    //  .month = 3,
      "00000015"    //  .day = 21,
      "00000001"    //  .hour = 1,
      "00000000"    //  .minute = 0,
      "00000000"    //  .second = 0,
      "00000000"    //  .timezone = 0,
    // callForwardInfo[1]
      "00000001"    // status = 1
      "00000002"    // reason = 2
      "00000001"    // serviceClass = 1
      "00000002"    // toa = 2
      "00000005"    // number length
      "3132333435"  // number ("12345")
      "0000003C"    // timeSeconds = 60
      "00"          // .hasCallFwdTimerStart = 0
      "00"          // .hasCallFwdTimerEnd = 0
      ;

  RIL_IMS_SipErrorInfo errorDetails{
    .errorCode = 404,
    .errorString = "12345",
  };

  std::string s_errorDetails =
      "00000194"    // errorCode = 404
      "00000005"    // errorString length
      "3132333435"  // errorString ("12345")
      ;

  RIL_IMS_QueryCallForwardStatusInfo request {
    .callForwardInfoLen = 2,
    .callForwardInfo = (RIL_IMS_CallForwardInfo*)callForwardInfo,
    .errorDetails = &errorDetails,
  };

  std::string s;
  s += "00000002";    // .callForwardInfoLen = 2,
  s += s_callForwardInfo;
  s += "00000001";    // errorDetails length = 1
  s += s_errorDetails;


  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_QueryCallForwardStatusInfo readData{};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(readData), Marshal::Result::FAILURE);

  ASSERT_EQ(request.callForwardInfoLen, readData.callForwardInfoLen);

  RIL_IMS_CallForwardInfo *r_callForwardInfo = readData.callForwardInfo;
  ASSERT_EQ(callForwardInfo[0].status, r_callForwardInfo[0].status);
  ASSERT_EQ(callForwardInfo[0].reason, r_callForwardInfo[0].reason);
  ASSERT_EQ(callForwardInfo[0].serviceClass, r_callForwardInfo[0].serviceClass);
  ASSERT_EQ(callForwardInfo[0].toa, r_callForwardInfo[0].toa);
  ASSERT_STREQ(callForwardInfo[0].number, r_callForwardInfo[0].number);
  ASSERT_EQ(callForwardInfo[0].timeSeconds, r_callForwardInfo[0].timeSeconds);
  ASSERT_EQ(callForwardInfo[0].hasCallFwdTimerStart, r_callForwardInfo[0].hasCallFwdTimerStart);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerStart.year, r_callForwardInfo[0].callFwdTimerStart.year);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerStart.month, r_callForwardInfo[0].callFwdTimerStart.month);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerStart.day, r_callForwardInfo[0].callFwdTimerStart.day);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerStart.hour, r_callForwardInfo[0].callFwdTimerStart.hour);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerStart.minute, r_callForwardInfo[0].callFwdTimerStart.minute);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerStart.second, r_callForwardInfo[0].callFwdTimerStart.second);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerStart.timezone, r_callForwardInfo[0].callFwdTimerStart.timezone);
  ASSERT_EQ(callForwardInfo[0].hasCallFwdTimerEnd, r_callForwardInfo[0].hasCallFwdTimerEnd);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerEnd.year, r_callForwardInfo[0].callFwdTimerEnd.year);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerEnd.month, r_callForwardInfo[0].callFwdTimerEnd.month);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerEnd.day, r_callForwardInfo[0].callFwdTimerEnd.day);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerEnd.hour, r_callForwardInfo[0].callFwdTimerEnd.hour);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerEnd.minute, r_callForwardInfo[0].callFwdTimerEnd.minute);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerEnd.second, r_callForwardInfo[0].callFwdTimerEnd.second);
  ASSERT_EQ(callForwardInfo[0].callFwdTimerEnd.timezone, r_callForwardInfo[0].callFwdTimerEnd.timezone);

  ASSERT_EQ(callForwardInfo[1].status, r_callForwardInfo[1].status);
  ASSERT_EQ(callForwardInfo[1].reason, r_callForwardInfo[1].reason);
  ASSERT_EQ(callForwardInfo[1].serviceClass, r_callForwardInfo[1].serviceClass);
  ASSERT_EQ(callForwardInfo[1].toa, r_callForwardInfo[1].toa);
  ASSERT_STREQ(callForwardInfo[1].number, r_callForwardInfo[1].number);
  ASSERT_EQ(callForwardInfo[1].timeSeconds, r_callForwardInfo[1].timeSeconds);
  ASSERT_EQ(callForwardInfo[1].hasCallFwdTimerStart, r_callForwardInfo[1].hasCallFwdTimerStart);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerStart.year, r_callForwardInfo[1].callFwdTimerStart.year);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerStart.month, r_callForwardInfo[1].callFwdTimerStart.month);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerStart.day, r_callForwardInfo[1].callFwdTimerStart.day);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerStart.hour, r_callForwardInfo[1].callFwdTimerStart.hour);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerStart.minute, r_callForwardInfo[1].callFwdTimerStart.minute);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerStart.second, r_callForwardInfo[1].callFwdTimerStart.second);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerStart.timezone, r_callForwardInfo[1].callFwdTimerStart.timezone);
  ASSERT_EQ(callForwardInfo[1].hasCallFwdTimerEnd, r_callForwardInfo[1].hasCallFwdTimerEnd);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerEnd.year, r_callForwardInfo[1].callFwdTimerEnd.year);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerEnd.month, r_callForwardInfo[1].callFwdTimerEnd.month);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerEnd.day, r_callForwardInfo[1].callFwdTimerEnd.day);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerEnd.hour, r_callForwardInfo[1].callFwdTimerEnd.hour);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerEnd.minute, r_callForwardInfo[1].callFwdTimerEnd.minute);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerEnd.second, r_callForwardInfo[1].callFwdTimerEnd.second);
  ASSERT_EQ(callForwardInfo[1].callFwdTimerEnd.timezone, r_callForwardInfo[1].callFwdTimerEnd.timezone);


  RIL_IMS_SipErrorInfo r_errorDetails = *readData.errorDetails;
  ASSERT_EQ(errorDetails.errorCode, r_errorDetails.errorCode);
  ASSERT_STREQ(errorDetails.errorString, r_errorDetails.errorString);
}

TEST(Marshalling, IMSSetCallForwardStatusInfo_marshall_001) {
  Marshal marshal;

  RIL_IMS_SipErrorInfo cfStatus_errorDetails{
    .errorCode = 404,
    .errorString = "23456",
  };
  std::string s_cfStatus_errorDetails =
      "00000194"    // errorCode = 404
      "00000005"    // errorString length
      "3233343536"  // errorString ("23456")
      ;

  RIL_IMS_SetCallForwardStatus setCallForwardStatus[2] {
    { .reason = 1,
      .status = 1,
      .errorDetails = nullptr
    },
    { .reason = 2,
      .status = 0,
      .errorDetails = &cfStatus_errorDetails,
    },
  };

  std::string s_setCallForwardStatus;
  s_setCallForwardStatus +=
    // setCallForwardStatus[0]
      "00000001"    // reason = 1
      "01"          // status = 1
      ;
  s_setCallForwardStatus += "FFFFFFFF";    // errorDetails length = -1 (invalid)
    // setCallForwardStatus[1]
  s_setCallForwardStatus +=
      "00000002"    // reason = 2
      "00"          // status = 0
      ;
  s_setCallForwardStatus += "00000001";    // errorDetails length = 1
  s_setCallForwardStatus += s_cfStatus_errorDetails;

  RIL_IMS_SipErrorInfo errorDetails{
    .errorCode = 404,
    .errorString = "12345",
  };

  std::string s_errorDetails =
      "00000194"    // errorCode = 404
      "00000005"    // errorString length
      "3132333435"  // errorString ("12345")
      ;

  RIL_IMS_SetCallForwardStatusInfo request {
    .setCallForwardStatusLen = 2,
    .setCallForwardStatus = (RIL_IMS_SetCallForwardStatus*)setCallForwardStatus,
    .errorDetails = &errorDetails,
  };

  std::string s = std::string("");
  s += "00000002";    // .setCallForwardStatusLen = 2,
  s += s_setCallForwardStatus;
  s += "00000001";    // errorDetails length = 1
  s += s_errorDetails;


  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  //ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_SetCallForwardStatusInfo readData{};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(readData), Marshal::Result::FAILURE);

  ASSERT_EQ(request.setCallForwardStatusLen, readData.setCallForwardStatusLen);

  RIL_IMS_SetCallForwardStatus *r_setCallForwardStatus = readData.setCallForwardStatus;
  ASSERT_EQ(setCallForwardStatus[0].reason, r_setCallForwardStatus[0].reason);
  ASSERT_EQ(setCallForwardStatus[0].status, r_setCallForwardStatus[0].status);

  ASSERT_EQ(setCallForwardStatus[1].reason, r_setCallForwardStatus[1].reason);
  ASSERT_EQ(setCallForwardStatus[1].status, r_setCallForwardStatus[1].status);
  RIL_IMS_SipErrorInfo r_cfStatus_errorDetails = *r_setCallForwardStatus[1].errorDetails;
  ASSERT_EQ(cfStatus_errorDetails.errorCode, r_cfStatus_errorDetails.errorCode);
  ASSERT_STREQ(cfStatus_errorDetails.errorString, r_cfStatus_errorDetails.errorString);

  RIL_IMS_SipErrorInfo r_errorDetails = *readData.errorDetails;
  ASSERT_EQ(errorDetails.errorCode, r_errorDetails.errorCode);
  ASSERT_STREQ(errorDetails.errorString, r_errorDetails.errorString);
}
