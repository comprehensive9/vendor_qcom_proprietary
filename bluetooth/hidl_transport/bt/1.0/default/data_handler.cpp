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
// Copyright 2016 The Android Open Source Project
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

#include <pthread.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <hidl/HidlSupport.h>
#include "data_handler.h"
#include "logger.h"
#include <cutils/properties.h>
#include "bluetooth_address.h"
#include <utils/Log.h>
#include <signal.h>
#include "state_info.h"
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.bluetooth@1.0-data_handler"
#define PROC_PANIC_PATH     "/proc/sysrq-trigger"

extern "C" {
#include "libsoc_helper.h"
}

namespace {

using android::hardware::bluetooth::V1_0::implementation::DataHandler;

DataHandler *data_handler = nullptr;
std::mutex init_mutex_;

std::mutex evt_wait_mutex_;
std::condition_variable event_wait_cv;
bool event_wait;
uint16_t awaited_evt;

const char *convertMSMToStr(soc_info_v0_1 soc_info) {
  switch (soc_info.msm_cpu){
    case MSM_CPU_LAHAINA:
      return "MSM_CPU_LAHAINA";
    default:
      ALOGE("%s: INVALID_SOC_INFO", __func__);
      return "INVALID_SOC_INFO";
  }
}

}

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_vec;
using DataReadCallback = std::function<void(HciPacketType, const hidl_vec<uint8_t>*)>;
using InitializeCallback = std::function<void(bool success)>;

bool soc_need_reload_patch = true;
bool DataHandler :: caught_signal = false;
unsigned int ClientStatus;
unsigned int RxthreadStatus;
std::mutex DataHandler::init_timer_mutex_;

#define MIN_OPCODE_LEN     (5)
/* Startup time is set to 2.85 sec w.r.t HIDL.
 * This is done because many times HIDL detect close triggered before
 * startup timer expiry but in actual startup timer is expired.
 * Delay of Close() call excution from BT Stack to HIDL should be considered.
 */
#define HIDL_INIT_TIMEOUT  (2850)
/* HIDL INIT timeout in case XMEM patch file
 * is used with default download configuration.
 */
#define HIDL_INIT_TIMEOUT_DEFAULT_XMEM (11850)
/* HIDL INIT timeout in case XMEM patch file
 * is used with download configuration set to
 * have rsp for every tlv download cmd.
 */
#define HIDL_INIT_TIMEOUT_XMEM (19850)
#define HCI_CMD_TIMEOUT  (2000)
#define INTERNAL_CMD_TIMEOUT (500)

#define HOST_ADD_ON_ADV_AUDIO_UNICAST_FEAT_MASK   0x01
#define HOST_ADD_ON_ADV_AUDIO_BCA_FEAT_MASK       0x02
#define HOST_ADD_ON_ADV_AUDIO_BCS_FEAT_MASK       0x04
#define HOST_ADD_ON_ADV_AUDIO_STEREO_RECORDING    0x08
#define HOST_ADD_ON_ADV_AUDIO_LC3Q_FEAT_MASK      0x10
#define HOST_ADD_ON_QHS_FEAT_MASK                 0x20

#ifdef USER_DEBUG
void DataHandler::SetTransport(HciUartTransport *uart_transport)
{
  uart_transport_ = uart_transport;
}

void DataHandler::SendCTSStatusToClient(unsigned char status) {
  ALOGV("%s: ##",__func__);
  hidl_vec<uint8_t> hidl_data;
  std::vector<uint8_t> hidl_vector  = FRAME_GET_CTS_SOC_STATUS;
  hidl_vector.push_back(static_cast<uint8_t>(status));
  hidl_data =  hidl_vector;
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  ProtocolCallbacksType *cb_data = nullptr;
  it = protocol_info_.find(TYPE_BT);
  if (it != protocol_info_.end()) {
    cb_data = (ProtocolCallbacksType*)it->second;
  }

  // execute callbacks here
  if (cb_data != nullptr && controller_ != nullptr) {
    cb_data->data_read_cb(HCI_PACKET_TYPE_EVENT, &hidl_data);
  }
}

int DataHandler::HandleFlowControl(userial_vendor_ioctl_op_t op)
{
  int flags = 0, err = -1;
  switch (op) {
  case USERIAL_OP_FLOW_ON:
    ALOGV("%s: ## userial_vendor_ioctl: UART Flow On ", __func__);
    if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_ON, &flags)) < 0) {
      ALOGE("%s: HW Flow-ON error: 0x%x \n", __func__, err);
      return err;
    }
    break;
  case USERIAL_OP_FLOW_OFF:
    ALOGV("%s: ## userial_vendor_ioctl: UART Flow OFF ", __func__);
    if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_OFF, &flags)) < 0) {
      ALOGE("%s: HW Flow-OFF error: 0x%x \n", __func__, err);
      return err;
    }
    break;
  default:
    break;
  }
  return err;
}

int DataHandler::GetCTSLineStatus()
{
  int serial;
  int ret = -1;
  ALOGV("%s:", __func__);
  ret = ioctl(uart_transport_->GetCtrlFd(), TIOCMGET, &serial);
  if (ret < 0) {
    ALOGE("%s: TIOCMGET error =%d", __func__, ret);
    return ret;
  } else {
    ALOGI("%s: TIOCM_CTS SUCCESS = %02x", __func__, serial & TIOCM_CTS);
    return (serial & TIOCM_CTS ? 0 : 1);
  }
}

bool DataHandler::command_is_get_cts_status(const unsigned char* buf, int len)
{
  bool result = false;
  const unsigned char get_cts_status[] = COMMAND_GET_CTS_STATUS;

  ALOGV("%s: length = %d ", __func__, len);
  if (memcmp(get_cts_status, buf, ARRAY_SIZE(get_cts_status)) == 0) {
    result= true;
  }
  return result;
}

bool DataHandler::command_is_get_rts_status(const unsigned char* buf, int len)
{
  bool result = false;
  const unsigned char get_rts_status[] = COMMAND_GET_SOC_CTS_STATUS;

  ALOGV("%s: length = %d ", __func__, len);
  if (len >= ARRAY_SIZE(get_rts_status) &&
      memcmp(get_rts_status, buf, ARRAY_SIZE(get_rts_status)) == 0) {
    result= true;
  }
  return result;
}

bool DataHandler::command_is_reset_uart_flow(const unsigned char* buf, int len)
{
  bool result = false;
  const unsigned char reset_uart_flow[] = COMMAND_RESET_UART_FLOW_ON;

  ALOGV("%s: length = %d ", __func__, len);
  if (memcmp(reset_uart_flow, buf, ARRAY_SIZE(reset_uart_flow)) == 0) {
    result= true;
  }
  return result;
}
#endif

void DataHandler::data_service_sighandler(int signum)
{
  ALOGD("%s: Setting signal 15 caught status as true", __func__);
  if (data_handler)
    data_handler->SetSignalCaught();
  // lock is required incase of multiple binder threads
  std::unique_lock<std::mutex> guard(init_mutex_);
  ALOGW("%s: Caught Signal: %d", __func__, signum);

  if (data_handler) {
    if (data_handler->Close(TYPE_BT))
        goto cleanup;
    if (data_handler->Close(TYPE_FM))
        goto cleanup;
    if (data_handler->Close(TYPE_ANT))
        goto cleanup;
    return;
cleanup:
    ALOGI("%s: deleting data_handler", __func__);
    delete data_handler;
    data_handler = NULL;
    kill(getpid(), SIGKILL);
  }
}

int DataHandler::data_service_setup_sighandler(void)
{
  struct sigaction sig_act;

  ALOGI("%s: Entry", __func__);
  memset(&sig_act, 0, sizeof(sig_act));
  sig_act.sa_handler = data_handler->data_service_sighandler;
  sigemptyset(&sig_act.sa_mask);

  sigaction(SIGTERM, &sig_act, NULL);

  return 0;
}

