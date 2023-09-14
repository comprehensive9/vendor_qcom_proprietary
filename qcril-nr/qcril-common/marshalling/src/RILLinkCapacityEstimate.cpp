/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/RILLinkCapacityEstimate.h>

template <>
Marshal::Result Marshal::write<RIL_LinkCapacityEstimate>(const RIL_LinkCapacityEstimate &arg) {
    WRITE_AND_CHECK(arg.downlinkCapacityKbps);
    WRITE_AND_CHECK(arg.uplinkCapacityKbps);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_LinkCapacityEstimate>(RIL_LinkCapacityEstimate &arg) const {
    READ_AND_CHECK(arg.downlinkCapacityKbps);
    READ_AND_CHECK(arg.uplinkCapacityKbps);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_LinkCapacityEstimate>(RIL_LinkCapacityEstimate &arg)
{
    release(arg.downlinkCapacityKbps);
    release(arg.uplinkCapacityKbps);
    return Result::SUCCESS;
}