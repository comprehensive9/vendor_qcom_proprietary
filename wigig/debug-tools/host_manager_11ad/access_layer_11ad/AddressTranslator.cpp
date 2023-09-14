/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "AddressTranslator.h"
#include "DebugLogger.h"

#include <ostream>
#include <cstdint>
#include <vector>

std::ostream& operator<<(std::ostream& os, const fw_map& region)
{
    return os << region.RegionName
        << " [" << Address(region.LinkerRegionBegin) << ", " << Address(region.LinkerRegionEnd)  << "]"
        << " AHB: " << Address(region.AhbRegionBegin)
        << " Size: " << (region.LinkerRegionEnd - region.LinkerRegionBegin) << " B"
        << " Dumped: " << BoolStr(region.CrashDump);
}

// memory remapping table for Sparrow B0
const std::vector<struct fw_map> AddressTranslator::sparrow_b0_fw_mapping =
{
    {0x000000, 0x040000, 0x8c0000, "fw_code",       true, false},   /* FW code RAM 256k */
    {0x800000, 0x808000, 0x900000, "fw_data",       true, true},    /* FW data RAM 32k  */
    {0x840000, 0x860000, 0x908000, "fw_peri",       true, true},    /* periph data 128k */
    {0x880000, 0x88a000, 0x880000, "rgf",           true, true},    /* various RGF 40k  */
    {0x88a000, 0x88b000, 0x88a000, "AGC_tbl",       true, true},    /* AGC table   4k   */
    {0x88b000, 0x88c000, 0x88b000, "rgf_ext",       true, true},    /* Pcie_ext_rgf 4k  */
    {0x88c000, 0x88c200, 0x88c000, "mac_rgf_ext",   true, true},    /* mac_ext_rgf 512b */
    {0x8c0000, 0x949000, 0x8c0000, "upper",         true, true},    /* upper area 548k  */
    /* UCODE areas - accessible by debugfs blobs but not by wmi_addr_remap. UCODE areas MUST be added AFTER FW areas! */
    {0x000000, 0x020000, 0x920000, "uc_code",       false, false},  /* ucode code RAM 128k */
    {0x800000, 0x804000, 0x940000, "uc_data",       false, true},   /* ucode data RAM 16k  */
};

// memory remapping table for Sparrow D0
const std::vector<struct fw_map> AddressTranslator::sparrow_d0_fw_mapping =
{
    {0x000000, 0x040000, 0x8c0000, "fw_code",       true, false},   /* FW code RAM 256k */
    {0x800000, 0x808000, 0x900000, "fw_data",       true, true},    /* FW data RAM 32k  */
    {0x840000, 0x860000, 0x908000, "fw_peri",       true, true},    /* periph data 128k */
    {0x880000, 0x88a000, 0x880000, "rgf",           true, true},    /* various RGF 40k  */
    {0x88a000, 0x88b000, 0x88a000, "AGC_tbl",       true, true},    /* AGC table   4k   */
    {0x88b000, 0x88c000, 0x88b000, "rgf_ext",       true, true},    /* Pcie_ext_rgf 4k  */
    {0x88c000, 0x88c500, 0x88c000, "mac_rgf_ext",   true, true},    /* mac_ext_rgf 1280b */
    {0x8c0000, 0x949000, 0x8c0000, "upper",         true, true},    /* upper area 548k  */
    /* UCODE areas - accessible by debugfs blobs but not by wmi_addr_remap. UCODE areas MUST be added AFTER FW areas! */
    {0x000000, 0x020000, 0x920000, "uc_code",       false, false},  /* ucode code RAM 128k */
    {0x800000, 0x804000, 0x940000, "uc_data",       false, true},   /* ucode data RAM 16k  */
};

// memory remapping table for Talyn MA
const std::vector<struct fw_map> AddressTranslator::talyn_ma_fw_mapping =
{
    {0x000000, 0x100000, 0x900000, "fw_code",       true, false},   /* FW code RAM 1M */
    {0x800000, 0x820000, 0xa00000, "fw_data",       true, true},    /* FW data RAM 128k */
    {0x840000, 0x858000, 0xa20000, "fw_peri",       true, true},    /* periph. data RAM 96k */
    {0x880000, 0x88a000, 0x880000, "rgf",           true, true},    /* various RGF 40k */
    {0x88a000, 0x88b000, 0x88a000, "AGC_tbl",       true, true},    /* AGC table 4k */
    {0x88b000, 0x88c000, 0x88b000, "rgf_ext",       true, true},    /* Pcie_ext_rgf 4k */
    {0x88c000, 0x88c200, 0x88c000, "mac_rgf_ext",   true, true},    /* mac_ext_rgf 512b */
    {0x8c0000, 0x949000, 0x8c0000, "upper",         true, true},    /* upper area 548k */
    /* UCODE areas - accessible by debugfs blobs but not by wmi_addr_remap. UCODE areas MUST be added AFTER FW areas! */
    {0x000000, 0x040000, 0xa38000, "uc_code",       false, false},  /* ucode code RAM 256k */
    {0x800000, 0x808000, 0xa78000, "uc_data",       false, true},   /* ucode data RAM 32k */
};