bool DataHandler::isBTSarEvent(HciPacketType type, const uint8_t* data) {
  uint16_t opcode = ((data[4] << 8) | data[3]);
  if (type == HCI_PACKET_TYPE_EVENT && (opcode == cmd_opcode)) {
    cmd_opcode = 0x00;
    ALOGD("%s: true",__func__);
    return true;
  }
  return false;
}

bool DataHandler::SendBTSarData(const uint8_t *data, size_t length, DataReadCallback event_cb)
{
  ALOGD("DataHandler::SendBTSarData()");
  if (!isProtocolInitialized(TYPE_BT)) {
    ALOGE("BT HAL not registered, hence not sending BTSAR command");
    return false;
  }
  if (event_cb)
    btsar_event_cb = event_cb;
  if (data != nullptr) {
    cmd_opcode = ((data[1] << 8) | data[0]);
    if (SendData(TYPE_BT, HCI_PACKET_TYPE_COMMAND, data, length) > 0) {
      return true;
    } else {
      // sending command failed, reset cmd_opcode again
      cmd_opcode = 0x00;
    }
  }
  return false;
}

bool DataHandler::Init(ProtocolType type, InitializeCallback init_cb,
                       DataReadCallback data_read_cb)
{
  // lock required incase of multiple binder threads
  ALOGW("DataHandler:: Init()");
  std::unique_lock<std::mutex> guard(init_mutex_);
  if (!data_handler) {
    data_handler = new DataHandler();
    data_handler->data_service_setup_sighandler();
  }
  return data_handler->Open(type, init_cb, data_read_cb);
}

void DataHandler::CleanUp(ProtocolType type)
{
  // lock is required incase of multiple binder threads
  std::unique_lock<std::mutex> guard(init_mutex_);
  ALOGW("DataHandler::CleanUp()");
  if (data_handler && data_handler->Close(type)) {
    delete data_handler;
    data_handler = NULL;
  }
}

bool DataHandler::isProtocolInitialized(ProtocolType pType)
{
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  bool status = false;
  ALOGD("%s:",__func__);

  it = protocol_info_.find(pType);
  if (it != protocol_info_.end()) {
    ProtocolCallbacksType *cb_data = (ProtocolCallbacksType*)it->second;
    if(!cb_data->is_pending_init_cb && init_status_ == INIT_STATUS_SUCCESS) {
      status = true;
    }
    else {
      status = false;
    }
  }
  else {
    status = false;
  }
  ALOGI("%s: status:%d",__func__, status);
  return status;
}

bool DataHandler::isProtocolAdded(ProtocolType pType)
{
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  bool status = false;
  ALOGD("%s:",__func__);

  it = protocol_info_.find(pType);
  if (it != protocol_info_.end()) {
    status = true;
  }
  else {
    status = false;
  }

  ALOGI("%s: status:%d",__func__, status);
  return status;
}

DataHandler * DataHandler::Get()
{
  return data_handler;
}

DataHandler::DataHandler() {
  logger_ = Logger::Get();
  soc_type_ = GetSocTypeInt();
  cmd_opcode = 0x00;
  is_xmem_read_ = false;
  is_init_thread_killed = true;
  xmem_prop_val_ = 0;
  memset(&host_add_on_features, 0, sizeof( HostAddOnFeatures_t));
}

bool DataHandler::IsSocAlwaysOnEnabled()
{
  /* If HAL is configured as LAZY, the library will be unloaded
   * from the primary memory when the client's usage count goes to
   * zero. For the next BT ON fw will be loaded back. Having SoC
   * always on will have unnecessary power lekage.
   */
#ifndef LAZY_SERVICE
  char value[PROPERTY_VALUE_MAX] = {'\0'};
  logger_->PropertyGet("persist.vendor.service.bdroid.soc.alwayson", value, "false");
  return ((strcmp(value, "true") == 0) &&
            ((soc_type_ == BT_SOC_CHEROKEE) ||
              (soc_type_ == BT_SOC_HASTINGS) ||
                (soc_type_ == BT_SOC_MOSELLE)));
#else
  ALOGD("%s SoC always ON not supported on this platform", __func__);
  return false;
#endif
}

BluetoothSocType DataHandler::GetSocType()
{
  return soc_type_;
}

BluetoothSocType DataHandler::GetSocTypeInt()
{
  int ret = 0;
  char soc[PROPERTY_VALUE_MAX];
  ret = logger_->PropertyGet("persist.vendor.qcom.bluetooth.soc", soc, NULL);
  if (ret == 0) {
    ALOGW("SOC property is not set, so using Default pronto\n ");
    soc_type_ = BT_SOC_DEFAULT;
  } else {
    ALOGW("SOC property  set to %s\n", soc);
    if (!strncasecmp(soc, "rome", sizeof("rome"))) {
      soc_type_ = BT_SOC_ROME;
    } else if (!strncasecmp(soc, "cherokee", sizeof("cherokee"))) {
      soc_type_ = BT_SOC_CHEROKEE;
    } else if (!strncasecmp(soc, "ath3k", sizeof("ath3k"))) {
      soc_type_ = BT_SOC_AR3K;
    } else if (!strncasecmp(soc, "hastings", sizeof("hastings"))) {
      soc_type_ = BT_SOC_HASTINGS;
    } else if (!strncasecmp(soc, "genoa", sizeof("genoa"))) {
      soc_type_ = BT_SOC_GENOA;
    } else if (!strncasecmp(soc, "moselle", sizeof("moselle"))) {
      soc_type_ = BT_SOC_MOSELLE;
    } else {
      ALOGI("persist.vendor.qcom.bluetooth.soc not set, so using pronto.\n");
      soc_type_ = BT_SOC_DEFAULT;
    }
  }
  return soc_type_;
}

// this is used to send the actual packet.
size_t DataHandler::SendData(ProtocolType ptype, HciPacketType packet_type,
                             const uint8_t *data, size_t length)
{
  if (CheckSignalCaughtStatus()) {
    ALOGD("%s: Return as SIGTERM Signal is caught", __func__);
    return 0;
  }

  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  {
    it = protocol_info_.find(ptype);
    if (it == protocol_info_.end()) {
      ALOGE("%s: NO entry found for the protocol %d \n", __func__, ptype);
      return 0;
    }
    if (init_status_ != INIT_STATUS_SUCCESS) {
      ALOGE("%s: BT Daemon not initialized, ignore packet", __func__);
      return 0;
    }
  }

#ifdef USER_DEBUG
  if (command_is_reset_uart_flow(data, length)) {
    ALOGD("<%s: received reset UART flow command", __func__);
    //set UART flow on
    HandleFlowControl(USERIAL_OP_FLOW_ON);
    return 0;
  }

  if (command_is_get_cts_status(data, length)) {
    ALOGD("<%s: received get UART CTS command", __func__);
    SendCTSStatusToClient(GetCTSLineStatus());
    return 0;
  }
#endif

  UpdateRingBuffer(packet_type, data, length);
  if (controller_ != nullptr)
     return controller_->SendPacket(packet_type, data, length);

  return 0;
}

#ifdef DUMP_RINGBUF_LOG
void DataHandler::AddHciCommandTag(char* dest_tag_str, struct timeval& time_val, char * opcode)
{
  uint32_t w_index = 0;

  memset(dest_tag_str, 0, RX_TAG_STR_LEN);
  add_time_str(dest_tag_str, &time_val);

  w_index = strlen(dest_tag_str);
  snprintf(dest_tag_str+w_index, strlen(opcode) + 1, "-%s", opcode);
}
#endif

