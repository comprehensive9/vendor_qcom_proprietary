/**
 * Copyright (c) 2018, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android/log.h>
#include <cstring>
#include <QTEEConnectorClient.h>
#include <EsePowerManager.h>
#include <stdlib.h>
#include <string>
#include "QPayAdapter.h"
#include "QPayAdapter-apdu.h"

static mChannelInfo_t channelsInfo[MAX_CHANNELS];

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#if DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...) do {} while (0)
#endif

using namespace android;

struct send_cmd {
    uint32_t session_handle;
    uint32_t channel_number;
    uint32_t cmd_id;
    uint32_t cmd_len;
    unsigned char tApdu[QPAY_MAX_APDU_SIZE];
};

struct send_cmd_rsp {
    uint32_t session_handle;
    uint32_t channel_number;
    uint32_t rsp_len;
    unsigned char rApdu[QPAY_MAX_APDU_SIZE];
};

static uint32_t handle = 0;
static uint32_t sessionHandle = 0;
static uint32_t channelNumber = 0;
static const char *path = "/vendor/firmware_mnt/image";
static const char *name = "qpay";
static QTEE::QTEEConnectorClient *client = NULL;

static bool isQpayConnectionOpen = false;
static bool isQpaySessionOpen = false;
static bool isClientCreated = false;

static int qpay_init()
{
    int reqLen = sizeof(struct send_cmd);
    int rspLen = sizeof(struct send_cmd_rsp);
    int buf_size = 0;
    int ret = QPAY_SUCCESS;

    /* let's load the TZ QSEE APP in TZ */
    buf_size = reqLen + rspLen;
    do {
        if (client != NULL) {
          LOGE("%s: Application already loaded", __func__);
          break;
        }

        client = new QTEE::QTEEConnectorClient(path, name, buf_size);
        if (client == NULL) {
            LOGE("%s: Error creating client", __func__);
            ret = QPAY_FAILURE;
            break;
        }
        client->load();
        isClientCreated = true;
    } while(0);
    return ret;
}

static int qpay_deInit()
{
    if (client != NULL) {
        LOGD("%s: UnLoading the TA APP", __func__);
        client->unload();
        isClientCreated = false;
        delete client;
        client = NULL;
        return QPAY_SUCCESS;
    }
    return QPAY_FAILURE;
}

int qpay_open()
{
    struct send_cmd cmd = {0};
    struct send_cmd_rsp msgrsp = {0};
    int ret = QPAY_SUCCESS;
    int32_t reqLen = 0;
    int32_t rspLen = 0;

    LOGD("%s: Entry", __func__);

    cmd.cmd_id = QPAY_OPEN_CONNECTION;
    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    //check if already open was success, return immediately
    if(isClientCreated && isQpayConnectionOpen && isQpaySessionOpen && nativeeSEGetState())
        return QPAY_SUCCESS;

    if (client == NULL) {
        LOGE("%s: client not initialized yet", __func__);
        ret = qpay_init();
        if(ret) {
            LOGE("%s: Client creation failed with ret = %d\n", __func__, ret);
            return QPAY_FAILURE;
        }
    }

    ret = client->sendCommand(&cmd, reqLen, &msgrsp, rspLen);
    if (ret || msgrsp.rApdu[STATUS_BYTE_INDEX] == 0xFF) {
        LOGE("%s: send command failed with ret = %d\n", __func__, ret);
        return QPAY_FAILURE;
    }
    LOGD("%s: response length for QPAY_OPEN_CONNECTION = %d, rApdu[0] = 0x%x\n", __func__, msgrsp.rsp_len, msgrsp.rApdu[STATUS_BYTE_INDEX]);
    isQpayConnectionOpen = true;

    if (!nativeeSEPowerOn()) {
        ALOGE("%s: eSE not powered on", __func__);
        return QPAY_FAILURE;
    }

    ret = getReader();
    if (ret) {
        LOGE("%s: qpay getReader failed with ret = %d\n", __func__, ret);
        return QPAY_FAILURE;
    }

    ret = openSession();
    if (ret) {
        LOGE("%s: qpay open session failed with ret = %d\n", __func__, ret);
        return QPAY_FAILURE;
    }

    LOGD("%s: Exit", __func__);
    return QPAY_SUCCESS;
}

