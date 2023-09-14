/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <marshal/RILDataCallResponsev11.h>
#include <iostream>

template <>
Marshal::Result Marshal::write<RIL_Data_Call_Response_v11>(const RIL_Data_Call_Response_v11 &arg) {
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
    write(arg.mtu);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Data_Call_Response_v11>(RIL_Data_Call_Response_v11 &arg) const {
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
    read(arg.mtu);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_Data_Call_Response_v11>(RIL_Data_Call_Response_v11 &arg) {
    release(arg.status);
    release(arg.suggestedRetryTime);
    release(arg.cid);
    release(arg.active);
    release(arg.type);
    release(arg.ifname);
    release(arg.addresses);
    release(arg.dnses);
    release(arg.gateways);
    release(arg.pcscf);
    release(arg.mtu);
    return Result::SUCCESS;
}
