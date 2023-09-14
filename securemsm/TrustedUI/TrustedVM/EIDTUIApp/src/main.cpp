/*========================================================================
Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/
#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>
#include "object.h"
#include "impl_base.hpp"
#include "proxy_base.hpp"
#include "IOpener_invoke.hpp"
#include "CTrustedUIApp.hpp" //definition of CTrustedUIApp_open
#include "ITrustedUIApp.hpp" //definitions of initSession, deleteSession
#include "TUIUtils.h"
#include "fdwrapper.h"
#include "heap.h"
#include "minkipc.h"
#include "TUILog.h"
#include "IRegisterApp.h"

#define LOG_TAG "EIDTUIAPP_SERVICE"

#define EIDTUI_UID 1235

#define WAKELOCK_STR "eidtuiapp_crash"

static sem_t sWaitForever;
static  MinkIPC *client;
static Object tuiAppObject = Object_NULL;
static Object letzdOpener = Object_NULL;

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
  ret = 0;
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
  Object_ASSIGN_NULL(letzdOpener);
  Object_ASSIGN_NULL(tuiAppObject);
  if (client != NULL ) {
    TUILOGE("%s:%d Release connection with letzd ", __func__, __LINE__);
    MinkIPC_release(client);
    client = NULL;
  }
}

static void SignalHandler(int signalNum) {
  uint8_t secret[] = "password";
  TUILOGE("%s::%d - !!! TUIApp process crashed, received signal:%d !!!",  __func__,
      __LINE__, signalNum);

  _acquireWakeLock();

  if (!Object_isNull(tuiAppObject)) {
    TrustedUIApp* app = new TrustedUIApp(tuiAppObject);
    if (app) {
      TUILOGE("%s:%u Abort current session", __func__, __LINE__);
      app->stopSession();
      app->deleteSession();
    }
  }

  if (!Object_isNull(letzdOpener)) {
    int32_t ret = IRegisterApp_deregisterApp(letzdOpener, EIDTUI_UID, secret, sizeof(secret)/sizeof(uint8_t));
    TUILOGE("%s:%u Deregistering app ret=%d", __func__, __LINE__, ret);
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

/**
 * @brief This class provides the interface to opener object for TUI APP Service
 */
class CTUIAppOpener : public IOpenerImplBase {
public:
  CTUIAppOpener() {}
  virtual ~CTUIAppOpener() {}
  int32_t open(uint32_t id_val, Object *obj_ref);
};

/*
 * @brief Get TUI App Object. Creates a new app object per TUI session
 * @return Object_OK if success
 * @return Object_ERROR_BADOBJ in case of failure to create an app obj
 * @return ERROR_NOT_FOUND if no spp was regitered with the given UID
 * */
int32_t CTUIAppOpener::open(uint32_t id_val, Object *obj_ref) {
  int32_t ret = Object_OK;
  Object objOut = Object_NULL;
  TUILOGD("%s:%u id=%u", __func__, __LINE__, id_val);
  switch (id_val) {
  case CTrustedUIApp_UID:
    /* Create a new TUI app object for every session*/
    ret = CTrustedUIApp_open(obj_ref);
    if ((ret != 0) || Object_isNull(*obj_ref)) {
      TUILOGE("Failed to get TUI object from tui  service");
      return Object_ERROR_BADOBJ;
    }
    return ret;
  default:
    break;
  }
  TUILOGE("%s: No object found with UID: %d", __func__, id_val);
  return CTUIAppOpener::ERROR_NOT_FOUND;
}

int32_t main() {
  int32_t ret = 0;
  uint8_t  secret[] = "password";
  client  = MinkIPC_connect("/dev/socket/letzd", &letzdOpener);

  sem_init(&sWaitForever, 0, 0);

  if (!client) {
    TUILOGE("Failed to create MinkIPC client");
    ret = Object_ERROR_BADOBJ;
    goto end;
  }
  else if (Object_isNull(letzdOpener)) {
    TUILOGE("Failed to get TVM Opener");
    ret = Object_ERROR_BADOBJ;
    goto end;
  }
  ret = CTrustedUIApp_open(&tuiAppObject);
  if ((ret != 0) || Object_isNull(tuiAppObject)) {
    TUILOGE("Failed to get TUI app object");
    ret = Object_ERROR_BADOBJ;
    goto end;
  }
  TUILOGE("registering eidtui app");
  ret = IRegisterApp_registerApp(letzdOpener, EIDTUI_UID, tuiAppObject, secret, sizeof(secret)/sizeof(uint8_t));

  TUILOGE("%s Registered EIDTUI App to TVM Service", __func__);

  registerSignalHandler();

  _releaseWakeLock();

  TUILOGE("%s:%u eidtuiapp is running .. ", __func__, __LINE__);
  sem_wait(&sWaitForever);

end:
  releaseAllObjects();
  _releaseWakeLock();
  return ret;
}
