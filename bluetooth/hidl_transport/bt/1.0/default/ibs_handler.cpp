/*==========================================================================
Description
  Wcnss_ibs has the state-machine and logic to process sleep bytes to excercise
  SIBS (Software In-band sleep) protocol

# Copyright (c) 2013-2014, 2016-2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <asm-generic/ioctls.h>
#include <hidl/HidlSupport.h>
#include <ibs_handler.h>
#include "hci_uart_transport.h"
#include "uart_controller.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.bluetooth@1.0-ibs_handler"

#define UART_VOTEOFF_DELAY      (100)
#define WAKELOCK_RELEASE_DELAY  (150)
#define CLOCK_OFF               (0)
#define CLOCK_ON                (1)
#define RXDWAKEUP_REPEAT_INTERVAL  (10)
#define DEFAULT_LPM_IDLE_TIMEOUT (1000)
#define SLEEP_IND_WAIT_MS        (1000)
#define SIGNAL_WAIT_PERIOD       (75)
/* 250ms is buffer for command timeout */
#define MAX_WACK_TIMEOUT         (5000 - 250)
#define INDV_TIMEOUT             (100)
#define UART_SPURIOUS_WAKE_SURPRESS_DELAY      (10*1000)

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef WCNSS_IBS_ENABLED
std::mutex ibs_mutex_;
extern bool soc_need_reload_patch;
bool sibs_enable_status = false;
bool is_sibs_status_read = false;
IbsHandler *IbsHandler::instance_ = NULL;
bool IbsHandler::rxd_wakeup_enabled = false;

IbsHandler *IbsHandler::Init(HciUartTransport* theTransport, HealthInfoLog* theHealthInfo,
           Controller* theController)
{
  if (!instance_) {
    instance_ = new IbsHandler(theTransport, theHealthInfo, theController);
  }
  instance_->InitInt();
  return instance_;
}

void IbsHandler::InitInt()
{
  int status = 0;
  struct timeval tv;

  memset(&bt_lpm_, 0, sizeof(bt_lpm_));
  memset(&voteoff_timer_, 0, sizeof(voteoff_timer_));
  memset(&ibs_spurious_wake_struct, 0, sizeof(ibs_spurious_wake_struct));
  bt_lpm_.timeout_ms = DEFAULT_LPM_IDLE_TIMEOUT;

  tty_fd_ = uart_transport_->GetCtrlFd();
  ALOGD("%s: tty_fd = %d", __func__, tty_fd_);

  /* SoC Rx state is ASLEEP, when soc always ON is enabled.
   * Because during HIDL close, SoC is sent to sleep state.
   * Soc Rx state is AWAKE when SoC is booting up.
   */
  memset(&state_machine_, 0, sizeof(state_machine_));
  if (soc_need_reload_patch) {
    state_machine_.CLK_STATE = CLOCK_ON;
    UpdateRxState(IBS_RX_AWAKE);
    SetRxVote(1);
  }

  num_try_ = 1;

  struct sigevent se;
  se.sigev_notify_function = VoteOffTimeout;
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &voteoff_timer_.timer_id;
  se.sigev_notify_attributes = NULL;

  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK,
                                            "TIMER CREATE API: VOTE OFF TIMER");
  status = timer_create(CLOCK_MONOTONIC, &se, &voteoff_timer_.timer_id);
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);

  if (status == 0) {
    ALOGV("%s: Vote off timer created", __func__);
    voteoff_timer_.timer_created = true;
  }
  gettimeofday(&tv, NULL);
  bt_logger_->SetCurrentactivityStartTime(tv, BT_HOST_REASON_PROPERTY_GET_STUCK,
                                            "PROPERTY_GET: persist.vendor.bluetooth.rx_delay");
  delayed_wakelock = property_get_int32("persist.vendor.bluetooth.rx_delay", WAKELOCK_RELEASE_DELAY);
  gettimeofday(&tv, NULL);
  bt_logger_->CheckAndAddToDelayList(&tv);

  time_last_rx_sleep = 0;
  time_last_tx_packet = 0;

  /* use CLOCK_MONOTONIC to avoid potential time issue
   * as it won't be changed by system time or user settings
   */
  pthread_condattr_t attr;
  pthread_condattr_init(&attr);
  pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
  pthread_cond_init(&wack_cond_, &attr);
  pthread_condattr_destroy(&attr);
}

IbsHandler * IbsHandler::Get()
{
  return instance_;
}

void IbsHandler::CleanUp(void)
{
  std::unique_lock<std::mutex> guard(ibs_mutex_);
  if (instance_) {
    pthread_cond_destroy(&instance_->wack_cond_);
    delete instance_;
    instance_ = NULL;
  }
}

