/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C N S S _ P L A T _ I P C _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the cnss_platform service Data structures.

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Tue Nov 24 2020 (Spin 0)
   From IDL File: cnss_plat_ipc_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "cnss_plat_ipc_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t cnss_plat_ipc_qmi_init_setup_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_init_setup_req_msg_v01, dms_mac_addr_supported),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_init_setup_req_msg_v01, qdss_hw_trace_override),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_init_setup_req_msg_v01, cal_file_available_bitmask)
};

static const uint8_t cnss_plat_ipc_qmi_init_setup_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_init_setup_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_init_setup_resp_msg_v01, drv_status)
};

static const uint8_t cnss_plat_ipc_qmi_file_download_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_ind_msg_v01, file_name),
  CNSS_PLAT_IPC_QMI_MAX_FILE_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_ind_msg_v01, file_id)
};

static const uint8_t cnss_plat_ipc_qmi_file_download_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_req_msg_v01, file_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_req_msg_v01, file_size),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_req_msg_v01, end),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_req_msg_v01, seg_index),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x05,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_req_msg_v01, seg_buf),
  ((CNSS_PLAT_IPC_QMI_MAX_DATA_SIZE_V01) & 0xFF), ((CNSS_PLAT_IPC_QMI_MAX_DATA_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_req_msg_v01, seg_buf) - QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_req_msg_v01, seg_buf_len)
};

static const uint8_t cnss_plat_ipc_qmi_file_download_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_resp_msg_v01, file_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_download_resp_msg_v01, seg_index)
};

static const uint8_t cnss_plat_ipc_qmi_file_upload_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_ind_msg_v01, file_name),
  CNSS_PLAT_IPC_QMI_MAX_FILE_NAME_LEN_V01,

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_ind_msg_v01, file_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_ind_msg_v01, file_size)
};

static const uint8_t cnss_plat_ipc_qmi_file_upload_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_req_msg_v01, file_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_req_msg_v01, seg_index)
};

static const uint8_t cnss_plat_ipc_qmi_file_upload_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_resp_msg_v01, file_id),

  0x04,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_resp_msg_v01, end),

  0x05,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_resp_msg_v01, seg_index),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x06,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_resp_msg_v01, seg_buf),
  ((CNSS_PLAT_IPC_QMI_MAX_DATA_SIZE_V01) & 0xFF), ((CNSS_PLAT_IPC_QMI_MAX_DATA_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_resp_msg_v01, seg_buf) - QMI_IDL_OFFSET8(cnss_plat_ipc_qmi_file_upload_resp_msg_v01, seg_buf_len)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry cnss_platform_message_table_v01[] = {
  {sizeof(cnss_plat_ipc_qmi_init_setup_req_msg_v01), cnss_plat_ipc_qmi_init_setup_req_msg_data_v01},
  {sizeof(cnss_plat_ipc_qmi_init_setup_resp_msg_v01), cnss_plat_ipc_qmi_init_setup_resp_msg_data_v01},
  {sizeof(cnss_plat_ipc_qmi_file_download_ind_msg_v01), cnss_plat_ipc_qmi_file_download_ind_msg_data_v01},
  {sizeof(cnss_plat_ipc_qmi_file_download_req_msg_v01), cnss_plat_ipc_qmi_file_download_req_msg_data_v01},
  {sizeof(cnss_plat_ipc_qmi_file_download_resp_msg_v01), cnss_plat_ipc_qmi_file_download_resp_msg_data_v01},
  {sizeof(cnss_plat_ipc_qmi_file_upload_ind_msg_v01), cnss_plat_ipc_qmi_file_upload_ind_msg_data_v01},
  {sizeof(cnss_plat_ipc_qmi_file_upload_req_msg_v01), cnss_plat_ipc_qmi_file_upload_req_msg_data_v01},
  {sizeof(cnss_plat_ipc_qmi_file_upload_resp_msg_v01), cnss_plat_ipc_qmi_file_upload_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object cnss_platform_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *cnss_platform_qmi_idl_type_table_object_referenced_tables_v01[] =
{&cnss_platform_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object cnss_platform_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(cnss_platform_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  cnss_platform_message_table_v01,
  cnss_platform_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry cnss_platform_service_command_messages_v01[] = {
  {CNSS_PLAT_IPC_QMI_INIT_SETUP_REQ_V01, QMI_IDL_TYPE16(0, 0), 15},
  {CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_REQ_V01, QMI_IDL_TYPE16(0, 3), 61470},
  {CNSS_PLAT_IPC_QMI_FILE_UPLOAD_REQ_V01, QMI_IDL_TYPE16(0, 6), 14}
};

static const qmi_idl_service_message_table_entry cnss_platform_service_response_messages_v01[] = {
  {CNSS_PLAT_IPC_QMI_INIT_SETUP_RESP_V01, QMI_IDL_TYPE16(0, 1), 18},
  {CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_RESP_V01, QMI_IDL_TYPE16(0, 4), 21},
  {CNSS_PLAT_IPC_QMI_FILE_UPLOAD_RESP_V01, QMI_IDL_TYPE16(0, 7), 61470}
};

static const qmi_idl_service_message_table_entry cnss_platform_service_indication_messages_v01[] = {
  {CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_IND_V01, QMI_IDL_TYPE16(0, 2), 42},
  {CNSS_PLAT_IPC_QMI_FILE_UPLOAD_IND_V01, QMI_IDL_TYPE16(0, 5), 49}
};

/*Service Object*/
struct qmi_idl_service_object cnss_platform_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x42E,
  61470,
  { sizeof(cnss_platform_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(cnss_platform_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(cnss_platform_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { cnss_platform_service_command_messages_v01, cnss_platform_service_response_messages_v01, cnss_platform_service_indication_messages_v01},
  &cnss_platform_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type cnss_platform_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( CNSS_PLATFORM_V01_IDL_MAJOR_VERS != idl_maj_version || CNSS_PLATFORM_V01_IDL_MINOR_VERS != idl_min_version
       || CNSS_PLATFORM_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&cnss_platform_qmi_idl_service_object_v01;
}

