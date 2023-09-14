/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef UTILS_H
#define UTILS_H

#include <gralloc_priv.h>
#include <hardware/gralloc1.h>
#include <time.h>
#include <utils/Log.h>

uint32_t calcBufferSize(uint32_t format, uint32_t width, uint32_t stride,
                        uint32_t height);

bool isYUVSPFormat(uint32_t format);

uint32_t calcYStride(uint32_t format, uint32_t stride);

uint32_t calcUVStride(uint32_t format, uint32_t stride);

uint32_t calcUVOffset(uint32_t format, uint32_t stride, uint32_t height);

int64_t getTimeUsec();

#endif  // UTILS_H
