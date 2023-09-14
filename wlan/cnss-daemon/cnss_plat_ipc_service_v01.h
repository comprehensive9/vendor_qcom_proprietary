#ifndef CNSS_PLATFORM_SERVICE_01_H
#define CNSS_PLATFORM_SERVICE_01_H
/**
  @file cnss_plat_ipc_service_v01.h

  @brief This is the public header file which defines the cnss_platform service Data structures.

  This header file defines the types and structures that were defined in
  cnss_platform. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
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

/** @defgroup cnss_platform_qmi_consts Constant values defined in the IDL */
/** @defgroup cnss_platform_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup cnss_platform_qmi_enums Enumerated types used in QMI messages */
/** @defgroup cnss_platform_qmi_messages Structures sent as QMI messages */
/** @defgroup cnss_platform_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup cnss_platform_qmi_accessor Accessor for QMI service object */
/** @defgroup cnss_platform_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup cnss_platform_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define CNSS_PLATFORM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define CNSS_PLATFORM_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define CNSS_PLATFORM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define CNSS_PLATFORM_V01_MAX_MESSAGE_ID 0x0005
/**
    @}
  */


/** @addtogroup cnss_platform_qmi_consts
    @{
  */

/**  Max data size 60KB */
#define CNSS_PLAT_IPC_QMI_MAX_DATA_SIZE_V01 61440

/**  Max QMI msg size including the data buffer */
#define CNSS_PLAT_IPC_QMI_MAX_MSG_SIZE_V01 65535

/**  File name in root fs */
#define CNSS_PLAT_IPC_QMI_MAX_FILE_NAME_LEN_V01 32
/**
    @}
  */

/**  CNSS Platform driver status bitmask */
typedef uint64_t cnss_driver_status_bitmask_v01;
#define CNSS_PLAT_IPC_QMI_DRIVER_CBC_DONE_V01 ((cnss_driver_status_bitmask_v01)0x01ull) /**<  Cold boot calibration complete  */
#define CNSS_PLAT_IPC_QMI_DRIVER_WLAN_ACTIVE_V01 ((cnss_driver_status_bitmask_v01)0x02ull) /**<  WLAN Driver is active  */
/** @addtogroup cnss_platform_qmi_messages
    @{
  */
