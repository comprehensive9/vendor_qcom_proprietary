/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
//
// Copyright 2017 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <hidl/HidlSupport.h>
#include "controller.h"
#include "patch_dl_manager.h"
#include "ibs_handler.h"
#include "health_info_log.h"
#include "hci_internals.h"
#include "state_info.h"

#define RX_THREAD_USAGE_TIMEOUT    (1500)
#define RX_THREAD_SCHEDULING_DELAY (50)
#define MAX_CONTINUOUS_RXTHREAD_STUCK (5)

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

typedef struct {
  TimerState timer_state;
  timer_t timer_id;
  uint32_t timeout_ms;
} ThreadTimer;

// Action definition
//
// Action to Add, Delete or Clear the reporting of quality event(s).
// Delete will clear specific quality event(s) reporting. Clear will clear all
// quality events reporting.
enum BqrReportAction : uint8_t {
  BQR_ACTION_ADD = 0x00,
  BQR_ACTION_DELETE = 0x01,
  BQR_ACTION_CLEAR = 0x02
};

// singleton class
class UartController : public Controller {
 public:
  // used to get the instance_ controller class
  // open expects the packet reader callback and when the data is available it will call this callback with
  // protocol and pacekt type, and the actual packet.
  virtual bool Init(PacketReadCallback packet_read_cb) override;
  // send packet is used to send the data. protocol type, actual packet types are expected.
  virtual size_t SendPacket(HciPacketType packet_type, const uint8_t *data, size_t length) override;
  virtual bool Cleanup(void) override;
  virtual SocAddOnFeatures_t* GetAddOnFeatureList() override;
  virtual uint64_t GetChipVersion() override;

  void OnDataReady(int fd);
  bool IsBqrRieEnabled();
  void OnPacketReady(hidl_vec<uint8_t> *data);
  int bt_kernel_panic(void);
  UartController(BluetoothSocType soc_type);
  static void SocCrashWaitTimeout(union sigval sig);
  virtual void WaitforCrashdumpFinish();
  virtual void SignalCrashDumpFinish();
  virtual bool Disconnect();
  virtual void UnlockControllerMutex();
  void SsrCleanup(PrimaryReasonCode reason);
  void StartRxThreadTimer();
  void StopRxThreadTimer();
  void ResetInvalidByteCounter();
  void StartSocCrashWaitTimer();
  void SendCrashPacket();
  void SendBqrRiePacket();
  ~UartController();
  double GetRxThreadSchedTSDiff();
  void LogPwrSrcsUartFlowCtrl();
  bool CheckForUartFailureStatus();
  void SetCleanupStatusDuringSSR();
  PrimaryReasonCode GetPreviousReason();

 private:
  void StartSocCrashWaitTimer(int ssrtimeout);
  void ReportSocFailure(bool dumped_uart_log, PrimaryReasonCode reason,
                        bool cleanupSocCrashWaitTimer, bool cleanupIbs);
  bool StopSocCrashWaitTimer();
  void CleanupSocCrashWaitTimer();
  bool command_is_get_dbg_info(const unsigned char* buf, int len);
  void CheckForBQRRootInflammationBit(const unsigned char* buf);
  bool IsHciPacketValid(HciPacketType type);
  void SendSpecialBuffer(PrimaryReasonCode reason);
  void HciTransportCleanup(bool);
  bool IsSecondaryReasonValid();
  static void ClientInitTimeOut(union sigval);
  PrimaryReasonCode GetPrimaryReason();

  static void RxThreadTimeOut(union sigval);
  static void RecoveryStuckTimeout(union sigval);
  void SetRxThreadTimerState(TimerState);
  TimerState GetRxThreadTimerState();
  void StartRecoveryStuckTimer();
  void StopRecoveryStuckTimer();
  void CleanupRecoveryStuckTimer();
  bool IsSoCCrashNotNeeded(PrimaryReasonCode reason);
  bool CheckCleanupStatusDuringSSR();
  bool ResetForceSsrTriggeredIfNoCleanup();
  PrimaryReasonCode prv_reason;

 public:
  bool is_spurious_wake;

 private:
  int soc_crash_wait_timer_state_;
  bool is_bqr_rie_enabled_; /* BQR Root Inflammation Event */
  timer_t soc_crash_wait_timer_;
  bool force_special_byte_enabled_;
  bool sibs_enabled_;
  bool soc_crashed;
  BluetoothSocType soc_type_;
  HciPacketType hci_packet_type_{HCI_PACKET_TYPE_UNKNOWN};
  HciPacketizer hci_packetizer_;
  std::condition_variable cv;
  std::mutex cv_m;
  bool is_cmd_timeout;
  bool is_invalid_pkt_from_soc;
  bool is_soc_wakeup_failure;
  bool is_bqr_rie_sent_already;
  bool cleanup_status_ssr_;
  HealthInfoLog* health_info;
  int invalid_bytes_counter_;
  std::mutex controller_mutex;
  std::mutex check_cleanup_mutex_;
  BtState *state_info_;
  struct timeval time_wk_lockacq_rel_;
  ThreadTimer rx_timer_state_machine_;
  static ThreadTimer recovery_timer_state_machine_;
  uint64_t chipset_ver_;
  int sync_bytes_rcvd_;
  bool out_of_sync_;
  uint8_t inv_bytes[TX_RX_PKT_ASC_SIZE];
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
