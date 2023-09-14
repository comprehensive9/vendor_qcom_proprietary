/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#if !defined(HOST_EXECUTABLE_BUILD) && !defined(QMI_RIL_UTF)
#include <Logger.h>
using namespace qti::ril::logger;

#define TAG "util"
#define UTIL_LOG_DEBUG(...) QCRIL_HAL_LOG_DEBUG(__VA_ARGS__)
#define UTIL_LOG_ERROR(...) QCRIL_HAL_LOG_HIGH(__VA_ARGS__)
#define UTIL_LOG_INFO(...) QCRIL_HAL_LOG_HIGH(__VA_ARGS__)
#define UTIL_LOG_FATAL(...) QCRIL_HAL_LOG_FATAL(__VA_ARGS__)
#else
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
size_t strlcpy(char *d, const char *s, size_t sz);
size_t strlcat(char *d, const char *s, size_t sz);
#ifdef __cplusplus
}
#endif
// TODO: log for host
#define UTIL_LOG_DEBUG(...)
#define UTIL_LOG_ERROR(...)
#define UTIL_LOG_INFO(...)
#define UTIL_LOG_FATAL(...)
#endif
