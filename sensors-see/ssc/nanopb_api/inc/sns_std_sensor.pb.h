/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.6 at Tue Apr 14 12:31:31 2020. */

#ifndef PB_SNS_STD_SENSOR_PB_H_INCLUDED
#define PB_SNS_STD_SENSOR_PB_H_INCLUDED
#include <pb.h>

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _sns_std_sensor_msgid {
    SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG = 513,
    SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG = 514,
    SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT = 768,
    SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT = 1025
} sns_std_sensor_msgid;
#define _sns_std_sensor_msgid_MIN SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
#define _sns_std_sensor_msgid_MAX SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT
#define _sns_std_sensor_msgid_ARRAYSIZE ((sns_std_sensor_msgid)(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT+1))
#define sns_std_sensor_msgid_SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
#define sns_std_sensor_msgid_SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG
#define sns_std_sensor_msgid_SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT
#define sns_std_sensor_msgid_SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT

typedef enum _sns_std_sensor_sample_status {
    SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE = 0,
    SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW = 1,
    SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_MEDIUM = 2,
    SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH = 3
} sns_std_sensor_sample_status;
#define _sns_std_sensor_sample_status_MIN SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE
#define _sns_std_sensor_sample_status_MAX SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH
#define _sns_std_sensor_sample_status_ARRAYSIZE ((sns_std_sensor_sample_status)(SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH+1))
#define sns_std_sensor_sample_status_SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE
#define sns_std_sensor_sample_status_SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW
#define sns_std_sensor_sample_status_SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_MEDIUM SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_MEDIUM
#define sns_std_sensor_sample_status_SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH

typedef enum _sns_std_sensor_attr_id {
    SNS_STD_SENSOR_ATTRID_NAME = 0,
    SNS_STD_SENSOR_ATTRID_VENDOR = 1,
    SNS_STD_SENSOR_ATTRID_TYPE = 2,
    SNS_STD_SENSOR_ATTRID_AVAILABLE = 3,
    SNS_STD_SENSOR_ATTRID_VERSION = 4,
    SNS_STD_SENSOR_ATTRID_API = 5,
    SNS_STD_SENSOR_ATTRID_RATES = 6,
    SNS_STD_SENSOR_ATTRID_RESOLUTIONS = 7,
    SNS_STD_SENSOR_ATTRID_FIFO_SIZE = 8,
    SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT = 9,
    SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT = 10,
    SNS_STD_SENSOR_ATTRID_RANGES = 11,
    SNS_STD_SENSOR_ATTRID_OP_MODES = 12,
    SNS_STD_SENSOR_ATTRID_DRI = 13,
    SNS_STD_SENSOR_ATTRID_STREAM_SYNC = 14,
    SNS_STD_SENSOR_ATTRID_EVENT_SIZE = 15,
    SNS_STD_SENSOR_ATTRID_STREAM_TYPE = 16,
    SNS_STD_SENSOR_ATTRID_DYNAMIC = 17,
    SNS_STD_SENSOR_ATTRID_HW_ID = 18,
    SNS_STD_SENSOR_ATTRID_RIGID_BODY = 19,
    SNS_STD_SENSOR_ATTRID_PLACEMENT = 20,
    SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR = 21,
    SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS = 22,
    SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION = 23,
    SNS_STD_SENSOR_ATTRID_SELECTED_RANGE = 24,
    SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES = 25,
    SNS_STD_SENSOR_ATTRID_PASSIVE_REQUEST = 26
} sns_std_sensor_attr_id;
#define _sns_std_sensor_attr_id_MIN SNS_STD_SENSOR_ATTRID_NAME
#define _sns_std_sensor_attr_id_MAX SNS_STD_SENSOR_ATTRID_PASSIVE_REQUEST
#define _sns_std_sensor_attr_id_ARRAYSIZE ((sns_std_sensor_attr_id)(SNS_STD_SENSOR_ATTRID_PASSIVE_REQUEST+1))
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_NAME SNS_STD_SENSOR_ATTRID_NAME
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_VENDOR SNS_STD_SENSOR_ATTRID_VENDOR
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_TYPE SNS_STD_SENSOR_ATTRID_TYPE
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_AVAILABLE SNS_STD_SENSOR_ATTRID_AVAILABLE
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_VERSION SNS_STD_SENSOR_ATTRID_VERSION
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_API SNS_STD_SENSOR_ATTRID_API
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_RATES SNS_STD_SENSOR_ATTRID_RATES
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_RESOLUTIONS SNS_STD_SENSOR_ATTRID_RESOLUTIONS
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_FIFO_SIZE SNS_STD_SENSOR_ATTRID_FIFO_SIZE
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_RANGES SNS_STD_SENSOR_ATTRID_RANGES
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_OP_MODES SNS_STD_SENSOR_ATTRID_OP_MODES
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_DRI SNS_STD_SENSOR_ATTRID_DRI
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_STREAM_SYNC SNS_STD_SENSOR_ATTRID_STREAM_SYNC
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_EVENT_SIZE SNS_STD_SENSOR_ATTRID_EVENT_SIZE
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_STREAM_TYPE SNS_STD_SENSOR_ATTRID_STREAM_TYPE
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_DYNAMIC SNS_STD_SENSOR_ATTRID_DYNAMIC
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_HW_ID SNS_STD_SENSOR_ATTRID_HW_ID
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_RIGID_BODY SNS_STD_SENSOR_ATTRID_RIGID_BODY
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_PLACEMENT SNS_STD_SENSOR_ATTRID_PLACEMENT
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_SELECTED_RANGE SNS_STD_SENSOR_ATTRID_SELECTED_RANGE
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES
#define sns_std_sensor_attr_id_SNS_STD_SENSOR_ATTRID_PASSIVE_REQUEST SNS_STD_SENSOR_ATTRID_PASSIVE_REQUEST

