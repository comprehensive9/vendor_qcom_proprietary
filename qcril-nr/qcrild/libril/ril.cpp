/*
* Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Not a Contribution.
* Apache license notifications and license are retained
* for attribution purposes only.
*/
/* //device/libs/telephony/ril.cpp
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#define LOG_TAG "RILC"
#define TAG LOG_TAG

#include <inttypes.h>
#ifdef RIL_FOR_MDM_LE
#include <utils/Log2.h>
#else
#include "ril_service.h"
#include <sys/system_properties.h>
#include <log/log.h>
#endif
#include <telephony/ril.h>
#include <telephony/ril_ims.h>
#include <libril/ril_ex.h>
#include <telephony/ril_cdma_sms.h>
#include <cutils/sockets.h>
#include <telephony/record_stream.h>
#include <utils/Log.h>
#include <utils/SystemClock.h>
#include <pthread.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <locale>
#include <inttypes.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <sys/un.h>
#include <assert.h>
#include <limits.h>
#include <codecvt>
#include <netinet/in.h>
#include <cutils/properties.h>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "ril_event.h"
#include "ril_internal.h"
#include "WakeLock.h"
#include <sys/stat.h>
#include <string.h>

#include <Marshal.h>
#include <SocketRequestContext.h>
#include <ril_socket_api.h>

extern "C" void
RIL_onRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen);

extern "C" void
RIL_onRequestAck(RIL_Token t);

RIL_RadioFunctions *s_vendorFunctions = NULL;

#define RIL_SOCKET_PATH "/dev/socket/qcrild/rild"

namespace android {

#define PHONE_PROCESS "radio"
#define BLUETOOTH_PROCESS "bluetooth"

#define ANDROID_WAKE_LOCK_NAME "radio-interface"

#define ANDROID_WAKE_LOCK_SECS 0
#define ANDROID_WAKE_LOCK_USECS 200000

#define PROPERTY_RIL_IMPL "gsm.version.ril-impl"

// match with constant in RIL.java
#define MAX_COMMAND_BYTES (8 * 1024)

// Basically: memset buffers that the client library
// shouldn't be using anymore in an attempt to find
// memory usage issues sooner.
#define MEMSET_FREED 1

#define NUM_ELEMS(a)     (sizeof (a) / sizeof (a)[0])

/* Negative values for private RIL errno's */
#define RIL_ERRNO_INVALID_RESPONSE (-1)
#define RIL_ERRNO_NO_MEMORY (-12)

enum WakeType {DONT_WAKE, WAKE_PARTIAL};

typedef struct {
    int requestNumber;
    int (*responseFunction) (int slotId, int responseType, int token,
            RIL_Errno e, void *response, size_t responselen);
    WakeType wakeType;
} UnsolResponseInfo;

typedef struct UserCallbackInfo {
    RIL_TimedCallback p_callback;
    void *userParam;
    struct ril_event event;
    struct UserCallbackInfo *p_next;
} UserCallbackInfo;

extern "C" const char * failCauseToString(RIL_Errno);
extern "C" const char * callStateToString(RIL_CallState);
extern "C" const char * radioStateToString(RIL_RadioState);

#ifndef RIL_FOR_MDM_LE
extern "C"
char ril_service_name_base[MAX_SERVICE_NAME_LENGTH] = RIL_SERVICE_NAME_BASE;
extern "C"
char ril_service_name[MAX_SERVICE_NAME_LENGTH] = RIL1_SERVICE_NAME;
#endif
/*******************************************************************/

RIL_RadioFunctions s_callbacks = {0, NULL, NULL, NULL, NULL, NULL};

static int s_registerCalled = 0;

static pthread_t s_tid_dispatch;
static int s_started = 0;

static int s_fdWakeupRead;
static int s_fdWakeupWrite;

int s_wakelock_count = 0;

static struct ril_event s_wakeupfd_event;

static qtimutex::QtiSharedMutex s_pendingRequestsMutex;
static qtimutex::QtiSharedMutex s_wakeLockCountMutex;
static RequestInfo *s_pendingRequests = NULL;

static qtimutex::QtiSharedMutex s_pendingSocketRequestsMutex;
static std::unordered_map<SocketRequestInfo*, std::shared_ptr<SocketRequestInfo>> s_pendingSocketRequests;

static const struct timeval TIMEVAL_WAKE_TIMEOUT = {ANDROID_WAKE_LOCK_SECS,ANDROID_WAKE_LOCK_USECS};


static qtimutex::QtiSharedMutex s_startupMutex;
static std::condition_variable_any s_startupCond;

static UserCallbackInfo *s_last_wake_timeout_info = NULL;

static void *s_lastNITZTimeData = NULL;
static size_t s_lastNITZTimeDataSize;

#if RILC_LOG
    static char printBuf[PRINTBUF_SIZE];
#endif

WakeLock androidWakeLock(ANDROID_WAKE_LOCK_NAME);
WakeLock socketWakeLock("SocketWakeLock");

/*******************************************************************/
void grabPartialWakeLock();
void releaseWakeLock();
static void wakeTimeoutCallback(void *);

void RIL_onSocketRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responseLen);

#ifdef RIL_SHLIB
#if defined(ANDROID_MULTI_SIM)
extern "C" void RIL_onUnsolicitedResponse(int unsolResponse, const void *data, size_t datalen, RIL_SOCKET_ID socket_id);
void sendUnsolicitedResponseOverHidl(int unsolResponse, const void *data, size_t datalen, RIL_SOCKET_ID socket_id);
#else
extern "C" void RIL_onUnsolicitedResponse(int unsolResponse, const void *data, size_t datalen);
void sendUnsolicitedResponseOverHidl(int unsolResponse, const void *data, size_t datalen);
#endif

void sendUnsolicitedResponseOverSocket(int unsolResponse, const void *buffer, size_t bufferLength);
#endif

#if defined(ANDROID_MULTI_SIM)
#define RIL_UNSOL_RESPONSE(a, b, c, d) RIL_onUnsolicitedResponse((a), (b), (c), (d))
#else
#define RIL_UNSOL_RESPONSE(a, b, c, d) RIL_onUnsolicitedResponse((a), (b), (c))
#endif

static UserCallbackInfo * internalRequestTimedCallback
    (RIL_TimedCallback callback, void *param,
        const struct timeval *relativeTime);

/** Index == requestNumber */
static CommandInfo s_commands[] = {
#ifndef RIL_FOR_MDM_LE
#include "ril_commands.h"
#endif
};

static UnsolResponseInfo s_unsolResponses[] = {
#ifndef RIL_FOR_MDM_LE
#include "ril_unsol_commands.h"
#endif
};

#ifndef RIL_FOR_MDM_LE
char * RIL_getServiceName() {
    return ril_service_name;
}
#endif

RequestInfo *
addRequestToList(int serial, int slotId, int request) {
    RequestInfo *pRI;
    RIL_SOCKET_ID socket_id = (RIL_SOCKET_ID) slotId;
    /* Hook for current context */
    /* pendingRequestsMutextHook refer to &s_pendingRequestsMutex */
    qtimutex::QtiSharedMutex* pendingRequestsMutexHook = &s_pendingRequestsMutex;
    /* pendingRequestsHook refer to &s_pendingRequests */
    RequestInfo** pendingRequestsHook = &s_pendingRequests;

    pRI = (RequestInfo *)calloc(1, sizeof(RequestInfo));
    if (pRI == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(request));
        return NULL;
    }

    pRI->token = serial;
    pRI->pCI = &(s_commands[request]);
    pRI->socket_id = socket_id;

    pendingRequestsMutexHook->lock();

    pRI->p_next = *pendingRequestsHook;
    *pendingRequestsHook = pRI;

    pendingRequestsMutexHook->unlock();

    return pRI;
}

static void triggerEvLoop() {
    int ret;
    if (!pthread_equal(pthread_self(), s_tid_dispatch)) {
        /* trigger event loop to wakeup. No reason to do this,
         * if we're in the event loop thread */
         do {
            ret = write (s_fdWakeupWrite, " ", 1);
         } while (ret < 0 && errno == EINTR);
    }
}

static void rilEventAddWakeup(struct ril_event *ev) {
    ril_event_add(ev);
    triggerEvLoop();
}

/**
 * A write on the wakeup fd is done just to pop us out of select()
 * We empty the buffer here and then ril_event will reset the timers on the
 * way back down
 */
static void processWakeupCallback(int /*fd*/, short /*flags*/, void * /*param*/) {
    char buff[16];
    int ret;

    RLOGV("processWakeupCallback");

    /* empty our wakeup socket out */
    do {
        ret = read(s_fdWakeupRead, &buff, sizeof(buff));
    } while (ret > 0 || (ret < 0 && errno == EINTR));
}

static void resendLastNITZTimeData(RIL_SOCKET_ID socket_id) {
#ifndef RIL_FOR_MDM_LE
    if (s_lastNITZTimeData != NULL) {
        int responseType = (s_callbacks.version >= 13)
                           ? RESPONSE_UNSOLICITED_ACK_EXP
                           : RESPONSE_UNSOLICITED;
        int ret = radio::nitzTimeReceivedInd(
            (int)socket_id, responseType, 0,
            RIL_E_SUCCESS, s_lastNITZTimeData, s_lastNITZTimeDataSize);
        if (ret == 0) {
            free(s_lastNITZTimeData);
            s_lastNITZTimeData = NULL;
        }
    }
#endif
}

// TODO: Needs to be adapted for converged LA/LE RIL
void onNewCommandConnect(RIL_SOCKET_ID socket_id) {
    // Inform we are connected and the ril version
    int rilVer = s_callbacks.version;
    RIL_UNSOL_RESPONSE(RIL_UNSOL_RIL_CONNECTED,
                                    &rilVer, sizeof(rilVer), socket_id);

    // implicit radio state changed
    RIL_UNSOL_RESPONSE(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED,
                                    NULL, 0, socket_id);

    // Send last NITZ time data, in case it was missed
    if (s_lastNITZTimeData != NULL) {
        resendLastNITZTimeData(socket_id);
    }

    // Get version string
    if (s_callbacks.getVersion != NULL) {
        const char *version;
        version = s_callbacks.getVersion();
#ifndef RIL_FOR_MDM_LE
        RLOGI("RIL Daemon version: %s\n", version);

        property_set(PROPERTY_RIL_IMPL, version);
    } else {
        RLOGI("RIL Daemon version: unavailable\n");
        property_set(PROPERTY_RIL_IMPL, "unavailable");
#endif
    }

}

static void userTimerCallback (int /*fd*/, short /*flags*/, void *param) {
    UserCallbackInfo *p_info;

    p_info = (UserCallbackInfo *)param;

    p_info->p_callback(p_info->userParam);

    // FIXME generalize this...there should be a cancel mechanism
    s_wakeLockCountMutex.lock();
    if (s_last_wake_timeout_info != NULL && s_last_wake_timeout_info == p_info) {
        s_last_wake_timeout_info = NULL;
    }
    s_wakeLockCountMutex.unlock();

    free(p_info);
}


static void *
eventLoop(void * /*param*/) {
    int ret;
    int filedes[2];

    ril_event_init();

    s_startupMutex.lock();

    s_started = 1;
    s_startupCond.notify_all();

    s_startupMutex.unlock();

    ret = pipe(filedes);

    if (ret < 0) {
        RLOGE("Error in pipe() errno:%d", errno);
        return NULL;
    }

    s_fdWakeupRead = filedes[0];
    s_fdWakeupWrite = filedes[1];

    fcntl(s_fdWakeupRead, F_SETFL, O_NONBLOCK);

    ril_event_set (&s_wakeupfd_event, s_fdWakeupRead, true,
                processWakeupCallback, NULL);

    rilEventAddWakeup (&s_wakeupfd_event);

    // Only returns on error
    ril_event_loop();
    RLOGE ("error in event_loop_base errno:%d", errno);
    // kill self to restart on error
    kill(0, SIGKILL);

    return NULL;
}

