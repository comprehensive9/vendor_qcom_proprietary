/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.9.5 at Tue Oct 27 14:27:29 2020. */

#ifndef PB_QSH_WIFI_PB_H_INCLUDED
#define PB_QSH_WIFI_PB_H_INCLUDED
#include <pb.h>

#include "sns_std.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _qsh_wifi_msgid {
    QSH_WIFI_MSGID_QSH_WIFI_CMD_OPEN = 512,
    QSH_WIFI_MSGID_QSH_WIFI_CMD_SCAN_REQ = 513,
    QSH_WIFI_MSGID_QSH_WIFI_CMD_MONITOR_REQ = 514,
    QSH_WIFI_MSGID_QSH_WIFI_CMD_RANGING_REQ = 515,
    QSH_WIFI_MSGID_QSH_WIFI_CMD_ACK = 768,
    QSH_WIFI_MSGID_QSH_WIFI_EVT_SCAN_RESULTS = 769,
    QSH_WIFI_MSGID_QSH_WIFI_EVT_RANGING_RESULTS = 770
} qsh_wifi_msgid;
#define _qsh_wifi_msgid_MIN QSH_WIFI_MSGID_QSH_WIFI_CMD_OPEN
#define _qsh_wifi_msgid_MAX QSH_WIFI_MSGID_QSH_WIFI_EVT_RANGING_RESULTS
#define _qsh_wifi_msgid_ARRAYSIZE ((qsh_wifi_msgid)(QSH_WIFI_MSGID_QSH_WIFI_EVT_RANGING_RESULTS+1))
#define qsh_wifi_msgid_QSH_WIFI_MSGID_QSH_WIFI_CMD_OPEN QSH_WIFI_MSGID_QSH_WIFI_CMD_OPEN
#define qsh_wifi_msgid_QSH_WIFI_MSGID_QSH_WIFI_CMD_SCAN_REQ QSH_WIFI_MSGID_QSH_WIFI_CMD_SCAN_REQ
#define qsh_wifi_msgid_QSH_WIFI_MSGID_QSH_WIFI_CMD_MONITOR_REQ QSH_WIFI_MSGID_QSH_WIFI_CMD_MONITOR_REQ
#define qsh_wifi_msgid_QSH_WIFI_MSGID_QSH_WIFI_CMD_RANGING_REQ QSH_WIFI_MSGID_QSH_WIFI_CMD_RANGING_REQ
#define qsh_wifi_msgid_QSH_WIFI_MSGID_QSH_WIFI_CMD_ACK QSH_WIFI_MSGID_QSH_WIFI_CMD_ACK
#define qsh_wifi_msgid_QSH_WIFI_MSGID_QSH_WIFI_EVT_SCAN_RESULTS QSH_WIFI_MSGID_QSH_WIFI_EVT_SCAN_RESULTS
#define qsh_wifi_msgid_QSH_WIFI_MSGID_QSH_WIFI_EVT_RANGING_RESULTS QSH_WIFI_MSGID_QSH_WIFI_EVT_RANGING_RESULTS

typedef enum _qsh_wifi_error {
    QSH_WIFI_ERROR_NONE = 0,
    QSH_WIFI_ERROR = 1,
    QSH_WIFI_ERROR_INVALID_ARGS = 2,
    QSH_WIFI_ERROR_BUSY = 3,
    QSH_WIFI_ERROR_NO_MEMORY = 4,
    QSH_WIFI_ERROR_NOT_SUPPORTED = 5,
    QSH_WIFI_ERROR_TIMEOUT = 6,
    QSH_WIFI_ERROR_INTF_DISABLED = 7
} qsh_wifi_error;
#define _qsh_wifi_error_MIN QSH_WIFI_ERROR_NONE
#define _qsh_wifi_error_MAX QSH_WIFI_ERROR_INTF_DISABLED
#define _qsh_wifi_error_ARRAYSIZE ((qsh_wifi_error)(QSH_WIFI_ERROR_INTF_DISABLED+1))
#define qsh_wifi_error_QSH_WIFI_ERROR_NONE QSH_WIFI_ERROR_NONE
#define qsh_wifi_error_QSH_WIFI_ERROR QSH_WIFI_ERROR
#define qsh_wifi_error_QSH_WIFI_ERROR_INVALID_ARGS QSH_WIFI_ERROR_INVALID_ARGS
#define qsh_wifi_error_QSH_WIFI_ERROR_BUSY QSH_WIFI_ERROR_BUSY
#define qsh_wifi_error_QSH_WIFI_ERROR_NO_MEMORY QSH_WIFI_ERROR_NO_MEMORY
#define qsh_wifi_error_QSH_WIFI_ERROR_NOT_SUPPORTED QSH_WIFI_ERROR_NOT_SUPPORTED
#define qsh_wifi_error_QSH_WIFI_ERROR_TIMEOUT QSH_WIFI_ERROR_TIMEOUT
#define qsh_wifi_error_QSH_WIFI_ERROR_INTF_DISABLED QSH_WIFI_ERROR_INTF_DISABLED

