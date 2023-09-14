/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CarrierInfoForImsiEncryption.h>


template <>
Marshal::Result Marshal::write<RIL_CarrierInfoForImsiEncryption>(const RIL_CarrierInfoForImsiEncryption &arg) {
    WRITE_AND_CHECK(arg.mcc);
    WRITE_AND_CHECK(arg.mnc);
    WRITE_AND_CHECK(arg.carrierKey, static_cast<size_t> (arg.carrierKeyLength));
    WRITE_AND_CHECK(arg.keyIdentifier);
    WRITE_AND_CHECK(arg.expirationTime);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CarrierInfoForImsiEncryption>(RIL_CarrierInfoForImsiEncryption &arg) const {
    READ_AND_CHECK(arg.mcc);
    READ_AND_CHECK(arg.mnc);
    size_t sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.carrierKey, sz));
    arg.carrierKeyLength = static_cast<int32_t>(sz);
    READ_AND_CHECK(arg.keyIdentifier);
    READ_AND_CHECK(arg.expirationTime);
    return Result::SUCCESS;
}

template<>
Marshal::Result Marshal::release<RIL_CarrierInfoForImsiEncryption>(RIL_CarrierInfoForImsiEncryption &arg) {
    release(arg.mcc);
    release(arg.mnc);
    release(arg.carrierKey, arg.carrierKeyLength);
    release(arg.keyIdentifier);
    release(arg.expirationTime);
    return Result::SUCCESS;
}