bool IbsHandler::IsEnabled()
{
  char value[PROPERTY_VALUE_MAX] = { '\0' };
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  if (is_sibs_status_read == false) {
    ALOGD("%s: Reading sibs property", __func__);
    gettimeofday(&tv, NULL);
    BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Reading sibs property");
    BtState::Get()->SetPreReadSibsPropertyTS(dst_buff);
    Logger::Get()->PropertyGet("persist.vendor.service.bdroid.sibs", value, "true");
    ALOGD("%s: Done reading sibs property", __func__);
    gettimeofday(&tv, NULL);
    sibs_enable_status = (strcmp(value, "true") == 0) ? true : false;
    is_sibs_status_read = true;
    BtState::Get()->AddLogTag(dst_buff, tv, (char *)"Done reading sibs property");
    BtState::Get()->SetPostReadSibsPropertyTS(dst_buff);
  }
  ALOGD("%s: SIBS enabled: %s", __func__, ((sibs_enable_status == true) ? "true" : "false"));
  return sibs_enable_status;
}

IbsHandler::IbsHandler(HciUartTransport* theTransport,
                      HealthInfoLog* theHealthInfo,
                      Controller* theController)
{
  ALOGD("%s", __func__);
  uart_transport_ = theTransport;
  bt_logger_ = Logger::Get();
  health_info = theHealthInfo;
  controller = theController;
  state_info_  = BtState::Get();
  state_machine_.timer_created = false;
  voteoff_timer_.timer_created = false;
  ibs_spurious_wake_struct.timer_created = false;
  bt_lpm_.timer_created = false;
  sleep_ind_recvd_ = false;
  if (!soc_need_reload_patch && IbsHandler::rxd_wakeup_enabled)
    ALOGD("%s: RXD Wakeup config enabled in always on mode", __func__);
}

IbsHandler::~IbsHandler()
{
  ALOGI("%s", __func__);

  if (bt_lpm_.timer_created == true) {
    timer_delete(bt_lpm_.timer_id);
    bt_lpm_.timer_created = false;
  }
  if (state_machine_.timer_created == true) {
    timer_delete(state_machine_.timer_id);
    state_machine_.timer_created = false;
  }
  if (voteoff_timer_.timer_created == true) {
    timer_delete(voteoff_timer_.timer_id);
    voteoff_timer_.timer_created = false;
  }
  if (ibs_spurious_wake_struct.timer_created == true) {
    timer_delete(ibs_spurious_wake_struct.timer_id);
    ibs_spurious_wake_struct.timer_created = false;
  }
#ifdef WAKE_LOCK_ENABLED
  struct timeval tv;
  char dst_buff[MAX_BUFF_SIZE];
  gettimeofday(&tv, NULL);
  state_info_->AddLogTag(dst_buff, tv, (char *)"Wakelock :: Release in ~IbsHandler destructor");
  state_info_->SetWakeLockRelTimeName(dst_buff);
  Wakelock :: Release();
#endif
}

inline uint8_t IbsHandler::GetTxVote() {
  return state_machine_.tx_vote;
}

inline void IbsHandler::SetTxVote(uint8_t vote_value) {
  state_machine_.tx_vote = vote_value;
  if (health_info)
    health_info->UpdateTxVote(state_machine_.tx_vote);
}

inline uint8_t IbsHandler::GetRxVote() {
  return state_machine_.rx_vote;
}

inline void IbsHandler::SetRxVote(uint8_t vote_value) {
  state_machine_.rx_vote = vote_value;
  if (health_info)
    health_info->UpdateRxVote(state_machine_.rx_vote);
}


bool IbsHandler::IsIbsCmd(unsigned char byte)
{
  if (byte == IBS_WAKE_ACK || byte == IBS_WAKE_IND || byte == IBS_SLEEP_IND) {
    return true;
  }
  return false;
}
void IbsHandler::WriteIbsCmd(uint8_t ibs_data)
{
  int ret;

  ret = uart_transport_->UartWrite(&ibs_data, 1);
  if (ret < 0) {
    ALOGE("%s: Failed to write IBS data %x", __func__, ibs_data);
  }

#ifdef DUMP_RINGBUF_LOG
  bt_logger_->SaveIbsToRingBuf(HCI_PACKET_TYPE_IBS_CMD, ibs_data);
#endif
}