typedef enum _qsh_wifi_capabilities {
    QSH_WIFI_CAP_NONE = 0,
    QSH_WIFI_CAP_SCAN_MONITORING = 1,
    QSH_WIFI_CAP_SCAN_REQUEST_BASED = 2,
    QSH_WIFI_CAP_RANGING_SCAN = 4
} qsh_wifi_capabilities;
#define _qsh_wifi_capabilities_MIN QSH_WIFI_CAP_NONE
#define _qsh_wifi_capabilities_MAX QSH_WIFI_CAP_RANGING_SCAN
#define _qsh_wifi_capabilities_ARRAYSIZE ((qsh_wifi_capabilities)(QSH_WIFI_CAP_RANGING_SCAN+1))
#define qsh_wifi_capabilities_QSH_WIFI_CAP_NONE QSH_WIFI_CAP_NONE
#define qsh_wifi_capabilities_QSH_WIFI_CAP_SCAN_MONITORING QSH_WIFI_CAP_SCAN_MONITORING
#define qsh_wifi_capabilities_QSH_WIFI_CAP_SCAN_REQUEST_BASED QSH_WIFI_CAP_SCAN_REQUEST_BASED
#define qsh_wifi_capabilities_QSH_WIFI_CAP_RANGING_SCAN QSH_WIFI_CAP_RANGING_SCAN

typedef enum _qsh_wifi_band_mask {
    QSH_WIFI_BAND_MASK_2P4_GHZ = 1,
    QSH_WIFI_BAND_MASK_5_GHZ = 2
} qsh_wifi_band_mask;
#define _qsh_wifi_band_mask_MIN QSH_WIFI_BAND_MASK_2P4_GHZ
#define _qsh_wifi_band_mask_MAX QSH_WIFI_BAND_MASK_5_GHZ
#define _qsh_wifi_band_mask_ARRAYSIZE ((qsh_wifi_band_mask)(QSH_WIFI_BAND_MASK_5_GHZ+1))
#define qsh_wifi_band_mask_QSH_WIFI_BAND_MASK_2P4_GHZ QSH_WIFI_BAND_MASK_2P4_GHZ
#define qsh_wifi_band_mask_QSH_WIFI_BAND_MASK_5_GHZ QSH_WIFI_BAND_MASK_5_GHZ

typedef enum _qsh_wifi_radio_chain_index {
    QSH_WIFI_RADIO_CHAIN_INDEX_UNKNOWN = 0,
    QSH_WIFI_RADIO_CHAIN_INDEX_0 = 1,
    QSH_WIFI_RADIO_CHAIN_INDEX_1 = 2
} qsh_wifi_radio_chain_index;
#define _qsh_wifi_radio_chain_index_MIN QSH_WIFI_RADIO_CHAIN_INDEX_UNKNOWN
#define _qsh_wifi_radio_chain_index_MAX QSH_WIFI_RADIO_CHAIN_INDEX_1
#define _qsh_wifi_radio_chain_index_ARRAYSIZE ((qsh_wifi_radio_chain_index)(QSH_WIFI_RADIO_CHAIN_INDEX_1+1))
#define qsh_wifi_radio_chain_index_QSH_WIFI_RADIO_CHAIN_INDEX_UNKNOWN QSH_WIFI_RADIO_CHAIN_INDEX_UNKNOWN
#define qsh_wifi_radio_chain_index_QSH_WIFI_RADIO_CHAIN_INDEX_0 QSH_WIFI_RADIO_CHAIN_INDEX_0
#define qsh_wifi_radio_chain_index_QSH_WIFI_RADIO_CHAIN_INDEX_1 QSH_WIFI_RADIO_CHAIN_INDEX_1

typedef enum _qsh_wifi_scan_type {
    QSH_WIFI_SCAN_TYPE_ACTIVE = 0,
    QSH_WIFI_SCAN_TYPE_ACTIVE_AND_PASSIVE_DFS = 1,
    QSH_WIFI_SCAN_TYPE_PASSIVE = 2
} qsh_wifi_scan_type;
#define _qsh_wifi_scan_type_MIN QSH_WIFI_SCAN_TYPE_ACTIVE
#define _qsh_wifi_scan_type_MAX QSH_WIFI_SCAN_TYPE_PASSIVE
#define _qsh_wifi_scan_type_ARRAYSIZE ((qsh_wifi_scan_type)(QSH_WIFI_SCAN_TYPE_PASSIVE+1))
#define qsh_wifi_scan_type_QSH_WIFI_SCAN_TYPE_ACTIVE QSH_WIFI_SCAN_TYPE_ACTIVE
#define qsh_wifi_scan_type_QSH_WIFI_SCAN_TYPE_ACTIVE_AND_PASSIVE_DFS QSH_WIFI_SCAN_TYPE_ACTIVE_AND_PASSIVE_DFS
#define qsh_wifi_scan_type_QSH_WIFI_SCAN_TYPE_PASSIVE QSH_WIFI_SCAN_TYPE_PASSIVE