void DataHandler::OnPacketReady(ProtocolType ptype, HciPacketType type,
                                const hidl_vec<uint8_t>*hidl_data)
{
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;

  uint16_t len = hidl_data->size();
  const uint8_t* data = hidl_data->data();
  static bool reset_rxthread_stuck_prop = true;

  logger_->ProcessRx(type, data, len);

  /*  if BQR RIE is enabled then dont send HW err evt coming from SoC
   *  to BT Stack as we will be sending BQR RIE */
  if (len == LENGTH_HW_ERROR_EVT && data[0] == BT_HW_ERR_EVT && data[1] == BT_HW_ERR_FRAME_SIZE
      && data[2] != HOST_SPECIFIC_HW_ERR_EVT && controller_
      && ((UartController *)controller_)->IsBqrRieEnabled()) {
    ALOGD("%s: HW err event from SoC handled internally and not sent to BT stack", __func__);
    ALOGD("%s: HW err packet:  %02x %02x  %02x", __func__, data[0], data[1], data[2]);
    delete hidl_data;
    return;
  }

  if (logger_->IsControllerLogPkt(type, data, len)) {
    ALOGV("%s:Received a controller log packet\n", __func__);
    if(!logger_->IsHciFwSnoopEnabled()) {
      delete hidl_data;
      return;
    }
  }
  if (type == HCI_PACKET_TYPE_EVENT && data_handler->isAwaitedEvent(data, len)) {
    ALOGW("%s: Received event for command sent internally: %02x %02x \n",
            __func__, data[3], data[4]);
    delete hidl_data;
    event_wait = true;
    event_wait_cv.notify_all();
    return;
  }
  if (cmd_opcode != 0x00 && ptype == TYPE_BT && len >= MIN_OPCODE_LEN) {
    if (isBTSarEvent(type, data)) {
      if (btsar_event_cb) {
        btsar_event_cb(type, hidl_data);
      }
      delete hidl_data;
      return;
    }
  }

  ProtocolCallbacksType *cb_data = nullptr;
  /* Dont send any data if cleanup is in progress for FM/ANT */
  if (ptype != TYPE_BT && diag_interface_.GetCleanupStatus(ptype)) {
    if (GetRxthreadStatus(ptype)) {
      SetRxthreadStatus(false, ptype);
      ALOGW("Skip sending packet to client: %d as cleanup in process\n", ptype);
    }
    delete hidl_data;
    return;
  }

  /* stack_timeout_triggered stands to true if BREDR_CLEANUP
   * and STACK_DISABLE timeouts are triggered in stack during
   * BT OFF.
   */
  if (ptype == TYPE_BT && logger_->stack_timeout_triggered) {
    ALOGW("%s: Timeout triggered in stack discarding packet", __func__);
    delete hidl_data;
    return;
  }

  std::unique_lock<std::mutex> guard(internal_mutex_);

  // update the pending Init cb and other callbacks
  it = protocol_info_.find(ptype);
  if (it != protocol_info_.end()) {
    cb_data = (ProtocolCallbacksType*)it->second;
  }

  // execute callbacks here
  if (cb_data != nullptr && controller_ != nullptr) {
    if (!diag_interface_.isSsrTriggered() || !((UartController *)controller_)->IsBqrRieEnabled())
      controller_->StartRxThreadTimer();
    cb_data->data_read_cb(type, hidl_data);
    controller_->StopRxThreadTimer();
    logger_->inc_rx_stats_counter();
    /* Reset the prop if previous iterations have incremented
     * the Rx thread stuck property.
     */
    if (reset_rxthread_stuck_prop) {
      std::unique_lock<std::mutex> lock(property_reset_mutex_);
      if (is_last_client_cleanup_in_progress == false) {
        prop_reset_thread = std::thread([]() {
        struct sigaction old_sa, sa;

        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = usr2_handler;
        sigaction(SIGUSR1, &sa, &old_sa);
        ALOGD("%s: Resetting Rx thread stuck prop", __func__);
        property_set("persist.vendor.service.bdroid.rxthread.stuck.count", "0");
        ALOGD("%s: Resetting Rx thread stuck prop completed", __func__);
        });
      } else {
        ALOGD("%s: Unable to start prop_reset_thread as cleanup is in process", __func__);
      }
      reset_rxthread_stuck_prop = false;
    }
  } else {
    ALOGD("%s: packet discarded and not handled", __func__);
    for (int pktindex = 0; pktindex < len && pktindex < 5; ++pktindex)
      ALOGD("%s: discarded packet[%d] = \t0x%02x ", __func__, pktindex, data[pktindex]);
  }
  delete hidl_data;
}

// signal handler
void DataHandler::usr1_handler(int /* s */)
{
  bool status = TRUE;
  ALOGD("%s: Unlocking bugreport mutex as init thread killed", __func__);
  Logger::bugreport_mutex.unlock();
  ALOGI("%s: exit\n", __func__);
  Wakelock :: UnlockWakelockMutex();
  pthread_exit(&status);
}

// signal handler
void DataHandler::usr2_handler(int /* s */)
{
  bool status = TRUE;

  ALOGI("%s: exit\n", __func__);
  pthread_exit(&status);
}

unsigned int  DataHandler :: GetRxthreadStatus(ProtocolType Type)
{
  return (RxthreadStatus & (0x01 << Type));
}

void DataHandler :: SetRxthreadStatus(bool status, ProtocolType Type)
{
  if (status)
    RxthreadStatus = (RxthreadStatus | (0x01 << Type));
  else
    RxthreadStatus = (RxthreadStatus & (~(0x01 << Type)));

}

unsigned int  DataHandler :: GetClientStatus(ProtocolType Type)
{
  return (ClientStatus & (0x01 << Type));
}

void DataHandler :: SetClientStatus(bool status, ProtocolType Type)
{
  if (status)
    ClientStatus = (ClientStatus | (0x01 << Type));
  else
    ClientStatus = (ClientStatus & (~(0x01 << Type)));

  if (logger_)
    logger_->SetClientStatus(status, Type);
}

