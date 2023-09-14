/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/RILDataCallResponsev9.h>

template <>
Marshal::Result Marshal::write<RIL_Data_Call_Response_v9>(const RIL_Data_Call_Response_v9 &arg) {
    write(arg.status);
    write(arg.suggestedRetryTime);
    write(arg.cid);
    write(arg.active);
    write(arg.type);
    write(arg.ifname);
    write(arg.addresses);
    write(arg.dnses);
    write(arg.gateways);
    write(arg.pcscf);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Data_Call_Response_v9>(RIL_Data_Call_Response_v9 &arg) const {
    read(arg.status);
    read(arg.suggestedRetryTime);
    read(arg.cid);
    read(arg.active);
    read(arg.type);
    read(arg.ifname);
    read(arg.addresses);
    read(arg.dnses);
    read(arg.gateways);
    read(arg.pcscf);
    return Result::SUCCESS;
}


