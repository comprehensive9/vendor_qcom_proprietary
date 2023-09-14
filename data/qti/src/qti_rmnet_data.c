/******************************************************************************

                        QTI_RMNET_DATA.C

******************************************************************************/

/******************************************************************************

  @file    qti_rmnet_data.c
  @brief   Tethering Interface module for RmNET DATA interaction.


  DESCRIPTION
  This file has functions which interact with RmNET DATA for
  RMNET tethering.

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        -------------------------------------------------------
01/22/14   sb         Add support for Fusion

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/rmnet_data.h>
#include <linux/msm_rmnet.h>

#include "qti.h"
#include "librmnetctl.h"


static  qti_rmnet_param        * rmnet_state_config = NULL;

/*===========================================================================
                               FUNCTION DEFINITIONS
=========================================================================*/


/*===========================================================================

FUNCTION QTI_RMNET_CALL_IOCTL_ON_DEV()

DESCRIPTION
  - calls the specified IOCTL on the specified interface

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_rmnet_call_ioctl_on_dev
(
  const char         *dev,
  unsigned int        req,
  struct ifreq       *ifr
)
{
  int fd;

  /* Open a temporary socket of datagram type to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("call_ioctl_on_dev: socket open failed", 0, 0, 0);
    return QTI_FAILURE;
  }

  /* Set device name in the ioctl req struct */
  (void)strlcpy(ifr->ifr_name, dev, sizeof(ifr->ifr_name));

  /* Issue ioctl on the device */
  if (ioctl(fd, req, ifr) < 0)
  {
    LOG_MSG_ERROR("call_ioctl_on_dev: ioctl failed", 0, 0, 0);
    close(fd);
    return QTI_FAILURE;
  }

  /* Close temporary socket */
  close(fd);
  return QTI_SUCCESS;
}

/*===========================================================================

FUNCTION QTI_RMNET_DATA_INIT()

DESCRIPTION
- Initialize the rmnet data module

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None
==========================================================================*/
int qti_rmnet_data_init( qti_rmnet_param* rmnet_state )
{
  uint16_t                status_code;
  struct rmnetctl_hndl_s  *rmnet_cfg_lib_handle;
  int                     ret_val;

  if(rmnet_state == NULL)
  {
    LOG_MSG_ERROR("RmNet state not valid", 0, 0, 0);
    return QTI_FAILURE;
  }

  rmnet_state_config = rmnet_state;

  if ((ret_val = rtrmnet_ctl_init(&rmnet_cfg_lib_handle, &status_code)) != RMNETCTL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize RmNet data driver handle."
                "Return value = %d. Status code =%d",ret_val, status_code, 0);
    return QTI_FAILURE;
  }
  rmnet_state_config->rmnet_cfg_lib_handle = rmnet_cfg_lib_handle;

  return QTI_SUCCESS;
}

/*===========================================================================

FUNCTION QTI_RMNET_DATA_INIT_BRIDGE()

DESCRIPTION
  - initializes the RmNet data driver

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_data_init_bridge(char* modem_iface, char* peripheral_iface)
{
  char          command[MAX_COMMAND_STR_LEN];
  uint16_t        status_code;
  struct        rmnetctl_hndl_s  *rmnet_cfg_lib_handle;
  int           ret_val;
  struct        ifreq ifr;
  int           mhi_ep_id;
  int           mtu_mru_size = DEFAULT_MTU_MRU_VALUE;
  static char   args[QTI_PROPERTY_VALUE_MAX];
  char          def[PROPERTY_MTU_SIZE+1];
/*---------------------------------------------------------------------------*/
  if(rmnet_state_config == NULL ||
     rmnet_state_config->rmnet_cfg_lib_handle == NULL)
  {
    LOG_MSG_ERROR("RmNet state/handle not valid", 0, 0, 0);
    return QTI_FAILURE;
  }
  /*---------------------------------------------------------------------------
     Get the RmNet data driver handle
  ----------------------------------------------------------------------------*/
  rmnet_cfg_lib_handle = rmnet_state_config->rmnet_cfg_lib_handle;

/*---------------------------------------------------------------------------
  To handle QTI restart teardown bridge before setting it up
----------------------------------------------------------------------------*/
  qti_rmnet_data_teardown_bridge(modem_iface, peripheral_iface);


/*---------------------------------------------------------------------------
  Associate MHI interface to RmNet data driver
----------------------------------------------------------------------------*/
  if ((ret_val = rtrmnet_ctl_bridgevnd(rmnet_cfg_lib_handle, modem_iface, peripheral_iface, &status_code )) != RMNETCTL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to associate MHI interface with RmNet data driver."
                  "Return value = %d. Status code =%d",ret_val, status_code, 0);
    return QTI_FAILURE;
  }

/*---------------------------------------------------------------------------
  Set IP mode on USB interface
----------------------------------------------------------------------------*/
  if (qti_rmnet_call_ioctl_on_dev(peripheral_iface,
                                  RMNET_IOCTL_SET_LLP_IP,
                                  &ifr) != QTI_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to set IP mode on USB interface", 0, 0, 0);
    qti_rmnet_data_teardown_bridge(modem_iface, peripheral_iface);
    return QTI_FAILURE;
  }