bool DataHandler::Open(ProtocolType type, InitializeCallback init_cb,
                       DataReadCallback data_read_cb)
{
  char dst_buff[MAX_BUFF_SIZE];
  char init_buff[MAX_BUFF_SIZE];
  struct timeval tv;
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  std::unique_lock<std::mutex> guard(internal_mutex_);

  /* Don't register new client when SSR in progress. This avoids
   * Crash as we will kill daemon after collecting the dump.
   */
  if (diag_interface_.isSsrTriggered()) {
    ALOGE("<%s: Returning as SSR or cleanup in progress>", __func__);
    return false;
  }

  if (isProtocolAdded(type)) {
    ALOGE("<%s: Returning as protocol already added>", __func__);
    return false;
  }

  ALOGI("Open init_status %d \n", init_status_);
  SetClientStatus(true, type);
  SetRxthreadStatus(true, type);

  gettimeofday(&tv, NULL);
  snprintf(init_buff, sizeof(init_buff), "HCI initialize rcvd from client type = %d", type);
  BtState::Get()->AddLogTag(dst_buff, tv, init_buff);
  BtState::Get()->SetTsHCIInitClose(HCI_INIT, dst_buff);

  // update the pending Init cb and other callbacks
  it = protocol_info_.find(type);
  if (it == protocol_info_.end()) {
    ProtocolCallbacksType *cb_data  = new (ProtocolCallbacksType);
    cb_data->type = type;
    cb_data->is_pending_init_cb = true;
    cb_data->init_cb = init_cb;
    cb_data->data_read_cb = data_read_cb;
    protocol_info_[type] = cb_data;
  }
  switch (init_status_) {
    case INIT_STATUS_INITIALIZING:
      return true;
      break;
    case INIT_STATUS_SUCCESS:
      /* During previous BT ON, Stack timeout might be triggered
       * but HIDL is still active to serve other clients. Resetting
       * the flag to clear previous BT stack timeout state.
       */
      if (type == TYPE_BT && logger_)
        logger_->stack_timeout_triggered = false;

      it = protocol_info_.find(type);
      if (it != protocol_info_.end()) {
        ProtocolCallbacksType *cb_data = (ProtocolCallbacksType*)it->second;
        cb_data->is_pending_init_cb = false;
        cb_data->init_cb(true);
      }
      return true;
      break;
    case INIT_STATUS_FAILED:
      init_thread_.join();
      [[fallthrough]];
    case INIT_STATUS_IDLE:
      init_status_ = INIT_STATUS_INITIALIZING;
      if (!IsSocAlwaysOnEnabled()) {
        soc_need_reload_patch = true;
      }
      ALOGI("%s: soc_need_reload_patch = %d",
                  __func__, soc_need_reload_patch);
#ifdef WAKE_LOCK_ENABLED
      Wakelock :: Init();
#endif
      break;
  }

  logger_->ResetCrashReasons();
  init_thread_ = std::thread([this, type]() {
    // Init thread is alive now.
    is_init_thread_killed = false;
    init_thread_id = std::this_thread::get_id();
    bool status = false;
    struct sigaction old_sa, sa;
    char cb_status_buf[MAX_BUFF_SIZE] = {'\0'};
    char dst_buff[MAX_BUFF_SIZE];
    struct timeval tv;

    /* Start Init timer to detect fw download stuck. */
    StartInitTimer();
    is_last_client_cleanup_in_progress = false;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = usr1_handler;
    sigaction(SIGUSR1, &sa, &old_sa);

    if (soc_type_ == BT_SOC_SMD) {
      controller_ = static_cast<Controller *> (new MctController(soc_type_));
    } else {
      controller_ = static_cast<Controller *> (new UartController(soc_type_));
    }
    if (controller_) {
      status = controller_->Init([this](ProtocolType ptype, HciPacketType type,
                                        const hidl_vec<uint8_t> *hidl_data)   {
                                   OnPacketReady(ptype, type, hidl_data);
                                 });
      gettimeofday(&tv, NULL);
      snprintf(dst_buff, sizeof(dst_buff), "Controller Init status = %d", status);
      BtState::Get()->AddLogTag(cb_status_buf, tv, dst_buff);
      BtState::Get()->SetTsCtrlrInitStatus(cb_status_buf);
      SetHostAddOnFeatures(controller_->GetChipVersion());
    }

    if (status) {
      if (!soc_need_reload_patch) {
        if (!sendCommandWait(HCI_RESET_CMD, type)) {
          StopInitTimer();
          ALOGE("%s: Failed to receive rsp for first HCI RESET CMD", __func__);
          ALOGE("%s: SSR is in progress returning from here", __func__);
          // Init thread exited.
          is_init_thread_killed = true;
          return;
        }
        if (!sendCommandWait(HCI_WRITE_BD_ADDRESS, type)) {
          StopInitTimer();
          ALOGE("%s: Failed to receive rsp for HCI WRITE BD address CMD", __func__);
          ALOGE("%s: SSR is in progress returning from here", __func__);
          // Init thread exited.
          is_init_thread_killed = true;
          return;
        }
        // Reset reason to default.
        logger_->SetSecondaryCrashReason(BT_SOC_REASON_DEFAULT);
      }
    }

    StopInitTimer();
    std::unique_lock<std::mutex> guard(internal_mutex_);
    if (status) {
      /* Stop moving further if timeout detected */
      {
        guard.unlock();
        std::unique_lock<std::mutex> lock(DataHandler::init_timer_mutex_);
        if (GetInitTimerState() == TIMER_OVERFLOW) {
          ALOGW("Initialization timeout detected cleanup is in process");
          // Init thread exited.
          is_init_thread_killed = true;
          return;
        }
        guard.lock();
        init_status_ = INIT_STATUS_SUCCESS;
        ALOGD("Firmware download succeded.");
      }
    } else {
      /* Stop moving further if timeout is detected */
      {
        guard.unlock();
        std::unique_lock<std::mutex> lock(DataHandler::init_timer_mutex_);
        if (GetInitTimerState() == TIMER_OVERFLOW || logger_->isSsrTriggered()) {
          ALOGW("Init timeout or SSR detected discarding cleanup from init thread");
          // Init thread exited.
          is_init_thread_killed = true;
          return;
        }
        guard.lock();
        ALOGE("Controller Init failed ");
        init_status_ = INIT_STATUS_FAILED;
        logger_->SetRecoveryStartTime();
      }

      /* Setting primary and secondary reason as PATCH_DLDNG_FAILED
       * in case of Controller init failure
       */
      logger_->SetPrimaryCrashReason(BT_HOST_REASON_INIT_FAILED);
      if (logger_->GetInitFailureReason() != BT_SOC_REASON_DEFAULT) {
        logger_->SetSecondaryCrashReason(logger_->GetInitFailureReason());
      } else if (soc_type_ != BT_SOC_SMD && logger_->GetUartErrCode() != UART_REASON_DEFAULT) {
        logger_->SetSecondaryCrashReason(logger_->GetUartErrCode());
      } else {
        logger_->SetSecondaryCrashReason(BT_SOC_REASON_DEFAULT);
      }

      if (controller_) {
        /* Unlocking internal_mutex so that root inflammation event can be sent */
        guard.unlock();
        SendBqrRiePacket();
        guard.lock();
      }

      // Collect dumps only if there is no forced reboot
      if (data_handler->CheckSignalCaughtStatus() == false) {
        logger_->PrepareDumpProcess();
        logger_->StoreCrashReason();
        // Add Delay list info. in state file object.
        logger_->AddDelayListInfo();
        logger_->CollectDumps(true, true);
      }
    }

    std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
    for (auto& it: protocol_info_) {
      ProtocolCallbacksType *cb_data = (ProtocolCallbacksType*)it.second;
      cb_data->is_pending_init_cb = false;
      gettimeofday(&tv, NULL);
      snprintf(dst_buff, sizeof(dst_buff), "Init callback status = %d", status);
      BtState::Get()->AddLogTag(cb_status_buf, tv, dst_buff);
      BtState::Get()->SetTsStatusOfCbSent(cb_status_buf);
      cb_data->init_cb(status);
    }

    // clear the list if the controller open call fails
    if (!status) {
      /* clearing up all callback data as controller init itself failed */
      for (auto& it: protocol_info_) {
        ProtocolCallbacksType *cb_data = (ProtocolCallbacksType*)it.second;
        delete (cb_data);
      }
      protocol_info_.clear();
      BtState::Get()->Cleanup();
#ifdef USER_DEBUG
      if (!data_handler->CheckSignalCaughtStatus()) {
        char value[PROPERTY_VALUE_MAX] = {'\0'};
        property_get("persist.vendor.service.bdroid.trigger_crash", value, "0");
        // call kernel panic so that all dumps are collected
        if (strcmp(value, "1") == 0) {
          ALOGE("%s: Do kernel panic immediately as property \"trigger_crash\" set to %s",
                 __func__, value);
          if (((UartController *)controller_)->bt_kernel_panic() == 0) {
            return;
          } else {
            ALOGE("%s: kernel panic failed, doing abort", __func__);
          }
        }
        ALOGE("%s: Aborting daemon to recover as controller init failed", __func__);
        abort();
      } else {
        // Delete current dumped logs, as issue triggered during reboot.
        logger_->DeleteCurrentDumpedFiles();
        /* user triggerred reboot, no need to call abort */
        ALOGE("%s: Killing daemon as user triggered forced reboot", __func__);
        kill(getpid(), SIGKILL);
      }
#else
      ALOGE("%s: Killing daemon to recover as controller init failed", __func__);
      kill(getpid(), SIGKILL);
#endif

    }
    guard.unlock();
    // BT ON successful
    property_set("persist.vendor.service.bdroid.system_delay_crash_count", "0");
    // Init thread exited.
    is_init_thread_killed = true;
    ALOGD("%s: init thread exited now", __func__);
  });
  return true;
}

