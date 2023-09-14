/**
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GPQESE_API_H_
#define __GPQESE_API_H_

#include "interface.h"

int ese_open();
int ese_close();
int resetJcopUpdate();
void prepare_and_do_transceive(const uint8_t *tx_buff, int32_t tx_len, uint8_t *respApdu, int32_t rx_len, bool isRawTransmit, int32_t *rx_len_out);

#endif /* __GPQESE_API_H_ */
