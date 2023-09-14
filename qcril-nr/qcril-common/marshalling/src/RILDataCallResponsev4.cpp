/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/RILDataCallResponsev4.h>

template <>
Marshal::Result Marshal::write<RIL_Data_Call_Response_v4>(const RIL_Data_Call_Response_v4 &arg) {
    write(arg.cid);
    write(arg.active);
    write(arg.type);
    write(arg.apn);
    write(arg.address);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Data_Call_Response_v4>(RIL_Data_Call_Response_v4 &arg) const {
    read(arg.cid);
    read(arg.active);
    read(arg.type);
    read(arg.apn);
    read(arg.address);
    return Result::SUCCESS;
}
