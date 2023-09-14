/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CallForwardInfo.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CallForwardInfo>(const RIL_CallForwardInfo& arg) {
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.reason);
  WRITE_AND_CHECK(arg.serviceClass);
  WRITE_AND_CHECK(arg.toa);
  if (arg.number) {
    WRITE_AND_CHECK(arg.number);
  } else {
    WRITE_AND_CHECK((int32_t)-1);
  }
  WRITE_AND_CHECK(arg.timeSeconds);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallForwardInfo>(RIL_CallForwardInfo& arg) const {
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.reason);
  READ_AND_CHECK(arg.serviceClass);
  READ_AND_CHECK(arg.toa);
  READ_AND_CHECK(arg.number);
  READ_AND_CHECK(arg.timeSeconds);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallForwardInfo>(RIL_CallForwardInfo& arg) {
  release(arg.number);
  return Result::SUCCESS;
}
