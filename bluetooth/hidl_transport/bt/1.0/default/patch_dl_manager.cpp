/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 *  Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *  Not a Contribution.
 *
 *  Copyright 2012 The Android Open Source Project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you
 *  may not use this file except in compliance with the License. You may
 *  obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  permissions and limitations under the License.
 *
 */

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <utils/Log.h>
#include <fcntl.h>
#include <string.h>
#include <asm-generic/ioctls.h>
#include <hidl/HidlSupport.h>
#include <patch_dl_manager.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#include <termios.h>
#include "logger.h"
#include "ibs_handler.h"
#include <sys/stat.h>
#include <regex>

#include "bluetooth_address.h"
#define UNUSED(x) (void)(x)
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.bluetooth@1.0-patch_dl_manager"

#define BT_VERSION_FILEPATH "/data/vendor/bluetooth/bt_fw_version.txt"
#define MSB_NIBBLE_MASK 0xF0
#define LSB_NIBBLE_MASK 0x0F
#define PA_OFFSET 118 //power amplifier offset
#define WAKEUP_CONFIG_MASK  0x03
#define WAKEUP_CONFIG_BYTE_CHE_2_AND_BEFORE  7
#define WAKEUP_CONFIG_BYTE_CHE_3_AND_LATER   4
#define WAKEUP_CONFIG_UART_RxD_CONFIG  1
#define WAKEUP_CONFIG_DEFAULT  0

uint8_t userial_to_tcio_baud(uint8_t cfg_baud, uint32_t *baud);
int userial_tcio_baud_to_int(uint32_t baud);

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef USER_DEBUG
version_info version_info_ins = {0, 0, "No Build label"};
#endif

bool wipower_enabled = false;
bool wipower_property_read = false;
bool is_alt_path_for_fw_to_be_used = false;
char alt_path_for_fw[PROPERTY_VALUE_MAX] = {'\0'};
uint64_t chipset_ver_ = 0;

PatchDLManager::PatchDLManager(BluetoothSocType soc_type, HciUartTransport* transport, PowerManager* power_manager) :
  soc_type_(soc_type), uart_transport_(transport), power_manager_(power_manager), dnld_fd_in_progress_(-1)
{
  ALOGD("%s", __func__);
  bt_logger_ = Logger::Get();
  fd_transport_ = uart_transport_->GetCtrlFd(),
  wait_vsc_evt_ = true;
  patch_dnld_pending_ = false;
  secure_bridge_enabled = false;
  is_mode_change_needed = false;
  elf_config_read_ = false;
  unified_hci = false;
  memset(&add_on_features, 0, sizeof(add_on_features));
  LoadPatchMaptable();
}

PatchDLManager::~PatchDLManager()
{
  patch_dnld_pending_ = false;
  unified_hci = false;
  for(auto & element: PatchPathInfoMap_)
    delete element.second;
  PatchPathInfoMap_.clear();
}

bool PatchDLManager::GetBdaddrFromProperty(const char *property, uint8_t res_bd_addr[6], bool is_reverse)
{
  bool is_valid = false;
  int i = 0;
  static char bd_addr[PROPERTY_VALUE_MAX];
  char* tok = NULL;
  char *tmp_token = NULL;

  if (bt_logger_->PropertyGet(property, bd_addr, NULL)) {
    ALOGV("BD address read from Boot property: %s\n", bd_addr);
    tok = strtok_r(bd_addr, ":", &tmp_token);
    while (tok != NULL) {
      ALOGV("bd add [%d]: %ld ", i, strtol(tok, NULL, 16));
      if (i >= 6) {
        ALOGE("bd property of invalid length");
        is_valid = false;
        break;
      }
      if (!ValidateToken(tok)) {
        ALOGE("Invalid token in BD address");
        is_valid = false;
        break;
      }
      if (is_reverse) {
        res_bd_addr[5 - i] = strtol(tok, NULL, 16);
      } else {
        res_bd_addr[i] = strtol(tok, NULL, 16);
      }
      tok = strtok_r(NULL, ":", &tmp_token);
      i++;
    }
  }
  if (i == 6)
    is_valid = true;

  return is_valid;
}

bool PatchDLManager::ValidateToken(char* bdaddr_tok)
{
  int i = 0;
  bool ret;

  if (strlen(bdaddr_tok) != 2) {
    ret = false;
    ALOGE("Invalid token length");
  } else {
    ret = true;
    for (i = 0; i < 2; i++) {
      if ((bdaddr_tok[i] >= '0' && bdaddr_tok[i] <= '9') ||
          (bdaddr_tok[i] >= 'A' && bdaddr_tok[i] <= 'F') ||
          (bdaddr_tok[i] >= 'a' && bdaddr_tok[i] <= 'f')) {
        ret = true;
        ALOGV("%s: tok %s @ %d is good", __func__, bdaddr_tok, i);
      } else {
        ret = false;
        ALOGE("invalid character in tok: %s at ind: %d", bdaddr_tok, i);
        break;
      }
    }
  }
  return ret;
}

int PatchDLManager::CheckEmbeddedMode()
{
  int err = 0;

  wipower_flag_ = 0;
  /* Get current wipower charging status */
  if ((err = WipowerCurrentChargingStatusReq()) < 0) {
    ALOGI("%s: Wipower status req failed (0x%x)", __func__, err);
  }
  usleep(500);

  ALOGE("%s: wipower_flag_: %d", __func__, wipower_flag_);

  return wipower_flag_;
}

int PatchDLManager::PerformChipInit()
{
  int ret = 0, err = -1, flags;
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;
  bool is_emb_wp_mode = false;
  char wipower_status[PROPERTY_VALUE_MAX];

  gettimeofday(&tv, NULL);
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Getting Local BD Address");
  BtState :: Get()->SetPreGetLocalAddressTS(dst_buff);

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_GET_LOCALADDR_STUCK);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_GET_LOCALADDR_STUCK,
                                            "Get Local BD Address");

  BluetoothAddress::GetLocalAddress(vnd_local_bd_addr_);

  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done getting Local BD Address");
  BtState :: Get()->SetPostGetLocalAddressTS(dst_buff);

  memset(dst_buff, 0, sizeof(dst_buff));

  gettimeofday(&tv, NULL);
  Logger::Get()->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK, "UART FLOW ON");
  /* Workaround UART issue: Make sure RTS is flowed ON in case it was not flowed on during cleanup due to UART issue */
  err = uart_transport_->Ioctl(USERIAL_OP_FLOW_ON, &flags);
  gettimeofday(&tv, NULL);
  Logger::Get()->CheckAndAddToDelayList(&tv);

  if (err < 0) {
    ALOGE("%s: HW Flow-on error: 0x%x \n", __func__, err);
    bt_logger_->SetInitFailureReason(BT_HOST_REASON_HW_FLOW_ON_FAILED);
    return err;
  }

  if (!wipower_property_read) {
    bt_logger_->PropertyGet("ro.vendor.bluetooth.wipower", wipower_status, "false");
    if (strcmp(wipower_status, "true") == 0)
      wipower_enabled = true;
    wipower_property_read = true;
  }
  if (wipower_enabled) {
    CheckEmbeddedMode();
    is_emb_wp_mode = true;
  } else {
    ALOGI("Wipower not enabled");
  }

  ret = SocInit(vnd_local_bd_addr_, is_emb_wp_mode);
  gettimeofday(&tv, NULL);
  if (ret < 0) {
    BtState::Get()->AddLogTag(dst_buff, tv, (char *)"SoC initialization failed");
    ALOGE("%s: SoC initialization failed: %d\n", __func__, ret);
  } else {
    BtState::Get()->AddLogTag(dst_buff, tv, (char *)"SoC initialization successful");
  }
  BtState::Get()->SetFwDwnldSucceed(dst_buff);

  return ret;
}

/*
 * For Hand-Off related Wipower commands, Command complete arrives first and
 * the followd with VS event
 *
 */
int PatchDLManager::HciSendWipowerVsCmd(unsigned char *cmd, unsigned char *rsp, int size)
{
  int ret = 0;
  int err = 0;

  /* Send the HCI command packet to UART for transmission */
  ret = uart_transport_->UartWrite(cmd, size);
  if (ret != size) {
    ALOGE("%s: WP Send failed with ret value: %d", __func__, ret);
    goto failed;
  }

  /* Wait for command complete event */
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Failed to charging status cmd on Controller", __func__);
    goto failed;
  }
  ALOGI("%s: WP Received HCI command complete Event from SOC", __func__);
 failed:
  return ret;
}


int PatchDLManager::WipowerCurrentChargingStatusReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_WIPOWER_VS_CMD_OCF);
  cmd_hdr->plen     = EDL_WIP_QUERY_CHARGING_STATUS_LEN;
  cmd[4]  = EDL_WIP_QUERY_CHARGING_STATUS_CMD;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_WIP_QUERY_CHARGING_STATUS_LEN);

  ALOGD("%s: Sending EDL_WIP_QUERY_CHARGING_STATUS_CMD", __func__);
  ALOGV("HCI-CMD: \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);

  err = HciSendWipowerVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send EDL_WIP_QUERY_CHARGING_STATUS_CMD command!");
    goto error;
  }

  /* Check for response from the Controller */
  if (!unified_hci) {
    if (ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
      err = -ETIMEDOUT;
      ALOGI("%s: WP Failed to get HCI-VS Event from SOC", __func__);
      goto error;
    }

    /* Read Command Complete Event */
    if (rsp[4] > NON_WIPOWER_MODE) {
      err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if (err < 0) {
        ALOGE("%s: Failed to get charging status", __func__);
        goto error;
      }
    }
   }
 error:
  return err;
}

int PatchDLManager::WipowerForwardHandoffReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_WIPOWER_VS_CMD_OCF);
  cmd_hdr->plen     = EDL_WIP_START_HANDOFF_TO_HOST_LEN;
  cmd[4]  = EDL_WIP_START_HANDOFF_TO_HOST_CMD;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_WIP_START_HANDOFF_TO_HOST_LEN);

  ALOGD("%s: Sending EDL_WIP_START_HANDOFF_TO_HOST_CMD", __func__);
  ALOGD("HCI-CMD: \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
  err = HciSendWipowerVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send EDL_WIP_START_HANDOFF_TO_HOST_CMD command!");
    goto error;
  }

  /* Check for response from the Controller */
  if (ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
    err = -ETIMEDOUT;
    ALOGI("%s: WP Failed to get HCI-VS Event from SOC", __func__);
    goto error;
  }

 error:
  return err;
}

int PatchDLManager::PatchVerReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char dst_buff[MAX_BUFF_SIZE] = {'\0'};
  char res_buff[MAX_BUFF_SIZE] = {'\0'};
  struct timeval tv;

  gettimeofday(&tv, NULL);
  ALOGD("%s: Sending Get Version CMD to SOC", __func__);
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Sending Get Version CMD to SOC");
  BtState :: Get()->SetGetVerReqTS(dst_buff, SEND_CMD_TS);

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_GETVER_SEND_STUCK);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_GETVER_SEND_STUCK,
                                            "SENDING GET VERSION CMD");

  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciPkt(cmd, EDL_PATCH_VER_REQ_CMD, 0, -1, EDL_PATCH_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to attach the patch payload to the Controller!");
    goto error;
  } else if( (wait_vsc_evt_) && ((rsp[CMD_RSP_OFFSET] != EDL_CMD_REQ_RES_EVT) ||
            (rsp[RSP_TYPE_OFFSET] != EDL_APP_VER_RES_EVT)) && (!unified_hci)) {
    /* This is a workaround for an issue where an unexpected event was received */
    ALOGE("%s: Unexpected event revceived! Reading the next event", __func__);
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  } else {
    ALOGE("%s: rsp[CMD_RSP_OFFSET] = %x ,  rsp[RSP_TYPE_OFFSET] = %x", __func__, rsp[CMD_RSP_OFFSET], rsp[RSP_TYPE_OFFSET]);
  }
  /* Read Command Complete Event*/
  if (!unified_hci) {
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
      ALOGE("%s: Failed to get patch version(s)", __func__);
      goto error;
    }
  }
  gettimeofday(&tv, NULL);
  snprintf(res_buff, sizeof(res_buff), "Get Version rsp rcvd, num bytes in last rsp = %d", err);
  BtState :: Get()->AddLogTag(dst_buff, tv, res_buff);
  BtState :: Get()->SetGetVerResEvtTS(dst_buff);
  bt_logger_->SetSecondaryCrashReason(BT_SOC_REASON_DEFAULT);

 error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;

}


int PatchDLManager::PatchConfigReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  struct timeval tv;

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PATCH_CONFIG_CMD_STUCK);
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PATCH_CONFIG_CMD_STUCK,
                                            "ELF PATCH CONFIG CMD");
  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciPkt(cmd, EDL_TLV_PATCH_CONFIG, 0, -1, EDL_PATCH_CONFIG_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CONFIG_CMD_LEN);
  ALOGD("%s: Sending Patch config CMD to SOC", __func__);
  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if (err != size) {
    ALOGE("Failed to attach the patch payload to the Controller!");
    err = -1;
    goto error;
  }

error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

void PatchDLManager::GetAppVerCmd() {
  int size, err;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  struct timeval tv;

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_GET_APP_VER_CMD_STUCK);
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_GET_APP_VER_CMD_STUCK,
                                            "GET APP VERSION CMD");

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);
  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]      = HCI_COMMAND_PKT;
  cmd_hdr->opcode = 0xfc00;
  cmd_hdr->plen   = 0x01;
  cmd[4]      = EDL_GETAPPVER_REQ;
  /* Total length of the packet to be sent to the Controller */
  size = 5;

  ALOGD("Sending %s", __func__);
  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if (err != size) {
    ALOGE("%s: Failed to send GetAppVerCmd to the SoC!", __func__);
    goto error;
  }

  if (GETAPPVER_REQ_INVALID_CMD == rsp[CMD_STATUS_OFFSET_UNIFIED]) {
    is_mode_change_needed = true;
    ALOGW("%s: SoC is in EDL mode, EDL_GETAPPVER_REQ status code : (0x%x)",
            __func__, rsp[CMD_STATUS_OFFSET_UNIFIED]);
  } else {
    ALOGD("%s: EDL_GETAPPVER_REQ status code : (0x%x)", __func__,
            rsp[CMD_STATUS_OFFSET_UNIFIED]);
  }
error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
}

void PatchDLManager::EdlModeChange() {
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  hci_command_hdr *cmd_hdr;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);
  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]      = HCI_COMMAND_PKT;
  cmd_hdr->opcode = 0xfc69;
  cmd_hdr->plen   = 0x01;
  cmd[4]      = 00;
  /* Total length of the packet to be sent to the Controller */
  size = 5;

  ALOGD("%s: Sending change mode CMD to SOC for going to normal mode", __func__);

  bt_logger_->ProcessTx(HCI_PACKET_TYPE_COMMAND, &cmd[1], size - 1);

  /* Send HCI Command packet to Controller */
  err = uart_transport_->UartWrite(cmd, size);
  if (err != size) {
    ALOGE("Failed to send chnage mode cmd to the SoC!");
    return;
  }
  // Sleep for 500 ms to give SoC time to come back to normal mode.
  usleep(1000 * 500);
}

int PatchDLManager::GetBuildInfoReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  struct timeval tv;

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_GETBLDINFO_CMD_STUCK,
                                            "BUILD INFO CMD & RSP");

  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciPkt(cmd, EDL_GET_BUILD_INFO, 0,
                -1, EDL_PATCH_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send get build info cmd to the SoC!");
    goto error;
  }

  if (!unified_hci) {
    err = ReadHciEvent( rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
      ALOGE("%s: Failed to get build info", __func__);
      goto error;
    }
  }
 error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;

}