extern "C" void
RIL_startEventLoop(void) {
    /* spin up eventLoop thread and wait for it to get started */
    s_started = 0;
    s_startupMutex.lock();

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int result = pthread_create(&s_tid_dispatch, &attr, eventLoop, NULL);
    if (result != 0) {
        RLOGE("Failed to create dispatch thread: %s", strerror(result));
        goto done;
    }

    while (s_started == 0) {
        s_startupCond.wait(s_startupMutex);
    }

done:
    s_startupMutex.unlock();
}

// Used for testing purpose only.
extern "C" void RIL_setcallbacks (const RIL_RadioFunctions *callbacks) {
    memcpy(&s_callbacks, callbacks, sizeof (RIL_RadioFunctions));
}

static void processCommandsCallback(int fd, short flags, void *param);
static int processCommandBuffer(void *buffer, size_t buflen, RIL_SOCKET_ID socket_id, bool& isDisconnectionRequest);
static int sendResponseRaw(const void *data, size_t dataSize);
static int blockingWrite(int fd, const void *buffer, size_t len);

static struct ril_event s_commands_event;
static struct ril_event s_listen_event;
static pthread_mutex_t s_writeMutex = PTHREAD_MUTEX_INITIALIZER;
static SocketListenParam s_ril_param_socket = {
                        RIL_SOCKET_1,             /* socket_id */
                        -1,                       /* fdListen */
                        -1,                       /* fdCommand */
                        PHONE_PROCESS,            /* processName */
                        &s_commands_event,        /* commands_event */
                        &s_listen_event,          /* listen_event */
                        processCommandsCallback,  /* processCommandsCallback */
                        NULL,                     /* p_rs */
                        RIL_TELEPHONY_SOCKET      /* type */
                        };

// TODO: Accept the SocketRequestContext instead of the token
SocketRequestInfo* addSocketRequestToList(uint64_t token, const SocketResponseCallback& responseCallback) {
    std::shared_ptr<SocketRequestInfo> requestInfo = std::make_shared<SocketRequestInfo>();
    if (requestInfo == nullptr) {
        RLOGE("Failed to allocate memory for request info.");
        return nullptr;
    }

    requestInfo->token = token;
    requestInfo->responseCallback = responseCallback;

    {
        std::lock_guard<qtimutex::QtiSharedMutex> lock(s_pendingSocketRequestsMutex);
        s_pendingSocketRequests[requestInfo.get()] = requestInfo;
    }

    return requestInfo.get();
}

static int blockingWrite(int fd, const void *buffer, size_t len) {
    size_t writeOffset = 0;
    const uint8_t *toWrite;

    toWrite = (const uint8_t *)buffer;

    while (writeOffset < len) {
        ssize_t written;
        do {
            written = write (fd, toWrite + writeOffset,
                                len - writeOffset);
        } while (written < 0 && ((errno == EINTR) || (errno == EAGAIN)));

        if (written >= 0) {
            writeOffset += written;
        } else {   // written < 0
            RLOGE ("RIL Response: unexpected error on write errno:%d", errno);
            close(fd);
            return -1;
        }
    }

    RLOGI("RIL Response bytes written:%zu", writeOffset);

    return 0;
}

static int sendResponseRaw(const void *data, size_t dataSize) {
    int fd = s_ril_param_socket.fdCommand;
    int ret;
    pthread_mutex_t* writeMutexHook = &s_writeMutex;

    if (fd < 0) {
        return -1;
    }

    if (dataSize > MAX_COMMAND_BYTES) {
        RLOGE("RIL: packet larger than %u (%u)",
                MAX_COMMAND_BYTES, (unsigned int )dataSize);
        return -1;
    }

    pthread_mutex_lock(writeMutexHook);

    uint32_t header = htonl(dataSize);
    socketWakeLock.acquire();
    ret = blockingWrite(fd, (void*) &header, sizeof(header));

    if (ret < 0) {
        pthread_mutex_unlock(writeMutexHook);
        return ret;
    }

    ret = blockingWrite(fd, data, dataSize);
    socketWakeLock.release();
    if (ret < 0) {
        pthread_mutex_unlock(writeMutexHook);
        return ret;
    }

    pthread_mutex_unlock(writeMutexHook);

    return 0;
}

void printBuffer(const void* buffer, size_t bufferLength, std::string& s) {
    char hex[4];
    for (int i = 0; i < bufferLength; i++) {
        std::snprintf(hex, sizeof(hex), "%-3.2hhX", *(static_cast<const uint8_t*>(buffer) + i));
        s += hex;
    }
}

