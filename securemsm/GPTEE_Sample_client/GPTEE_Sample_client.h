/**
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "TEE_client_api.h"

#define QTEEC_SAMPLE_LOGD(...) //disabled
#define QTEEC_SAMPLE_LOGE printf
#define QTEEC_SAMPLE_LOGV printf

#define BUFFERSIZE 4096

#define TEST(test_name, expected_result)  retval = test_name(); \
  if (retval != expected_result) \
  { \
    QTEEC_SAMPLE_LOGE("%s failed\n", #test_name); \
    ++failed_tests; \
  } \
  else \
  { \
    QTEEC_SAMPLE_LOGV("%s passed\n", #test_name); \
    ++passed_tests; \
  } \

#define TEST_CMD(test_name,command_id,expected_result)  retval = test_name(command_id); \
  if (retval != expected_result) \
  { \
    QTEEC_SAMPLE_LOGE("%s failed\n", #test_name); \
    ++failed_tests; \
  } \
  else \
  { \
    QTEEC_SAMPLE_LOGV("%s passed\n", #test_name); \
    ++passed_tests; \
  } \

//GP Sample tests Case IDs
typedef enum {
  GP_INVALID_TEST = 0x00,
  GP_SAMPLE_BUFFER_MULTIPLY_TEST = 1,
  GP_SAMPLE_WAIT_TEST = 2,
  GP_SAMPLE_CRYPTO_TEST = 3,
  GP_SAMPLE_PERSISTENT_OBJ_BASIC_TEST = 4,
  GP_SAMPLE_TA_TA_TEST = 5,
} GP_SAMPLE_TESTS_IDS;

    /* UUID for gptest app */
const TEEC_UUID gpSample2UUID = {
    0Xc02cac07, 0X2639, 0X4ef0, {0Xbc, 0x12,0Xc4, 0Xaf, 0X1f, 0Xb3, 0Xe2, 0X76}};

    /* Unknown UUIDs for test purpose */
const TEEC_UUID unknownUUIDs[] = {
    {0x3fd852b0, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
    {0x3fd856ac, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
    {0x3fd85968, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
    {0x3fd85be8, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
    {0x3fd8617e, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
    {0x3fd86412, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
    {0x3fd86692, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
    {0x3fd86926, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
    {0x3fd86be2, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
    {0x3fd86e58, 0x5563, 0x11e9, {0x5F, 0x54, 0x49, 0x4D, 0x45, 0x41, 0x50, 0x49}},
};

static inline size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size) {
	  size_t  copy_size = (dst_size <= src_size) ? dst_size : src_size;
	  memcpy(dst, src, copy_size);
	  return copy_size;
}