/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "dppCommon.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#include <chrono>
#include <memory>
#include <regex>
#include <sys/stat.h>
#include <unistd.h>

/* Safe string copy api */
size_t strbufcpy(char *dst, const char *src, size_t bufsize) {
    size_t len = 0;

    if (!dst || !src || (int)bufsize <= 0) {
        return len;
    }

    len = std::strlen(src);
    if (len >= bufsize) {
        len = bufsize - 1;
    }
    std::memcpy(dst, src, len);
    dst[len] = '\0';

    return len;
}

bool IsValidPath(const std::string path, bool is_file) {
    struct stat stats;
    int ret = stat(path.c_str(), &stats);
    if (ret == -1) {
        return false;
    }
    mode_t mode = stats.st_mode;
    if (is_file) {
        if ((mode & R_OK) && (mode & W_OK)) {
            return true;
        }
    } else if (S_ISDIR(mode)) {
        if (access(path.c_str(), R_OK) == 0 &&
            access(path.c_str(), W_OK) == 0) {
            /* directory check */
            return true;
        }
    }
    return false;
}

/* DPP print */
#ifdef ANDROID
static enum android_LogPriority level_to_android_priority(int level) {
    switch (level) {
        case DPPDAEMON_MSG_ERROR:
                return ANDROID_LOG_ERROR;
        case DPPDAEMON_MSG_INFO:
                return ANDROID_LOG_INFO;
        case DPPDAEMON_MSG_DEBUG:
                return ANDROID_LOG_DEBUG;
        default:
                return ANDROID_LOG_VERBOSE;
    }
}
#endif

#ifndef ANDROID
static char level_to_log_char(int level) {
    switch (level) {
        case DPPDAEMON_MSG_ERROR:
                return 'E';
        case DPPDAEMON_MSG_INFO:
                return 'I';
        case DPPDAEMON_MSG_DEBUG:
                return 'D';
        default:
                return 'D';
    }
}
#endif

void dpp_daemon_print(const DppConfig *dpp_config_p, int level, const char *fmt, ...) {
    va_list ap;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    if (dpp_config_p == nullptr) {
        goto print_on_console;
    }

    if (level < dpp_config_p->dppdaemon_print_level) {
        return;
    }

#ifdef ANDROID
    va_start(ap, fmt);
    __android_log_vprint(level_to_android_priority(level),
                         "dpp_daemon", fmt, ap);
    va_end(ap);
#else /* ANDROID */
    if (dpp_config_p->log_file_handle) {
        va_start(ap, fmt);
        fprintf(dpp_config_p->log_file_handle, "%ld.%06u: (%c) ",
                (long)tv.tv_sec, (unsigned int)tv.tv_usec,
                level_to_log_char(level));
        vfprintf(dpp_config_p->log_file_handle, fmt, ap);
        fprintf(dpp_config_p->log_file_handle, "\n");
        va_end(ap);
    } else {
        goto print_on_console;
    }
#endif /* ANDROID */
print_on_console:
     va_start(ap, fmt);
#ifdef ANDROID
    __android_log_vprint(level_to_android_priority(level),
                         "dpp_daemon", fmt, ap);
#else /* ANDROID */
     printf("%ld.%06u: (%c) ",
            (long) tv.tv_sec, (unsigned int) tv.tv_usec,
            level_to_log_char(level));
     vprintf(fmt, ap);
     printf("\n");
     va_end(ap);
#endif /* ANDROID */
}