typedef enum _sns_std_sensor_stream_type {
    SNS_STD_SENSOR_STREAM_TYPE_STREAMING = 0,
    SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE = 1,
    SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT = 2
} sns_std_sensor_stream_type;
#define _sns_std_sensor_stream_type_MIN SNS_STD_SENSOR_STREAM_TYPE_STREAMING
#define _sns_std_sensor_stream_type_MAX SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT
#define _sns_std_sensor_stream_type_ARRAYSIZE ((sns_std_sensor_stream_type)(SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT+1))
#define sns_std_sensor_stream_type_SNS_STD_SENSOR_STREAM_TYPE_STREAMING SNS_STD_SENSOR_STREAM_TYPE_STREAMING
#define sns_std_sensor_stream_type_SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE
#define sns_std_sensor_stream_type_SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT

typedef enum _sns_std_sensor_rigid_body_type {
    SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY = 0,
    SNS_STD_SENSOR_RIGID_BODY_TYPE_KEYBOARD = 1,
    SNS_STD_SENSOR_RIGID_BODY_TYPE_EXTERNAL = 2
} sns_std_sensor_rigid_body_type;
#define _sns_std_sensor_rigid_body_type_MIN SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY
#define _sns_std_sensor_rigid_body_type_MAX SNS_STD_SENSOR_RIGID_BODY_TYPE_EXTERNAL
#define _sns_std_sensor_rigid_body_type_ARRAYSIZE ((sns_std_sensor_rigid_body_type)(SNS_STD_SENSOR_RIGID_BODY_TYPE_EXTERNAL+1))
#define sns_std_sensor_rigid_body_type_SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY
#define sns_std_sensor_rigid_body_type_SNS_STD_SENSOR_RIGID_BODY_TYPE_KEYBOARD SNS_STD_SENSOR_RIGID_BODY_TYPE_KEYBOARD
#define sns_std_sensor_rigid_body_type_SNS_STD_SENSOR_RIGID_BODY_TYPE_EXTERNAL SNS_STD_SENSOR_RIGID_BODY_TYPE_EXTERNAL

