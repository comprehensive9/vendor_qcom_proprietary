/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/LastCallFailCauseResponse.h>

template <>
Marshal::Result Marshal::write<RIL_LastCallFailCauseInfo>(const RIL_LastCallFailCauseInfo& arg) {
  write(static_cast<uint32_t>(arg.cause_code));
  int32_t vendorCausePresent = 0;
  if (arg.vendor_cause) {
    vendorCausePresent = 1;
    write(vendorCausePresent);
    write<char>(arg.vendor_cause);
  } else {
    write(vendorCausePresent);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_LastCallFailCauseInfo>(RIL_LastCallFailCauseInfo& arg) const {
  read(*reinterpret_cast<uint32_t*>(&arg.cause_code));
  int32_t vendorCausePresent = 0;
  read(vendorCausePresent);
  if (vendorCausePresent != 0) {
    read(arg.vendor_cause);
  } else {
    arg.vendor_cause = nullptr;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_LastCallFailCauseInfo>(RIL_LastCallFailCauseInfo& arg) {
  release(arg.vendor_cause);
  return Result::SUCCESS;
}
