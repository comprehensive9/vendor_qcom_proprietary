/******************************************************************************

                          N E T M G R _ T I P C . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_tipc.c
  @brief   Network Manager module for tipc messasges

  DESCRIPTION
  Implementation of NetMgr module for tipc messages..

******************************************************************************/
/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#include <linux/tipc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#ifndef NETMGR_OFFTARGET
#include <netinet/in.h>
#endif
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>

#include "netmgr.h"
#include "netmgr_defs.h"
#include "netmgr_util.h"
#include "netmgr_netlink.h"
#include "netmgr_tipc.h"

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

/* TIPC NETLINK message sequence number */
uint32                   tipc_seqno = 0;
#define NETMGR_TIPC_PID_MASK  (0x7FFFFFFF)
#define NETMGR_TIPC_PID       (getpid() & NETMGR_TIPC_PID_MASK)

/*===========================================================================
  FUNCTION  netmgr_tipc_open_sock
===========================================================================*/
/*!
@brief
  Opens a tipc socket for the netmgr tipc multicast group

@return
  int - NETMGR_SUCCESS if socket is successfully opened,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_tipc_open_sock
(
  netmgr_nl_sk_info_t * sk_info
)
{
  int rval = NETMGR_FAILURE;
  int * p_sk_fd;
  struct sockaddr_tipc * p_sk_addr_loc, * p_sk_addr_rem;
  struct sockaddr_tipc server = {
    .family = AF_TIPC,
    .addrtype = TIPC_SERVICE_ADDR,
    .scope = TIPC_NODE_SCOPE,
    .addr.name.name.type = TIPC_GRP_ID,
    .addr.name.name.instance = TIPC_INST
  };
  NETMGR_LOG_FUNC_ENTRY;

  p_sk_fd = &sk_info->sk_fd;
  p_sk_addr_loc = &sk_info->tipc_addr_loc;
  p_sk_addr_rem = &sk_info->tipc_addr_rem;

  /* Open TIPC socket for specified protocol */
  if ((*p_sk_fd =  socket(AF_TIPC, SOCK_RDM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("tipc_open_sock: socket failed:");
    goto error;
  }

  /* Initialize socket addresses to null */
  memset(p_sk_addr_loc, 0, sizeof(struct sockaddr_tipc));
  memset(p_sk_addr_rem, 0, sizeof(struct sockaddr_tipc));

  memcpy(p_sk_addr_loc, &server, sizeof(struct sockaddr_tipc));

  /* Bind socket to the local address, i.e. specified groups. This ensures
  ** that multicast messages for these groups are delivered over this
  ** socket.
  */
  if( bind( *p_sk_fd,
            (struct sockaddr *)p_sk_addr_loc,
            sizeof(struct sockaddr_tipc) ) < 0) {
    close(*p_sk_fd);
    netmgr_log_sys_err("tipc_open_sock: bind failed:");
    goto error;
  }
  rval = NETMGR_SUCCESS;

error:

  NETMGR_LOG_FUNC_EXIT;
  return rval;
}

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
)
{
  int tmp;
  struct sockaddr_tipc server = {
    .family = AF_TIPC,
    .scope = TIPC_NODE_SCOPE,
    .addrtype = TIPC_ADDR_MCAST,
    .addr.nameseq.type = TIPC_GRP_ID,
    .addr.nameseq.lower = TIPC_INST,
    .addr.nameseq.upper = TIPC_INST + 1
  };

  if (0 > (tmp  = sendto(fd, buffer, buflen, 0,
                         (void*)&server, sizeof(server))))
  {
    netmgr_log_sys_err("Sending tipc msg, sendto failed:");
    return NETMGR_FAILURE;
  }

  netmgr_log_err("Writing tipc_msg: %d, Sent %d ", buflen, tmp);

  return buflen;
}

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
)
{
  NETMGR_LOG_FUNC_ENTRY;
  netmgr_log_err("Starting tipc listener thread\n");

  /* Initialize socket listener thread. This thread is used to listen for
  ** incoming messages on all netink sockets.
  */
  if (read_f && netmgr_nl_socklthrd_init( sk_thrd_info,
                                          sk_thrd_fdmap,
                                          sk_thrd_fdmap_size ) < 0)
  {
    netmgr_log_err("cannot init sock listener thread\n");
    return NETMGR_FAILURE;
  }

  /* Open a TIPC socket for TIPC protocol. This socket is used
  ** for receiving netlink messaging over TIPC protocol.
  */
  if (netmgr_tipc_open_sock( sk_info ) < 0)
  {
    netmgr_log_err("cannot open tipc routing sock\n");
    return NETMGR_FAILURE;
  }

  /* Add the TIPC socket to the list of sockets that the listener
  ** thread should listen on.
  */
  if (read_f && netmgr_nl_socklthrd_addfd( sk_thrd_info,
                                           sk_info->sk_fd,
                                           read_f ) < 0)
  {
    netmgr_log_err("cannot add tipc routing sock for reading\n");
    return NETMGR_FAILURE;
  }

  /* Start the socket listener thread */
  if (read_f &&  netmgr_nl_socklthrd_start( sk_thrd_info ) < 0 ) {
    netmgr_log_err("cannot start sock listener thread\n");
    return NETMGR_FAILURE;
  }

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}

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
)
{
  struct msghdr * msgh = NULL;
  ssize_t rmsgl = 0;
  NETMGR_ASSERT(msg_pptr);
  NETMGR_ASSERT(msglen_ptr);

  NETMGR_LOG_FUNC_ENTRY;

  if( NULL == (msgh =  netmgr_malloc( NL_MSG_MAX_LEN )) ) {
    netmgr_log_err("failed on netmgr_nl_alloc_msg\n");
    goto error;
  }

  /* Receive message over the socket */
   rmsgl = recv(fd, msgh,NL_MSG_MAX_LEN , 0);

  /* Verify that something was read */
  if (rmsgl <= 0) {
    netmgr_log_sys_err("Received nl_msg, recvmsg failed:");
    goto error;
  }
  *msg_pptr    = msgh;
  *msglen_ptr = (unsigned int)rmsgl;

  /* Return message ptr. Caller is responsible for freeing the memory */
  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;

error:
  /* An error occurred while receiving the message. Free all memory before
  ** returning.
  */
  netmgr_free( msgh );
  *msg_pptr    = NULL;
  *msglen_ptr  = 0;
  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_FAILURE;
}

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
int
netmgr_tipc_send_nl_msg( int fd, void* buffer, uint16 buflen )
{
  struct msghdr * msgh = NULL;
  struct nlmsghdr * nlh = NULL;
  int smsgl;
  int rc = NETMGR_SUCCESS;

  NETMGR_LOG_FUNC_ENTRY;

  if( NULL == (msgh = netmgr_nl_alloc_msg( NLMSG_SPACE(buflen) )) ) {
    netmgr_log_err("failed on netmgr_nl_alloc_msg\n");
    return NETMGR_FAILURE;
  }

  /* Fill the netlink message header */
  nlh = msgh->msg_iov->iov_base;
  nlh->nlmsg_len = NLMSG_SPACE(buflen);
  nlh->nlmsg_type = NETMGR_NL_EVENT;
  nlh->nlmsg_flags = 0;
  nlh->nlmsg_seq = (__u32)++tipc_seqno;
  nlh->nlmsg_pid = NETMGR_TIPC_PID;  /* thread pid */

  /* Fill in the netlink message payload */
  if( buffer && buflen )
    memcpy( NLMSG_DATA(nlh), buffer, buflen );

  /* Send message over the socket */
   smsgl = netmgr_tipc_send_event(fd, nlh,  NLMSG_SPACE(buflen));

  /* Verify that something was written */
  if (smsgl <= 0) {
    netmgr_log_sys_err("Writing tipc_msg, sendmsg failed:");
    rc = NETMGR_FAILURE;
  } else {
    netmgr_log_med("Generated tipc msg, sendmsg returned %d\n", smsgl);
  }

  /* Release dynamic memory */
  netmgr_nl_release_msg( msgh );

  NETMGR_LOG_FUNC_EXIT;
  return rc;
}