int PatchDLManager::GetXmemTlvFile(const char *file_path, const char* alt_file_path1,
                                     const char* alt_file_path2){

    FILE * pFile = NULL;
    long fileSize;
    int readSize;
    int file_path_length = 0;

    if (file_path)
      file_path_length = strlen(file_path);
      //Try opening from "data/vendor/bluetooth" in case of XMEM ROM.
    if (file_path_length == 0 || !(pFile = fopen(file_path, "r"))) {
      if(file_path_length != 0)
        ALOGE("%s File Opening from: %s %s failed (%d)", __func__,
              file_path, strerror(errno), errno);
      return GetTlvFile(alt_file_path1, alt_file_path2);
    } else {
      ALOGI("File open %s succeeded", file_path);
      /* Get File Size */
      fseek(pFile, 0, SEEK_END);
      fileSize = ftell(pFile);
      rewind(pFile);

      pdata_buffer_ = (unsigned char*)new char[fileSize];
      if (pdata_buffer_ == NULL) {
        ALOGE("Allocated Memory failed");
        fclose(pFile);
        return -1;
      }

      /* Copy file into allocated buffer */
      readSize = fread(pdata_buffer_, 1, fileSize, pFile);
      /* File Close */
      fclose(pFile);

      if (readSize != fileSize) {
        ALOGE("Read file size(%d) not matched with actual file size (%ld bytes)",
                readSize, fileSize);
        delete []pdata_buffer_;
        return -1;
      }
      if (ReadTlvInfo())
        return readSize;
      else
        return -1;
    }

}
int PatchDLManager::GetTlvFile(const char *file_path, const char* alt_file_path)
{
  FILE * pFile = NULL;
  int fileSize;
  int readSize;

  if (pFile == NULL) {
    pFile = OpenPatchFile(file_path, alt_file_path);
  }

  if( pFile == NULL) {
    return -1;
  }

  /* Get File Size */
  fseek(pFile, 0, SEEK_END);
  fileSize = ftell(pFile);
  rewind(pFile);

  pdata_buffer_ = (unsigned char*)new char[fileSize];
  if (pdata_buffer_ == NULL) {
    ALOGE("Allocated Memory failed");
    fclose(pFile);
    return -1;
  }
  /* Copy file into allocated buffer */
  readSize = fread(pdata_buffer_, 1, fileSize, pFile);

  /* File Close */
  fclose(pFile);

  if (readSize != fileSize) {
    ALOGE("Read file size(%d) not matched with actual file size (%ld bytes)", readSize, fileSize);
    delete []pdata_buffer_;
    return -1;
  }

  if (ReadTlvInfo())
    return readSize;
  else
    return -1;
}

bool PatchDLManager::ReadTlvInfo() {
  int nvm_length, nvm_tot_len, nvm_index, i;
  bool status = false;
  unsigned short nvm_tag_len;
  tlv_patch_info *ptlv_header;
  tlv_nvm_hdr *nvm_ptr;
  unsigned char data_buf[PRINT_BUF_SIZE] = { 0, };
  unsigned char *nvm_byte_ptr;
  ptlv_header = (tlv_patch_info*)pdata_buffer_;
  /* checking for type of patch file */
  if (pdata_buffer_[0] == ELF_FLAG && !memcmp(&pdata_buffer_[1], "ELF", 3)) {
   tlv_type_ = ELF_TYPE_PATCH;
  } else {
    /* To handle different event between rampatch and NVM */
    tlv_type_ = ptlv_header->tlv_type;
    tlv_dwn_cfg_ = ptlv_header->tlv.patch.dwnd_cfg;
  }

  if (tlv_type_ == ELF_TYPE_PATCH) {
    ALOGI("====================================================");
    ALOGI("ELF Type Patch File\t\t\t : 0x%x %c %c %c", pdata_buffer_[0], pdata_buffer_[1],
          pdata_buffer_[2], pdata_buffer_[3]);
    ALOGI("File Class\t\t\t : 0x%x", pdata_buffer_[4]);
    ALOGI("Data Encoding\t\t\t : 0x%x", pdata_buffer_[5]);
    ALOGI("File version\t\t\t : 0x%x", pdata_buffer_[6]);
    ALOGI("====================================================");
    status = true;
  } else if (ptlv_header->tlv_type == TLV_TYPE_PATCH ||
              ptlv_header->tlv_type == TLV_TYPE_PATCH_XMEM) {
    ALOGI("====================================================");
    ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
    ALOGI("Length\t\t\t : %d bytes", (ptlv_header->tlv_length1) |
          (ptlv_header->tlv_length2 << 8) |
          (ptlv_header->tlv_length3 << 16));
    ALOGI("Total Length\t\t\t : %d bytes", ptlv_header->tlv.patch.tlv_data_len);
    ALOGI("Patch Data Length\t\t\t : %d bytes", ptlv_header->tlv.patch.tlv_patch_data_len);
    ALOGI("Signing Format Version\t : 0x%x", ptlv_header->tlv.patch.sign_ver);
    ALOGI("Signature Algorithm\t\t : 0x%x", ptlv_header->tlv.patch.sign_algorithm);
    ALOGI("Event Handling\t\t\t : 0x%x", ptlv_header->tlv.patch.dwnd_cfg);
    ALOGI("Reserved\t\t\t : 0x%x", ptlv_header->tlv.patch.reserved1);
    ALOGI("Product ID\t\t\t : 0x%04x\n", ptlv_header->tlv.patch.prod_id);
    ALOGI("Rom Build Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.build_ver);
    ALOGI("Patch Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.patch_ver);
    ALOGI("Reserved\t\t\t : 0x%x\n", ptlv_header->tlv.patch.reserved2);
    ALOGI("Patch Entry Address\t\t : 0x%x\n", (ptlv_header->tlv.patch.patch_entry_addr));
    ALOGI("====================================================");
    status = true;
  } else if ( (ptlv_header->tlv_type >= TLV_TYPE_BT_NVM) &&
              ((ptlv_header->tlv_type <= TLV_TYPE_BT_FM_NVM)) ) {
    ALOGI("====================================================");
    ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
    nvm_tot_len = nvm_length  = (ptlv_header->tlv_length1) |
                                (ptlv_header->tlv_length2 << 8) |
                                (ptlv_header->tlv_length3 << 16);
    ALOGI("Length\t\t\t : %d bytes",  nvm_tot_len);
    ALOGI("====================================================");

    if (nvm_tot_len <= 0)
      return status;

    if (ptlv_header->tlv_type == TLV_TYPE_BT_FM_NVM) {
      nvm_byte_ptr = (unsigned char*)ptlv_header;
      nvm_byte_ptr += 4;
      ptlv_header = (tlv_patch_info*)nvm_byte_ptr;
      nvm_tot_len -= 4;
      ALOGI("====================================================");
      ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
      nvm_length = (ptlv_header->tlv_length1) |
                   (ptlv_header->tlv_length2 << 8) |
                   (ptlv_header->tlv_length3 << 16);
      ALOGI("Length\t\t\t : %d bytes",  nvm_length);
      ALOGI("====================================================");
    }

 multi_nvm:
    for (nvm_byte_ptr = (unsigned char*)(nvm_ptr = &(ptlv_header->tlv.nvm)), nvm_index = 0;
         nvm_index < nvm_length; nvm_ptr = (tlv_nvm_hdr*)nvm_byte_ptr) {
      ALOGV("TAG ID\t\t\t : %d", nvm_ptr->tag_id);
      nvm_tag_len = nvm_ptr->tag_len;
      ALOGV("TAG Length\t\t : %d", nvm_tag_len);
      ALOGV("TAG Pointer\t\t : %d", nvm_ptr->tag_ptr);
      ALOGV("TAG Extended Flag\t : %d", nvm_ptr->tag_ex_flag);

      /* Increase nvm_index to NVM data */
      nvm_index += sizeof(tlv_nvm_hdr);
      nvm_byte_ptr += sizeof(tlv_nvm_hdr);
      if (ptlv_header->tlv_type == TLV_TYPE_BT_NVM) {
        /* Write BD Address */
        if (nvm_ptr->tag_id == TAG_NUM_2) {
          memcpy(nvm_byte_ptr, vnd_local_bd_addr_, 6);
          ALOGI("BD Address: %.02x:%.02x:%.02x:%.02x:%.02x:%.02x",
                *nvm_byte_ptr, *(nvm_byte_ptr + 1), *(nvm_byte_ptr + 2),
                *(nvm_byte_ptr + 3), *(nvm_byte_ptr + 4), *(nvm_byte_ptr + 5));
        }

        if (IS_NEWER_THAN_CHEROKEE_2_1(chipset_ver_)) {
          UpdateNewNvmFormat(nvm_ptr);
        } else {
          /* Change SIBS setting */
          if (!IsSibsEnabled()) {
            if (nvm_ptr->tag_id == 17) {
              *nvm_byte_ptr = ((*nvm_byte_ptr) & ~(0x80));
            }

            if (nvm_ptr->tag_id == 27) {
              *nvm_byte_ptr = ((*nvm_byte_ptr) & ~(0x01));
            }
          } else {
            if (nvm_ptr->tag_id == 17) {
              /* check for wakeup config */
              CheckForWakeupMechanism(nvm_byte_ptr, WAKEUP_CONFIG_BYTE_CHE_2_AND_BEFORE);
            }
          }

          /* Change Baudrate setting based on Host setting */
          if (nvm_ptr->tag_id == 17) {
            *(nvm_byte_ptr + 2) = uart_transport_->GetMaxBaudrate();
          }
        }
      }
      for (i = 0; (i < nvm_ptr->tag_len && (i * 3 + 2) < PRINT_BUF_SIZE); i++)
        snprintf((char*)data_buf, PRINT_BUF_SIZE, "%s%.02x ", (char*)data_buf, *(nvm_byte_ptr + i));

      ALOGV("TAG Data\t\t\t : %s", data_buf);

      /* Clear buffer */
      memset(data_buf, 0x0, PRINT_BUF_SIZE);

      /* increased by tag_len */
      nvm_index += nvm_ptr->tag_len;
      nvm_byte_ptr += nvm_ptr->tag_len;
    }

    nvm_tot_len -= nvm_index;

    if (nvm_tot_len > 4 ) {
      nvm_byte_ptr = (unsigned char*)ptlv_header;
      nvm_byte_ptr += (4 + nvm_index);
      ptlv_header = (tlv_patch_info*)nvm_byte_ptr;
      nvm_tot_len -= 4;
      ALOGI("====================================================");
      ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
      nvm_length = (ptlv_header->tlv_length1) |
                   (ptlv_header->tlv_length2 << 8) |
                   (ptlv_header->tlv_length3 << 16);
      ALOGI("Length\t\t\t : %d bytes",  nvm_length);
      ALOGI("====================================================");
      goto multi_nvm;
    }

    ALOGI("====================================================");
    status = true;

  } else {
    ALOGE("TLV Header type is unknown (%d) ", ptlv_header->tlv_type);
  }

  return status;
}

FILE* PatchDLManager::OpenPatchFile(const char *file_path, const char* alt_file_path) {
  FILE *pFile = NULL;

  if (!(file_path && (pFile = fopen( file_path, "r" )))) {
    ALOGE("%s File Open Fail %s (%d)", file_path, strerror(errno), errno);
    //Try opening from alternate path
    if (!(alt_file_path && (pFile = fopen(alt_file_path, "r")))) {
      ALOGE("%s File Opening from alternate path: Fail %s (%d)", alt_file_path,
            strerror(errno), errno);
      return NULL;
    } else {
      ALOGI("File open %s succeeded", alt_file_path);
      return pFile;
    }
  } else {
    ALOGI("File open %s succeeded", file_path);
    return pFile;
  }
}

int PatchDLManager::TlvDnldSegment(int index, int seg_size, unsigned char wait_cc_evt)
{
  int size = 0, err = -1;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  ALOGV("%s: Downloading TLV Patch segment no.%d, size:%d", __func__, index, seg_size);

  /* Frame the HCI CMD PKT to be sent to Controller*/
  FrameHciPkt(cmd, EDL_PATCH_TLV_REQ_CMD, 0, index, seg_size);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

  /* Initialize the RSP packet everytime to 0 */
  memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);

  if ( err != size) {
    ALOGE("Failed to send the patch payload to the Controller! 0x%x", err);
    return err;
  }

  if (!unified_hci) {
    if (wait_cc_evt) {
      err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if ( err < 0) {
        ALOGE("%s: Failed to downlaod patch segment: %d!",  __func__, index);
        return err;
      }
    }
  }
  ALOGV("%s: Successfully downloaded patch segment: %d", __func__, index);
  return err;
}

int PatchDLManager::TlvDnldReq(int tlv_size)
{
  int total_segment, remain_size, i, err = -1;
  unsigned char wait_cc_evt = true;
  bool is_last_seg = false;
  int segment_download_len = MAX_SIZE_PER_TLV_SEGMENT;

  total_segment = tlv_size / MAX_SIZE_PER_TLV_SEGMENT;
  remain_size = (tlv_size < MAX_SIZE_PER_TLV_SEGMENT) ? \
                tlv_size : (tlv_size % MAX_SIZE_PER_TLV_SEGMENT);

  ALOGI("%s: TLV size: %d, Total Seg num: %d, remain size: %d",
        __func__, tlv_size, total_segment, remain_size);

  if (tlv_type_ == TLV_TYPE_PATCH || tlv_type_ == ELF_TYPE_PATCH
       || tlv_type_ == TLV_TYPE_PATCH_XMEM) {

    /* Prior to Rome version 3.2(including inital few rampatch release of Rome 3.2), the event
     * handling mechanism is SKIP_EVT_NONE. After few release of rampatch for Rome 3.2, the
     * mechamism is changed to SKIP_EVT_VSE_CC. Rest of the mechanism is not used for now
     */
    switch (tlv_dwn_cfg_) {
      case SKIP_EVT_NONE:
        wait_vsc_evt_ = true;
        wait_cc_evt = true;
        ALOGI("Event handling type: SKIP_EVT_NONE");
        break;
      case SKIP_EVT_VSE_CC:
        wait_vsc_evt_ = false;
        wait_cc_evt = false;
        ALOGI("Event handling type: SKIP_EVT_VSE_CC");
        break;
      /* Not handled for now */
      case SKIP_EVT_VSE:
      case SKIP_EVT_CC:
      default:
        ALOGE("Unsupported Event handling: %d", tlv_dwn_cfg_);
        break;
    }
  } else {
    wait_vsc_evt_ = true;
    wait_cc_evt = true;
  }

  for (i = 0; i <= total_segment && !is_last_seg; i++) {
    /* check for last segment based on remaining size
     * and total number of segments.
     */
    if ((remain_size && i == total_segment) ||
        (!remain_size && (i + 1) == total_segment)) {
      is_last_seg = true;
      // Update segment download len if last segment is being downloaded
      if (remain_size)
        segment_download_len = remain_size;
      ALOGD("%s: Updating seg len to %d as last segment",
        __func__, segment_download_len);
    }

    if ((tlv_type_ == TLV_TYPE_PATCH || tlv_type_ == TLV_TYPE_PATCH_XMEM
          || tlv_type_ == ELF_TYPE_PATCH) && is_last_seg) {

      /*
       * 1. None of the command segments receive CCE
       * 2. No command segments receive VSE except the last one
       * 3. If tlv_dwn_cfg_ is SKIP_EVT_NONE then wait for VSE and CCE
       * ( except CCE is not received for last segment)
       */
        wait_cc_evt = false;
        wait_vsc_evt_ = true;
    }

    patch_dnld_pending_ = true;
    if ((err = TlvDnldSegment(i, segment_download_len, wait_cc_evt )) < 0) {
      bt_logger_->SetInitFailureReason(BT_HOST_REASON_TLV_DOWNLOAD_FAILED);
      goto error;
    }
    patch_dnld_pending_ = false;
  }

 error:
  if (patch_dnld_pending_)
    patch_dnld_pending_ = false;
  return err;
}

void PatchDLManager::FrameHciCmdPkt(
  unsigned char *cmd,
  int edl_cmd, unsigned int p_base_addr,
  int segtNo, int size
)
{
  int offset = 0;
  hci_command_hdr *cmd_hdr;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);

  cmd[0]      = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_PATCH_CMD_OCF);
  cmd_hdr->plen   = size;
  cmd[4]      = edl_cmd;

  switch (edl_cmd) {
    case EDL_PATCH_SET_REQ_CMD:
      /* Copy the patch header info as CMD params */
      memcpy(&cmd[5], phdr_buffer_, PATCH_HDR_LEN);
      ALOGD("%s: Sending EDL_PATCH_SET_REQ_CMD", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_DLD_REQ_CMD:
      offset = ((segtNo - 1) * MAX_DATA_PER_SEGMENT);
      p_base_addr += offset;
      cmd_hdr->plen   = (size + 6);
      cmd[5]  = (size + 4);
      cmd[6]  = EXTRACT_BYTE(p_base_addr, 0);
      cmd[7]  = EXTRACT_BYTE(p_base_addr, 1);
      cmd[8]  = EXTRACT_BYTE(p_base_addr, 2);
      cmd[9]  = EXTRACT_BYTE(p_base_addr, 3);
      memcpy(&cmd[10], (pdata_buffer_ + offset), size);

      ALOGD("%s: Sending EDL_PATCH_DLD_REQ_CMD: size: %d bytes",
            __func__, size);
      ALOGD("HCI-CMD %d:\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t"
            "0x%x\t0x%x\t0x%x\t\n", segtNo, cmd[0], cmd[1], cmd[2],
            cmd[3], cmd[4], cmd[5], cmd[6], cmd[7], cmd[8], cmd[9]);
      break;
    case EDL_PATCH_ATCH_REQ_CMD:
      ALOGD("%s: Sending EDL_PATCH_ATTACH_REQ_CMD", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_RST_REQ_CMD:
      ALOGD("%s: Sending EDL_PATCH_RESET_REQ_CMD", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_VER_REQ_CMD:
      ALOGD("%s: Sending EDL_PATCH_VER_REQ_CMD", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_TLV_REQ_CMD:
      ALOGD("%s: Sending EDL_PATCH_TLV_REQ_CMD", __func__);
      /* Parameter Total Length */
      cmd[3] = size + 2;

      /* TLV Segment Length */
      cmd[5] = size;
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
      offset = (segtNo * MAX_SIZE_PER_TLV_SEGMENT);
      memcpy(&cmd[6], (pdata_buffer_ + offset), size);
      break;
    case EDL_GET_BUILD_INFO:
      ALOGD("%s: Sending EDL_GET_BUILD_INFO", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_GET_BOARD_ID:
      ALOGD("%s: Sending EDL_GET_BOARD_ID", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    default:
      ALOGE("%s: Unknown EDL CMD !!!", __func__);
  }
}

int PatchDLManager::GetBoardIdReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  bool cmd_supported = true;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_GETBOARDID_CMD_STUCK,
                                            "GET BOARDID CMD & RSP");
  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciCmdPkt(cmd, EDL_GET_BOARD_ID, 0,
                    -1, EDL_PATCH_CMD_LEN);
  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

  ALOGI("%s: Sending EDL_GET_BOARD_ID", __func__);
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send EDL_GET_BOARD_ID command!");
    cmd_supported = false;
  }

  if (!unified_hci) {
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if (err < 0) {
      ALOGE("%s: Failed to get feature request", __func__);
    }
  }

  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return (cmd_supported == true ? err : -1);
}

int PatchDLManager::EnableSecureBridge()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  struct timeval tv;

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_SECURE_BRIDGE_CMD_STUCK);
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_SECURE_BRIDGE_CMD_STUCK,
                                            "GET SECURE BRIDGE CMD & RSP");

  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciPkt(cmd, EDL_SECURE_BRIDGE_CMD, 0, -1, EDL_SECURE_BRIDGE_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_SECURE_BRIDGE_CMD_LEN);
  ALOGD("%s: Sending enable secure bridge CMD to SOC", __func__);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("%s: Failed to send enable secure bridge cmd", __func__);
    err = -1;
  }
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;
}

