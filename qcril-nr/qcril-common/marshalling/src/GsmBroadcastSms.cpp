/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/GsmBroadcastSms.h>
#include <Marshal.h>

template <>
Marshal::Result Marshal::write<RIL_GSM_BroadcastSmsConfigInfo>(
    const RIL_GSM_BroadcastSmsConfigInfo& arg) {
  WRITE_AND_CHECK(arg.fromServiceId);
  WRITE_AND_CHECK(arg.toServiceId);
  WRITE_AND_CHECK(arg.fromCodeScheme);
  WRITE_AND_CHECK(arg.toCodeScheme);
  WRITE_AND_CHECK(arg.selected);
  return Marshal::Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_GSM_BroadcastSmsConfigInfo>(
    RIL_GSM_BroadcastSmsConfigInfo& arg) const {
  READ_AND_CHECK(arg.fromServiceId);
  READ_AND_CHECK(arg.toServiceId);
  READ_AND_CHECK(arg.fromCodeScheme);
  READ_AND_CHECK(arg.toCodeScheme);
  READ_AND_CHECK(arg.selected);
  return Marshal::Result::SUCCESS;
}
