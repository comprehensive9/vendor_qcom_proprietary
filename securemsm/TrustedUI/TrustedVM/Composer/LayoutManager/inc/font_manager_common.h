/*===========================================================================
 * Copyright(c) 2013-2015, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ===========================================================================*/

#ifndef _FONT_MANAGER_COMMON_H
#define _FONT_MANAGER_COMMON_H

#include <stdint.h>

#ifdef __RVCT__
#pragma anon_unions
#endif

#define MAX_FILENAME_LEN (256)
#define FONT_BUFF_SIZE 256000  // 256K

// max supported chars (and therefore max number of segments in use) by a font
#define MAX_SUPPORTED_CHARS (1000)

/* Command IDs to the listener */
typedef enum {
    FONT_MGR_CMD_GET_DYN_FONT_INDEX     = 0x00000601,
    FONT_MGR_CMD_GET_DYN_FONT_SEGMENT,
    FONT_MGR_CMD_WARM_UP_DYN_FONT_SEGMENTS,
    FONT_MGR_CMD_UNKNOWN                = 0x7FFFFFFF
} font_mgr_cmd_type;

typedef struct dyn_font_req_index {
    uint32_t u32CmdId;
    char fontPath[MAX_FILENAME_LEN]; /* Path to the font file */
} dyn_font_req_index_t;

typedef struct dyn_font_req_segment {
    uint32_t u32CmdId;
    char fontPath[MAX_FILENAME_LEN]; /* Path to the font file */
    uint32_t u32SegIndex;
} dyn_font_req_segment_t;

typedef struct dyn_font_warmup_segments {
    uint32_t u32CmdId;
    char fontPath[MAX_FILENAME_LEN];             /* Path to the font file */
    uint32_t u32SegIndexes[MAX_SUPPORTED_CHARS]; /* Array of segment indexes */
    uint32_t numSegments; /* Number of elements in u32SegIndexes */
} dyn_font_warmup_segments_t;

struct dyn_font_res {
    int32_t n32Status;
    uint32_t u32DataLen;
    uint32_t u32BufLen;
    int32_t bufferHandle;
};

typedef union {
    struct dyn_font_req_index dynamicFontReqIndex;
    struct dyn_font_req_segment dynamicFontReqSegment;
    uint32_t u32CmdId;
} listener_request_t;

typedef union {
    struct dyn_font_res dynamicFontRes;
    int32_t  n32Status;
} listener_response_t;

typedef union {
    listener_request_t request;
    listener_response_t response;
} listener_mem_t;
typedef struct dynamic_index_prefix {
    uint32_t w;
    uint32_t h;
    uint32_t numSegments;
} dynamic_index_prefix_t;

typedef struct dynamic_font_index_entry {
    uint32_t firstCharIndex;
    uint32_t lastCharIndex;
    uint32_t offset;
} dynamic_font_index_entry_t;

typedef struct dynamic_font_index {
    dynamic_index_prefix_t indexPrefix;
    dynamic_font_index_entry_t* indexData;
} dynamic_font_index_t;

#endif  //_FONT_MANAGER_COMMON_H
