/*==========================================================================

                     FM Test App Common Header File

Description
  Global Data declarations of the fm test common ccomponent.

# Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================

                         Edit History


when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#include <sys/types.h>

typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef unsigned char boolean;

#define FALSE 0
#define TRUE 1
/* FM power state enum */
typedef enum
{
  FM_POWER_OFF,
  FM_POWER_TRANSITION,
  FM_POWER_ON
}fm_power_state;

/* FM command status enum */
typedef enum
{
  FM_CMD_SUCCESS,
  FM_CMD_PENDING,
  FM_CMD_NO_RESOURCES,
  FM_CMD_INVALID_PARAM,
  FM_CMD_DISALLOWED,
  FM_CMD_UNRECOGNIZED_CMD,
  FM_CMD_FAILURE
}fm_cmd_status_type;

enum {
    FM_SUCCESS,
    FM_FAILURE
};

typedef enum radio_band_type
{
  FM_RX_US_EUROPE = 0x1,
  FM_RX_JAPAN_STANDARD = 0x2,
  FM_RX_JAPAN_WIDE = 0x3,
  FM_RX_USER_DEFINED = 0x4
}radio_band_type;

typedef struct band_limit_freq
{
  uint32 lower_limit;
  uint32 upper_limit;
}band_limit_freq;


typedef enum rds_sync_type
{
 FM_RDS_NOT_SYNCED = 0x0,
 FM_RDS_SYNCED = 0x1
}rds_sync_type;

typedef enum stereo_type
{
 FM_RX_MONO = 0x0,
 FM_RX_STEREO = 0x1
}stereo_type;

typedef enum fm_service_available
{
  FM_SERVICE_NOT_AVAILABLE = 0x0,
  FM_SERVICE_AVAILABLE = 0x1
}fm_service_available;

typedef enum mute_type
{
  FM_RX_NO_MUTE = 0x00,
  FM_RX_MUTE_RIGHT = 0x01,
  FM_RX_MUTE_LEFT = 0x02,
  FM_RX_MUTE_BOTH = 0x03
}mute_type;

/**
*  RDS/RBDS Program Type type.
*/
typedef uint8  fm_prgm_type;

/**
*   RDS/RBDS Program Identification type.
*/
typedef uint16 fm_prgmid_type;
/**
*  RDS/RBDS Program Services type.
*/
typedef char fm_prm_services;
/**
*  RDS/RBDS Radio Text type.
*/
typedef char fm_radiotext_info;
/**
*  FM Global Paramaters struct.
*/
typedef struct
{
  uint32  current_station_freq;/*a frequency in kHz the band range*/
  uint8 service_available;
  uint8 rssi; /* rssi range from 0-100*/
  uint8 stype;
  uint8 rds_sync_status;
  uint8 mute_status;
  fm_prgmid_type pgm_id; /* Program Id */
  fm_prgm_type pgm_type; /* Program type */
  fm_prm_services  pgm_services[96]; /*Program services Can maximum hold 96*/
  fm_radiotext_info  radio_text[64];/* RT maximum is 64 bytes */
}fm_station_params_available;
/**
*  FM Config Request structure.
*/
typedef struct fm_config_data
{
  uint8 band;
  uint8 emphasis;
  uint8 spacing;
  uint8 rds_system;
  band_limit_freq bandlimits;
}fm_config_data;

/*
* FM RDS Options Config Request
*/
typedef struct fm_rds_options
{
  uint32 rds_group_mask;
  uint32 rds_group_buffer_size;
  uint8 rds_change_filter;
}fm_rds_options;
/*
* FM RX Search stations request
*/
typedef struct fm_search_stations
{
  uint8 search_mode;
  uint8 dwell_period;
  uint8 search_dir;
}fm_search_stations;

/*
* FM RX Search DDS stations request
*/
typedef struct fm_search_rds_stations
{
  uint8 search_mode;
  uint8 dwell_period;
  uint8 search_dir;
  uint8 program_type;
  uint16 program_id;
}fm_search_rds_stations;

/*
* FM RX Search station lists request
*/
typedef struct fm_search_list_stations
{
  uint8 search_mode;
  uint8 search_dir;
  uint32 srch_list_max;
  /**< Maximum number of stations that can be returned from a search. */
  uint8 program_type;
} __attribute__ ((packed)) fm_search_list_stations;

