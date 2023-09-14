/*==========================================================================
Description
  It has implementation for logger class

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <hidl/HidlSupport.h>
#include <fcntl.h>
#include <errno.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"
#include "uart_controller.h"
#include "hci_transport.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.bluetooth@1.0-logger"

#define SOC_DUMP_FIRST_PKT 0x0000
#define SOC_DUMP_LAST_PKT 0xFFFF

#define SOC_DUMP_PATH_BUF_SIZE 255
#define SOC_DUMP_SIGNATURE_SIZE 24

#define HAL_DUMP_FILE_SIZE 256
#define HAL_DUMP_TIMESTAMP_LENGTH 20
#define HAL_DUMP_TIMESTAMP_OFFSET 23
#define HAL_DUMP_SIZE 85
#ifdef USER_DEBUG
#define HAL_DUMP_ITERATION_LIMIT 80
#else
#define HAL_DUMP_ITERATION_LIMIT 10
#endif

#define BT_VND_SPECIFIC_EVENT    0xFF
#define BT_HW_ERR_EVT_BYTE       0x08
#define BT_HW_ERR_EVT_VALID      0xBD

#define BT_CONTROLLER_LOG        0x01
#define LOG_BT_MESSAGE_TYPE_VSTR     0x02
#define LOG_BT_MESSAGE_TYPE_PACKET   0x05
#define LOG_BT_MESSAGE_TYPE_MEM_DUMP 0x08
#define LOG_BT_MESSAGE_TYPE_HW_ERR   0x09
#define DIAG_SSR_BT_CMD     0x0007

#define LAST_SEQUENCE_NUM 0xFFFF
#define CRASH_REASON_NOT_FOUND  ((char *)"Crash reason not found")

uint8_t clientstatus;
struct crash_reason {
  uint8_t reason;
  time_t time_instance;
} sec_crash_reason;

uint8_t pri_crash_reason;

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

Logger * Logger :: instance_ = NULL;
std::recursive_mutex Logger :: bugreport_mutex;
bool Logger :: is_crash_dump_in_progress_ = false;
pthread_mutex_t Logger :: crash_dump_lock;
pthread_cond_t Logger :: crash_dump_cond;
bool Logger :: is_bugreport_triggered = false;
std::mutex Logger :: bugreport_wait_mutex;
std::condition_variable Logger :: bugreport_wait_cv;
bool Logger :: any_reg_file = false;
bool Logger :: is_bugreport_triggered_during_crash_dump = false;
std::recursive_mutex Logger :: monitor_rx_stats;

Logger * Logger :: Get()
{
  if (!instance_) {
    instance_ = new Logger();
  }
  return instance_;
}

Logger :: Logger()
{
  /*To me initiated dynamically by config input*/
  ibs_log_level_ = MID;
  rx_log_level_ = MID;
  tx_log_level_ = MID;
  gen_log_level_ = MID;
  is_crash_dump_in_progress_ = false;
  crash_dump_status_cb_ = nullptr;
  dump_reason_ = BT_HOST_REASON_SOC_CRASHED;
  init_failed_code_ = BT_SOC_REASON_DEFAULT;
  state_info_  = BtState::Get();
  host_crash_during_init = false;
  stack_timeout_triggered = false;
  is_recovery_time_set_ = false;
  crashcmdneeded = true;
  is_activity_in_progress = false;
  is_delay_list_processed = false;
  InitialIdealTimeTable();
  pc_address_ = 0;
  uart_err_code_ = UART_REASON_DEFAULT;
  hci_buffer_dump_size_ = 0;
  mini_dump_size_ = 0;
  memset(hci_buffer_dump_buff_, 0, sizeof(hci_buffer_dump_buff_));
  memset(mini_dump_buff_, 0, sizeof(mini_dump_buff_));
  is_this_fp_issue = false;
}

bool Logger :: IsSnoopLogEnabled(void)
{
  char value[PROPERTY_VALUE_MAX] = {'\0'};
  PropertyGet("persist.vendor.service.bdroid.snooplog", value, "false");
  return (strcmp(value, "true") == 0);
}

bool Logger :: IsHciFwSnoopEnabled(void)
{
#ifdef USER_DEBUG
  return true;
#else
  char value[PROPERTY_VALUE_MAX] = {'\0'};
  PropertyGet("persist.vendor.service.bdroid.fwsnoop", value, "false");
  return (strcmp(value, "true") == 0);
#endif
}

bool Logger::IsControllerLogPkt(HciPacketType type, const uint8_t *buff, uint16_t len)
{
  if (len < 3) {
    return false;
  } else if ((HCI_PACKET_TYPE_EVENT == type) &&
             (BT_VND_SPECIFIC_EVENT == buff[0]) &&
             (BT_CONTROLLER_LOG == buff[2])) {
    return true;
  } else {
    return false;
  }
}

bool Logger :: IsCrashDumpStarted(void)
{
  return is_crash_dump_in_progress_;
}

bool Logger:: RegisterCrashDumpCallback(CrashDumpStatusCallback crash_dump_cb)
{
  crash_dump_status_cb_ = crash_dump_cb;
  return true;
}

void Logger:: RegisterHWEventCallback(HWEventCallback hw_event_cb)
{
  hw_event_cb_= hw_event_cb;
}

bool Logger :: ProcessRx(HciPacketType type, const uint8_t *buff, uint16_t len)
{
  bool ret  = true;

#ifdef DUMP_RINGBUF_LOG
      state_info_->SetLastRxPacket((uint8_t*)buff, len, type);
#endif
  switch (type) {
    case HCI_PACKET_TYPE_ACL_DATA:
#ifdef DUMP_RINGBUF_LOG
      snoop_buff_.AddLog(RingBuffer::SOC_TO_HOST, type, (uint8_t*)buff, len);
#endif

#ifdef DIAG_ENABLED
      /* Post the event once the Diag init is success */
      if (diag_interface_.GetDiagInitStatus() && IsSnoopLogEnabled()) {
        struct timeval tv;
        char dst_buff[MAX_BUFF_SIZE];

        gettimeofday(&tv, NULL);
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"ACL RX packet going for diag logging");
        BtState::Get()->SetPreTsDiagRxAcl(dst_buff, tv);
        diag_interface_.SendLogs(buff, len, LOG_BT_HCI_RX_ACL_C);
        gettimeofday(&tv, NULL);
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Diag logging done for ACL RX packet");
        BtState::Get()->SetPostTsDiagRxAcl(dst_buff, tv);
      }
#endif
      break;

    case HCI_PACKET_TYPE_EVENT:
      ret =  HandleHciEvent(type, (uint8_t*)buff, len);

#ifdef DIAG_ENABLED
      /* Post the event once the Diag init is success */
      if (diag_interface_.GetDiagInitStatus() && IsSnoopLogEnabled()) {
        struct timeval tv;
        char dst_buff[MAX_BUFF_SIZE];

        gettimeofday(&tv, NULL);
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"HCI RX packet going for diag logging");
        BtState::Get()->SetPreTsDiagRxHciEvt(dst_buff, tv);
        diag_interface_.SendLogs(buff, len, LOG_BT_HCI_EV_C);
        gettimeofday(&tv, NULL);
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Diag logging done for HCI RX packet");
        BtState::Get()->SetPostTsDiagRxHciEvt(dst_buff, tv);
      }
      break;
#endif

    case HCI_PACKET_TYPE_FM_EVENT:
    case HCI_PACKET_TYPE_ANT_CTRL:
    case HCI_PACKET_TYPE_ANT_DATA:
#ifdef DUMP_RINGBUF_LOG
      snoop_buff_.AddLog(RingBuffer::SOC_TO_HOST, type, (uint8_t*)buff, len);
#endif
     break;
    default:
      ret =  false;
      break;
  }
  return ret;
}

bool Logger :: ProcessTx(HciPacketType type, const uint8_t *buff, uint16_t len)
{
#ifdef DUMP_RINGBUF_LOG
  snoop_buff_.AddLog(RingBuffer::HOST_TO_SOC, type, (uint8_t*)buff, len);
  state_info_->SetLastTxPacket((uint8_t*)buff, len, type);
#endif

#ifdef DIAG_ENABLED
  /* Post the event once the Diag init is success */
  if (diag_interface_.GetDiagInitStatus() && IsSnoopLogEnabled()) {
    struct timeval tv;
    char dst_buff[MAX_BUFF_SIZE];

    if (type == HCI_PACKET_TYPE_COMMAND) {
      gettimeofday(&tv, NULL);
      BtState::Get()->AddLogTag(dst_buff, tv, (char *)"HCI TX packet going for diag logging");
      BtState::Get()->SetPreTsDiagTxHciCmd(dst_buff, tv);
      diag_interface_.SendLogs(buff, len, LOG_BT_HCI_CMD_C);
      gettimeofday(&tv, NULL);
      BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Diag logging done for HCI TX packet");
      BtState::Get()->SetPostTsDiagTxHciCmd(dst_buff, tv);
    } else if (type == HCI_PACKET_TYPE_ACL_DATA) {
      gettimeofday(&tv, NULL);
      BtState::Get()->AddLogTag(dst_buff, tv, (char *)"ACL TX packet going for diag logging");
      BtState::Get()->SetPreTsDiagTxAcl(dst_buff, tv);
      diag_interface_.SendLogs(buff, len, LOG_BT_HCI_TX_ACL_C);
      gettimeofday(&tv, NULL);
      BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Diag logging done for ACL TX packet");
      BtState::Get()->SetPostTsDiagTxAcl(dst_buff, tv);
    }
  }
#endif

  return true;
}

