/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ActivityInfo.h>

template <>
Marshal::Result Marshal::write<RIL_ActivityStatsInfo>(const RIL_ActivityStatsInfo& arg) {
    WRITE_AND_CHECK(arg.sleep_mode_time_ms);
    WRITE_AND_CHECK(arg.idle_mode_time_ms);
    WRITE_AND_CHECK(arg.tx_mode_time_ms);
    WRITE_AND_CHECK(arg.rx_mode_time_ms);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_ActivityStatsInfo>(RIL_ActivityStatsInfo& arg) const {
    READ_AND_CHECK(arg.sleep_mode_time_ms);
    READ_AND_CHECK(arg.idle_mode_time_ms);
    READ_AND_CHECK(arg.tx_mode_time_ms);
    READ_AND_CHECK(arg.rx_mode_time_ms);
    return Marshal::Result::SUCCESS;
}