static int processCommandBuffer(void *buffer, size_t buflen, RIL_SOCKET_ID socket_id,
                                [[maybe_unused]] bool& isDisconnectionRequest) {
    Marshal p;
    int32_t request;
    uint64_t token;

    std::string s;
    printBuffer(buffer, buflen, s);
    RLOGI("Buffer size: %zu", buflen);
    RLOGI("Buffer contents: %s", s.c_str());

    size_t requestParameterParcelOffset = sizeof(request) + sizeof(token);

    // Buffer is expected to contain at least 12 bytes. First 4 bytes contain
    // the request ID, and the following 8 bytes contain the token.
    if (buflen < requestParameterParcelOffset) {
        RLOGI("Command buffer should contain at least %zu bytes.", requestParameterParcelOffset);
        return 0;
    }

    request = *(reinterpret_cast<int32_t*>(buffer));
    token = *(reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(buffer) + sizeof(request)));

    p.setData(std::string(reinterpret_cast<const char*>(buffer) + requestParameterParcelOffset,
                          buflen - requestParameterParcelOffset));

    // Received an Ack for the previous result sent to RIL.java,
    // so release wakelock and exit
    // TODO: See if this is really needed
    if (request == RIL_RESPONSE_ACKNOWLEDGEMENT) {
        releaseWakeLock();
        return 0;
    }

    std::shared_ptr<SocketRequestContext> context = std::make_shared<SocketRequestContext>(static_cast<uint8_t>(socket_id), request, token);
    // TODO: Check if context was allocated

    switch (request) {
#if defined(RIL_ENABLE_SOCKET_INTERFACE) || defined(RIL_FOR_MDM_LE)
        case RIL_REQUEST_DIAL:
            ril::socket::api::dispatchDial(context, p);
            break;
        case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG:
            ril::socket::api::dispatchRilRequestGsmGetBroadcastSmsConfig(context, p);
            break;
        case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG:
            ril::socket::api::dispatchRilRequestGsmSetBroadcastSmsConfig(context, p);
            break;
        case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION:
            ril::socket::api::dispatchRilRequestGsmSmsBroadcastActivation(context, p);
            break;
        case RIL_REQUEST_VOICE_REGISTRATION_STATE:
            ril::socket::api::dispatchGetVoiceRegState(context, p);
            break;
        case RIL_REQUEST_DATA_REGISTRATION_STATE:
            ril::socket::api::dispatchGetDataRegState(context, p);
            break;
        case RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER:
            ril::socket::api::dispatchSetIndicationFilter(context, p);
            break;
        case RIL_REQUEST_HANGUP:
            ril::socket::api::dispatchHangup(context, p);
            break;
        case RIL_REQUEST_SEND_USSD:
            ril::socket::api::dispatchSendUssd(context, p);
            break;
        case RIL_REQUEST_CANCEL_USSD:
            ril::socket::api::dispatchCancelUssd(context, p);
            break;
        case RIL_REQUEST_DTMF:
            ril::socket::api::dispatchDtmf(context, p);
            break;
        case RIL_REQUEST_DTMF_START:
            ril::socket::api::dispatchDtmfStart(context, p);
            break;
        case RIL_REQUEST_DTMF_STOP:
            ril::socket::api::dispatchDtmfStop(context, p);
            break;
        case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
            ril::socket::api::dispatchLastCallFailCause(context, p);
            break;
        case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
            ril::socket::api::dispatchSetNetworkType(context, p);
            break;
        case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
            ril::socket::api::dispatchGetNetworkType(context, p);
            break;
        case RIL_REQUEST_GET_CURRENT_CALLS:
            ril::socket::api::dispatchGetCurrentCalls(context, p);
            break;
        case RIL_REQUEST_ANSWER:
            ril::socket::api::dispatchAnswer(context, p);
            break;
        case RIL_REQUEST_CONFERENCE:
            ril::socket::api::dispatchConference(context, p);
            break;
        case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
            ril::socket::api::dispatchSwitchWaitingOrHoldingAndActive(context, p);
            break;
        case RIL_REQUEST_UDUB:
            ril::socket::api::dispatchUdub(context, p);
            break;
        case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
            ril::socket::api::dispatchHangupWaitingOrBackground(context, p);
            break;
        case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
            ril::socket::api::dispatchHangupForegroundResumeBackground(context, p);
            break;
        case RIL_REQUEST_SEPARATE_CONNECTION:
            ril::socket::api::dispatchSeparateConnection(context, p);
            break;
        case RIL_REQUEST_SHUTDOWN:
            ril::socket::api::dispatchShutDown(context, p);
            break;
        case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
            ril::socket::api::dispatchExitEmergencyCbMode(context, p);
            break;
        case RIL_REQUEST_SET_BAND_MODE:
            ril::socket::api::dispatchSetBandMode(context, p);
            break;
        case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE:
            ril::socket::api::dispatchQueryAvailableBandMode(context, p);
            break;
        case RIL_REQUEST_SIGNAL_STRENGTH:
            ril::socket::api::dispatchSignalStrength(context, p);
            break;
        case RIL_REQUEST_RADIO_POWER:
            ril::socket::api::dispatchRadioPower(context, p);
            break;
        case RIL_REQUEST_ALLOW_DATA:
            ril::socket::api::dispatchAllowData(context, p);
            break;
        case RIL_REQUEST_GET_UICC_ENABLEMENT_STATUS:
            ril::socket::api::dispatchGetEnableSimStatus(context, p);
            break;
        case RIL_REQUEST_ENABLE_UICC_APPLICATIONS:
            ril::socket::api::dispatchEnableSim(context, p);
            break;
        case RIL_REQUEST_SET_LOCATION_UPDATES:
            ril::socket::api::dispatchSetLocationUpdates(context, p);
            break;
        case RIL_REQUEST_GET_ACTIVITY_INFO:
            ril::socket::api::dispatchGetActivityInfo(context, p);
            break;
        case RIL_REQUEST_VOICE_RADIO_TECH:
            ril::socket::api::dispatchVoiceRadioTech(context, p);
            break;
        case RIL_REQUEST_DEVICE_IDENTITY:
            ril::socket::api::dispatchDeviceIdentity(context, p);
            break;
        case RIL_REQUEST_OPERATOR:
            ril::socket::api::dispatchOperatorName(context, p);
            break;
        case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:
            ril::socket::api::dispatchQueryNetworkSelectionMode(context, p);
            break;
        case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC:
            ril::socket::api::dispatchSetNetworkSelectionAutomatic(context, p);
            break;
        case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL:
            ril::socket::api::dispatchSetNetworkSelectionManual(context, p);
            break;
        case RIL_REQUEST_BASEBAND_VERSION:
            ril::socket::api::dispatchBasebandVersion(context, p);
            break;
        case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
            ril::socket::api::dispatchExplicitCallTransfer(context, p);
            break;
        case RIL_REQUEST_SET_MUTE:
            ril::socket::api::dispatchSetMute(context, p);
            break;
        case RIL_REQUEST_GET_MUTE:
            ril::socket::api::dispatchGetMute(context, p);
            break;
        case RIL_REQUEST_SET_CLIR:
            ril::socket::api::dispatchSetClir(context, p);
            break;
        case RIL_REQUEST_GET_CLIR:
            ril::socket::api::dispatchGetClir(context, p);
            break;
        case RIL_IMS_REQUEST_SEND_RTT_MSG:
            ril::socket::api::dispatchImsSendRttMsg(context, p);
            break;
        case RIL_IMS_REQUEST_SEND_GEOLOCATION_INFO:
            ril::socket::api::dispatchImsSendGeolocationInfo(context, p);
            break;
        case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE:
            ril::socket::api::dispatchSetUnsolCellInfoListRate(context, p);
            break;
        case RIL_REQUEST_SETUP_DATA_CALL:
            ril::socket::api::dispatchSetupDataCall(context, p);
            break;
        case RIL_REQUEST_START_LCE:
            ril::socket::api::dispatchStartLce(context, p);
            break;
        case RIL_REQUEST_STOP_LCE:
            ril::socket::api::dispatchStopLce(context, p);
            break;
        case RIL_REQUEST_PULL_LCEDATA:
            ril::socket::api::dispatchPullLceData(context, p);
            break;
        case RIL_REQUEST_DEACTIVATE_DATA_CALL:
            ril::socket::api::dispatchDeactivateDataCall(context, p);
            break;
        case RIL_REQUEST_SET_DATA_PROFILE:
            ril::socket::api::dispatchSetDataProfile(context, p);
            break;
        case RIL_REQUEST_DATA_DUMP:
            ril::socket::api::dispatchCaptureRilDataDump(context,p);
            break;
        case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
            ril::socket::api::dispatchSetInitialAttachApn(context, p);
            break;
        case RIL_REQUEST_DATA_CALL_LIST:
            ril::socket::api::dispatchGetDataCallList(context, p);
            break;
        case RIL_REQUEST_GET_CELL_INFO_LIST:
            ril::socket::api::dispatchCellInfo(context, p);
            break;
        case RIL_REQUEST_SEND_SMS:
            ril::socket::api::dispatchSendSms(context, p);
            break;
        case RIL_REQUEST_SMS_ACKNOWLEDGE:
            ril::socket::api::dispatchSmsAck(context, p);
            break;
        case RIL_REQUEST_GET_SMSC_ADDRESS:
            ril::socket::api::dispatchGetSmscAddress(context, p);
            break;
        case RIL_REQUEST_SET_SMSC_ADDRESS:
            ril::socket::api::dispatchSetSmscAddress(context, p);
            break;
        case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
            ril::socket::api::dispatchEnterNetworkDepersonalization(context, p);
            break;
        case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
            ril::socket::api::dispatchStkSendEnvelopeCommand(context, p);
            break;
        case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM:
            ril::socket::api::dispatchStkHandleCallSetupRequestedFromSim(context, p);
            break;
        case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
            ril::socket::api::dispatchStkSendTerminalResponse(context, p);
            break;
        case RIL_REQUEST_UPDATE_ADN_RECORD:
            ril::socket::api::dispatchUpdateAdnRecord(context, p);
            break;
        case RIL_REQUEST_GET_ADN_RECORD:
            ril::socket::api::dispatchGetAdnRecord(context, p);
            break;
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
            ril::socket::api::dispatchStkServiceIsRunning(context, p);
            break;
        case RIL_REQUEST_QUERY_TTY_MODE:
            ril::socket::api::dispatchGetTtyMode(context, p);
            break;
        case RIL_REQUEST_SET_TTY_MODE:
            ril::socket::api::dispatchSetTtyMode(context, p);
            break;
        case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE:
            ril::socket::api::dispatchGetVoicePrivacyMode(context, p);
            break;
        case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE:
            ril::socket::api::dispatchSetVoicePrivacyMode(context, p);
            break;
        case RIL_REQUEST_CDMA_FLASH:
            ril::socket::api::dispatchCdmaFlash(context, p);
            break;
        case RIL_REQUEST_CDMA_BURST_DTMF:
            ril::socket::api::dispatchCdmaBurstDtmf(context, p);
            break;
        case RIL_REQUEST_IMS_REGISTRATION_STATE:
            ril::socket::api::dispatchGetImsRegistration(context, p);
            break;
        case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
            ril::socket::api::dispatchQueryAvailableNetworks(context, p);
            break;
        case RIL_REQUEST_START_NETWORK_SCAN:
            ril::socket::api::dispatchStartNetworkScanRequest(context, p);
            break;
        case RIL_REQUEST_STOP_NETWORK_SCAN:
            ril::socket::api::dispatchStopNetworkScanRequest(context, p);
            break;
        case RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS:
            ril::socket::api::dispatchSetSystemSelectionChannels(context, p);
            break;
        case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
            ril::socket::api::dispatchQueryCallForwardStatus(context, p);
            break;
        case RIL_REQUEST_SET_CALL_FORWARD:
            ril::socket::api::dispatchSetCallForward(context, p);
            break;
        case RIL_REQUEST_QUERY_CALL_WAITING:
            ril::socket::api::dispatchQueryCallWaiting(context, p);
            break;
        case RIL_REQUEST_SET_CALL_WAITING:
            ril::socket::api::dispatchSetCallWaiting(context, p);
            break;
        case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
            ril::socket::api::dispatchChangeBarringPassword(context, p);
            break;
        case RIL_REQUEST_QUERY_CLIP:
            ril::socket::api::dispatchQueryClip(context, p);
            break;
        case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION:
            ril::socket::api::dispatchSetSuppSvcNotification(context, p);
            break;
        case RIL_REQUEST_GET_IMSI:
            ril::socket::api::dispatchSimGetImsiReq(context, p);
            break;
        case RIL_REQUEST_OEM_HOOK_RAW:
            ril::socket::api::dispatchOemhookRaw(context, p);
            break;
        case RIL_REQUEST_SIM_IO:
            ril::socket::api::dispatchSimIOReq(context, p);
            break;
        case RIL_REQUEST_GET_SIM_STATUS:
            ril::socket::api::dispatchSimGetSimStatusReq(context, p);
            break;
        case RIL_REQUEST_SIM_OPEN_CHANNEL:
            ril::socket::api::dispatchSimOpenChannelReq(context, p);
            break;
        case RIL_REQUEST_SIM_CLOSE_CHANNEL:
            ril::socket::api::dispatchSimCloseChannelReq(context, p);
            break;
        case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC:
            ril::socket::api::dispatchSimTransmitApduBasicReq(context, p);
            break;
        case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:
            ril::socket::api::dispatchSimTransmitApduChannelReq(context, p);
            break;
        case RIL_REQUEST_CHANGE_SIM_PIN:
            ril::socket::api::dispatchSimChangePinReq(context, p);
            break;
        case RIL_REQUEST_CHANGE_SIM_PIN2:
            ril::socket::api::dispatchSimChangePin2Req(context, p);
            break;
        case RIL_REQUEST_ENTER_SIM_PUK:
            ril::socket::api::dispatchSimEnterPukReq(context, p);
            break;
        case RIL_REQUEST_ENTER_SIM_PUK2:
            ril::socket::api::dispatchSimEnterPuk2Req(context, p);
            break;
        case RIL_REQUEST_ENTER_SIM_PIN:
            ril::socket::api::dispatchSimEnterPinReq(context, p);
            break;
        case RIL_REQUEST_ENTER_SIM_PIN2:
            ril::socket::api::dispatchSimEnterPin2Req(context, p);
            break;
        case RIL_REQUEST_QUERY_FACILITY_LOCK:
            ril::socket::api::dispatchSimQueryFacilityLockReq(context, p);
            break;
        case RIL_REQUEST_SET_FACILITY_LOCK:
            ril::socket::api::dispatchSimSetFacilityLockReq(context, p);
            break;
        case RIL_REQUEST_ISIM_AUTHENTICATION:
            ril::socket::api::dispatchSimIsimAuthenticationReq(context, p);
            break;
        case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS:
            // TODO - Find the right UIM module message
            ril::socket::api::dispatchSimStkSendEnvelopeWithStatusReq(context, p);
            break;
        case RIL_REQUEST_SIM_AUTHENTICATION:
            ril::socket::api::dispatchSimAuthenticationReq(context, p);
            break;
            // IMS Requests
        case RIL_IMS_REQUEST_GET_REGISTRATION_STATE:
            ril::socket::api::dispatchImsGetRegistrationState(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_SERVICE_STATUS:
            ril::socket::api::dispatchImsQueryServiceStatus(context, p);
            break;
        case RIL_IMS_REQUEST_REGISTRATION_CHANGE:
            ril::socket::api::dispatchImsRegistrationChange(context, p);
            break;
        case RIL_IMS_REQUEST_SET_SERVICE_STATUS:
            ril::socket::api::dispatchImsSetServiceStatus(context, p);
            break;
        case RIL_IMS_REQUEST_SUPP_SVC_STATUS:
            ril::socket::api::dispatchImsSuppSvcStatus(context, p);
            break;
        case RIL_IMS_REQUEST_GET_IMS_SUB_CONFIG:
            ril::socket::api::dispatchImsGetImsSubConfig(context, p);
            break;
        case RIL_REQUEST_SET_LINK_CAP_FILTER:
            ril::socket::api::dispatchSetLinkCapFilter(context, p);
            break;
        case RIL_REQUEST_SET_LINK_CAP_RPT_CRITERIA:
            ril::socket::api::dispatchSetLinkCapRptCriteria(context, p);
            break;
        case RIL_IMS_REQUEST_DIAL:
            ril::socket::api::dispatchImsDial(context, p);
            break;
        case RIL_IMS_REQUEST_ANSWER:
            ril::socket::api::dispatchImsAnswer(context, p);
            break;
        case RIL_IMS_REQUEST_HANGUP:
            ril::socket::api::dispatchImsHangup(context, p);
            break;
        case RIL_IMS_REQUEST_DTMF:
            ril::socket::api::dispatchImsDtmf(context, p);
            break;
        case RIL_IMS_REQUEST_DTMF_START:
            ril::socket::api::dispatchImsDtmfStart(context, p);
            break;
        case RIL_IMS_REQUEST_DTMF_STOP:
            ril::socket::api::dispatchImsDtmfStop(context, p);
            break;
        case RIL_IMS_REQUEST_MODIFY_CALL_INITIATE:
            ril::socket::api::dispatchImsModifyCallInitiate(context, p);
            break;
        case RIL_IMS_REQUEST_MODIFY_CALL_CONFIRM:
            ril::socket::api::dispatchImsModifyCallConfirm(context, p);
            break;
        case RIL_IMS_REQUEST_CANCEL_MODIFY_CALL:
            ril::socket::api::dispatchImsCancelModifyCall(context, p);
            break;
        case RIL_IMS_REQUEST_ADD_PARTICIPANT:
            ril::socket::api::dispatchImsAddParticipant(context, p);
            break;
        case RIL_IMS_REQUEST_SET_IMS_CONFIG:
            ril::socket::api::dispatchImsSetConfig(context, p);
            break;
        case RIL_IMS_REQUEST_GET_IMS_CONFIG:
            ril::socket::api::dispatchImsGetConfig(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_CALL_FORWARD_STATUS:
            ril::socket::api::dispatchImsQueryCallForwardStatus(context, p);
            break;
        case RIL_IMS_REQUEST_SET_CALL_FORWARD_STATUS:
            ril::socket::api::dispatchImsSetCallForwardStatus(context, p);
            break;
        case RIL_IMS_REQUEST_EXPLICIT_CALL_TRANSFER:
            ril::socket::api::dispatchImsExplicitCallTransfer(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_CLIP:
            ril::socket::api::dispatchImsQueryClip(context, p);
            break;
        case RIL_IMS_REQUEST_GET_CLIR:
            ril::socket::api::dispatchImsGetClir(context, p);
            break;
        case RIL_IMS_REQUEST_SET_CLIR:
            ril::socket::api::dispatchImsSetClir(context, p);
            break;
        case RIL_IMS_REQUEST_DEFLECT_CALL:
            ril::socket::api::dispatchImsDeflectCall(context, p);
            break;
        case RIL_IMS_REQUEST_SEND_UI_TTY_MODE:
            ril::socket::api::dispatchImsSendUiTtyMode(context, p);
            break;
        case RIL_IMS_REQUEST_SET_SUPP_SVC_NOTIFICATION:
            ril::socket::api::dispatchImsSetSuppSvcNotification(context, p);
            break;
        case RIL_IMS_REQUEST_CONFERENCE:
            ril::socket::api::dispatchImsRequestConference(context, p);
            break;
        case RIL_IMS_REQUEST_HOLD:
            ril::socket::api::dispatchImsRequestHold(context, p);
            break;
        case RIL_IMS_REQUEST_RESUME:
            ril::socket::api::dispatchImsRequestResume(context, p);
            break;
        case RIL_IMS_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
            ril::socket::api::dispatchImsExitEmergencyCallbackMode(context, p);
            break;
        case RIL_IMS_REQUEST_SEND_SMS:
            ril::socket::api::imsSendSms(context, p);
            break;
        case RIL_IMS_REQUEST_ACK_SMS:
            ril::socket::api::imsAckSms(context, p);
            break;
        case RIL_IMS_REQUEST_GET_SMS_FORMAT:
            ril::socket::api::imsGetSmsFormat(context, p);
            break;
        case RIL_IMS_REQUEST_SET_CALL_WAITING:
            ril::socket::api::imsSetCallWaiting(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_CALL_WAITING:
            ril::socket::api::imsQueryCallWaiting(context, p);
            break;
        case RIL_REQUEST_GET_RADIO_CAPABILITY:
            ril::socket::api::dispatchGetRadioCapability(context, p);
            break;
        case RIL_REQUEST_SET_RADIO_CAPABILITY:
            ril::socket::api::dispatchSetRadioCapability(context, p);
            break;
        case RIL_REQUEST_CDMA_SEND_SMS:
            ril::socket::api::dispatchSendCdmaSms(context, p);
            break;
        case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE:
            ril::socket::api::dispatchAckCdmaSms(context, p);
            break;
        case RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG:
            ril::socket::api::dispatchCdmaGetBroadcastSmsConfig(context, p);
            break;
        case RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG:
            ril::socket::api::dispatchCdmaSetBroadcastSmsConfig(context, p);
            break;
        case RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION:
            ril::socket::api::dispatchCdmaSmsBroadcastActivation(context, p);
            break;
        case RIL_REQUEST_SET_UNSOLICITED_DATA_RESPONSE_FILTER:
            ril::socket::api::dispatchRegisterDataUnsolIndication(context, p);
            break;
        case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM:
            ril::socket::api::dispatchCdmaWriteSmsToRuim(context, p);
            break;
        case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM:
            ril::socket::api::dispatchCdmaDeleteSmsOnRuim(context, p);
            break;
        case RIL_REQUEST_CDMA_SUBSCRIPTION:
            ril::socket::api::dispatchCdmaSubscription(context, p);
            break;
        case RIL_REQUEST_WRITE_SMS_TO_SIM:
            ril::socket::api::dispatchWriteSmsToSim(context, p);
            break;
        case RIL_REQUEST_DELETE_SMS_ON_SIM:
            ril::socket::api::dispatchDeleteSmsOnSim(context, p);
            break;
        case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS:
            ril::socket::api::reportSmsMemoryStatus(context, p);
            break;
        case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE:
            ril::socket::api::dispatchCdmaSetSubscriptionSource(context, p);
            break;
        case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE:
            ril::socket::api::dispatchCdmaGetSubscriptionSource(context, p);
            break;
        case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE:
            ril::socket::api::dispatchCdmaSetRoamingPreference(context, p);
            break;
        case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE:
            ril::socket::api::dispatchCdmaGetRoamingPreference(context, p);
            break;
        case RIL_REQUEST_GET_DATA_NR_ICON_TYPE:
            ril::socket::api::dispatchGetDataNrIconType(context, p);
            break;
        case RIL_REQUEST_SET_PREFERRED_DATA_MODEM:
            ril::socket::api::dispatchSetPreferredDataModem(context, p);
            break;
        case RIL_REQUEST_START_KEEPALIVE:
            ril::socket::api::dispatchStartKeepAlive(context, p);
            break;
        case RIL_REQUEST_STOP_KEEPALIVE:
            ril::socket::api::dispatchStopKeepAlive(context, p);
            break;
        case RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION:
            ril::socket::api::dispatchCarrierInfoImsiEncryption(context, p);
            break;
        case RIL_IMS_REQUEST_GET_COLR:
            ril::socket::api::dispatchImsGetColr(context, p);
            break;
        case RIL_IMS_REQUEST_SET_COLR:
            ril::socket::api::dispatchImsSetColr(context, p);
            break;
        case RIL_IMS_REQUEST_REGISTER_MULTI_IDENTITY_LINES:
            ril::socket::api::dispatchImsRegisterMultiIdentityLines(context, p);
            break;
        case RIL_IMS_REQUEST_QUERY_VIRTUAL_LINE_INFO:
            ril::socket::api::dispatchImsQueryVirtualLineInfo(context, p);
            break;
        case RIL_REQUEST_SIM_GET_ATR:
            ril::socket::api::dispatchSimGetAtr(context, p);
            break;
        case RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA:
            ril::socket::api::setSignalStrengthReportingCriteria(context, p);
            break;
        case RIL_REQUEST_DISCONNECT:
            ril::socket::api::handleClientDisconnectionRequest(context, p);
            isDisconnectionRequest = true;
            break;
#endif
        default:
            RLOGE("Unsupported request (%d) token %" PRIx64 "x", request, token);
            sendResponse(context, RIL_E_REQUEST_NOT_SUPPORTED, nullptr);
            break;
    }

    return 0;
}

static void processCommandsCallback(int fd, short /*flags*/, void *param) {
    RecordStream* p_rs;
    void* p_record;
    size_t recordlen;
    int ret;
    bool isDisconnectionRequest = false;
    SocketListenParam* p_info = (SocketListenParam*) param;

    assert(fd == p_info->fdCommand);

    p_rs = p_info->p_rs;

    for (;;) {
        /* loop until EAGAIN/EINTR, end of stream, or other error */
        ret = record_stream_get_next(p_rs, &p_record, &recordlen);

        if (ret == 0 && p_record == NULL) {
            /* end-of-stream */
            break;
        } else if (ret < 0) {
            break;
        } else if (ret == 0) {
            processCommandBuffer(p_record, recordlen, p_info->socket_id, isDisconnectionRequest);
        }

        if (isDisconnectionRequest) {
            break;
        }
    }

    if (isDisconnectionRequest || ret == 0 || !(errno == EAGAIN || errno == EINTR)) {
        /* fatal error or end-of-stream or disconnection request from client */
        if (isDisconnectionRequest) {
            RLOGD("Client requested to disconnect. Closing command socket.");
        } else if (ret != 0) {
            RLOGE("error on reading command socket errno: %s", strerror(errno));
        } else {
            RLOGW("EOS. Closing command socket: %s", strerror(errno));
        }

        // TODO: Check if the client will be notified of the disconnection
        close(fd);
        p_info->fdCommand = -1;

        ril_event_del(p_info->commands_event);

        record_stream_free(p_rs);

        /* start listening for new connections again */
        rilEventAddWakeup(&s_listen_event);
    }
}

#if defined(RIL_ENABLE_SOCKET_INTERFACE) || defined(RIL_FOR_MDM_LE)
static void notifyClientServerIsReady() {
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p && p->write(s_callbacks.version) == Marshal::Result::SUCCESS) {
        sendUnsolResponse(RIL_UNSOL_RIL_CONNECTED, p);
    }
}

static void listenCallback(int fd, short /*flags*/, void *param) {
    int ret;
    int fdCommand = -1;
    RecordStream *p_rs;

    SocketListenParam *p_info = (SocketListenParam*) param;

    struct sockaddr_un peeraddr;
    socklen_t socklen = sizeof(peeraddr);

    assert (p_info->fdCommand < 0);
    assert (fd == p_info->fdListen);

    fdCommand = accept(fd, (sockaddr*) &peeraddr, &socklen);

    if (fdCommand < 0 ) {
        RLOGE("Error on accept() errno: %d", errno);
        /* start listening for new connections again */
        rilEventAddWakeup(p_info->listen_event);
        return;
    }

    ret = fcntl(fdCommand, F_SETFL, O_NONBLOCK);

    if (ret < 0) {
        RLOGE("Error setting O_NONBLOCK errno: %d", errno);
    }

    RLOGI("libril: new socket connection");

    p_info->fdCommand = fdCommand;
    p_rs = record_stream_new(p_info->fdCommand, MAX_COMMAND_BYTES);
    p_info->p_rs = p_rs;

    ril_event_set(p_info->commands_event, p_info->fdCommand, 1,
                  p_info->processCommandsCallback, p_info);
    rilEventAddWakeup(p_info->commands_event);

    notifyClientServerIsReady();
}

static int createServerSocket(const std::string& name) {
    RLOGI("Creating socket named %s", name.c_str());

    int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0) {
        RLOGI("Failed to create socket: %s", strerror(errno));
        return fd;
    }

    struct sockaddr_un socketAddress = {};
    socketAddress.sun_family = AF_LOCAL;
    if (strlcpy(socketAddress.sun_path, name.c_str(),
                sizeof(socketAddress.sun_path)) >= sizeof(socketAddress.sun_path))
    {
        return -1;
    }

    unlink(socketAddress.sun_path);
    if (bind(fd, reinterpret_cast<struct sockaddr*>(&socketAddress), sizeof(socketAddress)) < 0) {
        RLOGI("Failed to bind socket: %s", strerror(errno));
        close(fd);
        return -1;
    }
    int rc = chmod(name.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    int errc = errno;
    if (rc < 0) {
        RLOGE("Unable to set permissions on socket with chmod: '%s' (%d)", strerror_r(errc, nullptr, 0), errc);
    }

    return fd;
}

static void startListen(SocketListenParam* socket_listen_p,
        const std::string& socket_path) {
    int fdListen = -1;
    int ret;

    fdListen = createServerSocket(socket_path);
    if (fdListen < 0) {
        RLOGE("Failed to create socket %s", socket_path.c_str());
        return;
    }

    RLOGI("Start to listen on socket %s.", socket_path.c_str());

    ret = listen(fdListen, 4);
    if (ret < 0) {
        RLOGE("Failed to listen on control socket '%d': %s", fdListen, strerror(errno));
        return;
    }

    socket_listen_p->fdListen = fdListen;

    /* note: non-persistent so we can accept only one connection at a time */
    ril_event_set(socket_listen_p->listen_event, fdListen, false,
                  listenCallback, socket_listen_p);

    rilEventAddWakeup(socket_listen_p->listen_event);
}
#endif

#ifndef QMI_RIL_UTF
extern "C" void
RIL_register (const RIL_RadioFunctions *callbacks) {
    if (callbacks == NULL) {
        RLOGE("RIL_register: RIL_RadioFunctions * null");
        return;
    }
    if (callbacks->version < RIL_VERSION_MIN) {
        RLOGE("RIL_register: version %d is to old, min version is %d",
             callbacks->version, RIL_VERSION_MIN);
        return;
    }

    RLOGE("RIL_register: RIL version %d", callbacks->version);

    if (s_registerCalled > 0) {
        RLOGE("RIL_register has been called more than once. "
                "Subsequent call ignored");
        return;
    }

    memcpy(&s_callbacks, callbacks, sizeof (RIL_RadioFunctions));

    s_registerCalled = 1;

    RLOGI("s_registerCalled flag set, %d", s_started);
    // Little self-check

    for (int i = 0; i < (int)NUM_ELEMS(s_commands); i++) {
        assert(i == s_commands[i].requestNumber);
    }

    for (int i = 0; i < (int)NUM_ELEMS(s_unsolResponses); i++) {
        assert(i + RIL_UNSOL_RESPONSE_BASE
                == s_unsolResponses[i].requestNumber);
    }

#ifdef RIL_FOR_MDM_LE
    s_vendorFunctions = &s_callbacks;
#else
    radio::registerService(&s_callbacks, s_commands);
    RLOGI("RILHIDL called registerService");
#endif

#if defined(RIL_ENABLE_SOCKET_INTERFACE) || defined(RIL_FOR_MDM_LE)
    // start listening for connections from clients
    startListen(&s_ril_param_socket, std::string(RIL_SOCKET_PATH) +
            std::to_string(qmi_ril_get_process_instance_id()));
#endif
}
#endif

// Check and remove RequestInfo if its a response and not just ack sent back
static int
checkAndDequeueRequestInfoIfAck(struct RequestInfo *pRI, bool isAck) {
    int ret = 0;
    /* Hook for current context
       pendingRequestsMutextHook refer to &s_pendingRequestsMutex */
    qtimutex::QtiSharedMutex* pendingRequestsMutexHook = &s_pendingRequestsMutex;
    /* pendingRequestsHook refer to &s_pendingRequests */
    RequestInfo ** pendingRequestsHook = &s_pendingRequests;

    if (pRI == NULL) {
        return 0;
    }

    pendingRequestsMutexHook->lock();

    for(RequestInfo **ppCur = pendingRequestsHook
        ; *ppCur != NULL
        ; ppCur = &((*ppCur)->p_next)
    ) {
        if (pRI == *ppCur) {
            ret = 1;
            if (isAck) { // Async ack
                if (pRI->wasAckSent == 1) {
                    RLOGD("Ack was already sent for %s", requestToString(pRI->pCI->requestNumber));
                } else {
                    pRI->wasAckSent = 1;
                }
            } else {
                *ppCur = (*ppCur)->p_next;
            }
            break;
        }
    }

    pendingRequestsMutexHook->unlock();

    return ret;
}

extern "C" void
RIL_onRequestAck(RIL_Token t) {
    RequestInfo *pRI;

    RIL_SOCKET_ID socket_id = RIL_SOCKET_1;

    pRI = (RequestInfo *)t;

    if (!checkAndDequeueRequestInfoIfAck(pRI, true)) {
        RLOGE ("RIL_onRequestAck: invalid RIL_Token");
        return;
    }

    socket_id = pRI->socket_id;

#if VDBG
    RLOGD("Request Ack");
#endif

    appendPrintBuf("Ack [%04d]< %s", pRI->token, requestToString(pRI->pCI->requestNumber));

    if (pRI->cancelled == 0) {
#ifndef RIL_FOR_MDM_LE
        qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock(
                (int) socket_id);
        radioServiceRwlockPtr->lock_shared();

        radio::acknowledgeRequest((int) socket_id, pRI->token);

        radioServiceRwlockPtr->unlock_shared();
#endif
    }
}

#ifndef QMI_RIL_UTF
extern "C" void
RIL_onRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
    RequestInfo *pRI;
    int ret;
    RIL_SOCKET_ID socket_id = RIL_SOCKET_1;

    pRI = (RequestInfo *)t;

    if (!checkAndDequeueRequestInfoIfAck(pRI, false)) {
        RLOGE ("RIL_onRequestComplete: invalid RIL_Token");
        // If a request with the given token can't be found, check
        // if it's a request that was received via the socket and
        // invoke the corresponding response processing callback.
        RIL_onSocketRequestComplete(t, e, response, responselen);
        return;
    }

    socket_id = pRI->socket_id;
#if VDBG
    RLOGD("RequestComplete");
#endif

    if (pRI->local > 0) {
        // Locally issued command...void only!
        // response does not go back up the command socket
        RLOGD("C[locl]< %s", requestToString(pRI->pCI->requestNumber));

        free(pRI);
        return;
    }

    appendPrintBuf("[%04d]< %s",
        pRI->token, requestToString(pRI->pCI->requestNumber));

    if (pRI->cancelled == 0) {
        int responseType;
        if (s_callbacks.version >= 13 && pRI->wasAckSent == 1) {
            // If ack was already sent, then this call is an asynchronous response. So we need to
            // send id indicating that we expect an ack from RIL.java as we acquire wakelock here.
            responseType = RESPONSE_SOLICITED_ACK_EXP;
            grabPartialWakeLock();
        } else {
            responseType = RESPONSE_SOLICITED;
        }

        // there is a response payload, no matter success or not.
#if VDBG
        RLOGE ("Calling responseFunction() for token %d", pRI->token);
#endif

#ifndef RIL_FOR_MDM_LE
        qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock((int) socket_id);
        radioServiceRwlockPtr->lock_shared();

        if (pRI->pCI->responseFunction != NULL)
            ret = pRI->pCI->responseFunction((int) socket_id,
                    responseType, pRI->token, e, response, responselen);

        radioServiceRwlockPtr->unlock_shared();
#endif
    }
    free(pRI);
}

void RIL_onSocketRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responseLen) {
    if (t == nullptr) {
        return;
    }

    SocketResponseCallback onResponseCallback;

    {
        std::lock_guard<qtimutex::QtiSharedMutex> lock(s_pendingSocketRequestsMutex);
        auto requestInfoIter = s_pendingSocketRequests.find(reinterpret_cast<SocketRequestInfo*>(t));
        if (requestInfoIter != s_pendingSocketRequests.end()) {
            if (requestInfoIter->second) {
                onResponseCallback = requestInfoIter->second->responseCallback;
            }
            s_pendingSocketRequests.erase(requestInfoIter);
        }
    }

    if (onResponseCallback) {
        onResponseCallback(e, response, responseLen);
    } else {
        RLOGE("Cannot find response callback for socket request.");
    }
}
#endif

