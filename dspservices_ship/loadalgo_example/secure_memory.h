/**=============================================================================

@file
   secure_memory.h

@brief
   Header file for secure memory management.

Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================**/

#ifndef SECURE_MEMORY_H
#define SECURE_MEMORY_H

typedef int ion_user_handle_t;

#include "AEEStdDef.h"
#include <stdio.h>
#include <linux/dma-buf.h>
#include <ion/ion.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef LOADALGO_QMAA_ENABLED
/*
 * In QMAA mode, kernel dependencies are not allowed. So,
 * redefine ION macros from locally to maintain compilation.
 * These hardcoded values are from the 5.4 kernel, so this
 * module will not be functional on QMAA builds compiled
 * with an older kernel.
 */
#define ION_HEAP(bit)                   bit
#define ION_BIT(nr)                     (1U << (nr))
#define ION_SYSTEM_HEAP_ID              ION_BIT(25)
#define ION_SECURE_CARVEOUT_HEAP_ID     ION_BIT(15)
#define ION_FLAG_SECURE                 ION_BIT(31)
#define ION_QSECOM_HEAP_ID              ION_BIT(7)

#else
#include <linux/msm_ion.h>
#endif //LOADALGO_QMAA_ENABLED

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mem_handle.h"

#define ION_FLAG_CACHED    1
#define ION_FLAG_CP_CDSP   ION_BIT(29)

/* @brief: Error codes retruned by secure memory module */
enum securemem_error_codes {
    SECUREMEMORY_INVALIDHANDLE = 600,
    SECUREMEMORY_INVALIDSIZE,
    SECUREMEMORY_OPENFAILED,
    SECUREMEMORY_ALLOCFAILED
};

/* @brief: Free the allocated ion memory.
 * @param pIon: Memory handle that stores address, fd, size etc.
 * @retval : 0 in case of success.
 */
int secure_mem_free(mem_handle *pIon);

/* @brief: Allocate ion memory.
 * @param pIon: Memory handle that stores address, fd, size etc.
 * @param nBytes: Size to allocate.
 * @param nByteAlignment: Alignment.
 * @param secure: Allocate from secure or non-secure heap.
 *                True in case you want to allocate from secure heap.
 * @retval : 0 in case of success.
 */
int secure_mem_alloc(mem_handle *pIon, uint32_t nBytes, uint32_t nByteAlignment, boolean secure);

#endif
