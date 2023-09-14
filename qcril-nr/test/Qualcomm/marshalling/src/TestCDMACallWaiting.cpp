/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/CdmaCallWaiting.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, CDMACallWaiting_marshall_001) {
  Marshal marshal;
  RIL_CDMA_CallWaiting_v6 request{ .number = "123456",
                    .numberPresentation = 1,
                    .name = "ABC",
                    .signalInfoRecord = {
                      .isPresent = 1,
                      .signalType = 2,
                      .alertPitch = 3,
                      .signal = 4, },
                    .number_type = 2,
                    .number_plan = 1, };

  std::string s =
      "00000006"     // number length
      "313233343536" // number ("123456")
      "00000001"     // numberPresentation = 1
      "00000003"     // name length
      "414243"       // name ("ABC")
      "01"           //  isPresent = 1
      "02"           //  signalType = 2
      "03"           //  alertPitch = 3
      "04"           //  signal = 4
      "00000002"     // number_type = 2
      "00000001"     // number_plan = 1
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_CDMA_CallWaiting_v6 request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_STREQ(request.number, request_r.number);
  ASSERT_EQ(request.numberPresentation, request_r.numberPresentation);
  ASSERT_STREQ(request.name, request_r.name);
  ASSERT_EQ(request.signalInfoRecord.isPresent, request_r.signalInfoRecord.isPresent);
  ASSERT_EQ(request.signalInfoRecord.signalType, request_r.signalInfoRecord.signalType);
  ASSERT_EQ(request.signalInfoRecord.alertPitch, request_r.signalInfoRecord.alertPitch);
  ASSERT_EQ(request.signalInfoRecord.signal, request_r.signalInfoRecord.signal);
  ASSERT_EQ(request.number_type, request_r.number_type);
  ASSERT_EQ(request.number_plan, request_r.number_plan);
}
