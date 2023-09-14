////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file camxstabilization.h
///
/// @brief Defines a stabilization class.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXSTABILIZATION_H
#define CAMXSTABILIZATION_H

#include "camxdefs.h"
#include "camxtypes.h"

CAMX_NAMESPACE_BEGIN

static const UINT32 StabilizationMaxHistory          = 10;
static const UINT32 StabilizationMaxObjectAttributes = 10;
static const UINT32 StabilizationMaxObjects          = 10;

/// @brief enum to define the history object attribute reserved indexes
enum StabilizationIndex
{
    ObjectSizeIndex,       ///< Index in the stabilization history that is reserved for size of the object.
                           ///< Keep ObjectSizeIndex smaller than ObjectPositionIndex in order to process size attribute first
                           ///< and if size is triggered as moving in current frame, directly make position as moving too
    ObjectPositionIndex,   ///< Index in the stabilization history that is reserved for position of the object
    ObjectUnreservedIndex  ///< Index in the stabilization history that starts the unreserved index section
};

/// @brief enum to define all the different stabilization states
enum StabilizationState
{
    StableState,     ///< State is stable, previous data will be used
    UnstableState,   ///< State is unstable, previous data will be used
    StabilizingState ///< Stabilizing, switch to stable state along with new position data
};

/// @brief enum to define all the stabilization object position types
enum StabilizationPosition
{
    ObjectSame,   ///< Object is considered to be the same
    ObjectBefore, ///< Object is considered to be before
    ObjectAfter   ///< Object is considered to be after
};

/// @brief enum to define all the different stabilization modes
enum StabilizationMode
{
    Equal,                          ///< Values will be marked as stable when two consecutive values are equal
    Smaller,                        ///< Values will be marked as stable if new values are bigger than old ones
    Bigger,                         ///< Values will be marked as stable if new values are smaller than old ones
    CloserToReference,              ///< Values will be marked as stable when the distance to reference is smaller
    ContinuesEqual,                 ///< The same as Equal, but it works in continues mode
    ContinuesSmaller,               ///< The same as Smaller, but it works in continues mode
    ContinuesBigger,                ///< The same as Bigger, but it works in continues mode
    ContinuesCloserToReference,     ///< The same as CloserToReference, but it works in continues mode
    WithinThreshold                 ///< Values are marked as stable when values are within threshold
};

/// @brief enum to define all the different stabilization filters
enum StabilizationFilter
{
    NoFilter,      ///< No stabilization filter
    Temporal,      ///< Temporal filter
    Hysteresis,    ///< Hysteresis
    Average,       ///< Average filter
    Median         ///< Median filter
};

/// @brief Structure to define Temporal Filter
struct TemporalFilter
{
    UINT32 numerator;    ///< Strength numerator
    UINT32 denominator;  ///< Strength denominator
};

/// @brief Structure to define Hysteresis Filter
struct HysteresisFilter
{
    UINT32 startA;    ///< Start point of Zone A
    UINT32 endA;      ///< End point of Zone A
    UINT32 startB;    ///< Start point of Zone B
    UINT32 endB;      ///< End point of Zone B
};

/// @brief Structure to define Average Filter
struct AverageFilter
{
    UINT32 historyLength;           ///< History length of the filter
    UINT32 movingHistoryLength;     ///< History length of the filter for moving attribute
    UINT32 preFilterHistoryLength;  ///< History length of the prefilter only for size attribute
};

/// @brief Structure to define Median Filter
struct MedianFilter
{
    UINT32 historyLength;  ///< History length of the filter
};

/// @brief Structure to define default motion detector parameters
struct DefaultMotionDetector
{
    FLOAT  movingLinkFactor;       ///< Scale factor for determining if attribute is changed or not with previous 2 history
    UINT32 movingThreshold;        ///< Threshold for determining if attribute is changed or not
    UINT32 movingScalerAtBoundary; ///< Scale factor for determining if attribute is changed or not when object is at boundary
};

/// @brief Structure to define size attribute motion detector parameters
struct SizeMotionDetector
{
    INT32 cutOffThreshold;        ///< Threshold for determining if size is chanaged or not
};

/// @brief Structure to hold stabilization tuning parameters
struct StabilizationAttributeConfig
{
    BOOL                  enable;                ///< Enable stabilization
    StabilizationMode     mode;                  ///< Stabilization Mode
    UINT32                minStableState;        ///< Minimum state count needed to go to a stable state
    UINT32                stableThreshold;       ///< Stabilization threshold to go into stable state
    UINT32                threshold;             ///< Within threshold, new values will not be accepted
    UINT32                maxUnstableStateCount; ///< Number of consecutive frames to wait until determining stable
    UINT32                useReference;          ///< Stabilize data by a reference
    StabilizationFilter   filterType;            ///< Filter type to be used for stabilization
    UINT32                movingInitStateCount;  ///< Init state count for moving attribute during stabilizing state
    UINT32                movingMaxDelayCount;   ///< Maximum of delayCount to determine if attribute is moving in Stable
                                                 ///< state
    UINT32                movingMaxHoldCount;    ///< Maximum of holdCount to determine if attribute stops moving in
                                                 ///< Stabilizing state
    DefaultMotionDetector defaultMotionDetector; ///< motion detector by default
    SizeMotionDetector    sizeMotionDetector;    ///< motion detector for size attribute