void IbsHandler::WakeRetransTimeout(union sigval /* sig */)
{
  uint8_t ibs_data;

  std::unique_lock<std::mutex> guard(ibs_mutex_);
  ALOGV("%s: Retransmit wake ind and restart timer", __func__);
  if (!instance_) {
    ALOGD("%s: IBS handler has been destroyed ", __func__);
    return;
  }
  if (instance_->num_try_ == NUM_WACK_RETRANSMIT) {
    ALOGD("%s: Failed to get wake ack from the SOC ", __func__);
    instance_->wack_recvd_ = false;
    pthread_cond_signal(&instance_->wack_cond_);
    return;
  }

  // Repeat SoC RxD wakeup after every 10 FD sent(wake indication) but no FC(wake ack) from SoC.
  if (IbsHandler::rxd_wakeup_enabled && (instance_->num_try_ % RXDWAKEUP_REPEAT_INTERVAL) == 0) {
    /* Acquiring lock TX wake lock here to prevent race condition
     * between TX and WakeRetransTimeout thread.
     */
    pthread_mutex_lock(&(instance_->state_machine_.wack_lock));
    if (instance_->is_tx_signaled_) {
      ALOGD("%s: TX already signaled by RX thread, ignoring RXDwakeup", __func__);
      pthread_mutex_unlock(&(instance_->state_machine_.wack_lock));
      return;
    }
    ALOGD("%s: Retrying RxDwakeup again before FD number: %d", __func__, instance_->num_try_+1);
    instance_->SocRxDWakeup();
    pthread_mutex_unlock(&(instance_->state_machine_.wack_lock));
  }

  instance_->StartWackTimer();
  ibs_data = IBS_WAKE_IND;
  ALOGI("%s: Writing HCI_IBS_WAKE_IND", __func__);
  if (instance_->tty_fd_ > 0)
    instance_->WriteIbsCmd(ibs_data);
  else {
    ALOGE("%s: Invalid FD", __func__);
    return;
  }
  instance_->num_try_++;
}

void IbsHandler::StopWackTimer()
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);
  num_try_ = 1;
  if (state_machine_.timer_created == true) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    status = timer_settime(state_machine_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to stop set wack timer", __func__);
    else if (status == 0)
      ALOGV("%s: Wack timer Stopped", __func__);
  }
}

void IbsHandler::SerialClockVote(uint8_t vote)
{
  uint8_t new_vote;

  pthread_mutex_lock(&state_machine_.clock_vote_lock);
  uint8_t old_vote = (GetRxVote() | GetTxVote());
  ALOGV("%s", __func__);

  switch (vote) {
    case IBS_TX_VOTE_CLOCK_ON:
      SetTxVote(1);
      new_vote = 1;
      break;
    case IBS_RX_VOTE_CLOCK_ON:
      SetRxVote(1);
      new_vote = 1;
      break;
    case IBS_TX_VOTE_CLOCK_OFF:
      SetTxVote(0);
      new_vote = GetRxVote() | GetTxVote();
      break;
    case IBS_RX_VOTE_CLOCK_OFF:
      SetRxVote(0);
      new_vote = GetRxVote() | GetTxVote();
      break;
    default:
      ALOGE("SerialClockVote: Wrong vote requested!\n");
      pthread_mutex_unlock(&state_machine_.clock_vote_lock);
      return;
  }
  ALOGV("new_vote: (%d) ## old-vote: (%d)", new_vote, old_vote);

  if (new_vote != old_vote) {
      UartController* uart_controller = static_cast<UartController*>(instance_->controller);
      /* UART CLK toggle can corrupts bytes on wire, hence reset the invalid byte counter */
      uart_controller->ResetInvalidByteCounter();
    if (new_vote) {
      /*vote UART CLK ON using UART driver's ioctl() */
      ALOGD("%s: vote for UART CLK ON", __func__);
      VoteOnClock();
      state_machine_.CLK_STATE = CLOCK_ON;

      // Trigger UART RxD wakeup mechanism for SoC wakeup.
      if (IbsHandler::rxd_wakeup_enabled && vote == IBS_TX_VOTE_CLOCK_ON) {
        uart_transport_->SocRxDWakeup();
      }

#ifdef WAKE_LOCK_ENABLED
      struct timeval tv;
      char dst_buff[MAX_BUFF_SIZE];
      gettimeofday(&tv, NULL);
      state_info_->AddLogTag(dst_buff, tv, (char *)"Wakelock::Acquire during vote for UART CLK ON");
      state_info_->SetWakeLockAcqTimeName(dst_buff);
      Wakelock :: Acquire();
#endif
    } else {
      /*vote UART CLK OFF using UART driver's ioctl() */
      ALOGD("%s: vote for UART CLK OFF", __func__);
      VoteOffClock();
      state_machine_.CLK_STATE = CLOCK_OFF;
#ifdef WAKE_LOCK_ENABLED
      struct timeval tv;
      char dst_buff[MAX_BUFF_SIZE];
      gettimeofday(&tv, NULL);
      state_info_->AddLogTag(dst_buff, tv, (char *)"Wakelock::ReleaseDelay after UART CLK OFF");
      state_info_->SetWakeLockRelTimeName(dst_buff);
      Wakelock :: ReleaseDelay(delayed_wakelock);
#endif
    }
  }
  pthread_mutex_unlock(&state_machine_.clock_vote_lock);
}