void grabPartialWakeLock() {
#ifndef QMI_RIL_UTF
    if (s_callbacks.version >= 13) {
        s_wakeLockCountMutex.lock();
        //acquire_wake_lock(PARTIAL_WAKE_LOCK, ANDROID_WAKE_LOCK_NAME);
        androidWakeLock.acquire();

        UserCallbackInfo *p_info =
                internalRequestTimedCallback(wakeTimeoutCallback, NULL, &TIMEVAL_WAKE_TIMEOUT);
        if (p_info == NULL) {
            //release_wake_lock(ANDROID_WAKE_LOCK_NAME);
            androidWakeLock.release();
        } else {
            s_wakelock_count++;
            if (s_last_wake_timeout_info != NULL) {
                s_last_wake_timeout_info->userParam = (void *)1;
            }
            s_last_wake_timeout_info = p_info;
        }
        s_wakeLockCountMutex.unlock();
    } else {
        //acquire_wake_lock(PARTIAL_WAKE_LOCK, ANDROID_WAKE_LOCK_NAME);
        androidWakeLock.acquire();
    }
#endif
}

void
releaseWakeLock() {
#ifndef QMI_RIL_UTF
    if (s_callbacks.version >= 13) {
        s_wakeLockCountMutex.lock();

        if (s_wakelock_count > 1) {
            s_wakelock_count--;
        } else {
            s_wakelock_count = 0;
            //release_wake_lock(ANDROID_WAKE_LOCK_NAME);
            androidWakeLock.release();
            if (s_last_wake_timeout_info != NULL) {
                s_last_wake_timeout_info->userParam = (void *)1;
            }
        }

        s_wakeLockCountMutex.unlock();
    } else {
        //release_wake_lock(ANDROID_WAKE_LOCK_NAME);
        androidWakeLock.release();
    }
#endif
}

