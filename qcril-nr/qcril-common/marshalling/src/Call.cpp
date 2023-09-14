/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/Call.h>
#include <marshal/UUS_Info.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_Call>(const RIL_Call& arg) {
  write(arg.state);
  write(arg.index);
  write(arg.toa);
  write(arg.isMpty);
  write(arg.isMT);
  write(arg.als);
  write(arg.isVoice);
  write(arg.isVoicePrivacy);
  if (arg.number) {
    write<char>(arg.number);
  } else {
    write((int32_t)-1);
  }
  write(arg.numberPresentation);
  if (arg.name) {
    write<char>(arg.name);
  } else {
    write((int32_t)-1);
  }
  write(arg.namePresentation);
  int32_t uusPresent = 0;
  if (arg.uusInfo) {
    uusPresent = 1;
    write(uusPresent);
    write(*(arg.uusInfo));
  } else {
    write(uusPresent);
  }
  write(arg.audioQuality);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Call>(RIL_Call& arg) const {
  read(arg.state);
  read(arg.index);
  read(arg.toa);
  read(arg.isMpty);
  read(arg.isMT);
  read(arg.als);
  read(arg.isVoice);
  read(arg.isVoicePrivacy);
  read(arg.number);
  read(arg.numberPresentation);
  read(arg.name);
  read(arg.namePresentation);
  int32_t uusPresent = 0;
  read(uusPresent);
  if (uusPresent) {
    arg.uusInfo = new RIL_UUS_Info();
    if (arg.uusInfo) {
      read(*(arg.uusInfo));
    }
  }
  read(arg.audioQuality);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_Call>(RIL_Call& arg) {
  release(arg.state);
  release(arg.index);
  release(arg.toa);
  release(arg.isMpty);
  release(arg.isMT);
  release(arg.als);
  release(arg.isVoice);
  release(arg.isVoicePrivacy);
  release(arg.number);
  release(arg.numberPresentation);
  release(arg.name);
  release(arg.namePresentation);
  if (arg.uusInfo) {
    release(*arg.uusInfo);
    delete arg.uusInfo;
    arg.uusInfo = nullptr;
  }
  release(arg.audioQuality);
  release(arg.redirNum);
  release(arg.redirNumPresentation);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CallState>(const RIL_CallState& arg) {
  //  using inttype = std::underlying_type<RIL_CallState>::type;
  //  return write(*reinterpret_cast<const inttype *>(&arg));
  return write(static_cast<uint32_t>(arg));
}
template <>
Marshal::Result Marshal::read<RIL_CallState>(RIL_CallState& arg) const {
  //    using inttype = std::underlying_type<RIL_CallState>::type;
  //    return read(*reinterpret_cast<inttype *>(&arg));
  return read(*reinterpret_cast<uint32_t*>(&arg));
}

template <>
Marshal::Result Marshal::write<RIL_AudioQuality>(const RIL_AudioQuality& arg) {
  //  using inttype = std::underlying_type<RIL_AudioQuality>::type;
  //  return write(*reinterpret_cast<const inttype *>(&arg));
  return write(static_cast<uint32_t>(arg));
}
template <>
Marshal::Result Marshal::read<RIL_AudioQuality>(RIL_AudioQuality& arg) const {
  //  using inttype = std::underlying_type<RIL_AudioQuality>::type;
  //  return read(*reinterpret_cast<inttype *>(&arg));
  return read(*reinterpret_cast<uint32_t*>(&arg));
}
