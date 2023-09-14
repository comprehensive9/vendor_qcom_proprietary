/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSRegistrationBlockStatus.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSRegistrationBlockStatus_marshall_001) {
  Marshal marshal;
  RIL_IMS_RegistrationBlockStatus request{
    .hasBlockStatusOnWwan = 1,
      .blockStatusOnWwan = {
        .blockReason = RIL_IMS_BLOCK_REASON_TYPE_OTHER_FAILURE, // = 4
        .hasBlockReasonDetails = 1,
        .blockReasonDetails = {
          .regFailureReasonType = RIL_IMS_REG_FAILURE_REASON_TYPE_INTERNAL, // = 3
          .hasRegFailureReason = 1,
          .regFailureReason = 0xA,
        },
      },
      .hasBlockStatusOnWlan = 1,
      .blockStatusOnWlan = {
        .blockReason = RIL_IMS_BLOCK_REASON_TYPE_HANDOVER_FAILURE, // = 3
        .hasBlockReasonDetails = 1,
        .blockReasonDetails = {
          .regFailureReasonType = RIL_IMS_REG_FAILURE_REASON_TYPE_SPEC_DEFINED, // = 5
          .hasRegFailureReason = 1,
          .regFailureReason = 0x32,
        },
      },
  };

  std::string s =
      "01"    // .hasBlockStatusOnWwan = 1
      // .blockStatusOnWwan
        "00000004"    // .blockReason = RIL_IMS_BLOCK_REASON_TYPE_OTHER_FAILURE, // = 4
        "01"          // .hasBlockReasonDetails = 1,
        // .blockReasonDetails
          "00000003"    // .regFailureReasonType = RIL_IMS_REG_FAILURE_REASON_TYPE_INTERNAL, // = 3
          "01"          // .hasRegFailureReason = 1,
          "0000000A"    // .regFailureReason = 0xA,
      "01"    // .hasBlockStatusOnWlan = 1
      // .blockStatusOnWlan
        "00000003"    // .blockReason = RIL_IMS_BLOCK_REASON_TYPE_HANDOVER_FAILURE, // = 3
        "01"          // .hasBlockReasonDetails = 1,
        // .blockReasonDetails
          "00000005"    // .regFailureReasonType = RIL_IMS_REG_FAILURE_REASON_TYPE_SPEC_DEFINED, // = 5
          "01"          // .hasRegFailureReason = 1,
          "00000032"    // .regFailureReason = 0x32,
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_RegistrationBlockStatus request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.hasBlockStatusOnWwan, request_r.hasBlockStatusOnWwan);
  ASSERT_EQ(request.blockStatusOnWwan.blockReason, request_r.blockStatusOnWwan.blockReason);
  ASSERT_EQ(request.blockStatusOnWwan.hasBlockReasonDetails,
            request_r.blockStatusOnWwan.hasBlockReasonDetails);
  ASSERT_EQ(request.blockStatusOnWwan.blockReasonDetails.regFailureReasonType,
            request_r.blockStatusOnWwan.blockReasonDetails.regFailureReasonType);
  ASSERT_EQ(request.blockStatusOnWwan.blockReasonDetails.hasRegFailureReason,
            request_r.blockStatusOnWwan.blockReasonDetails.hasRegFailureReason);
  ASSERT_EQ(request.blockStatusOnWwan.blockReasonDetails.regFailureReason,
            request_r.blockStatusOnWwan.blockReasonDetails.regFailureReason);

  ASSERT_EQ(request.hasBlockStatusOnWlan, request_r.hasBlockStatusOnWlan);
  ASSERT_EQ(request.blockStatusOnWlan.blockReason, request_r.blockStatusOnWlan.blockReason);
  ASSERT_EQ(request.blockStatusOnWlan.hasBlockReasonDetails,
            request_r.blockStatusOnWlan.hasBlockReasonDetails);
  ASSERT_EQ(request.blockStatusOnWlan.blockReasonDetails.regFailureReasonType,
            request_r.blockStatusOnWlan.blockReasonDetails.regFailureReasonType);
  ASSERT_EQ(request.blockStatusOnWlan.blockReasonDetails.hasRegFailureReason,
            request_r.blockStatusOnWlan.blockReasonDetails.hasRegFailureReason);
  ASSERT_EQ(request.blockStatusOnWlan.blockReasonDetails.regFailureReason,
            request_r.blockStatusOnWlan.blockReasonDetails.regFailureReason);
}