void IbsHandler::StopAllTimers()
{
  ALOGV("%s", __func__);

  StopIdleTimer();
  StopWackTimer();
  StopVoteOffTimer();

  return;
}

void IbsHandler::StartWackTimer()
{
  int status;
  struct itimerspec ts;
  struct sigevent se;

  ALOGV("%s", __func__);
  if (state_machine_.timer_created == false) {
    se.sigev_notify_function = (void (*)(union sigval))WakeRetransTimeout;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &state_machine_.timer_id;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &state_machine_.timer_id);
    if (status == 0) {
      ALOGV("%s: WACK timer created", __func__);
      state_machine_.timer_created = true;
    }
  }
  if (state_machine_.timer_created == true) {
    if (!state_machine_.timeout_ms)
      state_machine_.timeout_ms = TX_IDLE_DELAY;
    ts.it_value.tv_sec = state_machine_.timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000000 * (state_machine_.timeout_ms % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(state_machine_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to set wack timer", __func__);
  }
}
void IbsHandler::VoteOffTimeout(union sigval /* sig */)
{
  std::unique_lock<std::mutex> guard(ibs_mutex_);
  ALOGI("%s: uart serival vote off", __func__);
  if (!instance_) {
    ALOGD("%s: IBS handler has been destroyed ", __func__);
    return;
  }
  instance_->state_machine_.CLK_STATE = CLOCK_OFF;
  instance_->VoteOffClock();
}

void IbsHandler::StopVoteOffTimer()
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);

  if (voteoff_timer_.timer_created == true) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    status = timer_settime(voteoff_timer_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to stop set wack timer", __func__);
    else if (status == 0)
      ALOGV("%s: Wack timer Stopped", __func__);
  }
}
void IbsHandler::StartVoteOffTimer()
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);

  voteoff_timer_.timeout_ms = UART_VOTEOFF_DELAY;
  ts.it_value.tv_sec = voteoff_timer_.timeout_ms / 1000;
  ts.it_value.tv_nsec = 1000000 * (voteoff_timer_.timeout_ms % 1000);
  ts.it_interval.tv_sec = 0;
  ts.it_interval.tv_nsec = 0;

  status = timer_settime(voteoff_timer_.timer_id, 0, &ts, 0);
  if (status == -1)
    ALOGE("%s:Failed to set wack timer", __func__);
}

void IbsHandler::ibs_spurious_wake_timeout(union sigval /* sig */)
{
#ifdef DETECT_SPURIOUS_WAKE
  std::unique_lock<std::mutex> guard(ibs_mutex_);
  ALOGE("%s:expired",__func__);

  if (!instance_) {
    ALOGD("%s: IBS handler has been destroyed ", __func__);
    return;
  }

  UartController* uart_controller = static_cast<UartController*>(instance_->controller);
  uart_controller->SsrCleanup(BT_HOST_REASON_SSR_SPURIOUS_WAKEUP);

  return;
#endif /* DETECT_SPURIOUS_WAKE */
}

void IbsHandler::ibs_spurious_wake_timer_stop()
{
#ifdef DETECT_SPURIOUS_WAKE
    int status;
    struct itimerspec ts;

    if(ibs_spurious_wake_struct.timer_created == true)
    {
        ts.it_value.tv_sec = 0;
        ts.it_value.tv_nsec = 0;
        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;
        status = timer_settime(ibs_spurious_wake_struct.timer_id, 0, &ts, 0);
        if(status == -1)
            ALOGE("%s:Failed to stop set spurious wake timer",__func__);
        else if(status == 0)
            ALOGV("%s: spurious wake timer Stopped",__func__);
     }
#endif /* DETECT_SPURIOUS_WAKE */
}