void Logger :: DecodeHwErrorEvt(uint8_t *buff)
{
  /* We receive dump via an VSE command, the dump reason hold two
   * bytes, one is fixed byte and other holds the reason for crash.
   *
   * SoC crash reasons should always have MSB set,
   * incase if it is not set, set MSB bit - 7th bit
   * (assuming indexing from 0)
   */
  SecondaryReasonCode reason = (SecondaryReasonCode)(buff[BT_HW_ERR_EVT_BYTE] | BT_SOC_ERR_MASK);
  ALOGE("Rx VSE HW error event::%s\n", get_reset_reason_str(reason));
  mini_dump_size_ = buff[DUMP_PKT_SIZE_OFFSET] - (MINI_DUMP_ARM_REG_SIZE_OFFSET -
                                    DUMP_PKT_SIZE_OFFSET - 1);
  // Update minidump data if arm reg. and call stack data are present.
  if (mini_dump_size_ > 0) {
    memcpy(mini_dump_buff_, buff + MINI_DUMP_ARM_REG_SIZE_OFFSET, mini_dump_size_);
  } else {
    mini_dump_size_ = 0;
  }
  ALOGD("%s mini_dump_size = %d", __func__, mini_dump_size_);

  if (hw_event_cb_)
    hw_event_cb_();
  if (isDiagSsrTriggered()) {
    dump_reason_ = BT_HOST_REASON_SOC_CRASHED_DIAG_SSR;
    SetPrimaryCrashReason(dump_reason_);
  }
  /* Incase of crash triggered by host during init
   * secondary reason is already set and we dont
   * set reason given by SoC.
   * If we already got valid UART err code then skip setting secondary reason.
   */
  if (host_crash_during_init == false && GetUartErrCode() == UART_REASON_DEFAULT) {
    SetSecondaryCrashReason(reason);
  }
}

bool Logger :: HandleHciEvent(HciPacketType type, uint8_t *buff, uint16_t len)
{
  bool ret = true;

  if ((buff[0] == BT_VND_SPECIFIC_EVENT) &&
      (buff[2] == BT_CONTROLLER_LOG)) {
    switch (buff[3]) {
      case LOG_BT_MESSAGE_TYPE_VSTR:
      case LOG_BT_MESSAGE_TYPE_PACKET:
#ifdef DUMP_RINGBUF_LOG
        snoop_buff_.AddLog(RingBuffer::SOC_TO_HOST, type, buff, len);
#endif
        break;

      case LOG_BT_MESSAGE_TYPE_MEM_DUMP:
      {
        if (isDiagSsrTriggered())
          dump_reason_ = BT_HOST_REASON_SOC_CRASHED_DIAG_SSR;

        /* Set SSR triggered flag if crash automatically
         * happened from SoC side.
         */
        if (SetSsrTriggeredFlag()) {
          DataHandler * instance = DataHandler::Get();
          SetPrimaryCrashReason(BT_HOST_REASON_SOC_CRASHED);
          // Stop init timer  during init phase.
          if (instance) {
              if (instance->GetInitStatus() == INIT_STATUS_INITIALIZING) {
                instance->StopInitTimer();
              }
              /* Check for valid UART err code and err code should not be
               * "SoC RFR high" as SoC might have intentionally set RFR line high
               * inorder to not rcv any data from host while its sending dumps.
               * If UART err code is "SoC RFR high" reset it to default inorder
               * to ignore it in case SoC crash itself case.
               */
              if (instance->CheckForUartFailureStatus()) {
                if (GetUartErrCode() == SOC_REASON_START_TX_IOS_SOC_RFR_HIGH ||
                    GetUartErrCode() == SOC_REASON_START_TX_IOS_SOC_RFR_HIGH_DURING_INIT) {
                  ALOGI("%s: Ignoring SoC rfr high err code returned by UART",
                          __func__);
                  ALOGD("%s: Reset UART err code to default", __func__);
                  SetUartErrCode(UART_REASON_DEFAULT);
                } else {
                  SetSecondaryCrashReason(GetUartErrCode());
                }
              }
              // log all power srcs and UART flow ctrl even if SoC crashed itself.
              instance->LogPwrSrcsUartFlowCtrl();
          }
        }

        SaveSocMemDump(buff, len, dump_reason_);
      }
      break;

      case LOG_BT_MESSAGE_TYPE_HW_ERR:
        if (buff[1] >= BT_HW_ERR_EVT_BYTE &&
            buff[9] == BT_HW_ERR_EVT_VALID) {
          /* Set SSR triggered flag if crash automatically
           * happened from SoC side.
           */
          if (SetSsrTriggeredFlag()) {
            DataHandler * instance = DataHandler::Get();
            SetPrimaryCrashReason(BT_HOST_REASON_SOC_CRASHED);
            // Stop init timer during init phase.
            if (instance) {
              if (instance->GetInitStatus() == INIT_STATUS_INITIALIZING) {
                instance->StopInitTimer();
              }
              /* Check for valid UART err code and err code should not be
               * "SoC RFR high" as SoC might have intentionally set RFR line high
               * inorder to not rcv any data from host while its sending dumps.
               * If UART err code is "SoC RFR high" reset it to default inorder
               * to ignore it in case SoC crash itself case.
               */
              if (instance->CheckForUartFailureStatus()) {
                if (GetUartErrCode() == SOC_REASON_START_TX_IOS_SOC_RFR_HIGH ||
                    GetUartErrCode() == SOC_REASON_START_TX_IOS_SOC_RFR_HIGH_DURING_INIT) {
                  ALOGI("%s: Ignoring SoC rfr high err code returned by UART",
                          __func__);
                  ALOGD("%s: Reset UART err code to default", __func__);
                  SetUartErrCode(UART_REASON_DEFAULT);
                } else {
                  SetSecondaryCrashReason(GetUartErrCode());
                }
              }
              // log all power srcs and UART flow ctrl even if SoC crashed itself.
              instance->LogPwrSrcsUartFlowCtrl();
            }
          }
          DecodeHwErrorEvt(buff);
#ifdef DUMP_RINGBUF_LOG
          snoop_buff_.AddLog(RingBuffer::SOC_TO_HOST, type, buff, len);
#endif
        }
        else
          ALOGE("Rx HW error event with wrong payload");
        break;
      case LOG_BT_HCI_BUFFER_DUMP:
      {
        ALOGD("%s : Rcvd HCI BUFFER DUMP event of size %d", __func__,
               buff[HCI_BUFFER_DUMP_SIZE_OFFSET]);
        hci_buffer_dump_size_ = buff[HCI_BUFFER_DUMP_SIZE_OFFSET];
        memcpy(hci_buffer_dump_buff_, buff + HCI_BUFFER_DUMP_SIZE_OFFSET + 1,
                hci_buffer_dump_size_);
      }
      break;
      default:
        ret = false;
        break;
    }
  } else if (buff[0] == BT_HW_ERR_EVT) {
    /* Check the size of the event */
    if (buff[1] == BT_HW_ERR_FRAME_SIZE) {
      ALOGE("Rx HW error event::%s\n", get_reset_reason_str((SecondaryReasonCode)buff[2]));
      /* Incase of crash triggered by host during init
       * secondary reason is already set and we dont
       * set reason given by SoC.
       * If we already got valid UART err code then skip setting secondary reason.
       */
      if (host_crash_during_init == false && GetUartErrCode() == UART_REASON_DEFAULT) {
        /* SoC crash reasons should always have MSB set,
         * incase if it is not set, set MSB bit - 7th bit
         * (assuming indexing from 0)
         */
        SetSecondaryCrashReason((SecondaryReasonCode)(buff[2]  | BT_SOC_ERR_MASK));
      }
      if (isDiagSsrTriggered()) {
       dump_reason_ = BT_HOST_REASON_SOC_CRASHED_DIAG_SSR;
       SetPrimaryCrashReason(dump_reason_);
      }
    } else {
      ALOGE("Rx HW error event with wrong payload");
    }
#ifdef DUMP_RINGBUF_LOG
    snoop_buff_.AddLog(RingBuffer::SOC_TO_HOST, type, buff, len);
#endif
    if (hw_event_cb_)
      hw_event_cb_();
  } else {
#ifdef DUMP_RINGBUF_LOG
    snoop_buff_.AddLog(RingBuffer::SOC_TO_HOST, type, buff, len);
#endif
  }
  return ret;
}

#ifdef DIAG_ENABLED
const diagpkt_user_table_entry_type Logger :: ssr_bt_tbl_[] = {
  { DIAG_SSR_BT_CMD, DIAG_SSR_BT_CMD, SsrBtHandler },
};
#endif

void Logger :: UnlockRingbufferMutex()
{
#ifdef DUMP_RINGBUF_LOG
  snoop_buff_.UnlockMutex();
#endif
}

