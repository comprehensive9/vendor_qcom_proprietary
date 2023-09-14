/********************************************************************
/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
/*
Not a contribution.

 *  Copyright (C) 2012 The Android Open Source Project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you
 *  may not use this file except in compliance with the License.  You may
 *  obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *  implied.  See the License for the specific language governing
 *  permissions and limitations under the License.
----------------------------------------------------------------------*/
/*
Sample SP license code.
*********************************************************************/
/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
6/10/20    sn      Added sanity checks and FID,License value changes
11/28/19   yb      Initial version
===========================================================================*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <linux/msm_ion.h>
#include <linux/dma-buf.h>

#if TARGET_ION_ABI_VERSION >= 2
#include <ion/ion.h>
#endif
#include <utils/Log.h>
#include "common_log.h"
#include "QSEEComAPI.h"
#include <sys/mman.h>
#include "comdef.h"


/** adb log */
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "SP_LICENSE_SAMPLE: "
#ifdef LOG_NDDEBUG
#undef LOG_NDDEBUG
#endif
#define LOG_NDDEBUG 0 //Define to enable LOGD
#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#define LOG_NDEBUG  0 //Define to enable LOGV



#define SPU_LICENSE_INSTALL_REQUEST                          1

#define ION_BUFFER_LENGTH (4096)
/* Error code: status sent as response to command from app client*/

#define SPU_LICENSE_SUCCESS                                 0
#define SPU_LICENSE_GENERAL_FAILED                         -2
#define SPU_LICENSE_INVALID_LICENSE_FAILED                 -3
#define SPU_LICENSE_INVALID_PARAM_FAILED                   -4
#define SPU_LICENSE_UNSUPPORTED_CMD_FAILED                 -5

#define SPU_LICENSE_APP_VERSION_MAJOR 0x1
#define SPU_LICENSE_APP_VERSION_MINOR 0x0

#pragma pack (push, 1)

 typedef struct sp_install_license_cmd{
        uint32_t cmd_id;
        uint32_t feature_id;
        uint64_t license_addr;
        uint32_t license_len;
  } sp_install_license_cmd_t;


typedef struct sp_install_license_rsp{
    int32_t status;
 } sp_install_license_rsp_t;

 struct qsc_ion_info {
     int32_t ion_fd;
     int32_t ifd_data_fd;
 #ifndef TARGET_ION_ABI_VERSION
     struct ion_handle_data ion_alloc_handle;
 #endif
     unsigned char * ion_sbuffer;
     uint32_t sbuf_len;
 };

 struct qsc_send_cmd_rsp {
   uint32_t data;
   int32_t status;
 };

#pragma pack (pop)

