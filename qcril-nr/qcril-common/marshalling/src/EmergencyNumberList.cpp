/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/EmergencyNumberList.h>

template <>
Marshal::Result Marshal::write<RIL_EmergencyNumberInfo>(const RIL_EmergencyNumberInfo& arg) {
    WRITE_AND_CHECK(arg.number);
    WRITE_AND_CHECK(arg.mcc);
    WRITE_AND_CHECK(arg.mnc);
    WRITE_AND_CHECK(arg.category);
    WRITE_AND_CHECK(arg.sources);
    WRITE_AND_CHECK(arg.no_of_urns);
    for(int i=0; i<arg.no_of_urns; i++) {
        WRITE_AND_CHECK(arg.urn[i]);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_EmergencyNumberInfo>(RIL_EmergencyNumberInfo& arg) const {
    READ_AND_CHECK(arg.number);
    READ_AND_CHECK(arg.mcc);
    READ_AND_CHECK(arg.mnc);
    READ_AND_CHECK(arg.category);
    READ_AND_CHECK(arg.sources);
    READ_AND_CHECK(arg.no_of_urns);
    for(int i=0; i<arg.no_of_urns; i++) {
        READ_AND_CHECK(arg.urn[i]);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_EmergencyList>(const RIL_EmergencyList& arg) {
    WRITE_AND_CHECK(arg.emergency_list_length);
    for(int i=0; i<arg.emergency_list_length; i++) {
        WRITE_AND_CHECK(arg.emergency_numbers[i]);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_EmergencyList>(RIL_EmergencyList& arg) const {
    READ_AND_CHECK(arg.emergency_list_length);
    for(int i=0; i<arg.emergency_list_length; i++) {
        READ_AND_CHECK(arg.emergency_numbers[i]);
    }
    return Result::SUCCESS;
}