#ifdef DIAG_ENABLED
void * Logger :: SsrBtHandler(void *req_pkt, uint16_t pkt_len)
{
  Logger *logger = Logger :: Get();

  if (logger) {
    return logger->diag_interface_.SsrBtHandler(req_pkt, pkt_len);
  } else {
    return nullptr;
  }
}
#endif

char * Logger::get_reset_reason_str(SecondaryReasonCode reason)
{
  for(int i =0; i < (int)(sizeof(secReasonMap)/sizeof(SecondaryReasonMap)); i++)
    if (secReasonMap[i].reason == reason)
      return secReasonMap[i].reasonstr;

  return CRASH_REASON_NOT_FOUND;
}

void Logger::StoreCrashReason()
{
  char temp[MAX_CRASH_BUFF_SIZE] = {'\0'};
  char buff[MAX_CRASH_BUFF_SIZE] = {'\0'};
  struct tm *timeptr = NULL;

  if (sec_crash_reason.reason == BT_SOC_REASON_DEFAULT &&
      pri_crash_reason == BT_HOST_REASON_RX_THREAD_STUCK) {
      timeptr = localtime(&time_rx_call_back);
      if (timeptr)
        snprintf(temp, MAX_CRASH_BUFF_SIZE, "%s", asctime(timeptr));
  }
  else {
      timeptr = localtime(&sec_crash_reason.time_instance);
      if (timeptr)
        snprintf(temp, MAX_CRASH_BUFF_SIZE, "%s", asctime(timeptr));
  }

  if (pri_crash_reason == BT_HOST_REASON_DEFAULT_NONE)
    pri_crash_reason = BT_HOST_REASON_SOC_CRASHED;

  snprintf(buff, MAX_CRASH_BUFF_SIZE, "BtPrimaryCrashReason:%s",
           snoop_buff_.GetPrimaryReason((PrimaryReasonCode)pri_crash_reason));
  ALOGI("%s", buff);
  state_info_->SetPrimaryCrashReason(buff);

  snprintf(buff, MAX_CRASH_BUFF_SIZE, "BtSecondaryCrashReason:%s",
           get_reset_reason_str((SecondaryReasonCode)sec_crash_reason.reason));
  ALOGI("%s", buff);
  state_info_->SetSecondaryCrashReason(buff);

  snprintf(buff, MAX_CRASH_BUFF_SIZE, "TS for SoC Crash:%s", temp);
  ALOGI("%s", buff);
  state_info_->SetCrashTS(buff);

}

void Logger::FrameCrashEvent(hidl_vec<uint8_t>*bt_packet_)
{
  unsigned char bt_eve_buf[PROPERTY_VALUE_MAX] = { 0xff, 0x0e, 0xfc, 0x00};
  int count = 4;
  char crash_ts[MAX_CRASH_BUFF_SIZE];

  /* LSB byte holds the crash reason, for safer side keeping in
   * view of future implementations using lower two LSB bytes.
   */
  bt_eve_buf[count] = ((uint8_t) ((pri_crash_reason) & 0xFF));
  bt_eve_buf[count + 1] = ((uint8_t) ((pri_crash_reason >> 0x08) & 0xFF));
  bt_eve_buf[count + 2] = ((uint8_t) ((sec_crash_reason.reason) & 0xFF));
  bt_eve_buf[count + 3] = ((uint8_t) ((sec_crash_reason.reason >> 0x08) & 0xFF));
  count += 4;
  if (sec_crash_reason.reason == BT_SOC_REASON_DEFAULT &&
      pri_crash_reason == BT_HOST_REASON_RX_THREAD_STUCK)
      snprintf(crash_ts, MAX_CRASH_BUFF_SIZE, "%s", asctime(localtime(&time_rx_call_back)));
  else
      snprintf(crash_ts, MAX_CRASH_BUFF_SIZE, "%s",
               asctime(localtime(&sec_crash_reason.time_instance)));
  /* copy Crash TS string */
  memcpy(bt_eve_buf + count, crash_ts, strlen(crash_ts));
  count = count + strlen(crash_ts);
  /*Discard the separator after time stamp */
  count = count - 1;
  bt_eve_buf[1] = count - 2;
  bt_packet_->resize(count);
  memcpy(bt_packet_->data(), bt_eve_buf, count);
  ALOGI("%s: for primary %d - secondary %d crash reason with TS:%s",
        __func__, pri_crash_reason, sec_crash_reason.reason, crash_ts);

}


void Logger::FrameBqrRieEvent(hidl_vec<uint8_t>*bt_packet_)
{

  /* Crash reason frame format:
   * 0xff for VSE | 0x04 for parameter length | 0x58 for BQR sub-event code
   * | 0x05 for root inflammation event | 0x00 for error code | 0x** for vendor
   * specific error code(crash reason) | [optional: 0x** for number of vendor specific
   * parameters | 0x** for first parameter id | 0x** for length of first parameter |
   * 0x** for first parameter value | ..so on..]
   */
  unsigned char bt_eve_buf[MAX_BQR_RIE_PKT_SIZE] = { 0xff, 0x04, 0x58, 0x05, 0x00};
  const uint8_t para_len_offset = 1;
  int count = BQR_RIE_VS_ERR_CODE_OFFSET;
  int remaining_bytes_for_vs_params = MAX_BQR_VS_PARAMS_SIZE;
  uint8_t crash_reason;
  std::string crash_string;
  std::stringstream ss;

  /* filtering out crash reasons to be sent to client */
  if (sec_crash_reason.reason == BT_SOC_REASON_DEFAULT) {
    crash_reason = pri_crash_reason;
    crash_string = std::string(snoop_buff_.GetPrimaryReason((PrimaryReasonCode)pri_crash_reason));
  } else if (pri_crash_reason == BT_HOST_REASON_INIT_FAILED
             || pri_crash_reason == BT_HOST_REASON_SOC_CRASHED
             || sec_crash_reason.reason == BT_HOST_REASON_DIAGDEINIT_STUCK
             //Check if we have a valid UART err code available.
             || (sec_crash_reason.reason > UART_ERR_CODE_LOWER_BOUND
             && sec_crash_reason.reason <= UART_ERR_CODE_UPPER_BOUND)) {
    crash_reason = sec_crash_reason.reason;
    crash_string = std::string(get_reset_reason_str((SecondaryReasonCode)sec_crash_reason.reason));
  } else {
    crash_reason = pri_crash_reason;
    crash_string = std::string(snoop_buff_.GetPrimaryReason((PrimaryReasonCode)pri_crash_reason));
  }

  BtState ::Get()->UpdateBqrRieErrCodeAndErrString(crash_reason, crash_string);
  /* Storing carsh reason byte in crash packet */
  bt_eve_buf[count] = crash_reason;
  count++;

  auto update_bqr_pkt = [&] (size_t buffer_size, char* buffer,
                             BqrRieVsParamsOrder param_id) {
   /* Increment parameter length:
    * 1 for parameter id + 1 for parameter length + parameter value lenght)
    */
    bt_eve_buf[para_len_offset] += BQR_RIE_VSP_META_DATA_SIZE + buffer_size;
    remaining_bytes_for_vs_params -= BQR_RIE_VSP_META_DATA_SIZE + buffer_size;
    ALOGD("%s : para_len = %d,  remaining_bytes_for_vs_params = %d", __func__,
            bt_eve_buf[para_len_offset], remaining_bytes_for_vs_params);
    // Add first VS parameter
    bt_eve_buf[BQR_RIE_VS_PARAM_COUNT_OFFSET] += 1;
    bt_eve_buf[count++] = param_id;
    bt_eve_buf[count++] = buffer_size;
    memcpy(bt_eve_buf + count, (unsigned char*)buffer, buffer_size);
    count += buffer_size;
    for (int i = 0; i < buffer_size; ++i) {
      ss <<  std::uppercase << std::hex << (int)buffer[i] << " ";
    }
  };

  // Send Pc address only if SoC crash itself and no UART err
  if (pri_crash_reason == BT_HOST_REASON_SOC_CRASHED
      && GetUartErrCode() == UART_REASON_DEFAULT) {
    // 1 byte assigned for number of VS parameters.
    count++;
    bt_eve_buf[para_len_offset] += 1;
    remaining_bytes_for_vs_params -= 1;

    if (pc_address_ != 0 and
          remaining_bytes_for_vs_params >= (sizeof(uint32_t) + BQR_RIE_VSP_META_DATA_SIZE)) {
      ss << "    PC address : ";
      update_bqr_pkt(sizeof(uint32_t), (char*)&pc_address_, PC_ADDRESS);
      ss << "\n";
      ALOGD("%s : Sending PC address = 0x%x along with BQR RIE", __func__, pc_address_);
    }
    if (hci_buffer_dump_size_ and remaining_bytes_for_vs_params > BQR_RIE_VSP_META_DATA_SIZE) {
      int size_diff = 0;
      if (hci_buffer_dump_size_ + BQR_RIE_VSP_META_DATA_SIZE > remaining_bytes_for_vs_params) {
        size_diff = hci_buffer_dump_size_ +
                      BQR_RIE_VSP_META_DATA_SIZE - remaining_bytes_for_vs_params;
      }
      ALOGD ("%s : size_diff = %d", __func__, size_diff);
      hci_buffer_dump_size_ = std::min((int)hci_buffer_dump_size_,
                                        remaining_bytes_for_vs_params - BQR_RIE_VSP_META_DATA_SIZE);
      ss << "    HCI BUFFER DUMP : ";
      /* Skip first few bytes which can not be accommodated
         in remaining bytes for BQR RIE VS params */
      update_bqr_pkt(hci_buffer_dump_size_, hci_buffer_dump_buff_ + size_diff, HCI_BUFFER_DUMP);
      ss << "\n";
      ALOGD("%s : Sending hci buffer dump of size %d along with BQR RIE", __func__,
            hci_buffer_dump_size_);
    } else if (mini_dump_size_ and remaining_bytes_for_vs_params > BQR_RIE_VSP_META_DATA_SIZE) {
      ss << "    MINI DUMP : ";
      mini_dump_size_ = std::min((int)mini_dump_size_,
                                  remaining_bytes_for_vs_params - BQR_RIE_VSP_META_DATA_SIZE);
      update_bqr_pkt(mini_dump_size_, mini_dump_buff_, MINI_DUMP);
      ss << "\n";
      ALOGD("%s : Sending minidump of size %d along with BQR RIE", __func__,
            mini_dump_size_);
    }
  }

  if (ss.str().length() > 0) {
    std::string params = "BQR RIE VS PARAMS:\n";
    params += ss.str();
    BtState ::Get()->UpdateBQRVSParams(params);
  }

  bt_packet_->resize(count);
  memcpy(bt_packet_->data(), bt_eve_buf, count);
  ALOGD("%s : pkt size = %d", __func__, count);
  ALOGI("%s: for crash reason code :0x%x", __func__, crash_reason);
}