/** Request Message; This command is for exchanging setup information */
typedef struct {

  /* Mandatory */
  /*  Platform Modem (DMS) support for MAC Address provisioning in NV */
  uint8_t dms_mac_addr_supported;

  /* Mandatory */
  /*  QDSS override - Android Property */
  uint8_t qdss_hw_trace_override;

  /* Mandatory */
  /*  CNSS Calibration file availability. Uses wlfw_cal_temp_id_enum from wlan_firmware_service IDL */
  uint32_t cal_file_available_bitmask;
}cnss_plat_ipc_qmi_init_setup_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cnss_platform_qmi_messages
    @{
  */
/** Response Message; This command is for exchanging setup information */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Mandatory */
  /*  CNSS Platform driver status */
  cnss_driver_status_bitmask_v01 drv_status;
}cnss_plat_ipc_qmi_init_setup_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cnss_platform_qmi_messages
    @{
  */
/** Indication Message; This command sends file download indication from cnss platform driver  */
typedef struct {

  /* Mandatory */
  /*  File name */
  char file_name[CNSS_PLAT_IPC_QMI_MAX_FILE_NAME_LEN_V01 + 1];

  /* Mandatory */
  /*  File ID corresponding to file name. Used in file download request from daemon */
  uint32_t file_id;
}cnss_plat_ipc_qmi_file_download_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cnss_platform_qmi_messages
    @{
  */
/** Request Message; This command sends file data from daemon to platform driver  */
typedef struct {

  /* Mandatory */
  /*  File ID corresponding to file name. Received from file download indication from driver */
  uint32_t file_id;

  /* Mandatory */
  /*  File total size */
  uint32_t file_size;

  /* Mandatory */
  /*  File download msg end marker. Last segment is identified based on this. */
  uint8_t end;

  /* Mandatory */
  /*  File buf seg index */
  uint32_t seg_index;

  /* Mandatory */
  /*  File seg buf */
  uint32_t seg_buf_len;  /**< Must be set to # of elements in seg_buf */
  uint8_t seg_buf[CNSS_PLAT_IPC_QMI_MAX_DATA_SIZE_V01];
}cnss_plat_ipc_qmi_file_download_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cnss_platform_qmi_messages
    @{
  */
/** Response Message; This command sends file data from daemon to platform driver  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Mandatory */
  /*  File ID corresponding to file name. */
  uint32_t file_id;

  /* Mandatory */
  /*  Acknowledge status of file buf seg index received. */
  uint32_t seg_index;
}cnss_plat_ipc_qmi_file_download_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cnss_platform_qmi_messages
    @{
  */
/** Indication Message; This command tells client to update local-stored file data */
typedef struct {

  /* Mandatory */
  /*  File name */
  char file_name[CNSS_PLAT_IPC_QMI_MAX_FILE_NAME_LEN_V01 + 1];

  /* Mandatory */
  /*  File ID corresponding to file name. */
  uint32_t file_id;

  /* Mandatory */
  /*  File total size */
  uint32_t file_size;
}cnss_plat_ipc_qmi_file_upload_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cnss_platform_qmi_messages
    @{
  */
/** Request Message; This command request file data from cnss platform driver  */
typedef struct {

  /* Mandatory */
  /*  File id */
  uint32_t file_id;

  /* Mandatory */
  /*  File buf seg index */
  uint32_t seg_index;
}cnss_plat_ipc_qmi_file_upload_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cnss_platform_qmi_messages
    @{
  */
/** Response Message; This command request file data from cnss platform driver  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Mandatory */
  /*  File id */
  uint32_t file_id;

  /* Mandatory */
  /*  File download msg end marker */
  uint8_t end;

  /* Mandatory */
  /*  File buf seg index */
  uint32_t seg_index;

  /* Mandatory */
  /*  File seg buf */
  uint32_t seg_buf_len;  /**< Must be set to # of elements in seg_buf */
  uint8_t seg_buf[CNSS_PLAT_IPC_QMI_MAX_DATA_SIZE_V01];
}cnss_plat_ipc_qmi_file_upload_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_V01
//#define REMOVE_CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_IND_V01
//#define REMOVE_CNSS_PLAT_IPC_QMI_FILE_UPLOAD_V01
//#define REMOVE_CNSS_PLAT_IPC_QMI_FILE_UPLOAD_IND_V01
//#define REMOVE_CNSS_PLAT_IPC_QMI_INIT_SETUP_V01

/*Service Message Definition*/
/** @addtogroup cnss_platform_qmi_msg_ids
    @{
  */
#define CNSS_PLAT_IPC_QMI_INIT_SETUP_REQ_V01 0x0001
#define CNSS_PLAT_IPC_QMI_INIT_SETUP_RESP_V01 0x0001
#define CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_IND_V01 0x0002
#define CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_REQ_V01 0x0003
#define CNSS_PLAT_IPC_QMI_FILE_DOWNLOAD_RESP_V01 0x0003
#define CNSS_PLAT_IPC_QMI_FILE_UPLOAD_IND_V01 0x0004
#define CNSS_PLAT_IPC_QMI_FILE_UPLOAD_REQ_V01 0x0005
#define CNSS_PLAT_IPC_QMI_FILE_UPLOAD_RESP_V01 0x0005
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro cnss_platform_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type cnss_platform_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define cnss_platform_get_service_object_v01( ) \
          cnss_platform_get_service_object_internal_v01( \
            CNSS_PLATFORM_V01_IDL_MAJOR_VERS, CNSS_PLATFORM_V01_IDL_MINOR_VERS, \
            CNSS_PLATFORM_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

