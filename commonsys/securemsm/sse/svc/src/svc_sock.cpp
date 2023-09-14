/**
 * Copyright (c) 2014-2015, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "SUI_svcsock"
#define LOGV(...) \
    do {          \
    } while (0)  // Comment this line to enable LOGV
#define LOGD(...) \
    do {          \
    } while (0)  // Comment this line to enable LOGD

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <utils/Log.h>

#include <hidl/HidlTransportSupport.h>
#include <hidl/LegacySupport.h>

#include "common_log.h"
#include "svc_msg.h"
#include "svc_sock.h"

#include <vendor/qti/hardware/tui_comm/1.0/ITuiComm.h>
#include <vendor/qti/hardware/tui_comm/1.0/ITuiCommCallBack.h>

/**---------------------------------------------------------------------------
 * Constant and Macro Declarations
 * -------------------------------------------------------------------------*/

#ifndef max
#define max(A, B) ((A) > (B)) ? (A) : (B)
#endif

#define ENTER LOGD("%s(%s)+", __func__, g_lname);
#define EXIT LOGD("%s(%s)-", __func__, g_lname);
#define EXITV(x)                                \
    LOGD("%s(%s)- %d", __func__, g_lname, (x)); \
    return (x)

/** Length of the internal array used in the async use case */
#define MESSAGE_ARRAY_LENGTH 8

/** Maximum number of bind attempts.
 *  This addresses the case when a bind happens immediately after a close */
#define BIND_MAX_ATTEMPTS 1024

#define USLEEP_TIME (20000)

#define MAX_SHCED_COUNTER (10000)

#define COMMAND_REPLY_TIMEOUT (120)  // 2 minutes

#define USE_CASE_STARTED (1)
#define USE_CASE_STOPPED (0)

