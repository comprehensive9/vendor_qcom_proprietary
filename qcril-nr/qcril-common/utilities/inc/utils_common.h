/******************************************************************************
#  Copyright (c) 2013, 2017, 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/***************************************************************************************************
    @file
    utils_common.h

    @brief
    Exports standard and custom utilities.

***************************************************************************************************/

#ifndef UTILS_COMMON
#define UTILS_COMMON

#include <stdint.h>

#define ESUCCESS     0
#define NIL          0
#define FALSE        0
#define TRUE         1
#define UTIL_ARR_SIZE( arr ) ( sizeof( ( arr ) ) / sizeof( ( arr[ 0 ] ) ) )

#define IS_BIG_ENDIAN()     \
({ \
  union {uint32_t d32; uint8_t d8;} data = {.d32 = 1}; \
  !data.d8; \
})

#define UTILS_FAILURE 1
#define UTILS_SUCCESS 0

#include "qcril_memory_management.h" 

#endif