/* This is a production License mapped to reserved FID 1396
   and set to expire on 2020-12-30
*/

 const uint8_t havent_testlicense[] = {
  0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x42, 0x45, 0x47, 0x49, 0x4E, 0x20, 0x43, 0x45, 0x52, 0x54, 0x49, 0x46, 0x49, 0x43, 0x41, 0x54, 0x45, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x4D, 0x49, 0x49, 0x45, 0x66, 0x7A, 0x43, 0x43, 0x41, 0x7A, 0x65, 0x67, 0x41, 0x77, 0x49, 0x42, 0x41, 0x67, 0x49, 0x55, 0x42, 0x65, 0x78, 0x71, 0x55, 0x66, 0x78, 0x50, 0x4A, 0x48, 0x43, 0x57, 0x6C, 0x53, 0x64, 0x51, 0x41, 0x41, 0x41, 0x42, 0x63, 0x70, 0x41, 0x72, 0x45, 0x75, 0x63, 0x77, 0x50, 0x51, 0x59, 0x4A, 0x4B, 0x6F, 0x5A, 0x49, 0x68, 0x76, 0x63, 0x4E, 0x41, 0x51, 0x45, 0x4B, 0x4D, 0x44, 0x43, 0x67, 0x44, 0x54, 0x41, 0x4C, 0x42, 0x67, 0x6C, 0x67, 0x68, 0x6B, 0x67, 0x42, 0x5A, 0x51, 0x4D, 0x45, 0x41, 0x67, 0x47, 0x68, 0x47, 0x6A, 0x41, 0x59, 0x42, 0x67, 0x6B, 0x71, 0x68, 0x6B, 0x69, 0x47, 0x39, 0x77, 0x30, 0x42, 0x41, 0x51, 0x67, 0x77, 0x43, 0x77, 0x59, 0x4A, 0x59, 0x49, 0x5A, 0x49, 0x41, 0x57, 0x55, 0x44, 0x42, 0x41, 0x49, 0x42, 0x6F, 0x67, 0x4D, 0x43, 0x41, 0x53, 0x41, 0x77, 0x62, 0x7A, 0x45, 0x6B, 0x4D, 0x43, 0x49, 0x47, 0x41, 0x31, 0x55, 0x45, 0x43, 0x67, 0x77, 0x62, 0x55, 0x58, 0x56, 0x68, 0x62, 0x47, 0x4E, 0x76, 0x62, 0x57, 0x30, 0x67, 0x56, 0x47, 0x56, 0x6A, 0x61, 0x47, 0x35, 0x76, 0x62, 0x47, 0x39, 0x6E, 0x61, 0x57, 0x56, 0x7A, 0x4C, 0x43, 0x42, 0x4A, 0x62, 0x6D, 0x4D, 0x75, 0x4D, 0x53, 0x6F, 0x77, 0x4B, 0x41, 0x59, 0x44, 0x56, 0x51, 0x51, 0x4C, 0x44, 0x43, 0x46, 0x52, 0x64, 0x57, 0x46, 0x73, 0x59, 0x32, 0x39, 0x74, 0x62, 0x53, 0x42, 0x44, 0x63, 0x6E, 0x6C, 0x77, 0x64, 0x47, 0x39, 0x6E, 0x63, 0x6D, 0x46, 0x77, 0x61, 0x47, 0x6C, 0x6A, 0x49, 0x45, 0x39, 0x77, 0x5A, 0x58, 0x4A, 0x68, 0x64, 0x47, 0x6C, 0x76, 0x62, 0x6E, 0x4D, 0x78, 0x47, 0x7A, 0x41, 0x5A, 0x42, 0x67, 0x4E, 0x56, 0x42, 0x41, 0x4D, 0x4D, 0x45, 0x6B, 0x4E, 0x49, 0x54, 0x46, 0x4E, 0x46, 0x49, 0x46, 0x4E, 0x70, 0x5A, 0x32, 0x35, 0x70, 0x62, 0x6D, 0x63, 0x67, 0x51, 0x32, 0x56, 0x79, 0x64, 0x44, 0x41, 0x65, 0x46, 0x77, 0x30, 0x33, 0x4D, 0x44, 0x41, 0x78, 0x4D, 0x44, 0x45, 0x77, 0x4D, 0x44, 0x41, 0x77, 0x4D, 0x44, 0x46, 0x61, 0x46, 0x77, 0x30, 0x79, 0x4D, 0x44, 0x45, 0x79, 0x4D, 0x7A, 0x41, 0x79, 0x4D, 0x7A, 0x55, 0x35, 0x4E, 0x54, 0x6C, 0x61, 0x4D, 0x48, 0x38, 0x78, 0x4B, 0x7A, 0x41, 0x70, 0x42, 0x67, 0x4E, 0x56, 0x42, 0x41, 0x4D, 0x4D, 0x49, 0x6C, 0x4E, 0x51, 0x56, 0x53, 0x42, 0x42, 0x63, 0x48, 0x42, 0x73, 0x61, 0x57, 0x4E, 0x68, 0x64, 0x47, 0x6C, 0x76, 0x62, 0x69, 0x42, 0x46, 0x64, 0x6D, 0x46, 0x73, 0x64, 0x57, 0x46, 0x30, 0x61, 0x57, 0x39, 0x75, 0x49, 0x45, 0x78, 0x70, 0x59, 0x32, 0x56, 0x75, 0x63, 0x32, 0x55, 0x78, 0x4A, 0x44, 0x41, 0x69, 0x42, 0x67, 0x4E, 0x56, 0x42, 0x41, 0x6F, 0x4D, 0x47, 0x31, 0x46, 0x31, 0x59, 0x57, 0x78, 0x6A, 0x62, 0x32, 0x31, 0x74, 0x49, 0x46, 0x52, 0x6C, 0x59, 0x32, 0x68, 0x75, 0x62, 0x32, 0x78, 0x76, 0x5A, 0x32, 0x6C, 0x6C, 0x63, 0x79, 0x77, 0x67, 0x53, 0x57, 0x35, 0x6A, 0x4C, 0x6A, 0x45, 0x71, 0x4D, 0x43, 0x67, 0x47, 0x41, 0x31, 0x55, 0x45, 0x43, 0x77, 0x77, 0x68, 0x55, 0x58, 0x56, 0x68, 0x62, 0x47, 0x4E, 0x76, 0x62, 0x57, 0x30, 0x67, 0x51, 0x33, 0x4A, 0x35, 0x63, 0x48, 0x52, 0x76, 0x5A, 0x33, 0x4A, 0x68, 0x63, 0x47, 0x68, 0x70, 0x59, 0x79, 0x42, 0x50, 0x63, 0x47, 0x56, 0x79, 0x59, 0x58, 0x52, 0x70, 0x62, 0x32, 0x35, 0x7A, 0x4D, 0x49, 0x49, 0x42, 0x49, 0x44, 0x41, 0x4C, 0x42, 0x67, 0x6B, 0x71, 0x68, 0x6B, 0x69, 0x47, 0x39, 0x77, 0x30, 0x42, 0x41, 0x51, 0x45, 0x44, 0x67, 0x67, 0x45, 0x50, 0x41, 0x44, 0x43, 0x43, 0x41, 0x51, 0x6F, 0x43, 0x67, 0x67, 0x45, 0x42, 0x41, 0x50, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x38, 0x43, 0x41, 0x77, 0x45, 0x41, 0x41, 0x61, 0x4F, 0x42, 0x70, 0x44, 0x43, 0x42, 0x6F, 0x54, 0x41, 0x66, 0x42, 0x67, 0x4E, 0x56, 0x48, 0x53, 0x4D, 0x45, 0x47, 0x44, 0x41, 0x57, 0x67, 0x42, 0x53, 0x48, 0x46, 0x4F, 0x6B, 0x33, 0x4D, 0x6E, 0x36, 0x39, 0x74, 0x69, 0x6A, 0x38, 0x4D, 0x47, 0x70, 0x67, 0x4A, 0x42, 0x41, 0x77, 0x32, 0x4C, 0x33, 0x6E, 0x62, 0x6A, 0x41, 0x4D, 0x42, 0x67, 0x4E, 0x56, 0x48, 0x52, 0x4D, 0x42, 0x41, 0x66, 0x38, 0x45, 0x41, 0x6A, 0x41, 0x41, 0x4D, 0x41, 0x34, 0x47, 0x41, 0x31, 0x55, 0x64, 0x44, 0x77, 0x45, 0x42, 0x2F, 0x77, 0x51, 0x45, 0x41, 0x77, 0x49, 0x46, 0x6F, 0x44, 0x41, 0x56, 0x42, 0x67, 0x6B, 0x72, 0x42, 0x67, 0x45, 0x45, 0x41, 0x59, 0x73, 0x70, 0x44, 0x41, 0x45, 0x45, 0x43, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x56, 0x30, 0x4D, 0x42, 0x59, 0x47, 0x43, 0x69, 0x73, 0x47, 0x41, 0x51, 0x51, 0x42, 0x69, 0x79, 0x6B, 0x4C, 0x41, 0x51, 0x45, 0x45, 0x43, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x42, 0x4D, 0x42, 0x59, 0x47, 0x43, 0x69, 0x73, 0x47, 0x41, 0x51, 0x51, 0x42, 0x69, 0x79, 0x6B, 0x4C, 0x41, 0x51, 0x49, 0x45, 0x43, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x47, 0x41, 0x50, 0x4D, 0x42, 0x6B, 0x47, 0x43, 0x53, 0x73, 0x47, 0x41, 0x51, 0x51, 0x42, 0x69, 0x79, 0x6B, 0x4D, 0x41, 0x67, 0x51, 0x4D, 0x41, 0x41, 0x41, 0x41, 0x41, 0x51, 0x41, 0x41, 0x41, 0x41, 0x46, 0x66, 0x37, 0x52, 0x52, 0x2F, 0x4D, 0x44, 0x30, 0x47, 0x43, 0x53, 0x71, 0x47, 0x53, 0x49, 0x62, 0x33, 0x44, 0x51, 0x45, 0x42, 0x43, 0x6A, 0x41, 0x77, 0x6F, 0x41, 0x30, 0x77, 0x43, 0x77, 0x59, 0x4A, 0x59, 0x49, 0x5A, 0x49, 0x41, 0x57, 0x55, 0x44, 0x42, 0x41, 0x49, 0x42, 0x6F, 0x52, 0x6F, 0x77, 0x47, 0x41, 0x59, 0x4A, 0x4B, 0x6F, 0x5A, 0x49, 0x68, 0x76, 0x63, 0x4E, 0x41, 0x51, 0x45, 0x49, 0x4D, 0x41, 0x73, 0x47, 0x43, 0x57, 0x43, 0x47, 0x53, 0x41, 0x46, 0x6C, 0x41, 0x77, 0x51, 0x43, 0x41, 0x61, 0x49, 0x44, 0x41, 0x67, 0x45, 0x67, 0x41, 0x34, 0x49, 0x42, 0x41, 0x51, 0x43, 0x2B, 0x4E, 0x71, 0x71, 0x4B, 0x4E, 0x78, 0x50, 0x32, 0x72, 0x6A, 0x34, 0x36, 0x51, 0x4D, 0x42, 0x78, 0x78, 0x52, 0x44, 0x62, 0x57, 0x6B, 0x76, 0x35, 0x72, 0x50, 0x6B, 0x73, 0x46, 0x33, 0x78, 0x62, 0x4B, 0x4B, 0x50, 0x62, 0x4E, 0x6A, 0x69, 0x62, 0x6A, 0x76, 0x70, 0x6C, 0x49, 0x6C, 0x63, 0x45, 0x46, 0x33, 0x32, 0x70, 0x50, 0x70, 0x74, 0x6B, 0x39, 0x64, 0x6D, 0x36, 0x49, 0x77, 0x4F, 0x58, 0x61, 0x45, 0x6A, 0x42, 0x6C, 0x76, 0x43, 0x79, 0x78, 0x45, 0x79, 0x4C, 0x6E, 0x57, 0x45, 0x54, 0x4D, 0x6A, 0x36, 0x67, 0x30, 0x56, 0x6B, 0x59, 0x36, 0x51, 0x7A, 0x76, 0x55, 0x36, 0x47, 0x30, 0x5A, 0x36, 0x53, 0x61, 0x50, 0x73, 0x4D, 0x47, 0x59, 0x42, 0x52, 0x54, 0x4D, 0x73, 0x35, 0x50, 0x58, 0x4E, 0x38, 0x51, 0x4E, 0x31, 0x70, 0x7A, 0x76, 0x5A, 0x41, 0x78, 0x36, 0x43, 0x63, 0x31, 0x78, 0x49, 0x6A, 0x46, 0x6C, 0x58, 0x71, 0x63, 0x6E, 0x57, 0x55, 0x69, 0x44, 0x42, 0x38, 0x63, 0x34, 0x37, 0x39, 0x59, 0x5A, 0x56, 0x65, 0x67, 0x55, 0x37, 0x6E, 0x65, 0x58, 0x34, 0x41, 0x65, 0x38, 0x70, 0x2B, 0x4C, 0x39, 0x59, 0x47, 0x63, 0x38, 0x61, 0x48, 0x65, 0x43, 0x61, 0x32, 0x44, 0x47, 0x4A, 0x4F, 0x64, 0x52, 0x7A, 0x6A, 0x5A, 0x57, 0x2B, 0x45, 0x64, 0x45, 0x4B, 0x37, 0x44, 0x79, 0x44, 0x46, 0x4D, 0x32, 0x71, 0x59, 0x49, 0x56, 0x7A, 0x33, 0x58, 0x6F, 0x75, 0x37, 0x56, 0x63, 0x57, 0x43, 0x61, 0x61, 0x4F, 0x69, 0x5A, 0x61, 0x39, 0x36, 0x41, 0x4F, 0x2F, 0x47, 0x31, 0x58, 0x65, 0x6E, 0x79, 0x6F, 0x67, 0x49, 0x7A, 0x34, 0x6E, 0x39, 0x39, 0x52, 0x33, 0x6E, 0x64, 0x63, 0x2B, 0x72, 0x35, 0x44, 0x4C, 0x35, 0x71, 0x4B, 0x70, 0x6D, 0x72, 0x48, 0x64, 0x41, 0x5A, 0x62, 0x55, 0x59, 0x71, 0x79, 0x6D, 0x35, 0x76, 0x33, 0x67, 0x64, 0x6E, 0x58, 0x6C, 0x4E, 0x79, 0x2B, 0x76, 0x67, 0x75, 0x4C, 0x56, 0x35, 0x4B, 0x36, 0x37, 0x4B, 0x65, 0x71, 0x74, 0x49, 0x34, 0x6D, 0x74, 0x6C, 0x5A, 0x57, 0x30, 0x79, 0x34, 0x6A, 0x38, 0x4A, 0x49, 0x51, 0x45, 0x43, 0x57, 0x4C, 0x30, 0x39, 0x51, 0x6F, 0x4E, 0x6E, 0x71, 0x77, 0x34, 0x7A, 0x4A, 0x6B, 0x51, 0x30, 0x59, 0x38, 0x32, 0x34, 0x2B, 0x47, 0x61, 0x47, 0x56, 0x59, 0x64, 0x73, 0x6C, 0x44, 0x2F, 0x44, 0x34, 0x58, 0x74, 0x77, 0x6A, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x45, 0x4E, 0x44, 0x20, 0x43, 0x45, 0x52, 0x54, 0x49, 0x46, 0x49, 0x43, 0x41, 0x54, 0x45, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x42, 0x45, 0x47, 0x49, 0x4E, 0x20, 0x43, 0x45, 0x52, 0x54, 0x49, 0x46, 0x49, 0x43, 0x41, 0x54, 0x45, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x4D, 0x49, 0x49, 0x45, 0x52, 0x54, 0x43, 0x43, 0x41, 0x76, 0x32, 0x67, 0x41, 0x77, 0x49, 0x42, 0x41, 0x67, 0x49, 0x56, 0x64, 0x71, 0x4D, 0x45, 0x51, 0x31, 0x6A, 0x34, 0x31, 0x70, 0x31, 0x38, 0x44, 0x33, 0x41, 0x66, 0x7A, 0x43, 0x31, 0x54, 0x44, 0x78, 0x52, 0x44, 0x67, 0x79, 0x4B, 0x7A, 0x4D, 0x44, 0x30, 0x47, 0x43, 0x53, 0x71, 0x47, 0x53, 0x49, 0x62, 0x33, 0x44, 0x51, 0x45, 0x42, 0x43, 0x6A, 0x41, 0x77, 0x6F, 0x41, 0x30, 0x77, 0x43, 0x77, 0x59, 0x4A, 0x59, 0x49, 0x5A, 0x49, 0x41, 0x57, 0x55, 0x44, 0x42, 0x41, 0x49, 0x42, 0x6F, 0x52, 0x6F, 0x77, 0x47, 0x41, 0x59, 0x4A, 0x4B, 0x6F, 0x5A, 0x49, 0x68, 0x76, 0x63, 0x4E, 0x41, 0x51, 0x45, 0x49, 0x4D, 0x41, 0x73, 0x47, 0x43, 0x57, 0x43, 0x47, 0x53, 0x41, 0x46, 0x6C, 0x41, 0x77, 0x51, 0x43, 0x41, 0x61, 0x49, 0x44, 0x41, 0x67, 0x45, 0x67, 0x4D, 0x49, 0x47, 0x41, 0x4D, 0x53, 0x51, 0x77, 0x49, 0x67, 0x59, 0x44, 0x56, 0x51, 0x51, 0x4B, 0x44, 0x42, 0x74, 0x52, 0x64, 0x57, 0x46, 0x73, 0x59, 0x32, 0x39, 0x74, 0x62, 0x53, 0x42, 0x55, 0x5A, 0x57, 0x4E, 0x6F, 0x62, 0x6D, 0x39, 0x73, 0x62, 0x32, 0x64, 0x70, 0x5A, 0x58, 0x4D, 0x73, 0x49, 0x45, 0x6C, 0x75, 0x59, 0x79, 0x34, 0x78, 0x4B, 0x6A, 0x41, 0x6F, 0x42, 0x67, 0x4E, 0x56, 0x42, 0x41, 0x73, 0x4D, 0x49, 0x56, 0x46, 0x31, 0x59, 0x57, 0x78, 0x6A, 0x62, 0x32, 0x31, 0x74, 0x49, 0x45, 0x4E, 0x79, 0x65, 0x58, 0x42, 0x30, 0x62, 0x32, 0x64, 0x79, 0x59, 0x58, 0x42, 0x6F, 0x61, 0x57, 0x4D, 0x67, 0x54, 0x33, 0x42, 0x6C, 0x63, 0x6D, 0x46, 0x30, 0x61, 0x57, 0x39, 0x75, 0x63, 0x7A, 0x45, 0x73, 0x4D, 0x43, 0x6F, 0x47, 0x41, 0x31, 0x55, 0x45, 0x41, 0x77, 0x77, 0x6A, 0x52, 0x6D, 0x56, 0x68, 0x64, 0x48, 0x56, 0x79, 0x5A, 0x53, 0x42, 0x4D, 0x61, 0x57, 0x4E, 0x6C, 0x62, 0x6E, 0x4E, 0x6C, 0x49, 0x45, 0x46, 0x30, 0x64, 0x47, 0x56, 0x7A, 0x64, 0x47, 0x46, 0x30, 0x61, 0x57, 0x39, 0x75, 0x49, 0x46, 0x4A, 0x76, 0x62, 0x33, 0x51, 0x67, 0x51, 0x30, 0x45, 0x77, 0x48, 0x68, 0x63, 0x4E, 0x4D, 0x54, 0x67, 0x78, 0x4D, 0x6A, 0x45, 0x30, 0x4D, 0x44, 0x41, 0x77, 0x4D, 0x44, 0x41, 0x78, 0x57, 0x68, 0x63, 0x4E, 0x4D, 0x6A, 0x67, 0x78, 0x4D, 0x6A, 0x45, 0x7A, 0x4D, 0x6A, 0x4D, 0x31, 0x4F, 0x54, 0x55, 0x35, 0x57, 0x6A, 0x42, 0x76, 0x4D, 0x53, 0x51, 0x77, 0x49, 0x67, 0x59, 0x44, 0x56, 0x51, 0x51, 0x4B, 0x44, 0x42, 0x74, 0x52, 0x64, 0x57, 0x46, 0x73, 0x59, 0x32, 0x39, 0x74, 0x62, 0x53, 0x42, 0x55, 0x5A, 0x57, 0x4E, 0x6F, 0x62, 0x6D, 0x39, 0x73, 0x62, 0x32, 0x64, 0x70, 0x5A, 0x58, 0x4D, 0x73, 0x49, 0x45, 0x6C, 0x75, 0x59, 0x79, 0x34, 0x78, 0x4B, 0x6A, 0x41, 0x6F, 0x42, 0x67, 0x4E, 0x56, 0x42, 0x41, 0x73, 0x4D, 0x49, 0x56, 0x46, 0x31, 0x59, 0x57, 0x78, 0x6A, 0x62, 0x32, 0x31, 0x74, 0x49, 0x45, 0x4E, 0x79, 0x65, 0x58, 0x42, 0x30, 0x62, 0x32, 0x64, 0x79, 0x59, 0x58, 0x42, 0x6F, 0x61, 0x57, 0x4D, 0x67, 0x54, 0x33, 0x42, 0x6C, 0x63, 0x6D, 0x46, 0x30, 0x61, 0x57, 0x39, 0x75, 0x63, 0x7A, 0x45, 0x62, 0x4D, 0x42, 0x6B, 0x47, 0x41, 0x31, 0x55, 0x45, 0x41, 0x77, 0x77, 0x53, 0x51, 0x30, 0x68, 0x4D, 0x55, 0x30, 0x55, 0x67, 0x55, 0x32, 0x6C, 0x6E, 0x62, 0x6D, 0x6C, 0x75, 0x5A, 0x79, 0x42, 0x44, 0x5A, 0x58, 0x4A, 0x30, 0x4D, 0x49, 0x49, 0x42, 0x49, 0x6A, 0x41, 0x4E, 0x42, 0x67, 0x6B, 0x71, 0x68, 0x6B, 0x69, 0x47, 0x39, 0x77, 0x30, 0x42, 0x41, 0x51, 0x45, 0x46, 0x41, 0x41, 0x4F, 0x43, 0x41, 0x51, 0x38, 0x41, 0x4D, 0x49, 0x49, 0x42, 0x43, 0x67, 0x4B, 0x43, 0x41, 0x51, 0x45, 0x41, 0x37, 0x54, 0x79, 0x49, 0x2F, 0x59, 0x4A, 0x32, 0x4D, 0x66, 0x69, 0x70, 0x79, 0x65, 0x32, 0x39, 0x74, 0x63, 0x45, 0x57, 0x2F, 0x6A, 0x74, 0x57, 0x53, 0x2F, 0x36, 0x73, 0x43, 0x67, 0x4B, 0x4B, 0x6F, 0x67, 0x76, 0x6B, 0x6D, 0x65, 0x48, 0x42, 0x4D, 0x30, 0x37, 0x68, 0x33, 0x43, 0x77, 0x59, 0x44, 0x46, 0x42, 0x4F, 0x42, 0x67, 0x2B, 0x4D, 0x71, 0x67, 0x71, 0x7A, 0x4E, 0x55, 0x70, 0x79, 0x67, 0x6A, 0x33, 0x77, 0x4A, 0x35, 0x61, 0x68, 0x4C, 0x6B, 0x47, 0x2F, 0x48, 0x6D, 0x78, 0x4E, 0x67, 0x33, 0x43, 0x53, 0x58, 0x65, 0x49, 0x56, 0x6C, 0x54, 0x74, 0x2B, 0x63, 0x45, 0x5A, 0x68, 0x69, 0x65, 0x30, 0x54, 0x57, 0x2B, 0x73, 0x2F, 0x72, 0x78, 0x69, 0x34, 0x4F, 0x44, 0x4C, 0x51, 0x39, 0x76, 0x2B, 0x55, 0x41, 0x6F, 0x44, 0x73, 0x7A, 0x42, 0x30, 0x62, 0x6B, 0x38, 0x67, 0x4F, 0x64, 0x38, 0x48, 0x47, 0x73, 0x36, 0x43, 0x74, 0x4B, 0x58, 0x43, 0x4F, 0x4E, 0x44, 0x78, 0x2F, 0x57, 0x33, 0x50, 0x53, 0x47, 0x6E, 0x2F, 0x2B, 0x68, 0x54, 0x57, 0x70, 0x71, 0x45, 0x32, 0x47, 0x50, 0x73, 0x48, 0x34, 0x45, 0x71, 0x6A, 0x49, 0x32, 0x75, 0x65, 0x67, 0x79, 0x77, 0x34, 0x78, 0x4D, 0x78, 0x4F, 0x37, 0x38, 0x69, 0x43, 0x39, 0x61, 0x64, 0x6E, 0x37, 0x66, 0x4F, 0x47, 0x5A, 0x46, 0x65, 0x63, 0x31, 0x48, 0x61, 0x4C, 0x34, 0x4B, 0x6D, 0x72, 0x57, 0x65, 0x2B, 0x56, 0x71, 0x67, 0x31, 0x4B, 0x4C, 0x49, 0x59, 0x78, 0x35, 0x35, 0x5A, 0x72, 0x51, 0x46, 0x71, 0x35, 0x68, 0x2F, 0x76, 0x50, 0x2B, 0x68, 0x4B, 0x6C, 0x57, 0x53, 0x5A, 0x50, 0x2F, 0x48, 0x74, 0x59, 0x33, 0x6B, 0x6E, 0x6C, 0x51, 0x32, 0x44, 0x63, 0x71, 0x69, 0x63, 0x6B, 0x52, 0x41, 0x68, 0x7A, 0x2F, 0x2F, 0x43, 0x38, 0x77, 0x73, 0x78, 0x73, 0x6F, 0x2B, 0x70, 0x6A, 0x4B, 0x64, 0x48, 0x48, 0x66, 0x44, 0x65, 0x72, 0x73, 0x30, 0x57, 0x30, 0x32, 0x6B, 0x7A, 0x4C, 0x30, 0x43, 0x6C, 0x6A, 0x6B, 0x4F, 0x70, 0x4A, 0x65, 0x64, 0x70, 0x6E, 0x6A, 0x64, 0x2B, 0x4A, 0x48, 0x4C, 0x70, 0x72, 0x79, 0x73, 0x75, 0x45, 0x53, 0x68, 0x4A, 0x51, 0x35, 0x76, 0x44, 0x49, 0x43, 0x34, 0x38, 0x66, 0x73, 0x6A, 0x35, 0x71, 0x77, 0x79, 0x33, 0x31, 0x35, 0x75, 0x79, 0x66, 0x70, 0x58, 0x4F, 0x59, 0x63, 0x49, 0x49, 0x32, 0x71, 0x4F, 0x51, 0x49, 0x44, 0x41, 0x51, 0x41, 0x42, 0x6F, 0x32, 0x59, 0x77, 0x5A, 0x44, 0x41, 0x53, 0x42, 0x67, 0x4E, 0x56, 0x48, 0x52, 0x4D, 0x42, 0x41, 0x66, 0x38, 0x45, 0x43, 0x44, 0x41, 0x47, 0x41, 0x51, 0x48, 0x2F, 0x41, 0x67, 0x45, 0x41, 0x4D, 0x41, 0x34, 0x47, 0x41, 0x31, 0x55, 0x64, 0x44, 0x77, 0x45, 0x42, 0x2F, 0x77, 0x51, 0x45, 0x41, 0x77, 0x49, 0x43, 0x68, 0x44, 0x41, 0x64, 0x42, 0x67, 0x4E, 0x56, 0x48, 0x51, 0x34, 0x45, 0x46, 0x67, 0x51, 0x55, 0x68, 0x78, 0x54, 0x70, 0x4E, 0x7A, 0x4A, 0x2B, 0x76, 0x62, 0x59, 0x6F, 0x2F, 0x44, 0x42, 0x71, 0x59, 0x43, 0x51, 0x51, 0x4D, 0x4E, 0x69, 0x39, 0x35, 0x32, 0x34, 0x77, 0x48, 0x77, 0x59, 0x44, 0x56, 0x52, 0x30, 0x6A, 0x42, 0x42, 0x67, 0x77, 0x46, 0x6F, 0x41, 0x55, 0x38, 0x6B, 0x47, 0x59, 0x6A, 0x34, 0x49, 0x49, 0x67, 0x30, 0x75, 0x4A, 0x63, 0x41, 0x59, 0x56, 0x4F, 0x37, 0x61, 0x6C, 0x54, 0x70, 0x74, 0x35, 0x61, 0x41, 0x67, 0x77, 0x50, 0x51, 0x59, 0x4A, 0x4B, 0x6F, 0x5A, 0x49, 0x68, 0x76, 0x63, 0x4E, 0x41, 0x51, 0x45, 0x4B, 0x4D, 0x44, 0x43, 0x67, 0x44, 0x54, 0x41, 0x4C, 0x42, 0x67, 0x6C, 0x67, 0x68, 0x6B, 0x67, 0x42, 0x5A, 0x51, 0x4D, 0x45, 0x41, 0x67, 0x47, 0x68, 0x47, 0x6A, 0x41, 0x59, 0x42, 0x67, 0x6B, 0x71, 0x68, 0x6B, 0x69, 0x47, 0x39, 0x77, 0x30, 0x42, 0x41, 0x51, 0x67, 0x77, 0x43, 0x77, 0x59, 0x4A, 0x59, 0x49, 0x5A, 0x49, 0x41, 0x57, 0x55, 0x44, 0x42, 0x41, 0x49, 0x42, 0x6F, 0x67, 0x4D, 0x43, 0x41, 0x53, 0x41, 0x44, 0x67, 0x67, 0x45, 0x42, 0x41, 0x49, 0x6C, 0x59, 0x44, 0x4F, 0x37, 0x52, 0x6E, 0x49, 0x64, 0x50, 0x44, 0x6F, 0x7A, 0x76, 0x57, 0x73, 0x68, 0x66, 0x75, 0x76, 0x51, 0x33, 0x56, 0x61, 0x77, 0x63, 0x6A, 0x47, 0x53, 0x6D, 0x4B, 0x4C, 0x4A, 0x47, 0x70, 0x31, 0x31, 0x4D, 0x78, 0x4D, 0x32, 0x4B, 0x64, 0x35, 0x53, 0x50, 0x54, 0x56, 0x54, 0x2B, 0x75, 0x69, 0x73, 0x4C, 0x6E, 0x74, 0x4A, 0x36, 0x43, 0x59, 0x2B, 0x46, 0x4E, 0x64, 0x43, 0x63, 0x37, 0x7A, 0x46, 0x32, 0x46, 0x39, 0x56, 0x61, 0x33, 0x36, 0x72, 0x6C, 0x49, 0x4B, 0x59, 0x7A, 0x71, 0x4E, 0x42, 0x73, 0x6D, 0x74, 0x4F, 0x48, 0x46, 0x36, 0x4E, 0x66, 0x52, 0x42, 0x45, 0x73, 0x73, 0x6F, 0x4A, 0x62, 0x35, 0x54, 0x71, 0x38, 0x31, 0x5A, 0x59, 0x44, 0x4D, 0x35, 0x77, 0x73, 0x47, 0x2B, 0x43, 0x4D, 0x74, 0x53, 0x38, 0x77, 0x5A, 0x30, 0x39, 0x4D, 0x30, 0x4F, 0x62, 0x6B, 0x69, 0x78, 0x7A, 0x5A, 0x46, 0x65, 0x50, 0x69, 0x64, 0x46, 0x2F, 0x4E, 0x49, 0x66, 0x50, 0x78, 0x31, 0x63, 0x6A, 0x2F, 0x46, 0x58, 0x45, 0x79, 0x6F, 0x73, 0x5A, 0x38, 0x6C, 0x43, 0x30, 0x56, 0x58, 0x46, 0x38, 0x44, 0x47, 0x39, 0x30, 0x77, 0x61, 0x78, 0x72, 0x68, 0x59, 0x35, 0x57, 0x2F, 0x7A, 0x69, 0x42, 0x4A, 0x54, 0x79, 0x37, 0x42, 0x54, 0x46, 0x4E, 0x6C, 0x67, 0x64, 0x4D, 0x46, 0x77, 0x2F, 0x77, 0x50, 0x2B, 0x57, 0x6D, 0x2B, 0x31, 0x50, 0x7A, 0x71, 0x50, 0x45, 0x53, 0x6E, 0x44, 0x63, 0x67, 0x4B, 0x72, 0x32, 0x44, 0x38, 0x6C, 0x59, 0x47, 0x59, 0x6B, 0x79, 0x50, 0x32, 0x41, 0x55, 0x4E, 0x45, 0x6D, 0x51, 0x6B, 0x72, 0x2F, 0x50, 0x32, 0x77, 0x71, 0x6A, 0x59, 0x6D, 0x46, 0x77, 0x55, 0x6C, 0x38, 0x46, 0x75, 0x36, 0x76, 0x79, 0x66, 0x58, 0x58, 0x4C, 0x72, 0x42, 0x44, 0x33, 0x44, 0x4E, 0x2B, 0x4D, 0x70, 0x72, 0x64, 0x55, 0x50, 0x32, 0x64, 0x2B, 0x66, 0x78, 0x72, 0x36, 0x6B, 0x63, 0x56, 0x66, 0x6A, 0x69, 0x6B, 0x7A, 0x66, 0x55, 0x5A, 0x6B, 0x48, 0x72, 0x76, 0x57, 0x71, 0x55, 0x62, 0x33, 0x7A, 0x36, 0x33, 0x49, 0x32, 0x4E, 0x4D, 0x50, 0x58, 0x6E, 0x6F, 0x44, 0x71, 0x33, 0x5A, 0x63, 0x4E, 0x2B, 0x31, 0x65, 0x45, 0x31, 0x35, 0x4A, 0x71, 0x66, 0x46, 0x6B, 0x36, 0x38, 0x5A, 0x36, 0x67, 0x4E, 0x68, 0x33, 0x71, 0x6A, 0x63, 0x46, 0x79, 0x61, 0x49, 0x30, 0x67, 0x3D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x45, 0x4E, 0x44, 0x20, 0x43, 0x45, 0x52, 0x54, 0x49, 0x46, 0x49, 0x43, 0x41, 0x54, 0x45, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D
 };

