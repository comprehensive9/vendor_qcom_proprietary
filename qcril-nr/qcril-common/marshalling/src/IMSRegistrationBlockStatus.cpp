/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSRegistrationBlockStatus.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_BlockReasonDetails>(const RIL_IMS_BlockReasonDetails& arg) {
  WRITE_AND_CHECK(arg.regFailureReasonType);
  WRITE_AND_CHECK(arg.hasRegFailureReason);
  WRITE_AND_CHECK(arg.regFailureReason);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_BlockReasonDetails>(RIL_IMS_BlockReasonDetails& arg) const {
  READ_AND_CHECK(arg.regFailureReasonType);
  READ_AND_CHECK(arg.hasRegFailureReason);
  READ_AND_CHECK(arg.regFailureReason);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_BlockStatus>(const RIL_IMS_BlockStatus& arg) {
  WRITE_AND_CHECK(arg.blockReason);
  WRITE_AND_CHECK(arg.hasBlockReasonDetails);
  WRITE_AND_CHECK(arg.blockReasonDetails);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_BlockStatus>(RIL_IMS_BlockStatus& arg) const {
  READ_AND_CHECK(arg.blockReason);
  READ_AND_CHECK(arg.hasBlockReasonDetails);
  READ_AND_CHECK(arg.blockReasonDetails);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_RegistrationBlockStatus>(const RIL_IMS_RegistrationBlockStatus& arg) {
  WRITE_AND_CHECK(arg.hasBlockStatusOnWwan);
  WRITE_AND_CHECK(arg.blockStatusOnWwan);
  WRITE_AND_CHECK(arg.hasBlockStatusOnWlan);
  WRITE_AND_CHECK(arg.blockStatusOnWlan);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_RegistrationBlockStatus>(RIL_IMS_RegistrationBlockStatus& arg) const {
  READ_AND_CHECK(arg.hasBlockStatusOnWwan);
  READ_AND_CHECK(arg.blockStatusOnWwan);
  READ_AND_CHECK(arg.hasBlockStatusOnWlan);
  READ_AND_CHECK(arg.blockStatusOnWlan);
  return Result::SUCCESS;
}