void IbsHandler::ibs_spurious_wake_timer_start()
{
#ifdef DETECT_SPURIOUS_WAKE
    int status;
    struct itimerspec ts;
    struct sigevent se;
    static int spurious_wake_delay = UART_SPURIOUS_WAKE_SURPRESS_DELAY;

    if(ibs_spurious_wake_struct.timer_created == false)
    {
        se.sigev_notify_function = ibs_spurious_wake_timeout;
        se.sigev_notify = SIGEV_THREAD;
        se.sigev_value.sival_ptr = &ibs_spurious_wake_struct.timer_id;
        se.sigev_notify_attributes = NULL;

        status = timer_create(CLOCK_MONOTONIC, &se, &ibs_spurious_wake_struct.timer_id);
        if (status == 0)
        {
            ALOGD("%s: Spurious wake timer created",__func__);
            ibs_spurious_wake_struct.timer_created = true;
        }
    }

    if(ibs_spurious_wake_struct.timer_created == true)
    {
        if (spurious_wake_delay != 0) {
            ibs_spurious_wake_struct.timeout_ms = spurious_wake_delay;
            ts.it_value.tv_sec = ibs_spurious_wake_struct.timeout_ms/1000;
            ts.it_value.tv_nsec = 1000000*(ibs_spurious_wake_struct.timeout_ms%1000);
            ts.it_interval.tv_sec = 0;
            ts.it_interval.tv_nsec = 0;

            status = timer_settime(ibs_spurious_wake_struct.timer_id, 0, &ts, 0);
            if (status == -1) {
                ALOGE("%s:Failed to set wack timer",__func__);
            }
        }
    }
#endif /* DETECT_SPURIOUS_WAKE */
}

bool IbsHandler::SetNewWACKTimeout (unsigned int *wack_retries )
{
  std::lock_guard<std::recursive_mutex> lock(Logger::monitor_rx_stats);
  struct monitor_rx *rx_stats = bt_logger_->GetRxStats();
  if (!rx_stats) {
    ALOGE("%s: rx_stats is null & SoC not responding,stop sending wake bytes", __func__);
    return false;
  } else if ((rx_stats->lapsed_timeout >= MAX_WACK_TIMEOUT) ||
     (rx_stats->num_valid_pkt_rx == rx_stats->prev_num_valid_pkt_rx)) {
    ALOGE("%s: total No of pkt rx :%u in %u ms", __func__,
          rx_stats->num_valid_pkt_rx, rx_stats->lapsed_timeout);
    if (rx_stats->lapsed_timeout >= MAX_WACK_TIMEOUT) {
      ALOGE("%s: SoC might have responded with wack", __func__);
      ALOGE("%s: HIDL might not recevied due to system slowness", __func__);
      bt_logger_->SetasFpissue();
    } else {
      ALOGE("%s:SoC not responding,stop sending wake bytes", __func__);
    }
    return false;
  } else {
    unsigned int current_pkt = rx_stats->num_valid_pkt_rx - rx_stats->prev_num_valid_pkt_rx;
    rx_stats->prev_num_valid_pkt_rx = rx_stats->num_valid_pkt_rx;
    unsigned int new_timeout = (current_pkt * INDV_TIMEOUT);
    unsigned int remaining_timeout = MAX_WACK_TIMEOUT - rx_stats->lapsed_timeout;
    new_timeout = new_timeout > remaining_timeout ? remaining_timeout : new_timeout;
    /* Add extra waittime as a buffer */
    *wack_retries = (unsigned int)((new_timeout / SIGNAL_WAIT_PERIOD)) + 1;
    ALOGW("%s: total No of pkt rx :%u  and lapsed timeout :%u new timeout configured as :%u ms",
          __func__, rx_stats->num_valid_pkt_rx, rx_stats->lapsed_timeout,
          (*wack_retries) * SIGNAL_WAIT_PERIOD);
    rx_stats->lapsed_timeout += ((*wack_retries) * SIGNAL_WAIT_PERIOD);
    return true;
  }
}