int qpay_close()
{
    struct send_cmd cmd = {0};
    struct send_cmd_rsp msgrsp = {0};
    int ret = QPAY_SUCCESS;
    int32_t reqLen = 0;
    int32_t rspLen = 0;

    LOGD("%s: Entry", __func__);

    cmd.cmd_id = QPAY_CLOSE_CONNECTION;
    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    if (client == NULL) {
        LOGE("%s: client not initialized yet", __func__);
        return QPAY_FAILURE;
    }

    ret = closeSession();
    if (ret) {
        LOGE("%s: qpay close session failed with ret = %d\n", __func__, ret);
        return QPAY_FAILURE;
    }

    if (!nativeeSEPowerOff()) {
        ALOGE("eSE not powered Off");
        return QPAY_FAILURE;
    }

    ret = client->sendCommand(&cmd, reqLen, &msgrsp, rspLen);
    if (ret || msgrsp.rApdu[STATUS_BYTE_INDEX] == 0xFF) {
        LOGE("%s: send command failed with ret = %d\n", __func__, ret);
        return QPAY_FAILURE;
    }
    isQpayConnectionOpen = false;

    ret = qpay_deInit();
    if(ret) {
        LOGE("%s: qpay de_init failed with ret = %d\n", __func__, ret);
        return QPAY_FAILURE;
    }

    LOGD("%s: Exit", __func__);
    return QPAY_SUCCESS;
}

static int getReader()
{
    struct send_cmd cmd = {0};
    struct send_cmd_rsp msgrsp = {0};
    int ret = QPAY_SUCCESS;
    int32_t reqLen = 0;
    int32_t rspLen = 0;

    cmd.cmd_id = TEE_SEServiceGetReaders;
    cmd.tApdu[STATUS_BYTE_INDEX] = 0x00;
    cmd.cmd_len = 1;

    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    ret = client->sendCommand(&cmd, reqLen, &msgrsp, rspLen);
    if (ret || msgrsp.rApdu[STATUS_BYTE_INDEX] == 0xFF) {
        LOGE("%s: getReaders command failed with ret = %d\n", __func__, ret);
        return QPAY_FAILURE;
    }
    LOGD("%s: response length for GET_READERS = %d, rApdu[0] = 0x%x\n", __func__, msgrsp.rsp_len, msgrsp.rApdu[STATUS_BYTE_INDEX]);
    return QPAY_SUCCESS;
}

static int openSession()
{
    struct send_cmd cmd = {0};
    struct send_cmd_rsp msgrsp = {0};
    int ret = QPAY_SUCCESS;
    int32_t reqLen = 0;
    int32_t rspLen = 0;

    cmd.cmd_id = TEE_SEReaderOpenSession;
    cmd.tApdu[STATUS_BYTE_INDEX] = 0x00;
    cmd.cmd_len = 1;

    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    ret = client->sendCommand(&cmd, reqLen, &msgrsp, rspLen);
    if (ret || msgrsp.rApdu[STATUS_BYTE_INDEX] == 0xFF) {
        LOGE("%s: open session command failed with ret = %d\n", __func__, ret);
        return QPAY_FAILURE;
    }
    handle = msgrsp.session_handle;
    LOGD("%s: open session success and handle = %d\n", __func__, handle);
    LOGD("%s: response length for openSession = %d, rApdu[0] = 0x%x\n", __func__, msgrsp.rsp_len, msgrsp.rApdu[STATUS_BYTE_INDEX]);
    isQpaySessionOpen = true;
    return QPAY_SUCCESS;
}

static int closeSession()
{
    struct send_cmd cmd = {0};
    struct send_cmd_rsp msgrsp = {0};
    int ret = QPAY_SUCCESS;
    int32_t reqLen = 0;
    int32_t rspLen = 0;

    LOGD("%s: Entry", __func__);
    if (client == NULL) {
        LOGE("%s: client not initialized", __func__);
        return QPAY_FAILURE;
    }

    // Let's check if session already opened
    if (!isQpaySessionOpen) {
        LOGE("%s: session not opened yet", __func__);
        return QPAY_FAILURE;
    }

    cmd.cmd_id = TEE_SESessionClose;
    cmd.session_handle = handle;
    cmd.tApdu[STATUS_BYTE_INDEX] = 0x00;
    cmd.cmd_len = 1;

    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    ret = client->sendCommand(&cmd, reqLen, &msgrsp, rspLen);
    if (ret || msgrsp.rApdu[STATUS_BYTE_INDEX] == 0xFF) {
        LOGE("%s: close session command failed with ret = %d\n", __func__, ret);
        return QPAY_FAILURE;
    }
    isQpaySessionOpen = false;
    LOGD("%s: Exit", __func__);
    return QPAY_SUCCESS;
}