typedef enum _qsh_wifi_security {
    QSH_WIFI_SECURITY_MODE_UNKNOWN = 0,
    QSH_WIFI_SECURITY_MODE_OPEN = 1,
    QSH_WIFI_SECURITY_MODE_WEP = 2,
    QSH_WIFI_SECURITY_MODE_PSK = 3,
    QSH_WIFI_SECURITY_MODE_EAP = 4
} qsh_wifi_security;
#define _qsh_wifi_security_MIN QSH_WIFI_SECURITY_MODE_UNKNOWN
#define _qsh_wifi_security_MAX QSH_WIFI_SECURITY_MODE_EAP
#define _qsh_wifi_security_ARRAYSIZE ((qsh_wifi_security)(QSH_WIFI_SECURITY_MODE_EAP+1))
#define qsh_wifi_security_QSH_WIFI_SECURITY_MODE_UNKNOWN QSH_WIFI_SECURITY_MODE_UNKNOWN
#define qsh_wifi_security_QSH_WIFI_SECURITY_MODE_OPEN QSH_WIFI_SECURITY_MODE_OPEN
#define qsh_wifi_security_QSH_WIFI_SECURITY_MODE_WEP QSH_WIFI_SECURITY_MODE_WEP
#define qsh_wifi_security_QSH_WIFI_SECURITY_MODE_PSK QSH_WIFI_SECURITY_MODE_PSK
#define qsh_wifi_security_QSH_WIFI_SECURITY_MODE_EAP QSH_WIFI_SECURITY_MODE_EAP

typedef enum _qsh_wifi_channel_width {
    QSH_WIFI_CHANNEL_WIDTH_20 = 0,
    QSH_WIFI_CHANNEL_WIDTH_40 = 1,
    QSH_WIFI_CHANNEL_WIDTH_80 = 2,
    QSH_WIFI_CHANNEL_WIDTH_160 = 3,
    QSH_WIFI_CHANNEL_WIDTH_80_P_80 = 4
} qsh_wifi_channel_width;
#define _qsh_wifi_channel_width_MIN QSH_WIFI_CHANNEL_WIDTH_20
#define _qsh_wifi_channel_width_MAX QSH_WIFI_CHANNEL_WIDTH_80_P_80
#define _qsh_wifi_channel_width_ARRAYSIZE ((qsh_wifi_channel_width)(QSH_WIFI_CHANNEL_WIDTH_80_P_80+1))
#define qsh_wifi_channel_width_QSH_WIFI_CHANNEL_WIDTH_20 QSH_WIFI_CHANNEL_WIDTH_20
#define qsh_wifi_channel_width_QSH_WIFI_CHANNEL_WIDTH_40 QSH_WIFI_CHANNEL_WIDTH_40
#define qsh_wifi_channel_width_QSH_WIFI_CHANNEL_WIDTH_80 QSH_WIFI_CHANNEL_WIDTH_80
#define qsh_wifi_channel_width_QSH_WIFI_CHANNEL_WIDTH_160 QSH_WIFI_CHANNEL_WIDTH_160
#define qsh_wifi_channel_width_QSH_WIFI_CHANNEL_WIDTH_80_P_80 QSH_WIFI_CHANNEL_WIDTH_80_P_80

typedef enum _qsh_wifi_pref_radio_chain {
    QSH_WIFI_PREF_RADIO_CHAIN_DEFAULT = 0,
    QSH_WIFI_PREF_RADIO_CHAIN_LOW_LATENCY = 1,
    QSH_WIFI_PREF_RADIO_CHAIN_LOW_POWER = 2,
    QSH_WIFI_PREF_RADIO_CHAIN_HIGH_ACCURACY = 3
} qsh_wifi_pref_radio_chain;
#define _qsh_wifi_pref_radio_chain_MIN QSH_WIFI_PREF_RADIO_CHAIN_DEFAULT
#define _qsh_wifi_pref_radio_chain_MAX QSH_WIFI_PREF_RADIO_CHAIN_HIGH_ACCURACY
#define _qsh_wifi_pref_radio_chain_ARRAYSIZE ((qsh_wifi_pref_radio_chain)(QSH_WIFI_PREF_RADIO_CHAIN_HIGH_ACCURACY+1))
#define qsh_wifi_pref_radio_chain_QSH_WIFI_PREF_RADIO_CHAIN_DEFAULT QSH_WIFI_PREF_RADIO_CHAIN_DEFAULT
#define qsh_wifi_pref_radio_chain_QSH_WIFI_PREF_RADIO_CHAIN_LOW_LATENCY QSH_WIFI_PREF_RADIO_CHAIN_LOW_LATENCY
#define qsh_wifi_pref_radio_chain_QSH_WIFI_PREF_RADIO_CHAIN_LOW_POWER QSH_WIFI_PREF_RADIO_CHAIN_LOW_POWER
#define qsh_wifi_pref_radio_chain_QSH_WIFI_PREF_RADIO_CHAIN_HIGH_ACCURACY QSH_WIFI_PREF_RADIO_CHAIN_HIGH_ACCURACY

