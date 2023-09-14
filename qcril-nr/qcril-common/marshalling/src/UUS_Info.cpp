/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/UUS_Info.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_UUS_Info>(const RIL_UUS_Info& arg) {
  write(static_cast<uint32_t>(arg.uusType));
  write(static_cast<uint32_t>(arg.uusDcs));
  if (arg.uusData) {
    write<char>(arg.uusData, arg.uusLength);
  } else {
    write((int32_t)-1);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_UUS_Info>(RIL_UUS_Info& arg) const {
  read(*reinterpret_cast<uint32_t*>(&arg.uusType));
  read(*reinterpret_cast<uint32_t*>(&arg.uusDcs));
  size_t length = 0;
  readAndAlloc(arg.uusData, length);
  if (!arg.uusData) {
    arg.uusLength = -1;
  } else {
    arg.uusLength = (uint32_t)length;
  }
  return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_UUS_Info>(RIL_UUS_Info &arg) {
  release(arg.uusData);
  arg.uusLength = 0;
  return Result::SUCCESS;
}