/*
* FM RX I2C request
*/
typedef struct fm_i2c_params
{
  uint8 slaveaddress;
  uint8 offset;
  uint8 payload_length;
  uint8 data[64];
}fm_i2c_params;

/**
*  FM All Request Union type.
*/
typedef union fm_cfg_request
{
  fm_config_data cfg_param;
  uint8 mute_param;
  uint8 stereo_param;
  uint32 freq;
  fm_rds_options rds_options;
  uint8  power_mode;
  uint8  signal_threshold;
  fm_search_stations search_stations_options;
  fm_search_rds_stations search_rds_stations_options;
  fm_search_list_stations search_list_stations_options;
  fm_i2c_params i2c_params;
  uint32 rds_group_options;
}fm_cfg_request;

enum emphasis_type {
 FM_RX_EMP75 = 0x0,
 FM_RX_EMP50 = 0x1
};

enum channel_space_type {
 FM_RX_SPACE_200KHZ = 0x0,
 FM_RX_SPACE_100KHZ = 0x1,
 FM_RX_SPACE_50KHZ = 0x2
};

enum fm_rds_type {
 FM_RX_RDBS_SYSTEM = 0x0,
 FM_RX_RDS_SYSTEM = 0x1
};

enum iris_evt_t {
 IRIS_EVT_RADIO_READY,
 IRIS_EVT_TUNE_SUCC,
 IRIS_EVT_SEEK_COMPLETE,
 IRIS_EVT_SCAN_NEXT,
 IRIS_EVT_NEW_RAW_RDS,
 IRIS_EVT_NEW_RT_RDS,
 IRIS_EVT_NEW_PS_RDS,
 IRIS_EVT_ERROR,
 IRIS_EVT_BELOW_TH,
 IRIS_EVT_ABOVE_TH,
 IRIS_EVT_STEREO,
 IRIS_EVT_MONO,
 IRIS_EVT_RDS_AVAIL,
 IRIS_EVT_RDS_NOT_AVAIL,
 IRIS_EVT_NEW_SRCH_LIST,
 IRIS_EVT_NEW_AF_LIST,
 IRIS_EVT_TXRDSDAT,
 IRIS_EVT_TXRDSDONE,
 IRIS_EVT_RADIO_DISABLED,
 IRIS_EVT_NEW_ODA,
 IRIS_EVT_NEW_RT_PLUS,
 IRIS_EVT_NEW_ERT,
 IRIS_EVT_SPUR_TBL,
};
enum v4l2_cid_private_iris_t {
 V4L2_CID_PRIVATE_IRIS_SRCHMODE = (0x08000000 + 1),
 V4L2_CID_PRIVATE_IRIS_SCANDWELL,
 V4L2_CID_PRIVATE_IRIS_SRCHON,
 V4L2_CID_PRIVATE_IRIS_STATE,
 V4L2_CID_PRIVATE_IRIS_TRANSMIT_MODE,
 V4L2_CID_PRIVATE_IRIS_RDSGROUP_MASK,
 V4L2_CID_PRIVATE_IRIS_REGION,
 V4L2_CID_PRIVATE_IRIS_SIGNAL_TH,
 V4L2_CID_PRIVATE_IRIS_SRCH_PTY,
 V4L2_CID_PRIVATE_IRIS_SRCH_PI,
 V4L2_CID_PRIVATE_IRIS_SRCH_CNT,
 V4L2_CID_PRIVATE_IRIS_EMPHASIS,
 V4L2_CID_PRIVATE_IRIS_RDS_STD,
 V4L2_CID_PRIVATE_IRIS_SPACING,
 V4L2_CID_PRIVATE_IRIS_RDSON,
 V4L2_CID_PRIVATE_IRIS_RDSGROUP_PROC,
 V4L2_CID_PRIVATE_IRIS_LP_MODE,
 V4L2_CID_PRIVATE_IRIS_ANTENNA,
 V4L2_CID_PRIVATE_IRIS_RDSD_BUF,
 V4L2_CID_PRIVATE_IRIS_PSALL,  /*0x8000014*/