static int qpay_transceive(cmd_send_t *cmd_send, uint8_t *cmdPtr, uint8_t *rspPtr)
{
    struct send_cmd apdu_to_transmit;
    struct send_cmd_rsp msgrsp = {0};
    int32_t ret = 1;
    int32_t reqLen = 0;
    int32_t rspLen = 0;
    int32_t msg_rsp_len = 0;
    bool hasAnswered;
    bool isIrqPending;

    if (client == NULL) {
        LOGE("client not initialized");
        return QPAY_FAILURE;
    }

    apdu_to_transmit.cmd_id = cmd_send->cmdId;
    LOGD("NativeTransceive - cmd.id is %d", apdu_to_transmit.cmd_id);
    if (cmdPtr == NULL){
        LOGE("transmit buffer NULL");
        return QPAY_FAILURE;
    }

    if (QPAY_MAX_APDU_SIZE < cmd_send->cmdLen) {
        LOGE("cmdLen too long %d, max is %d", cmd_send->cmdLen, QPAY_MAX_APDU_SIZE);
        return QPAY_FAILURE;
    }

    apdu_to_transmit.cmd_len = cmd_send->cmdLen;
    apdu_to_transmit.tApdu[STATUS_BYTE_INDEX] = 0x00;
    if(cmd_send->cmdLen > STATUS_BYTE_LENGTH) {
        memcpy(&apdu_to_transmit.tApdu[DATA_BYTE_INDEX], cmdPtr, cmd_send->cmdLen - STATUS_BYTE_LENGTH);
    }
    print_text("tApdu to be sent :", &apdu_to_transmit.tApdu[STATUS_BYTE_INDEX], apdu_to_transmit.cmd_len);
    if (cmd_send->sessionHandle > 0){
        apdu_to_transmit.session_handle = cmd_send->sessionHandle;
    }
    //TODO ?
    apdu_to_transmit.channel_number = cmd_send->channelNumber;

    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    do {
        ret = client->sendCommand(&apdu_to_transmit, reqLen, &msgrsp, rspLen);
        if (ret != QPAY_SUCCESS || msgrsp.rApdu[STATUS_BYTE_INDEX] == 0xFF) {
          LOGE("%s: Command send failure", __func__);
          return QPAY_FAILURE;
        }
        apdu_to_transmit.tApdu[STATUS_BYTE_INDEX] = msgrsp.rApdu[STATUS_BYTE_INDEX];
        hasAnswered = ((msgrsp.rApdu[STATUS_BYTE_INDEX] == 0x00) || ((msgrsp.rApdu[STATUS_BYTE_INDEX] & 0xFF) == 0xFF));
        isIrqPending = msgrsp.rApdu[STATUS_BYTE_INDEX] & IRQ_PENDING;
        if (isIrqPending)
          LOGD("%s: IRQ pending, retry", __func__);
    } while (!(hasAnswered));

    msg_rsp_len = msgrsp.rsp_len - STATUS_BYTE_LENGTH;
    memscpy(rspPtr, msg_rsp_len, &msgrsp.rApdu[DATA_BYTE_INDEX], msg_rsp_len);
    print_text("rApdu received :", rspPtr, msg_rsp_len);
    return msg_rsp_len;
}

