/*
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "stdlib.h"
#include <stdbool.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include "remote.h"

//#define SNS_FASTRPC_UTILS_DEBUG

/*Funtion pointer declarations to the open , close and invoke methods*/
typedef int (*remote_handle_open_t)(const char* name, remote_handle *ph);
typedef int (*remote_handle_close_t)(remote_handle h);
typedef int (*remote_handle_invoke_t)(remote_handle h, uint32_t dwScalars, remote_arg *pra);
typedef void (*remote_register_buf_t)(void* buf, int size, int fd);
typedef void (*remote_register_buf_attr_t)(void* buf, int size, int fd, int attr);
typedef void* (*rpcmem_alloc_t)(int heapid, uint32_t flags, int size);
typedef void (*rpcmem_free_t)(void* po);
typedef int (*rpcmem_to_fd_t)(void* po);


/*sns_remote_handles contains handle for open, close and invoke methods*/
typedef struct
{
  remote_handle_open_t open;
  remote_handle_close_t close;
  remote_handle_invoke_t invoke;
  remote_register_buf_t reg_buf;
  remote_register_buf_attr_t reg_buf_attr;
  rpcmem_alloc_t mem_alloc;
  rpcmem_free_t mem_free;
  rpcmem_to_fd_t mem_to_fd;
}sns_remote_handles;

/*returns the libarry name which supposed to load at run time*/
const char *get_lib();

/**
 * @brief sns_rpc_get_handle, intialise the fastRPC symbols by loading proper
 *  library and storing handles.
 *
 * @param[i] pointer to input where this fucntion updates the remote handles like
 * open, close and invoke handles.
 *
 * @param[o] returns -1 in case of failure and 0 incase of succes.
 */
#ifdef SNS_FASTRPC_UTILS_DEBUG
int sns_rpc_get_handle_dbg(sns_remote_handles *remote_handles, const char* func, const int line);
#else
int sns_rpc_get_handle(sns_remote_handles *remote_handles);
#endif

/**
 * @brief sns_rpc_reset, will reset sources to be init state
 *
 */
void sns_rpc_reset();
#ifdef SNS_FASTRPC_UTILS_DEBUG
#define sns_rpc_get_handle(handles) sns_rpc_get_handle_dbg(handles, __FUNCTION__, __LINE__)
#endif