/**
 * Timer callback to put us back to sleep before the default timeout
 */
static void
wakeTimeoutCallback ([[maybe_unused]] void *param) {
#ifndef QMI_RIL_UTF
    // We're using "param != NULL" as a cancellation mechanism
    if (s_callbacks.version >= 13) {
        if (param == NULL) {
            s_wakeLockCountMutex.lock();
            s_wakelock_count = 0;
            //release_wake_lock(ANDROID_WAKE_LOCK_NAME);
            androidWakeLock.release();
            s_wakeLockCountMutex.unlock();
        }
    } else {
        if (param == NULL) {
            releaseWakeLock();
        }
    }
#endif
}

#ifndef QMI_RIL_UTF
#if defined(ANDROID_MULTI_SIM)
extern "C"
void RIL_onUnsolicitedResponse(int unsolResponse, const void *data, size_t datalen, RIL_SOCKET_ID socket_id)
#else
extern "C"
void RIL_onUnsolicitedResponse(int unsolResponse, const void *data, size_t datalen)
#endif
{
    #if defined(ANDROID_MULTI_SIM)
    sendUnsolicitedResponseOverHidl(unsolResponse, data, datalen, socket_id);
    #else
    sendUnsolicitedResponseOverHidl(unsolResponse, data, datalen);
    #endif

    // TODO: Send unsol over the socket only if there's an active socket client
    sendUnsolicitedResponseOverSocket(unsolResponse, data, datalen);
}

