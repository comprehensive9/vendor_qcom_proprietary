////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxifedemosaic37titan17x.h
/// @brief IFE Demosaic37 register setting for Titan17x
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CAMXIFEDEMOSAIC37TITAN17X_H
#define CAMXIFEDEMOSAIC37TITAN17X_H

#include "titan170_ife.h"
#include "camxisphwsetting.h"
#include "demosaic37setting.h"

CAMX_NAMESPACE_BEGIN

CAMX_BEGIN_PACKED
/// @brief Register Set of the Demosaic36 Module
struct IFEDemosaic37RegCmd1
{
    IFE_IFE_0_VFE_DEMO_CFG                     moduleConfig;             ///< DEMO_CFG
}CAMX_PACKED;

struct IFEDemosaic37RegCmd2
{
    IFE_IFE_0_VFE_DEMO_INTERP_COEFF_CFG        interpolationCoeffConfig; ///< INTERP_COEFF_CFG
    IFE_IFE_0_VFE_DEMO_INTERP_CLASSIFIER_0     interpolationClassifier0; ///< INTERP_CLASSIFIER_0
}CAMX_PACKED;

struct IFEDemosaic37RegCmd
{
    struct IFEDemosaic37RegCmd1                demosaic37RegCmd1; ///< Demo Module config
    struct IFEDemosaic37RegCmd2                demosaic37RegCmd2; ///< Demo inter classifier
}CAMX_PACKED;
CAMX_END_PACKED


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief IFE Demosaic37 register setting for Titan17x
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFEDemosaic37Titan17x final : public ISPHWSetting
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
    /// UpdateTuningMetadata
    ///
    /// @brief  Update Tuning Metadata
    ///
    /// @param  pTuningMetadata      Pointer to the Tuning Metadata
    ///
    /// @return CamxResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult UpdateTuningMetadata(
        VOID*  pTuningMetadata);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PackIQRegisterSetting
    ///
    /// @brief  Packing register setting based on calculation data
    ///
    /// @param  pInput       Pointer to the Input data to the Demosaic37 module for calculation
    /// @param  pOutput      Pointer to the Output data for DMI
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
    /// ~IFEDemosaic37Titan17x
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~IFEDemosaic37Titan17x();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IFEDemosaic37Titan17x
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IFEDemosaic37Titan17x();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DumpRegConfig
    ///
    /// @brief  Print register configuration of Crop module for debug
    ///
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID DumpRegConfig();

private:
    IFEDemosaic37RegCmd m_regCmd; ///< Register List of this Module

    IFEDemosaic37Titan17x(const IFEDemosaic37Titan17x&)            = delete; ///< Disallow the copy constructor
    IFEDemosaic37Titan17x& operator=(const IFEDemosaic37Titan17x&) = delete; ///< Disallow assignment operator
};

CAMX_NAMESPACE_END

#endif // CAMXIFEDEMOSAIC37TITAN17X_H