bool DataHandler::Close(ProtocolType type)
{
  char dst_buff[MAX_BUFF_SIZE];
  char type_buff[MAX_BUFF_SIZE];
  struct timeval tv;
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;

  /* stop init timer if close is called by last client. */
  if (protocol_info_.size() == MIN_CLIENTS_ACTIVE) {
    StopInitTimer();
    {
      std::unique_lock<std::mutex> lock(property_reset_mutex_);
      is_last_client_cleanup_in_progress = true;
      if (prop_reset_thread.joinable()) {
        ALOGD("%s: sending SIGUSR1 signal", __func__);
        pthread_kill(prop_reset_thread.native_handle(), SIGUSR1);
        ALOGD("%s: joining prop reset thread", __func__);
        prop_reset_thread.join();
        ALOGI("%s: joined prop reset thread", __func__);
      }
    }
  }

  if (!controller_) {
#ifdef WAKE_LOCK_ENABLED
    Wakelock :: CleanUp();
#endif
    /* Do cleanup if controller init failed */
    if (init_status_ == INIT_STATUS_FAILED) {
      if (init_thread_.joinable()) {
        ALOGD("%s: joining init thread", __func__);
        init_thread_.join();
        ALOGI("%s: joined init thread", __func__);
      }
      return true;
    } else {
      if (init_thread_.joinable()) {
        ALOGD("%s: sending SIGUSR1 signal", __func__);
        pthread_kill(init_thread_.native_handle(), SIGUSR1);
        ALOGD("%s: joining init thread", __func__);
        init_thread_.join();
        ALOGI("%s: joined Init thread", __func__);
      }

      /*callback cleanup incase when controller constructor
        call is ongoing/stuck and we call close*/
      ALOGD("%s: deleting callback data", __func__);
      it = protocol_info_.find(type);
      if (it != protocol_info_.end()) {
        ProtocolCallbacksType *cb_data = reinterpret_cast<ProtocolCallbacksType *> (it->second);
        delete (cb_data);
        protocol_info_.erase(it);
      }
#ifdef USER_DEBUG
      if (data_handler->GetInitTimerState() != TIMER_OVERFLOW) {
        /* close called before startup time (2.9 sec) is finished  */
        ALOGE("%s: Killing daemon to recover as close called before startup timer expiry",
              __func__);
        kill(getpid(), SIGKILL);
      } else if (data_handler->CheckSignalCaughtStatus()) {
        // Delete current dumped logs, as issue triggered during reboot.
        logger_->DeleteCurrentDumpedFiles();
        /* user triggerred reboot, no need to call abort */
        ALOGE("%s: Killing daemon as user triggered forced reboot", __func__);
        kill(getpid(), SIGKILL);
      } else {
        ALOGE("%s: Aborting daemon to recover as controller constructor is stuck", __func__);
        abort();
      }
#else
      ALOGE("%s: Killing daemon to recover as controller constructor is stuck", __func__);
      kill(getpid(), SIGKILL);
#endif
    }
    return false;
  }

  if (soc_type_ != BT_SOC_SMD && controller_)
    ((UartController *)controller_)->SetCleanupStatusDuringSSR();

  gettimeofday(&tv, NULL);
  snprintf(type_buff, sizeof(type_buff), "HCI Close rcvd from client type = %d", type);
  BtState::Get()->AddLogTag(dst_buff, tv, type_buff);
  BtState::Get()->SetTsHCIInitClose(HCI_CLOSE, dst_buff);
  controller_->WaitforCrashdumpFinish();

  bool status = false;
  bool Cleanup_Status = true;

  ALOGI("DataHandler:: init_status %d", init_status_);

  ALOGD("%s: Signal close to Diag interface", __func__);
  if (!diag_interface_.SignalHALCleanup(type)) {
    ALOGE("%s: Returning as SSR or cleanup in progress", __func__);
    Cleanup_Status = false;
  }
  /* In case FM/ANT Client we set client status false at the start of close
   * as no data will be sent from now on.
   * In case BT Client we set client status false if init status is INIT_STATUS_SUCCESS
   * as no data will be sent from now on.
   * If init status is INIT_STATUS_INITIALIZING then dont set client status as false as
   * we need to send BQR RIE to BT STACK and after that eventually kill/abort HIDL daemon.
   */
  if (type != TYPE_BT || (type == TYPE_BT && init_status_ == INIT_STATUS_SUCCESS))
    SetClientStatus(false, type);

  /* Stop moving forward if the HAL Cleanup is in process */
  if (!Cleanup_Status)
    return false;

  if (data_handler && type == TYPE_BT && init_status_ == INIT_STATUS_SUCCESS) {
    if (!data_handler->sendCommandWait(HCI_RESET_CMD, type)) {
      ALOGE("%s:Failed to receive response for second HCI RESET CMD", __func__);
      ALOGE("%s SSR is in progress returning from here", __func__);
      return false;
    }
  }

  std::unique_lock<std::mutex> guard(internal_mutex_);
  if (protocol_info_.size() == 1) {

    /* Unlock internal mutex to unblock Rx thread if they are any pending packets
     * to be posted, So that CC for Pre shutdown command will be recevied.
     */
    guard.unlock();
    if(data_handler && data_handler->soc_type_ >= BT_SOC_CHEROKEE &&
       init_status_ == INIT_STATUS_SUCCESS) {

      if (!data_handler->sendCommandWait(SOC_PRE_SHUTDOWN_CMD, type)) {
        ALOGE("%s: Failed to receive response for PRE SHUTDOWN CMD", __func__);
        ALOGE("%s: SSR is in progress returning from here", __func__);
        return false;
      }
    }

    if (init_status_ == INIT_STATUS_SUCCESS) {
      /* checking SocAlwaysOn property to decide whether to close
       * transport completely or partialy
       */
      if (!IsSocAlwaysOnEnabled()) {
        soc_need_reload_patch = true;
      } else {
        soc_need_reload_patch = false;
      }
      logger_->PrepareDumpProcess();
      /*collect ringbuffer dumps if its respective property is set to true*/
      logger_->CollectDumps(false, false);
      /* Cleanup returns false when dump collected and other thread
       * is in process of doing post dump procedure.
       */
      if (!controller_->Cleanup()) {
        ALOGW("Skip controller cleanup as other thread is in process of cleanup: %s", __func__);
        return false;
      }
      ALOGW("controller Cleanup done");
      if (!data_handler->CheckSignalCaughtStatus()) {
        delete controller_;
        controller_ = nullptr;
      }
    }
    guard.lock();

    if (init_status_ > INIT_STATUS_IDLE ) {
      if ( INIT_STATUS_INITIALIZING == init_status_) {
        // Kill init thread.
        KillInitThread();
        // Collect dumps only if there is no forced reboot
        if (!data_handler->CheckSignalCaughtStatus() &&
            data_handler->GetInitTimerState() == TIMER_OVERFLOW) {
          logger_->PrepareDumpProcess();
          logger_->SetPrimaryCrashReason(BT_HOST_REASON_INIT_FAILED);
          logger_->StoreCrashReason();
          logger_->CollectDumps(true, true);

          /* Unlocking internal_mutex so that root inflammation event can be sent */
          guard.unlock();
          SendBqrRiePacket();
          guard.lock();
        }

        /* complete transport cleanup as we are aborting/killing HIDL daemon */
        soc_need_reload_patch = true;

        // close the transport
        controller_->Disconnect();

#ifdef WAKE_LOCK_ENABLED
        Wakelock :: CleanUp();
#endif

#ifdef USER_DEBUG
        if (data_handler->GetInitTimerState() != TIMER_OVERFLOW) {
          /* close called before startup time (2.9 sec) is finished  */
          ALOGE("%s: Killing daemon to recover as close called before startup timer expiry",
                __func__);
          kill(getpid(), SIGKILL);
        } else if (data_handler->CheckSignalCaughtStatus()) {
          // Delete current dumped logs, as issue triggered during reboot.
          logger_->DeleteCurrentDumpedFiles();
          /* user triggerred reboot, no need to call abort */
          ALOGE("%s: Killing daemon as user triggered forced reboot", __func__);
          kill(getpid(), SIGKILL);
        } else {
          ALOGE("%s: Aborting daemon to recover as firmware download is stuck", __func__);
          abort();
        }
#else
        ALOGE("%s: Killing daemon to recover as firmware download is stuck", __func__);
        kill(getpid(), SIGKILL);
#endif
      }

      if (init_thread_.joinable()) {
        ALOGD("%s: joining init thread", __func__);
        init_thread_.join();
        ALOGI("DataHandler:: joined Init thread \n");
      }

      init_status_ = INIT_STATUS_IDLE;
    }
    BtState::Get()->Cleanup();
#ifdef WAKE_LOCK_ENABLED
    Wakelock :: CleanUp();
#endif

    status = true;
  }

  it = protocol_info_.find(type);
  if (it != protocol_info_.end()) {
    ProtocolCallbacksType *cb_data = reinterpret_cast<ProtocolCallbacksType *> (it->second);
    delete (cb_data);
    protocol_info_.erase(it);
  }

  diag_interface_.SignalEndofCleanup(type);
  return status;
}

