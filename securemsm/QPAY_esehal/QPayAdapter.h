/**
 * Copyright (c) 2018, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define QPAY_SUCCESS            0
#define QPAY_FAILURE            -1
#define IRQ_PENDING             0x06

#define DEBUG true
#define QPAY_OPEN_CONNECTION                    1
#define QPAY_TRANSCEIVE                         2
#define QPAY_CLOSE_CONNECTION                   3
#define QPAY_SWITCHOFF_ESE                      4
#define QPAY_MAX_APDU_SIZE                      2200

#define TEE_SEServiceGetReaders                 43
#define TEE_SEReaderOpenSession                 53
#define TEE_SESessionClose                      63
#define TEE_SESessionOpenBasicChannel           65
#define TEE_SESessionOpenLogicalChannel         66
#define TEE_SEChannelClose                      71
#define TEE_SEChannelTransmit                   74

#define STATUS_BYTE_INDEX                       0
#define DATA_BYTE_INDEX                         1
#define STATUS_BYTE_LENGTH                      1

#define QPAY_ERROR_OUT_OF_MEMORY                0xFFFF000C
#define QPAY_ERROR_ITEM_NOT_FOUND               0xFFFF0008

typedef struct {
    uint32_t cmdId;
    uint32_t cmdLen;
    uint32_t channelNumber;
    uint32_t sessionHandle;
} cmd_send_t;

static int qpay_transceive(uint32_t cmdId, uint32_t cmdLen, uint8_t *cmdPtr,
                                  uint32_t sHandle, uint32_t cNumber);
int qpay_open();
int qpay_close();
static int getReader();
static int openSession();
static int closeSession();

bool allChannelsClosed();

bool memsApducmp(const uint8_t *Apdu1, size_t Apdu1length, const uint8_t *Apdu2,
              size_t Apdu2length);
size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);
void print_text(std::string intro_message, unsigned const char *text_addr,
                int size);

int nativeeSEPowerOn();
int nativeeSEPowerOff();
int nativeeSEGetState();
