/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <iostream>
#include <Marshal.h>
#include <marshal/RILSetUpDataCallParams.h>

template <>
Marshal::Result Marshal::write<RIL_SetUpDataCallParams>(const RIL_SetUpDataCallParams &arg) {
    WRITE_AND_CHECK(static_cast<int>(arg.accessNetwork));
    WRITE_AND_CHECK(arg.profileInfo);
    WRITE_AND_CHECK(arg.roamingAllowed);
    WRITE_AND_CHECK(static_cast<int>(arg.reason));
    WRITE_AND_CHECK(arg.addresses);
    WRITE_AND_CHECK(arg.dnses);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SetUpDataCallParams>(RIL_SetUpDataCallParams &arg) const {
    READ_AND_CHECK(arg.accessNetwork);
    READ_AND_CHECK(arg.profileInfo);
    READ_AND_CHECK(arg.roamingAllowed);
    READ_AND_CHECK(arg.reason);
    READ_AND_CHECK(arg.addresses);
    READ_AND_CHECK(arg.dnses);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SetUpDataCallParams>(RIL_SetUpDataCallParams &arg)
{
    release(arg.accessNetwork);
    release(arg.profileInfo);
    release(arg.roamingAllowed);
    release(arg.reason);
    release(arg.addresses);
    release(arg.dnses);
    return Result::SUCCESS;
}