typedef enum _qsh_wifi_scan_result_flags {
    QSH_WIFI_SCAN_RESULT_FLAG_NONE = 0,
    QSH_WIFI_SCAN_RESULT_FLAG_HT_OPS_PRESENT = 1,
    QSH_WIFI_SCAN_RESULT_FLAG_VHT_OPS_PRESENT = 2,
    QSH_WIFI_SCAN_RESULT_FLAG_IS_FTM_RESPONDER = 4,
    QSH_WIFI_SCAN_RESULT_FLAG_HAS_SECONDARY_CHANNEL_OFFSET = 8,
    QSH_WIFI_SCAN_RESULT_FLAG_SECONDARY_CHANNEL_OFFSET_IS_BELOW = 16
} qsh_wifi_scan_result_flags;
#define _qsh_wifi_scan_result_flags_MIN QSH_WIFI_SCAN_RESULT_FLAG_NONE
#define _qsh_wifi_scan_result_flags_MAX QSH_WIFI_SCAN_RESULT_FLAG_SECONDARY_CHANNEL_OFFSET_IS_BELOW
#define _qsh_wifi_scan_result_flags_ARRAYSIZE ((qsh_wifi_scan_result_flags)(QSH_WIFI_SCAN_RESULT_FLAG_SECONDARY_CHANNEL_OFFSET_IS_BELOW+1))
#define qsh_wifi_scan_result_flags_QSH_WIFI_SCAN_RESULT_FLAG_NONE QSH_WIFI_SCAN_RESULT_FLAG_NONE
#define qsh_wifi_scan_result_flags_QSH_WIFI_SCAN_RESULT_FLAG_HT_OPS_PRESENT QSH_WIFI_SCAN_RESULT_FLAG_HT_OPS_PRESENT
#define qsh_wifi_scan_result_flags_QSH_WIFI_SCAN_RESULT_FLAG_VHT_OPS_PRESENT QSH_WIFI_SCAN_RESULT_FLAG_VHT_OPS_PRESENT
#define qsh_wifi_scan_result_flags_QSH_WIFI_SCAN_RESULT_FLAG_IS_FTM_RESPONDER QSH_WIFI_SCAN_RESULT_FLAG_IS_FTM_RESPONDER
#define qsh_wifi_scan_result_flags_QSH_WIFI_SCAN_RESULT_FLAG_HAS_SECONDARY_CHANNEL_OFFSET QSH_WIFI_SCAN_RESULT_FLAG_HAS_SECONDARY_CHANNEL_OFFSET
#define qsh_wifi_scan_result_flags_QSH_WIFI_SCAN_RESULT_FLAG_SECONDARY_CHANNEL_OFFSET_IS_BELOW QSH_WIFI_SCAN_RESULT_FLAG_SECONDARY_CHANNEL_OFFSET_IS_BELOW

typedef enum _qsh_wifi_monitor_action {
    QSH_WIFI_MONTIOR_ACTION_OFF = 0,
    QSH_WIFI_MONTIOR_ACTION_ON = 1
} qsh_wifi_monitor_action;
#define _qsh_wifi_monitor_action_MIN QSH_WIFI_MONTIOR_ACTION_OFF
#define _qsh_wifi_monitor_action_MAX QSH_WIFI_MONTIOR_ACTION_ON
#define _qsh_wifi_monitor_action_ARRAYSIZE ((qsh_wifi_monitor_action)(QSH_WIFI_MONTIOR_ACTION_ON+1))
#define qsh_wifi_monitor_action_QSH_WIFI_MONTIOR_ACTION_OFF QSH_WIFI_MONTIOR_ACTION_OFF
#define qsh_wifi_monitor_action_QSH_WIFI_MONTIOR_ACTION_ON QSH_WIFI_MONTIOR_ACTION_ON

