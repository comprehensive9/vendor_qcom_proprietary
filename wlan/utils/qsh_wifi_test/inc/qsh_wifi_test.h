/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef QSH_WIFI_TEST_H
#define QSH_WIFI_TEST_H

#include "qmi_client.h"
#include "pb_decode.h"
#include "pb_encode.h"

#include "sns_client_api_v01.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_client.pb.h"
#include "sns_suid.pb.h"
#include "qsh_wifi.pb.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif


#define MAC_ADDR_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

#define MACADDR(mac_addr) \
  mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]

#define TAG "QSH_WIFI_TEST"
/* Timeout for CMD response */
#define DEF_CMD_TIMEOUT_MS 1000 // 10 seconds
#define NS_PER_SEC 1000000000 //nano seconds in per second
#define MS_PER_SEC 1000       //mili seconds in per second
#define NS_PER_MS (NS_PER_SEC/MS_PER_SEC)
#define CMD_TIMEOUT_IN_MS 5000

#define MAX_AP_IN_SCAN_EVT    15
#define MAX_FREQ_IN_SCAN_EVT 128
#define MAX_SCAN_EVENTS       20
#define MAX_SSID_CNT          10
#define MAX_SSID_SIZE         32
#define MAX_REQ_RANGING_BSS   20
#define MAX_FRAG_RANGING_BSS  10
#define MAX_RTT_EVENTS        2

#define SCAN_FILE_PATH "/data/scan.xml"
const xmlChar XML_NODE_SCAN[]            = "scan";
const xmlChar XML_NODE_SCAN_TYPE[]       = "scan_type";
const xmlChar XML_NODE_SCAN_AGE_MS[]     = "age_in_ms";
const xmlChar XML_NODE_BAND[]            = "band";
const xmlChar XML_NODE_RADIO_CHAIN[]     = "radio_chain";
const xmlChar XML_NODE_FREQS[]           = "freqs";
const xmlChar XML_NODE_SSIDS[]           = "ssids";

#define RTT_FILE_PATH "/data/rtt.xml"
const xmlChar XML_NODE_RTT[]            = "rtt";
const xmlChar XML_NODE_AP[]             = "ap";
const xmlChar XML_NODE_MAC[]            = "mac";
const xmlChar XML_NODE_FREQ[]           = "freq";
const xmlChar XML_NODE_CF0[]            = "cent_freq0";
const xmlChar XML_NODE_CF1[]            = "cent_freq1";
const xmlChar XML_NODE_CW[]             = "ch_width";
const xmlChar XML_NODE_LCI[]            = "lci";
const xmlChar XML_NODE_NUM_FRAMES[]     = "num_frames";
/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/**
 * Utility structure used to encode a string or already encoded message
 * into another Protocol Buffer message.
 */
typedef struct sns_buffer_arg
{
  /* Buffer to be written, or reference to read buffer */
  void const *buf;
  /* Length of buf */
  size_t buf_len;
} sns_buffer_arg;

/**
 * Structure to be used with pb_decode_float_arr_cb
 */
typedef struct pb_float_arr_arg
{
  /* Where to place the decoded float array */
  float *arr;
  /* Length of the float array */
  uint8_t arr_len;
  /* Array index used for decoding.
     Must be initialized to 0 prior to calling pb_decode_float_arr_cb*/
  uint8_t *arr_index;
} pb_float_arr_arg;

typedef struct _qsh_wifi_scan_db {
    qsh_wifi_scan_type scan_type;
    bool has_max_scan_age_ms;
    uint32_t max_scan_age_ms;
    uint32_t band;
    uint32_t freq_count;
    uint32_t freq_list[MAX_FREQ_IN_SCAN_EVT];
    uint32_t ssid_count;
    uint32_t ssid_list[MAX_SSID_CNT][MAX_SSID_SIZE+1];
    bool has_radio_chain_pref;
    qsh_wifi_pref_radio_chain radio_chain_pref;
/* @@protoc_insertion_point(struct:qsh_wifi_cmd_scan_req) */
} qsh_wifi_scan_db;

typedef struct _qsh_wifi_scan_evt_db
{
  qsh_wifi_error status;
  uint32_t result_total;
  uint32_t last_in_series;
  bool has_scan_type;
  uint32_t scan_type;
  bool has_ref_time;
  uint64_t ref_time;
  uint32_t freq_count;
  uint32_t freq_array[MAX_FREQ_IN_SCAN_EVT];
  uint32_t result_count;
  qsh_wifi_bss_info ap_array[MAX_AP_IN_SCAN_EVT];
  bool has_radio_chain_pref;
  uint32_t radio_chain_pref;
}qsh_wifi_scan_evt_db;

typedef struct _qsh_wifi_ranging_req_db
{
  uint32_t bss_count;
  qsh_wifi_ranging_req_ap bss_list[MAX_REQ_RANGING_BSS];
}qsh_wifi_ranging_req_db;

typedef struct _qsh_wifi_ranging_evt_db
{
  bool last_in_series;
  bool has_ref_time;
  uint16_t result_count;
  qsh_wifi_error status;
  uint32_t result_total;
  qsh_wifi_ranging_result_ap bss_list[MAX_REQ_RANGING_BSS];
}qsh_wifi_ranging_evt_db;

typedef struct _qsh_wifi_ranging_evt_frag
{
  bool last_in_series;
  bool has_ref_time;
  uint16_t result_count;
  qsh_wifi_error status;
  uint32_t result_total;
  uint64_t ref_time;
  qsh_wifi_ranging_result_ap *bss_list;
}qsh_wifi_ranging_evt_frag;

enum CMDS
{
  OPEN                           = 1,
  MONITOR_MODE_ON                = 2,
  MONITOR_MODE_OFF               = 3,
  ON_DEMAND_SCAN                 = 4,
  ON_DEMAND_SCAN_USER_PARAMS     = 5,
  RTT_SCAN                       = 6,

  DUMP_EVENT                     = 8,
  EXIT                           = 9,
};

enum init_state
{
  INIT   = 0,
  DEINIT = 1,
  OPENED = 2,
};

int parse_rtt_xml(const char * const rtt_xml,
    qsh_wifi_ranging_req_db *cmd );
#endif //QSH_WIFI_TEST_H
