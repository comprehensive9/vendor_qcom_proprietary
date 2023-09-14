/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSGeolocationInfo.h>
#include <Marshal.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_GeolocationInfo>(const RIL_IMS_GeolocationInfo& arg) {
    WRITE_AND_CHECK(arg.latitude);
    WRITE_AND_CHECK(arg.longitude);
    WRITE_AND_CHECK(arg.countryCode);
    WRITE_AND_CHECK(arg.country);
    WRITE_AND_CHECK(arg.state);
    WRITE_AND_CHECK(arg.city);
    WRITE_AND_CHECK(arg.postalCode);
    WRITE_AND_CHECK(arg.street);
    WRITE_AND_CHECK(arg.houseNumber);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_GeolocationInfo>(RIL_IMS_GeolocationInfo& arg) const {
    READ_AND_CHECK(arg.latitude);
    READ_AND_CHECK(arg.longitude);
    std::size_t len = 0;
    RUN_AND_CHECK(readAndAlloc(arg.countryCode, len));
    RUN_AND_CHECK(readAndAlloc(arg.country, len));
    RUN_AND_CHECK(readAndAlloc(arg.state, len));
    RUN_AND_CHECK(readAndAlloc(arg.city, len));
    RUN_AND_CHECK(readAndAlloc(arg.postalCode, len));
    RUN_AND_CHECK(readAndAlloc(arg.street, len));
    RUN_AND_CHECK(readAndAlloc(arg.houseNumber, len));
    return Result::SUCCESS;
}
