/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSSuppSvcNotification.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_SuppSvcNotification>(const RIL_IMS_SuppSvcNotification& arg) {
  WRITE_AND_CHECK(arg.notificationType);
  WRITE_AND_CHECK(arg.code);
  WRITE_AND_CHECK(arg.index);
  WRITE_AND_CHECK(arg.type);
  WRITE_AND_CHECK(arg.number);
  WRITE_AND_CHECK(arg.connId);
  WRITE_AND_CHECK(arg.historyInfo);
  WRITE_AND_CHECK(arg.hasHoldTone);
  WRITE_AND_CHECK(arg.holdTone);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SuppSvcNotification>(RIL_IMS_SuppSvcNotification& arg) const {
  READ_AND_CHECK(arg.notificationType);
  READ_AND_CHECK(arg.code);
  READ_AND_CHECK(arg.index);
  READ_AND_CHECK(arg.type);
  READ_AND_CHECK(arg.number);
  READ_AND_CHECK(arg.connId);
  READ_AND_CHECK(arg.historyInfo);
  READ_AND_CHECK(arg.hasHoldTone);
  READ_AND_CHECK(arg.holdTone);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_SuppSvcNotification>(RIL_IMS_SuppSvcNotification& arg)  {
  release(arg.number);
  release(arg.historyInfo);
  return Result::SUCCESS;
}