int IbsHandler::DeviceWakeUp()
{
  uint8_t ibs_data;
  int status = 0;

  ALOGV("%s", __func__);

  struct timeval tv;
  char dst_buff[MAX_BUFF_SIZE];
  gettimeofday(&tv, NULL);
  if (time_last_tx_packet != 0) {
    int interval = tv.tv_sec - time_last_tx_packet;
    health_info->UpdateIntervalTxPackets(interval);
  }
  time_last_tx_packet = tv.tv_sec;

  pthread_mutex_lock(&state_machine_.hci_tx_ibs_lock);
  switch (state_machine_.tx_ibs_state) {
    case IBS_TX_ASLEEP:
      ALOGV("%s: TX state ASLEEP, acquire SM lock", __func__);
      StopVoteOffTimer();
      state_machine_.tx_ibs_state = IBS_TX_WAKING;
      ALOGV("%s: UART TX Vote ON", __func__);
      SerialClockVote(IBS_TX_VOTE_CLOCK_ON);
      ALOGV("%s: Acquire wake lock", __func__);
      pthread_mutex_lock(&state_machine_.wack_lock);
      StartWackTimer();
      wack_recvd_ = false;
      is_tx_signaled_ = false;
      ibs_data = IBS_WAKE_IND;
      ALOGI("%s: Writing IBS_WAKE_IND", __func__);
      if (tty_fd_ > 0) {
        bt_logger_->ResetRxStats();
        bt_logger_->StartRxStatsMonitor();
        wait_for_wack = false;
        wack_retries = 0;
        WriteIbsCmd(ibs_data);
        state_info_->AddLogTag(dst_buff, tv, (char *)"Writing FD to SOC");
        state_info_->SetLastWakeIndFromHost(dst_buff);
        health_info->IncreaseWakeSent();
      }
      else {
        ALOGE("%s: Invalid FD", __func__);
        ALOGV("%s: Realease wake lock and SM lock", __func__);
        pthread_mutex_unlock(&state_machine_.wack_lock);
        pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
        return -1;
      }
      while (wack_recvd_ == false) {
        /* use timeout to check wack_recvd_ repeatly as
         * we have found that it is still pending after
         * pthread_cond_signal(&wack_cond_) sometimes which
         * may be caused by not initialized wack_cond_, so
         * use pthread_cond_timedwait instead
         */
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        long long ns = (long long)ts.tv_nsec + 1000000 * (SIGNAL_WAIT_PERIOD%1000);
        ts.tv_nsec = ns%1000000000LL;
        ts.tv_sec += ns/1000000000LL + SIGNAL_WAIT_PERIOD/1000;
        int ret = pthread_cond_timedwait(&wack_cond_, &state_machine_.wack_lock, &ts);

        ALOGD("%s: Unblocked from waiting for FC, pthread_cond_timedwait ret = %d",
            __func__, ret);

        if (wack_recvd_)
          break;
        /* This flag wait_for_wack is set to true when Host already sent
         * enough WACKS, now it is waiting for WACK from SoC.
         */
        if (wait_for_wack)
          wack_retries--;

        if (num_try_ == NUM_WACK_RETRANSMIT && !wait_for_wack) {
          StopWackTimer();
          if (!SetNewWACKTimeout(&wack_retries)) {
            break;
          } else {
            wait_for_wack = true;
          }
         } else if (wait_for_wack && !wack_retries) {
           if (!SetNewWACKTimeout(&wack_retries)) {
            break;
          } else {
            wait_for_wack = true;
          }
         }
      }
      wait_for_wack = false;
      wack_retries = 0;
      bt_logger_->ResetRxStats();
      if (wack_recvd_ == true) {
        ALOGV("%s: Received wack ack", __func__);
        ALOGV("%s: TX state transition to AWAKE", __func__);
        state_machine_.tx_ibs_state = IBS_TX_AWAKE;
        wack_recvd_ = false;
        status = 0;
      } else {
        ALOGE("%s: Failed to wake SOC", __func__);
        state_machine_.tx_ibs_state = IBS_TX_ASLEEP;
        StopWackTimer();
        wack_recvd_ = false;
        status = -1;
      }
      ALOGV("%s: Realease wake lock and SM lock", __func__);
      pthread_mutex_unlock(&state_machine_.wack_lock);
      break;
    case IBS_TX_WAKING:
      ALOGV("%s: SOC is WAKING UP", __func__);
      break;
    case IBS_TX_AWAKE:
      ALOGV("%s: SOC is already AWAKE", __func__);
      break;
  }
  pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
  return status;
}

void IbsHandler::UpdateRxState(rx_ibs_states state)
{
  std::unique_lock<std::mutex> guard(ibs_rx_state_);
  state_machine_.rx_ibs_state = state;
}