    union
    {
        TemporalFilter   temporalFilter;     ///< Temporal Filter
        HysteresisFilter hysteresisFilter;   ///< HysteresisFilter
        AverageFilter    averageFilter;      ///< Average Filter
        MedianFilter     medianFilter;       ///< Median Filter
    };
};

/// @brief Structure to hold stabilization configuration
struct StabilizationConfig
{
    BOOL                         enable;                                             ///< Enable stabilization
    UINT32                       historyDepth;                                       ///< Depth of historical data
    FLOAT                        boundaryCaseFactor;                                 ///< Percentage factor of face ROI size to
                                                                                     ///< decide if object is at frame boundary
    StabilizationAttributeConfig attributeConfigs[StabilizationMaxObjectAttributes]; ///< Array of attibute configurations
};

/// @brief Structure to define a stabilization entry
struct StabilizationEntry
{
    INT32 data0;         ///< Arbitrary data 0
    INT32 data1;         ///< Arbitrary data 1
    INT32 filteredData0; ///< Filtered data 0
    INT32 filteredData1; ///< Filtered data 1
};

/// @brief Structure to hold stabilization data for a particular object attribute
struct StabilizationAttribute
{
    UINT32             index;                                    ///< Current acive index in the history
    UINT32             historySize;                              ///< Size of the history
    UINT32             numEntries;                               ///< Number of data entries in history
    UINT32             stateCount;                               ///< Number of frames in current state
    UINT32             movingDelayCount;                         ///< For stable state, number of frames holding for
                                                                 ///< deciding if face is moving
    UINT32             movingHoldCount;                          ///< For stablilizing state, number of frames holding for
                                                                 ///< deciding if face is not moving
    StabilizationState state;                                    ///< Current state
    StabilizationEntry stableEntry;                              ///< Current stable entry
    StabilizationEntry referenceEntry;                           ///< Reference if stabilization is used with reference
    StabilizationEntry entryHistory[StabilizationMaxHistory];    ///< Array of historical data entries
};

/// @brief Structure to hold stabilization data for a particular object
struct StabilizationObject
{
    UINT32                 id;                                                 ///< Unique object identification
    VOID*                  pFaceInfo;                                          ///< Face info for this object
    UINT32                 numAttributes;                                      ///< Number of attributes to track
    StabilizationAttribute objectAttributes[StabilizationMaxObjectAttributes]; ///< Attributes of an object to stabilize
};

/// @brief Structure to hold the historical stabilization data
struct StabilizationHistory
{
    UINT32              numObjects;                       ///< Number of objects in the history
    StabilizationObject objects[StabilizationMaxObjects]; ///< Array of objects for stabilization
};

/// @brief Structure to hold the stabilization data values for the stabilization calculation
struct StabilizationPoint
{
    BOOL               valid;    ///< Wheter the data here is valid
    StabilizationEntry entry;    ///< Data entry
};

/// @brief Structure to hold stabilization for an object's attributes for the current frame
struct StabilizationHolder
{
    UINT32             id;                                                   ///< Unique object identification
    UINT32             numAttributes;                                        ///< Number of attributes for this object
    StabilizationPoint attributeData[StabilizationMaxObjectAttributes];      ///< Array of attribute data of an object
    StabilizationPoint attributeReference[StabilizationMaxObjectAttributes]; ///< Array of attribute references of an object
    VOID*              pFaceInfo;                                            ///< Pointer to original face info for this
};

/// @brief Structure to hold stabilization for an object for current frame
struct StabilizationData
{
    UINT32              numObjects;                             ///< Number of objects for current frame
    StabilizationHolder objectData[StabilizationMaxObjects];    ///< Array of object data
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Class that implements the object Stabilization class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Stabilization
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create Stabilization object
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static Stabilization* Create();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  This method destroys the derived instance of the interface
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize the stabilization object
    ///
    /// @param  pConfig     Pointer to the structure to store the current config
    /// @param  frameWidth  Frame width
    /// @param  frameHeight Frame height
    ///
    /// @return CamxResultSuccess if success
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Initialize(
        StabilizationConfig* pConfig,
        UINT32               frameWidth,
        UINT32               frameHeight);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetConfig
    ///
    /// @brief  Get the stabilization configuration
    ///
    /// @param  pConfig Pointer to the structure to store the current config
    ///
    /// @return CamxResultSuccess if success
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult GetConfig(
        StabilizationConfig* pConfig) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetConfig
    ///
    /// @brief  Set the stabilization configuration
    ///
    /// @param  pConfig  Pointer to config data to set
    ///
    /// @return CamxResultSuccess if success
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult SetConfig(
        StabilizationConfig* pConfig);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ExecuteStabilization
    ///
    /// @brief  Execute stabilization, main function to stabilize current data using history
    ///
    /// @param  pCurrentData    Pointer to current stabilization data
    /// @param  pStabilizedData Pointer to structure that this function will write stabilized data to
    ///
    /// @return return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult ExecuteStabilization(
        StabilizationData* pCurrentData,
        StabilizationData* pStabilizedData);

