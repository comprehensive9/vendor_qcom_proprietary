/******************************************************************************
 *  Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *  Not a Contribution.
 *
 *  Copyright (C) 2017 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include "android/hardware/bluetooth/1.0/IBluetoothHci.h"
#include <android/hardware/bluetooth/1.0/IBluetoothHciCallbacks.h>
#include <android/hardware/bluetooth/1.0/types.h>
#include <hwbinder/ProcessState.h>

#ifdef QCOM_ANT_SUPPORTED
#include <com/dsi/ant/1.0/IAnt.h>
#include <com/dsi/ant/1.0/IAntCallbacks.h>
#include <com/dsi/ant/1.0/types.h>
#endif

#include <vendor/qti/hardware/fm/1.0/IFmHci.h>
#include <vendor/qti/hardware/fm/1.0/IFmHciCallbacks.h>
#include <vendor/qti/hardware/fm/1.0/types.h>

#include <sys/un.h>
#include <ctime>
#include <netinet/in.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <utils/Log.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "hidl_client.h"
#include "hci_internals.h"

using android::hardware::bluetooth::V1_0::IBluetoothHci;
using android::hardware::bluetooth::V1_0::IBluetoothHciCallbacks;
using android::hardware::bluetooth::V1_0::HciPacket;
using android::hardware::bluetooth::V1_0::Status;

#ifdef QCOM_ANT_SUPPORTED
using com::dsi::ant::V1_0::IAnt;
using com::dsi::ant::V1_0::IAntCallbacks;
#endif

using vendor::qti::hardware::fm::V1_0::IFmHci;
using vendor::qti::hardware::fm::V1_0::IFmHciCallbacks;

using android::hardware::ProcessState;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_vec;

extern void initialization_complete();
extern void *process_tool_data(void *arg);
static int safe_write(int server_fd, unsigned char* buf, int write_len);
static void RTSCTSTimerExpiry(union sigval sig);
bool StopRTSCTSStatusTimer();
void StartGetRTSCTSStatusTimer(int sleepTime);
static bool is_vs_req_set_rts_evt(const unsigned char* buf, int len);
static bool is_internal_cts_status_evt(const unsigned char* buf, int len);
static pthread_t client_rthread;
extern int pinCtrlTimeout;
enum SleepTimerState {
  TIMER_NOT_CREATED = 0x00,
  TIMER_CREATED,
  TIMER_ACTIVE
};
#define BT_CMD_PACKET_TYPE 0x01
#define BT_ACL_PACKET_TYPE 0x02
#define BT_SCO_PACKET_TYPE 0x03
#define BT_EVT_PACKET_TYPE 0x04

#define ANT_CTRL_PACKET_TYPE    0x0c
#define ANT_DATA_PACKET_TYPE    0x0e

#define FM_CMD_PACKET_TYPE     0x11
#define FM_EVT_PACKET_TYPE     0x14

#define INVALID_FD (-1)
#define FRAME_INTERNAL_EVT_CTS_STATUS \
  { 0x0e, 0x06, 0x01, 0x0c, 0xfc, 0x00, 0x3a }
#define COMMAND_RESET_UART_FLOW_ON \
  { 0x00, 0xFD, 0x00 }
#define COMMAND_GET_CTS_STATUS \
  { 0x10, 0xFD, 0x00 }
#define VS_REQ_SET_RTS_EVT \
  { 0x0e, 0x05, 0x01, 0x0c, 0xfc, 0x00, 0x3a }
static sem_t s_cond;
static int mode_type;
static volatile bool hidl_init = false;
static volatile int ant_init_status = 1;
int server_fd = -1;
int client_fd = -1;
static int pinctrl_timer_state_ = TIMER_NOT_CREATED;
timer_t pinctrl_timer_;
extern bool isUartRtsTC;
extern bool isUartCtsTC;
android::sp<IBluetoothHci> btHci;
#ifdef QCOM_ANT_SUPPORTED
android::sp<IAnt> antHci;
#endif
android::sp<IFmHci> fmHci;

class BluetoothHciCallbacks : public IBluetoothHciCallbacks {

    public:
    Return<void> initializationComplete(Status status) {
        if (status == Status::SUCCESS) {
            hidl_init = true;
        } else {
            ALOGE("Error in HIDL initialization");
        }
        sem_post(&s_cond);
        return Void();
    }

    Return<void> hciEventReceived(const hidl_vec<uint8_t>& event) {
        int len = static_cast<int>(event.size());
        unsigned char val = BT_EVT_PACKET_TYPE;
        const uint8_t* pkt = event.data();

        if (safe_write(server_fd, &val, 1) == -1) {
            ALOGE("%s: failed to write event to tool socket", __func__);
            return Void();
        }

        if (safe_write(server_fd, const_cast<unsigned char*>(event.data()), len) == -1) {
            ALOGE("%s: failed to write event to tool socket", __func__);
            return Void();
        }

        if (is_vs_req_set_rts_evt(pkt, len)) {
            isUartCtsTC = true;
            btHci->sendHciCommand(COMMAND_GET_CTS_STATUS);
        } else if (is_internal_cts_status_evt(pkt, len)) {
            if (isUartCtsTC) {
                StartGetRTSCTSStatusTimer(pinCtrlTimeout);
            }
        }
        return Void();
    }

    Return<void> aclDataReceived(const hidl_vec<uint8_t>& data) {
        int len = static_cast<int>(data.size());
        unsigned char val = BT_ACL_PACKET_TYPE;
        if (safe_write(server_fd, &val, 1) == -1) {
            ALOGE("%s: failed to write event to tool socket", __func__);
            return Void();
        }

        if (safe_write(server_fd, const_cast<unsigned char*>(data.data()), len) == -1) {
            ALOGE("%s: failed to write event to tool socket", __func__);
            return Void();
        }
        return Void();
    }

    Return<void> scoDataReceived(const hidl_vec<uint8_t>& data) {
        int len = static_cast<int>(data.size());
        unsigned char val = BT_SCO_PACKET_TYPE;
        if (safe_write(server_fd, &val, 1) == -1) {
            ALOGE("%s: failed to write event to tool socket", __func__);
            return Void();
        }

        if (safe_write(server_fd, const_cast<unsigned char*>(data.data()), len) == -1) {
            ALOGE("%s: failed to write event to tool socket", __func__);
            return Void();
        }
        return Void();
    }
};

namespace
{
#ifdef QCOM_ANT_SUPPORTED
using namespace com::dsi::ant::V1_0;

class AntHciCallbacks : public IAntCallbacks {
    public:
    AntHciCallbacks() {};
    virtual ~AntHciCallbacks() = default;

    Return<void> onTransportDown(const android::hardware::hidl_string & cause) {
        ALOGI("%s: start ", __func__);
        return Void();
    }

    Return<void> onMessageReceived(const hidl_vec<uint8_t>& event)
    {
        int len = static_cast<int>(event.size());
        unsigned char val = ANT_CTRL_PACKET_TYPE;
        std::vector<uint8_t> newcommand = std::vector<uint8_t> (event.begin(), event.end());
        std::vector<uint8_t>::iterator it;
        it = newcommand.begin();
        newcommand.insert(it , len);
        if (safe_write(server_fd, &val, 1) == -1) {
            ALOGE("%s: failed to write event to tool socket", __func__);
            return Void();
        }

        if (safe_write(server_fd, const_cast<unsigned char*>(newcommand.data()),
            newcommand.size()) == -1) {
            ALOGE("%s: failed to write event to tool socket", __func__);
            return Void();
        }
        return Void();
    }
};
#endif
namespace
{
 using vendor::qti::hardware::fm::V1_0::HciPacket;
 using vendor::qti::hardware::fm::V1_0::Status;
 class FmHciCallbacks : public IFmHciCallbacks {
    public:
        FmHciCallbacks() {
        };
        virtual ~FmHciCallbacks() = default;

       Return<void> initializationComplete(Status status) {
           ALOGV("%s: start ", __func__);
           if (status == Status::SUCCESS) {
              hidl_init = true;
           } else {
               ALOGE("Error in HIDL initialization");
           }
           sem_post(&s_cond);
           ALOGI("%s:end", __func__);
           return Void();
        }

        Return<void>  hciEventReceived(const hidl_vec<uint8_t>& event) {
            int len = static_cast<int>(event.size());
            ALOGV("%s: start ", __func__);
            unsigned char val = FM_EVT_PACKET_TYPE;
           if (safe_write(server_fd, &val, 1) == -1) {
              return Void();
           }
           if (safe_write(server_fd, const_cast<unsigned char*>(event.data()), len) == -1) {
               ALOGE("%s: failed to write event to tool socket", __func__);
               return Void();
           }
           return Void();
        }
};
}
}


#ifdef __cplusplus
extern "C"
{
#endif

bool hidl_client_initialize(int mode, int *tool_fd) {

    mode_type = mode;

    if (tool_fd == NULL) {
        ALOGE("%s: Invalid tool FD", __func__);
        return false;
    }
    switch (mode) {
        case MODE_BT:
            ALOGI("%s: Initialize the HIDL with Mode BT", __func__);
            btHci = IBluetoothHci::getService();
            // If android.hardware.bluetooth* is not found, Bluetooth can not continue.
            if (btHci != nullptr) {
                 ALOGI("%s: IBluetoothHci::getService() returned %p (%s)",
                    __func__, btHci.get(), (btHci->isRemote() ? "remote" : "local"));
            } else {
                ALOGE("Unable to initialize the HIDL");
                return false;
            }
            {
                android::sp<IBluetoothHciCallbacks> callbacks = new BluetoothHciCallbacks();
                hidl_init = false;
                isUartCtsTC = false;
                sem_init(&s_cond, 0, 0);
                btHci->initialize(callbacks);
                // waiting for initialisation callback
                sem_wait(&s_cond);
            }

            if (hidl_init == true) {
                break;
            } else {
                ALOGE("%s: HIDL failed to initialize, sending invalid FD to tool", __func__);
                return false;
            }

        case MODE_ANT:
#ifdef QCOM_ANT_SUPPORTED
            ALOGI("%s: Initialize the HIDL with Mode ANT", __func__);
            antHci = IAnt::getService();
            if (antHci != nullptr) {
                ALOGI("%s: IAnt::getService() returned %p (%s)",
                   __func__, antHci.get(), (antHci->isRemote() ? "remote" : "local"));
            } else {
                ALOGE("Unable to initialize the HIDL");
                return false;
            }
            {
              android::sp<IAntCallbacks> callbacks = new AntHciCallbacks();
              antHci->setCallbacks(callbacks);
              ant_init_status = antHci->enable();
            }
            if (ant_init_status == 0) {
                break;
            }
#else
            ALOGE("%s: Can't support HIDL with Mode ANT", __func__);
            return false;
#endif
            break;
        case MODE_FM:
            ALOGI("%s: Initialize the HIDL with Mode FM", __func__);
            fmHci = IFmHci::getService();
            if (fmHci != nullptr) {
                ALOGI("%s: IFmHci::getService() returned %p (%s)",
                   __func__, fmHci.get(), (fmHci->isRemote() ? "remote" : "local"));
            } else {
                ALOGE("Unable to initialize the HIDL");
                return false;
            }
            {
             android::sp<IFmHciCallbacks> callbacks = new FmHciCallbacks();
              hidl_init = false;
              sem_init(&s_cond, 0, 0);
              fmHci->initialize(callbacks);
              // waiting for initialisation callback
              ALOGV("%s: Wait for initialisation callback", __func__);
              sem_wait(&s_cond);
            }
            if (hidl_init == true) {
               if (client_fd > 0) {
                   *tool_fd = client_fd;
                   return true;
               }
               break;
            } else {
                ALOGE("%s: HIDL failed to initialize, sending invalid FD to tool", __func__);
                return false;
            }
            break;
        default:
            ALOGE("Unsupported mode");
            return false;
    }

    // creating a scoket pair
    int fds[2] = {INVALID_FD, INVALID_FD};

    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, fds) == -1) {
        ALOGE("%s error creating socketpair: %s", __func__,strerror(errno));
        return false;
    }
    server_fd = fds[0];
    if (pthread_create(&client_rthread, NULL, process_tool_data, NULL) != 0) {
        ALOGE("%s:Unable to create pthread err = %s", __func__,  strerror(errno));
        close(server_fd);
        close(fds[1]);
        return false;
    }
    client_fd = fds[1];
    ALOGE("Server FD=%d  Client FD: %d\n",server_fd,client_fd);
    *tool_fd = client_fd;
    return true;
}

// Close HIDL Client to prevent callbacks.
void hidl_client_close() {

    if ((hidl_init == false) && (ant_init_status == 1)) {
        return;
    }

    if (server_fd != -1) {
        close(server_fd);
    }

    pthread_join(client_rthread, NULL);
    server_fd = -1;

    switch (mode_type) {
        case MODE_BT:
            ALOGI("%s: Close HIDL with Mode BT", __func__);
            btHci->close();
            btHci = nullptr;
            break;

#ifdef QCOM_ANT_SUPPORTED
        case MODE_ANT:
            ALOGI("%s: Close HIDL with Mode ANT", __func__);
            antHci->disable();
            antHci = nullptr;
            break;
#endif

        case MODE_FM:
            ALOGI("%s: Close HIDL with Mode FM", __func__);
            fmHci->close();
            fmHci = nullptr;
            break;

        default:
            ALOGE("Unsupported mode");
            break;
    }
    return;
}

#ifdef __cplusplus
}
#endif

static int safe_write(int server_fd, unsigned char* buf, int write_len) {

    int bytes_written = 0;
    int ret;

    if (buf == NULL) {
        ALOGE("%s: The buffer is NULL", __func__);
        return -1;
    }

    if (!write_len) {
        ALOGE("%s: No data to write", __func__);
        return 0;
    }

    while (write_len > 0) {
        CLIENT_NO_INTR(ret = write(server_fd, buf+bytes_written, write_len));
        if (ret < 0) {
            ALOGE("%s: Write error: (%s)", __func__, strerror(errno));
            return -1;
        } else if (ret == 0) {
            ALOGE("%s: Write returned 0, err = %s, bytes written: %d",
                              __func__, strerror(errno), (unsigned int)(bytes_written));
            return -1;
        } else {
            write_len -= ret;
            bytes_written += ret;
        }
    }
    return bytes_written;
}

static void RTSCTSTimerExpiry(union sigval sig) {
    ALOGI("%s", __func__);
    if (isUartRtsTC) {
        hidl_vec<uint8_t> cmd = COMMAND_RESET_UART_FLOW_ON;
        btHci->sendHciCommand(cmd);
        StopRTSCTSStatusTimer();
        isUartRtsTC = false;
    } else if (isUartCtsTC) {
        hidl_vec<uint8_t> cmd = COMMAND_GET_CTS_STATUS;
        btHci->sendHciCommand(cmd);
        StopRTSCTSStatusTimer();
        isUartCtsTC = false;
    }
}

void StartGetRTSCTSStatusTimer(int sleepTime) {
    int status;
    struct itimerspec ts;
    struct sigevent se;
    uint32_t timeout_ms;

    ALOGI("%s", __func__);

    if (pinctrl_timer_state_ == TIMER_NOT_CREATED) {
        se.sigev_notify_function = (void (*)(union sigval))RTSCTSTimerExpiry;
        se.sigev_notify = SIGEV_THREAD;
        se.sigev_value.sival_ptr = NULL;
        se.sigev_notify_attributes = NULL;

        status = timer_create(CLOCK_BOOTTIME_ALARM, &se, &pinctrl_timer_);
        if (status == 0) {
            ALOGI("%s: created", __func__);
            pinctrl_timer_state_ = TIMER_CREATED;
        } else {
            ALOGE("%s: Failed to create pinctrl test timer %d\n", __func__, status);
        }
    }

    if (pinctrl_timer_state_ == TIMER_CREATED || pinctrl_timer_state_ == TIMER_ACTIVE) {
        timeout_ms = sleepTime;
        ts.it_value.tv_sec = timeout_ms / 1000;
        ts.it_value.tv_nsec = 1000000 * (timeout_ms % 1000);
        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;

        status = timer_settime(pinctrl_timer_, 0, &ts, 0);
        if (status < 0) {
            ALOGE("Failed to set %s with status= %d", __func__, status);
        } else {
            ALOGI("%s: timer started", __func__);
            pinctrl_timer_state_ = TIMER_ACTIVE;
        }
    }
  return;
}

bool StopRTSCTSStatusTimer() {
    int status;
    struct itimerspec ts;
    bool was_active = false;

    ALOGI("%s", __func__);

    if (pinctrl_timer_state_ == TIMER_ACTIVE) {
        ts.it_value.tv_sec = 0;
        ts.it_value.tv_nsec = 0;
        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;
        was_active = true;
        status = timer_settime(pinctrl_timer_, 0, &ts, 0);
        if (status == -1) {
            ALOGE("%s:Failed to stop pinctrl timer", __func__);
        } else if (status == 0) {
            ALOGV("%s: Pinctrl timer Stopped", __func__);
            pinctrl_timer_state_ = TIMER_CREATED;
        }
    }
    return was_active;
}

bool is_vs_req_set_rts_evt(const unsigned char* buf, int len) {
    bool result = false;
    const unsigned char vs_req_set_rts_evt[] = VS_REQ_SET_RTS_EVT;

    ALOGV("%s: length = %d ", __func__, len);
    if (memcmp(vs_req_set_rts_evt, buf, ARRAY_SIZE(vs_req_set_rts_evt)) == 0) {
        result= true;
    }
    return result;
}

bool is_internal_cts_status_evt(const unsigned char* buf, int len) {
    bool result = false;
    const unsigned char frame_internal_evt[] = FRAME_INTERNAL_EVT_CTS_STATUS;

    ALOGV("%s: length = %d ", __func__, len);
    if (memcmp(frame_internal_evt, buf, ARRAY_SIZE(frame_internal_evt)) == 0) {
        result= true;
    }
    return result;
}