bool PatchDLManager::IsXmemDownload(int* tlv_size)
{
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  DataHandler * instance = DataHandler::Get();
  auto itr = PatchPathInfoMap_.find(chipset_ver_);

  if (itr != PatchPathInfoMap_.end() && itr->second &&
      instance && instance->IsXMEMEnabled()) {
    BtState :: Get()->SetXmemEnabledFlag();
    FormRegularPaths(itr->second, true);
    gettimeofday(&tv, NULL);
    BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Opening and reading XMEM patch file");
    BtState :: Get()->SetPreXMEMPatchOpenTS(dst_buff);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_RAM_PATCH_READ_STUCK,
                                              "XMEM RAM PATCH FILE OPEN & READ");
    *tlv_size = GetXmemTlvFile(xmem_rampatch_file_path, rampatch_file_path,
                                     rampatch_alt_file_path);
    gettimeofday(&tv, NULL);
    bt_logger_->CheckAndAddToDelayList(&tv);
    if (*tlv_size > 0)
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done opening and reading XMEM patch file");
    else
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Failed in opening and reading XMEM patch file");
    BtState :: Get()->SetPostXMEMPatchOpenTS(dst_buff);
    if (*tlv_size > 0 && EnableSecureBridge() != -1
        && secure_bridge_enabled) {
      return true;
    } else {
      FormRegularPaths(itr->second, false);
      return false;
    }
  } else {
    return false;
  }

}

int PatchDLManager::DownloadTlvFile()
{
  int tlv_size = -1;
  int err = -1;
  char nvm_file_path_bid[256] = { 0, };
  char nvm_alt_file_path_bid[256] = { 0, };
  int nvm_file_path_len = strlen(nvm_file_path);
  int nvm_alt_file_path_len = 0;
  int xmem_nvm_file_path_len = 0;
  int board_id_cmd_status = -1;
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  /* Rampatch TLV file Downloading */
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_RAM_PATCH_READ_STUCK,
                                            "RAM PATCH OPEN & READ");
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Opening and reading patch file");
  BtState :: Get()->SetPrePatchOpenTS(dst_buff);
  if ((tlv_size = GetTlvFile(rampatch_file_path, rampatch_alt_file_path)) > 0) {
    gettimeofday(&tv, NULL);
    BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done opening and reading patch file");
    BtState :: Get()->SetPostPatchOpenTS(dst_buff);
    bt_logger_->CheckAndAddToDelayList(&tv);
    if (tlv_type_ == ELF_TYPE_PATCH) {
      ReadELFConfig();
      if (strcmp(elf_config_, "no_value") != 0) {
        err = PatchConfigReq();
        if (err < 0) {
          ALOGE("%s: Patch config CMD Failed", __func__);
          bt_logger_->SetInitFailureReason(BT_HOST_REASON_PATCH_CONFIG_FAILED);
          delete []pdata_buffer_;
          return err;
        } else {
          unsigned long elf_config;
          /* converting hex in string to real hex number */
          elf_config = strtoul (elf_config_, NULL, 16);
          /* Checking BIT 1:
           * BIT 1: Whether to perform Parallel (1) or Sequential 
           * validation (0)
           */
          if ((elf_config & 2) == 0) {
            if (tlv_dwn_cfg_ == SKIP_EVT_NONE)
              bt_logger_->UpdateElfPatchIdealDelay(tlv_size, false);
            else if (tlv_dwn_cfg_ == SKIP_EVT_VSE_CC)
              bt_logger_->UpdateElfPatchIdealDelay(tlv_size, true);
          }
        }
      } else {
        tlv_dwn_cfg_ = SKIP_EVT_VSE_CC;
      }
    } else {
      bt_logger_->UpdateRamPatchIdealDelay(tlv_size, tlv_dwn_cfg_ == SKIP_EVT_NONE ? false : true);
    }
    bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_PATCH_DNLD_STUCK);
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PATCH_DNLD_STUCK,
                                              "RAM PATCH DOWNLOAD");
    err = TlvDnldReq(tlv_size);
    gettimeofday(&tv, NULL);
    bt_logger_->CheckAndAddToDelayList(&tv);
    delete []pdata_buffer_;
    if (err < 0) {
      return err;
    }
  } else {
    gettimeofday(&tv, NULL);
    BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Failed in opening and reading patch file");
    BtState :: Get()->SetPostPatchOpenTS(dst_buff);
    bt_logger_->CheckAndAddToDelayList(&tv);
  }

#ifdef DEBUG_CRASH_SOC_WHILE_DLD
  {
    char trial_prop[PROPERTY_VALUE_MAX];
    int value = 0;
    bt_logger_->PropertyGet("persist.vendor.bluetooth.csoc.cnt", trial_prop, "0");
    value = atoi(trial_prop);
    ALOGV("%s: value : %d\n", __func__, value);
    if (value > 0) {
      sprintf(trial_prop, "%d", (value = value - 1));
      bt_logger_->PropertyGet("persist.vendor.bluetooth.csoc.cnt", trial_prop);
      SendCrashCommand();
    }
  }
#endif      //DEBUG_CRASH_SOC_WHILE_DLD

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_GETBOARDID_CMD_STUCK);
  if ((board_id_cmd_status = GetBoardIdReq()) < 0) {
    ALOGE("%s: failed to get board id(0x%x)", __func__, err);
  }

  tlv_size = -1;
  err = -1;
  if (IsXmemDownload(&tlv_size)) {
    ALOGI("%s: XMEM  enabled with tlv size: %d", __func__, tlv_size);
    /* Rampatch XMEM TLV(0x05) file Downloading */
    bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK);
    bt_logger_->UpdateXmemRamPatchIdealDelay(tlv_size, tlv_dwn_cfg_ == SKIP_EVT_NONE ? false : true);
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK,
                                              "XMEM RAM PATCH DOWNLOAD");
    err = TlvDnldReq(tlv_size);
    gettimeofday(&tv, NULL);
    bt_logger_->CheckAndAddToDelayList(&tv);

    delete []pdata_buffer_;
    if (err < 0) {
      return err;
    }

    //150 ms is the time required for SOC to boot up with new XMEM patch.
    usleep(150*1000);

    //set local UART baudarate to 115.2 Kbps.
    if ((err = SetUartBuadRate(USERIAL_BAUD_115200)) < 0) {
      bt_logger_->SetInitFailureReason(BT_HOST_REASON_UART_BAUDRATE_CHANGE_FAILED);
      return err;
    }

    //Local & soc uart buad rate to 3.2 Mbps.
    if ((err = SetBaudRateReq()) < 0) {
      ALOGE("%s: Baud rate change failed!", __func__);
      bt_logger_->SetInitFailureReason(BT_HOST_REASON_BAUDRATE_CHANGE_FAILED);
      return err;
    }

    tlv_size = -1;
    err = -1;
    char xmem_nvm_file_path_bid[256] = { 0, };
    nvm_file_path_len = strlen(nvm_file_path);
    nvm_alt_file_path_len = strlen(nvm_alt_file_path);
    xmem_nvm_file_path_len = strlen(xmem_nvm_file_path);

    if (board_id_cmd_status  != -1) {
      if (xmem_nvm_file_path_len != 0) {
        memcpy(xmem_nvm_file_path_bid, xmem_nvm_file_path, xmem_nvm_file_path_len - 2);
        strlcat(xmem_nvm_file_path_bid, (char*)board_id_, sizeof(xmem_nvm_file_path_bid));
      }
      if (nvm_file_path_len != 0) {
        memcpy(nvm_file_path_bid, nvm_file_path, nvm_file_path_len - 2);
        strlcat(nvm_file_path_bid, (char*)board_id_, sizeof(nvm_file_path_bid));
      }
      if (nvm_alt_file_path_len != 0) {
        memcpy(nvm_alt_file_path_bid, nvm_alt_file_path, nvm_alt_file_path_len - 2);
        strlcat(nvm_alt_file_path_bid, (char*)board_id_, sizeof(nvm_alt_file_path_bid));
      }
      gettimeofday(&tv, NULL);
      bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_NVM_PATCH_READ_STUCK,
                                                "XMEM NVM file with BID OPEN & READ");
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Opening and reading XMEM NVM file");
      BtState :: Get()->SetPreXmemNVMOpenTS(dst_buff);

      if ((tlv_size = GetXmemTlvFile(xmem_nvm_file_path_bid,
           nvm_file_path_bid, nvm_alt_file_path_bid)) < 0) {
        ALOGI("%s: %s: file doesn't exist, falling back to default file",
               __func__, nvm_file_path_bid);
        gettimeofday(&tv, NULL);
        BtState :: Get()->AddLogTag(dst_buff, tv,
          (char *)"Failed in opening and reading XMEM NVM file");
        BtState :: Get()->SetPostXmemNVMOpenTS(dst_buff);
        bt_logger_->CheckAndAddToDelayList(&tv);
      }
    }

    if (tlv_size < 0) {
      gettimeofday(&tv, NULL);
      bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_NVM_PATCH_READ_STUCK,
                                                "XMEM NVM file OPEN & READ");
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Opening and reading XMEM NVM file");
      BtState :: Get()->SetPreXmemNVMOpenTS(dst_buff);

      if ((tlv_size = GetXmemTlvFile(xmem_nvm_file_path, nvm_file_path, nvm_alt_file_path)) < 0) {
        gettimeofday(&tv, NULL);
        BtState :: Get()->AddLogTag(dst_buff, tv,
          (char *)"Failed in opening and reading XMEM NVM file");
        BtState :: Get()->SetPostXmemNVMOpenTS(dst_buff);
        bt_logger_->CheckAndAddToDelayList(&tv);
        bt_logger_->SetInitFailureReason(BT_HOST_REASON_NVM_FILE_NOT_FOUND);
        return err;
      }
    }
    gettimeofday(&tv, NULL);
    BtState :: Get()->AddLogTag(dst_buff, tv,
          (char *)"Done opening and reading XMEM NVM file");
    BtState :: Get()->SetPostXmemNVMOpenTS(dst_buff);
    bt_logger_->CheckAndAddToDelayList(&tv);

    bt_logger_->UpdateNvmPatchIdealDelay(tlv_size);
    /* XMEM NVM file Downloading */
    bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_XMEM_NVM_DNLD_STUCK);
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_XMEM_NVM_DNLD_STUCK,
                                              "XMEM NVM file download");
    err = TlvDnldReq(tlv_size);
    bt_logger_->CheckAndAddToDelayList(&tv);
  } else {
    /* NVM TLV file Downloading */
    tlv_size = -1;
    err = -1;
    nvm_alt_file_path_len = strlen(nvm_alt_file_path);
    nvm_file_path_len = strlen(nvm_file_path);

    if (board_id_cmd_status  != -1) {
      if (nvm_file_path_len != 0) {
        memcpy(nvm_file_path_bid, nvm_file_path, nvm_file_path_len - 2);
        strlcat(nvm_file_path_bid, (char*)board_id_, sizeof(nvm_file_path_bid));
      }
      if (nvm_alt_file_path_len != 0){
        memcpy(nvm_alt_file_path_bid, nvm_alt_file_path, nvm_alt_file_path_len - 2);
        strlcat(nvm_alt_file_path_bid, (char*)board_id_, sizeof(nvm_alt_file_path_bid));
      }
      gettimeofday(&tv, NULL);
      bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_NVM_PATCH_READ_STUCK,
                                                "NVM file with BID OPEN & READ");
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Opening and reading NVM file");
      BtState :: Get()->SetPreNVMOpenTS(dst_buff);
      if ((tlv_size = GetTlvFile(nvm_file_path_bid,nvm_alt_file_path_bid)) < 0) {
        ALOGI("%s: %s: file doesn't exist, falling back to default file", __func__, nvm_file_path_bid);
        gettimeofday(&tv, NULL);
        BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Failed in opening and reading NVM file");
        BtState :: Get()->SetPostNVMOpenTS(dst_buff);
        bt_logger_->CheckAndAddToDelayList(&tv);
      } else {
        gettimeofday(&tv, NULL);
        BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done opening and reading NVM file");
        BtState :: Get()->SetPostNVMOpenTS(dst_buff);
        bt_logger_->CheckAndAddToDelayList(&tv);
      }
    }

    if (tlv_size < 0) {
      gettimeofday(&tv, NULL);
      bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_NVM_PATCH_READ_STUCK,
                                                "NVM file OPEN & READ");
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Opening and reading NVM file");
      BtState :: Get()->SetPreNVMOpenTS(dst_buff);
      if ((tlv_size = GetTlvFile(nvm_file_path, nvm_alt_file_path)) < 0) {
        gettimeofday(&tv, NULL);
        BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Failed in opening and reading NVM file");
        BtState :: Get()->SetPostNVMOpenTS(dst_buff);
        bt_logger_->CheckAndAddToDelayList(&tv);
        bt_logger_->SetInitFailureReason(BT_HOST_REASON_NVM_FILE_NOT_FOUND);
        return err;
      }
      gettimeofday(&tv, NULL);
      BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done opening and reading NVM file");
      BtState :: Get()->SetPostNVMOpenTS(dst_buff);
      bt_logger_->CheckAndAddToDelayList(&tv);
    }
    bt_logger_->UpdateNvmPatchIdealDelay(tlv_size);
    bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_NVM_DNLD_STUCK);
    gettimeofday(&tv, NULL);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_NVM_DNLD_STUCK,
                                              "NVM file download");
    err = TlvDnldReq(tlv_size);
    gettimeofday(&tv, NULL);
    bt_logger_->CheckAndAddToDelayList(&tv);
  }
  delete []pdata_buffer_;
  return err;
}

