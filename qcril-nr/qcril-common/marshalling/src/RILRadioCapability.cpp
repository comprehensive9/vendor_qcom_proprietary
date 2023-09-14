/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <type_traits>
#include <telephony/ril.h>
#include <marshal/RILRadioCapability.h>
#include <Marshal.h>

template <>
Marshal::Result Marshal::write<RIL_RadioCapability>(const RIL_RadioCapability &arg) {
    WRITE_AND_CHECK(arg.version);
    WRITE_AND_CHECK(arg.session);
    WRITE_AND_CHECK(arg.phase);
    WRITE_AND_CHECK(arg.rat);
    WRITE_AND_CHECK(arg.logicalModemUuid);
    WRITE_AND_CHECK(arg.status);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_RadioCapability>(RIL_RadioCapability &arg) const {
    READ_AND_CHECK(arg.version);
    READ_AND_CHECK(arg.session);
    READ_AND_CHECK(arg.phase);
    READ_AND_CHECK(arg.rat);
    READ_AND_CHECK(arg.logicalModemUuid);
    READ_AND_CHECK(arg.status);
    return Result::SUCCESS;
}