typedef enum _qsh_wifi_rtt_status {
    QSH_WIFI_RTT_STATUS_SUCCESS = 0,
    QSH_WIFI_RTT_STATUS_FAILURE = 1,
    QSH_WIFI_RTT_STATUS_FAIL_NO_RSP = 2,
    QSH_WIFI_RTT_STATUS_FAIL_REJECTED = 3,
    QSH_WIFI_RTT_STATUS_FAIL_TM_TIMEOUT = 4,
    QSH_WIFI_RTT_STATUS_FAIL_AP_ON_DIFF_CHANNEL = 5,
    QSH_WIFI_RTT_STATUS_FAIL_NO_CAPABILITY = 6,
    QSH_WIFI_RTT_STATUS_FAIL_INVALID_TS = 7,
    QSH_WIFI_RTT_STATUS_FAIL_BUSY_TRY_LATER = 8,
    QSH_WIFI_RTT_STATUS_INVALID_REQ = 9,
    QSH_WIFI_RTT_STATUS_NO_WIFI = 10
} qsh_wifi_rtt_status;
#define _qsh_wifi_rtt_status_MIN QSH_WIFI_RTT_STATUS_SUCCESS
#define _qsh_wifi_rtt_status_MAX QSH_WIFI_RTT_STATUS_NO_WIFI
#define _qsh_wifi_rtt_status_ARRAYSIZE ((qsh_wifi_rtt_status)(QSH_WIFI_RTT_STATUS_NO_WIFI+1))
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_SUCCESS QSH_WIFI_RTT_STATUS_SUCCESS
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_FAILURE QSH_WIFI_RTT_STATUS_FAILURE
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_FAIL_NO_RSP QSH_WIFI_RTT_STATUS_FAIL_NO_RSP
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_FAIL_REJECTED QSH_WIFI_RTT_STATUS_FAIL_REJECTED
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_FAIL_TM_TIMEOUT QSH_WIFI_RTT_STATUS_FAIL_TM_TIMEOUT
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_FAIL_AP_ON_DIFF_CHANNEL QSH_WIFI_RTT_STATUS_FAIL_AP_ON_DIFF_CHANNEL
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_FAIL_NO_CAPABILITY QSH_WIFI_RTT_STATUS_FAIL_NO_CAPABILITY
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_FAIL_INVALID_TS QSH_WIFI_RTT_STATUS_FAIL_INVALID_TS
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_FAIL_BUSY_TRY_LATER QSH_WIFI_RTT_STATUS_FAIL_BUSY_TRY_LATER
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_INVALID_REQ QSH_WIFI_RTT_STATUS_INVALID_REQ
#define qsh_wifi_rtt_status_QSH_WIFI_RTT_STATUS_NO_WIFI QSH_WIFI_RTT_STATUS_NO_WIFI

typedef enum _qsh_wifi_lci_altitude_type {
    QSH_WIFI_LCI_ALTITUDE_TYPE_UNKNOWN = 0,
    QSH_WIFI_LCI_ALTITUDE_TYPE_METERS = 1,
    QSH_WIFI_LCI_ALTITUDE_TYPE_FLOORS = 2
} qsh_wifi_lci_altitude_type;
#define _qsh_wifi_lci_altitude_type_MIN QSH_WIFI_LCI_ALTITUDE_TYPE_UNKNOWN
#define _qsh_wifi_lci_altitude_type_MAX QSH_WIFI_LCI_ALTITUDE_TYPE_FLOORS
#define _qsh_wifi_lci_altitude_type_ARRAYSIZE ((qsh_wifi_lci_altitude_type)(QSH_WIFI_LCI_ALTITUDE_TYPE_FLOORS+1))
#define qsh_wifi_lci_altitude_type_QSH_WIFI_LCI_ALTITUDE_TYPE_UNKNOWN QSH_WIFI_LCI_ALTITUDE_TYPE_UNKNOWN
#define qsh_wifi_lci_altitude_type_QSH_WIFI_LCI_ALTITUDE_TYPE_METERS QSH_WIFI_LCI_ALTITUDE_TYPE_METERS
#define qsh_wifi_lci_altitude_type_QSH_WIFI_LCI_ALTITUDE_TYPE_FLOORS QSH_WIFI_LCI_ALTITUDE_TYPE_FLOORS

typedef enum _qsh_rtt_result_has {
    QSH_RTT_RESULT_HAS_LCI = 1
} qsh_rtt_result_has;
#define _qsh_rtt_result_has_MIN QSH_RTT_RESULT_HAS_LCI
#define _qsh_rtt_result_has_MAX QSH_RTT_RESULT_HAS_LCI
#define _qsh_rtt_result_has_ARRAYSIZE ((qsh_rtt_result_has)(QSH_RTT_RESULT_HAS_LCI+1))
#define qsh_rtt_result_has_QSH_RTT_RESULT_HAS_LCI QSH_RTT_RESULT_HAS_LCI

/* Struct definitions */
typedef struct _qsh_wifi_cmd_ranging_req {
    pb_callback_t bss_list;
/* @@protoc_insertion_point(struct:qsh_wifi_cmd_ranging_req) */
} qsh_wifi_cmd_ranging_req;