void Logger :: SaveSocMemDump(uint8_t *eventBuf, uint16_t packet_len, PrimaryReasonCode reason)
{
  static unsigned int dump_size = 0, total_size = 0;
  unsigned short seq_num = 0;
  static unsigned short seq_num_cnt = 0;
  uint8_t *dump_ptr = NULL;
  static char *temp_buf, *p, path[SOC_DUMP_PATH_BUF_SIZE + 1];
  static int dump_fd = -1;
  char nullBuff[255] = { 0 };
  char dst_buff[MAX_BUFF_SIZE] = {'\0'};
  dump_ptr = &eventBuf[7];
  seq_num = eventBuf[4] | (eventBuf[5] << 8);
  packet_len -= 7;

  ALOGV("%s: LOG_BT_MESSAGE_TYPE_MEM_DUMP (%d) ", __func__, seq_num);

  if ((seq_num != seq_num_cnt) && seq_num != LAST_SEQUENCE_NUM) {
    ALOGE("%s: current sequence number: %d, expected seq num: %d ", __func__,
          seq_num, seq_num_cnt);
  }
  std::unique_lock<std::recursive_mutex> guard(Logger::bugreport_mutex);
  if (seq_num == 0x0000) {
    PrepareDumpProcess();
    SetPrimaryCrashReason(reason);
    CollectDumps(true, false);
    dump_size = (unsigned int)
                (eventBuf[7] | (eventBuf[8] << 8) | (eventBuf[9] << 16) | (eventBuf[10] << 24));
    dump_ptr = &eventBuf[11];
    total_size = seq_num_cnt = 0;
    packet_len -= 4;

    //memset(path, 0, SOC_DUMP_PATH_BUF_SIZE);
    /* first pack has total ram dump size (4 bytes) */
    ALOGD("%s: Crash Dump Start - total Size: %d ", __func__, dump_size);
    is_crash_dump_in_progress_ = true;
    p = temp_buf = (char*)malloc(dump_size);
    if (p != NULL) {
      memset(p, 0, dump_size);
    } else {
      ALOGE("Failed to allocate mem for the crash dump size: %d", dump_size);
    }

    GetCrashDumpFileName(path);
    dump_fd = open(path, O_CREAT | O_SYNC | O_WRONLY,  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (dump_fd < 0) {
      ALOGE("%s: File open (%s) failed: errno: %d", __func__, path, errno);
      seq_num_cnt++;
      return;
    }
    ALOGV("%s: File Open (%s) successfully ", __func__, path);
  }

  if (dump_fd >= 0) {
    for (; (seq_num > seq_num_cnt) && (seq_num != LAST_SEQUENCE_NUM); seq_num_cnt++) {
      ALOGE("%s: controller missed packet : %d, write null (%d) into file ",
            __func__, seq_num_cnt, packet_len);

      if (p != NULL) {
        memcpy(temp_buf, nullBuff, packet_len);
        temp_buf = temp_buf + packet_len;
      }
    }

    if (p != NULL) {
      memcpy(temp_buf, dump_ptr, packet_len);
      temp_buf = temp_buf + packet_len;
    }
    total_size += packet_len;
  }

  seq_num_cnt++;
  if (seq_num == LAST_SEQUENCE_NUM && p != NULL) {
    char dst_buff[MAX_BUFF_SIZE] = {'\0'};
    char seq_buff[MAX_BUFF_SIZE] = {'\0'};
    struct timeval tv;

    ALOGE("Writing crash dump of size %d bytes", total_size);
    gettimeofday(&tv, NULL);
    snprintf(dst_buff, sizeof(dst_buff), "Last sequence num %d rcvd from SOC", seq_num_cnt);
    BtState ::Get()->AddLogTag(seq_buff, tv, dst_buff);
    BtState ::Get()->SetLastSeqTS(seq_buff);
    write(dump_fd, p, total_size);
    free(p);
    temp_buf = NULL;
    p = NULL;
    seq_num_cnt = 0;
  }

  if (seq_num == LAST_SEQUENCE_NUM) {
    if (dump_fd >= 0) {
      if ( fsync(dump_fd) < 0 ) {
        ALOGE("%s: File flush (%s) failed: %s, errno: %d", __func__,
              path, strerror(errno), errno);
      }
      close(dump_fd);
      dump_fd = -1;

      ALOGI("%s: Write crashdump successfully : \n"
            "\t\tFile: %s\n\t\tdump_size: %d\n\t\twritten_size: %d",
            __func__, path, dump_size, total_size );
      int cntr = 0;
      size_t readsize;
      unsigned char *dumpinfo, *tempptr;
      uint32_t ucFilename;
      uint32_t uLinenum;
      uint32_t uPCAddr;
      uint32_t uResetReason;
      uint32_t uBuildVersion;
      dump_fd = -1;
      int i = 0;
      char filenameBuf [SOC_DUMP_PATH_BUF_SIZE];
      char *decode_reason = NULL;

      memset(filenameBuf, 0, SOC_DUMP_PATH_BUF_SIZE);
      if (-1 != (dump_fd = open(path, O_RDONLY))) {
        if (NULL != (dumpinfo = (unsigned char*)malloc(SOC_DUMP_SIGNATURE_SIZE))) {
          tempptr = dumpinfo;
          lseek(dump_fd, 0xFEE0, SEEK_SET);
          readsize = SOC_DUMP_SIGNATURE_SIZE;
          while (readsize) {
            cntr = read(dump_fd, (void*)tempptr, readsize);
            tempptr += cntr;
            readsize -= cntr;
          }

          tempptr = dumpinfo;
          memcpy(&ucFilename, tempptr, 4); tempptr += 4;
          memcpy(&uLinenum, tempptr, 4); tempptr += 4;
          memcpy(&uPCAddr, tempptr, 4); tempptr += 4;
          memcpy(&uResetReason, tempptr, 4); tempptr += 4;
          memcpy(&uBuildVersion, tempptr, 4); tempptr += 4;
          /* SoC crash reasons should always have MSB set,
           * incase if it is not set, set MSB bit - 7th bit
           * (assuming indexing from 0)
           */
          uResetReason = uResetReason | BT_SOC_ERR_MASK;

          if (0 != ucFilename) {
            lseek(dump_fd, (off_t)ucFilename, SEEK_SET);
            cntr = read(dump_fd, (void*)filenameBuf, SOC_DUMP_PATH_BUF_SIZE);
            while (i < SOC_DUMP_PATH_BUF_SIZE && filenameBuf[i++] != '\0' ) ;
            if (i < SOC_DUMP_PATH_BUF_SIZE) {
              ALOGE("Filename::%s\n", filenameBuf);
            }
          }

          decode_reason = get_reset_reason_str((SecondaryReasonCode)uResetReason);
          ALOGE("Linenum::%d\n", uLinenum);
          ALOGE("PC Addr::0x%x\n", uPCAddr);
          pc_address_ = uPCAddr;
          ALOGE("Reset reason::%s\n", decode_reason);
          ALOGE("Reset reason::%d\n", uResetReason);
          ALOGE("Build Version::0x%x\n", uBuildVersion);
          /* Incase of crash triggered by host during init
           * secondary reason is already set and we dont
           * set reason given by SoC.
           */
          if(host_crash_during_init == false && GetUartErrCode() == UART_REASON_DEFAULT) {
            SetSecondaryCrashReason((SecondaryReasonCode)uResetReason);
          }

         snprintf(dst_buff, MAX_BUFF_SIZE, (char *) "PC Addr::0x%x", uPCAddr);
         state_info_->SetPCAddr(dst_buff);
         memset(dst_buff, 0, MAX_BUFF_SIZE);
         snprintf(dst_buff, MAX_BUFF_SIZE, (char *) "Build Version::0x%x", uBuildVersion);
         state_info_->SetBuildVersion(dst_buff);
        }
        if (NULL != dumpinfo) free(dumpinfo);
        close(dump_fd);
      }
    }

    is_crash_dump_in_progress_ = false;
  }

  if ( (0 == seq_num || LAST_SEQUENCE_NUM == seq_num) && crash_dump_status_cb_) {
    crash_dump_status_cb_(is_crash_dump_in_progress_);
  }
}

void Logger :: Init(HciTransport *hci_transport)
{
  bool status = false;
#ifdef DIAG_ENABLED
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  SetSecondaryCrashReason(BT_HOST_REASON_DIAGINIT_STUCK);
#endif

  status = diag_interface_.Init(hci_transport);

#ifdef DIAG_ENABLED
  gettimeofday(&tv, NULL);
  memset(dst_buff, 0, MAX_BUFF_SIZE);
  if (status) {
    state_info_->AddLogTag(dst_buff, tv, (char *)"Diag Init successful");

    ALOGD("%s: Registered Diag callbacks", __func__);
    DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_BT, ssr_bt_tbl_);
  } else {
    state_info_->AddLogTag(dst_buff, tv, (char *)"Diag Init failed");
  }
  state_info_->SetDiagInitStatus(dst_buff);
#endif

}