int openChannel(const hidl_vec<uint8_t> &aid, uint8_t p2, int *cNumber,
                        uint8_t *respApdu, int *rLen, bool isBasic) {

  cmd_send_t input_cmd;
  int sw1, sw2, sw;
  int result = QPAY_FAILURE;
  uint8_t buff[] = {0x00};

  if (rLen == nullptr) {
    ALOGE("rLen is null");
    return result;
  } else if (respApdu == nullptr) {
    ALOGE("respApdu is null");
    return result;
  } else if (cNumber == nullptr) {
    ALOGE("cNumber is null");
    return result;
  }

  //Let's check if the eSE is powered ON, and the TA ready.
  result = qpay_open();
  if (result != QPAY_SUCCESS) {
    return result;
  }

  input_cmd.sessionHandle = handle;
  input_cmd.channelNumber = 0;

  if (!isBasic) {
    input_cmd.cmdId = TEE_SESessionOpenLogicalChannel;
  }
  else {
    input_cmd.cmdId = TEE_SESessionOpenBasicChannel;
  }

  if (aid.size()) {
    input_cmd.cmdLen = aid.size() + 1;
    *rLen = qpay_transceive(&input_cmd, (uint8_t *)aid.data(), respApdu);
  }
  else {
    input_cmd.cmdLen = 1;
    *rLen = qpay_transceive(&input_cmd, buff, respApdu);
  }

  if (*rLen == QPAY_FAILURE) {
      result = QPAY_FAILURE;
      ALOGE("%s: Opening channel failed for %d Channel", __func__, isBasic);
      goto clean_up;
  }

  //response handle
  channelNumber = respApdu[0] & 0xff;

  sw1 = respApdu[*rLen - 2] & 0xFF;
  sw2 = respApdu[*rLen - 1] & 0xFF;
  sw = (sw1 << 8) | sw2;
  *rLen -= 1; //remove channel number

  if(isBasic)
  {
      if (sw == 0x6A81) {
          ALOGE("%s: No free channel available", __func__);
          result = QPAY_ERROR_OUT_OF_MEMORY;
          goto clean_up;
      } else if ((sw == 0x6a82) || (sw == 0x6999)) {
          result = QPAY_ERROR_ITEM_NOT_FOUND;
          ALOGE("%s: Applet not found", __func__);
      } else if (sw == 0x6985) {
          ALOGE("%s: Conditions of use not satisfied", __func__);
          result = QPAY_ERROR_ITEM_NOT_FOUND;
          goto clean_up;
      }  else if ((sw != 0x9000) && (sw != 0x6283)) {
          ALOGE("%s: SELECT CMD failed - SW is not 0x9000/0x6283", __func__ );
          result = QPAY_FAILURE;
          goto clean_up;
      } else {
          ALOGD("%s: openBasicChannel Success", __func__);
          channelsInfo[0].isOpened = true;
          print_text("openBasicChannel Success, Select Response :", &respApdu[DATA_BYTE_INDEX], *rLen);
      }
  }
  else
  {
      if (sw == 0x6A81) {
          ALOGE("%s: No free channel available", __func__);
          result = QPAY_ERROR_OUT_OF_MEMORY;
          goto clean_up;
      } else if ((sw == 0x6a82) || (sw == 0x6999)) {
          ALOGE("%s: Applet not found", __func__);
          result = QPAY_ERROR_ITEM_NOT_FOUND;
      } else if (sw == 0x6985) {
          ALOGE("%s: Conditions of use not satisfied", __func__);
          result = QPAY_ERROR_ITEM_NOT_FOUND;
          goto clean_up;
      } else if (channelNumber == 0 || channelNumber > 19) {
          ALOGE("%s: Invalid logical channel number returned", __func__);
          result = QPAY_FAILURE;
          goto clean_up;
      } else {
          ALOGD("%s: openLogicalChannel Success", __func__);
          *cNumber = channelNumber;
          channelsInfo[*cNumber].isOpened = true;
          print_text("openLogicalChannel Success, Select Response :", &respApdu[DATA_BYTE_INDEX], *rLen);
      }
  }

  return QPAY_SUCCESS;
clean_up:
  qpay_close();
  return result;
}

int transmitApdu (const hidl_vec<uint8_t> &data, uint8_t *respApdu,
                          int *rLen) {
  int result = QPAY_FAILURE;
  cmd_send_t input_cmd;

  if (respApdu == nullptr) {
    ALOGE("%s: respApdu is null", __func__);
    return result;
  } else if (rLen == nullptr) {
    ALOGE("%s: rLen is null", __func__);
    return result;
  }

  //Let's check if the eSE is powered ON, and the TA ready.
  result = qpay_open();
  if (result != QPAY_SUCCESS) {
    return result;
  }

  input_cmd.sessionHandle = handle;
  input_cmd.channelNumber = 0;
  input_cmd.cmdId = TEE_SEChannelTransmit;
  input_cmd.cmdLen = data.size() + 1;

  *rLen = qpay_transceive(&input_cmd, (uint8_t *)data.data(), respApdu);
  if(*rLen == QPAY_FAILURE) {
      ALOGE("%s: Transmit failed", __func__);
      return QPAY_FAILURE;
  }
  //TODO validate response
  return QPAY_SUCCESS;
}

int closeChannel(uint8_t channelNumber) {
    int ret = QPAY_SUCCESS;
    int rLen;
    uint8_t buff[1];
    uint8_t respApdu[STATUS_BYTE_LENGTH];
    cmd_send_t input_cmd;

    if ((channelNumber < 0) || (channelNumber >= MAX_CHANNELS)) {
        ALOGE("Channel number not valid : %d", channelNumber);
        return QPAY_FAILURE;
    }

    buff[0] = channelNumber & 0xFF;
    ALOGE("%s: channel number = %d", __func__, channelNumber);

    input_cmd.sessionHandle = handle;
    input_cmd.channelNumber = 0;
    input_cmd.cmdId = TEE_SEChannelClose;
    input_cmd.cmdLen = sizeof(buff) + STATUS_BYTE_LENGTH;

    rLen = qpay_transceive(&input_cmd, buff, respApdu);
    if(rLen == QPAY_FAILURE) {
        ALOGE("%s: Close channel failed", __func__);
        return rLen;
    }
    else {
        channelsInfo[channelNumber].isOpened = false;
        if (allChannelsClosed()) {
          qpay_close();
        }
    }
    return QPAY_SUCCESS;
}

bool allChannelsClosed() {
  for (int i = 0; i < MAX_CHANNELS; i++) {
    if (channelsInfo[i].isOpened == true) {
      ALOGD("Channel number %d still open", i);
      return false;
    }
  }
  return true;
}