typedef PB_BYTES_ARRAY_T(6) qsh_wifi_bss_info_bssid_t;
typedef struct _qsh_wifi_bss_info {
    qsh_wifi_bss_info_bssid_t bssid;
    bool has_age_ms;
    uint32_t age_ms;
    uint32_t capability_info;
    bool has_ssid;
    char ssid[33];
    bool has_flags;
    uint32_t flags;
    int32_t rssi;
    qsh_wifi_band_mask band;
    uint32_t freq;
    uint32_t channel_width;
    bool has_cent_freq0;
    uint32_t cent_freq0;
    bool has_cent_freq1;
    uint32_t cent_freq1;
    bool has_security_mode;
    uint32_t security_mode;
    bool has_radio_chain;
    uint32_t radio_chain;
    bool has_rssi_chain0;
    int32_t rssi_chain0;
    bool has_rssi_chain1;
    int32_t rssi_chain1;
/* @@protoc_insertion_point(struct:qsh_wifi_bss_info) */
} qsh_wifi_bss_info;

typedef struct _qsh_wifi_cmd_ack {
    qsh_wifi_error status;
    qsh_wifi_msgid command;
    bool has_version;
    uint32_t version;
    bool has_capabilities;
    uint32_t capabilities;
/* @@protoc_insertion_point(struct:qsh_wifi_cmd_ack) */
} qsh_wifi_cmd_ack;

typedef struct _qsh_wifi_cmd_monitor_req {
    qsh_wifi_monitor_action action;
/* @@protoc_insertion_point(struct:qsh_wifi_cmd_monitor_req) */
} qsh_wifi_cmd_monitor_req;

typedef struct _qsh_wifi_cmd_open {
    bool get_capability;
    bool has_get_version;
    bool get_version;
/* @@protoc_insertion_point(struct:qsh_wifi_cmd_open) */
} qsh_wifi_cmd_open;

typedef struct _qsh_wifi_cmd_scan_req {
    qsh_wifi_scan_type scan_type;
    bool has_max_scan_age_ms;
    uint32_t max_scan_age_ms;
    uint32_t band;
    pb_callback_t freq_list;
    pb_callback_t ssid_list;
    bool has_radio_chain_pref;
    qsh_wifi_pref_radio_chain radio_chain_pref;
/* @@protoc_insertion_point(struct:qsh_wifi_cmd_scan_req) */
} qsh_wifi_cmd_scan_req;

typedef struct _qsh_wifi_evt_ranging_result {
    qsh_wifi_error status;
    uint32_t result_total;
    uint32_t result_count;
    bool last_in_series;
    pb_callback_t bss_list;
/* @@protoc_insertion_point(struct:qsh_wifi_evt_ranging_result) */
} qsh_wifi_evt_ranging_result;

typedef struct _qsh_wifi_evt_scan_result {
    qsh_wifi_error status;
    uint32_t result_total;
    uint32_t result_count;
    bool last_in_series;
    bool has_scan_type;
    uint32_t scan_type;
    bool has_ref_time;
    uint64_t ref_time;
    pb_callback_t scanned_freq_list;
    pb_callback_t ap_list;
    bool has_radio_chain_pref;
    qsh_wifi_pref_radio_chain radio_chain_pref;
/* @@protoc_insertion_point(struct:qsh_wifi_evt_scan_result) */
} qsh_wifi_evt_scan_result;

typedef PB_BYTES_ARRAY_T(6) qsh_wifi_ranging_req_ap_mac_addr_t;
typedef struct _qsh_wifi_ranging_req_ap {
    qsh_wifi_ranging_req_ap_mac_addr_t mac_addr;
    uint32_t freq;
    qsh_wifi_channel_width channel_width;
    bool has_cent_freq0;
    uint32_t cent_freq0;
    bool has_cent_freq1;
    uint32_t cent_freq1;
    bool request_lci;
    uint32_t num_frames_per_burst;
/* @@protoc_insertion_point(struct:qsh_wifi_ranging_req_ap) */
} qsh_wifi_ranging_req_ap;

typedef PB_BYTES_ARRAY_T(6) qsh_wifi_ranging_result_ap_mac_addr_t;
typedef struct _qsh_wifi_ranging_result_ap {
    qsh_wifi_ranging_result_ap_mac_addr_t mac_addr;
    qsh_wifi_rtt_status rtt_status;
    uint32_t result_flag;
    bool has_timestamp;
    uint64_t timestamp;
    bool has_rssi;
    int32_t rssi;
    bool has_distance;
    int32_t distance;
    bool has_distance_stddev;
    int32_t distance_stddev;
    bool has_latitude;
    int64_t latitude;
    bool has_longitude;
    int64_t longitude;
    bool has_altitude;
    int32_t altitude;
    bool has_latitude_uncertainty;
    uint32_t latitude_uncertainty;
    bool has_longitude_uncertainty;
    uint32_t longitude_uncertainty;
    bool has_altitude_type;
    qsh_wifi_lci_altitude_type altitude_type;
    bool has_altitude_uncertainty;
    uint32_t altitude_uncertainty;
/* @@protoc_insertion_point(struct:qsh_wifi_ranging_result_ap) */
} qsh_wifi_ranging_result_ap;

