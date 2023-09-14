/**
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <utils/Log.h>
#define MAX_CHANNELS    5
#define MAX_AID_SIZE    16

using ::android::hardware::hidl_vec;

typedef struct mChannelInfo {
  bool isOpened = false;
  uint8_t aid[MAX_AID_SIZE] = {0x00};
} mChannelInfo_t;

int openChannel(const hidl_vec<uint8_t> &aid, uint8_t p2, int *cNumber,
                        uint8_t *respApdu, int *rLen, bool isBasic);
int transmitApdu(const hidl_vec<uint8_t> &data, uint8_t *respApdu,
                         int *rLen);
int closeChannel(uint8_t channelNumber);

