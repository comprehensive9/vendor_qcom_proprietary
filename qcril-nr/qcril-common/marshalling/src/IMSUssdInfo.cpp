/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSUssdInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_UssdInfo>(const RIL_IMS_UssdInfo& arg) {
  WRITE_AND_CHECK(arg.mode);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_UssdInfo>(RIL_IMS_UssdInfo& arg) const {
  READ_AND_CHECK(arg.mode);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_UssdInfo>(RIL_IMS_UssdInfo& arg) {
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}