#if defined(ANDROID_MULTI_SIM)
void sendUnsolicitedResponseOverHidl(int unsolResponse, const void *data, size_t datalen, RIL_SOCKET_ID socket_id)
#else
void sendUnsolicitedResponseOverHidl(int unsolResponse, const void *data, size_t datalen)
#endif
{
    int unsolResponseIndex;
    int ret;
    bool shouldScheduleTimeout = false;
    RIL_SOCKET_ID soc_id = RIL_SOCKET_1;

#if defined(ANDROID_MULTI_SIM)
    soc_id = socket_id;
#endif


    if (s_registerCalled == 0) {
        // Ignore RIL_onUnsolicitedResponse before RIL_register
        RLOGW("RIL_onUnsolicitedResponse called before RIL_register");
        return;
    }

    unsolResponseIndex = unsolResponse - RIL_UNSOL_RESPONSE_BASE;

    if ((unsolResponseIndex < 0)
        || (unsolResponseIndex >= (int32_t)NUM_ELEMS(s_unsolResponses))) {
        RLOGE("unsupported unsolicited response code %d", unsolResponse);
        return;
    }

    // Grab a wake lock if needed for this reponse,
    // as we exit we'll either release it immediately
    // or set a timer to release it later.
    switch (s_unsolResponses[unsolResponseIndex].wakeType) {
        case WAKE_PARTIAL:
            grabPartialWakeLock();
            shouldScheduleTimeout = true;
        break;

        case DONT_WAKE:
        default:
            // No wake lock is grabed so don't set timeout
            shouldScheduleTimeout = false;
            break;
    }

    appendPrintBuf("[UNSL]< %s", requestToString(unsolResponse));

    int responseType;
    if (s_callbacks.version >= 13
                && s_unsolResponses[unsolResponseIndex].wakeType == WAKE_PARTIAL) {
        responseType = RESPONSE_UNSOLICITED_ACK_EXP;
    } else {
        responseType = RESPONSE_UNSOLICITED;
    }

#ifndef RIL_FOR_MDM_LE
    qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock((int) soc_id);
    radioServiceRwlockPtr->lock_shared();

    ret = s_unsolResponses[unsolResponseIndex].responseFunction(
            (int) soc_id, responseType, 0, RIL_E_SUCCESS, const_cast<void*>(data),
            datalen);

    radioServiceRwlockPtr->unlock_shared();
#endif

    if (s_callbacks.version < 13) {
        if (shouldScheduleTimeout) {
            UserCallbackInfo *p_info = internalRequestTimedCallback(wakeTimeoutCallback, NULL,
                    &TIMEVAL_WAKE_TIMEOUT);

            if (p_info == NULL) {
                goto error_exit;
            } else {
                s_wakeLockCountMutex.lock();
                // Cancel the previous request
                if (s_last_wake_timeout_info != NULL) {
                    s_last_wake_timeout_info->userParam = (void *)1;
                }
                s_last_wake_timeout_info = p_info;
                s_wakeLockCountMutex.unlock();
            }
        }
    }

#if VDBG
    RLOGI("UNSOLICITED: %s length:%zu", requestToString(unsolResponse),
            datalen);
#endif

    if (ret != 0 && unsolResponse == RIL_UNSOL_NITZ_TIME_RECEIVED) {
        // Unfortunately, NITZ time is not poll/update like everything
        // else in the system. So, if the upstream client isn't connected,
        // keep a copy of the last NITZ response (with receive time noted
        // above) around so we can deliver it when it is connected

        if (s_lastNITZTimeData != NULL) {
            free(s_lastNITZTimeData);
            s_lastNITZTimeData = NULL;
        }

        s_lastNITZTimeData = calloc(datalen, 1);
        if (s_lastNITZTimeData == NULL) {
            RLOGE("Memory allocation failed in RIL_onUnsolicitedResponse");
            goto error_exit;
        }
        s_lastNITZTimeDataSize = datalen;
        memcpy(s_lastNITZTimeData, data, datalen);
    }

    // Normal exit
    return;

error_exit:
    if (shouldScheduleTimeout) {
        releaseWakeLock();
    }
}
#endif

#ifndef QMI_RIL_UTF
RIL_RadioState getRadioState() {
  const char *radio_state_name;
  RIL_RadioState radio_state;

  radio_state = qcril_get_current_radio_state();

  radio_state_name = qcril_log_ril_radio_state_to_str(radio_state);

  RLOGI( "currentState() -> %s(%d)", radio_state_name, radio_state );

  return radio_state;
}

void sendUnsolicitedResponseOverSocket(int unsolResponse, const void *buffer, size_t bufferLength)
{
    if (s_registerCalled == 0) {
        // Ignore sendUnsolicitedResponseOverSocket before RIL_register
        RLOGW("sendUnsolicitedResponseOverSocket called before RIL_register");
        return;
    }

    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if(!p) {
        RLOGE("Unable to allocate memory for payload. Exiting");
        return;
    }

    switch (unsolResponse) {
        case RIL_UNSOL_CELL_INFO_LIST:
            // TODO: Stop writing the array size when Marshal supports the ability to read an entire array
            p->write(static_cast<uint64_t>(bufferLength / sizeof(RIL_CellInfo_v12)));
            p->write(reinterpret_cast<const RIL_CellInfo_v12*>(buffer), bufferLength / sizeof(RIL_CellInfo_v12));
            sendUnsolResponse(unsolResponse, p);
            break;
        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:
            RIL_RadioState state;
            state = getRadioState();
            p->write(static_cast<int>(state));
            sendUnsolResponse(unsolResponse, p);
            break;
        case RIL_UNSOL_MODEM_RESTART:
            if (buffer && bufferLength > 0) {
                p->write(std::string(reinterpret_cast<const char*>(buffer), bufferLength));
            } else {
                p->write(std::string("Modem Restart"));
            }
            sendUnsolResponse(unsolResponse, p);
            break;
        default:
            RLOGE("Unsupported unsolicited response code %d", unsolResponse);
            return;
    }

    appendPrintBuf("[UNSL]< %s", requestToString(unsolResponse));
    RLOGI("UNSOLICITED: %s length: %zu", requestToString(unsolResponse), bufferLength);
}
#endif

/** FIXME generalize this if you track UserCAllbackInfo, clear it
    when the callback occurs
*/
static UserCallbackInfo *
internalRequestTimedCallback (RIL_TimedCallback callback, void *param,
                                const struct timeval *relativeTime)
{
    struct timeval myRelativeTime;
    UserCallbackInfo *p_info;

    p_info = (UserCallbackInfo *) calloc(1, sizeof(UserCallbackInfo));
    if (p_info == NULL) {
        RLOGE("Memory allocation failed in internalRequestTimedCallback");
        return p_info;

    }

    p_info->p_callback = callback;
    p_info->userParam = param;

    if (relativeTime == NULL) {
        /* treat null parameter as a 0 relative time */
        memset (&myRelativeTime, 0, sizeof(myRelativeTime));
    } else {
        /* FIXME I think event_add's tv param is really const anyway */
        memcpy (&myRelativeTime, relativeTime, sizeof(myRelativeTime));
    }

    ril_event_set(&(p_info->event), -1, false, userTimerCallback, p_info);

    ril_timer_add(&(p_info->event), &myRelativeTime);

    triggerEvLoop();
    return p_info;
}

#ifndef QMI_RIL_UTF
extern "C" void
RIL_requestTimedCallback (RIL_TimedCallback callback, void *param,
                                const struct timeval *relativeTime) {
    internalRequestTimedCallback (callback, param, relativeTime);
}
#endif

// Unfortunately, NITZ time is not poll/update like everything
// else in the system. So, if the upstream client isn't connected,
// keep a copy of the last NITZ response (with receive time noted
// above) around so we can deliver it when it is connected
void storeNITZTimeData(const std::string& time) {
    if (s_lastNITZTimeData != NULL) {
        free(s_lastNITZTimeData);
        s_lastNITZTimeData = NULL;
    }

    auto timeLen = time.length();
    s_lastNITZTimeData = calloc(timeLen + 1, 1);
    if (s_lastNITZTimeData == NULL) {
        RLOGE("Memory allocation failed in RIL_onUnsolicitedResponse");
        return;
    }
    s_lastNITZTimeDataSize = timeLen + 1;
    memcpy(s_lastNITZTimeData, time.c_str(), timeLen + 1);
}

const char *
failCauseToString(RIL_Errno e) {
    switch(e) {
        case RIL_E_SUCCESS: return "E_SUCCESS";
        case RIL_E_RADIO_NOT_AVAILABLE: return "E_RADIO_NOT_AVAILABLE";
        case RIL_E_GENERIC_FAILURE: return "E_GENERIC_FAILURE";
        case RIL_E_PASSWORD_INCORRECT: return "E_PASSWORD_INCORRECT";
        case RIL_E_SIM_PIN2: return "E_SIM_PIN2";
        case RIL_E_SIM_PUK2: return "E_SIM_PUK2";
        case RIL_E_REQUEST_NOT_SUPPORTED: return "E_REQUEST_NOT_SUPPORTED";
        case RIL_E_CANCELLED: return "E_CANCELLED";
        case RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL: return "E_OP_NOT_ALLOWED_DURING_VOICE_CALL";
        case RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW: return "E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW";
        case RIL_E_SMS_SEND_FAIL_RETRY: return "E_SMS_SEND_FAIL_RETRY";
        case RIL_E_SIM_ABSENT:return "E_SIM_ABSENT";
        case RIL_E_ILLEGAL_SIM_OR_ME:return "E_ILLEGAL_SIM_OR_ME";
#ifdef FEATURE_MULTIMODE_ANDROID
        case RIL_E_SUBSCRIPTION_NOT_AVAILABLE:return "E_SUBSCRIPTION_NOT_AVAILABLE";
        case RIL_E_MODE_NOT_SUPPORTED:return "E_MODE_NOT_SUPPORTED";
#endif
        case RIL_E_FDN_CHECK_FAILURE: return "E_FDN_CHECK_FAILURE";
        case RIL_E_MISSING_RESOURCE: return "E_MISSING_RESOURCE";
        case RIL_E_NO_SUCH_ELEMENT: return "E_NO_SUCH_ELEMENT";
        case RIL_E_DIAL_MODIFIED_TO_USSD: return "E_DIAL_MODIFIED_TO_USSD";
        case RIL_E_DIAL_MODIFIED_TO_SS: return "E_DIAL_MODIFIED_TO_SS";
        case RIL_E_DIAL_MODIFIED_TO_DIAL: return "E_DIAL_MODIFIED_TO_DIAL";
        case RIL_E_USSD_MODIFIED_TO_DIAL: return "E_USSD_MODIFIED_TO_DIAL";
        case RIL_E_USSD_MODIFIED_TO_SS: return "E_USSD_MODIFIED_TO_SS";
        case RIL_E_USSD_MODIFIED_TO_USSD: return "E_USSD_MODIFIED_TO_USSD";
        case RIL_E_USSD_CS_FALLBACK: return "E_USSD_CS_FALLBACK";
        case RIL_E_SS_MODIFIED_TO_DIAL: return "E_SS_MODIFIED_TO_DIAL";
        case RIL_E_SS_MODIFIED_TO_USSD: return "E_SS_MODIFIED_TO_USSD";
        case RIL_E_SUBSCRIPTION_NOT_SUPPORTED: return "E_SUBSCRIPTION_NOT_SUPPORTED";
        case RIL_E_SS_MODIFIED_TO_SS: return "E_SS_MODIFIED_TO_SS";
        case RIL_E_LCE_NOT_SUPPORTED: return "E_LCE_NOT_SUPPORTED";
        case RIL_E_NO_MEMORY: return "E_NO_MEMORY";
        case RIL_E_INTERNAL_ERR: return "E_INTERNAL_ERR";
        case RIL_E_SYSTEM_ERR: return "E_SYSTEM_ERR";
        case RIL_E_MODEM_ERR: return "E_MODEM_ERR";
        case RIL_E_INVALID_STATE: return "E_INVALID_STATE";
        case RIL_E_NO_RESOURCES: return "E_NO_RESOURCES";
        case RIL_E_SIM_ERR: return "E_SIM_ERR";
        case RIL_E_INVALID_ARGUMENTS: return "E_INVALID_ARGUMENTS";
        case RIL_E_INVALID_SIM_STATE: return "E_INVALID_SIM_STATE";
        case RIL_E_INVALID_MODEM_STATE: return "E_INVALID_MODEM_STATE";
        case RIL_E_INVALID_CALL_ID: return "E_INVALID_CALL_ID";
        case RIL_E_NO_SMS_TO_ACK: return "E_NO_SMS_TO_ACK";
        case RIL_E_NETWORK_ERR: return "E_NETWORK_ERR";
        case RIL_E_REQUEST_RATE_LIMITED: return "E_REQUEST_RATE_LIMITED";
        case RIL_E_SIM_BUSY: return "E_SIM_BUSY";
        case RIL_E_SIM_FULL: return "E_SIM_FULL";
        case RIL_E_NETWORK_REJECT: return "E_NETWORK_REJECT";
        case RIL_E_OPERATION_NOT_ALLOWED: return "E_OPERATION_NOT_ALLOWED";
        case RIL_E_EMPTY_RECORD: return "E_EMPTY_RECORD";
        case RIL_E_INVALID_SMS_FORMAT: return "E_INVALID_SMS_FORMAT";
        case RIL_E_ENCODING_ERR: return "E_ENCODING_ERR";
        case RIL_E_INVALID_SMSC_ADDRESS: return "E_INVALID_SMSC_ADDRESS";
        case RIL_E_NO_SUCH_ENTRY: return "E_NO_SUCH_ENTRY";
        case RIL_E_NETWORK_NOT_READY: return "E_NETWORK_NOT_READY";
        case RIL_E_NOT_PROVISIONED: return "E_NOT_PROVISIONED";
        case RIL_E_NO_SUBSCRIPTION: return "E_NO_SUBSCRIPTION";
        case RIL_E_NO_NETWORK_FOUND: return "E_NO_NETWORK_FOUND";
        case RIL_E_DEVICE_IN_USE: return "E_DEVICE_IN_USE";
        case RIL_E_ABORTED: return "E_ABORTED";
        case RIL_E_INVALID_RESPONSE: return "INVALID_RESPONSE";
        case RIL_E_OEM_ERROR_1: return "E_OEM_ERROR_1";
        case RIL_E_OEM_ERROR_2: return "E_OEM_ERROR_2";
        case RIL_E_OEM_ERROR_3: return "E_OEM_ERROR_3";
        case RIL_E_OEM_ERROR_4: return "E_OEM_ERROR_4";
        case RIL_E_OEM_ERROR_5: return "E_OEM_ERROR_5";
        case RIL_E_OEM_ERROR_6: return "E_OEM_ERROR_6";
        case RIL_E_OEM_ERROR_7: return "E_OEM_ERROR_7";
        case RIL_E_OEM_ERROR_8: return "E_OEM_ERROR_8";
        case RIL_E_OEM_ERROR_9: return "E_OEM_ERROR_9";
        case RIL_E_OEM_ERROR_10: return "E_OEM_ERROR_10";
        case RIL_E_OEM_ERROR_11: return "E_OEM_ERROR_11";
        case RIL_E_OEM_ERROR_12: return "E_OEM_ERROR_12";
        case RIL_E_OEM_ERROR_13: return "E_OEM_ERROR_13";
        case RIL_E_OEM_ERROR_14: return "E_OEM_ERROR_14";
        case RIL_E_OEM_ERROR_15: return "E_OEM_ERROR_15";
        case RIL_E_OEM_ERROR_16: return "E_OEM_ERROR_16";
        case RIL_E_OEM_ERROR_17: return "E_OEM_ERROR_17";
        case RIL_E_OEM_ERROR_18: return "E_OEM_ERROR_18";
        case RIL_E_OEM_ERROR_19: return "E_OEM_ERROR_19";
        case RIL_E_OEM_ERROR_20: return "E_OEM_ERROR_20";
        case RIL_E_OEM_ERROR_21: return "E_OEM_ERROR_21";
        case RIL_E_OEM_ERROR_22: return "E_OEM_ERROR_22";
        case RIL_E_OEM_ERROR_23: return "E_OEM_ERROR_23";
        case RIL_E_OEM_ERROR_24: return "E_OEM_ERROR_24";
        case RIL_E_OEM_ERROR_25: return "E_OEM_ERROR_25";
        default: return "<unknown error>";
    }
}

