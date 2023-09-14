/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/Dial.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Call_marshall_write_001) {
  Marshal marshal;
  RIL_Call request{ .state = RIL_CALL_ALERTING,
                    .index = 1,
                    .toa = 2,
                    .isMpty = 0,
                    .isMT = 0,
                    .als = 0,
                    .isVoice = 0,
                    .isVoicePrivacy = 0,
                    .number = "12345",
                    .numberPresentation = 1,
                    .name = nullptr,
                    .namePresentation = 1,
                    .uusInfo = nullptr,
                    .audioQuality = RIL_AUDIO_QUAL_AMR_WB /*2*/ };

  std::string s =
      "00000003"    // RIL_CALL_ALERTING (3)
      "00000001"    // index
      "00000002"    // toa
      "00"          // isMpty
      "00"          // isMT
      "00"          // als
      "00"          // isVoice
      "00"          // isVoicePrivacy
      "00000005"    // number length
      "3132333435"  // number ("12345")
      "00000001"    // numberPresentation
      "ffffffff"    // name length
      "00000001"    // namePresentation
      "00000000"    // uusPresent
      "00000002"    // RIL_AUDIO_QUAL_AMR_WB (2)
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_Call request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.state, request_r.state);
  ASSERT_EQ(request.index, request_r.index);
  ASSERT_EQ(request.toa, request_r.toa);
  ASSERT_EQ(request.isMpty, request_r.isMpty);
  ASSERT_EQ(request.isMT, request_r.isMT);
  ASSERT_EQ(request.als, request_r.als);
  ASSERT_EQ(request.isVoice, request_r.isVoice);
  ASSERT_EQ(request.isVoicePrivacy, request_r.isVoicePrivacy);
  ASSERT_STREQ(request.number, request_r.number);
  ASSERT_EQ(request.numberPresentation, request_r.numberPresentation);
  ASSERT_EQ(request.namePresentation, request_r.namePresentation);
  ASSERT_EQ(request.audioQuality, request_r.audioQuality);
}

TEST(Marshalling, Call_marshall_write_002) {
  Marshal marshal;
  RIL_UUS_Info uusInfo{ .uusType = RIL_UUS_TYPE2_REQUIRED, .uusDcs = RIL_UUS_DCS_X244, 5, "12345" };
  RIL_Call request{ .state = RIL_CALL_ALERTING,
                    .index = 1,
                    .toa = 2,
                    .isMpty = 0,
                    .isMT = 0,
                    .als = 0,
                    .isVoice = 0,
                    .isVoicePrivacy = 0,
                    .number = "12345",
                    .numberPresentation = 1,
                    .name = "TEST",
                    .namePresentation = 1,
                    .uusInfo = &uusInfo,
                    .audioQuality = RIL_AUDIO_QUAL_AMR_WB /*2*/ };

  std::string s =
      "00000003"    // RIL_CALL_ALERTING (3)
      "00000001"    // index
      "00000002"    // toa
      "00"          // isMpty
      "00"          // isMT
      "00"          // als
      "00"          // isVoice
      "00"          // isVoicePrivacy
      "00000005"    // number length
      "3132333435"  // number ("12345")
      "00000001"    // numberPresentation
      "00000004"    // name length
      "54455354"    // name ("TEST")
      "00000001"    // namePresentation
      "00000001"    // uusPresent
      "00000003"    // UUS Type
      "00000002"    // UUS DCS
      "00000005"    // Data length (5)
      "3132333435"  // Data ("12345")
      "00000002"    // RIL_AUDIO_QUAL_AMR_WB (2)
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_Call request_r = {};
  Marshal marshal_r;
  marshal_r.setData(expected);
  ASSERT_NE(marshal_r.read(request_r), Marshal::Result::FAILURE);
  // marshal.setDataPosition(0);
  // ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.state, request_r.state);
  ASSERT_EQ(request.index, request_r.index);
  ASSERT_EQ(request.toa, request_r.toa);
  ASSERT_EQ(request.isMpty, request_r.isMpty);
  ASSERT_EQ(request.isMT, request_r.isMT);
  ASSERT_EQ(request.als, request_r.als);
  ASSERT_EQ(request.isVoice, request_r.isVoice);
  ASSERT_EQ(request.isVoicePrivacy, request_r.isVoicePrivacy);
  ASSERT_STREQ(request.number, request_r.number);
  ASSERT_EQ(request.numberPresentation, request_r.numberPresentation);
  ASSERT_STREQ(request.name, request_r.name);
  ASSERT_EQ(request.namePresentation, request_r.namePresentation);
  ASSERT_EQ(request.uusInfo->uusType, request_r.uusInfo->uusType);
  ASSERT_EQ(request.uusInfo->uusDcs, request_r.uusInfo->uusDcs);
  ASSERT_EQ(request.uusInfo->uusLength, request_r.uusInfo->uusLength);
  ASSERT_STREQ(request.uusInfo->uusData, request_r.uusInfo->uusData);
  ASSERT_EQ(request.audioQuality, request_r.audioQuality);
}