void IbsHandler::ProcessIbsCmd(uint8_t *data)
{
  uint8_t ibs_data;
  int interval = 0;
  char dst_buff[MAX_BUFF_SIZE];
  ALOGV("%s", __func__);

  struct timeval tv;
  gettimeofday(&tv, NULL);

#ifdef DUMP_RINGBUF_LOG
  bt_logger_->SaveIbsToRingBuf(HCI_PACKET_TYPE_IBS_EVENT, *data);
#endif

  switch (*data) {
    case IBS_WAKE_IND: //0xFD
      ALOGI("%s: Received IBS_WAKE_IND: 0xFD", __func__);

      state_info_->AddLogTag(dst_buff, tv, (char *)"Received FD from SOC");
      state_info_->SetLastWakeIndFromSoc(dst_buff);
      health_info->IncreaseWakeRcv();
      if (time_last_rx_sleep != 0) {
        interval = tv.tv_sec - time_last_rx_sleep;
        health_info->UpdateIntervalRxSleepWake(interval);
      }

      StopVoteOffTimer();
      switch (state_machine_.rx_ibs_state) {
        case IBS_RX_ASLEEP:
          ALOGV("%s: UART RX Vote ON", __func__);
          SerialClockVote(IBS_RX_VOTE_CLOCK_ON);
          UpdateRxState(IBS_RX_AWAKE);
          ibs_data = IBS_WAKE_ACK;
          ALOGI("%s: Writing IBS_WAKE_ACK", __func__);

          if (tty_fd_ > 0) {
            ALOGV("%s: Lock to write", __func__);
            WriteIbsCmd(ibs_data);
            state_info_->AddLogTag(dst_buff, tv, (char *)"Writing FC to SOC");
            state_info_->SetLastWakeAckFromHost(dst_buff);
            ibs_spurious_wake_timer_start();
          } else {
            ALOGE("%s: Invalid FD", __func__);
            ALOGV("%s: Realease SM lock", __func__);
            return;
          }
          break;
        case IBS_RX_AWAKE:
          ibs_data = IBS_WAKE_ACK;
          ALOGI("%s: Writing IBS_WAKE_ACK", __func__);
          if (tty_fd_ > 0 ) {
            WriteIbsCmd(ibs_data);
            state_info_->AddLogTag(dst_buff, tv, (char *)"Writing FC to SOC");
            state_info_->SetLastWakeAckFromHost(dst_buff);
          }
          else {
            ALOGE("%s: Invalid FD", __func__);
            return;
          }
          break;
      }
      break;
    case IBS_SLEEP_IND: //0xFE
      ALOGI("%s: Received IBS_SLEEP_IND: 0xFE", __func__);
      state_info_->AddLogTag(dst_buff, tv, (char *)"Received FE from SOC");
      state_info_->SetLastSleepIndFromSoc(dst_buff);
      health_info->IncreaseSleepRcv();
      time_last_rx_sleep = tv.tv_sec;
      ibs_spurious_wake_timer_stop();

      switch (state_machine_.rx_ibs_state) {
        case IBS_RX_AWAKE:
          ALOGV("%s: RX path is awake, Vote Off uart", __func__);
          UpdateRxState(IBS_RX_ASLEEP);
          ALOGV("%s: UART RX Vote Off", __func__);
          SerialClockVote(IBS_RX_VOTE_CLOCK_OFF);
          break;
        case IBS_RX_ASLEEP:
          ALOGV("%s: RX path is asleep", __func__);
          break;
      }
      {
        std::unique_lock<std::mutex> guard(ibs_rx_state_);
        sleep_ind_recvd_ = true;
        sleep_ind_wait_cv.notify_all();
      }
      break;
    case IBS_WAKE_ACK: ////0xFC
      state_info_->AddLogTag(dst_buff, tv, (char *)"Received FC from SOC");
      state_info_->SetLastWakeAckFromSoc(dst_buff);
      switch (state_machine_.tx_ibs_state) {
        case IBS_TX_WAKING:
          ALOGI("%s: Received IBS_WAKE_ACK: 0xFC", __func__);
          pthread_mutex_lock(&state_machine_.wack_lock);
          ALOGI("%s: Signal wack_cond_", __func__);
          wack_recvd_ = true;
          StopWackTimer();
          pthread_cond_signal(&wack_cond_);
          is_tx_signaled_ = true;
          pthread_mutex_unlock(&state_machine_.wack_lock);
          break;
        case IBS_TX_AWAKE:
          ALOGD("%s: TX SM is awake already, stop wack timer if running", __func__);
          StopWackTimer();
          break;
        default:
          ALOGE("%s: WAKE ACK from SOC, Unexpected TX state", __func__);

          if (pthread_mutex_trylock(&state_machine_.hci_tx_ibs_lock) == 0) {
            if (IBS_RX_AWAKE == state_machine_.rx_ibs_state) {
              if (bt_logger_->isSsrTriggered()) {
                /* Stop timers if active */
                ibs_spurious_wake_timer_stop();
                ALOGW("%s: Ignoring unexpected wake ACK in SSR Phase", __func__);
                pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
                break;
              }

              ALOGE("%s: Rx WAKE ACK from SOC,Tx state is ASLEEP and Rx state is AWAKE", __func__);
            }

            pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
          }
          break;
      }
      break;
  }
}
void IbsHandler::DeviceSleep(void)
{
  uint8_t ibs_data;
  char dst_buff[MAX_BUFF_SIZE];
  struct timeval tv;

  ALOGV("%s: Acquire SM lock", __func__);
  pthread_mutex_lock(&state_machine_.hci_tx_ibs_lock);
  switch (state_machine_.tx_ibs_state) {
    case IBS_TX_AWAKE:
      if (bt_lpm_.wake_state == WCNSS_IBS_WAKE_ASSERTED) {
        ALOGD("%s: Tx in progress", __func__);
        pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
        return;
      }
      ALOGI("%s: TX Awake, Sending SLEEP_IND", __func__);
      ibs_data = IBS_SLEEP_IND;
      if (tty_fd_ > 0) {
        gettimeofday(&tv, NULL);
        state_info_->AddLogTag(dst_buff, tv, (char *)"Writing FE to SoC");
        state_info_->SetLastSleepIndFromHost(dst_buff);
        WriteIbsCmd(ibs_data);
        health_info->IncreaseSleepSent();
      }
      else {
        ALOGE("%s: Invalid FD", __func__);
        pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
        return;
      }
      state_machine_.tx_ibs_state    = IBS_TX_ASLEEP;
      ALOGV("%s: UART TX Vote Off", __func__);
      SerialClockVote(IBS_TX_VOTE_CLOCK_OFF);
      break;
    case IBS_TX_ASLEEP:
      ALOGV("%s: TX Asleep", __func__);
      break;
    default:
      ALOGE("%s: Invalid TX SM", __func__);
      break;
  }
  pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
  return;
}

