/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <marshal/Dial.h>
#include <marshal/UUS_Info.h>


template <>
Marshal::Result Marshal::write<RIL_Dial>(const RIL_Dial &arg)
{
    write<char>(arg.address);
    write(arg.clir);
    int32_t uusPresent = 0;
    if (arg.uusInfo) {
        uusPresent = 1;
        write(uusPresent);
        write(*arg.uusInfo);
    } else {
        write(uusPresent);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Dial>(RIL_Dial &arg) const
{
    read(arg.address);
    read(arg.clir);
    int32_t uusPresent = 0;
    read(uusPresent);
    if (uusPresent != 0) {
        arg.uusInfo = new RIL_UUS_Info();
        if (arg.uusInfo) {
            read(*arg.uusInfo);
        }
    } else {
        arg.uusInfo = nullptr;
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_Dial>(RIL_Dial &arg) {
  release(arg.address);
  if (arg.uusInfo) {
    release(*arg.uusInfo);
    delete arg.uusInfo;
    arg.uusInfo = nullptr;
  }
  return Result::SUCCESS;
}
