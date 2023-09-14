/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    ril_utf_data_stubs.cpp
  @brief   RIL UTF Datamodule Stubs

  DESCRIPTION
    Defines stubs for HAL UTF

******************************************************************************/


//===========================================================================
//
//                           INCLUDE FILES
//
//===========================================================================

#include <string.h>
#include "qcril_qmi_pdc.h"
#include <fstream>
#include <errno.h>
#include <cutils/memory.h>
#include <cutils/properties.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log.h>

#ifdef QMI_RIL_UTF
#include <signal.h>
#include <time.h>
#endif
#include "modules/nas/NasModule.h"
#include "modules/dms/DmsModule.h"

#include "qcril_qmi_cell_info.h"
#include "qcril_qmi_cell_info_v12.h"
#include "modules/voice/qcril_qmi_voice.h"
extern "C" {
#include "qcril_log.h"
#include "telephony/ril.h"
#include <telephony/librilutils.h>
#include "IxErrno.h"
#include "comdef.h"
#include "qcrili.h"
#include "qcril_reqlist.h"
#include "qcril_cm_ss.h"
#include "qcril_other.h"
#include "qcril_other.h"
#include "qcril_mbn_hw_update.h"
#include "qcril_mbn_meta.h"
#include "qcril_am.h"

#include "qmi_ril_platform_dep.h"
#include "qmi_ril_file_observer.h"
#include "time_genoff.h"
}

#ifdef QMI_RIL_UTF
void ril_utf_embms_active_msg
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  return;
}

void qcril_data_embms_enable_data_req
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  return;
}

void qcril_data_embms_activate_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  return;
}

void qcril_data_embms_deactivate_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  return;
}

void qcril_data_embms_get_available_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  return;
}

void qcril_data_embms_get_active_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  return;
}

void qcril_data_embms_activate_deactivate_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  return;
}

void qcril_data_embms_send_interested_list
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  return;
}

void qcril_data_embms_content_desc_update
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  return;
}

#endif
