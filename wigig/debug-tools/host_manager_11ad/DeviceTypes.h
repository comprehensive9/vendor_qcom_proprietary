/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <iosfwd>

enum BasebandType
{
    BASEBAND_TYPE_NONE,
    BASEBAND_TYPE_SPARROW,   // added here to keep backward compatibility. some tools assume MARLON == 2, we don't brake this assumption
    BASEBAND_TYPE_MARLON,
    BASEBAND_TYPE_TALYN,
    BASEBAND_TYPE_BORRELLY,
    BASEBAND_TYPE_LAST
};

std::ostream& operator<<(std::ostream& os, const BasebandType& basebandType);

enum BasebandRevision
{
    // In this enum the order does matter. newer revisions should be last!
    BB_REV_UNKNOWN = 0,
    MAR_DB1 = 1,
    MAR_DB2 = 2,
    SPR_A0 = 3,
    SPR_A1 = 4,
    SPR_B0 = 5,
    SPR_C0 = 6,
    SPR_D0 = 7,
    TLN_M_A0 = 8,
    TLN_M_B0 = 9,
    TLN_M_C0 = 10,
    BRL_P1 = 20, // [11-19] range is reserved for networking chips
};

std::ostream& operator<<(std::ostream& os, const BasebandRevision& rev);