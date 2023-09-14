/*
 * Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <string.h>
#define LOG_TAG "sns_fastrpc_util"
#include <utils/Log.h>
#include <pthread.h>
extern "C" {
  #include "sns_fastRPC_utils.h"
}

static void *g_dlhandler = NULL;
static remote_handle remote_handle_fd = 0;
static sns_remote_handles handles;
static pthread_mutex_t rpc_lock = PTHREAD_MUTEX_INITIALIZER;

const char *get_lib() {
  /*check for slpi or adsp*/
  struct stat sb;
  if(!stat("/sys/kernel/boot_slpi", &sb)){
      return "libsdsprpc.so";
  }
  else {
      return "libadsprpc.so";
  }
}

#ifdef SNS_FASTRPC_UTILS_DEBUG
int sns_rpc_get_handle_dbg(sns_remote_handles *remote_handles, const char* func, const int line)
#else
int sns_rpc_get_handle(sns_remote_handles *remote_handles)
#endif
{
  void *dlhandler = NULL;
#ifdef SNS_FASTRPC_UTILS_DEBUG
  ALOGI("[DRM_DBG] sns_rpc_get_handle: %s: %d", func, line);
#endif

  pthread_mutex_lock(&rpc_lock);
  if (g_dlhandler != NULL) {
    memcpy((void*)remote_handles, (void*)&handles, sizeof(sns_remote_handles));
    ALOGI("fastrpc resources are already initialized");
    pthread_mutex_unlock(&rpc_lock);
    return 0;
  }

  memset((void*)&handles, 0, sizeof(sns_remote_handles));

  if ( NULL == (dlhandler = dlopen(get_lib(), RTLD_NOW))) {
    ALOGE("%s: load stub(%s) failed !", __FUNCTION__, get_lib());
    pthread_mutex_unlock(&rpc_lock);
    return -1;
  }

  ALOGI("%s: dlopen(%s) done !", __FUNCTION__, get_lib());
  handles.open = (remote_handle_open_t)dlsym(dlhandler,"remote_handle_open");
  handles.close = (remote_handle_close_t)dlsym(dlhandler,"remote_handle_close");
  handles.invoke = (remote_handle_invoke_t)dlsym(dlhandler,"remote_handle_invoke");
  handles.reg_buf = (remote_register_buf_t)dlsym(dlhandler,"remote_register_buf");
  handles.reg_buf_attr = (remote_register_buf_attr_t)dlsym(dlhandler,"remote_register_buf_attr");
  handles.mem_alloc = (rpcmem_alloc_t)dlsym(dlhandler,"rpcmem_alloc");
  handles.mem_free = (rpcmem_free_t)dlsym(dlhandler,"rpcmem_free");
  handles.mem_to_fd = (rpcmem_to_fd_t)dlsym(dlhandler,"rpcmem_to_fd");

  if ((NULL == handles.open) ||
      (NULL == handles.close) ||
      (NULL == handles.invoke) ||
      (NULL == handles.mem_alloc) ||
      (NULL == handles.mem_free) ||
      (NULL == handles.mem_to_fd)) {
    ALOGE("%s: dlsym failed !", __FUNCTION__);
    dlclose(dlhandler);
    pthread_mutex_unlock(&rpc_lock);
    return -1;
  }

  if (!remote_handle_fd) {
    if (handles.open(ITRANSPORT_PREFIX "createstaticpd:sensorspd", &remote_handle_fd)){
      ALOGE("failed to open remote handle for sensorspd");
      dlclose(dlhandler);
      pthread_mutex_unlock(&rpc_lock);
      return -1;
    } else {
      ALOGI("opened remote handle for sensorspd");
      remote_handle_fd = 1;
    }
  } else {
      ALOGI("using opened remote handle for sensorspd");
  }

  g_dlhandler = dlhandler;
  memcpy((void*)remote_handles, (void*)&handles, sizeof(sns_remote_handles));
  ALOGI("fastrpc resources are initialized");
  pthread_mutex_unlock(&rpc_lock);
  return 0;
}

void sns_rpc_reset()
{
  pthread_mutex_lock(&rpc_lock);
  ALOGI("reset fastrpc resources");
  if (g_dlhandler == NULL) {
    ALOGE("fastrpc resources are not initialized before. nothing to reset");
    pthread_mutex_unlock(&rpc_lock);
    return;
  }
  if (remote_handle_fd != 0) {
    ALOGI("close remote handle for sensor pd");
    handles.close(remote_handle_fd);
    remote_handle_fd = 0;
  }
  ALOGI("unload fastrpc lib");
  dlclose(g_dlhandler);
  g_dlhandler = NULL;
  ALOGI("all fastrpc resources are reset successfully");
  pthread_mutex_unlock(&rpc_lock);
}

