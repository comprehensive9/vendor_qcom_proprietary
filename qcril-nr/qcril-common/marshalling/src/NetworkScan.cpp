/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CellInfo.h>
#include <marshal/NetworkScan.h>

template <>
Marshal::Result Marshal::write<RIL_NetworkScanRequest>(const RIL_NetworkScanRequest& arg) {
    WRITE_AND_CHECK(static_cast<int>(arg.type));
    WRITE_AND_CHECK(arg.interval);
    WRITE_AND_CHECK(arg.specifiers_length);
    for (int i = 0; i < arg.specifiers_length; i++) {
        WRITE_AND_CHECK(arg.specifiers[i]);
    }
    WRITE_AND_CHECK(arg.maxSearchTime);
    WRITE_AND_CHECK(arg.incrementalResults);
    WRITE_AND_CHECK(arg.incrementalResultsPeriodicity);
    WRITE_AND_CHECK(arg.mccMncLength);
    for (int i = 0; i < arg.mccMncLength; i++) {
        WRITE_AND_CHECK(arg.mccMncs[i]);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_NetworkScanRequest>(RIL_NetworkScanRequest& arg) const {
    READ_AND_CHECK(arg.type);
    READ_AND_CHECK(arg.interval);
    READ_AND_CHECK(arg.specifiers_length);
    for (int i = 0; i < arg.specifiers_length; i++) {
        READ_AND_CHECK(arg.specifiers[i]);
    }
    READ_AND_CHECK(arg.maxSearchTime);
    READ_AND_CHECK(arg.incrementalResults);
    READ_AND_CHECK(arg.incrementalResultsPeriodicity);
    READ_AND_CHECK(arg.mccMncLength);
    for (int i = 0; i < arg.mccMncLength; i++) {
        READ_AND_CHECK(arg.mccMncs[i]);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_NetworkScanResult>(const RIL_NetworkScanResult& arg) {
    WRITE_AND_CHECK(arg.status);
    WRITE_AND_CHECK(arg.network_infos, arg.network_infos_length);
    WRITE_AND_CHECK(arg.error);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_NetworkScanResult>(RIL_NetworkScanResult& arg) const {
    READ_AND_CHECK(arg.status);
    RUN_AND_CHECK(readAndAlloc(arg.network_infos, arg.network_infos_length));
    READ_AND_CHECK(arg.error);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_NetworkScanResult>(RIL_NetworkScanResult& arg) {
    release(arg.network_infos, arg.network_infos_length);
    arg.network_infos_length = 0;
    return Result::SUCCESS;
}
