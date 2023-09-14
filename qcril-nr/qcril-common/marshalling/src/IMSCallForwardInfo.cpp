/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSCallForwardInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CallFwdTimerInfo>(const RIL_IMS_CallFwdTimerInfo& arg) {
  WRITE_AND_CHECK(arg.year);
  WRITE_AND_CHECK(arg.month);
  WRITE_AND_CHECK(arg.day);
  WRITE_AND_CHECK(arg.hour);
  WRITE_AND_CHECK(arg.minute);
  WRITE_AND_CHECK(arg.second);
  WRITE_AND_CHECK(arg.timezone);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallFwdTimerInfo>(RIL_IMS_CallFwdTimerInfo& arg) const {
  READ_AND_CHECK(arg.year);
  READ_AND_CHECK(arg.month);
  READ_AND_CHECK(arg.day);
  READ_AND_CHECK(arg.hour);
  READ_AND_CHECK(arg.minute);
  READ_AND_CHECK(arg.second);
  READ_AND_CHECK(arg.timezone);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_CallForwardInfo>(const RIL_IMS_CallForwardInfo& arg) {
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.reason);
  WRITE_AND_CHECK(arg.serviceClass);
  WRITE_AND_CHECK(arg.toa);
  WRITE_AND_CHECK(arg.number);
  WRITE_AND_CHECK(arg.timeSeconds);
  WRITE_AND_CHECK(arg.hasCallFwdTimerStart);
  if (arg.hasCallFwdTimerStart) {
    WRITE_AND_CHECK(arg.callFwdTimerStart);
  }
  WRITE_AND_CHECK(arg.hasCallFwdTimerEnd);
  if (arg.hasCallFwdTimerEnd) {
    WRITE_AND_CHECK(arg.callFwdTimerEnd);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallForwardInfo>(RIL_IMS_CallForwardInfo& arg) const {
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.reason);
  READ_AND_CHECK(arg.serviceClass);
  READ_AND_CHECK(arg.toa);
  READ_AND_CHECK(arg.number);
  READ_AND_CHECK(arg.timeSeconds);
  READ_AND_CHECK(arg.hasCallFwdTimerStart);
  if (arg.hasCallFwdTimerStart) {
    READ_AND_CHECK(arg.callFwdTimerStart);
  }
  READ_AND_CHECK(arg.hasCallFwdTimerEnd);
  if (arg.hasCallFwdTimerEnd) {
    READ_AND_CHECK(arg.callFwdTimerEnd);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_CallForwardInfo>(RIL_IMS_CallForwardInfo& arg) {
  release(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_QueryCallForwardStatusInfo>(
    const RIL_IMS_QueryCallForwardStatusInfo& arg) {
  WRITE_AND_CHECK(arg.callForwardInfo, arg.callForwardInfoLen);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_QueryCallForwardStatusInfo>(
    RIL_IMS_QueryCallForwardStatusInfo& arg) const {
  RUN_AND_CHECK(readAndAlloc(arg.callForwardInfo, arg.callForwardInfoLen));
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_QueryCallForwardStatusInfo>(
    RIL_IMS_QueryCallForwardStatusInfo& arg) {
  release(arg.callForwardInfo, arg.callForwardInfoLen);
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_SetCallForwardStatus>(const RIL_IMS_SetCallForwardStatus& arg) {
  WRITE_AND_CHECK(arg.reason);
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SetCallForwardStatus>(RIL_IMS_SetCallForwardStatus& arg) const {
  READ_AND_CHECK(arg.reason);
  READ_AND_CHECK(arg.status);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_SetCallForwardStatus>(RIL_IMS_SetCallForwardStatus& arg)  {
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_SetCallForwardStatusInfo>(
    const RIL_IMS_SetCallForwardStatusInfo& arg) {
  WRITE_AND_CHECK(arg.setCallForwardStatus, arg.setCallForwardStatusLen);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SetCallForwardStatusInfo>(
    RIL_IMS_SetCallForwardStatusInfo& arg) const {
  RUN_AND_CHECK(readAndAlloc(arg.setCallForwardStatus, arg.setCallForwardStatusLen));
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_SetCallForwardStatusInfo>(
    RIL_IMS_SetCallForwardStatusInfo& arg) {
  release(arg.setCallForwardStatus, arg.setCallForwardStatusLen);
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}
