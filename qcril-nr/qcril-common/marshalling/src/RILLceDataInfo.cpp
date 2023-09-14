/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/RILLceDataInfo.h>

template <>
Marshal::Result Marshal::write<RIL_LceDataInfo>(const RIL_LceDataInfo &arg) {
    WRITE_AND_CHECK(arg.last_hop_capacity_kbps);
    WRITE_AND_CHECK(arg.confidence_level);
    WRITE_AND_CHECK(arg.lce_suspended);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_LceDataInfo>(RIL_LceDataInfo &arg) const {
    READ_AND_CHECK(arg.last_hop_capacity_kbps);
    READ_AND_CHECK(arg.confidence_level);
    READ_AND_CHECK(arg.lce_suspended);
    return Result::SUCCESS;
}
