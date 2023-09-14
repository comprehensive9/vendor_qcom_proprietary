////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxipecs20titan17x.h
/// @brief IPE CS20 register setting for Titan17x
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CAMXIPECS20TITAN17X_H
#define CAMXIPECS20TITAN17X_H

#include "camxtitan17xdefs.h"
#include "camxisphwsetting.h"
#include "camxcmdbuffer.h"

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief IPE CS20 register setting for Titan17x
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IPECS20Titan17x final : public ISPHWSetting
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IPECS20Titan17x
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IPECS20Titan17x();

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
    /// UpdateTuningMetadata
    ///
    /// @brief  Update Tuning Metadata
    ///
    /// @param  pInputData      Pointer to the InputData
    ///
    /// @return CamxResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult UpdateTuningMetadata(
        VOID*  pInputData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PackIQRegisterSetting
    ///
    /// @brief  Calculate register settings based on common library result
    ///
    /// @param  pInput       Pointer to the Input data to the IQ module for calculation
    /// @param  pOutput      Pointer to the Output data to the IQ module for DMI calculation
    ///
    /// @return Return CamxResult.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult PackIQRegisterSetting(
        VOID*  pInput,
        VOID*  pOutput);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetupRegisterSetting
    ///
    /// @brief  Setup register value based on CamX Input
    ///
    /// @param  pInput       Pointer to the Input data to the Demosaic37 module for calculation
    ///
    /// @return Return CamxResult.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult SetupRegisterSetting(
        VOID*  pInput);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DumpRegConfig
    ///
    /// @brief  Print register configuration of Crop module for debug
    ///
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual VOID DumpRegConfig();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetRegSize
    ///
    /// @brief  Returns register size
    ///
    ///
    /// @return Number of bytes of register structure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual UINT GetRegSize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~IPECS20Titan17x
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~IPECS20Titan17x();

private:
    IPECS20Titan17x(const IPECS20Titan17x&)            = delete; ///< Disallow the copy constructor
    IPECS20Titan17x& operator=(const IPECS20Titan17x&) = delete; ///< Disallow assignment operator

    VOID* m_pRegCmd;                                                 ///< Register List of this Module
};

CAMX_NAMESPACE_END

#endif // CAMXIPECS20TITAN17X_H