//---------------------------------------------------------------------
#if TARGET_ION_ABI_VERSION >= 2
/* uses the new version of ION */

static int32_t qsc_ION_memalloc(struct qsc_ion_info *handle,
                uint32_t size)
{
    int32_t ret = 0;
    unsigned char *v_addr;
    int32_t ion_fd = -1;
    int32_t map_fd = -1;
    uint32_t len = (size + 4095) & (~4095);
    uint32_t align = 0;
    uint32_t flags = 0;
    struct dma_buf_sync buf_sync;
    unsigned int heap_id = ION_QSECOM_HEAP_ID;


    ion_fd  = ion_open();
    if (ion_fd < 0) {
        LOGE("Error::Cannot open ION device\n");
        return -1;
    }

    ret = ion_alloc_fd(ion_fd, len, align, ION_HEAP(heap_id), flags, &map_fd);
    if (ret) {
        LOGE("Error::ion_alloc_fd for heap %u size %u failed ret = %d, errno = %d\n",
            heap_id, size, ret, errno);
        ret = -1;
        goto alloc_fail;
    }

    v_addr = (unsigned char *)mmap(NULL, len, PROT_READ | PROT_WRITE,
                    MAP_SHARED, map_fd, 0);
    if (v_addr == MAP_FAILED) {
        LOGE("Error::mmap failed\n");
        ret = -1;
        goto map_fail;
    }

    handle->ion_fd = ion_fd;
    handle->ifd_data_fd = map_fd;
    handle->ion_sbuffer = v_addr;
    handle->sbuf_len = size;

    buf_sync.flags = DMA_BUF_SYNC_START | DMA_BUF_SYNC_RW;
    ret = ioctl(map_fd, DMA_BUF_IOCTL_SYNC, &buf_sync);
    if (ret) {
        LOGE("Error:: DMA_BUF_IOCTL_SYNC start failed, ret = %d, errno = %d\n",
            ret, errno);
        ret = -1;
        goto sync_fail;
    }

    return ret;

sync_fail:
    if (v_addr) {
        ret = munmap(v_addr, len);
        if (ret){
            LOGE("Error::Unmapping v_addr failed with ret = %d\n",ret);
        }
        handle->ion_sbuffer = NULL;
    }
map_fail:
    if (map_fd >= 0 ) {
        ion_close(map_fd);
        handle->ifd_data_fd = -1;
    }
alloc_fail:
    if (ion_fd >= 0 ) {
        ion_close(ion_fd);
        handle->ion_fd = -1;
    }
    return ret;
}