void IbsHandler::IdleTimeout(union sigval /* sig */)
{
  std::unique_lock<std::mutex> guard(ibs_mutex_);
  ALOGV("%s: Deassert SOC", __func__);
  if (!instance_) {
    ALOGD("%s: IBS handler has been destroyed ", __func__);
    return;
  }
  instance_->DeviceSleep();
}

void IbsHandler::StartIdleTimer(void)
{
  int status;
  struct itimerspec ts;
  struct sigevent se;

  ALOGV("%s", __func__);
  if (bt_lpm_.timer_created == false) {
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &bt_lpm_.timer_id;
    se.sigev_notify_function = (void (*)(union sigval))IdleTimeout;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &bt_lpm_.timer_id);

    if (status == 0)
      bt_lpm_.timer_created = true;
  }

  if (bt_lpm_.timer_created == true) {
    ts.it_value.tv_sec = bt_lpm_.timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000000 * (bt_lpm_.timeout_ms % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(bt_lpm_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s: Failed to set LPM idle timeout", __func__);
  }
}

void IbsHandler::StopIdleTimer(void)
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);
  if (bt_lpm_.timer_created == true) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(bt_lpm_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("[STOP] Failed to set LPM idle timeout");
 }
}

void IbsHandler::TransmitDone(uint8_t tx_done)
{
  ALOGV("%s", __func__);
  bt_lpm_.no_tx_data = tx_done;
  if (tx_done == true) {
    ALOGV("%s: tx_done. Start idle timeout", __func__);
    bt_lpm_.wake_state = WCNSS_IBS_WAKE_W4_TIMEOUT;
    StartIdleTimer();
  }
}
void IbsHandler::VoteOnClock()
{
  uart_transport_->ClockOperation(USERIAL_OP_CLK_ON);
}
void IbsHandler::VoteOffClock()
{
  StopIdleTimer();
  uart_transport_->ClockOperation(USERIAL_OP_CLK_OFF);
}

int IbsHandler::WakeAssert(void)
{
  int ret = 0;

  ALOGV("%s", __func__);
  StopIdleTimer();
  bt_lpm_.wake_state = WCNSS_IBS_WAKE_ASSERTED;
  ret = DeviceWakeUp();

  if (ret != -1) {
    TransmitDone(false);
  }
  return ret;
}

bool IbsHandler::waitUntilSleepIndRecvd(void)
{
  ALOGD("%s, begin", __func__);
  std::unique_lock<std::mutex> guard(ibs_rx_state_);
  if (state_machine_.rx_ibs_state == IBS_RX_ASLEEP) {
    ALOGD("%s, rx is already asleep", __func__);
    return true;
  }

  sleep_ind_recvd_ = false;

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ALOGI("%s: system time before waiting for SLEEP IND tv_sec:%ld tv_nsec :%ld", __func__,
        ts.tv_sec, ts.tv_nsec);
  sleep_ind_wait_cv.wait_for(guard, std::chrono::milliseconds(SLEEP_IND_WAIT_MS), [this]
                         {return sleep_ind_recvd_;});

  clock_gettime(CLOCK_REALTIME, &ts);
  ALOGI("%s: current system time tv_sec:%ld tv_nsec :%ld sleep_ind_recvd_:%d", __func__,
         ts.tv_sec, ts.tv_nsec, sleep_ind_recvd_);

  return sleep_ind_recvd_;
}

void IbsHandler::SetRxDwakeupEnabled() {
  IbsHandler::rxd_wakeup_enabled = true;
}

void IbsHandler::SocRxDWakeup() {
  uart_transport_->SocRxDWakeup();

#ifdef DUMP_RINGBUF_LOG
  bt_logger_->SaveIbsToRingBuf(HCI_PACKET_TYPE_IBS_CMD, 0x00);
#endif
}

#endif

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
