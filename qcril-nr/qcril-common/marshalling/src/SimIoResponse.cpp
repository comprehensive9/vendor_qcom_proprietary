/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <telephony/ril.h>
#include <marshal/SimIoResponse.h>

template <>
Marshal::Result Marshal::write<RIL_SIM_IO_Response>(const RIL_SIM_IO_Response& arg) {

  WRITE_AND_CHECK(arg.sw1);
  WRITE_AND_CHECK(arg.sw2);
  WRITE_AND_CHECK(arg.simResponse);
  
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SIM_IO_Response>(RIL_SIM_IO_Response& arg) const {

  READ_AND_CHECK(arg.sw1);
  READ_AND_CHECK(arg.sw2);
  READ_AND_CHECK(arg.simResponse);
  
  return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_SIM_IO_Response>(RIL_SIM_IO_Response& arg) {
  release(arg.simResponse);
  return Result::SUCCESS;
}
