/******************************************************************************
 * #  Copyright (c) 2017, 2019 Qualcomm Technologies, Inc.
 * #  All Rights Reserved.
 * #  Confidential and Proprietary - Qualcomm Technologies, Inc.
 * #******************************************************************************/
/******************************************************************************
 *   @file    qcril_memory_management.h
 *   @brief   qcril qmi - memory management
 *
 *   DESCRIPTION
 *     Provides generic memory allocation and the correct typecasting.
 *     Determines the type from the destination address passed as a
 *     reference parameter.
 *
 *******************************************************************************/

#ifndef QCRIL_MEMORY_MANAGEMENT
#define QCRIL_MEMORY_MANAGEMENT

/*===========================================================================
 *
 *                            INCLUDE FILES
 *
 *===========================================================================*/
#include <stddef.h>

#ifndef QMI_RIL_UTF
#define qcril_malloc(size) qcril_malloc_adv(size, __func__, __LINE__)
#else
#define qcril_malloc(size) utf_qcril_malloc_adv(size, __func__, __LINE__)
#ifdef __cplusplus
extern "C" {
#endif
void *utf_qcril_malloc_adv( size_t size, const char* func_name, int line_num );
#ifdef __cplusplus
}
#endif
#endif

#ifndef QMI_RIL_UTF
#define qcril_free(mem_ptr) qcril_free_adv(mem_ptr, __func__, __LINE__)
#else
#define qcril_free(mem_ptr) utf_qcril_free_adv(mem_ptr, __func__, __LINE__)
#ifdef __cplusplus
extern "C" {
#endif
void utf_qcril_free_adv( void *mem_ptr, const char* func_name, int line_num );
#ifdef __cplusplus
}
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
void *qcril_malloc_adv( size_t size, const char* func_name, int line_num );
void qcril_free_adv( void *mem_ptr, const char* func_name, int line_num );
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
//===========================================================================
// templated malloc
//===========================================================================
template<typename T>
inline T* qcril_malloc2(T*& adr, size_t n = 1) {
    adr = static_cast<T*>(qcril_malloc(sizeof(T) * n));
    return adr;
}
#endif

#endif //QCRIL_MEMORY_MANAGEMENT