 /*v4l2 Tx controls*/
 V4L2_CID_PRIVATE_IRIS_TX_SETPSREPEATCOUNT,
 V4L2_CID_PRIVATE_IRIS_STOP_RDS_TX_PS_NAME,
 V4L2_CID_PRIVATE_IRIS_STOP_RDS_TX_RT,
 V4L2_CID_PRIVATE_IRIS_IOVERC,
 V4L2_CID_PRIVATE_IRIS_INTDET,
 V4L2_CID_PRIVATE_IRIS_MPX_DCC,
 V4L2_CID_PRIVATE_IRIS_AF_JUMP,
 V4L2_CID_PRIVATE_IRIS_RSSI_DELTA,
 V4L2_CID_PRIVATE_IRIS_HLSI, /*0x800001d*/

 /*Diagnostic commands*/
 V4L2_CID_PRIVATE_IRIS_SOFT_MUTE,
 V4L2_CID_PRIVATE_IRIS_RIVA_ACCS_ADDR,
 V4L2_CID_PRIVATE_IRIS_RIVA_ACCS_LEN,
 V4L2_CID_PRIVATE_IRIS_RIVA_PEEK,
 V4L2_CID_PRIVATE_IRIS_RIVA_POKE,
 V4L2_CID_PRIVATE_IRIS_SSBI_ACCS_ADDR,
 V4L2_CID_PRIVATE_IRIS_SSBI_PEEK,
 V4L2_CID_PRIVATE_IRIS_SSBI_POKE,
 V4L2_CID_PRIVATE_IRIS_TX_TONE,
 V4L2_CID_PRIVATE_IRIS_RDS_GRP_COUNTERS,
 V4L2_CID_PRIVATE_IRIS_SET_NOTCH_FILTER, /* 0x8000028 */
 V4L2_CID_PRIVATE_IRIS_SET_AUDIO_PATH, /* TAVARUA specific command */
 V4L2_CID_PRIVATE_IRIS_DO_CALIBRATION,
 V4L2_CID_PRIVATE_IRIS_SRCH_ALGORITHM, /* TAVARUA specific command */
 V4L2_CID_PRIVATE_IRIS_GET_SINR,
 V4L2_CID_PRIVATE_INTF_LOW_THRESHOLD,
 V4L2_CID_PRIVATE_INTF_HIGH_THRESHOLD,
 V4L2_CID_PRIVATE_SINR_THRESHOLD,
 V4L2_CID_PRIVATE_SINR_SAMPLES,
 V4L2_CID_PRIVATE_SPUR_FREQ,
 V4L2_CID_PRIVATE_SPUR_FREQ_RMSSI,
 V4L2_CID_PRIVATE_SPUR_SELECTION,
 V4L2_CID_PRIVATE_UPDATE_SPUR_TABLE,
 V4L2_CID_PRIVATE_VALID_CHANNEL,
 V4L2_CID_PRIVATE_AF_RMSSI_TH,
 V4L2_CID_PRIVATE_AF_RMSSI_SAMPLES,
 V4L2_CID_PRIVATE_GOOD_CH_RMSSI_TH,
 V4L2_CID_PRIVATE_SRCHALGOTYPE,
 V4L2_CID_PRIVATE_CF0TH12,
 V4L2_CID_PRIVATE_SINRFIRSTSTAGE,
 V4L2_CID_PRIVATE_RMSSIFIRSTSTAGE,
 V4L2_CID_PRIVATE_RXREPEATCOUNT,
 V4L2_CID_PRIVATE_IRIS_RSSI_TH,
 V4L2_CID_PRIVATE_IRIS_AF_JUMP_RSSI_TH,
 V4L2_CID_PRIVATE_BLEND_SINRHI,
 V4L2_CID_PRIVATE_BLEND_RMSSIHI,

 /*using private CIDs under userclass*/
 V4L2_CID_PRIVATE_IRIS_READ_DEFAULT = 0x00980928,
 V4L2_CID_PRIVATE_IRIS_WRITE_DEFAULT,
 V4L2_CID_PRIVATE_IRIS_SET_CALIBRATION,
 V4L2_CID_PRIVATE_IRIS_SET_SPURTABLE = 0x0098092D,
 V4L2_CID_PRIVATE_IRIS_GET_SPUR_TBL  = 0x0098092E,
};
