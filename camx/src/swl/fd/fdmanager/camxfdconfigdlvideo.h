////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxfdconfigdlvideo.h
/// @brief FD video configuration for DL solution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

.maxNumberOfFaces             = 10,
.maxFPSWithFaces              = 15,
.maxFPSWithNoFaces            = 15,
.multiCameraMaxFPSWithFaces   = 15,
.multiCameraMaxFPSWithNoFaces = 15,
.lockDetectedFaces            = TRUE,
.initialNoFrameSkipCount      = 15,
.maxPendingFrames             = 2,
.delayCount                   = 1,
.holdCount                    = 2,
.accuracy                     = FDAccuracyNormal,

.hwConfig =
{
    .enable                 = TRUE,
    .minFaceSize =
    {
        .type               = FDFaceAdjFixed,
        .size               = 32,
    },
    .enableUpFrontAngles    = FALSE,
    .angle                  = FDAngleAll,
    .threshold              = 1,
    .noFaceFrameSkip        = 0,
    .newFaceFrameSkip       = 0,
    .enableHWFP             = FALSE,
},

.FPFilter =
{
    .enable                     = TRUE,
    .baseThreshold              = 500,
    .innerThreshold             = 800,
    .expandFaceSizePercentage   = 25,
    .expandBoxBorderPercentage  = 25,
    .faceSpreadTolerance        = 0.3,
    .searchDensity              = FDSearchDensityNormal,
},

.ROIGenerator =
{
    .enable                     = TRUE,
    .baseThreshold              = 500,
    .innerThreshold             = 800,
    .expandFaceSizePercentage   = 25,
    .expandBoxBorderPercentage  = 100,
    .faceSpreadTolerance        = 0.9,
    .searchDensity              = FDSearchDensityNormal,
},

.managerConfig =
{
    .enable                         = TRUE,
    .newGoodFaceConfidence          = 865,
    .newNormalFaceConfidence        = 815,
    .existingFaceConfidence         = 300,
    .angleDiffForStrictConfidence   = 55,
    .strictNewGoodFaceConfidence    = 865,
    .strictNewNormalFaceConfidence  = 815,
    .strictExistingFaceConfidence   = 300,
    .faceLinkMoveDistanceRatio      = 1.1,
    .faceLinkMinSizeRatio           = 0.666666,
    .faceLinkMaxSizeRatio           = 1.5,
    .faceLinkRollAngleDifference    = 60.0,
},

.stabilization =
{
    .enable             = TRUE,
    .historyDepth       = 10,
    .boundaryCaseFactor = 0.7f,

    .position =
    {
        .enable                     = TRUE,
        .mode                       = WithinThreshold,
        .minStableState             = 4,
        .stableThreshold            = 6,
        .threshold                  = 170,
        .maxUnstableStateCount      = 4,
        .useReference               = FALSE,
        .filterType                 = Average,
        .movingInitStateCount       = 2,
        .movingMaxDelayCount        = 6,
        .movingMaxHoldCount         = 2,
        .defaultMotionDetector =
        {
            .movingLinkFactor       = 1.2f,
            .movingThreshold        = 25,
            .movingScalerAtBoundary = 1,
        },
        .averageFilter =
        {
            .historyLength          = 5,
            .movingHistoryLength    = 2,
        },
    },

    .size =
    {
        .enable                     = TRUE,
        .mode                       = WithinThreshold,
        .minStableState             = 4,
        .stableThreshold            = 8,
        .threshold                  = 275,
        .maxUnstableStateCount      = 4,
        .useReference               = FALSE,
        .filterType                 = Average,
        .movingInitStateCount       = 2,
        .movingMaxDelayCount        = 8,
        .movingMaxHoldCount         = 2,
        .sizeMotionDetector =
        {
            .cutOffThreshold        = 2,
        },
        .averageFilter =
        {
            .historyLength          = 5,
            .movingHistoryLength    = 3,
            .preFilterHistoryLength = 10,
        },
    },
},

.swPreprocess =
{
    .exposureShortBySafeThreshold = 0.8f,
    .deltaEVFromTargetThreshold   = -0.3f,
},

.facialAttrConfig =
{
    .PTDMaxNumberOfFaces      = 2,
    .SMDMaxNumberOfFaces      = 2,
    .GBDMaxNumberOfFaces      = 2,
    .CTDMaxNumberOfFaces      = 2,
    .bFDStabilizationOverride = FALSE,
},
