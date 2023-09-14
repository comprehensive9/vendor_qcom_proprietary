/*
 * Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef SAMPLE_UTILS_H
#define SAMPLE_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <keymaster_defs.h>



//safe implementation of memcpy
static inline size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size) {
      size_t  copy_size = (dst_size <= src_size) ? dst_size : src_size;
      memcpy(dst, src, copy_size);
      return copy_size;
}

//helper function to print encrypted material in hexa
static inline void hexdump(const uint8_t* buffer, size_t size) {
    for (uint32_t i = 0; i < size; i++)
        printf("%02X", buffer[i]);
}

// helper function to create AES key that need upgrade
keymaster_error_t get_old_aes_key(keymaster_key_blob_t* aes_key_blob);

//initialize keymaster - need to be called only once
keymaster_error_t initialize_keymaster(void);

//helper function to print a specific keymaster tag value from key characteristics
void print_tag_value_from_characterstic(keymaster_tag_t tag, const keymaster_key_characteristics_t* characterstics);

#endif