void DataHandler::UpdateRingBuffer(HciPacketType packet_type, const uint8_t *data, int length)
{
#ifdef DUMP_RINGBUF_LOG
  gettimeofday(&time_hci_cmd_arrived_, NULL);
  snprintf(last_hci_cmd_timestamp_.opcode, OPCODE_LEN, "0x%02x%02x", data[0], data[1]);
  AddHciCommandTag(last_hci_cmd_timestamp_.hcicmd_timestamp,
                   time_hci_cmd_arrived_, last_hci_cmd_timestamp_.opcode);
  logger_->SaveLastStackHciCommand(last_hci_cmd_timestamp_.hcicmd_timestamp);
#endif
  logger_->ProcessTx(packet_type, data, length);
}

bool DataHandler::sendCommandWait(HciCommand cmd, ProtocolType type)
{
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;
  bool event_wait_temp;
  uint16_t awaited_evt_temp;
  PrimaryReasonCode reason;

  /* Data handler is exposed to different layers. In future
   * might be this call can be used in those layers too.
   * To monitor command credit flow,  lock is acquired until
   * the response is received or timeout is occurred.
   */
  std::unique_lock<std::mutex> guard(evt_wait_mutex_);
  gettimeofday(&tv, NULL);
  BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Failed to send internal CMD");
  BtState::Get()->SetHciInternalCmdSent(dst_buff);

  awaited_evt_temp = 0;
  event_wait_temp = event_wait = false;
  reason = BT_HOST_REASON_DEFAULT_NONE;

  switch (cmd) {
    case HCI_RESET_CMD:
    {
      HciPacketType packet_type = HCI_PACKET_TYPE_COMMAND;
      const uint8_t data[] = {0x03, 0x0C, 0x00};
      int length = 3;

      ALOGI("Sending HCI Reset \n");
      awaited_evt = *(uint16_t *)(&data);
      UpdateRingBuffer(packet_type, data, length);
      if (!controller_->SendPacket(packet_type, data, length)) {
        ALOGE("Unable to send HCI Reset \n");
        awaited_evt = 0;
      }
      break;
    }

    case SOC_PRE_SHUTDOWN_CMD:
    {
      HciPacketType packet_type = HCI_PACKET_TYPE_COMMAND;
      const uint8_t data[] = {0x08, 0xFC, 0x00};
      int length = 3;

      ALOGI("Sending Pre-shutdown command \n");
      awaited_evt = *(uint16_t *)(&data);
      UpdateRingBuffer(packet_type, data, length);
      if (!controller_->SendPacket(packet_type, data, length)) {
        ALOGE("Unable to send Pre-shutdown command \n");
        awaited_evt = 0;
      }
      break;
    }

    case HCI_WRITE_BD_ADDRESS:
    {
      HciPacketType packet_type = HCI_PACKET_TYPE_COMMAND;
      uint8_t data[HCI_WRITE_BD_ADDRESS_LENGTH] = {0x14, 0xFC, 0x06};

      BluetoothAddress::GetLocalAddress(&data[HCI_WRITE_BD_ADDRESS_OFFSET]);
      BluetoothAddress::le2bd(&data[HCI_WRITE_BD_ADDRESS_OFFSET]);
      ALOGI("Sending HCI_WRITE_BD_ADDRESS command \n");
      awaited_evt = *(uint16_t *)(&data);
      UpdateRingBuffer(packet_type, data, HCI_WRITE_BD_ADDRESS_LENGTH);
      if (!controller_->SendPacket(packet_type, data, HCI_WRITE_BD_ADDRESS_LENGTH)) {
        ALOGE("Unable to send HCI_WRITE_BD_ADDRESS \n");
        awaited_evt = 0;
      }
      break;
    }

    default:
      ALOGW("%s: fallback to default case as command not handled", __func__);
      return false;
  }

  if (awaited_evt) {
    SetAppropriateLog(cmd, event_wait_temp, type);
    int timeout = (logger_->stack_timeout_triggered == true) ? HCI_CMD_TIMEOUT :
                  INTERNAL_CMD_TIMEOUT;
    event_wait_cv.wait_for(guard, std::chrono::milliseconds(timeout), []
                           {return event_wait;});

    /* event_wait stands false if no response received
     * for the command sent.
    */
    event_wait_temp = event_wait;
    awaited_evt_temp = awaited_evt;
    awaited_evt = 0;
    /* Set appropriate logs based on even wait flag */
    SetAppropriateLog(cmd, event_wait_temp, type);
  } else {
    /* This block of code executes when failed to send command.
     * This failure can be due to SSR is in progress. Check SSR
     * status before updating secondary crash reasons.
     */
    if (diag_interface_.isSsrTriggered()) {
      ALOGE("%s: failed to send internal command as SSR in progress", __func__);
      return event_wait_temp;
    }
    else {
      logger_->SetSecondaryCrashReason(BT_HOST_REASON_FAILED_TO_SEND_CMD);
      reason = BT_HOST_REASON_FAILED_TO_SEND_INTERNAL_CMD;
    }
  }

  if (!event_wait_temp) {
     diag_interface_.SetInternalCmdTimeout();
     diag_interface_.ResetCleanupflag();
     if (reason == BT_HOST_REASON_DEFAULT_NONE) {
       reason = BT_HOST_REASON_SSR_INTERNAL_CMD_TIMEDOUT;
       ALOGE("%s: failed to receive rsp for:%02x %02x", __func__,
             (awaited_evt_temp & 0xFF), (((awaited_evt_temp >> 0x08) & 0xFF)));
     }

   /* Below code update the reason as Rx thread Stuck
    * when SSR is already triggered but not moved further due to cleanup in progress
    */
  if ((controller_->GetPreviousReason() == BT_HOST_REASON_RX_THREAD_STUCK) ||
      (controller_->GetPreviousReason() == BT_HOST_REASON_SSR_UNABLE_TO_WAKEUP_SOC)) {
        ALOGE("%s: overwriting the reason = %02x with prv_reason = %02x",
               __func__, reason, controller_->GetPreviousReason());
        reason = controller_->GetPreviousReason();
     }

     controller_->SsrCleanup(reason);
     controller_->WaitforCrashdumpFinish();
  }

  return event_wait_temp;
}

void DataHandler::SetAppropriateLog(HciCommand cmd, bool event_wait_temp,
                                    ProtocolType type)
{
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  gettimeofday(&tv, NULL);
  switch(cmd) {
    case HCI_RESET_CMD:
    {
      bool cleanup_status = logger_->GetCleanupStatus(type);

      if (awaited_evt && !cleanup_status) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Sent first HCI RESET CMD");
        logger_->SetSecondaryCrashReason(BT_HOST_REASON_HCI_RESET_CC_NOT_RCVD);
      } else if (awaited_evt) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Sent second HCI RESET CMD");
        logger_->SetSecondaryCrashReason(BT_HOST_REASON_HCI_RESET_CC_NOT_RCVD);
      } else if (event_wait_temp && !cleanup_status) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Rsp rcvd for first HCI RESET CMD");
      } else if (event_wait_temp) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Rsp rcvd for second HCI RESET CMD");
      } else if (!event_wait_temp && !cleanup_status) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Rsp not rcvd for first HCI RESET CMD");
      } else if (!event_wait_temp) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Rsp not rcvd for second HCI RESET CMD");
      }

      break;
    }
    case SOC_PRE_SHUTDOWN_CMD:
    {
      if (awaited_evt) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Sent HCI Pre shutdown CMD");
        logger_->SetSecondaryCrashReason(BT_HOST_REASON_HCI_PRE_SHUTDOWN_CC_NOT_RCVD);
      } else if (event_wait_temp) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Rsp rcvd for Pre shutdown CMD");
      } else {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Rsp not rcvd for Pre shutdown CMD");
      }
      break;
    }
    case HCI_WRITE_BD_ADDRESS:
    {
      if (awaited_evt) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Sent HCI BD address CMD");
        logger_->SetSecondaryCrashReason(BT_HOST_REASON_HCI_SET_BD_ADDRESS_CC_NOT_RCVD);
      } else if(event_wait_temp) {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Rsp rcvd for HCI BD address CMD");
      } else {
        BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Rsp not rcvd for HCI BD address CMD");
      }
      break;
    }
    default:
      ALOGW("%s: fallback to default case as command not handled", __func__);
      return;
  }

  if (awaited_evt)
    BtState::Get()->SetHciInternalCmdSent(dst_buff);
  else
    BtState::Get()->SetHciInternalCmdRsp(dst_buff);
}

