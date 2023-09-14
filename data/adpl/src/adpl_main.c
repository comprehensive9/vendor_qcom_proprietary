/******************************************************************************

                        ADPL_MAIN.C

******************************************************************************/

/******************************************************************************

  @file    adpl_main.c
  @brief   Accelerated Data Path Logging module

  DESCRIPTION
  Implementation of Accelerated Data Path Logging module.

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2015,2018,2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/25/18   vv         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <cutils/sockets.h>

#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "adpl.h"

/*===========================================================================
                              VARIABLE DECLARATIONS
===========================================================================*/

static adpl_param             adpl_config_param;
static odl_param              odl_config_param;
qcmap_sk_fd_set_info_t        sk_fdset;
static qmi_csi_os_params      os_params;

/* Global Netlink Socket. variable */
qcmap_sk_info_t     sk_info;

/*Server socket*/
unsigned int adpl_dpm_sockfd;

/*Client socket*/
unsigned int dpm_adpl_sockfd;

/*==========================================================================
Fusion target can be determined with ro.baseband android property and this
property value set to "mdm" for all fusion targets.
is_fusion_supported flag set to true if ro.baseband property is "mdm"
==========================================================================*/
boolean             is_fusion_supported = FALSE;

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/*==========================================================================

FUNCTION PRINT_MSG()

DESCRIPTION

  This function is used to print QTI logs to a file

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

==========================================================================*/

void PRINT_msg
(
  const char *funcName,
  uint32_t lineNum,
  const char*fmt, ...
)
{
}

