/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "DeviceTypes.h"

#include <cstdint>
#include <iosfwd>
#include <vector>

// Defines a Linker-AHB region mapping. this struct mimics the fw_map
// structure used by the Linux driver to map addresses. All translation
// tables are copied from the driver code.
// Linker address region: [LinkerRegionBegin, LinkerRegionEnd)
// AHB Region: [AhbRegionBegin, AhbRegionBegin + LinkerRegionEnd - LinkerRegionBegin)
// Notes:
// 1. FW and uCode may have linker regions with the same addresses.
//    These addresses cannot be accessed through linker address, only AHB address is supported.
// 2. Other regions may be accessed by either linker or AHB address as driver performs translation
//    according to the same table.

struct fw_map
{
    uint32_t LinkerRegionBegin;     // linker address - from, inclusive
    uint32_t LinkerRegionEnd;       // linker address - to, exclusive
    uint32_t AhbRegionBegin;        //  PCI/Host address - BAR0 + 0x880000
    const char* RegionName;         // Region name
    bool IsFwRegion;                // true if FW mapping, false if UCODE mapping
    bool CrashDump;                 // true if should be dumped during crash dump
};

std::ostream& operator<<(std::ostream& os, const fw_map& region);

class AddressTranslator
{
public:

    // Converts a linker address srcAddr to AHB address dstAddr according to the provided baseband type.
    // Returns true for successful conversion, false otherwise. In case of failure, dstAddr cannot be used.
    static bool ToAhbAddress(uint32_t srcAddr, uint32_t& dstAddr, BasebandRevision deviceRevision);

    // Returns an address map for the provided baseband or null if no supported baseband can be found.
    // NOTE: The caller is responsible to check return value for null !!!
    static const std::vector<struct fw_map>* GetAddressMap(BasebandRevision deviceRevision);

private:
    static bool Remap(uint32_t srcAddr, uint32_t& dstAddr, const std::vector<struct fw_map>& mapping);

    static const std::vector<struct fw_map> sparrow_b0_fw_mapping;
    static const std::vector<struct fw_map> sparrow_d0_fw_mapping;
    static const std::vector<struct fw_map> talyn_ma_fw_mapping;
    static const std::vector<struct fw_map> talyn_mb_fw_mapping;
    static const std::vector<struct fw_map> borrelly_p1_fw_mapping;
};