/* Get addon features that are supported by FW */
SocAddOnFeatures_t* PatchDLManager::GetAddOnFeatureList()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  uint8_t param_len, cmd_status;
  struct timeval tv;

  if (add_on_features.feat_mask_len == SOC_ADD_ON_FEATURE_MASK_INVALID_LENGTH) {
    ALOGW("%s: Invalid features mask length is set, returning NULL", __func__);
    return NULL;
  } else if (add_on_features.feat_mask_len > 0) {
    return &add_on_features;
  }
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_ADDONFEAT_CMD_STUCK,
                                            "ADDONFEAT CMD & RSP");
  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_VS_GET_ADDON_FEATURES_SUPPORT);
  cmd_hdr->plen     = 0x00;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);

  ALOGD("%s: Sending HCI_VS_GET_ADDON_FEATURES_SUPPORT", __func__);
  ALOGD("HCI-CMD: \t0x%x \t0x%x \t0x%x \t0x%x", cmd[0], cmd[1], cmd[2], cmd[3]);

  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send HCI_VS_GET_ADDON_FEATURES_SUPPORT command!");
    goto error;
  }

  if (!unified_hci) {
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if (err < 0) {
      ALOGE("%s: Failed to get feature request", __func__);
      goto error;
    }
  }

  param_len = rsp[PARM_LEN_BYTE];
  cmd_status = rsp[STATUS_BYTE];

  ALOGI("%s: param_len %d, cmd_status = %d", __func__,
      param_len, cmd_status);

  if ((param_len > HCI_EVENT_PARAMS)  && (cmd_status == 0x00)) {
    PTR_TO_UINT16(add_on_features.product_id, &rsp[PRODUCT_ID_BYTE]);
    PTR_TO_UINT16(add_on_features.rsp_version, &rsp[RSP_VER_BYTE]);
    add_on_features.feat_mask_len = param_len - HCI_EVENT_PARAMS;
    ALOGI("%s: feat_mask_len %d product_id = %d, rsp_version = %d", __func__,
        add_on_features.feat_mask_len, add_on_features.product_id,
        add_on_features.rsp_version);
    memcpy(add_on_features.features, &rsp[FEAT_MASK_BYTES],
        add_on_features.feat_mask_len);
    gettimeofday(&tv, NULL);
    bt_logger_->CheckAndAddToDelayList(&tv);
    return &add_on_features;
  }

 error:
  //older fw does not return proper mask length, hence setting length to invalid
  add_on_features.feat_mask_len = SOC_ADD_ON_FEATURE_MASK_INVALID_LENGTH;
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);

  return NULL;
}

void PatchDLManager::GetOTPBytesInfo()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_OTP_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_OTP_INFO_GET_CMD_STUCK,
                                            "GET OTP INFO CMD & RSP");

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);
  /* Frame the HCI CMD to be sent to the Controller */
  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0] = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_VS_PERSIST_ACCESS_COMMAND);
  cmd_hdr->plen   = 0x02;
  cmd[4] = HCI_VS_PERSIST_ACCESS_OTP_GET;
  cmd[5] = HCI_VS_PERSIST_ACCESS_OTP_GET_CONFIG_TYPE;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + HCI_VS_PERSIST_ACCESS_CMD_LEN);

  ALOGI("%s: Sending HCI_VS_PERSIST_ACCESS_OTP_GET cmd", __func__);
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send HCI_VS_PERSIST_ACCESS_OTP_GET command!");
  }
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
}

uint64_t PatchDLManager :: GetChipVersion()
{
  return chipset_ver_;
}

void PatchDLManager::EnableControllerLog()
{
  int ret = 0;
  /* VS command to enable controller logging to the HOST. By default it is disabled */
  unsigned char cmd[6] = { 0x01, 0x17, 0xFC, 0x02, 0x00, 0x00 };
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char value[PROPERTY_VALUE_MAX] = { '\0' };
  unsigned char temp_wait_vsc_evt;
  struct timeval tv;

  bt_logger_->PropertyGet("persist.vendor.service.bdroid.soclog", value, "false");

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_ENHLOG_CMD_STUCK,
                                            "ENHANCE log CMD & RSP");

  // value at cmd[5]: 1 - to enable, 0 - to disable
  ret = (strcmp(value, "true") == 0) ? cmd[5] = 0x01 : 0;
  ALOGI("%s: %d", __func__, ret);

  temp_wait_vsc_evt = wait_vsc_evt_;
  wait_vsc_evt_ = false;
  /* only sending cmd */
  ret = HciSendVsCmd((unsigned char*)cmd, rsp, 6);
  if (ret != 6) {
    ALOGE("%s: command failed", __func__);
  }
  if (!unified_hci) {
    ret = ReadNewHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if (ret < 0) {
      ALOGE("%s: failed to read rsp", __func__);
    } else {
      bt_logger_->ProcessRx(HCI_PACKET_TYPE_EVENT, &rsp[1], ret - 1);
      /* Checking VSE rsp */
      if (rsp[1] != LOG_BT_EVT_VENDOR_SPECIFIC || rsp[2] != 1
          || rsp[3] != HCI_VS_HOST_LOG_CTRL_SUBID) {
        ALOGE("%s: Not rcvd correct VS event", __func__);
        ALOGE("%s: rsp Event type: 0x%x" , __func__, rsp[1]);
        ALOGE("%s: rsp parameter length: 0x%x" , __func__, rsp[2]);
        ALOGE("%s: rsp subopcode: 0x%x" , __func__, rsp[3]);
      }
    }
  }
  wait_vsc_evt_ = temp_wait_vsc_evt;

  ret = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if (ret < 0) {
    ALOGE("%s: Failed to get CC for enable SoC log", __func__);
  }

  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return;
}

void PatchDLManager::EnableEnhControllerLog()
{
  int ret = 0;
  struct timeval tv;
  /* VS command to enable/disable enhanced controller logging to the HOST */
  unsigned char cmd[6] = { 0x01, 0x17, 0xFC, 0x02, 0x14, 0x00 };
  unsigned char temp_wait_vsc_evt;
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char value[PROPERTY_VALUE_MAX] = { '\0' };
  unsigned char logMask = 0;

  bt_logger_->PropertyGet("persist.vendor.service.bdroid.soclog", value, "false");

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_ENHLOG_CMD_STUCK,
                                            "ENHANCE LOG CMD & RSP");

  if (strcmp(value, "true") == 0)
    logMask = 0x01;
  else
    logMask = (unsigned char)atoi(value);

  // if LSB is 0, enhanced logs are disabled
  if (logMask & 0x01)
    ALOGI("%s: enabled(%d)\n", __func__, logMask);
  else
    ALOGI("%s: disabled(%d)", __func__, logMask);
  cmd[5] = logMask;

  // Command Status not expected for enhanced logging
  temp_wait_vsc_evt = wait_vsc_evt_;
  wait_vsc_evt_ = false;

  ret = HciSendVsCmd((unsigned char*)cmd, rsp, 6);
  if (ret != 6) {
    ALOGE("%s: command failed", __func__);
  }
  wait_vsc_evt_ = temp_wait_vsc_evt;

  ret = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);

  if (ret < 0) {
   ALOGE("%s: Failed to get CC for enable enh SoC log", __func__);
  }

  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);

  return;
}

int PatchDLManager::SetBaudRateReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  int flags;
  uint8_t bt_baud_rate = uart_transport_->GetMaxBaudrate();
  struct timeval tv;

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_SETBAUDRATE_CMD_STUCK,
                                            "SETBAUDRATE CMD & RSP");

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_SET_BAUDRATE_CMD_OCF);
  cmd_hdr->plen     = VSC_SET_BAUDRATE_REQ_LEN;
  cmd[4]  = bt_baud_rate;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + VSC_SET_BAUDRATE_REQ_LEN);

  gettimeofday(&tv, NULL);
  Logger::Get()->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK, "UART FLOW OFF");

  /* Flow off during baudrate change */
  if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_OFF, &flags)) < 0) {
    ALOGE("%s: HW Flow-off error: 0x%x\n", __func__, err);
    goto error;
  }

  gettimeofday(&tv, NULL);
  Logger::Get()->CheckAndAddToDelayList(&tv);

  bt_logger_->ProcessTx(HCI_PACKET_TYPE_COMMAND, &cmd[1], size - 1);

  /* Send the HCI command packet to UART for transmission */
  err = uart_transport_->UartWrite(cmd, size);
  if (err != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, err);
    err  = -1;
    goto error;
  }

  usleep(20 * 1000);

  tcdrain(fd_transport_);
  /* Change Local UART baudrate to high speed UART */
  uart_transport_->SetBaudRate(bt_baud_rate);

  /* Check current Baudrate */
  uart_transport_->GetBaudRate();

  gettimeofday(&tv, NULL);
  Logger::Get()->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK, "UART FLOW ON");
  /* Flow on after changing local uart baudrate */
  if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_ON, &flags)) < 0) {
    ALOGE("%s: HW Flow-on error: 0x%x \n", __func__, err);
    goto error;
  }
  gettimeofday(&tv, NULL);
  Logger::Get()->CheckAndAddToDelayList(&tv);

  /* Check for response from the Controller */
  if (!unified_hci) {
    if ((err = ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE)) < 0) {
      ALOGE("%s: Failed to get HCI-VS Event from SOC", __func__);
      goto error;
    }
    ALOGV("%s: Received HCI-Vendor Specific Event from SOC", __func__);
  }

  /* Wait for command complete event */
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Baud rate rsp failed!", __func__);
    goto error;
  }

 error:
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);
  return err;

}

int PatchDLManager::SetUartBuadRate(uint8_t bt_baud_rate)
{
   int err = 0;
   int flags;
   uint32_t baud_rate_tcio;
   int baud_rate_value;
   struct timeval tv;
   int baud_rate_set;

   bool status = userial_to_tcio_baud(bt_baud_rate, &baud_rate_tcio);
   baud_rate_value = userial_tcio_baud_to_int(baud_rate_tcio);
   if (!status) {
     ALOGE("%s: invalid baud rate 0x%x", __func__, bt_baud_rate);
     err = -1;
     return err;
   }
   gettimeofday(&tv, NULL);
   Logger::Get()->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK, "UART FLOW OFF");
   /* Flow off during baudrate change */
   if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_OFF, &flags)) < 0) {
     ALOGE("%s: HW Flow-off error: 0x%x\n", __func__, err);
     goto exit;
   }
   gettimeofday(&tv, NULL);
   Logger::Get()->CheckAndAddToDelayList(&tv);

   usleep(20 * 1000);

   tcdrain(fd_transport_);
   /* Change Local UART baudrate to "bt_baud_rate"*/
   uart_transport_->SetBaudRate(bt_baud_rate);

   /* Check current Baudrate */
   baud_rate_set = uart_transport_->GetBaudRate();

   if (baud_rate_set != baud_rate_value) {
     ALOGE("%s: baudrate setting failed - actual %d expected: %d",
           __func__, baud_rate_set, baud_rate_value);
     err = -1;
     return err;
   }

   gettimeofday(&tv, NULL);
   Logger::Get()->SetCurrentactivityStartTime(tv, BT_HOST_REASON_UART_IOCTL_STUCK, "UART FLOW ON");
   /* Flow on after changing local uart baudrate */
   if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_ON, &flags)) < 0) {
     ALOGE("%s: HW Flow-on error: 0x%x \n", __func__, err);
   }

exit:
   gettimeofday(&tv, NULL);
   Logger::Get()->CheckAndAddToDelayList(&tv);

   return err;
}

