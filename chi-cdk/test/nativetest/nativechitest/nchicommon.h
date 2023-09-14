//*************************************************************************************************
// Copyright (c) 2016 - 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//*************************************************************************************************

#ifndef __NCHI_COMMON__
#define __NCHI_COMMON__

#include "camera3metadata.h"
#include "chi.h"
#include "camxdefs.h"

// Determine the compiler's environment
#if ((ULONG_MAX) == (UINT_MAX))
#define ENVIRONMENT32
#else
#define ENVIRONMENT64
#endif

namespace chitests {

    static const GrallocUsage GrallocUsageTP10 = 0x08000000;   ///< UBWC TP10 private usage flag.
    static const GrallocUsage GrallocUsageP010 = 0x40000000;   ///< UBWC P010 private usage flag.
    static const int METADATA_ENTRIES          = 200;
    const int MaximumPipelinesPerSession       = CAMXDEFS_H::MaxPipelinesPerSession;
    /// @brief Create session data
    struct SessionCreateData
    {
        CHIPIPELINEINFO     pPipelineInfos[MaximumPipelinesPerSession]; ///< Chi Pipeline infos
        uint32_t            numPipelines;                               ///< Number of pipelines in this session
        CHICALLBACKS*       pCallbacks;                                 ///< Callbacks
        void*               pPrivateCallbackData;                       ///< Private callbacks
    };

    /// @brief Pipeline create data
    struct PipelineCreateData
    {
        const CHAR*                   pPipelineName;                     ///< Pipeline name
        CHIPIPELINECREATEDESCRIPTOR*  pPipelineCreateDescriptor;         ///< Pipeline create descriptor
        uint32_t                      numOutputs;                        ///< Number of output buffers of this pipeline
        CHIPORTBUFFERDESCRIPTOR*      pOutputDescriptors;                ///< Output buffer descriptors
        uint32_t                      numInputs;                         ///< Number of inputs
        CHIPORTBUFFERDESCRIPTOR*      pInputDescriptors;                 ///< Input buffer descriptors
        CHIPIPELINEINPUTOPTIONS*      pInputBufferRequirements;          ///< Input buffer requirements for this pipeline
    };

    enum PipelineIndex
    {
        Realtime = 0,
        Offline,
        MaxPipelines
    };

}  // namespace chitests

#endif  // __NCHI_COMMON__