static int qsc_ion_dealloc(struct qsc_ion_info *handle)
{
    struct dma_buf_sync buf_sync;
    uint32_t len = (handle->sbuf_len + 4095) & (~4095);
    int ret = 0;

    buf_sync.flags = DMA_BUF_SYNC_END | DMA_BUF_SYNC_RW;
    ret = ioctl(handle->ifd_data_fd, DMA_BUF_IOCTL_SYNC, &buf_sync);
    if (ret) {
        LOGE("Error:: DMA_BUF_IOCTL_SYNC start failed, ret = %d, errno = %d\n",
                ret, errno);
    }

    if (handle->ion_sbuffer) {
        ret = munmap(handle->ion_sbuffer, len);
        if (ret){
            LOGE("Error::Unmapping ION Buffer failed with ret = %d\n",ret);
        }
        handle->ion_sbuffer = NULL;
    }
    if (handle->ifd_data_fd >= 0 ) {
        ion_close(handle->ifd_data_fd);
        handle->ifd_data_fd= -1;
    }
    if (handle->ion_fd >= 0 ) {
        ion_close(handle->ion_fd);
        handle->ion_fd = -1;
    }
    return ret;
}
#endif

#ifndef TARGET_ION_ABI_VERSION

static int32_t qsc_ION_memalloc(struct qsc_ion_info *handle,
        uint32_t size)
{
    int32_t ret = 0;
    int32_t iret = 0;
    int32_t fd = 0;
    unsigned char *v_addr;
    struct ion_allocation_data ion_alloc_data;
    int32_t ion_fd;
    int32_t rc;
    struct ion_fd_data ifd_data;
    struct ion_handle_data handle_data;

    /* open ION device for memory management
     * O_DSYNC -> uncached memory
     */


    LOGE("#ifndef TARGET_ION_ABI_VERSION\n");

    if(handle == NULL){
        LOGE("Error:: null handle received");
        return -1;
    }
    ion_fd  = open("/dev/ion", O_RDONLY);
    if (ion_fd < 0) {
        LOGE("Error::Cannot open ION device\n");
        return -1;
    }
    handle->ion_sbuffer = NULL;
    handle->ifd_data_fd = 0;

    /* Size of allocation */
    ion_alloc_data.len = (size + 4095) & (~4095);

    /* 4K aligned */
    ion_alloc_data.align = 4096;

    /* memory is allocated from EBI heap */
    ion_alloc_data.heap_id_mask= ION_HEAP(ION_QSECOM_HEAP_ID);

    /* Set the memory to be uncached */
    ion_alloc_data.flags = 0;

    /* IOCTL call to ION for memory request */
    rc = ioctl(ion_fd, ION_IOC_ALLOC, &ion_alloc_data);
    if (rc) {
        LOGE("Error::Error while trying to allocate data\n");
        goto alloc_fail;
    }

    if (ion_alloc_data.handle == NULL) {
        LOGE("Error::ION alloc data returned a NULL\n");
        goto alloc_fail;
    } else {
        ifd_data.handle = ion_alloc_data.handle;
    }
    /* Call MAP ioctl to retrieve the ifd_data.fd file descriptor */
    rc = ioctl(ion_fd, ION_IOC_MAP, &ifd_data);
    if (rc) {
        LOGE("Error::Failed doing ION_IOC_MAP call\n");
        goto ioctl_fail;
    }

    /* Make the ion mmap call */
    v_addr = (unsigned char *)mmap(NULL, ion_alloc_data.len,
            PROT_READ | PROT_WRITE,
            MAP_SHARED, ifd_data.fd, 0);
    if (v_addr == MAP_FAILED) {
        LOGE("Error::ION MMAP failed\n");
        ret = -1;
        goto map_fail;
    }
    handle->ion_fd = ion_fd;
    handle->ifd_data_fd = ifd_data.fd;
    handle->ion_sbuffer = v_addr;
    handle->ion_alloc_handle.handle = ion_alloc_data.handle;
    handle->sbuf_len = size;
    return ret;

    map_fail:
    if(handle->ion_sbuffer != NULL)
    {
        ret = munmap(handle->ion_sbuffer, ion_alloc_data.len);
        if (ret)
            LOGE("Error::Failed to unmap memory for load image. ret = %d\n",
                    ret);
    }

    ioctl_fail:
    handle_data.handle = ion_alloc_data.handle;
    if (handle->ifd_data_fd)
        close(handle->ifd_data_fd);
    iret = ioctl(ion_fd, ION_IOC_FREE, &handle_data);
    if (iret){
        LOGE("Error::ION FREE ioctl returned error = %d\n",iret);
    }

    alloc_fail:
    if (ion_fd)
        close(ion_fd);
    return ret;

}

