////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file camxstabilization.cpp
///
/// @brief Stabilization implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxdebugprint.h"
#include "camxmem.h"
#include "camxstabilization.h"
#include "camxutils.h"

CAMX_NAMESPACE_BEGIN

static const UINT32 WithinThresholdRatio = 10;  ///< Ratio to divide with while calculating thresholds for WithinThreshold mode

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SortObjectPosition
///
/// @brief  Comparison function that will be used for sorting objects
///
/// @param  pArg0 Pointer to object to compare for sorting
/// @param  pArg1 Pointer to object to compare for sorting
///
/// @return 1 if pArg0 is greater than pArg1, -1 otherwise
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT SortObjectPosition(
    const VOID* pArg0,
    const VOID* pArg1)
{
    CAMX_ASSERT(NULL != pArg0);
    CAMX_ASSERT(NULL != pArg1);

    const StabilizationHolder*  pFirst      = static_cast<const StabilizationHolder*>(pArg0);
    const StabilizationHolder*  pSecond     = static_cast<const StabilizationHolder*>(pArg1);
    INT                         comparison  = 0;
    UINT32                      size        = 0;
    UINT32                      diff        = 0;

    size = (pFirst->attributeData[ObjectSizeIndex].entry.data0 / 2) +
           (pSecond->attributeData[ObjectSizeIndex].entry.data0 / 2);
    diff = Utils::AbsoluteINT32((pFirst->attributeData[ObjectPositionIndex].entry.data1) -
                                (pSecond->attributeData[ObjectPositionIndex].entry.data1));

    if ((pFirst->id != 0) && (pSecond->id != 0))
    {
        if (pFirst->id > pSecond->id)
        {
            comparison = 1;
        }
        else
        {
            comparison = -1;
        }
    }
    else if (diff < size)
    {
        if ((pFirst->attributeData[ObjectPositionIndex].entry.data0) >
            (pSecond->attributeData[ObjectPositionIndex].entry.data0))
        {
            comparison = 1;
        }
        else
        {
            comparison = -1;
        }
    }
    else
    {
        if ((pFirst->attributeData[ObjectPositionIndex].entry.data1     >
             pSecond->attributeData[ObjectPositionIndex].entry.data1)   ||
            ((pFirst->attributeData[ObjectPositionIndex].entry.data1    ==
              pSecond->attributeData[ObjectPositionIndex].entry.data1)  &&
             (pFirst->attributeData[ObjectPositionIndex].entry.data0    >
              pSecond->attributeData[ObjectPositionIndex].entry.data0)))
        {
            comparison = 1;
        }
        else
        {
            comparison = -1;
        }
    }

    return comparison;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MedianSort
///
/// @brief  Comparison function that will be used for determining the median, sorts smaller to greater
///
/// @param  pArg0 Pointer to object to compare for sorting
/// @param  pArg1 Pointer to object to compare for sorting
///
/// @return 1 if pArg0 is greater than pArg1, 0 if equal, -1 otherwise
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT MedianSort(
    const VOID* pArg0,
    const VOID* pArg1)
{
    CAMX_ASSERT(NULL != pArg0);
    CAMX_ASSERT(NULL != pArg1);

    const UINT32*   pValue0     = static_cast<const UINT32*>(pArg0);
    const UINT32*   pValue1     = static_cast<const UINT32*>(pArg1);
    INT             comparison  = 0;

    if (*pValue0 == *pValue1)
    {
        comparison = 0;
    }
    else if (*pValue0 > *pValue1)
    {
        comparison = 1;
    }
    else
    {
        comparison = -1;
    }

    return comparison;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Stabilization::Stabilization
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Stabilization::Stabilization()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Stabilization::~Stabilization
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Stabilization::~Stabilization()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Stabilization::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Stabilization* Stabilization::Create()
{
    return CAMX_NEW Stabilization;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Stabilization::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::Destroy()
{
    CAMX_DELETE this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult Stabilization::Initialize(
    StabilizationConfig* pConfig,
    UINT32               frameWidth,
    UINT32               frameHeight)
{
    CamxResult result = CamxResultSuccess;

    m_frameWidth  = frameWidth;
    m_frameHeight = frameHeight;

    result = SetConfig(pConfig);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::GetConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult Stabilization::GetConfig(
    StabilizationConfig* pConfig
    ) const
{
    CamxResult result = CamxResultSuccess;

    if (NULL != pConfig)
    {
        *pConfig = m_configuration;
    }
    else
    {
        CAMX_LOG_ERROR(CamxLogGroupFD, "Input config is NULL");
        result = CamxResultEInvalidPointer;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::SetConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult Stabilization::SetConfig(
    StabilizationConfig* pConfig)
{
    CamxResult result = CamxResultSuccess;

    if (NULL != pConfig)
    {
        m_configuration = *pConfig;
    }
    else
    {
        CAMX_LOG_ERROR(CamxLogGroupFD, "Input config is NULL");
        result = CamxResultEInvalidPointer;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::ExecuteStabilization
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult Stabilization::ExecuteStabilization(
    StabilizationData* pCurrentData,
    StabilizationData* pStabilizedData)
{
    CamxResult            result               = CamxResultSuccess;
    UINT32                i                    = 0;
    UINT32                j                    = 0;
    UINT32                numObjects           = 0;
    StabilizationPosition position             = ObjectSame;
    UINT32                threshold0           = 0;
    UINT32                threshold1           = 0;
    UINT32                movingThreshold      = 0;
    StabilizationEntry    reference            = { 0 };
    StabilizationEntry*   pReference           = NULL;
    INT32                 frameWidth           = static_cast<INT32>(m_frameWidth);
    INT32                 frameHeight          = static_cast<INT32>(m_frameHeight);
    BOOL                  enableShrinking      = FALSE;
    BOOL                  atFrameBoundary      = FALSE;
    BOOL                  objectIsMoving       = FALSE;

    if ((NULL == pCurrentData) || (NULL == pStabilizedData))
    {
        CAMX_LOG_ERROR(CamxLogGroupFD, "Unable to execute stabilization, input data is NULL");
        result = CamxResultEInvalidPointer;
    }
    else
    {
        if (pCurrentData->numObjects > StabilizationMaxObjects)
        {
            CAMX_LOG_ERROR(CamxLogGroupFD, "Invalid number of objects %d", pCurrentData->numObjects);
            result = CamxResultEInvalidArg;
        }
    }

    if (m_configuration.historyDepth > StabilizationMaxHistory)
    {
        CAMX_LOG_ERROR(CamxLogGroupFD, "Invalid history size %d", m_configuration.historyDepth);
        result = CamxResultEInvalidArg;
    }

    if ((CamxResultSuccess == result) && (0 == pCurrentData->numObjects))
    {
        CAMX_LOG_VERBOSE(CamxLogGroupFD, "No objects detected, history reset");
    }

    if ((CamxResultSuccess == result) && (pCurrentData->numObjects > 1))
    {
        // Sort objects from smallest coordinate entry to largest
        Utils::Qsort(&pCurrentData->objectData[0], pCurrentData->numObjects, sizeof(pCurrentData->objectData[0]),
                     SortObjectPosition);
    }

    if (CamxResultSuccess == result)
    {
        numObjects = pCurrentData->numObjects;
        while (i < numObjects)
        {
            // If there are new objects, put them in history
            if (i >= m_historicalData.numObjects)
            {
                InitializeObjectEntry(&m_historicalData.objects[j], &pCurrentData->objectData[i]);
                m_historicalData.objects[j].pFaceInfo = pCurrentData->objectData[i].pFaceInfo;
                i++;
                j++;
                m_historicalData.numObjects++;
                continue;
            }

            position = CheckObject(&m_historicalData.objects[j], &pCurrentData->objectData[i]);

            if (ObjectSame == position)
            {
                AddObjectEntry(&m_historicalData.objects[j],
                               &pCurrentData->objectData[i],
                               &m_configuration);
                m_historicalData.objects[j].pFaceInfo = pCurrentData->objectData[i].pFaceInfo;
                i++;
                j++;
            }
            else if (ObjectBefore == position)
            {
                // Move the objects to the right if it is not the last element
                if (i < (pCurrentData->numObjects - 1))
                {
                    if (m_historicalData.numObjects < StabilizationMaxObjects)
                    {
                        Utils::Memmove(&m_historicalData.objects[j + 1],
                                       &m_historicalData.objects[j],
                                       (sizeof(m_historicalData.objects[0]) * (m_historicalData.numObjects - j)));

                        m_historicalData.numObjects++;
                    }
                    else
                    {
                        Utils::Memmove(&m_historicalData.objects[j + 1],
                                       &m_historicalData.objects[j],
                                       (sizeof(m_historicalData.objects[0]) * (StabilizationMaxObjects - (j + 1))));
                    }
                }

                InitializeObjectEntry(&m_historicalData.objects[j], &pCurrentData->objectData[i]);
                m_historicalData.objects[j].pFaceInfo = pCurrentData->objectData[i].pFaceInfo;
                i++;
                j++;
            }
            else if (ObjectAfter == position)
            {
                Utils::Memmove(&m_historicalData.objects[j],
                               &m_historicalData.objects[j + 1],
                               (sizeof(m_historicalData.objects[0]) * (m_historicalData.numObjects - (j + 1))));

                m_historicalData.numObjects--;
            }
        }

        if (m_historicalData.numObjects > numObjects)
        {
            m_historicalData.numObjects = numObjects;
        }
    }

    if (CamxResultSuccess == result)
    {
        // Stabilize all objects and their attributes
        for (i = 0; i < numObjects; i++)
        {
            objectIsMoving                                  = FALSE;
            atFrameBoundary                                 = FALSE;
            pStabilizedData->objectData[i].id               = m_historicalData.objects[i].id;
            pStabilizedData->objectData[i].numAttributes    = m_historicalData.objects[i].numAttributes;
            pStabilizedData->objectData[i].pFaceInfo        = m_historicalData.objects[i].pFaceInfo;

            INT32 data0 = static_cast<INT32>(m_historicalData.objects[i].objectAttributes[ObjectSizeIndex].stableEntry.data0*
                          m_configuration.boundaryCaseFactor);

            // Check if object is at left or right boundary
            if ((TRUE == (pCurrentData->objectData[i].attributeData[ObjectPositionIndex].entry.data0 < data0)) ||
                (TRUE == ((frameWidth-pCurrentData->objectData[i].attributeData[ObjectPositionIndex].entry.data0) < data0)))
            {
                CAMX_LOG_VERBOSE(CamxLogGroupFD, "Object %d is at boundary", i);
                atFrameBoundary = TRUE;
            }

            for (j = 0; j < pCurrentData->objectData[i].numAttributes; j++)
            {
                if (TRUE == m_configuration.attributeConfigs[j].enable)
                {
                    threshold0      = 0;
                    threshold1      = 0;
                    movingThreshold = 0;
                    pReference      = NULL;

                    // Determine if stabilizing with reference is being used
                    if ((TRUE == m_configuration.attributeConfigs[j].useReference)  &&
                        (TRUE == pCurrentData->objectData[i].attributeReference[j].valid))
                    {
                        reference = pCurrentData->objectData[i].attributeReference[j].entry;
                        pReference = &reference;
                    }

                    if (ObjectPositionIndex == j)
                    {
                        // Object Position is specific to frame dimensions when calculating threshold values
                        threshold0           = (m_historicalData.objects[i].objectAttributes[ObjectSizeIndex].stableEntry.data0*
                                                m_configuration.attributeConfigs[j].threshold) / 1000;
                        threshold1           = (m_historicalData.objects[i].objectAttributes[ObjectSizeIndex].stableEntry.data1*
                                                m_configuration.attributeConfigs[j].threshold) / 1000;
                        movingThreshold      = (m_historicalData.objects[i].objectAttributes[ObjectSizeIndex].stableEntry.data0*
                                               m_configuration.attributeConfigs[j].defaultMotionDetector.movingThreshold)/1000;
                    }
                    else if (ObjectSizeIndex == j)
                    {
                        // Size uses reference to calculate threshold
                        if (NULL != pReference)
                        {
                            threshold0 = (pReference->data0 *
                                          m_configuration.attributeConfigs[j].threshold) / 1000;
                            threshold1 = (pReference->data1 *
                                          m_configuration.attributeConfigs[j].threshold) / 1000;
                        }
                        else
                        {
                            threshold0 = (m_historicalData.objects[i].objectAttributes[j].stableEntry.data0 *
                                          m_configuration.attributeConfigs[j].threshold) / 1000;
                            threshold1 = (m_historicalData.objects[i].objectAttributes[j].stableEntry.data1 *
                                          m_configuration.attributeConfigs[j].threshold) / 1000;
                        }
                    }
                    else
                    {
                        threshold0 =        (m_historicalData.objects[i].objectAttributes[j].stableEntry.data0 *
                                             m_configuration.attributeConfigs[j].threshold) / 1000;
                        threshold1 =        (m_historicalData.objects[i].objectAttributes[j].stableEntry.data1 *
                                             m_configuration.attributeConfigs[j].threshold) / 1000;
                        movingThreshold =   (m_historicalData.objects[i].objectAttributes[j].stableEntry.data0 *
                                             m_configuration.attributeConfigs[j].defaultMotionDetector.movingThreshold) /
                                             1000;
                    }

                    CAMX_LOG_VERBOSE(CamxLogGroupFD,
                                     "Stabilizing object %d, attribute %d: current %d %d stable %d %d threshold %d %d %d",
                                     i,
                                     j,
                                     pCurrentData->objectData[i].attributeData[j].entry.data0,
                                     pCurrentData->objectData[i].attributeData[j].entry.data1,
                                     m_historicalData.objects[i].objectAttributes[j].stableEntry.data0,
                                     m_historicalData.objects[i].objectAttributes[j].stableEntry.data1,
                                     threshold0,
                                     threshold1,
                                     movingThreshold);

                    StabilizationFilter(&m_historicalData.objects[i].objectAttributes[j],
                                        j,
                                        &m_configuration.attributeConfigs[j],
                                        pReference,
                                        threshold0,
                                        threshold1,
                                        movingThreshold,
                                        atFrameBoundary,
                                        &m_historicalData.objects[i].objectAttributes[ObjectSizeIndex],
                                        &objectIsMoving);

                    pStabilizedData->objectData[i].attributeData[j].entry =
                        m_historicalData.objects[i].objectAttributes[j].stableEntry;
                    pStabilizedData->objectData[i].attributeData[j].valid = TRUE;
                    CAMX_LOG_VERBOSE(CamxLogGroupFD,
                                     "Stabilzed Output: object %d, attribute %d: current %d %d output %d %d",
                                     i,
                                     j,
                                     pCurrentData->objectData[i].attributeData[j].entry.data0,
                                     pCurrentData->objectData[i].attributeData[j].entry.data1,
                                     pStabilizedData->objectData[i].attributeData[j].entry.data0,
                                     pStabilizedData->objectData[i].attributeData[j].entry.data1);
                }

                if (TRUE == enableShrinking)
                {
                    // Handle object position corner cases
                    // Shrink size so that it does not go outside of frame

                    INT32* pWidth;
                    INT32* pHeight;
                    INT32* pCenterX;
                    INT32* pCenterY;

                    INT32  left;
                    INT32  top;
                    INT32  width;
                    INT32  height;

                    pCenterX = &(pStabilizedData->objectData[i].attributeData[ObjectPositionIndex].entry.data0);
                    pCenterY = &(pStabilizedData->objectData[i].attributeData[ObjectPositionIndex].entry.data1);
                    pWidth   = &(pStabilizedData->objectData[i].attributeData[ObjectSizeIndex].entry.data0);
                    pHeight  = &(pStabilizedData->objectData[i].attributeData[ObjectSizeIndex].entry.data1);

                    left   = (*pCenterX) - ((*pWidth) / 2);
                    top    = (*pCenterY) - ((*pHeight) / 2);
                    width  = *pWidth;
                    height = *pHeight;

                    // Check left and right boundaries
                    if (left < 0)
                    {
                        width = width + left;
                        left  = 0;
                    }
                    else if (left + width > frameWidth)
                    {
                        width = frameWidth - left;
                    }

                    // Check upper and lower boundaries
                    if (top < 0)
                    {
                        height = height + top;
                        top    = 0;
                    }
                    else if (top + height > frameHeight)
                    {
                        height = frameHeight - top;
                    }

                    *pCenterX = left + (width / 2);
                    *pCenterY = top  + (height / 2);
                    *pWidth   = width;
                    *pHeight  = height;

                    CAMX_LOG_VERBOSE(CamxLogGroupFD,
                                     "Stabilzed object %d after shrink, attribute %d: current %d %d output %d %d",
                                     i,
                                     j,
                                     pCurrentData->objectData[i].attributeData[j].entry.data0,
                                     pCurrentData->objectData[i].attributeData[j].entry.data1,
                                     pStabilizedData->objectData[i].attributeData[j].entry.data0,
                                     pStabilizedData->objectData[i].attributeData[j].entry.data1);
                }
            }
        }

        pStabilizedData->numObjects = numObjects;

        CAMX_LOG_VERBOSE(CamxLogGroupFD, "Stabilization history is tracking %d objects", m_historicalData.numObjects);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Stabilization Function Defintions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::StabilizationFilter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::StabilizationFilter(
    StabilizationAttribute*       pAttribute,
    UINT32                        attributIndex,
    StabilizationAttributeConfig* pConfig,
    StabilizationEntry*           pReference,
    UINT32                        threshold0,
    UINT32                        threshold1,
    UINT32                        movingThreshold,
    BOOL                          atFrameBoundary,
    StabilizationAttribute*       pSizeAttribute,
    BOOL*                         pObjectIsMoving)
{
    CAMX_ASSERT(NULL != pAttribute);
    CAMX_ASSERT(NULL != pConfig);

    StabilizationState newState            = StabilizingState;
    BOOL               withinLimit         = FALSE;
    BOOL               objectStable        = FALSE;
    UINT32             lastIndex           = 0;
    UINT32             lastLastIndex       = 0;
    BOOL               attributeIsChanged  = FALSE;
    BOOL               attributeIsMoving   = FALSE;

    if (ObjectSizeIndex == attributIndex)
    {
        ApplyAverageForSize(pAttribute, pConfig);
        CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: apply average filter, filter length = %d, filtered output = %d, %d",
                         attributIndex, pConfig->averageFilter.preFilterHistoryLength,
                         pAttribute->entryHistory[pAttribute->index].filteredData0,
                         pAttribute->entryHistory[pAttribute->index].filteredData1);
    }

    if (pAttribute->numEntries > 2)
    {
        lastIndex     = (pAttribute->index + pAttribute->numEntries - 1) % pAttribute->numEntries;
        lastLastIndex = (pAttribute->index + pAttribute->numEntries - 2) % pAttribute->numEntries;

        newState = pAttribute->state;
        CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: current state is %s, stateCount = %d", attributIndex, (newState == 0) ?
                         "Stable" : (newState == 1) ? "Unstable" : "Stablizing", pAttribute->stateCount);

        if (StabilizingState != pAttribute->state)
        {
            if (NULL != pReference)
            {
                withinLimit = WithinThreshold(pReference,
                                              &pAttribute->stableEntry,
                                              threshold0,
                                              threshold1);
            }
            else
            {
                withinLimit = WithinThreshold(&pAttribute->entryHistory[pAttribute->index],
                                              &pAttribute->stableEntry,
                                              threshold0,
                                              threshold1);
            }
        }

        if (ObjectSizeIndex == attributIndex)
        {
            if (pAttribute->numEntries > 3)
            {
                attributeIsChanged = CheckSizeAttributeMovement(pAttribute, pConfig, attributIndex);
            }
        }
        else
        {
            if (FALSE == *pObjectIsMoving)
            {
                attributeIsChanged = CheckAttributeMovement(&pAttribute->entryHistory[pAttribute->index],
                                                            &pAttribute->entryHistory[lastIndex],
                                                            &pAttribute->entryHistory[lastLastIndex],
                                                            attributIndex,
                                                            pConfig,
                                                            atFrameBoundary,
                                                            movingThreshold);
            }
        }

        // Stabilization state machine
        switch (pAttribute->state)
        {
            case UnstableState:

                if (TRUE == *pObjectIsMoving)
                {
                    attributeIsMoving = TRUE;
                    CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: object is moving; isMoving=%d", attributIndex,
                                     attributeIsMoving);
                }

                if (TRUE == attributeIsMoving)
                {
                    ApplyFilter(pAttribute, attributIndex, pConfig, attributeIsMoving);
                    newState = StabilizingState;
                }
                else if (TRUE == withinLimit)
                {
                    newState = StableState;
                }
                else if ((TRUE == attributeIsChanged) || (pConfig->maxUnstableStateCount <= pAttribute->stateCount))
                {
                    ApplyFilter(pAttribute, attributIndex, pConfig, attributeIsChanged);
                    newState          = StabilizingState;
                    attributeIsMoving = TRUE;
                }

                CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: withinThreshold=%d, IsChanged=%d, maxStateCount=%d, "
                                 "isMoving=%d; %s state", attributIndex, withinLimit, attributeIsChanged,
                                 pConfig->maxUnstableStateCount, attributeIsMoving, (newState == 0)? "goes to Stable":
                                 (newState == 1)?"stays in Unstable" : "goes to Stablizing");
                break;
            case StableState:

                if (TRUE == *pObjectIsMoving)
                {
                    attributeIsMoving = TRUE;
                    CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: object is moving; isMoving=%d", attributIndex,
                                     attributeIsMoving);
                }
                else
                {
                    if (0 == pAttribute->stateCount)
                    {
                        pAttribute->movingDelayCount = 0;
                    }

                    if ((TRUE == attributeIsChanged) && (pConfig->movingMaxDelayCount > pAttribute->movingDelayCount))
                    {
                        pAttribute->movingDelayCount++;
                    }

                    // Decide if face is moving based on attributeIsChanged status and delay count value
                    if ((TRUE == withinLimit) && (TRUE == attributeIsChanged) &&
                        (pConfig->movingMaxDelayCount <= pAttribute->movingDelayCount))
                    {
                        attributeIsMoving = TRUE;
                    }
                    else if ((FALSE == withinLimit) && (TRUE == attributeIsChanged))
                    {
                        attributeIsMoving = TRUE;
                    }
                    else if (FALSE == attributeIsChanged)
                    {
                        UINT32 halfMaxDelayCount = static_cast<UINT32>(static_cast<FLOAT>(pConfig->movingMaxDelayCount) / 2
                                                   + 0.5);
                        if (halfMaxDelayCount < pAttribute->movingDelayCount)
                        {
                            pAttribute->movingDelayCount = halfMaxDelayCount;
                        }
                        else
                        {
                            pAttribute->movingDelayCount = 0;
                        }
                    }
                    CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: withinThreshold=%d, isChanged=%d, movingDelayCount=%d, "
                                     "maxDelayCount=%d; isMoving=%d", attributIndex, withinLimit, attributeIsChanged,
                                     pAttribute->movingDelayCount, pConfig->movingMaxDelayCount, attributeIsMoving);
                }

                if (TRUE == attributeIsMoving)
                {
                    ApplyFilter(pAttribute, attributIndex, pConfig, attributeIsMoving);
                    newState = StabilizingState;
                }
                else if (TRUE == withinLimit)
                {
                    if (TRUE == IsContinuesMode(pConfig->mode))
                    {
                        objectStable = IsStable(pAttribute, pReference, pConfig, pSizeAttribute);
                        if (FALSE == objectStable)
                        {
                            ApplyFilter(pAttribute, attributIndex, pConfig, attributeIsMoving);
                        }
                    }
                }
                else if (TRUE == attributeIsChanged)
                {
                    ApplyFilter(pAttribute, attributIndex, pConfig, attributeIsChanged);
                    newState          = StabilizingState;
                    attributeIsMoving = TRUE;
                }
                else
                {
                    newState = UnstableState;
                }
                CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: withinThreshold=%d, isMoving=%d; %s state",
                                 attributIndex, withinLimit, attributeIsMoving, (newState == 0) ? "statys in Stable" :
                                 (newState == 1) ? "goes to Unstable" : "goes to Stablizing");
                break;
            case StabilizingState:
                if (TRUE == *pObjectIsMoving)
                {
                    attributeIsMoving = TRUE;
                    CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: object is moving; isMoving=%d", attributIndex,
                                     attributeIsMoving);
                }
                else
                {
                    if (0 == pAttribute->stateCount)
                    {
                        pAttribute->movingHoldCount = 0;
                    }

                    // Decide if face is moving based on hold count value
                    if (TRUE == attributeIsChanged)
                    {
                        pAttribute->movingHoldCount = pConfig->movingMaxHoldCount;
                        attributeIsMoving = TRUE;
                    }
                    else if (0 < pAttribute->movingHoldCount)
                    {
                        pAttribute->movingHoldCount--;
                        attributeIsMoving = TRUE;
                    }
                    CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: withinThreshold=%d, isChanged=%d, movingHoldCount=%d; "
                                     "isMoving=%d", attributIndex, withinLimit, attributeIsChanged,
                                     pAttribute->movingHoldCount, attributeIsMoving);
                }

                if (TRUE == attributeIsMoving)
                {
                    pAttribute->stateCount = pConfig->movingInitStateCount;
                }
                else
                {
                    // check if the state count execeeds the max state count
                    if ((pConfig->minStableState > 0) && (pAttribute->stateCount < pConfig->minStableState))
                    {
                        objectStable = FALSE;
                    }
                    else
                    {
                        objectStable = IsStable(pAttribute, pReference, pConfig, pSizeAttribute);
                    }
                    if (TRUE == objectStable)
                    {
                        if (NULL != pReference)
                        {
                            pAttribute->referenceEntry = *pReference;
                        }
                        newState = StableState;
                    }
                }

                // apply filter
                if (TRUE == IsContinuesMode(pConfig->mode))
                {
                    pAttribute->stableEntry = pAttribute->entryHistory[pAttribute->index];
                }
                else
                {
                    ApplyFilter(pAttribute, attributIndex, pConfig, attributeIsMoving);
                }

                if (FALSE == objectStable)
                {
                    attributeIsMoving = TRUE;
                }
                CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: isMoving=%d, minStateCount=%d, isStable=%d; %s state",
                                 attributIndex, attributeIsMoving, pConfig->minStableState, objectStable, (newState == 0) ?
                                 "goes to Stable" : (newState == 1) ? "goes to Unstable" : "stays in Stablizing");

                break;
            default:
                CAMX_LOG_ERROR(CamxLogGroupFD, "Attribute %d: invalid stabilization state", attributIndex);
                break;
        }

        if (TRUE == attributeIsMoving)
        {
            *pObjectIsMoving = TRUE;
        }

        if (newState != pAttribute->state)
        {
            pAttribute->state      = newState;
            pAttribute->stateCount = 0;
        }
        else
        {
            pAttribute->stateCount++;
        }
    }
    else
    {
        CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: history length is less than 3, bypass stab filter", attributIndex);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::InitializeObjectEntry
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::InitializeObjectEntry(
    StabilizationObject* pHistoryEntry,
    StabilizationHolder* pNewObjectEntry)
{
    CAMX_ASSERT(NULL != pHistoryEntry);
    CAMX_ASSERT(NULL != pNewObjectEntry);

    UINT32 index = 0;

    // Clear out the current location in history
    Utils::Memset(pHistoryEntry, 0x0, sizeof(*pHistoryEntry));

    pHistoryEntry->numAttributes = pNewObjectEntry->numAttributes;

    AddObjectEntry(pHistoryEntry, pNewObjectEntry, &m_configuration);

    for (UINT32 i = 0; i < pNewObjectEntry->numAttributes; i++)
    {
        if (TRUE == m_configuration.attributeConfigs[i].enable)
        {
            index                                          = pHistoryEntry->objectAttributes[i].index;
            pHistoryEntry->objectAttributes[i].state       = StableState;
            pHistoryEntry->objectAttributes[i].stateCount  = 0;
            pHistoryEntry->objectAttributes[i].stableEntry = pHistoryEntry->objectAttributes[i].entryHistory[index];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::AddObjectEntry
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::AddObjectEntry(
    StabilizationObject* pHistoryEntry,
    StabilizationHolder* pNewObjectEntry,
    StabilizationConfig* pConfig)
{
    CAMX_ASSERT(NULL != pHistoryEntry);
    CAMX_ASSERT(NULL != pNewObjectEntry);

    UINT32 index = 0;

    pHistoryEntry->id = pNewObjectEntry->id;

    for (UINT32 i = 0; i < pNewObjectEntry->numAttributes; i++)
    {
        if (TRUE == pConfig->attributeConfigs[i].enable)
        {
            index = (pHistoryEntry->objectAttributes[i].index + 1) % (pConfig->historyDepth);

            pHistoryEntry->objectAttributes[i].index               = index;
            pHistoryEntry->objectAttributes[i].entryHistory[index] = pNewObjectEntry->attributeData[i].entry;

            pHistoryEntry->objectAttributes[i].historySize = pConfig->historyDepth;
            if (pHistoryEntry->objectAttributes[i].numEntries < pConfig->historyDepth)
            {
                pHistoryEntry->objectAttributes[i].numEntries++;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::CheckObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StabilizationPosition Stabilization::CheckObject(
    StabilizationObject* pHistoryEntry,
    StabilizationHolder* pCurrentObject)
{
    CAMX_ASSERT(NULL != pHistoryEntry);
    CAMX_ASSERT(NULL != pCurrentObject);

    StabilizationPosition result         = ObjectSame;
    UINT32                thresholdX     = 0;
    UINT32                thresholdY     = 0;
    UINT32                centerCurrentX = 0;
    UINT32                centerCurrentY = 0;
    UINT32                centerHistoryX = 0;
    UINT32                centerHistoryY = 0;
    UINT32                deltaX         = 0;
    UINT32                deltaY         = 0;

    thresholdX     = ((pHistoryEntry->objectAttributes[ObjectSizeIndex].stableEntry.data0) <
                      (pCurrentObject->attributeData[ObjectSizeIndex].entry.data0))        ?
                      (pHistoryEntry->objectAttributes[ObjectSizeIndex].stableEntry.data0) :
                      (pCurrentObject->attributeData[ObjectSizeIndex].entry.data0);

    thresholdY     = ((pHistoryEntry->objectAttributes[ObjectSizeIndex].stableEntry.data1) <
                      (pCurrentObject->attributeData[ObjectSizeIndex].entry.data1))        ?
                      (pHistoryEntry->objectAttributes[ObjectSizeIndex].stableEntry.data1) :
                      (pCurrentObject->attributeData[ObjectSizeIndex].entry.data1);

    centerHistoryX = pHistoryEntry->objectAttributes[ObjectPositionIndex].stableEntry.data0;
    centerHistoryY = pHistoryEntry->objectAttributes[ObjectPositionIndex].stableEntry.data1;

    centerCurrentX = pCurrentObject->attributeData[ObjectPositionIndex].entry.data0;
    centerCurrentY = pCurrentObject->attributeData[ObjectPositionIndex].entry.data1;

    deltaX         = Utils::AbsoluteINT32(static_cast<INT32>(centerHistoryX) - static_cast<INT32>(centerCurrentX));
    deltaY         = Utils::AbsoluteINT32(static_cast<INT32>(centerHistoryY) - static_cast<INT32>(centerCurrentY));

    if (((0 == pCurrentObject->id) ||
         (0 == pHistoryEntry->id)) &&
        ((deltaX < thresholdX)     &&
         (deltaY < thresholdY)))
    {
        result = ObjectSame;
    }
    else if ((0                 != pHistoryEntry->id)  &&
             (0                 != pCurrentObject->id) &&
             (pHistoryEntry->id == pCurrentObject->id))
    {
        result = ObjectSame;
    }
    else if ((centerHistoryY   > centerCurrentY)   ||
             ((centerHistoryY == centerCurrentY)   &&
              (centerHistoryX  > centerCurrentX)))
    {
        result = ObjectBefore;
    }
    else
    {
        result = ObjectAfter;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::WithinThreshold
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Stabilization::WithinThreshold(
    StabilizationEntry* pCurrentEntry,
    StabilizationEntry* pStableEntry,
    UINT32              threshold0,
    UINT32              threshold1)
{
    CAMX_ASSERT(NULL != pCurrentEntry);
    CAMX_ASSERT(NULL != pStableEntry);

    return ((Utils::AbsoluteINT32(pCurrentEntry->data0 - pStableEntry->data0)) < threshold0) &&
           ((Utils::AbsoluteINT32(pCurrentEntry->data1 - pStableEntry->data1)) < threshold1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::IsContinuesMode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Stabilization::IsContinuesMode(
    StabilizationMode mode)
{
    BOOL continuesMode = FALSE;

    switch (mode)
    {
        case StabilizationMode::Equal:
        case StabilizationMode::Smaller:
        case StabilizationMode::Bigger:
        case StabilizationMode::CloserToReference:
        case StabilizationMode::WithinThreshold:
            continuesMode = FALSE;
            break;
        case StabilizationMode::ContinuesEqual:
        case StabilizationMode::ContinuesSmaller:
        case StabilizationMode::ContinuesBigger:
        case StabilizationMode::ContinuesCloserToReference:
            continuesMode = TRUE;
            break;
        default:
            CAMX_LOG_ERROR(CamxLogGroupFD, "Invalid stabilization mode");
            break;
    }

    return continuesMode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::IsStable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Stabilization::IsStable(
    StabilizationAttribute*       pCurrentEntry,
    StabilizationEntry*           pReference,
    StabilizationAttributeConfig* pConfig,
    StabilizationAttribute*       pSizeAttribute)
{
    CAMX_ASSERT(NULL != pCurrentEntry);

    BOOL    stable        = FALSE;
    INT32   deltaStableX  = 0;
    INT32   deltaStableY  = 0;
    INT32   deltaCurrentX = 0;
    INT32   deltaCurrentY = 0;
    UINT32  threshold0;
    UINT32  threshold1;

    switch (pConfig->mode)
    {
        case StabilizationMode::ContinuesSmaller:
        case StabilizationMode::Smaller:
            stable = ((pCurrentEntry->stableEntry.data0 < pCurrentEntry->entryHistory[pCurrentEntry->index].data0) &&
                      (pCurrentEntry->stableEntry.data1 < pCurrentEntry->entryHistory[pCurrentEntry->index].data1));
            break;
        case StabilizationMode::ContinuesBigger:
        case StabilizationMode::Bigger:
            stable = ((pCurrentEntry->stableEntry.data0 > pCurrentEntry->entryHistory[pCurrentEntry->index].data0) &&
                      (pCurrentEntry->stableEntry.data1 > pCurrentEntry->entryHistory[pCurrentEntry->index].data1));
            break;
        case StabilizationMode::ContinuesCloserToReference:
        case StabilizationMode::CloserToReference:
            if (NULL != pReference)
            {
                deltaStableX  = pCurrentEntry->stableEntry.data0 - pReference->data0;
                deltaStableY  = pCurrentEntry->stableEntry.data1 - pReference->data1;
                deltaCurrentX = pCurrentEntry->entryHistory[pCurrentEntry->index].data0 - pReference->data0;
                deltaCurrentY = pCurrentEntry->entryHistory[pCurrentEntry->index].data1 - pReference->data1;
                stable = (((deltaStableX * deltaStableX) + (deltaStableY * deltaStableY)) <
                          ((deltaCurrentX * deltaCurrentX) + (deltaCurrentY * deltaCurrentY)));
            }
            else
            {
                stable = TRUE;
            }
            break;
        case StabilizationMode::WithinThreshold:
            threshold0 = static_cast<UINT32> (((pConfig->stableThreshold *
                                                static_cast<UINT32>(Utils::Sqrt(pSizeAttribute->stableEntry.data0))) /
                                               WithinThresholdRatio) + 1);
            threshold1 = static_cast<UINT32> (((pConfig->stableThreshold *
                                                static_cast<UINT32>(Utils::Sqrt(pSizeAttribute->stableEntry.data1))) /
                                               WithinThresholdRatio) + 1);

            stable = WithinThreshold(&pCurrentEntry->entryHistory[pCurrentEntry->index],
                                     &pCurrentEntry->stableEntry,
                                     threshold0, threshold1);
            break;
        case StabilizationMode::Equal:
        default:
            stable = ((pCurrentEntry->stableEntry.data0 == pCurrentEntry->entryHistory[pCurrentEntry->index].data0) &&
                      (pCurrentEntry->stableEntry.data1 == pCurrentEntry->entryHistory[pCurrentEntry->index].data1));
            break;
    }

    return stable;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::CheckAttributeMovement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Stabilization::CheckAttributeMovement(
    StabilizationEntry*           pCurrentEntry,
    StabilizationEntry*           pLastEntry,
    StabilizationEntry*           pLastLastEntry,
    UINT32                        attributIndex,
    StabilizationAttributeConfig* pConfig,
    BOOL                          atFrameBoundary,
    UINT32                        movingThreshold)
{
    CAMX_ASSERT(NULL != pCurrentEntry);
    CAMX_ASSERT(NULL != pLastEntry);
    CAMX_ASSERT(NULL != pLastLastEntry);

    static INT32 frameWidth         = static_cast<INT32>(m_frameWidth);
    static INT32 frameHeight        = static_cast<INT32>(m_frameHeight);
    BOOL         attributeIsChanged = FALSE;
    INT32        threshold;
    FLOAT        linkFactor;

    // Check if face is moving
    FLOAT move1 = Utils::SqrtF(static_cast<FLOAT>(
                  ((pLastEntry->data0 - pLastLastEntry->data0) * (pLastEntry->data0 - pLastLastEntry->data0)) +
                  ((pLastEntry->data1 - pLastLastEntry->data1) * (pLastEntry->data1 - pLastLastEntry->data1))));
    FLOAT move2 = Utils::SqrtF(static_cast<FLOAT>(
                  ((pCurrentEntry->data0 - pLastEntry->data0) * (pCurrentEntry->data0 - pLastEntry->data0)) +
                  ((pCurrentEntry->data1 - pLastEntry->data1) * (pCurrentEntry->data1 - pLastEntry->data1))));
    FLOAT move3 = Utils::SqrtF(static_cast<FLOAT>(
                  ((pCurrentEntry->data0 - pLastLastEntry->data0) * (pCurrentEntry->data0 - pLastLastEntry->data0)) +
                  ((pCurrentEntry->data1 - pLastLastEntry->data1) * (pCurrentEntry->data1 - pLastLastEntry->data1))));

    if (TRUE == atFrameBoundary)
    {
        // Face is at boundary, check with higher threshold and linkFactor
        threshold  = movingThreshold * pConfig->defaultMotionDetector.movingScalerAtBoundary;
        linkFactor = pConfig->defaultMotionDetector.movingLinkFactor * pConfig->defaultMotionDetector.movingScalerAtBoundary;
    }
    else
    {
        // Face is not at boundary, check with normal threshold and linkFactor
        threshold  = movingThreshold;
        linkFactor = pConfig->defaultMotionDetector.movingLinkFactor;
    }

    if ((move3 > (move1 * linkFactor)) && (move1 > threshold) && (move2 > threshold))
    {
        attributeIsChanged = TRUE;
    }

    CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: Move1=%f, Move2=%f, Move3=%f, threshold=%d, linkFactor=%f, isChanged=%d",
                     attributIndex, move1, move2, move3, threshold, linkFactor, attributeIsChanged);

    return attributeIsChanged;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::CheckSizeAttributeMovement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Stabilization::CheckSizeAttributeMovement(
    StabilizationAttribute*       pAttribute,
    StabilizationAttributeConfig* pConfig,
    UINT32                        attributIndex)
{
    INT32 filteredValueX[4]  = { 0 };
    INT32 filteredValueY[4]  = { 0 };
    INT32 signX[3]           = { 0 };
    INT32 signY[3]           = { 0 };
    INT32 resultX            = 0;
    INT32 resultY            = 0;
    BOOL  attributeIsChanged = FALSE;
    INT32 threshold          = pConfig->sizeMotionDetector.cutOffThreshold;

    for (UINT32 i = 0; i < 4; i++)
    {
        UINT32 index;

        if (0 == i)
        {
            index = pAttribute->index;
        }
        else
        {
            index = (pAttribute->index + pAttribute->numEntries - i) % pAttribute->numEntries;
        }
        filteredValueX[i] = pAttribute->entryHistory[index].filteredData0;
        filteredValueY[i] = pAttribute->entryHistory[index].filteredData1;
    }

    for (UINT32 i = 0; i < 3; i++)
    {
        signX[i] =  (filteredValueX[3-i-1]-filteredValueX[3-i] > 0) ? 1 : (filteredValueX[3-i-1]-filteredValueX[3-i] == 0)
                     ? 0 : -1;
        signY[i] =  (filteredValueY[3-i-1]-filteredValueY[3-i] > 0) ? 1 : (filteredValueY[3-i-1]-filteredValueY[3-i] == 0)
                     ? 0 : -1;
        resultX  += signX[i];
        resultY  += signY[i];
    }

    if ((resultX >= threshold) || (resultX <= (-1) * threshold) || (resultY >= threshold) || (resultY <= (-1) * threshold))
    {
        attributeIsChanged = TRUE;
    }

    CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: widthChange=%d %d %d, heightChange=%d %d %d; isChanged=%d",
                     attributIndex, signX[0], signX[1], signX[2], signY[0], signY[1], signY[2], attributeIsChanged);

    return attributeIsChanged;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::ApplyAverageForSize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::ApplyAverageForSize(
    StabilizationAttribute*       pAttribute,
    StabilizationAttributeConfig* pConfig)
{
    CAMX_ASSERT(NULL != pAttribute);

    UINT32 index         = 0;
    UINT64 sumData0      = 0;
    UINT64 sumData1      = 0;
    UINT32 filterLength  = pConfig->averageFilter.preFilterHistoryLength;

    if ((2 > pAttribute->numEntries) || (2 > filterLength))
    {
        pAttribute->entryHistory[pAttribute->index].filteredData0 = pAttribute->entryHistory[pAttribute->index].data0;
        pAttribute->entryHistory[pAttribute->index].filteredData1 = pAttribute->entryHistory[pAttribute->index].data1;
    }
    else
    {
        if (filterLength > pAttribute->numEntries)
        {
            filterLength = pAttribute->numEntries;
        }

        index = (pAttribute->index + pAttribute->numEntries - filterLength + 1) % pAttribute->numEntries;

        for (UINT32 i = 0; i < filterLength; i++)
        {
            index %= pAttribute->historySize;
            sumData0 += pAttribute->entryHistory[index].data0;
            sumData1 += pAttribute->entryHistory[index].data1;
            index++;
        }

        pAttribute->entryHistory[pAttribute->index].filteredData0 = static_cast<INT32>((static_cast<FLOAT>(sumData0) /
                                                                    filterLength) + 0.5);
        pAttribute->entryHistory[pAttribute->index].filteredData1 = static_cast<INT32>((static_cast<FLOAT>(sumData1) /
                                                                    filterLength) + 0.5);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::ApplyFilter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::ApplyFilter(
    StabilizationAttribute*       pAttribute,
    UINT32                        attributIndex,
    StabilizationAttributeConfig* pConfig,
    BOOL                          attributeIsMoving)
{
    CAMX_ASSERT(NULL != pAttribute);
    CAMX_ASSERT(NULL != pConfig);

    StabilizationEntry* pCurrentEntry = &pAttribute->stableEntry;
    StabilizationEntry* pNewEntry     = &pAttribute->entryHistory[pAttribute->index];

    switch (pConfig->filterType)
    {
        case StabilizationFilter::Hysteresis:
            if (pNewEntry->data0 > pCurrentEntry->data0)
            {
                ApplyHysteresis(&pCurrentEntry->data0, &pNewEntry->data0, pConfig, TRUE);
            }
            else
            {
                ApplyHysteresis(&pCurrentEntry->data0, &pNewEntry->data0, pConfig, FALSE);
            }
            if (pNewEntry->data1 > pCurrentEntry->data1)
            {
                ApplyHysteresis(&pCurrentEntry->data1, &pNewEntry->data1, pConfig, TRUE);
            }
            else
            {
                ApplyHysteresis(&pCurrentEntry->data1, &pNewEntry->data1, pConfig, FALSE);
            }
            break;
        case StabilizationFilter::Temporal:
            ApplyTemporal(&pCurrentEntry->data0,
                          pConfig->temporalFilter.numerator,
                          &pNewEntry->data0,
                          pConfig->temporalFilter.denominator);
            ApplyTemporal(&pCurrentEntry->data1,
                          pConfig->temporalFilter.numerator,
                          &pNewEntry->data1,
                          pConfig->temporalFilter.denominator);
            break;
        case StabilizationFilter::Average:
            ApplyAverage(pAttribute, attributIndex, pConfig, attributeIsMoving);
            break;
        case StabilizationFilter::Median:
            ApplyMedian(pAttribute, pConfig);
            break;
        case StabilizationFilter::NoFilter:
        default:
            *pCurrentEntry = *pNewEntry;
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::ApplyHysteresis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::ApplyHysteresis(
    INT32*                        pCurrentEntry,
    INT32*                        pNewEntry,
    StabilizationAttributeConfig* pConfig,
    BOOL                          upDirection)
{
    CAMX_ASSERT(NULL != pCurrentEntry);
    CAMX_ASSERT(NULL != pNewEntry);

    if (TRUE == upDirection)
    {
        if (*pNewEntry > static_cast<INT32>(pConfig->hysteresisFilter.endB))
        {
            *pCurrentEntry = *pNewEntry;
        }
        else if (*pNewEntry > static_cast<INT32>(pConfig->hysteresisFilter.startB))
        {
            *pNewEntry = pConfig->hysteresisFilter.startB;
        }
        else if (*pNewEntry > static_cast<INT32>(pConfig->hysteresisFilter.endA))
        {
            *pCurrentEntry = *pNewEntry;
        }
        else if (*pNewEntry > static_cast<INT32>(pConfig->hysteresisFilter.startA))
        {
            *pNewEntry = pConfig->hysteresisFilter.startA;
        }
        else
        {
            *pCurrentEntry = *pNewEntry;
        }
    }
    else
    {
        if (*pNewEntry < static_cast<INT32>(pConfig->hysteresisFilter.startA))
        {
            *pCurrentEntry = *pNewEntry;
        }
        else if (*pNewEntry < static_cast<INT32>(pConfig->hysteresisFilter.endA))
        {
            *pNewEntry = pConfig->hysteresisFilter.endA;
        }
        else if (*pNewEntry < static_cast<INT32>(pConfig->hysteresisFilter.startB))
        {
            *pCurrentEntry = *pNewEntry;
        }
        else if (*pNewEntry < static_cast<INT32>(pConfig->hysteresisFilter.endB))
        {
            *pNewEntry = pConfig->hysteresisFilter.endB;
        }
        else
        {
            *pCurrentEntry = *pNewEntry;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::ApplyTemporal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::ApplyTemporal(
    INT32* pFirst,
    UINT32 weightFirst,
    INT32* pSecond,
    UINT32 weightSecond)
{
    CAMX_ASSERT(NULL != pFirst);
    CAMX_ASSERT(NULL != pSecond);

    if ((weightFirst + weightSecond) != 0)
    {
        *pFirst = (((*pSecond * weightSecond) +
                    (*pFirst * weightFirst)) /
                   (weightFirst + weightSecond));
    }
    else
    {
        *pFirst = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::ApplyAverage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::ApplyAverage(
    StabilizationAttribute*       pAttribute,
    UINT32                        attributIndex,
    StabilizationAttributeConfig* pConfig,
    BOOL                          attributeIsMoving)
{
    CAMX_ASSERT(NULL != pAttribute);
    CAMX_ASSERT(NULL != pConfig);

    UINT32 index         = 0;
    UINT32 historyLength = 5;
    UINT64 sumData0      = 0;
    UINT64 sumData1      = 0;

    if (TRUE == attributeIsMoving)
    {
        historyLength = pConfig->averageFilter.movingHistoryLength;
    }
    else
    {
        historyLength = pConfig->averageFilter.historyLength;
    }

    if (historyLength < 2)
    {
        pAttribute->stableEntry = pAttribute->entryHistory[pAttribute->index];
    }
    else
    {
        if (historyLength > pAttribute->numEntries)
        {
            historyLength = pAttribute->numEntries;
        }

        index = (pAttribute->index + pAttribute->numEntries - historyLength + 1) % pAttribute->numEntries;

        for (UINT32 i = 0; i < historyLength; i++)
        {
            index %= pAttribute->historySize;
            sumData0 += pAttribute->entryHistory[index].data0;
            sumData1 += pAttribute->entryHistory[index].data1;

            CAMX_LOG_VERBOSE(CamxLogGroupFD, "Attribute %d: apply average. history %d, data0 = %d, data1 = %d",
                             attributIndex,
                             i,
                             pAttribute->entryHistory[index].data0,
                             pAttribute->entryHistory[index].data1);

            index++;
        }

        pAttribute->stableEntry.data0 = static_cast<INT32>((sumData0 / historyLength));
        pAttribute->stableEntry.data1 = static_cast<INT32>((sumData1 / historyLength));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stabilization::ApplyMedian
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Stabilization::ApplyMedian(
    StabilizationAttribute*       pAttribute,
    StabilizationAttributeConfig* pConfig)
{
    CAMX_ASSERT(NULL != pAttribute);
    CAMX_ASSERT(NULL != pConfig);

    UINT32              index                                   = pAttribute->index;
    StabilizationEntry  medianEntry[StabilizationMaxHistory]    = { {0} };

    if (pConfig->medianFilter.historyLength > pAttribute->numEntries)
    {
        pAttribute->stableEntry = pAttribute->entryHistory[index];
    }
    else
    {
        for (UINT32 i = 0; i < pConfig->medianFilter.historyLength; i++)
        {
            medianEntry[i] = pAttribute->entryHistory[index % pAttribute->historySize];
            index++;
        }

        Utils::Qsort(&medianEntry[0].data0, pConfig->medianFilter.historyLength, sizeof(medianEntry[0]), MedianSort);

        index                         = (pConfig->medianFilter.historyLength / 2);
        pAttribute->stableEntry.data0 = medianEntry[index].data0;

        Utils::Qsort(&medianEntry[0].data1, pConfig->medianFilter.historyLength, sizeof(medianEntry[0]), MedianSort);

        index                         = (pConfig->medianFilter.historyLength / 2);
        pAttribute->stableEntry.data1 = medianEntry[index].data1;
    }
}

CAMX_NAMESPACE_END