extern "C" {
using android::sp;
using android::hardware::hidl_string;
using android::hardware::Return;
using android::hardware::Void;
using vendor::qti::hardware::tui_comm::V1_0::ITuiComm;
using vendor::qti::hardware::tui_comm::V1_0::ITuiCommCallBack;
using android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
/**---------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** Received message as stored in the internal message queue */
typedef struct {
    uint8_t data[BYTES_IN_MEX];
    secdisp_ip input;
    char src_name[UNIX_PATH_MAX];
    size_t src_len;
} svc_sock_message_t;

/**---------------------------------------------------------------------------
 * Global Variables
 * -------------------------------------------------------------------------*/

/** File descriptor associated with the server socket */
static volatile uint8_t fd = 0xff;
/** Callback associated with incoming commands */
static svc_sock_command_callback_t command_cback = NULL;
/** Callback associated with incoming notifications */
static svc_sock_notification_callback_t notification_cback = NULL;
/** Thread running the receiving loop */
static pthread_t rcv_thread = NULL;
/** Mutex to avoid concurrent initializations and terminations */
static pthread_mutex_t control_mtx = PTHREAD_MUTEX_INITIALIZER;
/** Mutex protecting the send_command function (only 1 command at a time) */
static pthread_mutex_t command_mtx = PTHREAD_MUTEX_INITIALIZER;
/** Mutex for protecting the condition variable use_case_started */
static pthread_mutex_t wait_mtx = PTHREAD_MUTEX_INITIALIZER;
/** pthread conditional variable regrarding  use_case_started */
static pthread_cond_t cond_wait = PTHREAD_COND_INITIALIZER;
/** Semaphores indicating new messages are available in the internal queue */
static sem_t g_msg_available;
/** Semaphore protecting access to the internal message queue */
static pthread_mutex_t mex_mtx = PTHREAD_MUTEX_INITIALIZER;
/** Name for the local endpoint of the socket */
static char g_lname[UNIX_PATH_MAX] = {0};  // excluding the initial \0

static char g_rname[UNIX_PATH_MAX] = {0};  // excluding the initial \0
/** Address of the remote endpoint of the socket */
static struct sockaddr_un addr_rmt = {0};  // remote
/** Its length */
static socklen_t addr_rmt_len = 0;
/** Whether incoming messages should be filtered, and only the ones coming from
 * the specified remote endpoint accepted. */
static uint32_t filter = 0;
/** Internal message queue, implemented as a static array */
static struct {
    svc_sock_message_t messages[MESSAGE_ARRAY_LENGTH];
    size_t head;
    size_t tail;
} g_mex_queue;  // initialized to 0 since static

sp<ITuiCommCallBack> myCallback = NULL;

static int msg_count = 0;
/* To indicate TUI APK if the use case has started */
int use_case_started = USE_CASE_STOPPED;

sp<ITuiComm> client = NULL;
bool tui_restarted = false;

/* Interface implementation of the Callback mechanism */
class TuiCommCallBack : public ITuiCommCallBack
{
   public:
    /* callback which gives the information on the status of TUI use case */
    Return<void> sendevent(uint8_t event_id)
    {
        pthread_mutex_lock(&wait_mtx);
        msg_count++;
        if (msg_count == 1) pthread_cond_signal(&cond_wait);
        pthread_mutex_unlock(&wait_mtx);
        return Void();
    }
};

struct TuiCommNativeDeathRecipient : public hidl_death_recipient {
    virtual void serviceDied(uint64_t cookie __unused,
                             const android::wp<IBase> &who __unused)
    {
        ENTER;
        /* Unblock suisvc from waiting for message and
           reinit with new instace of tui_comm*/
        client = NULL;
        if (!strcmp(g_lname, "suisvc")) {
            ALOGD("%s: tui_comm killed.. reinit %s", __func__, g_lname);
            tui_restarted = true;
            sem_post(&g_msg_available);
        } else if (!strcmp(g_lname, "suitch")) {
            /* Force abort touch as tui_comm is killed.
               No notifications can be received*/
            if (notification_cback) {
                ALOGD("%s: tui_comm killed.. notify abort %s", __func__,
                      g_lname);
                notification_cback(SUI_MSG_ID_ABORT, 0, NULL, 0);
            }
        }
        /* reset File descriptor associated with the socket
           to terminate listener */
        fd = 0xff;
        /* Stop all the rcv loops */
        msg_count++;
        pthread_cond_signal(&cond_wait);
        EXIT;
    }
};
sp<TuiCommNativeDeathRecipient> death_recipient_ = nullptr;

/**---------------------------------------------------------------------------
 * Function implementations
 * -------------------------------------------------------------------------*/

/** Process an incoming message.
 * Called from the rcv loop, it invokes the appropriate callback, and it might
 * send a response back. */
static int process_message(uint8_t const *const buffer,
                           secdisp_ip secdisp_input, size_t const size,
                           char *src)
{
    int nRet = 0;
    size_t src_len = 0;
    ENTER;

    do {
        if (src != NULL) {
            src_len = strnlen(src, UNIX_PATH_MAX);
            if (src_len == UNIX_PATH_MAX) {
                LOGD("Discarding message from src of invalid src_len: %d",
                     src_len);
                break;
            }
        } else {
            LOGD("Discarding message from invalid src, src is NULL");
            break;
        }

        if ((buffer == NULL) || (size != BYTES_IN_MEX)) {
            LOGD("Discarding. buffer = %p, size = %u", buffer, size);
            break;
        }

        if (buffer[0] == SUI_MSG_NOT_MSK) {
            if (notification_cback) {
                notification_cback(buffer[1], buffer[2], src, src_len);
            }
        } else if (buffer[0] == SUI_MSG_CMD_MSK) {
            if (command_cback) {
                nRet = command_cback(buffer[1], buffer[2], secdisp_input, src,
                                     src_len);
            }
        }
    } while (0);
    EXIT;
    return nRet;
}

bool get_service()
{
    LOGD("%s", __func__);
    // all references to remote are invalid
    client = nullptr;
    // try to find the service again
    for (int i = 0; i < 10; ++i) {
        client = ITuiComm::tryGetService();
        LOGD("%s: iteration: %d", __func__, i);
        if (client != nullptr) {
            auto rsp = client->ping();  // double-check whether remote is alive
            if (rsp.isOk()) {
                return true;
            }
        }
        /* NOTE : Whenever we do socket initialization , we do getService()
           for tui_comm and we wait for 10 micro second and then again try
           for sock init so we don't want to sleep for long time here*/
        usleep(10000);
    }
    return false;
}

/** Receiving loop */
static void *rcv(void *)
{
    int rv = 0;
    uint8_t buf[BYTES_IN_MEX] = {0};
    struct ITuiComm::sock_req input;
    struct ITuiComm::sock_msg output;
    secdisp_ip secdisp_input;
    uint8_t is_svc = 0;
    if (!strcmp(g_lname, "suisvc")) {
        is_svc = 1;
    }

    ENTER;
    do {
        while (1) {
            pthread_mutex_lock(&wait_mtx);
            while (msg_count == 0) pthread_cond_wait(&cond_wait, &wait_mtx);
            msg_count--;
            pthread_mutex_unlock(&wait_mtx);

            input.index = fd;
            input.name = g_lname;
            input.is_peep = 1;

            if (client == NULL) {
                if (get_service() == false) {
                    LOGE("%s(%s): getService failed. check tui_comm", __func__,
                         g_lname);
                    rv = -EFAULT;
                    break;
                }
            }

            client->recv_message(input, [&](ITuiComm::sock_msg data) {
                output.info_0 = data.info_0;
                output.response = data.response;
            });

            if (output.response == 0xff) {
                LOGE("%s(%s): Error reading from socket: %s", __func__, g_lname,
                     strerror(errno));
                rv = -errno;
                break;
            } else if (output.response == 0xf) {
                sched_yield();
                continue;
            }

            if (output.info_0 == SUI_MSG_RSP_MSK) {
                sched_yield();
                continue;
            }
            // we remove it from the socket internal buffer
            // POSIX here guarantees we get the same data, blessed POSIX!

            input.name = g_lname;
            input.is_peep = 0;

            client->recv_message(input, [&](ITuiComm::sock_msg data) {
                buf[0] = data.info_0;
                buf[1] = data.info_1;
                buf[2] = data.info_2;
                buf[3] = data.info_3;

                secdisp_input.bDisableOverlay = data.input.bDisableOverlay;
                secdisp_input.bToRender = data.input.bToRender;
                secdisp_input.colorFormat = data.input.colorFormat;
                secdisp_input.u32Height = data.input.u32Height;
                secdisp_input.u32Width = data.input.u32Width;
                secdisp_input.u32HPos = data.input.u32HPos;
                secdisp_input.u32VPos = data.input.u32VPos;
                secdisp_input.bufHandle = data.input.bufHandle;

                output.sender_name = data.sender_name;
                output.response = data.response;
            });
            LOGD("%s(%s): processing recv reading from socket: %s", __func__,
                 g_lname, strerror(errno));
            if (process_message(buf, secdisp_input, sizeof(buf),
                                (char *)output.sender_name.c_str())) {
                LOGD("%s(%s): registered callback reported an error", __func__,
                     g_lname);
            }
        }

    } while (0);
    EXIT;
    rcv_thread = NULL;
    return NULL;
}

/** Initialize the local endpoint */
int svc_sock_init(char const *const localname, size_t const localnamelen,
                  char const *const remotename, size_t const remotenamelen,
                  uint32_t /*_filter*/)
{
    int rv = 0;
    size_t addrInUseCounter = 0;
    ENTER;

    struct ITuiComm::sock_name input = {0};
    struct ITuiComm::sock_result output = {0};

    if (!localname || !remotename) {
        LOGE("%s(%s): NULL socket name", __func__, g_lname);
        return -1;
    }

    if (get_service() == false) {
        LOGE("%s(%s): getService failed. check tui_comm", __func__, localname);
        return -EFAULT;
    }

    if (myCallback == NULL) {
        LOGD("%s: callback register", __func__);
        myCallback = new TuiCommCallBack();
        client->register_callback(myCallback, [&](ITuiComm::sock_result data) {
            output.response = data.response;
        });
        if (output.response) {
            myCallback = NULL;
            return -1;
        }
    }

    if (death_recipient_ == NULL) {
        death_recipient_ = new TuiCommNativeDeathRecipient();
    }
    if (death_recipient_ == NULL) {
        // TODO Error handling
        LOGE("%s: Failed to register death_recipient", __func__);
    }

    Return<bool> linked = client->linkToDeath(death_recipient_, 0);
    if (!linked.isOk()) {
        LOGE("%s: Transaction error in linking to tui_comm", __func__);
    } else if (!linked) {
        LOGE("%s: unable to link to tui_comm death notification", __func__);
    } else {
        LOGD("%s: Successfully linked to tui_comm death notification",
             __func__);
    }

    pthread_mutex_lock(&control_mtx);
    do {
        if (fd != 0xff) {
            LOGE("%s(%s): already initialized", __func__, g_lname);
            break;
        }
        if (localnamelen > UNIX_PATH_MAX) {
            LOGE("%s(%s): local name too long: %zu vs %u", __func__, g_lname,
                 localnamelen, UNIX_PATH_MAX);
            rv = -EINVAL;
            break;
        }
        if (remotenamelen > UNIX_PATH_MAX) {
            LOGE("%s(%s): remote name too long: %zu vs %u", __func__, g_lname,
                 remotenamelen, UNIX_PATH_MAX);
            rv = -EINVAL;
            break;
        }
        /** a socket name of purely "0" would be invalid */
        if ((localnamelen <= 1) || (remotenamelen <= 1)) {
            LOGE("%s(%s): socket name too short: %zu %u", __func__, g_lname,
                 localnamelen, remotenamelen);
            rv = -EINVAL;
            break;
        }
        memcpy(g_lname, localname, localnamelen);
        memcpy(g_rname, remotename, remotenamelen);

        do {
            input.name = localname;
            client->sock_init(input, [&](ITuiComm::sock_result data) {
                output.response = data.response;
            });
            if (output.response == 0xff) {
                LOGD("%s(%s): Address in use, yield, #%u", __func__, g_lname,
                     addrInUseCounter);
                pthread_mutex_unlock(&control_mtx);
                return -1;
            } else
                break;

        } while (addrInUseCounter++ < BIND_MAX_ATTEMPTS);
        if (rv) {
            LOGE("%s(%s): Error binding socket: %s", __func__, g_lname,
                 strerror(errno));
            rv = -errno;
            break;
        }

        fd = output.response;
        if (rcv_thread == NULL) {
            rv = pthread_create(&rcv_thread, NULL, rcv, NULL);
            if (rv) {
                LOGE("%s(%s): Failed to start receiving thread", __func__,
                     g_lname);
                break;
            }
        } else
            LOGE("%s(%s): rcv thread already exists", __func__, g_lname);
    } while (0);
    if (rv != 0) {
        // join the thread in case of error
        if (0 == pthread_equal(rcv_thread, pthread_self())) {
            pthread_join(rcv_thread, NULL);
        }
    }
    pthread_mutex_unlock(&control_mtx);
    EXITV(rv);
}

/* get the current status of sockets*/
bool svc_sock_get_status()
{
    if (fd == 0xff)  // socket has been properly terminated
        return false;
    return true;
}

/** Close the local socket and the receiving thread */
int svc_sock_terminate(char *lname)
{
    struct ITuiComm::sock_req input;
    struct ITuiComm::sock_result output;
    int rv = 0;

    ENTER;

    myCallback = NULL;
    pthread_mutex_lock(&control_mtx);
    do {
        if (client == NULL) {
            if (get_service() == false) {
                LOGE("%s(%s): getService failed. check tui_comm", __func__,
                     lname);
                rv = -EFAULT;
                break;
            }
        }
        input.index = 0;
        input.name = lname;
        client->sock_close(input, [&](ITuiComm::sock_result data) {
            output.response = data.response;
        });
        fd = 0xff;
        command_cback = NULL;
        notification_cback = NULL;
        memset(g_lname, 0, sizeof(g_lname));
        memset(g_rname, 0, sizeof(g_lname));
        if (death_recipient_ != NULL) {
            client->unlinkToDeath(death_recipient_);
            death_recipient_.clear();
        }
    } while (0);
    pthread_mutex_unlock(&control_mtx);
    EXITV(rv);
}

static ssize_t svc_sock_send_message_getdisp(const uint8_t type,
                                             const uint8_t id,
                                             const uint8_t payload,
                                             secdisp_op *secdisp_output,
                                             char const *const dest_name)
{
    ssize_t n = 0;
    ENTER;

    struct ITuiComm::sock_msg input;
    struct ITuiComm::sock_result output;
    do {
        // compose message
        input.info_0 = type;
        input.info_1 = id;
        input.info_2 = payload;
        input.info_3 = 0;
        input.sender_name = g_lname;
        input.recv_name = dest_name;

        input.output.status = secdisp_output->status;
        input.output.u16NumBuffers = secdisp_output->u16NumBuffers;
        input.output.u32Height = secdisp_output->u32Height;
        input.output.u32Width = secdisp_output->u32Width;
        input.output.bufHandle = secdisp_output->bufHandle;
        input.output.u32Stride = secdisp_output->u32Stride;
        input.output.u32PixelWidth = secdisp_output->u32PixelWidth;
        input.output.fd = secdisp_output->fd;

        if (client == NULL) {
            if (get_service() == false) {
                LOGE("%s(%s): getService failed. check tui_comm", __func__,
                     g_lname);
                n = -EFAULT;
                break;
            }
        }
        client->send_smessage(input, [&](ITuiComm::sock_result data) {
            output.response = data.response;
        });
        if (output.response == 0xff) {
            LOGE("%s(%s): invalid remote socket %s", __func__, g_lname,
                 dest_name);
            n = -1;
            break;
        }

        n = 0;
    } while (0);

    EXITV(n);
}

/** Send a message over the socket */
static ssize_t svc_sock_send_message(const uint8_t type, const uint8_t id,
                                     const uint8_t payload,
                                     secdisp_ip *secdisp_input,
                                     char const *const dest_name)
{
    ssize_t n = 0;
    ENTER;
    struct ITuiComm::sock_msg input;
    struct ITuiComm::sock_result output;

    do {
        // compose message
        input.info_0 = type;
        input.info_1 = id;
        input.info_2 = payload;
        input.info_3 = 0;
        input.sender_name = g_lname;
        input.recv_name = dest_name;
        if (secdisp_input) {
            input.input.bDisableOverlay = secdisp_input->bDisableOverlay;
            input.input.bToRender = secdisp_input->bToRender;
            input.input.colorFormat = secdisp_input->colorFormat;
            input.input.u32Height = secdisp_input->u32Height;
            input.input.u32Width = secdisp_input->u32Width;
            input.input.u32HPos = secdisp_input->u32HPos;
            input.input.u32VPos = secdisp_input->u32VPos;
            input.input.bufHandle = secdisp_input->bufHandle;
        }

        if (client == NULL) {
            if (get_service() == false) {
                LOGE("%s(%s): getService failed. check tui_comm", __func__,
                     g_lname);
                n = -EFAULT;
                break;
            }
        }
        client->send_message(input, [&](ITuiComm::sock_result data) {
            output.response = data.response;
        });
        if (output.response == 0xff) {
            LOGE("%s(%s): invalid remote socket %s", __func__, g_lname,
                 dest_name);
            n = -1;
            break;
        }

        n = 0;
    } while (0);

    EXITV(n);
}

/** Send a notification */
int svc_sock_send_notification(uint8_t const id, uint8_t const payload,
                               char const *const dest_name,
                               size_t const dest_len)
{
    ssize_t n = 0;
    ENTER;
    do {
        if ((dest_name != NULL) && (dest_len != 0)) {
            n = svc_sock_send_message(SUI_MSG_NOT_MSK, id, payload, NULL,
                                      (char *)dest_name);
        } else {
            n = svc_sock_send_message(SUI_MSG_NOT_MSK, id, payload, NULL,
                                      g_rname);
        }

    } while (0);
    EXITV(n);
}

/** Send the notification from a temporarily created socket */
int svc_sock_send_notification_noreg(uint8_t id, uint8_t payload,
                                     char const *const remotename,
                                     size_t const remotenamelen)
{
    ssize_t n = 0;
    ENTER;
    int ret = 0;
    const char temp_name[] = "suitmp";
    do {
        if ((!remotename) || (remotenamelen <= 1) ||
            (remotenamelen > UNIX_PATH_MAX)) {
            LOGE("%s(%s): invalid remote socket", __func__, g_lname);
            n = -EINVAL;
            break;
        }

        /* Create a temporary socket to notify listener socket */
        ret = svc_sock_init(temp_name, sizeof(temp_name) - 1, remotename + 1,
                            strlen(remotename + 1), 0);
        if (ret) {
            LOGE("%s: Failed to create socket %s", __func__, temp_name);
            return -1;
        }
        n = svc_sock_send_message(SUI_MSG_NOT_MSK, id, payload, NULL,
                                  remotename + 1);
        /* shutdown the temporary socket */
        svc_sock_terminate("suitmp");
    } while (0);
    EXITV(n);
}

/** Send a response for a received command */
int svc_sock_send_response_getdisp(uint8_t const id, uint8_t const payload,
                                   secdisp_op *output,
                                   char const *const dest_name,
                                   size_t const dest_len)
{
    ssize_t n = 0;
    ENTER;
    do {
        if ((dest_name != NULL) && (dest_len != 0)) {
            n = svc_sock_send_message_getdisp(SUI_MSG_RSP_MSK, id, payload,
                                              output, dest_name);
        } else {
            n = svc_sock_send_message_getdisp(SUI_MSG_RSP_MSK, id, payload,
                                              output, g_rname);
        }
    } while (0);
    EXITV(n);
}

/** Send a response for a received command */
int svc_sock_send_response(uint8_t const id, uint8_t const payload,
                           char const *const dest_name, size_t const dest_len)
{
    ssize_t n = 0;
    ENTER;
    do {
        if ((dest_name != NULL) && (dest_len != 0)) {
            n = svc_sock_send_message(SUI_MSG_RSP_MSK, id, payload, NULL,
                                      dest_name);
        } else {
            n = svc_sock_send_message(SUI_MSG_RSP_MSK, id, payload, NULL,
                                      g_rname);
        }
    } while (0);
    EXITV(n);
}

/** Send a command and returns a response */
int svc_sock_send_command(uint8_t const id, uint8_t const payload,
                          char const *const dest_name, size_t const dest_len,
                          secdisp_ip *secdisp_input, secdisp_op *secdisp_output,
                          uint8_t *response)
{
    int rv = 0;
    uint8_t mex[BYTES_IN_MEX] = {0};
    ssize_t n = 0;
    struct ITuiComm::sock_req input = {0};
    struct ITuiComm::sock_msg output = {0};
    struct timespec stop, start;
    ENTER;
    pthread_mutex_lock(&command_mtx);  // only 1 command at a time
    do {
        if ((dest_name != NULL) && (dest_len != 0)) {
            n = svc_sock_send_message(SUI_MSG_CMD_MSK, id, payload,
                                      secdisp_input, dest_name);
        } else {
            n = svc_sock_send_message(SUI_MSG_CMD_MSK, id, payload,
                                      secdisp_input, g_rname);
        }

        if (n != 0) {
            LOGE("%s(%s): Error sending command: %s", __func__, g_lname,
                 strerror(n));
            rv = n;
            break;
        }

        // read the response

        clock_gettime(CLOCK_REALTIME, &start);
        while (1) {
            clock_gettime(CLOCK_REALTIME, &stop);
            if (stop.tv_sec - start.tv_sec >= COMMAND_REPLY_TIMEOUT) {
                rv = -1;
                break;
            }

            if (id < SUI_MSG_SEC_DISP_PROPERTIES) {
                input.name = g_lname;
                input.is_peep = 1;
                client->recv_message(input, [&](ITuiComm::sock_msg data) {
                    output.info_0 = data.info_0;
                    output.info_1 = data.info_1;
                    output.info_2 = data.info_2;
                    output.response = data.response;
                });
            } else {
                input.name = g_lname;
                input.is_peep = 0;
                client->recv_smessage(input, [&](ITuiComm::sock_msg data) {
                    output.response = data.response;
                    if (!output.response) {
                        mex[0] = data.info_0;
                        mex[1] = data.info_1;
                        mex[2] = data.info_2;
                        mex[3] = data.info_3;
                        secdisp_output->status = data.output.status;
                        secdisp_output->u16NumBuffers =
                            data.output.u16NumBuffers;
                        secdisp_output->u32Height = data.output.u32Height;
                        secdisp_output->u32Width = data.output.u32Width;
                        secdisp_output->bufHandle = data.output.bufHandle;
                        secdisp_output->u32Stride = data.output.u32Stride;
                        secdisp_output->u32PixelWidth =
                            data.output.u32PixelWidth;
                        secdisp_output->fd = data.output.fd;
                    }
                });
            }

            if (id >= SUI_MSG_SEC_DISP_PROPERTIES && !output.response) break;
            if (output.response == 0xff) {
                LOGE("%s(%s): Error reading from socket: %s -- %s", __func__,
                     g_lname, g_rname, strerror(errno));
                rv = -2;
                break;
            }

            if (output.response == 0xf) {
                sched_yield();
                continue;
            }

            // here we process only responses, so if this isn't
            // we yield and give time to the command thread to get it
            if (output.info_0 == SUI_MSG_RSP_MSK) {
                input.name = g_lname;
                input.is_peep = 0;

                client->recv_message(input, [&](ITuiComm::sock_msg data) {
                    mex[0] = data.info_0;
                    mex[1] = data.info_1;
                    mex[2] = data.info_2;
                    mex[3] = data.info_3;
                    // memcpy(secdisp_output,&data.output,sizeof(secdisp_op));
                    output.response = data.response;
                });

                break;
            } else {
                pthread_mutex_lock(&wait_mtx);
                if (msg_count == 0) {
                    msg_count++;
                    pthread_cond_signal(&cond_wait);
                }
                pthread_mutex_unlock(&wait_mtx);
                // a command or a notification, let it be processed elsewhere
                sched_yield();
                continue;
            }
        }

        LOGE("%s(%s):response = %02X", __func__, g_lname, output.response);
        if (mex[0] != SUI_MSG_RSP_MSK) {
            LOGE("%s(%s): No answer received: %02X %02X %02X %02X", __func__,
                 g_lname, mex[0], mex[1], mex[2], mex[3]);
            rv = -EFAULT;
            break;
        }
        if (mex[1] != id) {
            LOGE("%s(%s): Wrong id, %02X vs %02X: %02X %02X %02X %02X",
                 __func__, g_lname, mex[1], id, mex[0], mex[1], mex[2], mex[3]);
            rv = -EFAULT;
            break;
        }
        if (!output.response) {
            LOGD("%s(%s): Response: %02X", __func__, g_lname, mex[2]);
            *response = mex[2];
        }
    } while (0);
    pthread_mutex_unlock(&command_mtx);
    EXITV(rv);
}

/** Set the notification callback */
void svc_sock_register_notification_callback(
    svc_sock_notification_callback_t cback)
{
    notification_cback = cback;
}

/** Set the command callback */
void svc_sock_register_command_callback(svc_sock_command_callback_t cback)
{
    command_cback = cback;
}

/** Queue a received message. Used by the callback in the async usage model */
static void svc_sock_queue_message(const uint8_t type, const uint8_t id,
                                   const uint8_t payload, secdisp_ip input,
                                   char const *const src_name,
                                   size_t const src_len)
{
    pthread_mutex_lock(&mex_mtx);
    do {
        if (g_mex_queue.head == (g_mex_queue.tail + 1) % MESSAGE_ARRAY_LENGTH) {
            LOGE("%s(%s): Queue full, discarding message (0x%02X %02X, %02X)",
                 __func__, g_lname, type, id, payload);
            break;
        }
        if (src_len > UNIX_PATH_MAX) {
            LOGE(
                "%s(%s): Unsupported source socket length (0x%02X %02X, %02X), "
                "%zu vs%u max",
                __func__, g_lname, type, id, payload, src_len, UNIX_PATH_MAX);
            break;
        }

        // add it to tail
        g_mex_queue.messages[g_mex_queue.tail].data[0] = type;
        g_mex_queue.messages[g_mex_queue.tail].data[1] = id;
        g_mex_queue.messages[g_mex_queue.tail].data[2] = payload;

        g_mex_queue.messages[g_mex_queue.tail].input.bDisableOverlay =
            input.bDisableOverlay;
        g_mex_queue.messages[g_mex_queue.tail].input.bToRender =
            input.bToRender;
        g_mex_queue.messages[g_mex_queue.tail].input.colorFormat =
            input.colorFormat;
        g_mex_queue.messages[g_mex_queue.tail].input.u32Height =
            input.u32Height;
        g_mex_queue.messages[g_mex_queue.tail].input.u32Width = input.u32Width;
        g_mex_queue.messages[g_mex_queue.tail].input.u32HPos = input.u32HPos;
        g_mex_queue.messages[g_mex_queue.tail].input.u32VPos = input.u32VPos;
        g_mex_queue.messages[g_mex_queue.tail].input.bufHandle =
            input.bufHandle;

        g_mex_queue.messages[g_mex_queue.tail].src_len = src_len;
        memcpy(g_mex_queue.messages[g_mex_queue.tail].src_name, src_name,
               src_len);
        g_mex_queue.tail = (g_mex_queue.tail + 1) % MESSAGE_ARRAY_LENGTH;
        sem_post(&g_msg_available);
    } while (0);
    pthread_mutex_unlock(&mex_mtx);
}

/** Notification callback for the async usage model */
static void svc_sock_internal_notification_cback(const uint8_t id,
                                                 const uint8_t payload,
                                                 char const *const src_name,
                                                 size_t const src_len)
{
    secdisp_ip input = {0};
    svc_sock_queue_message(SUI_MSG_NOT_MSK, id, payload, input, src_name,
                           src_len);
}

/** Command callback for the async usage model */
static int svc_sock_internal_command_cback(uint8_t const id,
                                           uint8_t const payload,
                                           secdisp_ip input,
                                           char const *const src_name,
                                           size_t const src_len)
{
    svc_sock_queue_message(SUI_MSG_CMD_MSK, id, payload, input, src_name,
                           src_len);
    return -1;  // we don't want a response to be sent
}

/** Blocking function waiting for a message to be received */
int svc_sock_wait_for_message(uint8_t *mex, size_t mex_len,
                              secdisp_ip *secdisp_input, char *src_name,
                              size_t *src_len)
{
    int rv = 0;
    static int cback_registered = 0;
    ENTER;

    do {
        if ((mex_len != BYTES_IN_MEX) || (mex == NULL) ||
            (secdisp_input == NULL)) {
            LOGE("%s(%s): Bad input", __func__, g_lname);
            rv = -EINVAL;
            break;
        }
        if (!cback_registered) {
            svc_sock_register_notification_callback(
                svc_sock_internal_notification_cback);
            svc_sock_register_command_callback(svc_sock_internal_command_cback);
            rv = sem_init(&g_msg_available, 0, 0);
            if (rv) {
                LOGE("%s(%s): Failed to initialize semaphore: %s", __func__,
                     g_lname, strerror(errno));
                rv = errno;
                break;
            }
            cback_registered = 1;
        }
        rv = sem_wait(&g_msg_available);
        if (tui_restarted) {
            LOGE("%s(%s): tui_comm restarted!!", __func__, g_lname);
            tui_restarted = false;
            cback_registered = 0;
            rv = -EAGAIN;
            break;
        }
        if (rv != 0) {
            LOGE("%s(%s): Failed waiting on the semaphore: %s", __func__,
                 g_lname, strerror(errno));
            rv = errno;
            break;
        }

        pthread_mutex_lock(&mex_mtx);
        do {
            if (g_mex_queue.head == (g_mex_queue.tail) % MESSAGE_ARRAY_LENGTH) {
                LOGW("%s(%s): Queue empty, nothing to get!", __func__, g_lname);
                break;
            }

            mex[0] = g_mex_queue.messages[g_mex_queue.head].data[0];
            mex[1] = g_mex_queue.messages[g_mex_queue.head].data[1];
            mex[2] = g_mex_queue.messages[g_mex_queue.head].data[2];

            secdisp_input->bDisableOverlay =
                g_mex_queue.messages[g_mex_queue.head].input.bDisableOverlay;
            secdisp_input->bToRender =
                g_mex_queue.messages[g_mex_queue.head].input.bToRender;
            secdisp_input->colorFormat =
                g_mex_queue.messages[g_mex_queue.head].input.colorFormat;
            secdisp_input->u32Height =
                g_mex_queue.messages[g_mex_queue.head].input.u32Height;
            secdisp_input->u32Width =
                g_mex_queue.messages[g_mex_queue.head].input.u32Width;
            secdisp_input->u32HPos =
                g_mex_queue.messages[g_mex_queue.head].input.u32HPos;
            secdisp_input->u32VPos =
                g_mex_queue.messages[g_mex_queue.head].input.u32VPos;
            secdisp_input->bufHandle =
                g_mex_queue.messages[g_mex_queue.head].input.bufHandle;

            if (src_name && (*src_len == UNIX_PATH_MAX)) {
                *src_len = g_mex_queue.messages[g_mex_queue.head].src_len;
                memcpy(src_name,
                       g_mex_queue.messages[g_mex_queue.head].src_name,
                       *src_len);
            }
            // and zero it for safety
            g_mex_queue.messages[g_mex_queue.head].data[0] = 0;
            g_mex_queue.messages[g_mex_queue.head].data[1] = 0;
            g_mex_queue.messages[g_mex_queue.head].data[2] = 0;
            g_mex_queue.messages[g_mex_queue.head].src_len = 0;
            memset(g_mex_queue.messages[g_mex_queue.head].src_name, 0,
                   UNIX_PATH_MAX);
            // move head
            g_mex_queue.head = (g_mex_queue.head + 1) % MESSAGE_ARRAY_LENGTH;
        } while (0);
        pthread_mutex_unlock(&mex_mtx);
    } while (0);
    EXITV(rv);
}
}