void Logger :: Cleanup(void)
{
  diag_interface_.CleanUp();
#ifdef DIAG_ENABLED
  if (DiagInterface::GetDiagDeInitTimeoutTriggered())
    return;
#endif

  if (instance_) {
    delete instance_;
    instance_ = NULL;
  }
}

void Logger :: GetCrashDumpFileName(char* path)
{
  snprintf(path, SOC_DUMP_PATH_BUF_SIZE, SOC_DUMP_PATH, time_year, time_month, time_day, time_hour, time_min, time_sec);
  dump_files.push_back(std::string(path));
}

#ifdef DUMP_IPC_LOG
void Logger :: GetUartDumpFilename(short int uart_dump, char* path)
{
  switch (uart_dump){
  case IpcLogs::TX_LOG:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_IPC_TX_LOGS_DEST_PATH, time_year, time_month, time_day, time_hour, time_min, time_sec);
    dump_files.push_back(std::string(path));
    break;
  case IpcLogs::RX_LOG:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_IPC_RX_LOGS_DEST_PATH, time_year, time_month, time_day, time_hour, time_min, time_sec);
    dump_files.push_back(std::string(path));
    break;
  case IpcLogs::STATE_LOG:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_IPC_STATE_LOGS_DEST_PATH, time_year, time_month, time_day, time_hour, time_min, time_sec);
    dump_files.push_back(std::string(path));
    break;
  case IpcLogs::POWER_LOG:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_IPC_PWR_LOGS_DEST_PATH, time_year, time_month, time_day, time_hour, time_min, time_sec);
    dump_files.push_back(std::string(path));
    break;
  }
}
#endif

#ifdef DUMP_RINGBUF_LOG
void Logger :: GetSnoopFileName(char* path)
{
  snprintf(path, BT_FILE_NAME_LEN, SNOOP_FILE_PATH, time_year, time_month,
           time_day, time_hour, time_min, time_sec);
  dump_files.push_back(std::string(path));
}
#endif

void Logger :: GetStateFileName(char* path)
{
  snprintf(path, BT_FILE_NAME_LEN, BT_STATE_FILE_PATH, time_year, time_month,
           time_day, time_hour, time_min, time_sec);
  dump_files.push_back(std::string(path));
}

bool Logger::PrepareDumpProcess()
{
  SetDumpTimestamp();
#ifndef DONT_DELETE_DUMPS_SET
  DeleteDumpsIfRequired();
#endif
  return true;
}

void Logger::SetDumpTimestamp()
{
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  if (tm) {
    time_year = tm->tm_year + 1900;
    time_month = tm->tm_mon+ 1;
    time_day = tm->tm_mday;
    time_hour = tm->tm_hour;
    time_min = tm->tm_min;
    time_sec = tm->tm_sec;
  }
}

void Logger :: DeleteCurrentDumpedFiles() {
  ALOGD("%s: Delete current collected logs as not required", __func__);
  for(std::string file: dump_files) {
    ALOGD("%s: removing %s", __func__, file.c_str());
    remove(file.c_str());
  }
}

void Logger :: DeleteDumpsIfRequired()
{
  struct DIR* p_dir;
  struct dirent* p_dirent;
  char *first_entry = NULL;
  int count = 0;

#ifdef DUMP_IPC_LOG
  char *look_for = (char *)"uart_ipc_rx";
#else
  char *look_for = (char *)"fw_crashdump";
#endif

  p_dir = opendir(LOG_COLLECTION_DIR);
  if (p_dir == NULL) {
    ALOGE("%s: Cannot open dump location %s", __func__, LOG_COLLECTION_DIR);
    return;
  }
  while ((p_dirent = readdir(p_dir)) != NULL) {
    if (strstr(p_dirent->d_name, look_for)) {
      count++;
      if(count == 1) {
        first_entry = p_dirent->d_name;
      }
    }
  }
  if (count >= HAL_DUMP_ITERATION_LIMIT)
    DeleteDumps(first_entry);

  closedir(p_dir);
}

void Logger :: DeleteDumps(char *first_entry)
{
  struct DIR* p_dir;
  struct dirent* p_dirent;
  int ret = 0;

#ifdef DUMP_IPC_LOG
  char *look_for = (char *)"uart_ipc_rx";
#else
  char *look_for = (char *)"fw_crashdump";
#endif
  char timestamp[HAL_DUMP_TIMESTAMP_LENGTH];
  char path[HAL_DUMP_SIZE];
  char oldest_file[HAL_DUMP_FILE_SIZE];

  strlcpy(oldest_file, first_entry, HAL_DUMP_FILE_SIZE);

  p_dir = opendir(LOG_COLLECTION_DIR);
  if (p_dir == NULL) {
    ALOGE("%s: Cannot open dump location %s", __func__, LOG_COLLECTION_DIR);
    return;
  }

  while ((p_dirent = readdir(p_dir)) != NULL) {
    if (strstr(p_dirent->d_name, look_for) ) {
      if(strcmp(p_dirent->d_name, oldest_file) < 0) {
        strlcpy(oldest_file, p_dirent->d_name, HAL_DUMP_FILE_SIZE);
      }
    }
  }
  closedir(p_dir);

#ifdef DUMP_IPC_LOG
  strlcpy(timestamp, oldest_file + HAL_DUMP_TIMESTAMP_OFFSET, HAL_DUMP_TIMESTAMP_LENGTH);
#else
  strlcpy(timestamp, oldest_file + HAL_DUMP_TIMESTAMP_OFFSET + 1, HAL_DUMP_TIMESTAMP_LENGTH);
#endif

  p_dir = opendir(LOG_COLLECTION_DIR);
  if (p_dir == NULL) {
    ALOGE("%s: Cannot open dump location %s", __func__, LOG_COLLECTION_DIR);
    return;
  }

  while ((p_dirent = readdir(p_dir)) != NULL) {
   if (strstr(p_dirent->d_name, timestamp)) {
     strlcpy(path, LOG_COLLECTION_DIR, HAL_DUMP_SIZE);
     strlcat(path, p_dirent->d_name, HAL_DUMP_SIZE);
     ALOGE("%s: Deleting oldest dump file: %s", __func__, path);
     ret = remove(path);
     if(ret < 0) ALOGE("%s: Cannot delete file %s", __func__, path);
   }
  }
  closedir(p_dir);
}

void Logger:: ResetCrashReasons()
{
  pri_crash_reason  = BT_HOST_REASON_DEFAULT_NONE;
  sec_crash_reason.reason = BT_SOC_REASON_DEFAULT;
  sec_crash_reason.time_instance = 0;
}

void Logger:: SetPrimaryCrashReason(PrimaryReasonCode reason)
{
  pri_crash_reason  = reason;
  dump_reason_ = reason;
   /* Some host trigger crashes don't get crash dump, mapping the
    * time stamp at the time of primary reason. If we have dump
    * collected will take collection time stamp into account.
    */
  sec_crash_reason.time_instance = time(NULL);
}

void Logger:: SetSecondaryCrashReason(SecondaryReasonCode reason)
{
  /* Dont set secondary crash reason when inittimeout already occured.
   * As issue is already occured we will not try to change secondary reason
   * w.r.t init phase.
   */
  if (host_crash_during_init == true)
    return;

  sec_crash_reason.reason = reason;
  sec_crash_reason.time_instance = time(NULL);
}

char * Logger:: GetSecondaryCrashReason()
{
  return get_reset_reason_str((SecondaryReasonCode)sec_crash_reason.reason);
}

SecondaryReasonCode Logger:: GetSecondaryCrashReasonCode()
{
  return (SecondaryReasonCode)sec_crash_reason.reason;
}

bool Logger :: isSsrTriggered()
{
  return diag_interface_.isSsrTriggered();
}

bool Logger :: isDiagSsrTriggered()
{
  return diag_interface_.isDiagSsrTriggered();
}

void Logger :: ResetSsrTriggeredFlag()
{
  return diag_interface_.ResetSsrTriggeredFlag();
}

