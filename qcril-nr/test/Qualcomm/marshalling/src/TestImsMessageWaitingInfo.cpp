/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSMessageWaitingInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSMessageWaitingInfo_marshall_001) {
  Marshal marshal;
  RIL_IMS_MessageSummary messageSummary[2]{
    {
      .messageType = RIL_IMS_MESSAGE_TYPE_VOICE, // = 2
        .newMessageCount = 10,
        .oldMessageCount = 5,
        .newUrgentMessageCount = 45,
        .oldUrgentMessageCount = 9,
    },
      {
        .messageType = RIL_IMS_MESSAGE_TYPE_MULTIMEDIA, // = 6
        .newMessageCount = 11,
        .oldMessageCount = 1,
        .newUrgentMessageCount = 5,
        .oldUrgentMessageCount = 3,
      },
  };
  std::string s_messageSummary =
    // messageSummary[0]
      "00000002"    // .messageType = RIL_IMS_MESSAGE_TYPE_VOICE, // = 2
      "0000000A"    // .newMessageCount = 10,
      "00000005"    // .oldMessageCount = 5,
      "0000002D"    // .newUrgentMessageCount = 45,
      "00000009"    // .oldUrgentMessageCount = 9,
    // messageSummary[1]
      "00000006"    // .messageType = RIL_IMS_MESSAGE_TYPE_MULTIMEDIA, // = 6
      "0000000B"    // .newMessageCount = 11,
      "00000001"    // .oldMessageCount = 1,
      "00000005"    // .newUrgentMessageCount = 5,
      "00000003"    // .oldUrgentMessageCount = 3,
      ;

  RIL_IMS_MessageDetails messageDetails[2]{
    {
      .toAddress = "1234",
        .fromAddress = "5678",
        .subject = "1234",
        .date = "1234",
        .priority = RIL_IMS_MESSAGE_PRIORITY_NORMAL, // = 2
        .messageId = "1234",
        .messageType = RIL_IMS_MESSAGE_TYPE_TEXT, // 7
    },
      {
        .toAddress = "1234",
        .fromAddress = "5678",
        .subject = "ABC",
        .date = "1234",
        .priority = RIL_IMS_MESSAGE_PRIORITY_URGENT, // = 3
        .messageId = "1234",
        .messageType = RIL_IMS_MESSAGE_TYPE_PAGER, // 5
      },
  };
  std::string s_messageDetails =
    // messageDetails[0]
      "00000004"    // .toAddress length
      "31323334"    // .toAddress = "1234",
      "00000004"    // .fromAddress length
      "35363738"    // .fromAddress = "5678",
      "00000004"    // .subject length
      "31323334"    // .subject = "1234",
      "00000004"    // .date length
      "31323334"    // .date = "1234",
      "00000002"    // .priority = RIL_IMS_MESSAGE_PRIORITY_NORMAL, // = 2
      "00000004"    // .messageId length
      "31323334"    // .messageId = "1234",
      "00000007"    // .messageType = RIL_IMS_MESSAGE_TYPE_TEXT, // 7
    // messageDetails[1]
      "00000004"    // .toAddress length
      "31323334"    // .toAddress = "1234",
      "00000004"    // .fromAddress length
      "35363738"    // .fromAddress = "5678",
      "00000003"    // .subject length
      "414243"      // .subject = "ABC",
      "00000004"    // .date length
      "31323334"    // .date = "1234",
      "00000003"    // .priority = RIL_IMS_MESSAGE_PRIORITY_URGENT, // = 3
      "00000004"    // .messageId length
      "31323334"    // .messageId = "1234",
      "00000005"    // .messageType = RIL_IMS_MESSAGE_TYPE_PAGER, // 5
      ;

  RIL_IMS_MessageWaitingInfo request{
    .messageSummaryLen = 2, // = 1
      .messageSummary = (RIL_IMS_MessageSummary *)&messageSummary,
      .ueAddress = "123456",
      .messageDetailsLen = 2, // = 2
      .messageDetails = (RIL_IMS_MessageDetails *)&messageDetails,
  };
  std::string s = std::string("");
  s += "00000002";    // .messageSummaryLen = 2, // = 1
  s += s_messageSummary;
  s += "00000006"    // .ueAddress length
       "313233343536",  // .ueAddress ("123456")
  s += "00000002";    // .messageDetailsLen = 2, // = 2
  s += s_messageDetails;


  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_MessageWaitingInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.messageSummaryLen, request_r.messageSummaryLen);
  RIL_IMS_MessageSummary *r_messageSummary = request_r.messageSummary;
  ASSERT_EQ(messageSummary[0].messageType, r_messageSummary[0].messageType);
  ASSERT_EQ(messageSummary[0].newMessageCount, r_messageSummary[0].newMessageCount);
  ASSERT_EQ(messageSummary[0].oldMessageCount, r_messageSummary[0].oldMessageCount);
  ASSERT_EQ(messageSummary[0].newUrgentMessageCount, r_messageSummary[0].newUrgentMessageCount);
  ASSERT_EQ(messageSummary[0].oldUrgentMessageCount, r_messageSummary[0].oldUrgentMessageCount);
  ASSERT_EQ(messageSummary[1].messageType, r_messageSummary[1].messageType);
  ASSERT_EQ(messageSummary[1].newMessageCount, r_messageSummary[1].newMessageCount);
  ASSERT_EQ(messageSummary[1].oldMessageCount, r_messageSummary[1].oldMessageCount);
  ASSERT_EQ(messageSummary[1].newUrgentMessageCount, r_messageSummary[1].newUrgentMessageCount);
  ASSERT_EQ(messageSummary[1].oldUrgentMessageCount, r_messageSummary[1].oldUrgentMessageCount);
  ASSERT_STREQ(request.ueAddress, request_r.ueAddress);
  ASSERT_EQ(request.messageDetailsLen, request_r.messageDetailsLen);
  RIL_IMS_MessageDetails *r_messageDetails = request_r.messageDetails;
  ASSERT_STREQ(messageDetails[0].toAddress, r_messageDetails[0].toAddress);
  ASSERT_STREQ(messageDetails[0].fromAddress, r_messageDetails[0].fromAddress);
  ASSERT_STREQ(messageDetails[0].subject, r_messageDetails[0].subject);
  ASSERT_STREQ(messageDetails[0].date, r_messageDetails[0].date);
  ASSERT_EQ(messageDetails[0].priority, r_messageDetails[0].priority);
  ASSERT_STREQ(messageDetails[0].messageId, r_messageDetails[0].messageId);
  ASSERT_EQ(messageDetails[0].messageType, r_messageDetails[0].messageType);
  ASSERT_STREQ(messageDetails[1].toAddress, r_messageDetails[1].toAddress);
  ASSERT_STREQ(messageDetails[1].fromAddress, r_messageDetails[1].fromAddress);
  ASSERT_STREQ(messageDetails[1].subject, r_messageDetails[1].subject);
  ASSERT_STREQ(messageDetails[1].date, r_messageDetails[1].date);
  ASSERT_EQ(messageDetails[1].priority, r_messageDetails[1].priority);
  ASSERT_STREQ(messageDetails[1].messageId, r_messageDetails[1].messageId);
  ASSERT_EQ(messageDetails[1].messageType, r_messageDetails[1].messageType);
}