int PatchDLManager::SocInit(uint8_t *bdaddr, bool is_emb_wp_mode)
{
  int err = -1;
  PatchPathManager* info = NULL;
  struct timeval tv;
  UNUSED(bdaddr);
  UNUSED(is_emb_wp_mode);
  ALOGI(" %s ", __func__);

  dnld_fd_in_progress_ = fd_transport_;
  /* If wipower charging is going on in embedded mode then start hand off req */
  if (wipower_flag_ == WIPOWER_IN_EMBEDDED_MODE && wipower_handoff_ready_ != NON_WIPOWER_MODE) {
    wipower_flag_ = 0;
    wipower_handoff_ready_ = 0;
    if ((err = WipowerForwardHandoffReq()) < 0) {
      ALOGI("%s: Wipower handoff failed (0x%x)", __func__, err);
    }
  }

  /* Get Chipset version information */
  if ((err = PatchVerReq()) < 0) {
    ALOGE("%s: Fail to get chipset Version (0x%x)", __func__, err);
    dnld_fd_in_progress_ = -1;
    bt_logger_->SetInitFailureReason(BT_HOST_REASON_GETVER_CMD_FAILED);
    return err;
  }
  ALOGI("%s: Chipset Version (0x%16llx)", __func__,
    (unsigned long long)chipset_ver_);

  auto itr = PatchPathInfoMap_.find(chipset_ver_);
  if (itr != PatchPathInfoMap_.end())
    info = itr->second;
  if (info && !info->GetChipName().empty()) {
    FormRegularPaths(info, false);
  } else {
    FormDefaultPaths();
  }

  if (chipset_ver_ == MOSELLE_VER_1_1 || chipset_ver_ == MOSELLE_VER_1_0) {
    GetAppVerCmd();
    if (is_mode_change_needed)
      //send EDL mode change cmd
      EdlModeChange();
  }

  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_SETBAUDRATE_CMD_STUCK);
  /* Change baud rate 115.2 kbps to 3Mbps*/

  err = SetBaudRateReq();
  if (err < 0) {
    ALOGE("%s: Baud rate change failed!", __func__);
    dnld_fd_in_progress_ = -1;
    bt_logger_->SetInitFailureReason(BT_HOST_REASON_BAUDRATE_CHANGE_FAILED);
    return err;
  }

  ALOGI("%s: Baud rate changed successfully ", __func__);
  /* Donwload TLV files (rampatch, NVM) */
  err = DownloadTlvFile();
  if (err < 0) {
    ALOGE("%s: Download TLV file failed!", __func__);
    dnld_fd_in_progress_ = -1;
    return err;
  }
  ALOGI("%s: Download TLV file successfully ", __func__);

  /* Get SU FM label information */
  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_GETBLDINFO_CMD_STUCK);
  if ((err = GetBuildInfoReq()) < 0) {
    ALOGI("%s: Fail to get FW SU Build info (0x%x)", __func__, err);
    dnld_fd_in_progress_ = -1;
    bt_logger_->SetInitFailureReason(BT_HOST_REASON_FW_BUILD_INFO_CMD_FAILED);
    return err;
  }

  /* Disable internal LDO to use external LDO instead*/
  err = DisableInternalLdo();

  /*  get chipset supported feature request */
  ALOGV("%s: chipset_ver_: 0x%16llx Calling get addon feature",__func__,
    (unsigned long long)chipset_ver_);
  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_ADDONFEAT_CMD_STUCK);
  GetAddOnFeatureList();

  //Below cmd is only supported by SoC which have HCI Unified command interface support.
  if (unified_hci) {
    bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_OTP_INFO_GET_CMD_STUCK);
    GetOTPBytesInfo();
  }

  /* send enhanced controller logging cmd only for non rome
     chipsets as this command is supported only for them
  */
  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_ENHLOG_CMD_STUCK);

  if(soc_type_ == BT_SOC_ROME) {
    EnableControllerLog();
  } else {
    EnableEnhControllerLog();
  }

  /* Send HCI Reset */
  bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_HCI_RESET_STUCK);
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_HCI_RESET_STUCK,
                                            "HCI RESET CMD & RSP");
  err = HciReset();
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);

  if ( err < 0 ) {
    ALOGE("HCI Reset Failed !!");
    bt_logger_->SetInitFailureReason(BT_HOST_REASON_HCI_RESET_CMD_FAILED);
  } else {
    ALOGV("HCI Reset is done\n");
  }

  dnld_fd_in_progress_ = -1;
  return err;
}

void PatchDLManager::SetRampatchRegularPaths(std::string& tlv_name)
{
  snprintf(rampatch_file_path, sizeof(rampatch_file_path), "%s%s%s",
           BT_FW_REGULAR_PATH, alt_path_for_fw, tlv_name.c_str());
  snprintf(rampatch_alt_path_qti_check, sizeof(rampatch_alt_path_qti_check), "%s%s%s",
           BT_FW_QTI_ALT_PATH, alt_path_for_fw, tlv_name.c_str());
  snprintf(rampatch_alt_path_check, sizeof(rampatch_alt_path_check), "%s%s%s",
           BT_FW_ALT_PATH, alt_path_for_fw, tlv_name.c_str());
}

void PatchDLManager::SetNVMRegularPaths(std::string& nvm_name)
{
  snprintf(nvm_file_path, sizeof(nvm_file_path), "%s%s%s", BT_FW_REGULAR_PATH,
           alt_path_for_fw, nvm_name.c_str());
  snprintf(nvm_alt_path_qti_check, sizeof(nvm_alt_path_qti_check), "%s%s%s",
           BT_FW_QTI_ALT_PATH, alt_path_for_fw, nvm_name.c_str());
  snprintf(nvm_alt_path_check, sizeof(nvm_alt_path_check), "%s%s%s",
           BT_FW_ALT_PATH, alt_path_for_fw, nvm_name.c_str());
}

int PatchDLManager::CheckAndSetProperRampatchAltPath()
{
  struct stat sts;
  if (!stat(rampatch_alt_path_qti_check, &sts) && sts.st_size > 0) {
    strlcpy(rampatch_alt_file_path, rampatch_alt_path_qti_check,
            sizeof(rampatch_alt_file_path));
  } else if (!stat(rampatch_alt_path_check, &sts) && sts.st_size > 0) {
    strlcpy(rampatch_alt_file_path, rampatch_alt_path_check,
            sizeof(rampatch_alt_file_path));
  } else {
    ALOGE("%s: %s file absent", __func__, rampatch_alt_path_qti_check);
    ALOGE("%s: %s file absent", __func__, rampatch_alt_path_check);
    memset(rampatch_alt_path_qti_check, 0, sizeof(rampatch_alt_path_qti_check));
    memset(rampatch_alt_path_check, 0, sizeof(rampatch_alt_path_check));
    return -1;
  }
  return 0;
}

void PatchDLManager::FormRegularPaths(PatchPathManager * info, bool is_xmem)
{
  struct stat sts;
  std::string tlv_name;
  std::string nvm_name;
  XmemPatchPathManager * xmem_info = NULL;
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  if (!is_alt_path_for_fw_to_be_used) {
    /* Property value should be of format "btfw_x" where x is alphanumeric
     * and x can have max size of 2 and min size of 1 char(s).
     */
    ALOGI("%s: Getting value of persist.vendor.bluetooth.alt_path_for_fw",
           __func__);

    bt_logger_->PropertyGet("persist.vendor.bluetooth.alt_path_for_fw", alt_path_for_fw, "");

    is_alt_path_for_fw_to_be_used = true;
    bool valid = false;
    int prop_size = strlen(alt_path_for_fw);
    if (prop_size == 6 || prop_size == 7) {
      ALOGI("%s: persist.vendor.bluetooth.alt_path_for_fw is set to %s",
             __func__, alt_path_for_fw);
      valid = std::regex_match(alt_path_for_fw,
                     std::regex("(btfw_(([a-z1-9][a-z1-9])|[a-z1-9]))"));
      if (valid)
        strlcat(alt_path_for_fw, "/", PROPERTY_VALUE_MAX);
    }
    if (!valid) {
      if (prop_size == 0) {
        ALOGI("%s: persist.vendor.bluetooth.alt_path_for_fw not set!", __func__);
      } else {
        ALOGE("%s: persist.vendor.bluetooth.alt_path_for_fw "
              "property value = (%s) have invalid format", __func__, alt_path_for_fw);
        ALOGE("%s: Falling back to default path", __func__);
        is_alt_path_for_fw_to_be_used = false;
        memset(alt_path_for_fw, '\0', sizeof(alt_path_for_fw));
      }
    }
  }

  if (is_xmem && info->GetPatchType() == "XMEM" &&
     (xmem_info = (XmemPatchPathManager *)(info))) {
    tlv_name = xmem_info->GetXmemTLVName();
    nvm_name = xmem_info->GetXmemNVMName();
    snprintf(xmem_rampatch_file_path, sizeof(xmem_rampatch_file_path), "%s%s%s",
             BT_FW_XMEM_DEFAULT_PATH, alt_path_for_fw, tlv_name.c_str());
    snprintf(xmem_nvm_file_path, sizeof(xmem_nvm_file_path), "%s%s%s",
             BT_FW_XMEM_DEFAULT_PATH, alt_path_for_fw, nvm_name.c_str());
  } else {
     tlv_name = info->GetTLVName();
     nvm_name = info->GetNVMName();
  }

  SetRampatchRegularPaths(tlv_name);
  SetNVMRegularPaths(nvm_name);

  ALOGD("%s: Checking and Setting proper Rampatch and NVM path", __func__);
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK,
                                            "Setting regular Rampatch and NVM path");
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Setting proper Rampatch and NVM path");
  BtState :: Get()->SetPreCheckRampatchPathTS(dst_buff);

  if (CheckAndSetProperRampatchAltPath() == -1) {
    memset(rampatch_alt_file_path, 0, sizeof(rampatch_alt_file_path));
    if (info->GetPatchType() == "ELF" && stat(rampatch_file_path, &sts) == -1){
      ALOGE("%s: %s file absent", __func__, rampatch_file_path);
      ALOGE("%s: ELF(mbn) patch file %s is absent "
            "falling back to tlv file", __func__, tlv_name.c_str());
      memset(rampatch_file_path, 0, sizeof(rampatch_file_path));
      ELFPatchPathManager * elf_info = (ELFPatchPathManager *)(info);
      tlv_name = elf_info->GetAlternatePatchFileName();
      SetRampatchRegularPaths(tlv_name);
      CheckAndSetProperRampatchAltPath();
    }
  }
  if (!stat(nvm_alt_path_qti_check, &sts) && sts.st_size > 0) {
    strlcpy(nvm_alt_file_path, nvm_alt_path_qti_check, sizeof(nvm_alt_file_path));
  } else if (!stat(nvm_alt_path_check, &sts) && sts.st_size > 0) {
    strlcpy(nvm_alt_file_path, nvm_alt_path_check, sizeof(nvm_alt_file_path));
  } else {
    memset(nvm_alt_file_path, 0, sizeof(nvm_alt_file_path));
  }
  ALOGD("%s: Done checking and setting proper Rampatch and NVM path", __func__);
  gettimeofday(&tv, NULL);
  BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done setting proper rampatch and NVM path");
  BtState :: Get()->SetPostCheckRampatchPathTS(dst_buff);
  bt_logger_->CheckAndAddToDelayList(&tv);
}

void PatchDLManager::FormDefaultPaths()
{
  struct stat sts;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK,
                                            "Setting default Rampatch and NVM path");

  strlcpy(rampatch_file_path, TEMP_RAMPATCH_TLV_0_0_0_PATH, sizeof(rampatch_file_path));
  strlcpy(nvm_file_path, TEMP_NVM_TLV_0_0_0_PATH, sizeof(nvm_file_path));
  if (!stat(TEMP_RAMPATCH_TLV_0_0_0_PATH_ALT_QTI, &sts) && sts.st_size > 0) {
    strlcpy(rampatch_alt_file_path, TEMP_RAMPATCH_TLV_0_0_0_PATH_ALT_QTI,
            sizeof(rampatch_alt_file_path));
  } else if (!stat(TEMP_RAMPATCH_TLV_0_0_0_PATH_ALT, &sts) && sts.st_size > 0) {
    strlcpy(rampatch_alt_file_path, TEMP_RAMPATCH_TLV_0_0_0_PATH_ALT,
            sizeof(rampatch_alt_file_path));
  }
  if (!stat(TEMP_NVM_TLV_0_0_0_PATH_ALT_QTI, &sts) && sts.st_size > 0) {
    strlcpy(nvm_alt_file_path, TEMP_NVM_TLV_0_0_0_PATH_ALT_QTI, sizeof(nvm_alt_file_path));
  } else if (!stat(TEMP_NVM_TLV_0_0_0_PATH_ALT, &sts) && sts.st_size > 0) {
    strlcpy(nvm_alt_file_path, TEMP_NVM_TLV_0_0_0_PATH_ALT, sizeof(nvm_alt_file_path));
  }
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);

}

int PatchDLManager::HciReset()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
#ifdef DUMP_RINGBUF_LOG
  DataHandler *data_handler = DataHandler::Get();
#endif

  ALOGI("%s: HCI RESET ", __func__);

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = HCI_RESET;
  cmd_hdr->plen   = 0;

  /* Total length of the packet to be sent to the Controller */
#ifdef DUMP_RINGBUF_LOG
  gettimeofday(&data_handler->time_hci_cmd_arrived_, NULL);
  snprintf(data_handler->last_hci_cmd_timestamp_.opcode, OPCODE_LEN, "0x%x%x", cmd[0], cmd[1]);
  data_handler->AddHciCommandTag(data_handler->last_hci_cmd_timestamp_.hcicmd_timestamp,
                     data_handler->time_hci_cmd_arrived_, data_handler->last_hci_cmd_timestamp_.opcode);
  bt_logger_->SaveLastStackHciCommand(data_handler->last_hci_cmd_timestamp_.hcicmd_timestamp);
#endif

  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);
  bt_logger_->ProcessTx(HCI_PACKET_TYPE_COMMAND, &cmd[1], size - 1);
  err = uart_transport_->UartWrite(cmd, size);
  if (err != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, err);
    err = -1;
    goto error;
  }
  /* Wait for command complete event */
  err = ReadCmdCmplEvent(rsp, HCI_MAX_EVENT_SIZE);
  if (err < 0) {
    ALOGE("%s: Failed to read rsp for HCI Reset command", __func__);
    goto error;
  }

 error:
  return err;

}

int PatchDLManager::ReadNewHciEvent(unsigned char* buf, int size)
{
  int retval;
  unsigned char protocol_byte;
  unsigned char hdr[BT_EVT_HDR_SIZE];
  unsigned char packet_len;
  unsigned short tot_len;
  unsigned char* tmp_buf = NULL;

  ALOGV("%s: >", __func__);

  if (size < BT_ACL_HDR_SIZE) {
    ALOGE("%s: Invalid size: %d\n", __func__, size);
    return -1;
  }

  do {
    retval = uart_transport_->Read(&protocol_byte, 1);
    if (retval < 0) {
      ALOGE("%s: read error", __func__);
      return -1;
    }
    if (protocol_byte == LOG_BT_EVT_PACKET_TYPE) {
      break;
    } else if (protocol_byte == LOG_BT_ACL_PACKET_TYPE) {
      /* Check and parse ACL data if received during init phase to prevent init failures */
      retval = uart_transport_->Read(buf, BT_ACL_HDR_SIZE);
      if (retval < 0) {
        ALOGE("%s: read error", __func__);
        return -1;
      }

      PTR_TO_UINT16(tot_len, &buf[BT_ACL_HDR_LEN_OFFSET]);
      tmp_buf = new (std::nothrow)unsigned char[tot_len + BT_ACL_HDR_SIZE];
      if (tmp_buf == NULL) {
        ALOGE("%s: Heap memory allocation failed ", __func__);
        return -1;
      }
      memcpy(tmp_buf, buf, BT_ACL_HDR_SIZE);
      retval = uart_transport_->Read(tmp_buf + BT_ACL_HDR_SIZE, tot_len);
      if (retval < 0) {
        ALOGE("%s: read error", __func__);
        if (tmp_buf != NULL) {
          delete [] tmp_buf;
          tmp_buf = NULL;
        }
        return -1;
      }

      ALOGD("%s: Number of bytes of ACL data(header + payload) read = %d",
            __func__, tot_len + BT_ACL_HDR_SIZE);

      bt_logger_->ProcessRx(HCI_PACKET_TYPE_ACL_DATA, tmp_buf, tot_len + BT_ACL_HDR_SIZE);
      if (tmp_buf != NULL) {
        delete [] tmp_buf;
        tmp_buf = NULL;
      }

    } else if (protocol_byte == 0xFD) {
      ALOGI("%s: Got FD , responding with FC", __func__);
      uint8_t wake_byte = 0xFC;
      uart_transport_->UartWrite(&wake_byte, 1);
    } else {
      ALOGI("%s: Got an invalid proto byte: %d", __func__, protocol_byte);
    }
  } while (1);

  retval = uart_transport_->Read(hdr, BT_EVT_HDR_SIZE);
  if (retval < 0) {
    ALOGE("%s:error in reading hdr: %d", __func__, retval);
    return -1;
  }
  ALOGV("read scucesssssfully HDR");
  packet_len = hdr[BT_EVT_HDR_LEN_OFFSET];
  ALOGV("packet_len: %d\n", packet_len);

  buf[0] = protocol_byte;
  memcpy(buf + 1, hdr, BT_EVT_HDR_SIZE);
  retval = uart_transport_->Read(buf + BT_EVT_HDR_SIZE + 1, packet_len);
  if (retval < 0) {
    ALOGE("%s:error in reading buf: %d", __func__, retval);
    retval = -1;
    return retval;
  }
  tot_len = packet_len + BT_EVT_HDR_SIZE + 1;
  ALOGV("read scucesssssfully payload: tot_len: %d", tot_len);
  return tot_len;
}