/*---------------------------------------------------------------------------
  Set IP mode on MHI interface
----------------------------------------------------------------------------*/
  if (qti_rmnet_call_ioctl_on_dev(modem_iface,
                                  RMNET_IOCTL_SET_LLP_IP,
                                  &ifr) != QTI_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to set IP mode on MHI interface", 0, 0, 0);
    qti_rmnet_data_teardown_bridge(modem_iface, peripheral_iface);
    return QTI_FAILURE;
  }

/*---------------------------------------------------------------------------
  bring down both USB and MHI interfaces
----------------------------------------------------------------------------*/
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s down", modem_iface);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s down", peripheral_iface);
  ds_system_call(command, strlen(command));

/*---------------------------------------------------------------------------
  The following needs to be done only in RmNet case:
  Set MTU on modem iface
  Set MRU on peripheral iface
  (not needed in DPL case because of no UL traffic)
----------------------------------------------------------------------------*/
  
  LOG_MSG_INFO1("RmNet interface", 0, 0, 0);
  ret_val = qti_rmnet_modem_set_mtu(mtu_mru_size, modem_iface);
  if (ret_val != QTI_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to set default MTU value on modem", 0, 0, 0);
    qti_rmnet_data_teardown_bridge(modem_iface, peripheral_iface);
    return QTI_FAILURE;
  }

   ret_val = qti_rmnet_ph_set_mru(mtu_mru_size, peripheral_iface);
   if (ret_val != QTI_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to set default MTU value on peripheral", 0, 0, 0);
    qti_rmnet_data_teardown_bridge(modem_iface, peripheral_iface);
    return QTI_FAILURE;
  }


/*---------------------------------------------------------------------------
  The following needs to be done in both RmNet and DPL usecases:
  Set MRU on modem iface
  Set MTU on peripheral iface
----------------------------------------------------------------------------*/
  ret_val = qti_rmnet_modem_set_mru(mtu_mru_size, modem_iface);
  if (ret_val != QTI_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to set default MRU value on modem", 0, 0, 0);
    qti_rmnet_data_teardown_bridge(modem_iface, peripheral_iface);
    return QTI_FAILURE;
  }

  ret_val = qti_rmnet_ph_set_mtu(mtu_mru_size, peripheral_iface);
  if (ret_val != QTI_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to set default MTU value on peripheral", 0, 0, 0);
    qti_rmnet_data_teardown_bridge(modem_iface, peripheral_iface);
    return QTI_FAILURE;
  }

/*---------------------------------------------------------------------------
  Bring up MHI and USB interfaces
----------------------------------------------------------------------------*/
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s up", modem_iface);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s up", peripheral_iface);
  ds_system_call(command, strlen(command));

  ret_val = qti_is_iface_up(modem_iface);
  if(ret_val)
  {
    LOG_MSG_INFO1("Modem iface is up", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Modem interface is not up", 0, 0, 0);
    qti_rmnet_data_teardown_bridge(modem_iface, peripheral_iface);
    return QTI_FAILURE;
  }

  ret_val = qti_is_iface_up(peripheral_iface);
  if(ret_val)
  {
    LOG_MSG_INFO1("Peripheral iface is up", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Peripheral_iface interface is not up", 0, 0, 0);
    qti_rmnet_data_teardown_bridge(modem_iface, peripheral_iface);
    return QTI_FAILURE;
  }
  return QTI_SUCCESS;
}

/*===========================================================================

FUNCTION QTI_RMNET_DATA_TEARDOWN_BRIDGE()

DESCRIPTION
  resets RmNet data driver

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_data_teardown_bridge(char* modem_iface, char* peripheral_iface)
{
  char   command[MAX_COMMAND_STR_LEN];
  uint16_t status_code;
  struct rmnetctl_hndl_s  *rmnet_cfg_lib_handle;
  int    ret_val;
  int    mtu_mru_size;
  int    mhi_ep_id;
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Bring down MHI and USB interfaces
----------------------------------------------------------------------------*/
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s down", modem_iface);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s down", peripheral_iface);
  ds_system_call(command, strlen(command));

/*---------------------------------------------------------------------------
  Get RmNet data driver handle
----------------------------------------------------------------------------*/
  rmnet_cfg_lib_handle = rmnet_state_config->rmnet_cfg_lib_handle;

  if ((ret_val = rtrmnet_ctl_delvnd(rmnet_cfg_lib_handle, modem_iface, &status_code))!= RMNETCTL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to unset logical ep config for MHI interface with RmNet data driver."
                  "Return value = %d. Status code =%d",ret_val, status_code, 0);
  }
  /*TODO is it needed?*/
  if ((ret_val = rtrmnet_ctl_delvnd(rmnet_cfg_lib_handle, peripheral_iface, &status_code))!= RMNETCTL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to unset logical ep config for MHI interface with RmNet data driver."
                  "Return value = %d. Status code =%d",ret_val, status_code, 0);
  }

  ret_val = qti_is_iface_up(modem_iface);
  if(!ret_val)
  {
    LOG_MSG_INFO1("Modem iface is down", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Modem interface is not down", 0, 0, 0);
  }

  ret_val = qti_is_iface_up(peripheral_iface);
  if(!ret_val)
  {
    LOG_MSG_INFO1("Peripheral iface is down", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Peripheral_iface interface is not down", 0, 0, 0);
  }

  return QTI_SUCCESS;

}