const char *
radioStateToString(RIL_RadioState s) {
    switch(s) {
        case RADIO_STATE_OFF: return "RADIO_OFF";
        case RADIO_STATE_UNAVAILABLE: return "RADIO_UNAVAILABLE";
        case RADIO_STATE_ON:return"RADIO_ON";
        default: return "<unknown state>";
    }
}

const char *
callStateToString(RIL_CallState s) {
    switch(s) {
        case RIL_CALL_ACTIVE : return "ACTIVE";
        case RIL_CALL_HOLDING: return "HOLDING";
        case RIL_CALL_DIALING: return "DIALING";
        case RIL_CALL_ALERTING: return "ALERTING";
        case RIL_CALL_INCOMING: return "INCOMING";
        case RIL_CALL_WAITING: return "WAITING";
        default: return "<unknown state>";
    }
}

const char *
requestToString(int request) {
/*
 cat libs/telephony/ril_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),[^,]+,([^}]+).+/case RIL_\1: return "\1";/'


 cat libs/telephony/ril_unsol_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),([^}]+).+/case RIL_\1: return "\1";/'

*/
    switch(request) {
        case RIL_REQUEST_GET_CURRENT_CALLS: return "GET_CURRENT_CALLS";
        case RIL_REQUEST_DIAL: return "DIAL";
        case RIL_REQUEST_HANGUP: return "HANGUP";
        case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND: return "HANGUP_WAITING_OR_BACKGROUND";
        case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND: return "HANGUP_FOREGROUND_RESUME_BACKGROUND";
        case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE: return "SWITCH_WAITING_OR_HOLDING_AND_ACTIVE";
        case RIL_REQUEST_CONFERENCE: return "CONFERENCE";
        case RIL_REQUEST_UDUB: return "UDUB";
        case RIL_REQUEST_LAST_CALL_FAIL_CAUSE: return "LAST_CALL_FAIL_CAUSE";
        case RIL_REQUEST_SIGNAL_STRENGTH: return "SIGNAL_STRENGTH";
        case RIL_REQUEST_VOICE_REGISTRATION_STATE: return "VOICE_REGISTRATION_STATE";
        case RIL_REQUEST_DATA_REGISTRATION_STATE: return "DATA_REGISTRATION_STATE";
        case RIL_REQUEST_OPERATOR: return "OPERATOR";
        case RIL_REQUEST_RADIO_POWER: return "RADIO_POWER";
        case RIL_REQUEST_DTMF: return "DTMF";
        case RIL_REQUEST_SEND_SMS: return "SEND_SMS";
        case RIL_REQUEST_SEND_SMS_EXPECT_MORE: return "SEND_SMS_EXPECT_MORE";
        case RIL_REQUEST_SETUP_DATA_CALL: return "SETUP_DATA_CALL";
        case RIL_REQUEST_SEND_USSD: return "SEND_USSD";
        case RIL_REQUEST_CANCEL_USSD: return "CANCEL_USSD";
        case RIL_REQUEST_GET_CLIR: return "GET_CLIR";
        case RIL_REQUEST_SET_CLIR: return "SET_CLIR";
        case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS: return "QUERY_CALL_FORWARD_STATUS";
        case RIL_REQUEST_SET_CALL_FORWARD: return "SET_CALL_FORWARD";
        case RIL_REQUEST_QUERY_CALL_WAITING: return "QUERY_CALL_WAITING";
        case RIL_REQUEST_SET_CALL_WAITING: return "SET_CALL_WAITING";
        case RIL_REQUEST_SMS_ACKNOWLEDGE: return "SMS_ACKNOWLEDGE";
        case RIL_REQUEST_GET_IMEI: return "GET_IMEI";
        case RIL_REQUEST_GET_IMEISV: return "GET_IMEISV";
        case RIL_REQUEST_ANSWER: return "ANSWER";
        case RIL_REQUEST_DEACTIVATE_DATA_CALL: return "DEACTIVATE_DATA_CALL";
        case RIL_REQUEST_QUERY_FACILITY_LOCK: return "QUERY_FACILITY_LOCK";
        case RIL_REQUEST_SET_FACILITY_LOCK: return "SET_FACILITY_LOCK";
        case RIL_REQUEST_CHANGE_BARRING_PASSWORD: return "CHANGE_BARRING_PASSWORD";
        case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: return "QUERY_NETWORK_SELECTION_MODE";
        case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC: return "SET_NETWORK_SELECTION_AUTOMATIC";
        case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL: return "SET_NETWORK_SELECTION_MANUAL";
        case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS: return "QUERY_AVAILABLE_NETWORKS";
        case RIL_REQUEST_DTMF_START: return "DTMF_START";
        case RIL_REQUEST_DTMF_STOP: return "DTMF_STOP";
        case RIL_REQUEST_BASEBAND_VERSION: return "BASEBAND_VERSION";
        case RIL_REQUEST_SEPARATE_CONNECTION: return "SEPARATE_CONNECTION";
        case RIL_REQUEST_SET_MUTE: return "SET_MUTE";
        case RIL_REQUEST_GET_MUTE: return "GET_MUTE";
        case RIL_REQUEST_QUERY_CLIP: return "QUERY_CLIP";
        case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE: return "LAST_DATA_CALL_FAIL_CAUSE";
        case RIL_REQUEST_DATA_CALL_LIST: return "DATA_CALL_LIST";
        case RIL_REQUEST_RESET_RADIO: return "RESET_RADIO";
        case RIL_REQUEST_OEM_HOOK_RAW: return "OEM_HOOK_RAW";
        case RIL_REQUEST_OEM_HOOK_STRINGS: return "OEM_HOOK_STRINGS";
        case RIL_REQUEST_SCREEN_STATE: return "SCREEN_STATE";
        case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION: return "SET_SUPP_SVC_NOTIFICATION";
        case RIL_REQUEST_WRITE_SMS_TO_SIM: return "WRITE_SMS_TO_SIM";
        case RIL_REQUEST_DELETE_SMS_ON_SIM: return "DELETE_SMS_ON_SIM";
        case RIL_REQUEST_SET_BAND_MODE: return "SET_BAND_MODE";
        case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE: return "QUERY_AVAILABLE_BAND_MODE";
        case RIL_REQUEST_STK_GET_PROFILE: return "STK_GET_PROFILE";
        case RIL_REQUEST_STK_SET_PROFILE: return "STK_SET_PROFILE";
        case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND: return "STK_SEND_ENVELOPE_COMMAND";
        case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE: return "STK_SEND_TERMINAL_RESPONSE";
        case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM: return "STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM";
        case RIL_REQUEST_EXPLICIT_CALL_TRANSFER: return "EXPLICIT_CALL_TRANSFER";
        case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE: return "SET_PREFERRED_NETWORK_TYPE";
        case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE: return "GET_PREFERRED_NETWORK_TYPE";
        case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS: return "GET_NEIGHBORING_CELL_IDS";
        case RIL_REQUEST_SET_LOCATION_UPDATES: return "SET_LOCATION_UPDATES";
        case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE: return "CDMA_SET_SUBSCRIPTION_SOURCE";
        case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE: return "CDMA_SET_ROAMING_PREFERENCE";
        case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE: return "CDMA_QUERY_ROAMING_PREFERENCE";
        case RIL_REQUEST_SET_TTY_MODE: return "SET_TTY_MODE";
        case RIL_REQUEST_QUERY_TTY_MODE: return "QUERY_TTY_MODE";
        case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE: return "CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE";
        case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE: return "CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE";
        case RIL_REQUEST_CDMA_FLASH: return "CDMA_FLASH";
        case RIL_REQUEST_CDMA_BURST_DTMF: return "CDMA_BURST_DTMF";
        case RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY: return "CDMA_VALIDATE_AND_WRITE_AKEY";
        case RIL_REQUEST_CDMA_SEND_SMS: return "CDMA_SEND_SMS";
        case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE: return "CDMA_SMS_ACKNOWLEDGE";
        case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG: return "GSM_GET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG: return "GSM_SET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION: return "GSM_SMS_BROADCAST_ACTIVATION";
        case RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG: return "CDMA_GET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG: return "CDMA_SET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION: return "CDMA_SMS_BROADCAST_ACTIVATION";
        case RIL_REQUEST_CDMA_SUBSCRIPTION: return "CDMA_SUBSCRIPTION";
        case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM: return "CDMA_WRITE_SMS_TO_RUIM";
        case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM: return "CDMA_DELETE_SMS_ON_RUIM";
        case RIL_REQUEST_DEVICE_IDENTITY: return "DEVICE_IDENTITY";
        case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE: return "EXIT_EMERGENCY_CALLBACK_MODE";
        case RIL_REQUEST_GET_SMSC_ADDRESS: return "GET_SMSC_ADDRESS";
        case RIL_REQUEST_SET_SMSC_ADDRESS: return "SET_SMSC_ADDRESS";
        case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS: return "REPORT_SMS_MEMORY_STATUS";
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING: return "REPORT_STK_SERVICE_IS_RUNNING";
        case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE: return "CDMA_GET_SUBSCRIPTION_SOURCE";
        case RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU: return "ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU";
        case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS: return "STK_SEND_ENVELOPE_WITH_STATUS";
        case RIL_REQUEST_VOICE_RADIO_TECH: return "VOICE_RADIO_TECH";
        case RIL_REQUEST_GET_CELL_INFO_LIST: return "GET_CELL_INFO_LIST";
        case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: return "SET_UNSOL_CELL_INFO_LIST_RATE";
        case RIL_REQUEST_SET_INITIAL_ATTACH_APN: return "SET_INITIAL_ATTACH_APN";
        case RIL_REQUEST_IMS_REGISTRATION_STATE: return "IMS_REGISTRATION_STATE";
        case RIL_REQUEST_IMS_SEND_SMS: return "IMS_SEND_SMS";
        case RIL_REQUEST_NV_READ_ITEM: return "NV_READ_ITEM";
        case RIL_REQUEST_NV_WRITE_ITEM: return "NV_WRITE_ITEM";
        case RIL_REQUEST_NV_WRITE_CDMA_PRL: return "NV_WRITE_CDMA_PRL";
        case RIL_REQUEST_NV_RESET_CONFIG: return "NV_RESET_CONFIG";
        case RIL_REQUEST_SET_UICC_SUBSCRIPTION: return "SET_UICC_SUBSCRIPTION";
        case RIL_REQUEST_ALLOW_DATA: return "ALLOW_DATA";
        case RIL_REQUEST_GET_HARDWARE_CONFIG: return "GET_HARDWARE_CONFIG";
        case RIL_REQUEST_GET_DC_RT_INFO: return "GET_DC_RT_INFO";
        case RIL_REQUEST_SET_DC_RT_INFO_RATE: return "SET_DC_RT_INFO_RATE";
        case RIL_REQUEST_SET_DATA_PROFILE: return "SET_DATA_PROFILE";
        case RIL_REQUEST_DATA_DUMP: return "RIL_REQUEST_DATA_DUMP";
        case RIL_REQUEST_SHUTDOWN: return "SHUTDOWN";
        case RIL_REQUEST_GET_RADIO_CAPABILITY: return "GET_RADIO_CAPABILITY";
        case RIL_REQUEST_SET_RADIO_CAPABILITY: return "SET_RADIO_CAPABILITY";
        case RIL_REQUEST_START_LCE: return "START_LCE";
        case RIL_REQUEST_STOP_LCE: return "STOP_LCE";
        case RIL_REQUEST_PULL_LCEDATA: return "PULL_LCEDATA";
        case RIL_REQUEST_GET_ACTIVITY_INFO: return "GET_ACTIVITY_INFO";
        case RIL_REQUEST_SET_CARRIER_RESTRICTIONS: return "SET_CARRIER_RESTRICTIONS";
        case RIL_REQUEST_GET_CARRIER_RESTRICTIONS: return "GET_CARRIER_RESTRICTIONS";
        case RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION: return "SET_CARRIER_INFO_IMSI_ENCRYPTION";
        case RIL_RESPONSE_ACKNOWLEDGEMENT: return "RESPONSE_ACKNOWLEDGEMENT";
        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: return "UNSOL_RESPONSE_RADIO_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: return "UNSOL_RESPONSE_CALL_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: return "UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_NEW_SMS: return "UNSOL_RESPONSE_NEW_SMS";
        case RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: return "UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT";
        case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: return "UNSOL_RESPONSE_NEW_SMS_ON_SIM";
        case RIL_UNSOL_ON_USSD: return "UNSOL_ON_USSD";
        case RIL_UNSOL_ON_USSD_REQUEST: return "UNSOL_ON_USSD_REQUEST";
        case RIL_UNSOL_NITZ_TIME_RECEIVED: return "UNSOL_NITZ_TIME_RECEIVED";
        case RIL_UNSOL_SIGNAL_STRENGTH: return "UNSOL_SIGNAL_STRENGTH";
        case RIL_UNSOL_DATA_CALL_LIST_CHANGED: return "UNSOL_DATA_CALL_LIST_CHANGED";
        case RIL_UNSOL_SUPP_SVC_NOTIFICATION: return "UNSOL_SUPP_SVC_NOTIFICATION";
        case RIL_UNSOL_SIM_SMS_STORAGE_FULL: return "UNSOL_SIM_SMS_STORAGE_FULL";
        case RIL_UNSOL_CALL_RING: return "UNSOL_CALL_RING";
        case RIL_UNSOL_RESPONSE_CDMA_NEW_SMS: return "UNSOL_RESPONSE_CDMA_NEW_SMS";
        case RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS: return "UNSOL_RESPONSE_NEW_BROADCAST_SMS";
        case RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL: return "UNSOL_CDMA_RUIM_SMS_STORAGE_FULL";
        case RIL_UNSOL_RESTRICTED_STATE_CHANGED: return "UNSOL_RESTRICTED_STATE_CHANGED";
        case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE: return "UNSOL_ENTER_EMERGENCY_CALLBACK_MODE";
        case RIL_UNSOL_CDMA_CALL_WAITING: return "UNSOL_CDMA_CALL_WAITING";
        case RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: return "UNSOL_CDMA_OTA_PROVISION_STATUS";
        case RIL_UNSOL_CDMA_INFO_REC: return "UNSOL_CDMA_INFO_REC";
        case RIL_UNSOL_OEM_HOOK_RAW: return "UNSOL_OEM_HOOK_RAW";
        case RIL_UNSOL_RINGBACK_TONE: return "UNSOL_RINGBACK_TONE";
        case RIL_UNSOL_RESEND_INCALL_MUTE: return "UNSOL_RESEND_INCALL_MUTE";
        case RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: return "UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED";
        case RIL_UNSOL_CDMA_PRL_CHANGED: return "UNSOL_CDMA_PRL_CHANGED";
        case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE: return "UNSOL_EXIT_EMERGENCY_CALLBACK_MODE";
        case RIL_UNSOL_RIL_CONNECTED: return "UNSOL_RIL_CONNECTED";
        case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: return "UNSOL_VOICE_RADIO_TECH_CHANGED";
        case RIL_UNSOL_CELL_INFO_LIST: return "UNSOL_CELL_INFO_LIST";
        case RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED: return "UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED";
        case RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED: return "UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED";
        case RIL_UNSOL_SRVCC_STATE_NOTIFY: return "UNSOL_SRVCC_STATE_NOTIFY";
        case RIL_UNSOL_HARDWARE_CONFIG_CHANGED: return "UNSOL_HARDWARE_CONFIG_CHANGED";
        case RIL_UNSOL_DC_RT_INFO_CHANGED: return "UNSOL_DC_RT_INFO_CHANGED";
        case RIL_UNSOL_RADIO_CAPABILITY: return "UNSOL_RADIO_CAPABILITY";
        case RIL_UNSOL_MODEM_RESTART: return "UNSOL_MODEM_RESTART";
        case RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION: return "UNSOL_CARRIER_INFO_IMSI_ENCRYPTION";
        case RIL_UNSOL_ON_SS: return "UNSOL_ON_SS";
        case RIL_UNSOL_STK_CC_ALPHA_NOTIFY: return "UNSOL_STK_CC_ALPHA_NOTIFY";
        case RIL_UNSOL_LCEDATA_RECV: return "UNSOL_LCEDATA_RECV";
        case RIL_UNSOL_DATA_NR_ICON_TYPE: return "UNSOL_DATA_NR_ICON_TYPE";
        case RIL_REQUEST_GET_DATA_NR_ICON_TYPE: return "GET_DATA_NR_ICON_TYPE";
        case RIL_UNSOL_PCO_DATA: return "UNSOL_PCO_DATA";
        case RIL_REQUEST_SET_LINK_CAP_FILTER: return "RIL_REQUEST_SET_LINK_CAP_FILTER";
        case RIL_REQUEST_SET_LINK_CAP_RPT_CRITERIA: return "RIL_REQUEST_SET_LINK_CAP_RPT_CRITERIA";
        case RIL_REQUEST_SET_PREFERRED_DATA_MODEM: return "RIL_REQUEST_SET_PREFERRED_DATA_MODEM";
        case RIL_REQUEST_START_KEEPALIVE: return "RIL_REQUEST_START_KEEP_ALIVE";
        case RIL_UNSOL_KEEPALIVE_STATUS: return "RIL_UNSOL_KEEPALIVE_STATUS";
        case RIL_REQUEST_STOP_KEEPALIVE: return "RIL_REQUEST_STOP_KEEPALIVE";
        case RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG: return "RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG";
        case RIL_REQUEST_SET_UNSOLICITED_DATA_RESPONSE_FILTER: return "RIL_REQUEST_SET_UNSOLICITED_DATA_RESPONSE_FILTER";
        default: return "<unknown request>";
    }
}

} /* namespace android */

