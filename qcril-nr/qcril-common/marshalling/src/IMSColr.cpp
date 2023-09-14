/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSColr.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_ColrInfo>(const RIL_IMS_ColrInfo& arg) {
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.provisionStatus);
  WRITE_AND_CHECK(arg.presentation);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_ColrInfo>(RIL_IMS_ColrInfo& arg) const {
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.provisionStatus);
  READ_AND_CHECK(arg.presentation);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) return Result::FAILURE;
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_ColrInfo>(RIL_IMS_ColrInfo& arg) {
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}

