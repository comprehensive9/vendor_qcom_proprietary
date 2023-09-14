/*==========================================================================
Description
  It has implementation for bt state info class

# Copyright (c) 2019 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once

#include <hidl/HidlSupport.h>
#include <string>
#include <string.h>
#include "hci_internals.h"
#include "health_info_log.h"
#define MAX_BUFF_SIZE (64)
#define MAX_CRASH_BUFF_SIZE (128)
#define MAX_STATE_INFO_SIZE (512)
#define BT_STATE_FILE_NAME_PREFIX "ramdump_bt_state_"
#define MAX_PWR_RSRC_INFO_SIZE (1024)
#define HAL_DUMP_FILE_SIZE 256
#define OTP_INFO_BYTES_MAX_SIZE 1024
#define TX_RX_PKT_ASC_SIZE 21 //20 for pckt and 1 for Type
#define TX_RX_PKT_HEX_SIZE ((TX_RX_PKT_ASC_SIZE * 3) + 1)
#define HAL_DUMP_TIMESTAMP_LENGTH 20
#define HAL_DUMP_TIMESTAMP_OFFSET 23
#define HAL_DUMP_SIZE 85
#ifdef USER_DEBUG
#define HAL_DUMP_ITERATION_LIMIT 80
#else
#define HAL_DUMP_ITERATION_LIMIT 10
#endif
#define SEND_CMD_TS 1
#define READ_RSP_TS 2

#ifndef HCISTATUS
#define HCISTATUS
enum HCIStatus {
   HCI_INIT,
   HCI_CLOSE
};
 #endif

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

//forward declaration
struct DelayListElement;

typedef struct {
  char last_health_stat_info[MAX_STATE_INFO_SIZE];
  char last_wk_lck_acq_info[MAX_BUFF_SIZE];
  char last_wk_lck_rel_info[MAX_BUFF_SIZE];
  char last_hci_cmd_stack[MAX_BUFF_SIZE];
  char last_wk_ind_from_host[MAX_BUFF_SIZE];
  char last_wk_ack_from_soc[MAX_BUFF_SIZE];
  char last_wk_ind_from_soc[MAX_BUFF_SIZE];
  char last_wk_ack_from_host[MAX_BUFF_SIZE];
  char last_sleep_ind_from_host[MAX_BUFF_SIZE];
  char last_sleep_ind_from_soc[MAX_BUFF_SIZE];
  char pc_addr[MAX_BUFF_SIZE];
  char build_ver[MAX_BUFF_SIZE];
  char pre_stack_event[MAX_BUFF_SIZE];
  char post_stack_event[MAX_BUFF_SIZE];
  char pre_stack_acl[MAX_BUFF_SIZE];
  char post_stack_acl[MAX_BUFF_SIZE];
  char pre_select[MAX_BUFF_SIZE];
  char post_select[MAX_BUFF_SIZE];
  uint16_t last_tx_pkt_len;
  uint16_t actual_tx_pkt_len;
  char last_tx_pkt_ts[MAX_BUFF_SIZE];
  HciPacketType last_tx_pkt_type;
  char last_rx_pkt_ts[MAX_BUFF_SIZE];
  char ssr_rx_pkt_ts[MAX_BUFF_SIZE];
  uint16_t last_rx_pkt_len;
  uint16_t actual_rx_pkt_len;
  uint16_t ssr_rx_pkt_len;
  uint16_t actual_ssr_rx_pkt_len;
  HciPacketType last_rx_pkt_type;
  HciPacketType ssr_rx_pkt_type;
  uint16_t inv_bytes_len;
  int max_diag_rx_hci_delay;
  int max_diag_rx_acl_delay;
  int max_diag_tx_hci_delay;
  int max_diag_tx_acl_delay;
  struct timeval diag_rx_acl_start;
  struct timeval diag_tx_acl_start;
  struct timeval diag_rx_hci_start;
  struct timeval diag_tx_hci_start;
  int diag_pending_pkt;
  char inv_bytes_ts[MAX_BUFF_SIZE];
  char ts_getver_start_send_cmd[MAX_BUFF_SIZE];
  char ts_getver_start_read_rsp[MAX_BUFF_SIZE];
  char ts_getver_rcvd[MAX_BUFF_SIZE];
  char ts_hci_initialize[MAX_BUFF_SIZE];
  char ts_hci_close[MAX_BUFF_SIZE];
  char ts_last_seq_number[MAX_BUFF_SIZE];
  char ts_uart_flow_on[MAX_BUFF_SIZE];
  char ts_uart_flow_off[MAX_BUFF_SIZE];
  char ts_uart_clock_on[MAX_BUFF_SIZE];
  char ts_uart_clock_off[MAX_BUFF_SIZE];
  char ts_status_init_cb_sent[MAX_BUFF_SIZE];
  char ts_status_diag_init[MAX_BUFF_SIZE];
  char ts_status_ctrlr_init[MAX_BUFF_SIZE];
  char ts_hci_internal_cmd[MAX_BUFF_SIZE];
  char ts_hci_internal_rsp[MAX_BUFF_SIZE];
  char health_timer_status[MAX_BUFF_SIZE];
  char status_read_thread[MAX_BUFF_SIZE];
  char pri_crash_reason[MAX_CRASH_BUFF_SIZE];
  char sec_crash_reason[MAX_CRASH_BUFF_SIZE];
  char crash_ts[MAX_CRASH_BUFF_SIZE];
  char kernel_time[MAX_BUFF_SIZE];
  char last_tx_pkt_asc[TX_RX_PKT_ASC_SIZE];
  char last_tx_pkt_hex[TX_RX_PKT_HEX_SIZE];
  char last_rx_pkt_asc[TX_RX_PKT_ASC_SIZE];
  char last_rx_pkt_hex[TX_RX_PKT_HEX_SIZE];
  char ssr_rx_pkt_asc[TX_RX_PKT_ASC_SIZE];
  char ssr_rx_pkt_hex[TX_RX_PKT_HEX_SIZE];
  char inv_bytes_asc[TX_RX_PKT_ASC_SIZE];
  char inv_bytes_hex[TX_RX_PKT_HEX_SIZE];
  bool ssr_trigger;
  bool is_xmem_enabled;
  char alwayson_status[MAX_BUFF_SIZE];
  char post_diag_deinit_ts[MAX_BUFF_SIZE];
  char pre_diag_deinit_ts[MAX_BUFF_SIZE];
  char post_diag_init_ts[MAX_BUFF_SIZE];
  char pre_diag_init_ts[MAX_BUFF_SIZE];
  char fw_dwnld_success[MAX_BUFF_SIZE];
  char fw_su_build_info[MAX_BUFF_SIZE];
  char bt_soc_id[MAX_BUFF_SIZE];
  char power_resources_state[MAX_PWR_RSRC_INFO_SIZE];
  char bt_uart_cts[MAX_BUFF_SIZE];
  char pre_patch_open[MAX_BUFF_SIZE];
  char post_patch_open[MAX_BUFF_SIZE];
  char pre_nvm_open[MAX_BUFF_SIZE];
  char post_nvm_open[MAX_BUFF_SIZE];
  char pre_xmem_patch_open[MAX_BUFF_SIZE];
  char post_xmem_patch_open[MAX_BUFF_SIZE];
  char pre_xmem_nvm_open[MAX_BUFF_SIZE];
  char post_xmem_nvm_open[MAX_BUFF_SIZE];
  char pre_chk_patch_path[MAX_BUFF_SIZE];
  char post_chk_patch_path[MAX_BUFF_SIZE];
  char pre_version_info[MAX_BUFF_SIZE];
  char post_version_info[MAX_BUFF_SIZE];
  char pre_su_build_info[MAX_BUFF_SIZE];
  char post_su_build_info[MAX_BUFF_SIZE];
  char pre_local_addr[MAX_BUFF_SIZE];
  char post_local_addr[MAX_BUFF_SIZE];
  char pre_fetch_vendor_addr[MAX_BUFF_SIZE];
  char post_fetch_vendor_addr[MAX_BUFF_SIZE];
  char read_thread_start[MAX_BUFF_SIZE];
  char pre_tcs_config[MAX_BUFF_SIZE];
  char post_tcs_config[MAX_BUFF_SIZE];
  char pre_sibs_property_read[MAX_BUFF_SIZE];
  char post_sibs_property_read[MAX_BUFF_SIZE];
  char otp_info_bytes[OTP_INFO_BYTES_MAX_SIZE];
  char stuck_issue[MAX_BUFF_SIZE];
  char pre_diag_rx_acl[MAX_BUFF_SIZE];
  char post_diag_rx_acl[MAX_BUFF_SIZE];
  char pre_diag_rx_hci[MAX_BUFF_SIZE];
  char post_diag_rx_hci[MAX_BUFF_SIZE];
  char pre_diag_tx_acl[MAX_BUFF_SIZE];
  char post_diag_tx_acl[MAX_BUFF_SIZE];
  char pre_diag_tx_hci[MAX_BUFF_SIZE];
  char post_diag_tx_hci[MAX_BUFF_SIZE];
  char bqr_rie_info[MAX_CRASH_BUFF_SIZE];
  char uart_ipc_err[MAX_BUFF_SIZE];
  std::string delay_list_info;
  std::string bqr_rie_vs_params;
  struct timeval fw_su_build_info_set_time;
} DumpBtState;

class BtState
{
 private:
  static BtState *instance_;
  DumpBtState stat_info_obj;
  std::mutex diag_log_pending_mutex;
  int time_year, time_month, time_day, time_hour, time_min, time_sec;

 public:
  BtState();
  static BtState *Get(void);
  void AddLogTag(char* dest_tag_str, struct timeval& time_val, char * tag);
  void SetLastStatsInfo(char *buf);
  void SetTsCtrlrInitStatus(char *buf);
  void SetWakeLockAcqTimeName(char* buf);
  void SetWakeLockRelTimeName(char* buf);
  void SetLastHciCmdStack(char* buf);
  void SetReason(char* buf);
  void SetLastWakeIndFromHost(char* buf);
  void SetLastWakeAckFromHost(char* buf);
  void SetLastWakeIndFromSoc(char* buf);
  void SetLastWakeAckFromSoc(char* buf);
  void SetLastSleepIndFromHost(char* buf);
  void SetLastSleepIndFromSoc(char* buf);
  void SetPCAddr(char* buf);
  void SetBuildVersion(char* buf);
  void SetPreStackEvent(char* buf);
  void SetPostStackEvent(char* buf);
  void SetPreStackAcl(char* buf);
  void SetPostStackAcl(char* buf);
  void SetPreSelect(char* buf);
  void SetPostSelect(char* buf);
  void SetLastTxPacket(uint8_t *buf, uint16_t len, HciPacketType type);
  void SetLastRxPacket(uint8_t *buf, uint16_t len, HciPacketType type);
  void SetInvBytes(uint8_t *buf, uint16_t len);
  void SetGetVerResEvtTS(char *buf);
  void SetGetVerReqTS(char *buf, int type);
  void SetTsHCIInitClose(HCIStatus hci_status, char *buf);
  void SetLastSeqTS(char *buf);
  void SetUARTFlowOn(char *buf);
  void SetUARTFlowOff(char *buf);
  void SetUARTClockOn(char *buf);
  void SetUARTClockOff(char *buf);
  void SetTsStatusOfCbSent(char *buf);
  void SetDiagInitStatus(char *buf);
  void SetHelthStatsTimerStatus(char *buf);
  void SetReadThreadStatus(char *buf);
  void SetReadThreadStartTS(char *buf);
  void SetSSRtrigger(bool status);
  void SetAlwaysOnEnabled();
  void DeleteDumpsIfRequired();
  void DeleteDumps(char *first_entry);
  void DumpBtState();
  void Cleanup(void);
  void SetPrimaryCrashReason(char *);
  void SetSecondaryCrashReason(char *);
  void SetCrashTS(char *);
  void SetKernelTimeStamp(char *);
  void SetDiagDeInitPreTS(char *);
  void SetDiagDeInitPostTS(char *);
  void SetDiagInitPreTS(char *);
  void SetDiagInitPostTS(char *);
  void SetFwDwnldSucceed(char *);
  void SetFwSuBuildInfo(char *, struct timeval);
  void SetBtSocId(char *);
  void SetHciInternalCmdRsp(char *buf);
  void SetHciInternalCmdSent(char *buf);
  void SetPowerResourcesState(char *buf);
  void SetCtsState(char* buf);
  void SetPrePatchOpenTS(char* buf);
  void SetPostPatchOpenTS(char* buf);
  void SetPreNVMOpenTS(char* buf);
  void SetPostNVMOpenTS(char* buf);
  void SetPreXMEMPatchOpenTS(char* buf);
  void SetPostXMEMPatchOpenTS(char* buf);
  void SetPreXmemNVMOpenTS(char* buf);
  void SetPostXmemNVMOpenTS(char* buf);
  void SetPreCheckRampatchPathTS(char* buf);
  void SetPostCheckRampatchPathTS(char* buf);
  void SetPreVersioInfoTS(char* buf);
  void SetPostVersioInfoTS(char* buf);
  void SetPreSUBuildInfoTS(char* buf);
  void SetPostSUBuildInfoTS(char* buf);
  void SetPreGetLocalAddressTS(char* buf);
  void SetPostGetLocalAddressTS(char* buf);
  void SetPreFetchLocalAddressTS(char* buf);
  void SetPostFetchLocalAddressTS(char* buf);
  void SetPreTCSConfigIoctlTS(char* dst_buff);
  void SetPostTCSConfigIoctlTS(char* dst_buff);
  void SetPreReadSibsPropertyTS(char* dst_buff);
  void SetPostReadSibsPropertyTS(char* dst_buff);
  void SetXmemEnabledFlag();
  void SetOTPInfoBytes(uint8_t size, char* buf);
  void SetTsStuckTimeout(char* buf);
  void SetPostTsDiagRxAcl(char* buf, struct timeval tv);
  void SetPreTsDiagRxAcl(char* buf, struct timeval tv);
  void SetPostTsDiagTxAcl(char* buf, struct timeval tv);
  void SetPreTsDiagTxAcl(char* buf, struct timeval tv);
  void SetPreTsDiagRxHciEvt(char* buf, struct timeval tv);
  void SetPostTsDiagRxHciEvt(char* buf, struct timeval tv);
  void SetPreTsDiagTxHciCmd(char* buf, struct timeval tv);
  void SetPostTsDiagTxHciCmd(char* buf, struct timeval tv);
  void UpdateBqrRieErrCodeAndErrString(int crash_code, std::string crash_string);
  void AddDelayListInfo(std::vector<DelayListElement>& delay_list);
  void PrintDelayListInfo();
  void SetFailStatusForIpcLog(char* buf);
  bool isDiagLogApiStuck();
  void UpdateBQRVSParams(std::string& params);
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android

