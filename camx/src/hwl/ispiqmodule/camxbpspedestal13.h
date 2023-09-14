////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxbpspedestal13.h
/// @brief bpspedestal13 class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CAMXBPSPEDESTAL13_H
#define CAMXBPSPEDESTAL13_H

// Camx Includes
#include "camxispiqmodule.h"
#include "pedestal_1_3_0.h"

// Common library includes
#include "iqcommondefs.h"

CAMX_NAMESPACE_BEGIN


static const UINT32 RedLUTPedestal  = 0x1;
static const UINT32 BlueLUTPedestal = 0x2;
static const UINT32 MaxPedestalLUT  = 2;

static const UINT8  BPSPedestal13NumDMITables           = 2;
static const UINT32 BPSPedestal13DMISetSizeDword        = 130;   // DMI LUT table has 130 entries
static const UINT32 BPSPedestal13LUTTableSize           = BPSPedestal13DMISetSizeDword * sizeof(UINT32);
static const UINT32 BPSPedestal13DMILengthDword         = BPSPedestal13DMISetSizeDword * BPSPedestal13NumDMITables;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief BPS Pedestal 13 Class Implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BPSPedestal13 final : public ISPIQModule
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Create BPSPedestal13 Object
    ///
    /// @param  pCreateData Pointer to the BPSPedestal13 Creation
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
    /// @param  pCreateData Pointer to the BPSPedestal13 Creation
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
    /// ~BPSPedestal13
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~BPSPedestal13();

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BPSPedestal13
    ///
    /// @brief  Constructor
    ///
    /// @param  pNodeIdentifier     String identifier for the Node that creating this IQ Module object
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    explicit BPSPedestal13(
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
    /// @brief  Fetch DMI buffer
    ///
    /// @return CamxResult Indicates if configure DMI was success or failure
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
        ISPInputData* pInputData
        ) const;

    BPSPedestal13(const BPSPedestal13&)            = delete;           ///< Disallow the copy constructor
    BPSPedestal13& operator=(const BPSPedestal13&) = delete;           ///< Disallow assignment operator

    const CHAR*                               m_pNodeIdentifier;       ///< String identifier of this Node
    Pedestal13InputData                       m_dependenceData;        ///< Dependence Data for this Module
    CmdBufferManager*                         m_pLUTCmdBufferManager;  ///< Command buffer manager for all LUTs of Pedestal
    CmdBuffer*                                m_pLUTDMICmdBuffer;      ///< Command buffer for holding all LUTs

    UINT32*                                   m_pGRRLUTDMIBuffer;      ///< Pointer to GRR table
    UINT32*                                   m_pGBBLUTDMIBuffer;      ///< Pointer to GBB table
    UINT8                                     m_blacklevelLock;        ///< manual update Black Level lock
    pedestal_1_3_0::chromatix_pedestal13Type* m_pChromatix;            ///< Pointer to tuning mode data
};

CAMX_NAMESPACE_END

#endif // CAMXBPSPEDESTAL13_H