/*===========================================================================

FUNCTION adpl_print_logcat_msg()

DESCRIPTION
- print the message in logcat
- Used for LA targets only

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void adpl_print_logcat_msg
(
  uint32_t msgType,
  const char *funcName,
  uint32_t lineNum,
  const char *fileInfo,
  const char *fmt, ...
)
{
  char msg[ADPL_MAX_COMMAND_STR_LEN] = {0};
  int len = 0;
  va_list args;
  char *fileName = NULL;

  if (funcName == NULL || fileInfo == NULL || fmt == NULL)
  {
    LOG_MSG_ERROR("Invalid params", 0, 0, 0);
    return ;
  }

  fileName = strrchr(fileInfo, '/'); //absolute path to only filename
  if (fileName == NULL)
  {
    len = snprintf(msg, ADPL_MAX_COMMAND_STR_LEN, "[%s:%d] %s():",
                   fileInfo, lineNum, funcName);
  }
  else
  {
    len = snprintf(msg, ADPL_MAX_COMMAND_STR_LEN, "[%s:%d] %s():",
                   fileName + 1, lineNum, funcName);
  }
  if (len > 0 && len < ADPL_MAX_COMMAND_STR_LEN)
  {
    va_start (args, fmt);
    vsnprintf(msg + len, ADPL_MAX_COMMAND_STR_LEN - len, fmt, args);
    va_end (args);
  }

  if (msgType == LOGCAT_INFO)
    __android_log_print( ANDROID_LOG_INFO, MAIN_LOG_TAG,"%s", msg);
  else
    __android_log_print( ANDROID_LOG_ERROR, MAIN_LOG_TAG,"%s", msg);

  return ;
}

/*===========================================================================
  FUNCTION create_socket
=============================================================================
  @brief
    creates a unix domain socket

  @input
    sockfd - socket handler

  @return
    0  - success
  - 1 - failure

  @dependencies
    none

  @sideefects
  None
===========================================================================*/
int create_socket(unsigned int *sockfd)
{
  if ((*sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == ADPL_FAILURE)
  {
    LOG_MSG_ERROR("Error creating socket, errno: %d", errno, 0, 0);
    return ADPL_FAILURE;
  }

  if(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0)
  {
    LOG_MSG_ERROR("Couldn't set Close on Exec, errno: %d", errno, 0, 0);
  }

  return ADPL_SUCCESS;
}

/*===========================================================================
  FUNCTION create_server_socket
=============================================================================
  @brief
    creates server socket

  @input
    unsigned int sock

  @return
    0  - success
  - 1 - failure

  @dependencies
    none

  @sideefects
  None
===========================================================================*/
int create_server_socket(unsigned int* sockfd, char* file_path)
{
  int val, rval;
  struct sockaddr_un adpl_un;
  int len=0;
  struct timeval rcv_timeo;

  *sockfd = android_get_control_socket(file_path);
  if( *sockfd < 0 )
  {
    LOG_MSG_ERROR("failed to create server sockfd ", 0, 0, 0);
    return ADPL_FAILURE;
  }
  else if (*sockfd >= FD_SETSIZE)
  {
    LOG_MSG_ERROR(" create_server_socket fd=%d is exceeding range", *sockfd, 0, 0);
    return ADPL_FAILURE;
  }

  rcv_timeo.tv_sec = 0;
  rcv_timeo.tv_usec = 100000;
  setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
  val = fcntl(*sockfd, F_GETFL, 0);
  fcntl(*sockfd, F_SETFL, val | O_NONBLOCK);
  /* set bit in os_params */
  FD_SET(*sockfd, &(os_params.fds));
  os_params.max_fd = MAX(os_params.max_fd, *sockfd);

  return ADPL_SUCCESS;
}

/*===========================================================================
  FUNCTION adpl_cmd_process
=============================================================================
  @brief
    Process the cmd received in a socket

  @input
    int fd

  @return

  @dependencies
    none

  @sideefects
  None
===========================================================================*/
int adpl_cmd_process
(
  int fd
)
{
   adpl_event_t *            event_buf=NULL;
   int                       nbytes = 0;
   char                      buf[ADPL_MAX_BUF_LEN];
   struct sockaddr_storage   buf_addr;
   socklen_t                 addr_len=sizeof(struct sockaddr_storage);
   char                      buffer;

  if ( ( nbytes = recvfrom(fd, buf, ADPL_MAX_BUF_LEN-1 , 0, (struct sockaddr *)&buf_addr, &addr_len)) <= 0 )
  {
    if ( nbytes == 0 )
    {
      LOG_MSG_INFO1("Zero bytes received from the callback", 0, 0, 0);
    }
    else
    {
      LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
    }
  }

  else
  {
    event_buf = (adpl_event_t *)buf;

    if (fd == adpl_dpm_sockfd)
    {
      if(event_buf != NULL)
      {
        if(event_buf->event == ADPL_DPM_INIT_EVENT)
        {
          if (dpm_client_init() != ADPL_SUCCESS )
          {
            LOG_MSG_ERROR("Unable to init DPM client!!",0, 0, 0);
          }
        }
        else if(event_buf->event == ADPL_DPM_MODEM_NOT_IN_SERVICE_EVENT)
        {
          adpl_modem_not_in_service();
        }
      }
    }
  }
  return ADPL_SUCCESS;
}

/*==========================================================================

FUNCTION MAIN()

DESCRIPTION

  The main function for ADPL which is first called when ADPL gets started on
  boot up.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

==========================================================================*/
int main(int argc, char ** argv)
{
  int                       ret_val;
  int                       rval;
  FILE                      *fp = NULL;
  char                      ro_baseband_value[ADPL_MAX_COMMAND_STR_LEN];
  char                      ro_baseband_default_value[ADPL_PROPERTY_VALUE_MAX];
  char                      esoc_name[ADPL_MAX_COMMAND_STR_LEN];
  boolean                   is_LA_LE_fusion_target = FALSE;
/*-----------------------------------------------------------------------
    Initialize ADPL variables
------------------------------------------------------------------------*/
  memset(&sk_fdset, 0, sizeof(qcmap_sk_fd_set_info_t));
  memset(&adpl_config_param, 0, sizeof(adpl_param));

/*-------------------------------------------------------------------------
  Initializing Diag for QXDM logs
-------------------------------------------------------------------------*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
     printf("Diag_LSM_Init failed !!");
  }
  /*-----------------------------------------------------------------------
    Check for fusion targets
  ------------------------------------------------------------------------*/
  memset(ro_baseband_value, 0, sizeof(ro_baseband_value));
  memset(ro_baseband_default_value, 0x0, sizeof(ro_baseband_default_value) );
  memset(esoc_name, 0, sizeof(esoc_name));

  snprintf( ro_baseband_default_value, sizeof(ro_baseband_default_value)-1, "%s", "" );
  ret_val = property_get( ADPL_BASEBAND_PROP, ro_baseband_value, ro_baseband_default_value );

  if (ret_val > (int)(sizeof(BASEBAND_PROP_VAL)))
  {
    LOG_MSG_ERROR("Android property for baseband has unexpected size(%d)",
                   ret_val, 0, 0);
  }

  if(strncmp(ro_baseband_value, BASEBAND_PROP_VAL, strlen(BASEBAND_PROP_VAL)) == 0)
  {
    is_fusion_supported = TRUE;

    LOG_MSG_INFO1("ADPL:Fusion target detected", 0, 0, 0);

    fp = fopen(ADPL_ESOCINFO_ESOC_NAME_FILE_PATH,"re");
    if(NULL == fp)
    {
       LOG_MSG_ERROR("error opening socinfo file:%s",ADPL_ESOCINFO_ESOC_NAME_FILE_PATH, 0, 0);
    }
    else if (0 == fscanf(fp,"%s", esoc_name))
    {
      LOG_MSG_ERROR("error reading esoc_name", 0, 0, 0);
      fclose(fp);
      fp = NULL;
    }
    else
    {
      fclose(fp);
      fp = NULL;
      if(strncmp(esoc_name, ADPL_CARRERA_FUSION_VAL, strlen(ADPL_CARRERA_FUSION_VAL)) == 0)
      {
        /*carrera fusion which is similar to LA functioanlity*/
        is_fusion_supported = FALSE;
        LOG_MSG_INFO1("ADPL:It is %s fusion target : %s", esoc_name, "Carrera", 0);
      }
      else if(strncmp(esoc_name, ADPL_NAPALI_CHIRON_FUSION_VAL, strlen(ADPL_NAPALI_CHIRON_FUSION_VAL)) == 0)
      {
        is_LA_LE_fusion_target = TRUE;
        LOG_MSG_INFO1("ADPL:It is %s fusion target : %s", esoc_name, "Napali Chiron", 0);
      }
      else if( (strncmp(esoc_name, ADPL_STOP_FUSION, strlen(ADPL_STOP_FUSION)) == 0) ||
               (strncmp(esoc_name, ADPL_STOP_FUSION_SDX, strlen(ADPL_STOP_FUSION_SDX)) == 0) )
      {
        /*Stopping ADPL daemon in case of Kona fusion. esoc_name will return "SDX55M"*/
        LOG_MSG_INFO1("ADPL:It is %s fusion target : %s", esoc_name, "Kona/Hana Fusion", 0);
        LOG_MSG_INFO1("ADPL:Exiting from ADPL daemon",0, 0, 0);
        property_set("ctl.stop","vendor.dataadpl");
      }
      else
      {
        LOG_MSG_INFO1("ADPL:It is Legacy fusion target: %s, continue", esoc_name, 0, 0);
      }
    }
  }

  LOG_MSG_INFO1("Start ADPL", 0, 0, 0);

  odl_config_param.is_odl_ipa_enabled = ODL_INVALID_VALUE;

  if(is_fusion_supported)
  {
    if (is_LA_LE_fusion_target)
    {
      memcpy(adpl_config_param.adpl_iface_device_file,
             ADPL_USB_DEV_NODE,
             strlen(ADPL_USB_DEV_NODE));
    }
    else
    {
      memcpy(adpl_config_param.adpl_iface_device_file,
             ADPL_USB_DEV_FILE,
             strlen(ADPL_USB_DEV_FILE));
    }
  }
  else
  {
    memcpy(adpl_config_param.adpl_iface_device_file,
           ADPL_USB_DEV_FILE,
           strlen(ADPL_USB_DEV_FILE));

    memcpy(odl_config_param.odl_iface_device_file,
           ODL_CTL_DEV_FILE,
           strlen(ODL_CTL_DEV_FILE));
  }

/*---------------------------------------------------------------------
  Initialize ADPL modem interface
---------------------------------------------------------------------*/
  if (is_fusion_supported)
  {
    ret_val = adpl_modem_init_thru_qmuxd(&adpl_config_param,
                                        (qcmap_sock_thrd_fd_read_f)adpl_modem_recv_msg_thru_qmuxd);
    if(ret_val != ADPL_SUCCESS)
    {
      LOG_MSG_ERROR("Failed to initialize ADPL modem interface",
                    0, 0, 0);
      return ADPL_FAILURE;
    }
  }

/*---------------------------------------------------------------------
      Create ADPL client and server sockets
----------------------------------------------------------------------*/
  /* Create cmd -> adpl client socket */
  if (create_socket(&dpm_adpl_sockfd) != ADPL_SUCCESS)
  {
    LOG_MSG_ERROR("Error creating dpm_adpl_sockfd client socket", 0, 0, 0);
  }

  /* Create adpl -> cmd server socket */
  rval = create_server_socket(&adpl_dpm_sockfd, ADPL_CMD_UDS_FILE);

  if (rval != ADPL_SUCCESS)
  {
    LOG_MSG_ERROR("Unable to create adpl dpm server socket!", 0,0,0);
    return ADPL_FAILURE;
  }

  if( qcmap_nl_addfd_map(&sk_fdset,
                         adpl_dpm_sockfd,
                         adpl_cmd_process,
                         MAX_NUM_OF_FD) != ADPL_SUCCESS)
  {
    LOG_MSG_ERROR("Unable to map the server socket fd to "
                  "adpl cmd process function", 0,0,0);
    close(adpl_dpm_sockfd);
    return ADPL_FAILURE;

  }

/*---------------------------------------------------------------------
  Initialize DPM client
---------------------------------------------------------------------*/

    if(!is_fusion_supported)
    {
      ret_val = adpl_dpm_init(&adpl_config_param, &odl_config_param);

      if(ret_val != ADPL_SUCCESS)
      {
        LOG_MSG_ERROR("Failed to initialize DPM",
                      0, 0, 0);
        return ADPL_FAILURE;
      }
      ret_val = adpl_modem_init(&adpl_config_param, &odl_config_param);
      if(ret_val != ADPL_SUCCESS)
      {
        LOG_MSG_ERROR("Failed to initialize Modem",
                      0, 0, 0);
        return ADPL_FAILURE;
      }
    }

/*---------------------------------------------------------------------
  Initialize peripheral interface for ADPL logging
---------------------------------------------------------------------*/
  ret_val = adpl_ph_init(&adpl_config_param,
                        &sk_fdset,
                        adpl_ph_recv_msg);
  if(ret_val != ADPL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize peripheral interface for ADPL logging over USB",
                  0, 0, 0);
  }

/*---------------------------------------------------------------------
  Initialize peripheral interface for ODL logging
---------------------------------------------------------------------*/
  ret_val = odl_ph_init(&odl_config_param,
                        &sk_fdset,
                        odl_ph_recv_msg);
  if(ret_val != ADPL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize ODL char device %s for ADPL logging over ODL",
                  ODL_CTL_DEV_FILE, 0, 0);
  }

/*--------------------------------------------------------------------
  Start the listener which listens to netlink events and QMI packets
  coming on USB-Rmnet device file
---------------------------------------------------------------------*/
  ret_val = qcmap_listener_start(&sk_fdset);

  if(ret_val != QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to start NL listener",
                   0, 0, 0);
  }

  return ADPL_SUCCESS;
}


