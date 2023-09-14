////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxifelsc34.h
/// @brief camxifelsc34 class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CAMXIFELSC34_H
#define CAMXIFELSC34_H

// Camx Includes
#include "camxispiqmodule.h"
#include "chitintlessinterface.h"
#include "lsc34setting.h"
#include "lsc_3_4_0.h"
#include "tintless_2_0_0.h"

// Common library includes
#include "iqcommondefs.h"

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Class for IFE LSC Module
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFELSC34 final : public ISPIQModule
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Create IFELSC34 Object
    ///
    /// @param  pCreateData Pointer to data for LSC Creation
    ///
    /// @return CamxResultSuccess if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult Create(
        IFEModuleCreateData* pCreateData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Execute
    ///
    /// @brief  Generate Settings for LSC Object
    ///
    /// @param  pInputData Pointer to the Inputdata
    ///
    /// @return CamxResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult Execute(
        ISPInputData* pInputData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PrepareStripingParameters
    ///
    /// @brief  Prepare striping parameters for striping lib
    ///
    /// @param  pInputData Pointer to the Inputdata
    ///
    /// @return CamxResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult PrepareStripingParameters(
        ISPInputData* pInputData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetDualIFEData
    ///
    /// @brief  Provides information on how dual IFE mode affects the IQ module
    ///
    /// @param  pDualIFEData Pointer to dual IFE data the module will fill in
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual VOID GetDualIFEData(
        IQModuleDualIFEData* pDualIFEData)
    {
        CAMX_ASSERT(NULL != pDualIFEData);

        pDualIFEData->dualIFESensitive      = TRUE;
        pDualIFEData->dualIFEDMI32Sensitive = TRUE;
    }

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~IFELSC34
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~IFELSC34();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IFELSC34
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IFELSC34();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize parameters
    ///
    /// @param  pCreateData Input data to initialize the module
    ///
    /// @return CamxResultSuccess if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult Initialize(
        IFEModuleCreateData* pCreateData);

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
    /// TranslateCalibrationTableToCommonLibrary
    ///
    /// @brief  Translate calibration table to common library
    ///
    /// @param  pInputData Pointer to the Input Data
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID TranslateCalibrationTableToCommonLibrary(
        const ISPInputData* pInputData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CheckDependenceChange
    ///
    /// @brief  Check if the Dependence Data has changed
    ///
    /// @param  pInputData Pointer to the Input Data
    ///
    /// @return TRUE if dependence is met
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
    /// @return  CamxResultSuccess if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult RunCalculation(
        ISPInputData* pInputData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// UpdateIFEInternalData
    ///
    /// @brief  Update IFE internal data
    ///
    /// @param  pInputData Pointer to the ISP input data
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID UpdateIFEInternalData(
        ISPInputData* pInputData);

    IFELSC34(const IFELSC34&)            = delete;          ///< Disallow the copy constructor
    IFELSC34& operator=(const IFELSC34&) = delete;          ///< Disallow assignment operator

    LSCState*                       m_pState;               ///< Pointer to state
    LSCCalibrationData*             m_pLSCCalibrationData;  ///< LSC Calibration table
    LSC34CalibrationDataTable*      m_pCalibrationTable;    ///< LSC calibration Data table
    OSLIBRARYHANDLE                 m_hHandle;              ///< Tintless Algo Library handle
    CHITintlessAlgorithm*           m_pTintlessAlgo;        ///< Tintless Algorithm Instance
    TintlessConfig                  m_tintlessConfig;       ///< Tintless Config
    ParsedTintlessBGStatsOutput*    m_pTintlessBGStats;     ///< Pointer to TintlessBG stats
    LSC34UnpackedData               m_unpackedData;         ///< unpacked data
    UINT8                           m_shadingMode;          ///< Shading mode
    UINT8                           m_lensShadingMapMode;   ///< Lens shading map mode
    lsc_3_4_0::chromatix_lsc34Type* m_pChromatix;           ///< Pointer to tuning mode data
    BOOL                            m_AWBLock;              ///< Flag to track AWB state
    VOID*                           m_pInterpolationData;   ///< Interpolation Parameters
    LSC34TintlessRatio              m_tintlessRatio;        ///< Tintless ratio
    VOID*                           m_pStripeInputState;    ///< Pointer to the Striping Input Params
};

CAMX_NAMESPACE_END

#endif // CAMXIFELSC34_H
