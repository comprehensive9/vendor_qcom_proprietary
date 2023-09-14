/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 */

#ifndef SOURCES_LOG_H
#define SOURCES_LOG_H

#include<syslog.h>

#define RIL_LOG_ERROR         1
#define RIL_LOG_WARNING       2
#define RIL_LOG_DEBUG         3
#define RIL_LOG_VERBOSE       4
#define RIL_LOG_INFO          5

#define RLOGE(fmt, args...) \
    do_syslog_new(LOG_ERR, RIL_LOG_ERROR, "[DJ:E][%s:%d] " fmt, __func__, __LINE__, ## args)
#define RLOGI(fmt, args...) \
    do_syslog_new(LOG_ERR, RIL_LOG_INFO, "[DJ:I][%s:%d] " fmt, __func__, __LINE__, ## args)
#define RLOGW(fmt, args...) \
    do_syslog_new(LOG_ERR, RIL_LOG_WARNING, "[DJ:W][%s:%d] " fmt, __func__, __LINE__, ## args)
#define RLOGD(fmt, args...) \
    do_syslog_new(LOG_ERR, RIL_LOG_DEBUG, "[DJ:D][%s:%d] " fmt, __func__, __LINE__, ## args)
#define RLOGW_IF(...) do {do_syslog_new(LOG_ERR, RIL_LOG_WARNING, "RLOGW_IF: %s %s", __func__, __VA_ARGS__ );} while(0)
#define RLOGI_IF(...) do {do_syslog_new(LOG_ERR, RIL_LOG_INFO, "RLOGI_IF: %s %s", __func__, __VA_ARGS__ );} while(0)
#define RLOGV_IF(...) do {do_syslog_new(LOG_ERR, RIL_LOG_VERBOSE, "RLOGV_IF: %s %s", __func__, __VA_ARGS__ );} while(0)
#define ALOGE(...) do {do_syslog_new(LOG_ERR, RIL_LOG_ERROR, "ALOGE: %s %s", __func__, __VA_ARGS__ );} while(0)
#define RLOGV(...) do {do_syslog_new(LOG_ERR, RIL_LOG_VERBOSE, "RLOGV: %s %s", __func__, __VA_ARGS__ );} while(0)

static inline void do_syslog_new(int level, int ril_level, const char *fmt, ...)
{
    // LOG only ERROR by default into sys log
    if ( ril_level == RIL_LOG_ERROR) {
        va_list args;

        va_start(args, fmt);
        vsyslog(level, fmt, args);
        va_end(args);
    }
}

#endif //SOURCES_LOG_H
