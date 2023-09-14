/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/OperatorInfo.h>

template <>
Marshal::Result Marshal::write<RIL_OperatorInfo>(const RIL_OperatorInfo& arg) {
  WRITE_AND_CHECK(arg.alphaLong);
  WRITE_AND_CHECK(arg.alphaShort);
  WRITE_AND_CHECK(arg.operatorNumeric);
  WRITE_AND_CHECK(static_cast<uint32_t>(arg.status));
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_OperatorInfo>(RIL_OperatorInfo& arg) const {
  READ_AND_CHECK(arg.alphaLong);
  READ_AND_CHECK(arg.alphaShort);
  READ_AND_CHECK(arg.operatorNumeric);
  READ_AND_CHECK(*reinterpret_cast<uint32_t*>(&arg.status));
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_OperatorInfo>(RIL_OperatorInfo& arg) {
  release(arg.alphaLong);
  release(arg.alphaShort);
  release(arg.operatorNumeric);
  return Result::SUCCESS;
}