int sendResponse(std::shared_ptr<SocketRequestContext> context, RIL_Errno errorCode, std::shared_ptr<Marshal> p) {
    if (context == nullptr) {
        return -1;
    }

    Marshal responseHeader;
    responseHeader.write(static_cast<uint32_t>(RESPONSE_SOLICITED));
    responseHeader.write(context->getRequestToken());
    responseHeader.write(static_cast<uint32_t>(errorCode));

    if (p != nullptr) {
        std::string rh = responseHeader.toString();
        rh.resize(responseHeader.dataSize());

        std::string rd = p->toString();
        rd.resize(p->dataSize());

        Marshal m;
        m.setData(rh + rd);
        return android::sendResponseRaw(m.data(), m.dataSize());
    } else {
        return android::sendResponseRaw(responseHeader.data(), responseHeader.dataSize());
    }
}

int sendUnsolResponse(int unsolResponse, std::shared_ptr<Marshal> p) {
    Marshal unsolResponseHeader;
    unsolResponseHeader.write(static_cast<uint32_t>(RESPONSE_UNSOLICITED));
    unsolResponseHeader.write(static_cast<int32_t>(unsolResponse));

    if (p != nullptr) {
        std::string urh = unsolResponseHeader.toString();
        urh.resize(unsolResponseHeader.dataSize());

        std::string urd = p->toString();
        urd.resize(p->dataSize());

        Marshal m;
        m.setData(urh + urd);
        return android::sendResponseRaw(m.data(), m.dataSize());
    } else {
        return android::sendResponseRaw(unsolResponseHeader.data(), unsolResponseHeader.dataSize());
    }
}

extern void QCRIL_setProcessInstanceId(int clientId);
extern "C" void RIL_setProcessInstanceId(int clientId) {
  QCRIL_setProcessInstanceId(clientId);
}
