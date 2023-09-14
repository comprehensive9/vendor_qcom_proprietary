////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxcafalgorithmhandler.h
/// @brief This class handle creation  of AF algorithm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXCAFALGORITHMHANDLER_H
#define CAMXCAFALGORITHMHANDLER_H

#include "chiafinterface.h"

#include "camxstatscommon.h"

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This class handles AF algo creation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CAFAlgorithmHandler
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CreateAlgorithm
    ///
    /// @brief  This method creates an instance to the AFAlgorithm.
    ///
    /// @param  pCreateParams   Pointer to create params
    /// @param  ppAfAlgorithm   Pointer to the created AfAlgorithm instance
    /// @param  pfnCreate       Pointer to Algorithm entry function
    ///
    /// @return CamxResultSuccess if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult CreateAlgorithm(
        const AFAlgoCreateParamList* pCreateParams,
        CHIAFAlgorithm**             ppAfAlgorithm,
        CREATEAF                     pfnCreate);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CAFAlgorithmHandler
    ///
    /// @brief  default constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAFAlgorithmHandler();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~CAFAlgorithmHandler
    ///
    /// @brief  destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~CAFAlgorithmHandler();

private:

    CAFAlgorithmHandler(const CAFAlgorithmHandler&) = delete;               ///< Do not implement copy constructor
    CAFAlgorithmHandler& operator=(const CAFAlgorithmHandler&) = delete;    ///< Do not implement assignment operator

    OSLIBRARYHANDLE    m_hHandle;                                          ///< Handle to the loaded library
};

CAMX_NAMESPACE_END

#endif // CAMXCAFALGORITHMHANDLER_H
