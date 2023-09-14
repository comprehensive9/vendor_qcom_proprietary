/******************************************************************************

                        N E T M G R _ T I P C . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_tipc.h
  @brief   Network Manager tipc messaging Header File

  DESCRIPTION
  Header file for NetMgr tipc messaging functions.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef _NETMGR_TIPC_H_
#define _NETMGR_TIPC_H_

#include <linux/tipc.h>
#include "netmgr_netlink.h"

/*---------------------------------------------------------------------------
   Constants for TIPC message attribute types
---------------------------------------------------------------------------*/
#define TIPC_INST  94
#define TIPC_SERVICE_ADDR  2
#define TIPC_GRP_ID  1012
/*===========================================================================
  FUNCTION  netmgr_tipc_listener_init
===========================================================================*/
/*!
@brief
  Initialization routine for TIPC socket interface message listener.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - Listening thread is created
*/
/*=========================================================================*/
int
netmgr_tipc_listener_init
(
  netmgr_socklthrd_info_t  * sk_thrd_info,
  netmgr_socklthrd_fdmap_t * sk_thrd_fdmap,
  unsigned int               sk_thrd_fdmap_size,
  netmgr_nl_sk_info_t      * sk_info,
  netmgr_socklthrd_fd_read_f read_f
);

/*===========================================================================
  FUNCTION  netmgr_tipc_send_event
===========================================================================*/
/*!
@brief
  Generates an asynchronous event indicaiton messages using TIPC socket.
  The message will be using netlink TLV formatting.
  See kernel/include/net/netink.h for details on message TLV formatting.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_tipc_send_event
(
  int fd,
  void *buffer,
  int buflen
);

/*===========================================================================
  FUNCTION  netmgr_tipc_recv_msg
===========================================================================*/
/*!
@brief
  Reads a complete NETLINK message incoming over the specified socket
  descriptor and returns it. Note that the memory for the message is
  dynamically allocated.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - Message buffer allocated from system heap
*/
/*=========================================================================*/
int netmgr_tipc_recv_msg
(
  int              fd,
  struct msghdr ** msg_pptr,
  unsigned int  *  msglen_ptr
);

/*===========================================================================
  FUNCTION  netmgr_tipc_send_nl_msg
===========================================================================*/
/*!
@brief
  Given an already encoded event it will create a NETLINK formatted
  message and send it over the specified TIPC socket descriptor.
  The specified buffer is assumed to conform with NETLINK
  message format specified in include/net/netlink.h

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_tipc_send_nl_msg
(
  int fd,
  void *buffer,
  uint16 buflen
);

#endif
