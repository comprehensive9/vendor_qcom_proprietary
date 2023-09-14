/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSAnswer.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_Answer>(const RIL_IMS_Answer& arg) {
  WRITE_AND_CHECK(arg.callType);
  WRITE_AND_CHECK(arg.presentation);
  WRITE_AND_CHECK(arg.rttMode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_Answer>(RIL_IMS_Answer& arg) const {
  READ_AND_CHECK(arg.callType);
  READ_AND_CHECK(arg.presentation);
  READ_AND_CHECK(arg.rttMode);
  return Result::SUCCESS;
}

