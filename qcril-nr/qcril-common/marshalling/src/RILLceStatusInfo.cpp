/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/RILLceStatusInfo.h>

template <>
Marshal::Result Marshal::write<RIL_LceStatusInfo>(const RIL_LceStatusInfo &arg) {
    WRITE_AND_CHECK(arg.lce_status);
    WRITE_AND_CHECK(arg.actual_interval_ms);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_LceStatusInfo>(RIL_LceStatusInfo &arg) const {
    READ_AND_CHECK(arg.lce_status);
    READ_AND_CHECK(arg.actual_interval_ms);
    return Result::SUCCESS;
}