int PatchDLManager::ReadCmdCmplEvent(unsigned char* buf, int size)
{
  int tot_len = -1;
  do {
    tot_len = ReadNewHciEvent(buf, size);
    if (tot_len < 0) {
      ALOGE("%s: Error while reading the hci event", __func__);
      break;
    }
    bt_logger_->ProcessRx(HCI_PACKET_TYPE_EVENT, &buf[1], tot_len - 1);
    if (buf[1] == EVT_CMD_COMPLETE) {
      ALOGD("%s: Cmd Cmpl received for opcode %0x", __func__,
                                (buf[4] | (buf[5] << 8)));
      break;
    } else if (buf[4] != LOG_BT_MESSAGE_TYPE_HW_ERR &&
               buf[4] != LOG_BT_MESSAGE_TYPE_MEM_DUMP &&
               buf[4] != LOG_BT_HCI_BUFFER_DUMP) {
      ALOGE("%s: Unexpected event %0x received", __func__, buf[1]);
    }
  } while (buf[1] != EVT_CMD_COMPLETE);
  return tot_len;
}

int PatchDLManager::ReadHciEvent(unsigned char* buf, int size)
{
  int tot_len;
  unsigned short int opcode;

  tot_len = ReadNewHciEvent(buf, size);

  if (tot_len < 0) {
    ALOGE("%s: Error while reading the hci event", __func__);
    return -1;
  }

  bt_logger_->ProcessRx(HCI_PACKET_TYPE_EVENT, &buf[1], tot_len - 1);
  if (unified_hci && (buf[EVENTCODE_OFFSET] == VSEVENT_CODE) &&
      buf[4] != LOG_BT_MESSAGE_TYPE_HW_ERR &&
      buf[4] != LOG_BT_MESSAGE_TYPE_MEM_DUMP) {
    ALOGE("%s: Unexpected event recieved rather than CC", __func__);
    return 0;
  }
  if (buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC) {
    if (buf[3] == LOG_BT_CONTROLLER_LOG) {
      /* if it is RAMDUMP event OR
       * if normal logging event and diag is enabled
       */
      if (buf[4] == LOG_BT_MESSAGE_TYPE_MEM_DUMP || buf[4] == LOG_BT_MESSAGE_TYPE_HW_ERR) {
        ALOGE("It is RAMDUMP OR CRASH INFO. EVT");
        //HAVE A TERMINATION
        ReadVsHciEvent(buf, size);
      } else {
        ALOGV("%s: It is an LOG packet, just dumped it to DIAG", __func__);
        return 0;
      }
    }
  } else if (buf[1] == EVT_CMD_COMPLETE) {
    ALOGD("%s: Expected CC", __func__);
    if (tot_len > UNIFIED_HCI_CC_MIN_LENGTH) {
      opcode = (buf[4] | (buf[5] << 8));
      if (((HCI_VS_WIPOWER_CMD_OPCODE == opcode) && (UNIFIED_HCI_CODE == buf[6])) ||
          ((HCI_VS_GET_VER_CMD_OPCODE == opcode) && (buf[7] == EDL_PATCH_VER_REQ_CMD))) {
        unified_hci = true;
        ALOGI("HCI Unified command interface supported");
      }
    }
    /* Parse and check cmd response status */
    if (unified_hci) {
      if (GetVsHciEvent(buf) != HCI_CMD_SUCCESS)
        return -1;
    }
  } else {
    ALOGE("%s: Unexpected event : protocol byte: %d", __func__, buf[1]);
    tot_len = -1;
  }
  return tot_len;
}

int PatchDLManager::ReadVsHciEvent(unsigned char* buf, int size)
{
  int tot_len;
  bool collecting_ram_dump = false;
  unsigned short int opcode;

  do {
    tot_len = ReadNewHciEvent(buf, size);
    if (tot_len < 0) {
      ALOGE("%s: Error while reading the hci event", __func__);
      return -1;
    }

    bt_logger_->ProcessRx(HCI_PACKET_TYPE_EVENT, &buf[1], tot_len - 1);

    if (buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC) {
      /* if it is RAMDUMP event OR
       * if normal logging event and diag is enabled
       */
      if (buf[3] == LOG_BT_CONTROLLER_LOG) {
        if (buf[4] == LOG_BT_MESSAGE_TYPE_MEM_DUMP || buf[4] == LOG_BT_HCI_BUFFER_DUMP) {
          if (!collecting_ram_dump) {
            ALOGE("It is RAMDUMP OR HCI_BUFFER_DUMP, keep looping to get RAMDUMP events");
            collecting_ram_dump = true;
          } else {
            collecting_ram_dump = bt_logger_->IsCrashDumpStarted();
            // crash dump finished.
            if (collecting_ram_dump == false) {
              ALOGE("%s: crash dump finished in between!", __func__);
              return -1;
            }
          }
        } else {
          ALOGV("%s: It is an LOG packet, just dumped it to DIAG", __func__);
          return 0;
        }
      } else {
        ALOGV("VSC Event! good");
      }
    } else if (buf[1] == EVT_CMD_COMPLETE) {
      ALOGI("%s: Expected CC", __func__);
      if (tot_len > UNIFIED_HCI_CC_MIN_LENGTH) {
        opcode = (buf[4] | (buf[5] << 8));
        if (((HCI_VS_WIPOWER_CMD_OPCODE == opcode) && (UNIFIED_HCI_CODE == buf[6])) ||
            ((HCI_VS_GET_VER_CMD_OPCODE == opcode) && (buf[7] == EDL_PATCH_VER_REQ_CMD))) {
          unified_hci = true;
          ALOGI("HCI Unified command interface supported");
        }
      }
    } else {
      ALOGI("%s: Unexpected event! : opcode: %d", __func__, buf[1]);
      tot_len = -1;
    }

  } while (collecting_ram_dump);

  /* Check if the set patch command is successful or not */
  if (GetVsHciEvent(buf) != HCI_CMD_SUCCESS)
    return -1;

  return tot_len;
}

int PatchDLManager::DisableInternalLdo()
{
  int ret = 0;

  if (IsExtldoEnabled()) {
    unsigned char cmd[5] = { 0x01, 0x0C, 0xFC, 0x01, 0x32 };
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    ALOGI(" %s ", __func__);
    ret = HciSendVsCmd(cmd, rsp, 5);
    if (ret != 5) {
      ALOGE("%s: Send failed with ret value: %d", __func__, ret);
      ret = -1;
    } else {
      /* Wait for command complete event */
      ret = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if ( ret < 0) {
        ALOGE("%s: Failed to get response from controller", __func__);
      }
    }
  }
  return ret;
}

int PatchDLManager::GetVsHciEvent(unsigned char *rsp)
{
  int err = 0;
  unsigned char paramlen = 0;
  unsigned char EMBEDDED_MODE_CHECK = 0x02;
  unsigned int opcode = 0;
  unsigned char subOpcode = 0;
  unsigned int ocf = 0;
  unsigned int ogf = 0;
  unsigned char status = 0;
  uint8_t baudrate_rsp_status_offset = 0;
  uint8_t addon_features_bitmask_offset = 0;
  if ( (rsp[EVENTCODE_OFFSET] == VSEVENT_CODE) || (rsp[EVENTCODE_OFFSET] == EVT_CMD_COMPLETE))
    ALOGV("%s: Received HCI-Vendor Specific event", __func__);
  else {
    ALOGI("%s: Failed to receive HCI-Vendor Specific event", __func__);
    err = -EIO;
    goto failed;
  }

  if (!unified_hci) {
    ocf = rsp[CMD_RSP_OFFSET];
    subOpcode = rsp[RSP_TYPE_OFFSET];
    paramlen = rsp[EVT_PLEN];
    ALOGV("%s: Parameter Length: 0x%x", __func__, paramlen);
    ALOGV("%s: Command response: 0x%x", __func__, ocf);
    ALOGV("%s: Response type   : 0x%x", __func__, subOpcode);
  } else {
    paramlen = rsp[EVT_PLEN];
    opcode = rsp[5]<<8 | rsp[4];
    ocf = opcode & 0x03ff;
    ogf = opcode >> 10;
    status = rsp[6];
    subOpcode = rsp[7];
    ALOGV("%s: Opcode: 0x%x", __func__, opcode);
    ALOGV("%s: ocf: 0x%x", __func__, ocf);
    ALOGV("%s: ogf: 0x%x", __func__, ogf);
    ALOGV("%s: Status: 0x%x", __func__, status);
    ALOGV("%s: Sub-Opcode: 0x%x", __func__, subOpcode);
    ALOGV("%s: Parameter Length: 0x%x", __func__, paramlen);
  }

  /* Check the status of the operation */
  switch ( ocf ) {
    case EDL_CMD_REQ_RES_EVT:
      ALOGV("%s: Command Request Response", __func__);
      HandleEdlCmdResEvt(subOpcode, paramlen, rsp);
      break;
    case NVM_ACCESS_CODE:
      ALOGI("%s: NVM Access Code!!!", __func__);
      err = HCI_CMD_SUCCESS;
      break;
    case EDL_SET_BAUDRATE_RSP_EVT:
      baudrate_rsp_status_offset = BAUDRATE_RSP_STATUS_OFFSET;
      [[fallthrough]];
    /* in case of unified cmd, rsp contains cmd opcode */
    case EDL_SET_BAUDRATE_CMD_OCF:
      /* incase of unified hci we have different offset for baudrate status */
      if (unified_hci)
        baudrate_rsp_status_offset = BAUDRATE_RSP_STATUS_OFFSET_UNIFIED;
      if (rsp[baudrate_rsp_status_offset] != BAUDRATE_CHANGE_SUCCESS) {
        ALOGE("%s: Set Baudrate request failed with status: 0x%x",
              __func__, rsp[baudrate_rsp_status_offset]);
        err = -1;
      }
      break;
    case EDL_WIP_QUERY_CHARGING_STATUS_EVT:
      /* Query charging command has below return values
         0 - in embedded mode not charging
         1 - in embedded mode and charging
         2 - hadofff completed and in normal mode
         3 - no wipower supported on mtp. so irrepective of charging
         handoff command has to be sent if return values are 0 or 1.
         These change include logic to enable generic BT turn on sequence.*/
      if (rsp[4] < EMBEDDED_MODE_CHECK) {
        ALOGI("%s: WiPower Charging in Embedded Mode!!!", __func__);
        wipower_handoff_ready_ = rsp[4];
        wipower_flag_ = 1;
      }
      break;
    case EDL_WIP_START_HANDOFF_TO_HOST_EVENT:
      /*TODO: rsp code 00 mean no charging
         this is going to change in FW soon*/
      if (rsp[4] == NON_WIPOWER_MODE) {
        ALOGE("%s: WiPower Charging hand off not ready!!!", __func__);
      }
      break;
    case HCI_VS_GET_ADDON_FEATURES_EVENT:
      addon_features_bitmask_offset = ADDON_FEATURES_BITMASK_OFFSET;
      [[fallthrough]];
    /* incase of unified cmd, rsp contains cmd opcode */
    case HCI_VS_GET_ADDON_FEATURES_SUPPORT:
      /* incase of unified hci we have different offset for addon feature bitmask */
      if (unified_hci)
        addon_features_bitmask_offset = ADDON_FEATURES_BITMASK_OFFSET_UNIFIED;
      if ((rsp[addon_features_bitmask_offset] & ADDON_FEATURES_EVT_WIPOWER_MASK)) {
        ALOGD("%s: WiPower feature supported!!", __func__);
      }
      break;
    case HCI_VS_PERSIST_ACCESS_COMMAND:
      if (status == HCI_CMD_SUCCESS && subOpcode == HCI_VS_PERSIST_ACCESS_OTP_GET
         && rsp[OTP_GET_CMD_CONFIG_TYPE_OFFSET] == HCI_VS_PERSIST_ACCESS_OTP_GET_CONFIG_TYPE
         && rsp[OTP_GET_CMD_DATA_LEN_OFFSET] > 0) {
        ALOGI("%s: HCI_VS_PERSIST_ACCESS_OTP_GET cmd rsp data length: 0x%x",
              __func__, rsp[OTP_GET_CMD_DATA_LEN_OFFSET]);
        BtState :: Get()->SetOTPInfoBytes(rsp[OTP_GET_CMD_DATA_LEN_OFFSET],
                                     (char*)rsp+OTP_GET_CMD_DATA_OFFSET);
      } else {
        char otp_rsp_fail_msg[255];
        ALOGE("%s: HCI_VS_PERSIST_ACCESS_OTP_GET cmd failed "
              "config_type: 0x%x subopcode: 0x%x status: 0x%x",
              __func__, rsp[OTP_GET_CMD_CONFIG_TYPE_OFFSET], subOpcode, status);
        snprintf(otp_rsp_fail_msg, sizeof(otp_rsp_fail_msg),
          "config_type: 0x%x subopcode: 0x%x status: 0x%x",
          rsp[OTP_GET_CMD_CONFIG_TYPE_OFFSET], subOpcode, status);
        BtState :: Get()->SetOTPInfoBytes(0, otp_rsp_fail_msg);
      }
      break;
    case HCI_VS_STRAY_EVT:
      if (unified_hci && subOpcode == HCI_VS_HOST_LOG_ENH_ENABLE_SUBID) {
        if (status != LOG_ENH_ENABLE_SUCCESS) {
          ALOGE("%s: enable/disable Enhanced Logging cmd failed: 0x%x", __func__, status);
          err = -1;
        }
        break;
      }
      /* WAR to handle stray Power Apply EVT during patch download */
      ALOGD("%s: Stray HCI VS EVENT", __func__);
      if (patch_dnld_pending_ && dnld_fd_in_progress_ != -1) {
        unsigned char rsp[HCI_MAX_EVENT_SIZE];
        memset(rsp, 0x00, HCI_MAX_EVENT_SIZE);
        ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      } else {
        ALOGE("%s: Not a valid status!!!", __func__);
        err = -1;
      }
      break;
    default:
      ALOGE("%s: Not a valid status!!!", __func__);
      err = -1;
      break;
  }

 failed:
  return err;
}