inline bool DataHandler::isAwaitedEvent(const uint8_t *buff, uint16_t len)
{
  if (len < MIN_OPCODE_LEN)
    return false;

  if(awaited_evt && (*(uint16_t *)(&buff[3])) == awaited_evt)
    return true;
  else
    return false;
}

void DataHandler::SetSignalCaught()
{
  caught_signal = true;
}

bool DataHandler::CheckSignalCaughtStatus()
{
  return caught_signal;
}

SocAddOnFeatures_t* DataHandler:: GetSoCAddOnFeatures()
{
  if (isProtocolInitialized(TYPE_BT)) {
    return controller_->GetAddOnFeatureList();
  } else {
    return NULL;
  }
}

HostAddOnFeatures_t* DataHandler:: GetHostAddOnFeatures()
{
  if (isProtocolInitialized(TYPE_BT) && host_add_on_features.feat_mask_len > 0) {
    return &host_add_on_features;
  } else {
    return NULL;
  }
}

uint64_t DataHandler :: GetChipVersion()
{
  if (isProtocolInitialized(TYPE_BT)) {
    return controller_->GetChipVersion();
  } else {
    return INVALID_CHIP_VERSION;
  }
}

void DataHandler::StartSocCrashWaitTimer()
{
  if (controller_)
    controller_->StartSocCrashWaitTimer();
}

void DataHandler::SendBqrRiePacket()
{
  if (soc_type_ != BT_SOC_SMD && controller_)
    ((UartController *)controller_)->SendBqrRiePacket();
}

void inline DataHandler::SetInitTimerState(TimerState state)
{
  init_timer_.timer_state = state;
}

inline TimerState DataHandler::GetInitTimerState()
{
  return init_timer_.timer_state;
}

void DataHandler::InitTimeOut(union sigval sig)
{
  ALOGE("%s: SoC Initialization stuck detected", __func__);
  std::unique_lock<std::mutex> guard(init_mutex_);
  {
    std::unique_lock<std::mutex> lock(DataHandler::init_timer_mutex_);

    if (!data_handler) {
      ALOGE("%s data handler instance has been destroyed", __func__);
      return;
    }

    /* Discard the timeout callback execution if init_status and
    * timerstate status updated to success.
    */
    if (data_handler->init_status_ != INIT_STATUS_INITIALIZING ||
        data_handler->GetInitTimerState() != TIMER_ACTIVE) {
      ALOGW("Discarding %s:() as Initialization is successful", __func__);
      return;
    }

    data_handler->SetInitTimerState(TIMER_OVERFLOW);
  }
  // Start SSR.
  if (data_handler && data_handler->controller_ != nullptr) {
    UartController * instance = (UartController *)data_handler->controller_;
    instance->SsrCleanup(BT_HOST_REASON_INIT_FAILED);
  }

  if (data_handler && data_handler->isProtocolAdded(TYPE_BT))
    data_handler->Close(TYPE_BT);

  if (data_handler && data_handler->isProtocolAdded(TYPE_FM))
    data_handler->Close(TYPE_FM);

  if (data_handler && data_handler->isProtocolAdded(TYPE_ANT))
    data_handler->Close(TYPE_ANT);
}

void DataHandler::StopInitTimer()
{
  struct itimerspec ts;
  TimerState init_timer_state;

  std::unique_lock<std::mutex> lock(DataHandler::init_timer_mutex_);
  init_timer_state = GetInitTimerState();

  if (init_timer_state == TIMER_NOT_CREATED) {
      ALOGD("%s: InitTimer already stopped", __func__);
      return;
  } else if (init_timer_state != TIMER_NOT_CREATED &&
             init_timer_state!= TIMER_OVERFLOW) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    if (timer_settime(init_timer_.timer_id, 0, &ts, 0) == -1) {
      ALOGE("%s:Failed to stop Init thread timer", __func__);
    }
    timer_delete(init_timer_.timer_id);
    SetInitTimerState(TIMER_NOT_CREATED);
    ALOGD("%s: InitTimer Stopped", __func__);
    return;
  } else if(init_timer_state == TIMER_OVERFLOW) {
    ALOGW("%s: Failed to stop Init timer this could be due to TIMEOUT", __func__);
  }
}

