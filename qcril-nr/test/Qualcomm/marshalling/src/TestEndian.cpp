/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <Endian.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Endian_to_nw) {
    uint16_t ui16 = 0x2010;
    uint16_t ui16_expected = 0x1020;
    uint16_t oui16 = TO_NW16(ui16);
    ASSERT_EQ(ui16_expected, oui16);

    uint32_t ui32 = 0x80402010;
    uint32_t ui32_expected = 0x10204080;
    uint32_t oui32 = TO_NW32(ui32);
    ASSERT_EQ(ui32_expected, oui32);

    uint64_t ui64 = 0x8141211180402010;
    uint64_t ui64_expected = 0x1020408011214181;
    uint64_t oui64 = TO_NW64(ui64);
    ASSERT_EQ(ui64_expected, oui64);

    int intv = 0x80000000;
    int intv_expected = 0x00000080;
    int ointv = TO_NWINT(intv);
    ASSERT_EQ(intv_expected, ointv);
}

TEST(Marshalling, Endian_from_nw) {
    uint16_t ui16 = 0x2010;
    uint16_t ui16_expected = 0x1020;
    uint16_t oui16 = FROM_NW16(ui16);
    ASSERT_EQ(ui16_expected, oui16);

    uint32_t ui32 = 0x80402010;
    uint32_t ui32_expected = 0x10204080;
    uint32_t oui32 = FROM_NW32(ui32);
    ASSERT_EQ(ui32_expected, oui32);

    uint64_t ui64 = 0x8141211180402010;
    uint64_t ui64_expected = 0x1020408011214181;
    uint64_t oui64 = FROM_NW64(ui64);
    ASSERT_EQ(ui64_expected, oui64);

    int intv = 0x80000000;
    int intv_expected = 0x00000080;
    int ointv = FROM_NWINT(intv);
    ASSERT_EQ(intv_expected, ointv);
}