/* Default values for struct fields */
extern const uint32_t qsh_wifi_ranging_req_ap_num_frames_per_burst_default;

/* Initializer values for message structs */
#define qsh_wifi_cmd_open_init_default           {0, false, 0}
#define qsh_wifi_cmd_ack_init_default            {_qsh_wifi_error_MIN, _qsh_wifi_msgid_MIN, false, 0, false, 0}
#define qsh_wifi_cmd_scan_req_init_default       {_qsh_wifi_scan_type_MIN, false, 0, 0, {{NULL}, NULL}, {{NULL}, NULL}, false, _qsh_wifi_pref_radio_chain_MIN}
#define qsh_wifi_bss_info_init_default           {{0, {0}}, false, 0, 0, false, "", false, 0, 0, _qsh_wifi_band_mask_MIN, 0, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0}
#define qsh_wifi_evt_scan_result_init_default    {_qsh_wifi_error_MIN, 0, 0, 0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}, false, _qsh_wifi_pref_radio_chain_MIN}
#define qsh_wifi_cmd_monitor_req_init_default    {_qsh_wifi_monitor_action_MIN}
#define qsh_wifi_cmd_ranging_req_init_default    {{{NULL}, NULL}}
#define qsh_wifi_ranging_req_ap_init_default     {{0, {0}}, 0, _qsh_wifi_channel_width_MIN, false, 0, false, 0, 0, 8u}
#define qsh_wifi_evt_ranging_result_init_default {_qsh_wifi_error_MIN, 0, 0, 0, {{NULL}, NULL}}
#define qsh_wifi_ranging_result_ap_init_default  {{0, {0}}, _qsh_wifi_rtt_status_MIN, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, _qsh_wifi_lci_altitude_type_MIN, false, 0}
#define qsh_wifi_cmd_open_init_zero              {0, false, 0}
#define qsh_wifi_cmd_ack_init_zero               {_qsh_wifi_error_MIN, _qsh_wifi_msgid_MIN, false, 0, false, 0}
#define qsh_wifi_cmd_scan_req_init_zero          {_qsh_wifi_scan_type_MIN, false, 0, 0, {{NULL}, NULL}, {{NULL}, NULL}, false, _qsh_wifi_pref_radio_chain_MIN}
#define qsh_wifi_bss_info_init_zero              {{0, {0}}, false, 0, 0, false, "", false, 0, 0, _qsh_wifi_band_mask_MIN, 0, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0}
#define qsh_wifi_evt_scan_result_init_zero       {_qsh_wifi_error_MIN, 0, 0, 0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}, false, _qsh_wifi_pref_radio_chain_MIN}
#define qsh_wifi_cmd_monitor_req_init_zero       {_qsh_wifi_monitor_action_MIN}
#define qsh_wifi_cmd_ranging_req_init_zero       {{{NULL}, NULL}}
#define qsh_wifi_ranging_req_ap_init_zero        {{0, {0}}, 0, _qsh_wifi_channel_width_MIN, false, 0, false, 0, 0, 0}
#define qsh_wifi_evt_ranging_result_init_zero    {_qsh_wifi_error_MIN, 0, 0, 0, {{NULL}, NULL}}
#define qsh_wifi_ranging_result_ap_init_zero     {{0, {0}}, _qsh_wifi_rtt_status_MIN, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, _qsh_wifi_lci_altitude_type_MIN, false, 0}