/* Struct definitions */
typedef struct _sns_std_sensor_config {
    float sample_rate;
/* @@protoc_insertion_point(struct:sns_std_sensor_config) */
} sns_std_sensor_config;

typedef struct _sns_std_sensor_config_event {
    float sample_rate;
/* @@protoc_insertion_point(struct:sns_std_sensor_config_event) */
} sns_std_sensor_config_event;

typedef struct _sns_std_sensor_event {
    pb_callback_t data;
    sns_std_sensor_sample_status status;
/* @@protoc_insertion_point(struct:sns_std_sensor_event) */
} sns_std_sensor_event;

typedef struct _sns_std_sensor_physical_config_event {
    bool has_sample_rate;
    float sample_rate;
    bool has_water_mark;
    uint32_t water_mark;
    pb_size_t range_count;
    float range[2];
    bool has_resolution;
    float resolution;
    pb_callback_t operation_mode;
    bool has_active_current;
    uint32_t active_current;
    bool has_stream_is_synchronous;
    bool stream_is_synchronous;
    bool has_dri_enabled;
    bool dri_enabled;
    bool has_DAE_watermark;
    uint32_t DAE_watermark;
    bool has_sync_ts_anchor;
    uint64_t sync_ts_anchor;
/* @@protoc_insertion_point(struct:sns_std_sensor_physical_config_event) */
} sns_std_sensor_physical_config_event;

/* Default values for struct fields */
extern const sns_std_sensor_sample_status sns_std_sensor_event_status_default;

/* Initializer values for message structs */
#define sns_std_sensor_config_init_default       {0}
#define sns_std_sensor_event_init_default        {{{NULL}, NULL}, SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE}
#define sns_std_sensor_config_event_init_default {0}
#define sns_std_sensor_physical_config_event_init_default {false, 0, false, 0, 0, {0, 0}, false, 0, {{NULL}, NULL}, false, 0, false, 0, false, 0, false, 0, false, 0}
#define sns_std_sensor_config_init_zero          {0}
#define sns_std_sensor_event_init_zero           {{{NULL}, NULL}, (sns_std_sensor_sample_status)0}
#define sns_std_sensor_config_event_init_zero    {0}
#define sns_std_sensor_physical_config_event_init_zero {false, 0, false, 0, 0, {0, 0}, false, 0, {{NULL}, NULL}, false, 0, false, 0, false, 0, false, 0, false, 0}

/* Field tags (for use in manual encoding/decoding) */
#define sns_std_sensor_config_sample_rate_tag    1
#define sns_std_sensor_config_event_sample_rate_tag 1
#define sns_std_sensor_event_data_tag            1
#define sns_std_sensor_event_status_tag          2
#define sns_std_sensor_physical_config_event_sample_rate_tag 1
#define sns_std_sensor_physical_config_event_water_mark_tag 2
#define sns_std_sensor_physical_config_event_range_tag 3
#define sns_std_sensor_physical_config_event_resolution_tag 4
#define sns_std_sensor_physical_config_event_operation_mode_tag 5
#define sns_std_sensor_physical_config_event_active_current_tag 6
#define sns_std_sensor_physical_config_event_stream_is_synchronous_tag 7
#define sns_std_sensor_physical_config_event_dri_enabled_tag 8
#define sns_std_sensor_physical_config_event_DAE_watermark_tag 9
#define sns_std_sensor_physical_config_event_sync_ts_anchor_tag 10

/* Struct field encoding specification for nanopb */
extern const pb_field_t sns_std_sensor_config_fields[2];
extern const pb_field_t sns_std_sensor_event_fields[3];
extern const pb_field_t sns_std_sensor_config_event_fields[2];
extern const pb_field_t sns_std_sensor_physical_config_event_fields[11];

/* Maximum encoded size of messages (where known) */
#define sns_std_sensor_config_size               5
/* sns_std_sensor_event_size depends on runtime parameters */
#define sns_std_sensor_config_event_size         5
/* sns_std_sensor_physical_config_event_size depends on runtime parameters */

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define SNS_STD_SENSOR_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
