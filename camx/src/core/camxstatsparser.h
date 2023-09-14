////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxstatsparser.h
///
/// @brief Stats parser definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXSTATSPARSER_H
#define CAMXSTATSPARSER_H

#include "chiispstatsdefs.h"

#include "camxmetadatapool.h"
#include "camxtypes.h"

CAMX_NAMESPACE_BEGIN

/// Forward Declarations
class Node;

/// @brief Parameter set for parsing
struct ParseData
{
    Node*              pNode;            ///< Node for which parsing is being done (access to GetDataList/WriteDataList)
    BOOL               skipParse;        ///< Skip parsing (pStatsOutput should contain valid data) and publish tags/properties
    VOID*              pUnparsedBuffer;  ///< Pointer to buffer in which stats were written by hardware
    VOID*              pStatsOutput;     ///< Pointer to buffer in which parsed stats are to be output
    const ImageFormat* pImageFormat;     ///< Pointer to the image format of the pUnparsedBuffer. Can be NULL.
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Representation of a stats parser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class StatsParser
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Destroy the hw factory object.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Parse
    ///
    /// @brief  Pure virtual function to parse the unparsed stats buffer for a particular type of stat.
    ///
    /// @param  statsType Type of stats that is needed.
    /// @param  pInput    Pointer to parameter structure with additional parsing data
    ///
    /// @return CamxResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult Parse(
        ISPStatsType  statsType,
        ParseData*    pInput) = 0;

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~StatsParser
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~StatsParser();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StatsParser
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    StatsParser();

private:
    StatsParser(const StatsParser&) = delete;             ///< Disallow the copy constructor.
    StatsParser& operator=(const StatsParser&) = delete;  ///< Disallow assignment operator
};

CAMX_NAMESPACE_END

#endif // CAMXSTATSPARSER_H
