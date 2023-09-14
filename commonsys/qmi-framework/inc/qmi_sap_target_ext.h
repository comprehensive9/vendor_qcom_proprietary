#ifndef QMI_SAP_TARGET_EXT_H
#define QMI_SAP_TARGET_EXT_H
/******************************************************************************
  ---------------------------------------------------------------------------
  Copyright (c) 2011, 2013, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define QSAP_MALLOC(size)      malloc(size)
#define QSAP_CALLOC(num, size) calloc(num, size)
#define QSAP_FREE(ptr)         free(ptr)
#define QSAP_REALLOC(ptr,size) realloc(ptr, size)

#ifdef __cplusplus
}
#endif
#endif /* QMI_SAP_TARGET_EXT_H */
