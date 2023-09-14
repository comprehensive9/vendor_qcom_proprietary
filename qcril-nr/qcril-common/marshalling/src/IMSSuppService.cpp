/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSSuppService.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CallBarringNumbersInfo>(
    const RIL_IMS_CallBarringNumbersInfo& arg) {
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallBarringNumbersInfo>(
    RIL_IMS_CallBarringNumbersInfo& arg) const {
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_CallBarringNumbersInfo>(
    RIL_IMS_CallBarringNumbersInfo& arg) {
  release(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_CallBarringNumbersListInfo>(
    const RIL_IMS_CallBarringNumbersListInfo& arg) {
  WRITE_AND_CHECK(arg.serviceClass);
  WRITE_AND_CHECK(arg.callBarringNumbersInfo, arg.callBarringNumbersInfoLen);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallBarringNumbersListInfo>(
    RIL_IMS_CallBarringNumbersListInfo& arg) const {
  READ_AND_CHECK(arg.serviceClass);
  RUN_AND_CHECK(readAndAlloc(arg.callBarringNumbersInfo, arg.callBarringNumbersInfoLen));
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_CallBarringNumbersListInfo>(
    RIL_IMS_CallBarringNumbersListInfo& arg) {
  release(arg.callBarringNumbersInfo, arg.callBarringNumbersInfoLen);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_SuppSvcRequest>(const RIL_IMS_SuppSvcRequest& arg) {
  WRITE_AND_CHECK(arg.operationType);
  WRITE_AND_CHECK(arg.facilityType);
  WRITE_AND_CHECK(arg.callBarringNumbersListInfo, 1);
  WRITE_AND_CHECK(arg.password);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SuppSvcRequest>(RIL_IMS_SuppSvcRequest& arg) const {
  READ_AND_CHECK(arg.operationType);
  READ_AND_CHECK(arg.facilityType);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.callBarringNumbersListInfo, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  READ_AND_CHECK(arg.password);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_SuppSvcResponse>(const RIL_IMS_SuppSvcResponse& arg) {
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.provisionStatus);
  WRITE_AND_CHECK(arg.facilityType);
  WRITE_AND_CHECK(arg.callBarringNumbersListInfo, arg.callBarringNumbersListInfoLen);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SuppSvcResponse>(RIL_IMS_SuppSvcResponse& arg) const {
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.provisionStatus);
  READ_AND_CHECK(arg.facilityType);
  RUN_AND_CHECK(readAndAlloc(arg.callBarringNumbersListInfo, arg.callBarringNumbersListInfoLen));
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_SuppSvcResponse>(RIL_IMS_SuppSvcResponse& arg) {
  release(arg.callBarringNumbersListInfo, arg.callBarringNumbersListInfoLen);
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}