private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StabilizationFilter
    ///
    /// @brief  Calculate the stabilization state and apply the appropriate filter to the object's attribute
    ///
    /// @param  pAttribute          Pointer to object's attribute to stabilize
    /// @param  attributIndex       Attribute index of the object
    /// @param  pConfig             Pointer to attribute's configuration info
    /// @param  pReference          Pointer to attribute reference entry
    /// @param  threshold0          Threshold for stabilization of data0
    /// @param  threshold1          Threshold for stabilization of data1
    /// @param  movingThreshold     Threshold for moving face
    /// @param  atFrameBoundary     Flag to show if object is at frame boundary
    /// @param  pSizeAttribute      Pointer to object's size attribute
    /// @param  pObjectIsMoving     Flag to decide if object is moving
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID StabilizationFilter(
        StabilizationAttribute*       pAttribute,
        UINT32                        attributIndex,
        StabilizationAttributeConfig* pConfig,
        StabilizationEntry*           pReference,
        UINT32                        threshold0,
        UINT32                        threshold1,
        UINT32                        movingThreshold,
        BOOL                          atFrameBoundary,
        StabilizationAttribute*       pSizeAttribute,
        BOOL*                         pObjectIsMoving);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InitializeObjectEntry
    ///
    /// @brief  Initialized and add new object entry to the history
    ///
    /// @param  pHistoryEntry Pointer to data in the history to write to
    /// @param  pObjectEntry  Pointer to data of the object to add to the history
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID InitializeObjectEntry(
        StabilizationObject* pHistoryEntry,
        StabilizationHolder* pObjectEntry);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddObjectEntry
    ///
    /// @brief  Add new object entry to the history
    ///
    /// @param  pHistoryEntry Pointer to data in the history to write to
    /// @param  pObjectEntry  Pointer to data of the object to add to the history
    /// @param  pConfig       Pointer to the object configuration info
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID AddObjectEntry(
        StabilizationObject* pHistoryEntry,
        StabilizationHolder* pObjectEntry,
        StabilizationConfig* pConfig);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CheckObject
    ///
    /// @brief  Check and determine the position of the current object with respect to the history
    ///
    /// @param  pHistoryEntry  Pointer to data in the history object
    /// @param  pCurrentObject Pointer to data of the current object
    ///
    /// @return StabilizationPosition position used to determine location of current object
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    StabilizationPosition CheckObject(
        StabilizationObject* pHistoryEntry,
        StabilizationHolder* pCurrentObject);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// WithinThreshold
    ///
    /// @brief  Determine if the current data entry is within the threshold of the historical stable entry
    ///
    /// @param  pCurrentEntry Pointer to object attribute's current entry
    /// @param  pStableEntry  Pointer to object attribute's stable entry
    /// @param  threshold0    Threshold for stabilization of data0
    /// @param  threshold1    Threshold for stabilization of data1
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL WithinThreshold(
        StabilizationEntry* pCurrentEntry,
        StabilizationEntry* pStableEntry,
        UINT32              threshold0,
        UINT32              threshold1);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsContinuesMode
    ///
    /// @brief  Determines if the given stabilization filter mode is continues mode
    ///
    /// @param  type The stabilization mode
    ///
    /// @return TRUE if given mode is a continues mode
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL IsContinuesMode(
        StabilizationMode type);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsStable
    ///
    /// @brief  Determine if the given Object's attribute is stable
    ///
    /// @param  pAttribute     Pointer to object's attribute to stabilize
    /// @param  pReference     Pointer to attribute reference entry
    /// @param  pConfig        Pointer to attribute's configuration info
    /// @param  pSizeAttribute Pointer to object's size attribute
    ///
    /// @return TRUE if stable
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL IsStable(
        StabilizationAttribute*       pAttribute,
        StabilizationEntry*           pReference,
        StabilizationAttributeConfig* pConfig,
        StabilizationAttribute*       pSizeAttribute);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CheckAttributeMovement
    ///
    /// @brief  Determine if the given attribute is changed
    ///
    /// @param  pCurrentEntry    Pointer to current object's attribute
    /// @param  pLastEntry       Pointer to last object's attribute
    /// @param  pLastLastEntry   Pointer to last last object's attribute
    /// @param  attributIndex    Attribute index of the object
    /// @param  pConfig          Pointer to attribute's configuration info
    /// @param  atFrameBoundary  Flag to show if object is at frame boundary
    /// @param  threshold0       Threshold for stabilization of data0
    ///
    /// @return TRUE if moving
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL CheckAttributeMovement(
        StabilizationEntry*           pCurrentEntry,
        StabilizationEntry*           pLastEntry,
        StabilizationEntry*           pLastLastEntry,
        UINT32                        attributIndex,
        StabilizationAttributeConfig* pConfig,
        BOOL                          atFrameBoundary,
        UINT32                        threshold0);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CheckSizeAttributeMovement
    ///
    /// @brief  Determine if the given size attribute is changed
    ///
    /// @param  pAttribute     Pointer to object's attribute current entry data
    /// @param  pConfig        Pointer to attribute's configuration info
    /// @param  attributIndex  Attribute index of the object
    ///
    /// @return TRUE if it's changed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL CheckSizeAttributeMovement(
        StabilizationAttribute*        pAttribute,
        StabilizationAttributeConfig*  pConfig,
        UINT32                         attributIndex);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ApplyAverageForSize
    ///
    /// @brief  Apply average filter for attribute data
    ///
    /// @param  pAttribute    Pointer to object's attribute current entry data
    /// @param  pConfig       Pointer to attribute's configuration info
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID ApplyAverageForSize(
        StabilizationAttribute*       pAttribute,
        StabilizationAttributeConfig* pConfig);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ApplyFilter
    ///
    /// @brief  Initialized and add new object entry to the history
    ///
    /// @param  pAttribute         Pointer to object's attribute to stabilize
    /// @param  attributIndex      Attribute index of the object
    /// @param  pConfig            Pointer to attribute's configuration info
    /// @param  attributeIsChanged Flag to determine if attribute is changed
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID ApplyFilter(
        StabilizationAttribute*       pAttribute,
        UINT32                        attributIndex,
        StabilizationAttributeConfig* pConfig,
        BOOL                          attributeIsChanged);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ApplyHysteresis
    ///
    /// @brief  Apply the Hysteresis filter
    ///
    /// @param  pCurrentEntry Pointer to object's attribute current entry data
    /// @param  pNewEntry     Pointer to attribute's new entry data
    /// @param  pConfig       Pointer to attribute's configuration info
    /// @param  upDirection   Is the Hysteresis input data in the up direction
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID ApplyHysteresis(
        INT32*                        pCurrentEntry,
        INT32*                        pNewEntry,
        StabilizationAttributeConfig* pConfig,
        BOOL                          upDirection);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ApplyTemporal
    ///
    /// @brief  Apply the Temporal filter
    ///
    /// @param  pFirst       Pointer to data of the first parameter for the temporal filter
    /// @param  weightFirst  Weight of the first data value
    /// @param  pSecond      Pointer to data of the second parameter for the temporal filter
    /// @param  weightSecond Weight of the second data value
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID ApplyTemporal(
        INT32* pFirst,
        UINT32 weightFirst,
        INT32* pSecond,
        UINT32 weightSecond);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ApplyAverage
    ///
    /// @brief  Apply the Average filter
    ///
    /// @param  pAttribute         Pointer to object's attribute to stabilize
    /// @param  attributIndex      Attribute index of the object
    /// @param  pConfig            Pointer to attribute's configuration info
    /// @param  attributeIsChanged Flag to determine if attribute is changed
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID ApplyAverage(
        StabilizationAttribute*       pAttribute,
        UINT32                        attributIndex,
        StabilizationAttributeConfig* pConfig,
        BOOL                          attributeIsChanged);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ApplyMedian
    ///
    /// @brief  Apply the Median filter
    ///
    /// @param  pAttribute Pointer to object's attribute to stabilize
    /// @param  pConfig    Pointer to attribute's configuration info
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID ApplyMedian(
        StabilizationAttribute*       pAttribute,
        StabilizationAttributeConfig* pConfig);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Stabilization
    ///
    /// @brief  Default constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Stabilization();
    Stabilization(const Stabilization&) = delete;                 ///< Disallow the copy constructor.
    Stabilization& operator=(const Stabilization&) = delete;      ///< Disallow assignment operator.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~Stabilization
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ~Stabilization();

    StabilizationHistory m_historicalData;  ///< Historical stabilization data
    StabilizationConfig  m_configuration;   ///< Stabilization configuration info
    UINT32               m_frameWidth;      ///< Frame width
    UINT32               m_frameHeight;     ///< Frame height
};

CAMX_NAMESPACE_END

#endif // CAMXSTABILIZATION_H
