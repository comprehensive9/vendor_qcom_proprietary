/**
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "QPAY-HAL"
#define DBG true

#include <string>
#include <cstdlib>
#include <cstdint>
#include <utils/Log.h>

#include "QPayAdapter.h"

size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size) {
  size_t min_size = (dst_size < src_size) ? dst_size : src_size;
  memcpy(dst, src, min_size);
  return min_size;
}

void print_text(std::string intro_message, unsigned const char *text_addr,
                int size) {
  char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  std::string str;

  for (int i = 0; i < size; ++i) {
    const char ch = text_addr[i];
    str.append(&hex[(ch  & 0xF0) >> 4], 1);
    str.append(&hex[ch & 0xF], 1);
  }
  if (DBG) {
    ALOGD("%s : %s", intro_message.c_str(), str.c_str());
  }
}