void PatchDLManager::HandleEdlCmdResEvt(unsigned char subOpcode, unsigned char paramlen,
  unsigned char* rsp)
{
  int err = 0;
  unsigned int soc_id = 0;
  unsigned int productid = 0;
  unsigned short patchversion = 0;
  unsigned short buildversion = 0;
  unsigned int bld_len_offset = 5;
  char build_label[255];
  int build_lbl_len;
  struct timeval tv;
#ifdef USER_DEBUG
  FILE *btversionfile = 0;
  char dst_buff[MAX_BUFF_SIZE];
#endif
  bool ret  = false;
  int build_label_prefix;

  switch (subOpcode) {
    case EDL_PATCH_VER_RES_EVT:
    case EDL_APP_VER_RES_EVT:
      if (!unified_hci) {
        productid = (unsigned int)(rsp[PATCH_PROD_ID_OFFSET + 3] << 24 |
                                  rsp[PATCH_PROD_ID_OFFSET + 2] << 16 |
                                  rsp[PATCH_PROD_ID_OFFSET + 1] << 8 |
                                  rsp[PATCH_PROD_ID_OFFSET]  );
        patchversion = (unsigned short)(rsp[PATCH_PATCH_VER_OFFSET + 1] << 8 |
                                       rsp[PATCH_PATCH_VER_OFFSET] );
        buildversion = (int)(rsp[PATCH_ROM_BUILD_VER_OFFSET + 1] << 8 |
                    rsp[PATCH_ROM_BUILD_VER_OFFSET] );

        ALOGI("\t Current Product ID\t\t: 0x%08x", productid);
        ALOGI("\t Current Patch Version\t\t: 0x%04x", patchversion);
        ALOGI("\t Current ROM Build Version\t: 0x%04x", buildversion);

        if (paramlen - 10) {
          soc_id = (unsigned int)(rsp[PATCH_SOC_VER_OFFSET + 3] << 24 |
                                 rsp[PATCH_SOC_VER_OFFSET + 2] << 16 |
                                 rsp[PATCH_SOC_VER_OFFSET + 1] << 8 |
                                 rsp[PATCH_SOC_VER_OFFSET] );
          ALOGI("\t Current SOC Version\t\t: 0x%08x", soc_id);
        }
      } else {
        productid = (unsigned int)(rsp[PATCH_PROD_ID_OFFSET_UNIFIED + 3] << 24 |
                                  rsp[PATCH_PROD_ID_OFFSET_UNIFIED + 2] << 16 |
                                  rsp[PATCH_PROD_ID_OFFSET_UNIFIED + 1] << 8 |
                                  rsp[PATCH_PROD_ID_OFFSET_UNIFIED]  );
        ALOGI("\t unified Current Product ID\t\t: 0x%08x", productid);

        /* Patch Version indicates FW patch version */
        patchversion = (unsigned short)(rsp[PATCH_PATCH_VER_OFFSET_UNIFIED + 1] << 8 |
                                             rsp[PATCH_PATCH_VER_OFFSET_UNIFIED] );
        ALOGI("\t unified Current Patch Version\t\t: 0x%04x", patchversion);

        /* ROM Build Version indicates ROM build version like 1.0/1.1/2.0 */
        buildversion =
              (int)(rsp[PATCH_ROM_BUILD_VER_OFFSET_UNIFIED + 1] << 8 |
                    rsp[PATCH_ROM_BUILD_VER_OFFSET_UNIFIED] );
        ALOGI("\t unified Current ROM Build Version\t: 0x%04x", buildversion);

        if (paramlen - 10) {
          soc_id =
                (unsigned int)(rsp[PATCH_SOC_VER_OFFSET_UNIFIED + 3] << 24 |
                                rsp[PATCH_SOC_VER_OFFSET_UNIFIED + 2] << 16 |
                                rsp[PATCH_SOC_VER_OFFSET_UNIFIED + 1] << 8 |
                                rsp[PATCH_SOC_VER_OFFSET_UNIFIED]  );
          ALOGI("\t unified Current SOC Version\t\t: 0x%08x", soc_id);
          char soc_id_info[255];
          snprintf(soc_id_info, sizeof(soc_id_info),
            "unified Current SOC Version : 0x%08x", soc_id);
          BtState::Get()->SetBtSocId(soc_id_info);
        }
      }
      chipset_ver_ = QCA_BT_VER(soc_id, productid, buildversion);
#ifdef USER_DEBUG
      if (version_info_ins.chipset_ver != chipset_ver_ ||
          version_info_ins.patchversion != patchversion) {
        ALOGD("%s: Writing version info. in %s", __func__, BT_VERSION_FILEPATH);
        gettimeofday(&tv, NULL);
        bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK,
                                                  "Writing version info. in file");
        BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Writing version info. in file");
        BtState :: Get()->SetPreVersioInfoTS(dst_buff);
        if (NULL != (btversionfile = fopen(BT_VERSION_FILEPATH, "wb"))) {
          version_info_ins.chipset_ver = chipset_ver_;
          version_info_ins.patchversion = patchversion;
          fprintf(btversionfile, "Bluetooth Controller Product ID    : 0x%08x\n", productid);
          fprintf(btversionfile, "Bluetooth Controller Patch Version : 0x%04x\n", patchversion);
          fprintf(btversionfile, "Bluetooth Controller Build Version : 0x%04x\n", buildversion);
          fprintf(btversionfile, "Bluetooth Controller SOC Version   : 0x%08x\n", soc_id);
          fclose(btversionfile);
        } else {
          ALOGE("Failed to dump SOC version info. Errno:%d", errno);
        }
        ALOGD("%s: Done writing version info. in %s", __func__, BT_VERSION_FILEPATH);
        gettimeofday(&tv, NULL);
        BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done writing version info. in file");
        BtState :: Get()->SetPostVersioInfoTS(dst_buff);
        bt_logger_->CheckAndAddToDelayList(&tv);
      }
#endif
      gettimeofday(&tv, NULL);
      bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_POWER_IOCTL_STUCK,
                                                  "Sending SoC version to Power driver");
      ret = power_manager_->SendSocVersion(soc_id);
      gettimeofday(&tv, NULL);
      bt_logger_->CheckAndAddToDelayList(&tv);
      if (ret != true ) {
         ALOGE ("SOC version ioctl failed");
      }
      /* Update ideal delay for cmds which have additional
       * delay due calibration at SoC side when these cmd are processed.
       */
      if (IS_NEWER_THAN_CHEROKEE_2_1(chipset_ver_)) {
        if (soc_type_ == BT_SOC_CHEROKEE) {
          bt_logger_->UpdateIdealDelay(BT_HOST_REASON_ADDONFEAT_CMD_STUCK,
                                       ADDON_FEATURE_CMD_RSP_IDEAL_TIME_DELAY_CHE3x);
        }
      } else {
        bt_logger_->UpdateIdealDelay(BT_HOST_REASON_HCI_RESET_STUCK,
                                       HCI_RESET_CMD_RSP_IDEAL_TIME_DELAY_CHE2x);
        bt_logger_->UpdateIdealDelay(BT_HOST_REASON_ADDONFEAT_CMD_STUCK,
                                       CMD_RSP_IDEAL_TIME_DELAY);
      }

      break;
    case EDL_TLV_PATCH_CONFIG:
      if (unified_hci) {
        err = rsp[CMD_STATUS_OFFSET_UNIFIED];
        if (err == HCI_CMD_SUCCESS) {
          ALOGD("%s: patch config cmd completed successfully", __func__);
        } else {
          ALOGE("%s: patch config cmd failed with HCI err code 0x%x", __func__, err);
        }
      }
      break;
    case EDL_TVL_DNLD_RES_EVT:
    case EDL_CMD_EXE_STATUS_EVT:
    /* In case of unified HCI cmd, rsp will contain cmd subopcode in this case 0x1E */
    case EDL_PATCH_TLV_REQ_CMD:
      if (unified_hci)
        err = rsp[CMD_STATUS_OFFSET_UNIFIED];
      else
        err = rsp[CMD_STATUS_OFFSET];
      DownloadAndCmdExeStatus(err);
      break;
    case EDL_GET_BUILD_INFO:
    case HCI_VS_GET_BUILD_VER_EVT:
      if (unified_hci) {
        bld_len_offset += 3;
      }
      build_lbl_len = rsp[bld_len_offset];
      snprintf(build_label, sizeof(build_label), "BT SoC FW SU Build info: ");
      build_label_prefix = strlen(build_label);
      memcpy(build_label + build_label_prefix, &rsp[bld_len_offset + 1], build_lbl_len);
      *(build_label + build_label_prefix + build_lbl_len) = '\0';
      gettimeofday(&tv, NULL);
      BtState::Get()->SetFwSuBuildInfo(build_label, tv);

      ALOGI("%s: %s, %d", __func__, build_label, build_lbl_len);

#ifdef USER_DEBUG
      if (strcmp(version_info_ins.build_label, build_label) != 0) {
        ALOGD("%s: Writing SU build info. in %s", __func__, BT_VERSION_FILEPATH);
        gettimeofday(&tv, NULL);
        bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK,
                                                  "Writing Controller SU Build info.");
        BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Writing Controller SU Build info.");
        BtState :: Get()->SetPreSUBuildInfoTS(dst_buff);
        if (NULL != (btversionfile = fopen(BT_VERSION_FILEPATH, "a+b"))) {
          snprintf(version_info_ins.build_label, sizeof(version_info_ins.build_label),
                   "%s", build_label);
          fprintf(btversionfile, "Bluetooth Contoller SU Build info  : %s\n", build_label);
          fclose(btversionfile);
        } else {
          ALOGI("Failed to dump  FW SU build info. Errno:%d", errno);
        }
        ALOGD("%s: Done Writing SU build info. in %s", __func__, BT_VERSION_FILEPATH);
        gettimeofday(&tv, NULL);
        BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done writing Controller SU Build info.");
        BtState :: Get()->SetPostSUBuildInfoTS(dst_buff);
        bt_logger_->CheckAndAddToDelayList(&tv);
      }
#endif
      break;
    case EDL_BOARD_ID_RESPONSE:
      {
        uint8_t msbBoardId = 0;
        uint8_t lsbBoardId = 0;
        uint8_t boardIdLen = 0;
        board_id_[0] = '\0';

        paramlen = (uint8_t)rsp[EVT_PLEN];
        if (unified_hci) {
          if (paramlen < 8) {
            ALOGE("%s: Invalid Param Len in BoardId rsp:%d!!", __func__, paramlen);
            break;
          }
          boardIdLen = (uint8_t)rsp[8];
          msbBoardId = (uint8_t)rsp[9];
          lsbBoardId = (uint8_t)rsp[10];
        } else {
          if (paramlen < 5) {
            ALOGE("%s: Invalid Param Len in BoardId rsp:%d!!", __func__, paramlen);
            break;
          }
          boardIdLen = (uint8_t)rsp[5];
          msbBoardId = (uint8_t)rsp[6];
          lsbBoardId = (uint8_t)rsp[7];
        }

        if (boardIdLen != 2) {
          ALOGI("%s: Invalid Board Id Len %d!!", __func__, boardIdLen);
          break;
        }

        ALOGI("%s: Board Id %x %x!!", __func__, msbBoardId, lsbBoardId);
        if (msbBoardId == 0x00) {
          board_id_[0] = Convert2Ascii((lsbBoardId & MSB_NIBBLE_MASK) >> 4);
          board_id_[1] = Convert2Ascii(lsbBoardId & LSB_NIBBLE_MASK);
          board_id_[2] = '\0';
        } else {
          board_id_[0] = Convert2Ascii((msbBoardId & MSB_NIBBLE_MASK) >> 4);
          board_id_[1] = Convert2Ascii(msbBoardId & LSB_NIBBLE_MASK);
          board_id_[2] = Convert2Ascii((lsbBoardId & MSB_NIBBLE_MASK) >> 4);
          board_id_[3] = Convert2Ascii(lsbBoardId & LSB_NIBBLE_MASK);
          board_id_[4] = '\0';
        }
      }
      break;
    case EDL_SECURE_BRIDGE_CMD:
      if (HCI_CMD_SUCCESS == rsp[CMD_STATUS_OFFSET_UNIFIED]) {
        secure_bridge_enabled = true;
      } else {
        secure_bridge_enabled = false;
        ALOGE("%s: Enable Secure bridge cmd failed with status: 0x%x", __func__,
              rsp[CMD_STATUS_OFFSET_UNIFIED]);
      }
      break;
  }
}

