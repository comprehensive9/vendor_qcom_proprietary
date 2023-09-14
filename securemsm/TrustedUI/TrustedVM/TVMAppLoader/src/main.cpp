/*========================================================================
Copyright (c) 2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/
#include "object.h"
#include "impl_base.hpp"
#include "proxy_base.hpp"
#include "TUIUtils.h"
#include "fdwrapper.h"
#include "heap.h"
#include "minkipc.h"
#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <semaphore.h>
#include <unistd.h>
#include "TUILog.h"
#include "IRegisterApp.h"
#include "CTVMAppLoader.hpp"

#define LOG_TAG "TVMAppLoader_SERVICE"
#define WAKELOCK_STR "tvmapploader_crash"

static sem_t sWaitForever;
static MinkIPC *client;

Object tvmAppOpener = Object_NULL;

static int32_t _acquireWakeLock() {
  int32_t ret = 0;
  const char * lockFile = "/sys/power/wake_lock";
  int lockFd = -1;
  errno = 0;

  lockFd = open(lockFile, O_WRONLY | O_APPEND);
  if (lockFd < 0) {
    TUILOGE("%s: Unable to open wake lock file: %s", __func__, strerror(errno));
    ret = -1;
    goto end;
  }

  ret = write(lockFd, WAKELOCK_STR, strlen(WAKELOCK_STR));
  if (ret < 0) {
    TUILOGE("%s: Unable to write to wake lock file: %s", __func__, strerror(errno));
    ret =  -1;
    goto end;
  }

  TUILOGE("%s wake-lock acquired successfully", __func__);
end:
  if (lockFd > 0) {
    close(lockFd);
  }

  return ret;
}

static int32_t _releaseWakeLock() {
  int32_t ret = 0;
  const char * unlockFile = "/sys/power/wake_unlock";
  int unlockFd = -1;
  errno = 0;
  unlockFd = open(unlockFile, O_WRONLY | O_APPEND);
  if (unlockFd < 0) {
    TUILOGE("%s: Unable to open wake unlock file: %s", __func__, strerror(errno));
    ret = -1;
    goto end;
  }
  ret = write(unlockFd, WAKELOCK_STR, strlen(WAKELOCK_STR));
  if (ret < 0) {
    TUILOGE("%s: Unable to write to wake unlock file: %s", __func__, strerror(errno));
    ret = -1;
    goto end;
  }
  TUILOGE("%s wake-lock released successfully", __func__);

end:
  if (unlockFd > 0) {
    close(unlockFd);
  }

  return ret;
}

void releaseAllObjects() {
  Object_ASSIGN_NULL(tvmAppOpener);
  if (client != NULL ) {
    TUILOGE("%s:%d Release connection with letzd ", __func__, __LINE__);
    MinkIPC_release(client);
    client = NULL;
   }
}

static void SignalHandler(int signalNum) {
  uint8_t  secret[] = "tvmapploader";
  int32_t ret = 0;
  TUILOGE("%s::%d - !!! TVMAppLoader process crashed, received signal:%d !!!",  __func__,
     __LINE__, signalNum);

  _acquireWakeLock();

  ret = CTVMAppLoader_cleanup();
  if(ret)
  {
    TUILOGE("cleanup failed, ret = %d", ret);
  }

  if (!Object_isNull(tvmAppOpener)) {
    ret = IRegisterApp_deregisterApp(tvmAppOpener, CTVMAppLoader_UID, secret, sizeof(secret)/sizeof(uint8_t));
    TUILOGE("%s:%u Deregistering TVM App Loader ret=%d", __func__, __LINE__, ret);
  }

  releaseAllObjects();
  sem_post(&sWaitForever);
  // terminate program
  exit(signalNum);
}

static void registerSignalHandler() {
  struct sigaction action;
  //Register signal handler
  memset(&action, 0, sizeof(action));
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_RESTART;
  action.sa_handler = SignalHandler;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGABRT, &action, NULL);
  sigaction(SIGSEGV, &action, NULL);
}

int32_t main() {
  int32_t ret = 0;
  Object tvmAppLoaderObject = Object_NULL;
  uint8_t  secret[] = "tvmapploader";
  client = MinkIPC_connect("/dev/socket/letzd", &tvmAppOpener);

  sem_init(&sWaitForever, 0, 0);

  if (!client) {
    TUILOGE("Failed to create MinkIPC client");
    ret = Object_ERROR_BADOBJ;
    goto end;
  }
  else if  (Object_isNull(tvmAppOpener)) {
    TUILOGE("Failed to get TVM Opener");
    ret = Object_ERROR_BADOBJ;
    goto end;
  }

  ret = CTVMAppLoader_open(&tvmAppLoaderObject);
  if ((ret != 0) || Object_isNull(tvmAppLoaderObject)) {
    TUILOGE("Failed to get TVM App Loader object");
    ret = Object_ERROR_BADOBJ;
    goto end;
  }

  TUILOGD("registering TVM App Loader");
  ret = IRegisterApp_registerApp(tvmAppOpener, CTVMAppLoader_UID, tvmAppLoaderObject, secret, sizeof(secret)/sizeof(uint8_t));

  if (ret < 0) {
    TUILOGE("%s: Failed to register tvmAppLoaderObject!", __func__);
    Object_ASSIGN_NULL(tvmAppLoaderObject);
    ret = Object_ERROR_BADOBJ;
    goto end;
  }

  registerSignalHandler();
  _releaseWakeLock();

  TUILOGE("%s Registered TVM Apploader", __func__);

  sem_wait(&sWaitForever);

end:
  releaseAllObjects();
  _releaseWakeLock();
  return ret;
}
