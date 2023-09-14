////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxbpsabf40.h
/// @brief bpsabf40 class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CAMXBPSABF40_H
#define CAMXBPSABF40_H

#include "abf_4_0_0.h"
#include "camxformats.h"
#include "camxispiqmodule.h"
#include "camxsensorproperty.h"
#include "iqcommondefs.h"

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief BPS ABF 40 Class Implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BPSABF40 final : public ISPIQModule
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Create BPSABF40 Object
    ///
    /// @param  pCreateData Pointer to the BPSABF40 Creation
    ///
    /// @return CamxResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult Create(
        BPSModuleCreateData* pCreateData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize parameters
    ///
    /// @param  pCreateData Pointer to the BPS Module CreateData
    ///
    /// @return CamxResultSuccess if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult Initialize(
        BPSModuleCreateData* pCreateData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FillCmdBufferManagerParams
    ///
    /// @brief  Fills the command buffer manager params needed by IQ Module
    ///
    /// @param  pInputData Pointer to the IQ Module Input data structure
    /// @param  pParam     Pointer to the structure containing the command buffer manager param to be filled by IQ Module
    ///
    /// @return CamxResultSuccess if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult FillCmdBufferManagerParams(
       const ISPInputData*     pInputData,
       IQModuleCmdBufferParam* pParam);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Execute
    ///
    /// @brief  Execute process capture request to configure module
    ///
    /// @param  pInputData Pointer to the ISP input data
    ///
    /// @return CamxResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult Execute(
        ISPInputData* pInputData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~BPSABF40
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~BPSABF40();

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BPSABF40
    ///
    /// @brief  Constructor
    ///
    /// @param  pNodeIdentifier   String identifier for the Node that creating this IQ Module object
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    explicit BPSABF40(
        const CHAR* pNodeIdentifier);

private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AllocateCommonLibraryData
    ///
    /// @brief  Allocate memory required for common library
    ///
    /// @return CamxResult success if the write operation is success
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult AllocateCommonLibraryData();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DeallocateCommonLibraryData
    ///
    /// @brief  Deallocate memory required for common library
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID DeallocateCommonLibraryData();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CheckDependenceChange
    ///
    /// @brief  Check to see if the Dependence Trigger Data Changed
    ///
    /// @param  pInputData Pointer to the ISP input data
    ///
    /// @return BOOL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL CheckDependenceChange(
        ISPInputData* pInputData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RunCalculation
    ///
    /// @brief   Calculate the Register Value
    ///
    /// @param   pInputData Pointer to the ISP input data
    ///
    /// @return  CamxResult Indicates if configure DMI and Registers was success or failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult RunCalculation(
        ISPInputData* pInputData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FetchDMIBuffer
    ///
    /// @brief  Fetch ABF DMI LUT
    ///
    /// @return CamxResult Indicates if fetch DMI was success or failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult FetchDMIBuffer();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// UpdateBPSInternalData
    ///
    /// @brief  Update BPS internal data
    ///
    /// @param  pInputData Pointer to the ISP input data
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID UpdateBPSInternalData(
        ISPInputData* pInputData);


    BPSABF40(const BPSABF40&)            = delete;           ///< Disallow the copy constructor
    BPSABF40& operator=(const BPSABF40&) = delete;           ///< Disallow assignment operator

    const CHAR*                     m_pNodeIdentifier;       ///< String identifier of this Node
    ABF40InputData                  m_dependenceData;        ///< Dependence Data for this Module
    CmdBufferManager*               m_pLUTCmdBufferManager;  ///< Command buffer manager for all LUTs of ABF
    CmdBuffer*                      m_pLUTDMICmdBuffer;      ///< Command buffer for holding all LUTs

    UINT32                          m_actEnable;             ///< Act EN
    UINT32                          m_darkSmoothEnable;      ///< Dark Smooth EN
    UINT32                          m_darkDesatEnable;       ///< Dark Desat EN
    UINT32                          m_minMaxEnable;          ///< MinMax EN
    UINT32                          m_crossPlaneEnable;      ///< Cross Panel EN
    UINT32                          m_blsEnable;             ///< BLS EN
    UINT32                          m_pixMatchLevelRB;       ///< Pix RB EN
    UINT32                          m_pixMatchLevelG;        ///< Pix G EN
    UINT32                          m_blockMatchPatternRB;   ///< Block Match pattern RB

    BOOL                            m_bilateralEnable;       ///< enable bilateral filtering
    BOOL                            m_minmaxEnable;          ///< enable built-in min-max pixel filter
    BOOL                            m_dirsmthEnable;         ///< enable Directional Smoothing filter

    UINT8                           m_noiseReductionMode;    ///< noise reduction mode

    UINT32*                         m_pABFNoiseLUT;          ///< Pointer for ABF noise LUT used for tuning data
    UINT32*                         m_pABFNoiseLUT1;         ///< Pointer for ABF noise LUT1 used for tuning data
    UINT32*                         m_pABFActivityLUT;       ///< Pointer for ABF activity LUT used for tuning data
    UINT32*                         m_pABFDarkLUT;           ///< Pointer for ABF dark LUT used for tuning data
    abf_4_0_0::chromatix_abf40Type* m_pChromatix;            ///< Pointer to tuning mode data
    bls_1_2_0::chromatix_bls12Type* m_pChromatixBLS;         ///
};

CAMX_NAMESPACE_END
#endif // CAMXBPSABF40_H