void DataHandler::StartInitTimer()
{
  struct itimerspec ts;
  struct sigevent se;
  uint32_t timeout;
  int prop_val = 0;

  ALOGV("%s", __func__);
  if (init_timer_.timer_state == TIMER_NOT_CREATED) {
    se.sigev_notify_function = (void (*)(union sigval))InitTimeOut;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &init_timer_.timer_id;
    se.sigev_notify_attributes = NULL;

    if (!timer_create(CLOCK_MONOTONIC, &se, &init_timer_.timer_id))
      SetInitTimerState(TIMER_CREATED);
    else
      ALOGE("%s: Failed to create InitTimer", __func__);
  }

  if ((GetInitTimerState() == TIMER_CREATED) ||
      (GetInitTimerState() == TIMER_OVERFLOW)) {
    prop_val = IsXMEMEnabled();
    if (prop_val == 1) {
      timeout = HIDL_INIT_TIMEOUT_DEFAULT_XMEM;
    } else if(prop_val == 2) {
      timeout = HIDL_INIT_TIMEOUT_XMEM;
    } else {
      timeout = HIDL_INIT_TIMEOUT;
    }
    ts.it_value.tv_sec = (timeout / 1000);
    ts.it_value.tv_nsec = 1000000 * (timeout % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    if ((timer_settime(init_timer_.timer_id, 0, &ts, 0)) == -1) {
      ALOGE("%s: Failed to start Init timer", __func__);
      return;
    } else {
      SetInitTimerState(TIMER_ACTIVE);
      ALOGD("%s: Init timer started", __func__);
    }
  }
}

int DataHandler::IsXMEMEnabled() {
  char value[PROPERTY_VALUE_MAX] = {'\0'};
  int prop_val = 0;

  if(!is_xmem_read_) {
    logger_->PropertyGet("persist.vendor.bluetooth.enable_XMEM", value, "0");
    prop_val = atoi(value);
    ALOGD("%s : XMEM property value = %d", __func__, prop_val);
    if (prop_val >=0 && prop_val <= 2) {
      xmem_prop_val_ = prop_val;
    } else {
      ALOGE("%s: Invalid value %d set for enable xmem property: \n"
            "\"persist.vendor.bluetooth.enable_XMEM\"\n"
            "set 1 for enabling default xmem patch download configuration OR\n"
            "set 2 for xmem patch with rsp for every tlv download cmd OR\n"
            "set 0 to disable xmem patch download", __func__, prop_val);
      return 0;
    }
    is_xmem_read_ = true;
  }
  return xmem_prop_val_;
}

int DataHandler::GetInitStatus() {
  return init_status_;
}

void DataHandler::StackTimeoutTriggered()
{
  ALOGW("%s: Stack was not properly closed", __func__);
  Logger::Get()->stack_timeout_triggered = true;
}

void DataHandler:: LogPwrSrcsUartFlowCtrl() {
  if (data_handler && data_handler->controller_ != nullptr) {
    UartController * instance = (UartController *)data_handler->controller_;
    instance->LogPwrSrcsUartFlowCtrl();
  }
}

bool DataHandler :: CheckForUartFailureStatus() {
  if (data_handler && data_handler->controller_ != nullptr) {
    UartController * instance = (UartController *)data_handler->controller_;
    return instance->CheckForUartFailureStatus();
  }
  return false;
}

void DataHandler :: SetHostAddOnFeatures(uint64_t chip_ver) {
  soc_info_v0_1 soc_info = {MSM_CPU_UNKNOWN};

  /* | UNICAST | BroadCast Assist |  BroadCast Service|
   * | Stereo recording |LC3Q |
   */
  uint8_t adv_audio_support_mask = 0;

  ALOGI("%s: chip_ver: (0x%16llx), chip_ver_str: %s", __func__,
      (unsigned long long)chip_ver, convertChipVerToStr(chip_ver));

  ALOGI("%s: before get_soc_info call", __func__);
  get_soc_info(&soc_info);
  ALOGI("%s: after get_soc_info call", __func__);

  switch (chip_ver) {
    case ROME_VER_2_1:
      break;
    case ROME_VER_3_0:
      break;
    case ROME_VER_3_2:
      break;
    case CHEROKEE_VER_2_0:
      break;
    case CHEROKEE_VER_2_0_1:
      break;
    case CHEROKEE_VER_2_1:
      break;
    case CHEROKEE_VER_2_1_1:
      break;
    case CHEROKEE_VER_3_1:
      break;
    case CHEROKEE_VER_3_2:
    case CHEROKEE_VER_3_2_UMC:
      break;
    case APACHE_VER_1_0_0:
      break;
    case APACHE_VER_1_1_0:
      break;
    case APACHE_VER_1_2_0:
      break;
    case APACHE_VER_1_2_1:
      break;
    case COMANCHE_VER_1_0_1:
      break;
    case COMANCHE_VER_1_1:
      break;
    case COMANCHE_VER_1_2:
    case COMANCHE_VER_1_2_UMC:
      break;
    case COMANCHE_VER_1_3:
    case COMANCHE_VER_1_3_TSMC:
    case COMANCHE_VER_1_3_UMC:
      break;
    case GENOA_VER_1_0:
      break;
    case GENOA_VER_2_0:
      break;
    case GENOA_VER_2_0_0_2:
      break;
    case HASTINGS_VER_2_0:
      break;
    case HSP_VER_1_0:
      break;
    case HSP_VER_1_1:
      break;
    case HSP_VER_2_0:
      break;
    case HSP_VER_2_0_G:
      break;
    case HSP_VER_2_1:
      break;
    case HSP_VER_2_1_G:
      break;
    case MOSELLE_VER_1_0:
      break;
    case MOSELLE_VER_1_1:
      break;
    case INVALID_CHIP_VERSION:
      if (soc_type_ == BT_SOC_SMD) {
        ALOGI("%s: soc type is pronto", __func__);
      }
      return;
    default:
      return;
  }

  if (soc_info.msm_cpu <= MSM_CPU_UNKNOWN) {
    ALOGW("%s: failed to get soc info or feat not req for this chip", __func__);
  } else {
    ALOGI("%s: soc_info.msm_cpu %d", __func__, soc_info.msm_cpu);

    if (soc_info.msm_cpu == MSM_CPU_LAHAINA) {
      if (chip_ver == HSP_VER_2_1 ||
        chip_ver == HSP_VER_2_1_G) {
        adv_audio_support_mask |= HOST_ADD_ON_ADV_AUDIO_BCA_FEAT_MASK;
        adv_audio_support_mask |= HOST_ADD_ON_ADV_AUDIO_BCS_FEAT_MASK;
      }
    }

    host_add_on_features.features[0] |= adv_audio_support_mask;

    /*  Look for the index of highest byte where feature bit set
     *  index + 1 gives the length of features mask
     */
    for (int i = HOST_ADD_ON_FEATURE_MASK_MAX_LENGTH - 1; i >= 0; i--) {
      if(host_add_on_features.features[i] & HOST_ADD_ON_FEATURE_MASK_MAX_LENGTH) {
        host_add_on_features.feat_mask_len = i + 1;
        break;
      }
    }

    if (host_add_on_features.feat_mask_len) {
      ALOGI("%s: Decoded host add on features for:%s with feature set val:%d",
             __func__, convertMSMToStr(soc_info), host_add_on_features.features[0]);
    }
  }
}

const char * DataHandler :: convertChipVerToStr(uint64_t chip_ver) {
  switch (chip_ver){
    case ROME_VER_2_1:
      return "ROME_VER_2_1";
    case ROME_VER_3_0:
      return "ROME_VER_3_0";
    case ROME_VER_3_2:
      return "ROME_VER_3_2";
    case CHEROKEE_VER_2_0:
      return "CHEROKEE_VER_2_0";
    case CHEROKEE_VER_2_0_1:
      return "CHEROKEE_VER_2_0_1";
    case CHEROKEE_VER_2_1:
      return "CHEROKEE_VER_2_1";
    case CHEROKEE_VER_2_1_1:
      return "CHEROKEE_VER_2_1_1";
    case CHEROKEE_VER_3_1:
      return "CHEROKEE_VER_3_1";
    case CHEROKEE_VER_3_2:
    case CHEROKEE_VER_3_2_UMC:
      return "CHEROKEE_VER_3_2";
    case APACHE_VER_1_0_0:
      return "APACHE_VER_1_0_0";
    case APACHE_VER_1_1_0:
      return "APACHE_VER_1_1_0";
    case APACHE_VER_1_2_0:
      return "APACHE_VER_1_2_0";
    case APACHE_VER_1_2_1:
      return "APACHE_VER_1_2_1";
    case COMANCHE_VER_1_0_1:
      return "COMANCHE_VER_1_0_1";
    case COMANCHE_VER_1_1:
      return "COMANCHE_VER_1_1";
    case COMANCHE_VER_1_2:
    case COMANCHE_VER_1_2_UMC:
      return "COMANCHE_VER_1_2";
    case COMANCHE_VER_1_3:
    case COMANCHE_VER_1_3_TSMC:
    case COMANCHE_VER_1_3_UMC:
      return "COMANCHE_VER_1_3";
    case GENOA_VER_1_0:
      return "GENOA_VER_1_0";
    case GENOA_VER_2_0:
      return "GENOA_VER_2_0";
    case GENOA_VER_2_0_0_2:
      return "GENOA_VER_2_0_0_2";
    case HASTINGS_VER_2_0:
      return "HASTINGS_VER_2_0";
    case HSP_VER_1_0:
      return "HSP_VER_1_0";
    case HSP_VER_1_1:
      return "HSP_VER_1_1";
    case HSP_VER_2_0:
    case HSP_VER_2_0_G:
      return "HSP_VER_2_0";
    case HSP_VER_2_1:
    case HSP_VER_2_1_G:
      return "HSP_VER_2_1";
    case MOSELLE_VER_1_0:
      return "MOSELLE_VER_1_0";
    case MOSELLE_VER_1_1:
      return "MOSELLE_VER_1_1";
    case INVALID_CHIP_VERSION:
      if (soc_type_ == BT_SOC_SMD)
        return "Pronto";
    default:
      return "INVALID_CHIP_VERSION";
  }
}

void DataHandler :: KillInitThread() {
  if (!is_init_thread_killed) {
    ALOGE("%s: Killing init thread", __func__);
    if (pthread_kill(init_thread_.native_handle(), SIGUSR1))
      ALOGE("%s: Failed to kill init thread, Errno: %s", __func__, strerror(errno));
    is_init_thread_killed = true;
    // Sleep for 50 ms to ensure that thread exit happens
    usleep(50 * 1000);

    // Ensure acquired mutex(s) are released
    logger_->UnlockRingbufferMutex();
    controller_->UnlockControllerMutex();
    logger_->UnlockDiagMutex();
  }
}

std::thread::id DataHandler :: GetInitThreadId() {
  return init_thread_id;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