// memory remapping table for Talyn MB and Talyn_MC
const std::vector<struct fw_map> AddressTranslator::talyn_mb_fw_mapping =
{
    {0x000000, 0x100000, 0x900000, "fw_code",       true, false},   /* FW code RAM 1M */
    {0x800000, 0x820000, 0xa00000, "fw_data",       true, true},    /* FW data RAM 128k */
    {0x840000, 0x858000, 0xa20000, "fw_peri",       true, true},    /* periph. data RAM 96k */
    {0x880000, 0x88a000, 0x880000, "rgf",           true, true},    /* various RGF 40k */
    {0x88a000, 0x88b000, 0x88a000, "AGC_tbl",       true, true},    /* AGC table 4k */
    {0x88b000, 0x88c000, 0x88b000, "rgf_ext",       true, true},    /* Pcie_ext_rgf 4k */
    {0x88c000, 0x88c8d0, 0x88c000, "mac_rgf_ext",   true, true},    /* mac_ext_rgf 2256b */
    {0x88d000, 0x88e000, 0x88d000, "ext_user_rgf",  true, true},    /* ext USER RGF 4k */
    {0x890000, 0x894000, 0x890000, "sec_pka",       true, true},    /* SEC PKA 16k */
    {0x898000, 0x898c18, 0x898000, "sec_kdf_rgf",   true, true},    /* SEC KDF RGF 3096b */
    {0x89a000, 0x89a84c, 0x89a000, "sec_main",      true, true},    /* SEC MAIN 2124b */
    {0x8a0000, 0x8a1000, 0x8a0000, "otp",           true, false},   /* OTP 4k */
    {0x8b0000, 0x8c0000, 0x8b0000, "dma_ext_rgf",   true, true},    /* DMA EXT RGF 64k */
    {0x8c0000, 0x8c0210, 0x8c0000, "dum_user_rgf",  true, true},    /* DUM USER RGF 528b */
    {0x8c2000, 0x8c2128, 0x8c2000, "dma_ofu",       true, true},    /* DMA OFU 296b */
    {0x8c3000, 0x8c3100, 0x8c3000, "ucode_debug",   true, true},    /* ucode debug 256b */
    {0x900000, 0x9c0000, 0x900000, "upper1",        true, true},    /* upper1 area 768k */
    {0xa00000, 0xa80000, 0xa00000, "upper2",        true, true },   /* upper2 area 512k */
    /* UCODE areas - accessible by debugfs blobs but not by wmi_addr_remap. UCODE areas MUST be added AFTER FW areas! */
    {0x000000, 0x040000, 0xa38000, "uc_code",       false, false},  /* ucode code RAM 256k */
    {0x800000, 0x808000, 0xa78000, "uc_data",       false, true},   /* ucode data RAM 32k */
};


// memory remapping table for Borrelly P1
const std::vector<struct fw_map> AddressTranslator::borrelly_p1_fw_mapping =
{
    {0x000000, 0x0A0000, 0xA00000, "user_code",     true, false},   /* USER code RAM 640KB */
    {0x800000, 0x820000, 0x960000, "user_data",     true, true},    /* USER data RAM 128KB */
    {0x840000, 0x880000, 0x980000, "user_peri",     true, true},    /* periph. data RAM 256KB */
    // RSXD areas, MUST be added AFTER FW areas!
    {0x000000, 0x020000, 0x9c0000, "rsxd_code",     false, false},  /* RSXD code RAM 128KB */
    {0x800000, 0x810000, 0x9e0000, "rsxd_data",     false, true},   /* RSXD data RAM 64KB */
};

bool AddressTranslator::Remap(uint32_t srcAddr, uint32_t& dstAddr, const std::vector<struct fw_map>& mapping)
{
    for (const fw_map& regionMap : mapping)
    {
        LOG_VERBOSE << "Checking " << Hex<8>(srcAddr) << " against " << regionMap << std::endl;

        if (regionMap.LinkerRegionBegin <= srcAddr && srcAddr < regionMap.LinkerRegionEnd)
        {
            dstAddr = srcAddr + regionMap.AhbRegionBegin - regionMap.LinkerRegionBegin;

            LOG_VERBOSE << "Successful address translation: "
                << Address(srcAddr) << " --> " << Address(dstAddr) << std::endl;
            return true;
        }
    }

    return false;
}

const std::vector<struct fw_map>* AddressTranslator::GetAddressMap(BasebandRevision deviceRevision)
{
    switch (deviceRevision)
    {
    case SPR_B0:
        return &AddressTranslator::sparrow_b0_fw_mapping;
    case SPR_D0:
        return &AddressTranslator::sparrow_d0_fw_mapping;
    case TLN_M_A0:
        return &AddressTranslator::talyn_ma_fw_mapping;
    case TLN_M_B0:
    case TLN_M_C0:
        return &AddressTranslator::talyn_mb_fw_mapping;
    case BRL_P1:
        return &AddressTranslator::borrelly_p1_fw_mapping;
    default:
        LOG_ERROR << "Unsupported device revision: " << deviceRevision;
        return nullptr;
    }
}

bool AddressTranslator::ToAhbAddress(uint32_t srcAddr, uint32_t& dstAddr, BasebandRevision deviceRevision)
{
    dstAddr = srcAddr;
    const std::vector<struct fw_map> *const pAddressMap = GetAddressMap(deviceRevision);
    return pAddressMap ? Remap(srcAddr, dstAddr, *pAddressMap) : false;
}
