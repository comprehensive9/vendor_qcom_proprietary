/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <type_traits>
#include <telephony/ril.h>
#include <marshal/CdmaSubscription.h>
#include <Marshal.h>

template <>
Marshal::Result Marshal::write<RIL_CDMASubInfo>(const RIL_CDMASubInfo &arg) {
    write(arg.mdn);
    write(arg.hSid);
    write(arg.hNid);
    write(arg.min);
    write(arg.prl);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMASubInfo>(RIL_CDMASubInfo &arg) const {
    read(arg.mdn);
    read(arg.hSid);
    read(arg.hNid);
    read(arg.min);
    read(arg.prl);

    return Result::SUCCESS;
}
