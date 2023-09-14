/******************************************************************************
  @file    log/log.h
  @brief   RIL UTF logging declarations

  DESCRIPTION


  ---------------------------------------------------------------------------
  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Not a contribution.

  Copyright (C) 2009 The Android Open Source Project

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  ---------------------------------------------------------------------------
******************************************************************************/

/* ------------------------------
   Dummy system/core/include/log/log.h file for UTF
   ------------------------------
*/

#ifndef _RIL_UTF_LOG_LOG_H
#define _RIL_UTF_LOG_LOG_H

#include <string.h>
#include <android/log.h>

#define ALOG                RIL_UTF_DEBUG_IGNORE
#define ALOGD               RIL_UTF_DEBUG_IGNORE
#define ALOGW               RIL_UTF_DEBUG_IGNORE
#define ALOG_ASSERT         RIL_UTF_DEBUG_IGNORE
#define LOG_ALWAYS_FATAL    RIL_UTF_DEBUG_IGNORE
#define LOG_ALWAYS_FATAL_IF RIL_UTF_DEBUG_IGNORE
#define ALOGW_IF            RIL_UTF_DEBUG_IGNORE
#define android_errorWriteLog(...) do{}while(0)

#define LOG_WARN ANDROID_LOG_WARN

#define RIL_UTF_DEBUG_IGNORE(...) do{}while(0)

#define RLOGI(...) do{}while(0)
#define RLOGE(...) do{}while(0)
#define RLOGW(...) do{}while(0)
#define RLOGD(...) do{}while(0)
#define RLOGV(...) do{}while(0)

#if 0
/*
 * Android log priority values, in ascending priority order.
 */
typedef enum android_LogPriority {
  ANDROID_LOG_UNKNOWN = 0,
  ANDROID_LOG_DEFAULT, /* only for SetMinPriority() */
  ANDROID_LOG_VERBOSE,
  ANDROID_LOG_DEBUG,
  ANDROID_LOG_INFO,
  ANDROID_LOG_WARN,
  ANDROID_LOG_ERROR,
  ANDROID_LOG_FATAL,
  ANDROID_LOG_SILENT, /* only for SetMinPriority(); must be last */
} android_LogPriority;
#endif

#endif