bool Logger :: SetSsrTriggeredFlag()
{
  bool ret = diag_interface_.SetSsrTriggeredFlag();
  if (ret) {
    SetRecoveryStartTime();
  }
  return ret;
}

void Logger :: SetRecoveryStartTime() {
  gettimeofday(&ssrtime, NULL);
  is_recovery_time_set_ = true;
  ALOGD("%s: recovery start time set", __func__);
}

bool Logger :: GetSsrTime(struct timeval* time) {
  if (is_recovery_time_set_) {
    *time = ssrtime;
    return true;
  }
  return false;
}

uint8_t Logger :: GetClientStatus(ProtocolType type)
{
  return (clientstatus & (0x01 << type));
}

void Logger :: SetClientStatus(bool status, ProtocolType type)
{
  if (status)
    clientstatus = (clientstatus | (0x01 << type));
  else
    clientstatus = (clientstatus & (~(0x01 << type)));
}

bool Logger :: GetCleanupStatus(ProtocolType ptype)
{
  return diag_interface_.GetCleanupStatus(ptype);
}

void Logger:: UpdateRxTimeStamp()
{
  time_rx_call_back = time(NULL);
}

PrimaryReasonCode Logger::GetPrimaryReason()
{
  return (PrimaryReasonCode)pri_crash_reason;
}

void Logger :: UnlockDiagMutex()
{
  diag_interface_.UnlockDiagMutex();
}

void Logger :: ResetForceSsrTriggeredFlag()
{
  diag_interface_.ResetForceSsrTriggeredFlag();
  is_recovery_time_set_ = false;
}

void Logger::DumpLogsInBugreport(int fd)
{
  struct stat info;
  if (stat(LOG_COLLECTION_DIR, &info) != 0) {
    ALOGD("%s: no %s dir found", __func__, LOG_COLLECTION_DIR );
    std::stringstream content;
    content << "No log dir: /data/vendor/ssrdump/ present inorder"
            <<" to collect latest logs in bugreport" << std::endl;
    write(fd, content.str().c_str(), content.str().length());
    return;
  }
  LogFileName latest_file, current_file;
  any_reg_file = false;

  current_file = LastestLogFileName(BT_STATE_FILE_NAME_PREFIX, LOG_COLLECTION_DIR);
  ALOGD("%s: log file returned %s", __func__, current_file.name.c_str());
  if (current_file.name.length() != 0) {
    if (isLastestLogFileName(current_file, latest_file)) {
        latest_file = current_file;
    }
  }

  if (!any_reg_file) {
    ALOGD("%s: no regular files in dir %s found", __func__, LOG_COLLECTION_DIR );
    std::stringstream content;
    content << "No log files present in dir: /data/vendor/ssrdump/ inorder"
            <<" to collect latest logs in bugreport" << std::endl;
    write(fd, content.str().c_str(), content.str().length());
    return;
  }
  current_file = LastestLogFileName(SNOOP_FILE_NAME_PREFIX, LOG_COLLECTION_DIR);
  ALOGD("%s: log file returned %s", __func__, current_file.name.c_str());
  if (current_file.name.length() != 0) {
    if (isLastestLogFileName(current_file, latest_file)) {
      latest_file = current_file;
    }
  }
  current_file = LastestLogFileName(UART_IPC_PWR_LOG_PREFIX, LOG_COLLECTION_DIR);
  ALOGD("%s: log file returned %s", __func__, current_file.name.c_str());
  if (current_file.name.length() != 0) {
    if (isLastestLogFileName(current_file, latest_file)) {
      latest_file = current_file;
    }
  }
  current_file = LastestLogFileName(UART_IPC_STATE_LOG_PREFIX, LOG_COLLECTION_DIR);
  ALOGD("%s: log file returned %s", __func__, current_file.name.c_str());
  if (current_file.name.length() != 0) {
    if (isLastestLogFileName(current_file, latest_file)) {
      latest_file = current_file;
    }
  }
  current_file = LastestLogFileName(UART_IPC_RX_LOG_PREFIX, LOG_COLLECTION_DIR);
  ALOGD("%s: log file returned %s", __func__, current_file.name.c_str());
  if (current_file.name.length() != 0) {
    if (isLastestLogFileName(current_file, latest_file)) {
      latest_file = current_file;
    }
  }
  current_file = LastestLogFileName(UART_IPC_TX_LOG_PREFIX, LOG_COLLECTION_DIR);
  ALOGD("%s: log file returned %s", __func__, current_file.name.c_str());
  if (current_file.name.length() != 0) {
    if (isLastestLogFileName(current_file, latest_file)) {
      latest_file = current_file;
    }
  }
  current_file = LastestLogFileName(SOC_DUMP_PREFIX, LOG_COLLECTION_DIR);
  ALOGD("%s: log file returned %s", __func__, current_file.name.c_str());
  if (current_file.name.length() != 0) {
    if (isLastestLogFileName(current_file, latest_file)) {
      latest_file = current_file;
    }
  }
  any_reg_file = false;
  WriteToFd(fd, latest_file.ts, BT_STATE_FILE_NAME_PREFIX, LOG_COLLECTION_DIR, LOGS_EXTN);
  WriteToFd(fd, latest_file.ts, SNOOP_FILE_NAME_PREFIX, LOG_COLLECTION_DIR, SNOOP_EXTN);
  WriteToFd(fd, latest_file.ts, UART_IPC_PWR_LOG_PREFIX, LOG_COLLECTION_DIR, LOGS_EXTN);
  WriteToFd(fd, latest_file.ts, UART_IPC_STATE_LOG_PREFIX, LOG_COLLECTION_DIR, LOGS_EXTN);
  WriteToFd(fd, latest_file.ts, UART_IPC_RX_LOG_PREFIX, LOG_COLLECTION_DIR, LOGS_EXTN);
  WriteToFd(fd, latest_file.ts, UART_IPC_TX_LOG_PREFIX, LOG_COLLECTION_DIR, LOGS_EXTN);
  WriteToFd(fd, latest_file.ts, SOC_DUMP_PREFIX, LOG_COLLECTION_DIR, FW_DUMP_EXTN);
}

void Logger::WriteToFd(int fd, const LogFileTS& latest_file_ts, const std::string log_prefix,
                       const std::string log_dir, const std::string log_sufix) {
  std::stringstream path;
  std::stringstream content;
  std::ifstream file;
  std::string latest_file;
  std::stringstream file_name;
  file_name << log_prefix << std::setfill('0') << std::setw(4) << (unsigned)latest_file_ts.year
            << "-" << std::setfill('0') << std::setw(2) << (unsigned)latest_file_ts.month << "-"
            << std::setfill('0') << std::setw(2) << (unsigned)latest_file_ts.date << "_"
            << std::setfill('0') << std::setw(2) << (unsigned)latest_file_ts.hour << "-"
            << std::setfill('0') << std::setw(2) << (unsigned)latest_file_ts.min << "-"
            << std::setfill('0') << std::setw(2) << (unsigned)latest_file_ts.sec << log_sufix;
  latest_file = file_name.str();
  path << log_dir << latest_file;
  ALOGD("Dumping file %s", path.str().c_str());
  file.open(path.str());
  content << "=============================================" << std::endl;
  content << "File Name:" << latest_file << std::endl;
  content << "=============================================" << std::endl;
  if (latest_file.length() != 0 && file.is_open()) {
    content << file.rdbuf() << std::endl;
  } else {
    content << "File open failed or latest file not present with prefix: "
            << log_prefix << std::endl;
  }
  content << std::endl;
  write(fd, content.str().c_str(), content.str().length());
}

bool Logger::isLastestLogFileName(const LogFileName& current_file, const LogFileName& latest_file) {
  if (current_file.ts.year != latest_file.ts.year) {
    return current_file.ts.year > latest_file.ts.year;
  }
  if (current_file.ts.month != latest_file.ts.month) {
    return current_file.ts.month > latest_file.ts.month;
  }
  if (current_file.ts.date != latest_file.ts.date) {
    return current_file.ts.date > latest_file.ts.date;
  }
  if (current_file.ts.hour != latest_file.ts.hour) {
    return current_file.ts.hour > latest_file.ts.hour;
  }
  if (current_file.ts.min != latest_file.ts.min) {
    return current_file.ts.min > latest_file.ts.min;
  }
  if (current_file.ts.sec != latest_file.ts.sec) {
    return current_file.ts.sec > latest_file.ts.sec;
  }
  return true;
}