/** @brief: Deallocate ION memory
 *
 *
 */
static int32_t qsc_ion_dealloc(struct qsc_ion_info *handle)
{
    struct ion_handle_data handle_data;
    int32_t ret = 0;

    /* Deallocate the memory for the listener */
    ret = munmap(handle->ion_sbuffer,
            (handle->sbuf_len + 4095) & (~4095));
    if (ret){
        LOGE("Error::Unmapping ION Buffer failed with ret = %d\n",
                ret);
    }

    handle_data.handle = handle->ion_alloc_handle.handle;
    close(handle->ifd_data_fd);
    ret = ioctl(handle->ion_fd, ION_IOC_FREE, &handle_data);
    if (ret){
        LOGE("Error::ION Memory FREE ioctl failed with ret = %d\n",
                ret);
    }
    close(handle->ion_fd);
    return ret;

}

#endif

 /**@brief:  Implement simple application start
  *
  * @param[in/out]    handle.
  * @param[in]        appname.
  * @param[in]        buffer size.
  * @return    zero on success or error count on failure.
  */
 int32_t qsc_start_app(struct QSEECom_handle **l_QSEEComHandle,
                         const char *appname, int32_t buf_size)
 {
     int32_t ret = 0;

     /* start the application */
    /* load split images */
    ret = QSEECom_start_app(l_QSEEComHandle, "/vendor/firmware_mnt/image",
                appname, buf_size);

     if (ret) {
        LOGE("Loading app -%s failed err:%d",appname,ret);
     } else {
        LOGD("Loading app -%s succeded",appname);
     }
     return ret;
 }

 /**@brief:  Implement simple shutdown app
  * @param[in]    handle.
  * @return    zero on success or error count on failure.
  */
 int32_t qsc_shutdown_app(struct QSEECom_handle **l_QSEEComHandle)
 {
     int32_t ret = 0;

     LOGD("qsc_shutdown_app: start");
     /* shutdown the application */
     if (*l_QSEEComHandle != NULL) {
        ret = QSEECom_shutdown_app(l_QSEEComHandle);
        if (ret) {
           LOGE("Shutdown app failed with ret = %d", ret);
        } else {
           LOGD("shutdown app: pass");
        }
     } else {
         LOGE("cannot shutdown as the handle is NULL");
     }
     return ret;
 }

