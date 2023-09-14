/*
 * Copyright (c) 2013-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 *
 */
#ifndef VERIFY_PRINT_ERROR
#define VERIFY_PRINT_ERROR
#endif

#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <remote.h>
#include <AEEStdErr.h>
#include <verify.h>

#ifndef SNS_TARGET_LE_PLATFORM
#ifndef ADSP_LIBHIDL_NAME
#define ADSP_LIBHIDL_NAME "libhidlbase.so"
#endif
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif // __clang__

typedef int (*adsp_default_listener_start_t)(int argc, char *argv[]);
typedef int (*remote_handle_control_t)(uint32_t req, void* data, uint32_t len);

/*check for slpi/adsp and load the library accordingly*/
const char *get_lib() {
  struct stat sb;
  if(!stat("/sys/kernel/boot_slpi", &sb)){
      return "libssc_default_listener.so";
  }
  else {
      return "libadsp_default_listener.so";
  }
}

int request_fastrpc_wakelock(void *adsphandler) {
  int nErr = 0;
#ifdef FASTRPC_WAKELOCK_CONTROL_SUPPORTED
  remote_handle_control_t handle_control;
  struct remote_rpc_control_wakelock data;

  data.enable = 1;

  if (NULL != (handle_control = (remote_handle_control_t)dlsym(adsphandler, "remote_handle_control"))) {
    VERIFY_IPRINTF("found remote_handle_control, requesting for wakelock");
    nErr = handle_control(DSPRPC_CONTROL_WAKELOCK, (void*)&data, sizeof(data));
    if (nErr == AEE_EUNSUPPORTEDAPI) {
      VERIFY_EPRINTF("fastrpc wakelock request is not supported");
      /* this feature may not be supported by all targets
         treat this case as normal since we still can call listener_start */
      nErr = AEE_SUCCESS;
    } else if (nErr) {
      VERIFY_EPRINTF("failed to enable fastrpc wake-lock control, %x", nErr);
    } else {
      VERIFY_EPRINTF("fastrpc wakelock control is enabled");
    }
  } else {
    VERIFY_EPRINTF("unable to find remote_handle_control, %s", dlerror());
    /* there should be no case where remote_handle_control doesn't exist */
    nErr = AEE_EFAILED;
  }
#endif
  return nErr;
}

int main(int argc, char *argv[]) {

  int nErr = 0;
  void *adsphandler = NULL, *libhidlbaseHandler = NULL;
  adsp_default_listener_start_t listener_start;
  bool is_le_target = false;
#ifndef SNS_TARGET_LE_PLATFORM
  libhidlbaseHandler = dlopen(ADSP_LIBHIDL_NAME, RTLD_NOW);
#else
  is_le_target = true;
#endif

  VERIFY_EPRINTF("sscrpcd daemon starting");
  if( (true == is_le_target) || (NULL != libhidlbaseHandler) ){
    while (1) {
      if(NULL != (adsphandler = dlopen(get_lib(), RTLD_NOW))) {
        if(NULL != (listener_start =
          (adsp_default_listener_start_t)dlsym(adsphandler, "adsp_default_listener_start"))) {
          VERIFY_EPRINTF("adsp_default_listener_start called for [%s]", argv[1]);
          nErr = request_fastrpc_wakelock(adsphandler);
          if (!nErr) {
            listener_start(argc, argv);
            VERIFY_EPRINTF("listener exits");
          } else {
            VERIFY_EPRINTF("can't start listener, %x", nErr);
          }
        }
        if(0 != dlclose(adsphandler)) {
          VERIFY_EPRINTF("dlclose failed");
        }
      } else {
        VERIFY_EPRINTF("sscrpcd daemon error %s", dlerror());
      }
      VERIFY_EPRINTF("sscrpcd daemon will restart after 25ms...");
      usleep(25000);
    }
#ifndef SNS_TARGET_LE_PLATFORM
    if(0 != dlclose(libhidlbaseHandler)) {
      VERIFY_EPRINTF("libhidlbase dlclose failed");
    }
#endif
    VERIFY_EPRINTF("sscrpcd daemon exiting %x", nErr);

    return nErr;
  }else {
    VERIFY_EPRINTF("libhidlbase dlopen failed");
  }
  return 0;
}
