/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CdmaCallWaiting.h>
#include <marshal/CdmaSignalInfoRecord.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CDMA_CallWaiting_v6>(const RIL_CDMA_CallWaiting_v6& arg) {
  if (arg.number) {
    WRITE_AND_CHECK(arg.number);
  } else {
    WRITE_AND_CHECK((int32_t)-1);
  }
  WRITE_AND_CHECK(arg.numberPresentation);
  if (arg.name) {
    WRITE_AND_CHECK(arg.name);
  } else {
    WRITE_AND_CHECK((int32_t)-1);
  }
  WRITE_AND_CHECK(arg.signalInfoRecord);
  WRITE_AND_CHECK(arg.number_type);
  WRITE_AND_CHECK(arg.number_plan);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_CallWaiting_v6>(RIL_CDMA_CallWaiting_v6& arg) const {
  READ_AND_CHECK(arg.number);
  READ_AND_CHECK(arg.numberPresentation);
  READ_AND_CHECK(arg.name);
  READ_AND_CHECK(arg.signalInfoRecord);
  READ_AND_CHECK(arg.number_type);
  READ_AND_CHECK(arg.number_plan);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CDMA_CallWaiting_v6>(RIL_CDMA_CallWaiting_v6& arg) {
  release(arg.number);
  release(arg.name);
  return Result::SUCCESS;
}