/* Field tags (for use in manual encoding/decoding) */
#define qsh_wifi_cmd_ranging_req_bss_list_tag    1
#define qsh_wifi_bss_info_bssid_tag              1
#define qsh_wifi_bss_info_age_ms_tag             2
#define qsh_wifi_bss_info_capability_info_tag    3
#define qsh_wifi_bss_info_ssid_tag               4
#define qsh_wifi_bss_info_flags_tag              5
#define qsh_wifi_bss_info_rssi_tag               6
#define qsh_wifi_bss_info_band_tag               7
#define qsh_wifi_bss_info_freq_tag               8
#define qsh_wifi_bss_info_channel_width_tag      9
#define qsh_wifi_bss_info_cent_freq0_tag         10
#define qsh_wifi_bss_info_cent_freq1_tag         11
#define qsh_wifi_bss_info_security_mode_tag      12
#define qsh_wifi_bss_info_radio_chain_tag        13
#define qsh_wifi_bss_info_rssi_chain0_tag        14
#define qsh_wifi_bss_info_rssi_chain1_tag        15
#define qsh_wifi_cmd_ack_status_tag              1
#define qsh_wifi_cmd_ack_command_tag             2
#define qsh_wifi_cmd_ack_version_tag             3
#define qsh_wifi_cmd_ack_capabilities_tag        4
#define qsh_wifi_cmd_monitor_req_action_tag      1
#define qsh_wifi_cmd_open_get_capability_tag     1
#define qsh_wifi_cmd_open_get_version_tag        2
#define qsh_wifi_cmd_scan_req_scan_type_tag      1
#define qsh_wifi_cmd_scan_req_max_scan_age_ms_tag 2
#define qsh_wifi_cmd_scan_req_band_tag           3
#define qsh_wifi_cmd_scan_req_freq_list_tag      4
#define qsh_wifi_cmd_scan_req_ssid_list_tag      5
#define qsh_wifi_cmd_scan_req_radio_chain_pref_tag 6
#define qsh_wifi_evt_ranging_result_status_tag   1
#define qsh_wifi_evt_ranging_result_result_total_tag 2
#define qsh_wifi_evt_ranging_result_result_count_tag 3
#define qsh_wifi_evt_ranging_result_last_in_series_tag 4
#define qsh_wifi_evt_ranging_result_bss_list_tag 5
#define qsh_wifi_evt_scan_result_status_tag      1
#define qsh_wifi_evt_scan_result_result_total_tag 2
#define qsh_wifi_evt_scan_result_result_count_tag 3
#define qsh_wifi_evt_scan_result_last_in_series_tag 4
#define qsh_wifi_evt_scan_result_scan_type_tag   5
#define qsh_wifi_evt_scan_result_ref_time_tag    6
#define qsh_wifi_evt_scan_result_scanned_freq_list_tag 7
#define qsh_wifi_evt_scan_result_ap_list_tag     8
#define qsh_wifi_evt_scan_result_radio_chain_pref_tag 10
#define qsh_wifi_ranging_req_ap_mac_addr_tag     1
#define qsh_wifi_ranging_req_ap_freq_tag         2
#define qsh_wifi_ranging_req_ap_channel_width_tag 3
#define qsh_wifi_ranging_req_ap_cent_freq0_tag   4
#define qsh_wifi_ranging_req_ap_cent_freq1_tag   5
#define qsh_wifi_ranging_req_ap_request_lci_tag  6
#define qsh_wifi_ranging_req_ap_num_frames_per_burst_tag 7
#define qsh_wifi_ranging_result_ap_mac_addr_tag  1
#define qsh_wifi_ranging_result_ap_rtt_status_tag 2
#define qsh_wifi_ranging_result_ap_result_flag_tag 3
#define qsh_wifi_ranging_result_ap_timestamp_tag 4
#define qsh_wifi_ranging_result_ap_rssi_tag      5
#define qsh_wifi_ranging_result_ap_distance_tag  6
#define qsh_wifi_ranging_result_ap_distance_stddev_tag 7
#define qsh_wifi_ranging_result_ap_latitude_tag  8
#define qsh_wifi_ranging_result_ap_longitude_tag 9
#define qsh_wifi_ranging_result_ap_altitude_tag  10
#define qsh_wifi_ranging_result_ap_latitude_uncertainty_tag 11
#define qsh_wifi_ranging_result_ap_longitude_uncertainty_tag 12
#define qsh_wifi_ranging_result_ap_altitude_type_tag 13
#define qsh_wifi_ranging_result_ap_altitude_uncertainty_tag 14

/* Struct field encoding specification for nanopb */
extern const pb_field_t qsh_wifi_cmd_open_fields[3];
extern const pb_field_t qsh_wifi_cmd_ack_fields[5];
extern const pb_field_t qsh_wifi_cmd_scan_req_fields[7];
extern const pb_field_t qsh_wifi_bss_info_fields[16];
extern const pb_field_t qsh_wifi_evt_scan_result_fields[10];
extern const pb_field_t qsh_wifi_cmd_monitor_req_fields[2];
extern const pb_field_t qsh_wifi_cmd_ranging_req_fields[2];
extern const pb_field_t qsh_wifi_ranging_req_ap_fields[8];
extern const pb_field_t qsh_wifi_evt_ranging_result_fields[6];
extern const pb_field_t qsh_wifi_ranging_result_ap_fields[15];

/* Maximum encoded size of messages (where known) */
#define qsh_wifi_cmd_open_size                   4
#define qsh_wifi_cmd_ack_size                    17
/* qsh_wifi_cmd_scan_req_size depends on runtime parameters */
#define qsh_wifi_bss_info_size                   132
/* qsh_wifi_evt_scan_result_size depends on runtime parameters */
#define qsh_wifi_cmd_monitor_req_size            2
/* qsh_wifi_cmd_ranging_req_size depends on runtime parameters */
#define qsh_wifi_ranging_req_ap_size             36
/* qsh_wifi_evt_ranging_result_size depends on runtime parameters */
#define qsh_wifi_ranging_result_ap_size          113

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define QSH_WIFI_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif