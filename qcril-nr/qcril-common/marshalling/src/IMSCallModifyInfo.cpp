/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSCallModifyInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CallModifyInfo>(const RIL_IMS_CallModifyInfo& arg) {
  WRITE_AND_CHECK(arg.callId);
  WRITE_AND_CHECK(arg.callType);
  WRITE_AND_CHECK(arg.callDomain);
  WRITE_AND_CHECK(arg.hasRttMode);
  WRITE_AND_CHECK(arg.rttMode);
  WRITE_AND_CHECK(arg.hasCallModifyFailCause);
  WRITE_AND_CHECK(arg.callModifyFailCause);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallModifyInfo>(RIL_IMS_CallModifyInfo& arg) const {
  READ_AND_CHECK(arg.callId);
  READ_AND_CHECK(arg.callType);
  READ_AND_CHECK(arg.callDomain);
  READ_AND_CHECK(arg.hasRttMode);
  READ_AND_CHECK(arg.rttMode);
  READ_AND_CHECK(arg.hasCallModifyFailCause);
  READ_AND_CHECK(arg.callModifyFailCause);
  return Result::SUCCESS;
}
