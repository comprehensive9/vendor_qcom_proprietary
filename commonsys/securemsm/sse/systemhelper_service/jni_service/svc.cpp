/*
 * Copyright (c) 2012-2013, 2018-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define MAIN_C
#include "SystemEvent.h"
#include "SystemResource.h"
#include <android-base/logging.h>
#include <common_log.h>
#include <errno.h>
#include <hidl/HidlTransportSupport.h>
#include <iostream>
#include <jni.h>
#include <mutex>
#include <queue>
#include <semaphore.h>
#include <string>
#include <unistd.h>

using namespace android;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;
using vendor::qti::hardware::systemhelper::V1_0::implementation::SystemEvent;
using vendor::qti::hardware::systemhelper::V1_0::implementation::SystemResource;
using vendor::qti::hardware::systemhelper::V1_0::SystemResourceType;
using namespace std;

std::mutex resourceLock;
static queue<string> msg_q;
static queue<sem_t *> request_q;
sem_t sem;

extern "C" {

#include "com_qualcomm_qti_services_systemhelper_SysHelperService.h"

extern "C" jint
Java_com_qualcomm_qti_services_systemhelper_SysHelperService_init(
    JNIEnv *env, jclass /*cls*/) {

  jint ret = -1;
  configureRpcThreadpool(1, false /*willJoinThreadpool*/);

  SystemEvent *sysEvent = new SystemEvent();
  LOGD("SystemEvent interface registering.");
  auto status = sysEvent->registerAsService();
  if (status != android::OK) {
    LOGE("Could not register SystemEvent 1.0 interface");
  } else {
    SystemResource *sysResource = new SystemResource();
    LOGD("SystemResource interface registering.");
    auto status1 = sysResource->registerAsService();
    if (status1 != android::OK) {
      LOGE("Could not register SystemResource 1.0 interface");
    } else {
      ret = 0;
      LOGD("Successfully registered for both SystemEvent and SystemResource "
           "1.0 interfaces");
    }
  }
  sem_init(&sem, 0, 0);
  return ret;
}

extern "C" jstring
Java_com_qualcomm_qti_services_systemhelper_SysHelperService_getMessage(
    JNIEnv *env, jclass /*cls*/) {
  do {
    sem_wait(&sem);
  } while(msg_q.empty());

  string str = msg_q.front();
  msg_q.pop();

  LOGD("%s: System Helper service processing req :%s", __func__, str.c_str());
  jstring jresource = env->NewStringUTF(str.c_str());
  return jresource;
}

extern "C" void
Java_com_qualcomm_qti_services_systemhelper_SysHelperService_sendConfirmation(
            JNIEnv *env, jclass /*cls*/) {

    LOGD("%s: ++ %d request_q.size: %d", __func__, __LINE__, request_q.size());
    std::lock_guard<std::mutex> lock(resourceLock);
    //request is done processing
    sem_t * s  = request_q.front();
    request_q.pop();
    LOGD("%s: %d Pending requests: %d", __func__, __LINE__, request_q.size());
    sem_post(s);
    if (s) {
        LOGD("%s: %d free sem", __func__, __LINE__);
        free(s);
    }
}

void processRequest(string resource) {
    sem_t* waitForRequest = (sem_t*) malloc(sizeof(sem_t));
    LOGD("%s: ++ %d request for %s", __func__, __LINE__,  resource.c_str());
    if (waitForRequest == nullptr) {
           LOGE("%s:  %d Out of system heap !!! ", __func__, __LINE__);
           LOGE("%s:  %d Cannot process request : %s ", __func__, __LINE__, resource.c_str());
           return;
    }

    {
        std::lock_guard<std::mutex> lock(resourceLock);
        msg_q.push(resource);
        sem_init(waitForRequest, 0, 0);
        request_q.push(waitForRequest);
        sem_post(&sem);
    }

    //Wait until request has been completed or 50ms
    struct timespec ts;
    //Initialize
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &ts);

    ts.tv_nsec += 50000000;

    int32_t ret = sem_timedwait(waitForRequest, &ts);
    LOGD("%s: wait returned ret: %d (%s)", __func__, ret, strerror(errno));
}

void acquireWakeLock() {
    processRequest("WL_AQUIRE");
}

void releaseWakeLock() {
    processRequest("WL_RELEASE");
}

void lockRotation(jboolean lock) {
    string str = (lock) ? "ROT_LOCK" : "ROT_UNLOCK";
    processRequest(str);
}

extern "C" jint
Java_com_qualcomm_qti_services_systemhelper_SysHelperService_terminate(
    JNIEnv * /*env*/, jclass /*cls*/) {
  return 0;
}
}