TEST(Marshalling, Call_marshall_write_003) {
  Marshal marshal;
  RIL_UUS_Info uusInfo{ .uusType = RIL_UUS_TYPE2_REQUIRED, .uusDcs = RIL_UUS_DCS_X244, 5, "12345" };
  RIL_Call request[2] = {
    { .state = RIL_CALL_ALERTING,
      .index = 2,
      .toa = 2,
      .isMpty = 0,
      .isMT = 0,
      .als = 0,
      .isVoice = 0,
      .isVoicePrivacy = 0,
      .number = "12345",
      .numberPresentation = 1,
      .name = nullptr,
      .namePresentation = 1,
      .uusInfo = &uusInfo,
      .audioQuality = RIL_AUDIO_QUAL_AMR_WB /*2*/ },
    { .state = RIL_CALL_HOLDING,
      .index = 1,
      .toa = 2,
      .isMpty = 0,
      .isMT = 0,
      .als = 0,
      .isVoice = 0,
      .isVoicePrivacy = 0,
      .number = "67890",
      .numberPresentation = 1,
      .name = nullptr,
      .namePresentation = 1,
      .uusInfo = nullptr,
      .audioQuality = RIL_AUDIO_QUAL_AMR_WB /*2*/ },
  };

  std::string s =
      "00000002"  // number of RIL_Call's
      // CallID 2
      "00000003"    // RIL_CALL_ALERTING (3)
      "00000002"    // index
      "00000002"    // toa
      "00"          // isMpty
      "00"          // isMT
      "00"          // als
      "00"          // isVoice
      "00"          // isVoicePrivacy
      "00000005"    // number length
      "3132333435"  // number ("12345")
      "00000001"    // numberPresentation
      "ffffffff"    // name length
      "00000001"    // namePresentation
      "00000001"    // uusPresent
      "00000003"    // UUS Type
      "00000002"    // UUS DCS
      "00000005"    // Data length (5)
      "3132333435"  // Data ("12345")
      "00000002"    // RIL_AUDIO_QUAL_AMR_WB (2)
      // CallID 1
      "00000001"    // RIL_CALL_HOLDING (1)
      "00000001"    // index
      "00000002"    // toa
      "00"          // isMpty
      "00"          // isMT
      "00"          // als
      "00"          // isVoice
      "00"          // isVoicePrivacy
      "00000005"    // number length
      "3637383930"  // number ("67890")
      "00000001"    // numberPresentation
      "ffffffff"    // name length
      "00000001"    // namePresentation
      "00000000"    // uusPresent
      "00000002"    // RIL_AUDIO_QUAL_AMR_WB (2)
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  int32_t numCalls = 2;
  ASSERT_NE(marshal.write(numCalls), Marshal::Result::FAILURE);
  ASSERT_NE(marshal.write(request[0]), Marshal::Result::FAILURE);
  ASSERT_NE(marshal.write(request[1]), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}