int main(int argc, char *argv[])
{
    struct QSEECom_handle *l_QSEEComHandle = NULL;
    struct QSEECom_ion_fd_info  ion_fd_info;
    struct qseecom_app_info app_info;

    sp_install_license_cmd_t* cmd;
    sp_install_license_rsp_t* rsp;
    struct qsc_ion_info ihandle;

    char app_name[20] = "sp_license";
    int32_t ret = -1;
    int32_t req_len = sizeof(sp_install_license_cmd_t);
    int32_t rsp_len = sizeof(sp_install_license_rsp_t);

    ret = qsc_start_app(&l_QSEEComHandle,app_name, 1024);
    if (ret)
    {
      LOGE("Start app: fail");
      return -1;
    }

    memset(&ihandle, 0, sizeof(ihandle));
    ret = qsc_ION_memalloc(&ihandle, ION_BUFFER_LENGTH);
    if (ret) {
        LOGD("Error allocating memory in ion\n");
        return -1;
    }
    memset(&ion_fd_info, 0, sizeof(struct QSEECom_ion_fd_info));

    ret = QSEECom_get_app_info(l_QSEEComHandle, &app_info);
    if (ret) {
        LOGE("Error to get app info\n");
        goto out;
    }


    if (!app_info.is_secure_app_64bit) {
        LOGD("send modified cmd to 32bit app - Invalid app info.");
        goto out;
    }

    cmd=(sp_install_license_cmd_t *)l_QSEEComHandle->ion_sbuffer;
    cmd->cmd_id = SPU_LICENSE_INSTALL_REQUEST;
    cmd->feature_id = 1396;
    cmd->license_len = sizeof(havent_testlicense);


    ion_fd_info.data[0].fd = ihandle.ifd_data_fd;
    ion_fd_info.data[0].cmd_buf_offset = 2*sizeof(uint32_t);

    req_len = sizeof(sp_install_license_cmd_t);
    LOGD("send modified cmd to 64bit app.");

    if (req_len & QSEECOM_ALIGN_MASK)
            req_len = QSEECOM_ALIGN(req_len);

    if (rsp_len & QSEECOM_ALIGN_MASK)
            rsp_len = QSEECOM_ALIGN(rsp_len);

    rsp = (sp_install_license_rsp_t *)l_QSEEComHandle->ion_sbuffer + req_len;
    rsp->status = 0;

    memcpy(ihandle.ion_sbuffer ,havent_testlicense, sizeof(havent_testlicense));


    /* send request from HLOS to sp_licnese QSEEApp */
    ret = QSEECom_send_modified_cmd_64(l_QSEEComHandle, cmd,
                req_len, rsp,
                rsp_len, &ion_fd_info);


out:

    ret = qsc_ion_dealloc(&ihandle);
    if (ret) {
        LOGD("Error de-allocating ion memory\n");
    }

    ret = qsc_shutdown_app(&l_QSEEComHandle);
    if (ret) {
        LOGE("   Failed to shutdown app: %d", ret);
    }
    return 0;
}