void PatchDLManager::DownloadAndCmdExeStatus(int err) {
  switch (err) {
    case HCI_CMD_SUCCESS:
      ALOGV("%s: Download Packet successfully!", __func__);
      break;
    case LENGTH_ERROR:
      ALOGE("%s: Invalid patch length argument passed for EDL PATCH "
                    "SET REQ cmd, (0x%x)", __func__, err);
      break;
    case VERSION_ERROR:
      ALOGE("%s: Invalid patch version argument passed for EDL PATCH "
                    "SET REQ cmd, (0x%x)", __func__, err);
      break;
    case SIGNATURE_ERROR:
      ALOGE("%s: SIGNATURE_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case NO_MEMORY_ERROR:
      ALOGE("%s: NO_MEMORY_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case NVM_TAG_ERROR:
      ALOGE("%s: NVM_TAG_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case NVM_LENGTH_ERROR:
      ALOGE("%s: NVM_LENGTH_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case BT_MODE_ERRO:
      ALOGE("%s: BT_MODE_ERRO (0x%x) detected!!!", __func__, err);
      break;
    case UNKOWN_ERROR:
      ALOGE("%s: UNKOWN_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case TYPE_ERROR:
      ALOGE("%s: TYPE_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case IMAGE_TYPE_ERROR:
      ALOGE("%s: IMAGE_TYPE_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case CRC_ERROR:
      ALOGE("%s: CRC_ERROR (0x%x) detected", __func__, err);
      break;
    case ID_ERROR:
      ALOGE("%s: ID_ERROR (0x%x) detected", __func__, err);
      break;
    case WRONG_VER_ERROR:
      ALOGE("%s: WRONG_VER_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case SMALL_VER_ERROR:
      ALOGE("%s: SMALL_VER_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case REGION_ERROR:
      ALOGE("%s: REGION_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case NO_PATCH_ERROR:
      ALOGE("%s: NO_PATCH_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case WRONG_ANTI_ROLLBACK_ERROR:
      ALOGE("%s: WRONG_ANTI_ROLLBACK_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case OTP_WR_ERROR:
      ALOGE("%s: OTP_WR_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_PATCH_PER_OTP_ERROR:
      ALOGE("%s: INVALID_PATCH_PER_OTP_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_PATCH_HEADER_ERROR:
      ALOGE("%s: INVALID_PATCH_HEADER_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_RSA_KEY_ERROR:
      ALOGE("%s: INVALID_RSA_KEY_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_RSA_SIGNATURE_ERROR:
      ALOGE("%s: INVALID_RSA_SIGNATURE_ERROR (0x%x) detected!!!", __func__, err);
      break;
    case INVALID_SERIAL_NUMBER_ERROR:
      ALOGE("%s: INVALID_SERIAL_NUMBER_ERROR (0x%x) detected!!!", __func__, err);
      break;
    default:
      ALOGE("%s: Undefined error (0x%x)", __func__, err);
      break;
  }
}

bool PatchDLManager::IsExtldoEnabled()
{
  bool is_extldo = false;

  is_extldo = power_manager_->ExternalldoStatus();
  return is_extldo;
}

int PatchDLManager::HciSendVsCmd(unsigned char *cmd, unsigned char *rsp, int size)
{
  int ret = 0;
  char dst_buff[MAX_BUFF_SIZE] = {'\0'};
  struct timeval tv;

  bt_logger_->ProcessTx(HCI_PACKET_TYPE_COMMAND, &cmd[1], size - 1);

  /* Send the HCI command packet to UART for transmission */
  ret = uart_transport_->UartWrite(cmd, size);
  if (ret != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, ret);
    goto failed;
  }
  /* checking cmd size, ocf and subopcode */
  if (size == 5 && cmd[1] == 0 && cmd[4] == EDL_PATCH_VER_REQ_CMD) {
    ALOGD("%s: Get Version Cmd sent to SOC", __func__);
    gettimeofday(&tv, NULL);
    bt_logger_->CheckAndAddToDelayList(&tv);

    bt_logger_->SetSecondaryCrashReason(BT_HOST_REASON_GETVER_NO_RSP_RCVD);
    bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_GETVER_NO_RSP_RCVD,
                                              "GET VERSION CMD RSP");
    BtState :: Get()->AddLogTag(dst_buff, tv,
                               (char *)"Reading Get Version CMD RSP from SOC");
    BtState :: Get()->SetGetVerReqTS(dst_buff, READ_RSP_TS);
  }

  if (wait_vsc_evt_) {
    /* Check for response from the Controller */
    if (!unified_hci) {
     if (ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
      ret = -ETIMEDOUT;
      ALOGI("%s: Failed to get ReadVsHciEvent Event from SOC", __func__);
      goto failed;
     }
      ALOGV("%s: Received HCI-Vendor Specific Event from SOC", __func__);
    }
    else
    {
     if (ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
      ret = -ETIMEDOUT;
      ALOGI("%s: Failed to get ReadHciEvent Event from SOC", __func__);
      goto failed;
     }
      ALOGV("%s: Received HCI-Vendor Specific Event from SOC", __func__);
    }
  }

 failed:
  return ret;
}

void PatchDLManager::FrameHciPkt(
  unsigned char *cmd,
  int edl_cmd, unsigned int p_base_addr,
  int segtNo, int size
)
{
  int offset = 0;
  hci_command_hdr *cmd_hdr;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);

  cmd[0]      = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_PATCH_CMD_OCF);
  cmd_hdr->plen   = size;
  cmd[4]      = edl_cmd;

  switch (edl_cmd) {
    case EDL_PATCH_SET_REQ_CMD:
      /* Copy the patch header info as CMD params */
      memcpy(&cmd[5], phdr_buffer_, PATCH_HDR_LEN);
      ALOGV("%s: Sending EDL_PATCH_SET_REQ_CMD", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_DLD_REQ_CMD:
      offset = ((segtNo - 1) * MAX_DATA_PER_SEGMENT);
      p_base_addr += offset;
      cmd_hdr->plen   = (size + 6);
      cmd[5]  = (size + 4);
      cmd[6]  = EXTRACT_BYTE(p_base_addr, 0);
      cmd[7]  = EXTRACT_BYTE(p_base_addr, 1);
      cmd[8]  = EXTRACT_BYTE(p_base_addr, 2);
      cmd[9]  = EXTRACT_BYTE(p_base_addr, 3);
      memcpy(&cmd[10], (pdata_buffer_ + offset), size);

      ALOGV("%s: Sending EDL_PATCH_DLD_REQ_CMD: size: %d bytes",
            __func__, size);
      ALOGV("HCI-CMD %d:\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t"
            "0x%x\t0x%x\t0x%x\t\n", segtNo, cmd[0], cmd[1], cmd[2],
            cmd[3], cmd[4], cmd[5], cmd[6], cmd[7], cmd[8], cmd[9]);
      break;
    case EDL_PATCH_ATCH_REQ_CMD:
      ALOGV("%s: Sending EDL_PATCH_ATTACH_REQ_CMD", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_RST_REQ_CMD:
      ALOGV("%s: Sending EDL_PATCH_RESET_REQ_CMD", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_VER_REQ_CMD:
      ALOGV("%s: Sending EDL_PATCH_VER_REQ_CMD", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_TLV_REQ_CMD:
      ALOGV("%s: Sending EDL_PATCH_TLV_REQ_CMD", __func__);
      /* Parameter Total Length */
      cmd[3] = size + 2;

      /* TLV Segment Length */
      cmd[5] = size;
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
      offset = (segtNo * MAX_SIZE_PER_TLV_SEGMENT);
      memcpy(&cmd[6], (pdata_buffer_ + offset), size);
      break;
    case EDL_GET_BUILD_INFO:
      ALOGV("%s: Sending EDL_GET_BUILD_INFO", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_GET_BOARD_ID:
      ALOGV("%s: Sending EDL_GET_BOARD_ID", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;

    case EDL_SECURE_BRIDGE_CMD:
      ALOGV("%s: Sending EDL_SECURE_BRIDGE_CMD", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;

    case EDL_TLV_PATCH_CONFIG: {
      /* patch configuration */
      if (strcmp(elf_config_, "no_value") != 0) {
        unsigned long ul;
        /* converting hex in string to real hex number */
        ul = strtoul (elf_config_, NULL, 16);
        /* patch config size is 4 */
        memcpy(&cmd[5], &ul, sizeof(uint32_t));
        /* Checking BIT 0 and BIT 1:
         * 1)  BIT 0: Whether SoC needs to respond for every 
         *     download command (0) or only at the end (1).
         * 2)  BIT 1: Whether to perform Parallel (1) or Sequential 
         *     validation (0)
         */
        if ((cmd[5] & 1) == 0) {
          tlv_dwn_cfg_ = SKIP_EVT_NONE;
        } else {
          tlv_dwn_cfg_ = SKIP_EVT_VSE_CC;
        }
        if ((cmd[5] & 2) == 0) {
          ALOGI("%s: Sequential validation enabled by SoC", __func__);
        } else {
          ALOGI("%s: Parallel validation enabled by SoC", __func__);
        }
        ALOGD("%s: Sending EDL_TLV_PATCH_CONFIG", __func__);
        ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x  \t0x%x \t0x%x \t0x%x",
              segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5],  cmd[6], cmd[7], cmd[8]);
      }
      break;
    }
    default:
      ALOGE("%s: Unknown EDL CMD !!!", __func__);
  }
}

unsigned char PatchDLManager::IsSibsEnabled()
{

  /* Check whether IBS is enabled through the macro 'WCNSS_IBS_ENABLED'
     and the property 'persist.vendor.service.bdroid.sibs'. */
#ifdef WCNSS_IBS_ENABLED
  return IbsHandler::IsEnabled();
#else
  return false;
#endif
}

unsigned char PatchDLManager::Convert2Ascii(unsigned char temp)
{
  unsigned char n = temp;

  if (n <= 9)
    n = n + 0x30;
  else
    n = n + 0x57;
  return n;
}

void PatchDLManager::UpdateNewNvmFormat(tlv_nvm_hdr *nvm)
{
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;
  uint8_t *nvm_byte_ptr = (uint8_t *)nvm;

  if (!nvm)
    return;

  nvm_byte_ptr += sizeof(tlv_nvm_hdr);

  /* Update Tag#17: HCI UART Settings */
  if (nvm->tag_id == TAG_NUM_17) {
    uint8_t baudrate = uart_transport_->GetMaxBaudrate();

    ALOGI("%s: baudrate %02x", __func__, baudrate);

    /* Byte#1: UART baudrate */
    *(nvm_byte_ptr + 1) = baudrate;
  }

  /* Update Tag#27: SIBS Settings */
  if (nvm->tag_id == 27) {
    if (!IsSibsEnabled()) {
      /* TxP Sleep Mode: Disable */
      *(nvm_byte_ptr + 1) &= ~0x01;
      ALOGI("%s: SIBS Disable", __func__);
    } else {
      /* TxP Sleep Mode-1:UART_SIBS, 2:USB_SUSPEND, 3: GPIO_OOB, 4: UART_HWIBS */
      *(nvm_byte_ptr + 1) |= 0x01;
      /* Check for wakeup config */
      CheckForWakeupMechanism(nvm_byte_ptr, WAKEUP_CONFIG_BYTE_CHE_3_AND_LATER);
      ALOGI("%s: SIBS Enable", __func__);
    }
  }
  if (chipset_ver_ == HSP_VER_1_0 ||
      chipset_ver_ == HSP_VER_1_1 ||
      chipset_ver_ == HSP_VER_2_0 ||
      chipset_ver_ == HSP_VER_2_0_G ||
      chipset_ver_ == HSP_VER_2_1 ||
      chipset_ver_ == HSP_VER_2_1_G) {
    if (nvm->tag_id == 35) {
      if (*(nvm_byte_ptr + PA_OFFSET) & (1 << 2)) {
        gettimeofday(&tv, NULL);
        BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"iPA supported, configure TCS now");
        BtState :: Get()->SetPreTCSConfigIoctlTS(dst_buff);
        bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_POWER_IOCTL_STUCK,
                                                  "iPA supported, Configure TCS");
        ALOGI("%s: Received iPA supported conf, configure TCS now", __func__);
        if (!power_manager_->SendIoctlToConfigTCS()) {
          ALOGE("%s: Board radio config ioctl failed", __func__);
        }
        gettimeofday(&tv, NULL);
        BtState :: Get()->AddLogTag(dst_buff, tv, (char *)"Done configuring TCS as iPA is supported");
        BtState :: Get()->SetPostTCSConfigIoctlTS(dst_buff);
        bt_logger_->CheckAndAddToDelayList(&tv);
      } else {
          ALOGI("%s: Received xPA supported conf from board", __func__);
      }
    }
  }
}

void PatchDLManager::CheckForWakeupMechanism(unsigned char *nvm_byte_ptr, int offset) {
  // Wakeup source config
  uint8_t wakeup_config = 0;
  // Check for Bit 3:2 for wakeup configuration
  wakeup_config = (*(nvm_byte_ptr + offset) >> 2) & WAKEUP_CONFIG_MASK;

  IbsHandler* ibs_ins = IbsHandler::Get();
  if (ibs_ins && wakeup_config == WAKEUP_CONFIG_UART_RxD_CONFIG) {
    ibs_ins->SetRxDwakeupEnabled();
  }
  ALOGD("%s: Wakeup source config : %u", __func__, wakeup_config);
}

#ifdef DEBUG_CRASH_SOC_WHILE_DLD
int PatchDLManager::SendCrashCommand()
{
  int ret = 0;
  unsigned char cmd[5] = { 0x01, 0x0C, 0xFC, 0x01, 0x28 };
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  ALOGI(" %s ", __func__);
  bt_logger_->ProcessTx(HCI_PACKET_TYPE_COMMAND, &cmd[1], size - 1);
  ret = uart_transport_->UartWrite(cmd, 5);
  if (ret != 5) {
    ALOGE("%s: Send failed with ret value: %d", __func__, ret);
    ret = -1;
  } else {
    /* Wait for command complete event */

    ret = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if ( ret < 0) {
      ALOGE("%s: Failed to get response from controller", __func__);
    }
  }
  return ret;
}
#endif    //DEBUG_CRASH_SOC_WHILE_DLD

void PatchDLManager::LoadPatchMaptable() {

  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(ROME_VER_2_1,
                          new PatchPathManager("ROME2_1", "rampatch_tlv_2.1.tlv", "nvm_tlv_2.1.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(ROME_VER_3_0,
                          new PatchPathManager("ROME3_0", "btfw30.tlv", "btnv30.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(ROME_VER_3_2,
                          new PatchPathManager("ROME3_2", "btfw32.tlv", "btnv32.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(CHEROKEE_VER_2_0,
                          new PatchPathManager("CHE2_0", "crbtfw20.tlv", "crnv20.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(CHEROKEE_VER_2_0_1,
                          new PatchPathManager("CHE2_0_1", "crbtfw20.tlv", "crnv20.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(CHEROKEE_VER_2_1,
                          new PatchPathManager("CHE2_1", "crbtfw21.tlv", "crnv21.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(CHEROKEE_VER_2_1_1,
                          new PatchPathManager("CHE2_1_1", "crbtfw21.tlv", "crnv21.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(CHEROKEE_VER_3_1,
                          new PatchPathManager("CHE3_1", "crbtfw30.tlv", "crnv30.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(CHEROKEE_VER_3_2,
                          new PatchPathManager("CHE3_2", "crbtfw32.tlv", "crnv32.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(CHEROKEE_VER_3_2_UMC,
                          new PatchPathManager("CHE3_2", "crbtfw32.tlv", "crnv32u.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(APACHE_VER_1_0_0,
                          new PatchPathManager("APA1_0_0", "apbtfw10.tlv", "apnv10.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(APACHE_VER_1_1_0,
                          new PatchPathManager("APA1_1_0", "apbtfw11.tlv", "apnv11.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(APACHE_VER_1_2_0,
                          new PatchPathManager("APA1_2_0", "apbtfw11.tlv", "apnv11.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(APACHE_VER_1_2_1,
                          new PatchPathManager("APA1_2_1", "apbtfw11.tlv", "apnv11.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(COMANCHE_VER_1_0_1,
                          new PatchPathManager("COM1_0_1", "cmbtfw10.tlv", "cmnv10.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(COMANCHE_VER_1_1,
                          new PatchPathManager("COM1_1", "cmbtfw11.tlv", "cmnv11.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(COMANCHE_VER_1_2_UMC,
                          new PatchPathManager("COM1_2", "cmbtfw12.tlv", "cmnv12.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(COMANCHE_VER_1_2,
                          new PatchPathManager("COM1_2", "cmbtfw12.tlv", "cmnv12s.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(COMANCHE_VER_1_3,
                          new PatchPathManager("COM1_3", "cmbtfw13.tlv", "cmnv13s.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(COMANCHE_VER_1_3_TSMC,
                          new PatchPathManager("COM1_3", "cmbtfw13.tlv", "cmnv13t.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(COMANCHE_VER_1_3_UMC,
                          new PatchPathManager("COM1_3", "cmbtfw13.tlv", "cmnv13.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(HASTINGS_VER_2_0,
                          new XmemPatchPathManager("HST2_0", "htbtfw20.tlv", "htnv20.bin",
                                                   "htbtxm.tlv", "htnv20xm.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(GENOA_VER_1_0,
                          new PatchPathManager("GEN1_0", "gnbtfw10.tlv", "gnnv10.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(GENOA_VER_2_0,
                          new PatchPathManager("GEN2_0", "gnbtfw20.tlv", "gnnv20.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(GENOA_VER_2_0_0_2,
                          new PatchPathManager("GEN2_0", "gnbtfw20.tlv", "gnnv20.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(HSP_VER_1_0,
                          new XmemPatchPathManager("HSP1_0", "hpbtfw10.tlv", "hpnv10.bin",
                                                   "hpbtxm.tlv", "hpnv10xm.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(HSP_VER_1_1,
                          new XmemPatchPathManager("HSP1_1", "hpbtfw10.tlv", "hpnv10.bin",
                                                   "hpbtxm.tlv", "hpnv10xm.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(HSP_VER_2_0,
                          new XmemPatchPathManager("HSP2_0", "hpbtfw20.tlv", "hpnv20.bin",
                                                   "hpbtxm.tlv", "hpnv20xm.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(HSP_VER_2_0_G,
                          new XmemPatchPathManager("HSP2_0_G", "hpbtfw20.tlv", "hpnv20g.bin",
                                                   "hpbtxm.tlv", "hpnv20gxm.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(HSP_VER_2_1,
                          new XmemPatchPathManager("HSP2_1", "hpbtfw21.tlv", "hpnv21.bin",
                                                   "hpbtxm.tlv", "hpnv21xm.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(HSP_VER_2_1_G,
                          new XmemPatchPathManager("HSP2_1_G", "hpbtfw21.tlv", "hpnv21g.bin",
                                                    "hpbtxm.tlv", "hpnv21gxm.bin")));
  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(MOSELLE_VER_1_0,
                          new PatchPathManager("MOS1_0", "msbtfw10.tlv", "msnv10.bin")));

  PatchPathInfoMap_.insert(std::make_pair<uint64_t, PatchPathManager*>(MOSELLE_VER_1_1,
                          new ELFPatchPathManager("MOS1_1", "msbtfw11.mbn", "msnv11.bin",
                                                  "msbtfw11.tlv")));
}

void PatchDLManager::ReadELFConfig() {
  if (!elf_config_read_) {
    const char* default_value = "no_value";
    //Enable sequential download by default for moselle.
    if (soc_type_ == BT_SOC_MOSELLE)
      default_value = "0x01";
    bt_logger_->PropertyGet("persist.vendor.bluetooth.patch_config", elf_config_, default_value);
    ALOGI("%s: ELF patch config: %s", __func__, elf_config_);
    elf_config_read_ = true;
  }
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
