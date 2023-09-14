////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxifehdrbhiststats13titan17x.h
/// @brief IFE HDRBHISTStats13 register setting for Titan17x
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CAMXIFEHDRBHISTSTATS13TITAN17X_H
#define CAMXIFEHDRBHISTSTATS13TITAN17X_H

#include "titan170_ife.h"
#include "camxdefs.h"
#include "camxisphwsetting.h"
#include "camxispstatsmodule.h"

CAMX_NAMESPACE_BEGIN

CAMX_BEGIN_PACKED

/// @brief BHist HW specific const values
static const UINT32 HDRBHistDMILength = HDRBHistBinsPerChannel;

static const UINT   HDRBHistRam0LUT = IFE_IFE_0_VFE_DMI_CFG_DMI_RAM_SEL_STATS_HDR_BHIST_RAM0;
static const UINT   HDRBHistRam1LUT = IFE_IFE_0_VFE_DMI_CFG_DMI_RAM_SEL_STATS_HDR_BHIST_RAM1;
static const UINT   HDRBHistRam2LUT = IFE_IFE_0_VFE_DMI_CFG_DMI_RAM_SEL_STATS_HDR_BHIST_RAM2;
static const UINT   HDRBHistRamXLUT = IFE_IFE_0_VFE_DMI_CFG_DMI_RAM_SEL_STATS_HDR_BHIST_RAMX;
static const UINT   HDRBHistNoLut   = 4;

/// @brief HDRBHist Stats Configuration
struct IFEHDRBHistRegionConfig
{
    IFE_IFE_0_VFE_STATS_HDR_BHIST_RGN_OFFSET_CFG    regionOffset;   ///< BHist stats region offset config
    IFE_IFE_0_VFE_STATS_HDR_BHIST_RGN_NUM_CFG       regionNumber;   ///< BHist stats region number config
} CAMX_PACKED;

CAMX_END_PACKED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief IFE HDR Bayer Histogram 1.3  register setting for Titan17x
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFEHDRBHistStats13Titan17x final : public ISPHWSetting
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CreateCmdList
    ///
    /// @brief  Generate the Command List
    ///
    /// @param  pInputData       Pointer to the Inputdata
    /// @param  pDMIBufferOffset Pointer for DMI Buffer
    ///
    /// @return CamxResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult CreateCmdList(
        VOID*   pInputData,
        UINT32* pDMIBufferOffset);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PackIQRegisterSetting
    ///
    /// @brief  Calculate register settings based on common library result
    ///
    /// @param  pInput       Pointer to the Input data to the module for calculation
    /// @param  pOutput      Pointer to the Output data to the module for DMI buffer
    ///
    /// @return Return CamxResult.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult PackIQRegisterSetting(
        VOID* pInput,
        VOID* pOutput);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetupRegisterSetting
    ///
    /// @brief  Setup register value based on CamX Input
    ///
    /// @param  pInput       Pointer to the Input data to the module for calculation
    ///
    /// @return Return CamxResult.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult SetupRegisterSetting(
        VOID*  pInput);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~IFEHDRBHistStats13Titan17x
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~IFEHDRBHistStats13Titan17x();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IFEHDRBHistStats13Titan17x
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IFEHDRBHistStats13Titan17x();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DumpRegConfig
    ///
    /// @brief  Print register configuration of Crop module for debug
    ///
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID DumpRegConfig();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CopyRegCmd
    ///
    /// @brief  Copy register settings to the input buffer
    ///
    /// @param  pData  Pointer to the Input data buffer
    ///
    /// @return Number of bytes copied
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 CopyRegCmd(
        VOID* pData);

private:
    IFEHDRBHistRegionConfig m_regCmd;                       ///< Register List of this Module
    UINT32                  m_DMIConfig[HDRBHistDMILength]; ///< DMI LUT configuration

    IFEHDRBHistStats13Titan17x(const IFEHDRBHistStats13Titan17x&)            = delete; ///< Disallow the copy constructor
    IFEHDRBHistStats13Titan17x& operator=(const IFEHDRBHistStats13Titan17x&) = delete; ///< Disallow assignment operator
};

CAMX_NAMESPACE_END

#endif // CAMXIFEHDRBHISTSTATS13TITAN17X_H