LogFileName Logger::LastestLogFileName(const std::string& log_prefix, const std::string& log_dir) {
  struct dirent* dp;
  LogFileName latest_file;
  std::unique_ptr<DIR, decltype(&closedir)> dump_dir(opendir(log_dir.c_str()), closedir);

  if (!dump_dir) {
    ALOGW("Failed to open log directory, skip %s", log_prefix.c_str());
    return latest_file;
  }

  while ((dp = readdir(dump_dir.get()))) {
    if (dp->d_type != DT_REG) {
      continue;
    }
    any_reg_file = true;
    std::string file_name = dp->d_name;
    if (file_name.length() != log_prefix.length() + LOG_FILE_TS_LENGTH) {
      continue;
    }

    size_t position = file_name.find(log_prefix);
    if (position == 0) {
      LogFileName current_file;
      current_file.name = file_name;
      position += log_prefix.length();
      current_file.ts.year = std::stoi(file_name.substr(position, position + LOG_YEAR_LENGTH),
                                       nullptr);
      position += LOG_YEAR_LENGTH + 1;
      current_file.ts.month = std::stoi(file_name.substr(position, position + LOG_TS_UNIT_LENGTH),
                                        nullptr);
      position += LOG_TS_UNIT_LENGTH + 1;
      current_file.ts.date = std::stoi(file_name.substr(position, position + LOG_TS_UNIT_LENGTH),
                                       nullptr);
      position += LOG_TS_UNIT_LENGTH + 1;
      current_file.ts.hour = std::stoi(file_name.substr(position, position + LOG_TS_UNIT_LENGTH),
                                       nullptr);
      position += LOG_TS_UNIT_LENGTH + 1;
      current_file.ts.min = std::stoi(file_name.substr(position, position + LOG_TS_UNIT_LENGTH),
                                      nullptr);
      position += LOG_TS_UNIT_LENGTH + 1;
      current_file.ts.sec = std::stoi(file_name.substr(position, position + LOG_TS_UNIT_LENGTH),
                                      nullptr);

      if (isLastestLogFileName(current_file, latest_file)) {
        latest_file = current_file;
      }
    }
  }
  return latest_file;
}

void Logger::CollectDumps(bool is_uart_ipc_enabled, bool is_state_log_enabled) {
  std::unique_lock<std::recursive_mutex> guard(bugreport_mutex);
#ifdef DUMP_IPC_LOG
  {
    DataHandler *data_handler = DataHandler::Get();
    if (is_uart_ipc_enabled && data_handler
        && data_handler->GetSocType() != BT_SOC_SMD) {
      DumpUartIpcLogs();
    }
  }
#endif

#ifdef DUMP_RINGBUF_LOG
  {
     //Dumping the ring buffer logs based on property for debugging purpose
    char value[PROPERTY_VALUE_MAX] = "false";
    if (!is_uart_ipc_enabled)
      PropertyGet("persist.vendor.service.bdroid.dump_ringbuff", value, "false");
    if (is_uart_ipc_enabled || (strcmp(value, "true") == 0))
      DumpRingBuffers();
  }
#endif
  if (is_state_log_enabled)
    //Dump last BT states of transport driver
    BtState :: Get()->DumpBtState();
}

void Logger :: ResetCleanupflag()
{
  diag_interface_.ResetCleanupflag();
}

void Logger :: SetInternalCmdTimeout()
{
  diag_interface_.SetInternalCmdTimeout();
}

bool Logger :: DiagInitOnGoing() {
  return diag_interface_.DiagInitOnGoing();
}

int Logger :: ReadSsrLevel() {
  char value[PROPERTY_VALUE_MAX] = {'\0'};
#ifdef ENABLE_FW_CRASH_DUMP
  const char* default_value = "1"; //crash device after collecting crash dump
#else
  const char* default_value = "3"; //restart BT host after collecting crash dump
#endif
  property_get("persist.vendor.service.bdroid.ssrlvl", value, default_value);
  int ssr_level = atoi(value);
  ALOGI("%s: ssr_level set to %d\n", __func__, ssr_level);
  return ssr_level;
}

#ifdef DIAG_ENABLED
bool Logger :: GetDiagInitStatus() {
  return diag_interface_.GetDiagInitStatus();
}
#endif

void Logger :: UpdateNvmPatchIdealDelay(int size) {
    ideal_time_mapper_[BT_HOST_REASON_NVM_DNLD_STUCK] = size/NVM_DOWNLOAD_RATE + MAX_PATCH_DOWNLOAD_VARIABLE_DELAY;
}

void Logger :: UpdateXmemNvmPatchIdealDelay(int size) {
    ideal_time_mapper_[BT_HOST_REASON_XMEM_NVM_DNLD_STUCK] = size/XMEM_NVM_PATCH_DOWNLOAD_RATE + MAX_PATCH_DOWNLOAD_VARIABLE_DELAY;
}

void Logger :: UpdateRamPatchIdealDelay(int size, bool fw_optimization) {
    if (fw_optimization)
      ideal_time_mapper_[BT_HOST_REASON_PATCH_DNLD_STUCK] = size/RAM_PATCH_DOWNLOAD_RATE;
    else
      ideal_time_mapper_[BT_HOST_REASON_PATCH_DNLD_STUCK] = size/RAM_PATCH_DOWNLOAD_RATE_WITH_CC;

    ideal_time_mapper_[BT_HOST_REASON_PATCH_DNLD_STUCK] += MAX_PATCH_DOWNLOAD_VARIABLE_DELAY;
}

void Logger :: UpdateXmemRamPatchIdealDelay(int size, bool fw_optimization) {
    if (fw_optimization)
      ideal_time_mapper_[BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK] = size/XMEM_RAM_PATCH_DOWNLOAD_RATE;
    else
      ideal_time_mapper_[BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK] = size/XMEM_RAM_PATCH_DOWNLOAD_RATE_WITH_CC;

    ideal_time_mapper_[BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK] += MAX_PATCH_DOWNLOAD_VARIABLE_DELAY;
}

void Logger :: UpdateElfPatchIdealDelay(int size, bool fw_optimization) {
    if (fw_optimization)
      ideal_time_mapper_[BT_HOST_REASON_PATCH_DNLD_STUCK] = size/ELF_PATCH_SEQUENTIAL_DOWNLOAD_RATE;
    else
      ideal_time_mapper_[BT_HOST_REASON_PATCH_DNLD_STUCK] = size/ELF_PATCH_SEQUENTIAL_DOWNLOAD_RATE_CC;

    ideal_time_mapper_[BT_HOST_REASON_PATCH_DNLD_STUCK] += MAX_PATCH_DOWNLOAD_VARIABLE_DELAY;
}

//Below function is called for CHE based SoC.
void Logger :: UpdateCheBasedIdealDelay() {
    ideal_time_mapper_[BT_HOST_REASON_UARTINIT_STUCK] = UART_INIT_IDEAL_TIME_DELAY_CHE_BASED;
    ideal_time_mapper_[BT_HOST_REASON_POWER_ON_REGS_STUCK] = POWER_ON_REGS_IDEAL_TIME_DELAY_CHE_BASED;
}

void Logger :: UpdateIdealDelay(SecondaryReasonCode activity_code, int ideal_delay) {
  if (ideal_time_mapper_.find(activity_code) != ideal_time_mapper_.end())
    ideal_time_mapper_[activity_code] = ideal_delay;
}

//This function is only used to track init thread activities.
void Logger :: SetCurrentactivityStartTime(struct timeval tv_start, SecondaryReasonCode activity_code,
                                             std::string activity_info) {
  /* Ignore tracking activities if init crash already in progress */
  if (host_crash_during_init == true)
    return;

  DataHandler* instance = DataHandler::Get();
  if (instance && (instance->GetInitStatus() == INIT_STATUS_SUCCESS))
    return;

  if (is_delay_list_processed) {
    ALOGE("%s: Delay list already processed", __func__);
    return;
  }

  //Acquire mutex lock to prevent race condition w.r.t "current_activities"
  std::unique_lock<std::mutex> guard(delay_list_mutex_);

  CurrentActivity cur_act;
  cur_act.start_time_val = tv_start;
  cur_act.cur_activity_code = activity_code;
  cur_act.cur_activity_info = activity_info;
  cur_act.recursive_overhead = 0;
  //push current activity info. as its become active.
  current_activities.push(cur_act);
  is_activity_in_progress = true;
}

//This function is only used to track init thread activities.
void Logger :: CheckAndAddToDelayListHelper(struct timeval* tv_end) {

  DataHandler* instance = DataHandler::Get();
  if (instance && (instance->GetInitStatus() == INIT_STATUS_SUCCESS))
    return;

  if (!is_activity_in_progress) {
    ALOGE("No activity is going on, why %s is called ?", __func__);
    return;
  }

  if (is_delay_list_processed) {
    ALOGE("%s: Delay list already processed", __func__);
    return;
  }

  //Acquire mutex lock to prevent race condition w.r.t "delay_list" & "current_activities"
  std::unique_lock<std::mutex> guard(delay_list_mutex_);

  if (current_activities.empty()) {
    ALOGE("%s: Activity stack is empty", __func__);
    return;
  }

  //pick recent activity info.
  CurrentActivity cur_act = current_activities.top();
  struct timeval tv_start = cur_act.start_time_val;

  //Convert time to ms.
  unsigned long long time_1 = tv_start.tv_sec*1e3 + tv_start.tv_usec*1e-3;
  unsigned long long time_2 = tv_end->tv_sec*1e3 + tv_end->tv_usec*1e-3;

  if (time_2 < time_1) {
    ALOGW("%s: time change detected start time:%llu end time:%llu", __func__, time_1, time_2);
    time_2 = time_1;
  }

  // Total time taken by activity(including its recursive activites).
  int time_taken = time_2 - time_1;

  // Pop top running activity as its completed and delay is calculated.
  current_activities.pop();

  if (!current_activities.empty())
    current_activities.top().recursive_overhead += time_taken;

  // Actual total time taken by activity
  time_taken -= cur_act.recursive_overhead;

  /* Check for time taken and add to delay list
   * if time taken is greater than ideal time.
   */
  if (time_taken > ideal_time_mapper_[cur_act.cur_activity_code]) {
    char start_time[20];
    char end_time[20];
    add_time_str(start_time, &tv_start);
    add_time_str(end_time, tv_end);
    delay_list.push_back(DelayListElement(cur_act.cur_activity_code, cur_act.cur_activity_info,
                                            start_time, end_time, time_taken,
                                            time_taken - ideal_time_mapper_[cur_act.cur_activity_code]));
  }

  if (current_activities.empty())
    is_activity_in_progress = false;
}

