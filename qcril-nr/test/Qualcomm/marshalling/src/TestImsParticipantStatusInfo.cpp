/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSParticipantStatusInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSParticipantStatusInfo_marshall_001) {
  Marshal marshal;
  RIL_IMS_ParticipantStatusInfo request{
    .callId = 1,
      .operation = RIL_IMS_CONF_PARTICIPANT_OP_ADD,  // = 1
      .sipStatus = 404,
      .participantUri = "123456",
      .hasIsEct = 1,
      .isEct = 1,
  };

  std::string s =
      "00000001"    // .callId = 1
      "00000001"    // .operation = 1
      "00000194"    // .sipStatus = 404
      "00000006"    // .participantUri length = 6,
      "313233343536" // .participantUri ("123456")
      "01"          // .hasIsEct = 1
      "01"          // .isEct = 1,
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_ParticipantStatusInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.callId, request_r.callId);
  ASSERT_EQ(request.operation, request_r.operation);
  ASSERT_EQ(request.sipStatus, request_r.sipStatus);
  ASSERT_STREQ(request.participantUri, request_r.participantUri);
  ASSERT_EQ(request.hasIsEct, request_r.hasIsEct);
  ASSERT_EQ(request.isEct, request_r.isEct);
}
