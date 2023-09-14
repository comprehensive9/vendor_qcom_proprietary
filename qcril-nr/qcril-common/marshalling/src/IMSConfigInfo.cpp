/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSConfigInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ConfigInfo>(const RIL_IMS_ConfigInfo& arg) {
  WRITE_AND_CHECK(arg.item);
  WRITE_AND_CHECK(arg.hasBoolValue);
  WRITE_AND_CHECK(arg.boolValue);
  WRITE_AND_CHECK(arg.hasIntValue);
  WRITE_AND_CHECK(arg.intValue);
  WRITE_AND_CHECK(arg.stringValue);
  WRITE_AND_CHECK(arg.hasErrorCause);
  WRITE_AND_CHECK(arg.errorCause);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_ConfigInfo>(RIL_IMS_ConfigInfo& arg) const {
  READ_AND_CHECK(arg.item);
  READ_AND_CHECK(arg.hasBoolValue);
  READ_AND_CHECK(arg.boolValue);
  READ_AND_CHECK(arg.hasIntValue);
  READ_AND_CHECK(arg.intValue);
  READ_AND_CHECK(arg.stringValue);
  READ_AND_CHECK(arg.hasErrorCause);
  READ_AND_CHECK(arg.errorCause);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_ConfigInfo>(RIL_IMS_ConfigInfo& arg) {
  release(arg.stringValue);
  return Result::SUCCESS;
}