//This function is only used to track init thread activities.
void Logger :: CheckAndAddToDelayList(struct timeval* tv_end) {
  /* Track activities only if init timeout not occured */
  if (host_crash_during_init == false) {
    CheckAndAddToDelayListHelper(tv_end);
  }
}

bool Logger :: CheckActivityCodeForCrashCmd(SecondaryReasonCode activity_code) {
  //Check if activity is related to non-system related delay.
  if (activity_code != BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK &&
      activity_code != BT_HOST_REASON_RX_THREAD_START_STUCK &&
      activity_code != BT_HOST_REASON_PROPERTY_GET_STUCK &&
      activity_code != BT_HOST_REASON_PROPERTY_SET_STUCK &&
      activity_code != BT_HOST_REASON_RAM_PATCH_READ_STUCK &&
      activity_code != BT_HOST_REASON_NVM_PATCH_READ_STUCK &&
      activity_code != BT_HOST_REASON_POWER_ON_REGS_STUCK &&
      activity_code != BT_HOST_REASON_GET_LOCALADDR_STUCK &&
      activity_code != BT_HOST_REASON_POWER_IOCTL_STUCK) {
         return true;
  }
  return false;
}

void Logger :: CheckDelayListAndSetCrashReason() {
    bool non_system_delay_detected = false;

    if (is_delay_list_processed) {
      ALOGE("%s: Delay list already processed", __func__);
      return;
    }

    //Check if any activity is not completed yet.
    if (is_activity_in_progress) {
      struct timeval tv_end;
      gettimeofday(&tv_end, NULL);
      while (is_activity_in_progress) {
        CheckAndAddToDelayListHelper(&tv_end);
      }
    }

    //Acquire mutex lock to prevent race condition w.r.t "delay_list"
    std::unique_lock<std::mutex> guard(delay_list_mutex_);

    //sort by decreasing delay value taken w.r.t ideal time.
    sort(delay_list.begin(), delay_list.end(), std::greater<DelayListElement>());

    crashcmdneeded = false;
    for (DelayListElement &activity: delay_list) {
      // Check for non-system related issue having highest delay.
      if (CheckActivityCodeForCrashCmd(activity.activity_code)) {
        if (!non_system_delay_detected) {
          /* Setting appropriate secondary crash reason */
          sec_crash_reason.reason = activity.activity_code;
          sec_crash_reason.time_instance = time(NULL);
          non_system_delay_detected = true;
        }

        if (activity.activity_code != BT_HOST_REASON_UART_IOCTL_STUCK &&
            activity.activity_code != BT_HOST_REASON_UARTINIT_STUCK &&
            activity.activity_code != BT_HOST_REASON_DIAGINIT_STUCK) {
           //crash cmd is needed as its not related to UART.
          crashcmdneeded = true;
        }
      }
    }

    if (uart_err_code_ != UART_REASON_DEFAULT) {
      /* Setting appropriate secondary crash reason */
      sec_crash_reason.reason = uart_err_code_;
      sec_crash_reason.time_instance = time(NULL);
    } else if (!non_system_delay_detected && delay_list.size() > 0) {
        /* Setting appropriate secondary crash reason */
        sec_crash_reason.reason = delay_list[0].activity_code;
        sec_crash_reason.time_instance = time(NULL);
    }

    //Store crash reason.
    StoreCrashReason();
    // Add Delay list info. in state file object.
    AddDelayListInfo();

    //Delay list processing done.
    is_delay_list_processed = true;

    ALOGE("%s: Secondary reason set as %s", __func__, GetSecondaryCrashReason());
}

bool Logger :: CrashCmdNeeded() {
  return crashcmdneeded;
}

void Logger :: AddDelayListInfo() {
  state_info_->AddDelayListInfo(delay_list);
}

//need to be used during init, inorder to track delay.
int Logger :: PropertyGet(const char* property_name, char* value, const char* default_value) {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  SetCurrentactivityStartTime(tv, BT_HOST_REASON_PROPERTY_GET_STUCK,
                                "PROPERTY GET: "+ std::string(property_name));
  int ret = property_get(property_name, value, default_value);

  gettimeofday(&tv, NULL);
  CheckAndAddToDelayList(&tv);
  return ret;
}

//need to be used during init, inorder to track delay.
int Logger :: PropertySet(const char* property_name, const char* value) {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  SetCurrentactivityStartTime(tv, BT_HOST_REASON_PROPERTY_SET_STUCK,
                                "PROPERTY SET: "+ std::string(property_name));
  int ret = property_set(property_name, value);

  gettimeofday(&tv, NULL);
  CheckAndAddToDelayList(&tv);
  return ret;
}

void Logger :: InitialIdealTimeTable() {
  //This table maps activity to its coresponding ideal time.
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_PROPERTY_GET_STUCK, PROPERTY_GET_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_PROPERTY_SET_STUCK, PROPERTY_SET_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_RX_THREAD_START_STUCK, RX_THREAD_START_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_UARTINIT_STUCK, UART_INIT_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_POWER_ON_REGS_STUCK, POWER_ON_REGS_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK, FILE_SYSTEM_CALL_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_RAM_PATCH_READ_STUCK, FILE_SYSTEM_CALL_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_NVM_PATCH_READ_STUCK, FILE_SYSTEM_CALL_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_PATCH_DNLD_STUCK, DEFAULT_RAM_PATCH_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_NVM_DNLD_STUCK, DEFAULT_NVM_PATCH_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK, DEFAULT_XMEM_RAM_PATCH_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_XMEM_NVM_DNLD_STUCK, DEFAULT_XMEM_NVM_PATCH_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_DIAGINIT_STUCK, DIAGINIT_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_GETVER_NO_RSP_RCVD, CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_GETBOARDID_CMD_STUCK, CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_GETBLDINFO_CMD_STUCK, CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_ADDONFEAT_CMD_STUCK, ADDON_FEATURE_CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_OTP_INFO_GET_CMD_STUCK, CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_ENHLOG_CMD_STUCK, CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_HCI_RESET_STUCK, CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_SETBAUDRATE_CMD_STUCK, BAUDRATE_CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_GETVER_SEND_STUCK, CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_UART_IOCTL_STUCK, UART_IOCTL_IDEAL_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_GET_LOCALADDR_STUCK, FILE_SYSTEM_CALL_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_POWER_IOCTL_STUCK, POWER_IOCTL_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_PATCH_CONFIG_CMD_STUCK, CMD_RSP_IDEAL_TIME_DELAY));
  ideal_time_mapper_.insert(std::make_pair<SecondaryReasonCode, int>
                            (BT_HOST_REASON_GET_APP_VER_CMD_STUCK, CMD_RSP_IDEAL_TIME_DELAY));
}

void Logger :: SetUartErrCode(SecondaryReasonCode uart_reason) {
  uart_err_code_ = uart_reason;
  ALOGD("%s: Got UART error code : 0x%x", __func__, uart_err_code_);
}

SecondaryReasonCode Logger :: GetUartErrCode() {
  return uart_err_code_;
}

char* Logger :: GetPrimaryReasonString(PrimaryReasonCode code) {
  return snoop_buff_.GetPrimaryReason(code);
}

void  Logger :: SetInitFailureReason(SecondaryReasonCode reason) {
  init_failed_code_ = reason;
}

SecondaryReasonCode  Logger :: GetInitFailureReason() {
  return init_failed_code_;
}

void Logger :: ResetRxStats(void)
{
  std::lock_guard<std::recursive_mutex> lock(monitor_rx_stats);
  memset(&rx_stats, 0, sizeof(struct monitor_rx));
}

void Logger :: inc_rx_stats_counter(void)
{
  std::lock_guard<std::recursive_mutex> lock(monitor_rx_stats);
  if(!rx_stats.is_monitor_enabled)
    return;
  rx_stats.num_valid_pkt_rx++;
}

void Logger :: StartRxStatsMonitor(void)
{
  std::lock_guard<std::recursive_mutex> lock(monitor_rx_stats);
  rx_stats.is_monitor_enabled = true;
  rx_stats.lapsed_timeout = NUM_WACK_RETRANSMIT * TX_IDLE_DELAY;
}

struct monitor_rx *Logger :: GetRxStats(void)
{
  std::lock_guard<std::recursive_mutex> lock(monitor_rx_stats);
  return &rx_stats;
}

void Logger :: SetasFpissue(void)
{
  std::lock_guard<std::recursive_mutex> lock(monitor_rx_stats);
  is_this_fp_issue = true;
}

bool Logger :: isThisFpissue(void)
{
  std::lock_guard<std::recursive_mutex> lock(monitor_rx_stats);
  return is_this_fp_issue;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
