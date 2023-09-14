/*=============================================================================
  @file qsh_wifi_test.c

  QSH WIFI TEST application.  This application will open a QMI connection to
  the Sensors Service (which resides on the SSC).  It will send a SUID Lookup
  Request for the data type "wifi_svc". For each found SUID, this application
  will send an Attributes Request, and subsequently an sends request with
  to Sensor.

  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <inttypes.h>
#include <pthread.h>
#include <qsh_wifi_test.h>

// Scan Results place holder for Request Based Scan
qsh_wifi_scan_evt_db g_scan_evt_list [MAX_SCAN_EVENTS];
uint32_t g_scan_evt_list_count = 0;
bool is_scan_requested = 0;
pthread_mutex_t scan_mutex;

// Scan Results place holder for Monitor Scan
qsh_wifi_scan_evt_db g_mon_evt_list [MAX_SCAN_EVENTS];
uint32_t g_mon_evt_list_count = 0;
bool is_monitor_on = false;

// Results place holder for Ranging Request
qsh_wifi_ranging_evt_db g_rtt_evt_list [MAX_RTT_EVENTS];
uint32_t g_rtt_evt_list_count = 0;
bool is_rtt_requested = false;
pthread_mutex_t rtt_mutex;

enum init_state state = DEINIT;
pthread_mutex_t mutex;
pthread_cond_t  cond;
pid_t pid;

/* KPI DUMP MASK on console -kdm */
#define DUMP_KPI_EXT  0x0001 // Mask for VENDORS ( default value)
#define DUMP_KPI_INT  0x0010 // Mask for dumping complete kpi

#define CAN_DUMP_INT_KPI_STATS(mask) (mask & DUMP_KPI_INT)
#define CAN_DUMP_EXT_KPI_STATS(mask) (mask & DUMP_KPI_EXT)

/* KPI FS Log MASK -kfm */
enum qsh_wifi_log_mask
{
  M_HIGH = 0x0001,
  M_MED  = 0x0002,
  M_LOW  = 0x0004
};

static inline void qsh_wifi_unblock_th()
{
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
}

/*=============================================================================
 * qsh_wifi_wait_for_signal_or_timeout
 *
 * Description:
 *   Wait for a signal or timeout
 *
 * Return value:
 *   1 if signaled
 *   0 if timed out
 ============================================================================*/
static int qsh_wifi_timeout_block(
              uint32_t timeout, pthread_cond_t* cmd_cond)
{
  struct timespec     present_time;
  int ret_val = -1;

  if (0 == clock_gettime(CLOCK_REALTIME, &present_time)) {
    present_time.tv_sec += timeout / MS_PER_SEC;
    present_time.tv_nsec += ((timeout % MS_PER_SEC) * NS_PER_MS);
    if (present_time.tv_nsec > NS_PER_SEC) {
      present_time.tv_sec++;
      present_time.tv_nsec -= (NS_PER_SEC);
    }

    pthread_mutex_lock(&mutex);
    ret_val = pthread_cond_timedwait(cmd_cond,
                                     &mutex,
                                     &present_time);
    pthread_mutex_unlock(&mutex);
  }
  return ret_val;
}

uint32_t g_log_mask = 7;

static void
qsh_wifi_printk(enum qsh_wifi_log_mask mask, const char *fmt, ...)
{
  if (g_log_mask & mask) {
    va_list args;
    va_start (args, fmt);
    __android_log_print(ANDROID_LOG_WARN, TAG, fmt, args);
    va_end (args);
  }
}

#define __qsh_wifi_pr qsh_wifi_printk
#define qsh_wifi_pr(log_level, format, args...) \
        __qsh_wifi_pr(log_level, format, ## args)

#define qsh_wifi_pr_high(params...) qsh_wifi_pr(M_HIGH, ## params)
#define qsh_wifi_pr_med(params...)  qsh_wifi_pr(M_MED, ## params)
#define qsh_wifi_pr_low(params...)  qsh_wifi_pr(M_LOW, ## params)

//fwd declarations
int qsh_wifi_init ();
/*=============================================================================
  Static Variable Definitions
  ===========================================================================*/

/* QMI Client handle created within create_client_conn(), and used to send
 * a request to the Sensor service */
static qmi_client_type qmi_sensors_handle = NULL;

/* The SUID of the SUID Lookup Sensor is a constant value, and published within
 * sns_suid.proto. */
static sns_std_suid suid_lookup = sns_suid_sensor_init_default;

/* SUID for the wifi Sensor, as received from the SUID Lookup Sensor */
static sns_std_suid wifi_suid = (sns_std_suid){ .suid_low = 0, .suid_high = 0 };

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

char const* get_cmd_name( qsh_wifi_msgid a)
{
  switch ( a )
  {
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_OPEN:
      return "CMD_OPEN";
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_SCAN_REQ:
      return "CMD_SCAN_REQ";
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_MONITOR_REQ:
      return "CMD_MONITOR_REQ";
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_RANGING_REQ:
      return "CMD_RANGING_REQ";
    default:
      return "INVALID_WIFI_CMD";
  }
}

char const* get_event_name( uint32_t a)
{
  switch ( a )
  {
    case SNS_STD_MSGID_SNS_STD_ATTR_EVENT:
      return "SNS_STD_ATTR_EVENT";
    case SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT:
      return "SNS_STD_SENSOR_EVENT";
    case SNS_STD_MSGID_SNS_STD_ERROR_EVENT:
      return "SNS_STD_ERROR_EVENT";
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_ACK:
      return "QSH_WIFI_CMD_ACK";
    case QSH_WIFI_MSGID_QSH_WIFI_EVT_SCAN_RESULTS:
      return "QSH_WIFI_EVT_SCAN_RESULTS";
    case QSH_WIFI_MSGID_QSH_WIFI_EVT_RANGING_RESULTS:
      return "QSH_WIFI_EVT_RANGING_RESULTS";
    default:
      return "INVALID_EVENT";
  }
}

/**
 * Copy an already encoded payload into a message field.  For the purpose of
 * this function, an encoded submessage is identical to a character string.
 */
static bool encode_payload(pb_ostream_t *stream,
    const pb_field_t *field, void *const *arg)
{
  sns_buffer_arg *info = (sns_buffer_arg*)*arg;

  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_string(stream, info->buf, info->buf_len))
    return false;

  return true;
}

/**
 * Decode a string of bytes from a field. This function is intended to be
 * used as a callback function during the decode process.
 *
 * @see pb_callback_s::decode
 */
static bool decode_payload(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_buffer_arg *data = (sns_buffer_arg*)*arg;

  data->buf_len = stream->bytes_left;
  data->buf = stream->state;
  return pb_read(stream, (pb_byte_t*)NULL, stream->bytes_left);
}

/**
 * Decode an array of float values, such as the array within the sensor
 * sample data.
 */
bool
decode_float_arr_cb(pb_istream_t *stream, const pb_field_t *field,
    void **arg)
{
  UNUSED_VAR(field);
  pb_float_arr_arg const *float_arr = (pb_float_arr_arg const*)*arg;

  if(!pb_decode_fixed32(stream, &(float_arr->arr[*float_arr->arr_index])))
      return false;

  (*float_arr->arr_index)++;

  return true;
}

/**
 * Handle an incoming QMI response message from the Sensors Service.
 */
static void qsh_wifi_qmi_immediate_resp_cb(
    qmi_client_type user_handle, unsigned int msg_id,
    void *resp_c_struct, unsigned int resp_c_struct_len,
    void *resp_cb_data, qmi_client_error_type transp_err)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(resp_cb_data);

  __android_log_print(ANDROID_LOG_WARN, TAG,
      "Received immediate response; error %i, msg_id %i, size %i",
      transp_err, msg_id, resp_c_struct_len);

  if(QMI_NO_ERR == transp_err) {
    sns_client_resp_msg_v01 *resp = (sns_client_resp_msg_v01*)resp_c_struct;

   __android_log_print(ANDROID_LOG_WARN, TAG,
      "Received client ID %" PRId64 " result %i",
      resp->client_id, resp->result);
  }

  free(resp_c_struct);
}

/**
 * Send a QMI request message.
 *
 * @param[i] qmi_handle QMI connecton created in create_client_conn
 * @param[i] req_msg Completed request message to be sent
 *
 * @return true on success, false otherwise
 */
static bool send_qmi_req(qmi_client_type qmi_handle,
    sns_client_req_msg_v01 const *req_msg)
{
  bool rv = false;
  size_t resp_len = sizeof(sns_client_resp_msg_v01);
  void *resp = malloc(resp_len);

  __android_log_print(ANDROID_LOG_VERBOSE, TAG,
  "Sending request payload len %i", req_msg->payload_len);

  if(NULL == resp) {
     qsh_wifi_pr_high("Unable to allocate sns_client_resp_msg_v01");
  } else {
    qmi_txn_handle txn_handle;

    qmi_client_error_type qmi_err = qmi_client_send_msg_async(
        qmi_handle, SNS_CLIENT_REQ_V01, (void*)req_msg, sizeof(*req_msg),
        resp, resp_len, qsh_wifi_qmi_immediate_resp_cb, resp, &txn_handle);

    if(QMI_NO_ERR != qmi_err) {
      qsh_wifi_pr_high("qmi_client_send_msg_async error %u", qmi_err);
      free(resp);
    } else {
      rv = true;
    }
  }

  return rv;
}

void hex_dump(char *msg, uint32_t len)
{
  if (NULL != msg)
  {
    uint32_t const MAX_ROW_LEN = 8;
    char buff[MAX_ROW_LEN];
    char *tmp = msg;
    uint32_t ii = 0;
    uint32_t jj = 0;
    uint32_t row_max = (len > MAX_ROW_LEN) ? len : MAX_ROW_LEN;
    if (row_max < MAX_ROW_LEN)
    {
      jj = row_max;
      goto out;
    }
    memset(buff, 0, MAX_ROW_LEN);
    __android_log_print(ANDROID_LOG_ERROR, TAG,
       "Dumping the hex bytes for on demand req %u", len);
    for (ii = 0; ii < len;)
    {
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "msg %02x %02x %02x %02x %02x %02x %02x %02x",
          tmp[0],  tmp[1],  tmp[2],  tmp[3],
          tmp[4],  tmp[5],  tmp[6],  tmp[7]);
      ii = ii+MAX_ROW_LEN;
      tmp = tmp + MAX_ROW_LEN;
      jj = len - ii;
      if ( jj < MAX_ROW_LEN)
      {
        break;
      }
    }
out:
    if (jj)
    {
      uint32_t written = 0;
      int retVal = -1;
      for (ii = 0; ii < jj && written < MAX_ROW_LEN; ii++)
      {
        snprintf(buff + written, jj - written, "%02x", tmp[ii]);
        written += retVal;
      }
      __android_log_print(ANDROID_LOG_ERROR, TAG, "msg %s", buff);
    }
  }
} // hexDump

/**
 * Send a request to the specified service.  Do not specify any batching
 * options.
 *
 * @param[i] payload Encoded Sensor-specific request message
 * @param[i] suid Destination SUID
 * @param[i] msg_id Sensor-specific message ID
 */
static bool send_basic_req(qmi_client_type qmi_handle,
    sns_buffer_arg const *payload, sns_std_suid suid, uint32_t msg_id)
{
  sns_client_request_msg pb_req_msg = sns_client_request_msg_init_default;
  sns_client_req_msg_v01 req_msg =
      {.payload_len = 0, .use_jumbo_report_valid = 0, .use_jumbo_report = 0};
  pb_ostream_t stream = pb_ostream_from_buffer(req_msg.payload,
                            SNS_CLIENT_REQ_LEN_MAX_V01);

  pb_req_msg.suid = suid;
  pb_req_msg.msg_id = msg_id;
  pb_req_msg.request.has_batching = false;

  if (NULL != payload) {
    pb_req_msg.request.payload.funcs.encode = &encode_payload;
    pb_req_msg.request.payload.arg = (void*)payload;
  }

  if(!pb_encode(&stream, sns_client_request_msg_fields, &pb_req_msg)) {
    qsh_wifi_pr_high("Error Encoding request: %s", PB_GET_ERROR(&stream));
    return false;
  } else {
    req_msg.payload_len = stream.bytes_written;
    send_qmi_req(qmi_handle, &req_msg);
    return true;
  }
}

/**
 * Create an encoded Attribute request message.
 *
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t
get_encoded_attr_req(void **encoded_req)
{
  size_t encoded_req_size;
  sns_std_attr_req attr_req = sns_std_attr_req_init_default;

  *encoded_req = NULL;

  if (!pb_get_encoded_size(&encoded_req_size,
    sns_std_attr_req_fields, &attr_req)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer,
                             encoded_req_size);

    if(!pb_encode(&stream, sns_std_attr_req_fields, &attr_req)) {
       __android_log_print(ANDROID_LOG_ERROR, TAG,
        "Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Send an attribute request for the specified SUID.
 */
static bool
send_attr_req(qmi_client_type qmi_handle, sns_std_suid *suid)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  encoded_req_len = get_encoded_attr_req(&encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid,
            SNS_STD_MSGID_SNS_STD_ATTR_REQ);
    free(encoded_req);
  }

  return rv;
}

/**
 * Create a SUID lookup request for the specified data type.
 *
 * @param[i] data_type Sensor type to search for
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t
get_encoded_suid_req(char const *data_type, void **encoded_req)
{
  size_t encoded_req_size;
  sns_suid_req suid_req = sns_suid_req_init_default;

  *encoded_req = NULL;

  suid_req.data_type.funcs.encode = &encode_payload;
  suid_req.data_type.arg = &((sns_buffer_arg)
    { .buf = data_type, .buf_len = strlen(data_type) });
  suid_req.has_register_updates = true;
  suid_req.register_updates = true;

  if(!pb_get_encoded_size(&encoded_req_size, sns_suid_req_fields, &suid_req)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer,
                              encoded_req_size);

    if(!pb_encode(&stream, sns_suid_req_fields, &suid_req)) {
      __android_log_print(ANDROID_LOG_ERROR, TAG,
        "Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Send a request to the SUID Lookup Sensor for the complete list of SUIDs
 *
 * @param[i] qmi_handle QMI connecton created in create_client_conn
 * @param[i] data_type Sensor type to search for
 *
 * @return true if request was sent successfully, false otherwise
 */
static bool qsh_wifi_send_suid_req(qmi_client_type qmi_handle,
    char const *data_type)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  encoded_req_len = get_encoded_suid_req(data_type, &encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, suid_lookup,
             SNS_SUID_MSGID_SNS_SUID_REQ);
    free(encoded_req);
  }

  return rv;
}


uint32_t freq_array[20] = {2412, 2417, 2422, 2427, 2432, 2437, 2442, 2447,
    2452, 2457, 2462, 2467, 2472, 2484};

static bool
qsh_wifi_encode_freq_list(pb_ostream_t *stream,
   const pb_field_t *field, void * const *arg)
{
  uint32_t cnt = 0;
  uint64_t value;
  qsh_wifi_scan_db *scan_db = (qsh_wifi_scan_db *)*arg;
  for (cnt = 0; cnt < scan_db->freq_count && cnt < MAX_FREQ_IN_SCAN_EVT; cnt++)
  {
    if (!pb_encode_tag_for_field(stream, field))
    {
      qsh_wifi_pr_high("error happen while filling the freq");
      return false;
    }
    value = scan_db->freq_list[cnt];
    __android_log_print(ANDROID_LOG_ERROR, TAG,
         "\n push freq  %u val %u", (uint32_t)value, scan_db->freq_list[cnt]);
    pb_encode_varint(stream, value);
  }
  return true;
}

static bool
qsh_wifi_encode_ssid_list(pb_ostream_t *stream,
   const pb_field_t *field, void * const *arg)
{
  uint32_t cnt = 0;
  qsh_wifi_scan_db *scan_db = (qsh_wifi_scan_db *)*arg;
  for (cnt = 0; cnt  < scan_db->ssid_count; cnt++)
  {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "ssid len %zu %-32s", strlen((char *)&scan_db->ssid_list[cnt][0]),
      (char *)&scan_db->ssid_list[cnt][0]);
    if (!pb_encode_tag_for_field(stream, field))
    {
      printf("\n error happen while tag ssid");
      return false;
    }
    /* This encodes the data for the field, based on our FileInfo structure. */
    if (!pb_encode_string(stream, (pb_byte_t *)&scan_db->ssid_list[cnt][0],
         strlen((char *)&scan_db->ssid_list[cnt][0])))
    {
      printf("\n error happen while encoding submessage");
      return false;
    }
  }
  return true;
}

/**
 * Create an encoded on demand scan request.
 *
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t qsh_wifi_encode_on_demand_scan(void **encoded_req,
    qsh_wifi_scan_db *scan_db)
{
  size_t encoded_req_size;
  qsh_wifi_cmd_scan_req scan_req = qsh_wifi_cmd_scan_req_init_default;

  scan_req.scan_type = scan_db->scan_type;
  scan_req.band = scan_db->band;
  if (scan_db->has_max_scan_age_ms)
  {
    scan_req.has_max_scan_age_ms = true;
    scan_req.max_scan_age_ms = scan_db->max_scan_age_ms;
  }
  if (scan_db->has_radio_chain_pref)
  {
    scan_req.has_radio_chain_pref = true;
    scan_req.radio_chain_pref = scan_db->radio_chain_pref;
  }
  scan_req.freq_list.funcs.encode = &qsh_wifi_encode_freq_list;
  scan_req.freq_list.arg = (void *)scan_db;

  scan_req.ssid_list.funcs.encode = &qsh_wifi_encode_ssid_list;
  scan_req.ssid_list.arg = (void *)scan_db;

  *encoded_req = NULL;
  // Roughly allocate 800 bytes
  encoded_req_size = 800;
  void *encoded_buffer = malloc(encoded_req_size);
  pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer,
                            encoded_req_size);

  if(!pb_encode(&stream, qsh_wifi_cmd_scan_req_fields, (void *)&scan_req)) {
    qsh_wifi_pr_high("Error Encoding on demand scan request: %s",
        PB_GET_ERROR(&stream));
     __android_log_print(ANDROID_LOG_ERROR, TAG,
        "on demand scanning got failed");
    return 0;
  } else {
    *encoded_req = encoded_buffer;
  }
  encoded_req_size = stream.bytes_written;
  __android_log_print(ANDROID_LOG_ERROR, TAG,
      "on demand encoded size %zu", encoded_req_size);
  //__android_log_print(ANDROID_LOG_ERROR, TAG,
  //    "*** dumping on demand req***");
  //hex_dump((char *)*encoded_req, encoded_req_size);
  return NULL == *encoded_req ? 0 : encoded_req_size;
}

static bool
qsh_wifi_encode_rtt_ap_list(pb_ostream_t *stream,
   const pb_field_t *field, void * const *arg)
{
  uint32_t cnt = 0;
  qsh_wifi_ranging_req_db *rtt_db = (qsh_wifi_ranging_req_db *)*arg;
  __android_log_print(ANDROID_LOG_ERROR, TAG,
      "%s total RTT ap count=%u", __func__, rtt_db->bss_count);

  for (cnt = 0; cnt < rtt_db->bss_count && cnt < MAX_REQ_RANGING_BSS; cnt++)
  {
    qsh_wifi_ranging_req_ap meas;
    memcpy(&meas, &rtt_db->bss_list[cnt], sizeof(meas));
    if (meas.num_frames_per_burst == 0)
      meas.num_frames_per_burst =
        qsh_wifi_ranging_req_ap_num_frames_per_burst_default;

    if(!pb_encode_tag_for_field(stream, field))
    {
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Error while pb_encode_tag of rtt ap info");
      return false;
    }

    if(!pb_encode_submessage(stream, qsh_wifi_ranging_req_ap_fields, &meas))
    {
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Error occur while encoding RTT ap info submsg");
      return false;
    }
  }
  return true;
}

/**
 * Create an encoded on rtt request.
 *
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t qsh_wifi_encode_rtt_request(void **encoded_req,
    qsh_wifi_ranging_req_db *rtt_db)
{
  size_t encoded_req_size;
  qsh_wifi_cmd_ranging_req rtt_req = qsh_wifi_cmd_ranging_req_init_default;

  rtt_req.bss_list.funcs.encode = &qsh_wifi_encode_rtt_ap_list;
  rtt_req.bss_list.arg = (void *)rtt_db;

  *encoded_req = NULL;
  // Roughly allocate 800 bytes
  encoded_req_size = 800;
  void *encoded_buffer = malloc(encoded_req_size);
  pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer,
                            encoded_req_size);

  if(!pb_encode(&stream, qsh_wifi_cmd_ranging_req_fields, (void *)&rtt_req)) {
    qsh_wifi_pr_high("Error Encoding on ranging request: %s",
        PB_GET_ERROR(&stream));
     __android_log_print(ANDROID_LOG_ERROR, TAG,
        "Ranging got failed");
    return 0;
  } else {
    *encoded_req = encoded_buffer;
  }
  encoded_req_size = stream.bytes_written;
  __android_log_print(ANDROID_LOG_ERROR, TAG,
      "ranging encoded size %zu", encoded_req_size);
  return NULL == *encoded_req ? 0 : encoded_req_size;
}

static int qsh_wifi_get_monitor_mode_req(void **encoded_req,
    bool action)
{
  size_t encoded_req_size;
  qsh_wifi_cmd_monitor_req mon_req = qsh_wifi_cmd_monitor_req_init_default;
  mon_req.action = (uint32_t)action;

  *encoded_req = NULL;

   __android_log_print(ANDROID_LOG_VERBOSE, TAG,
    "Get encoded Monitor mode request action=%s", action ? "ON" : "OFF");

  if(!pb_get_encoded_size(&encoded_req_size, qsh_wifi_cmd_monitor_req_fields, (void *)&mon_req)) {
    qsh_wifi_pr_high("pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, qsh_wifi_cmd_monitor_req_fields, (void *)&mon_req)) {
      qsh_wifi_pr_high("Error Encoding open request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;

}

static int qsh_wifi_send_monitor_mode_req(
    qmi_client_type qmi_handle, sns_std_suid *suid, bool action)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;
  int ret = 0;

  // State should be initialize
  if (state == DEINIT) {
    printf("Initialize the QMI Session first");
    return -1;
  }

  // Sanity for action
  if ((action == true) && (is_monitor_on == true)) {
    qsh_wifi_pr_high("Client is already requested monitor on, Ignore it");
    return 0;
  }

  if ((action == false) && (is_monitor_on == false)) {
    qsh_wifi_pr_high("Client have not requested monitor mode, Ignore it");
    return 0;
  }

  __android_log_print(ANDROID_LOG_VERBOSE, TAG,
      "Send Monitor request %s", action ? "ON" : "OFF");

  encoded_req_len = qsh_wifi_get_monitor_mode_req(&encoded_req, action);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid,
             QSH_WIFI_MSGID_QSH_WIFI_CMD_MONITOR_REQ);
    free(encoded_req);
  }

  qsh_wifi_pr_high("Waiting for Monitor mode Ack");
  printf("\nWaiting for Monitor mode Ack");
  ret = qsh_wifi_timeout_block(CMD_TIMEOUT_IN_MS, &cond);
  if (ret != 0) {
    printf("\nError occured while waiting for Monitor mode ack ret: %d", ret);
    return -2;
  }
  if (action)
    is_monitor_on = true;
  else
    is_monitor_on = false;
  return 0;
}


static int
qsh_wifi_send_on_demand_scan(qmi_client_type qmi_handle,
    sns_std_suid *suid, qsh_wifi_scan_db *scan_db)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;
  int ret = 0;

  // State should be initialize
  if (state == DEINIT) {
    printf("Initialize the QMI Session first");
    return -1;
  }

  qsh_wifi_pr_high("Send on demand scan request");

  encoded_req_len = qsh_wifi_encode_on_demand_scan(&encoded_req, scan_db);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid,
             QSH_WIFI_MSGID_QSH_WIFI_CMD_SCAN_REQ);
    free(encoded_req);
  }

  pthread_mutex_lock(&scan_mutex);
  // Reset the scan event result data base after sending the new scan request
  memset(&g_scan_evt_list, 0, (sizeof(qsh_wifi_scan_evt_db)*MAX_SCAN_EVENTS));
  g_scan_evt_list_count = 0;
  is_scan_requested = true;
  pthread_mutex_unlock(&scan_mutex);

  qsh_wifi_pr_high("Waiting for on demand scan Ack");
  printf("\nwaiting for on demand scan ack");
  ret = qsh_wifi_timeout_block(CMD_TIMEOUT_IN_MS * 2, &cond);
  if (ret != 0) {
    pthread_mutex_lock(&scan_mutex);
    is_scan_requested = false;
    pthread_mutex_unlock(&scan_mutex);
    printf("\nError occured while waiting for ACK ON DEMAND RSP: %d", ret);
    return -2;
  }
  return 0;
}

static int
qsh_wifi_send_rtt_req(qmi_client_type qmi_handle,
    sns_std_suid *suid, qsh_wifi_ranging_req_db *rtt_db)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;
  int ret = 0;

  // State should be initialize
  if (state == DEINIT) {
    printf("Initialize the QMI Session first");
    return -1;
  }

  qsh_wifi_pr_high("Send Ranging request");

  encoded_req_len = qsh_wifi_encode_rtt_request(&encoded_req, rtt_db);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid,
             QSH_WIFI_MSGID_QSH_WIFI_CMD_RANGING_REQ);
    free(encoded_req);
  }

  pthread_mutex_lock(&rtt_mutex);
  // Reset the scan event result data base after sending the new scan request
  memset(&g_rtt_evt_list, 0, (sizeof(qsh_wifi_ranging_evt_db)*MAX_RTT_EVENTS));
  g_rtt_evt_list_count = 0;
  is_rtt_requested = true;
  pthread_mutex_unlock(&rtt_mutex);

  qsh_wifi_pr_high("Waiting for on RTT request Ack");
  printf("\nwaiting for on  RTT request Ack");
  ret = qsh_wifi_timeout_block(CMD_TIMEOUT_IN_MS * 2, &cond);
  if (ret != 0) {
    pthread_mutex_lock(&rtt_mutex);
    is_rtt_requested = false;
    pthread_mutex_unlock(&rtt_mutex);
    printf("\nError occured while waiting for ACK ON RTT RSP: %d", ret);
    return -2;
  }
  return 0;
}

/**
 * Create an encoded location open request message.
 *
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t
qsh_wifi_encode_open_req(void **encoded_req)
{
  size_t encoded_req_size;
  qsh_wifi_cmd_open open_req = qsh_wifi_cmd_open_init_zero;
  open_req.get_capability = true;
  open_req.has_get_version = true;
  open_req.get_version = true;

  *encoded_req = NULL;

   __android_log_print(ANDROID_LOG_VERBOSE, TAG,
    "Get encoded Open request get_capability=%d get_version=%d",
     open_req.get_capability, open_req.get_version);

  if(!pb_get_encoded_size(&encoded_req_size, qsh_wifi_cmd_open_fields, &open_req)) {
    qsh_wifi_pr_high("pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, qsh_wifi_cmd_open_fields, &open_req)) {
      qsh_wifi_pr_high("Error Encoding open request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

static int qsh_wifi_send_open(qmi_client_type qmi_handle,
    sns_std_suid *suid)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;
  int ret = 0;

  // Before open intilization is essential.
  if (state != INIT) {
    qsh_wifi_pr_high("QMI is not ready yet");
    return -1;
  }

  qsh_wifi_pr_high("Send open request");

  encoded_req_len = qsh_wifi_encode_open_req(&encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid,
             QSH_WIFI_MSGID_QSH_WIFI_CMD_OPEN);
    free(encoded_req);
  }

  qsh_wifi_pr_high("Waiting for Open Ack");
  ret = qsh_wifi_timeout_block(CMD_TIMEOUT_IN_MS, &cond);
  if (ret != 0) {
    printf("\nError occured while waiting for Open Ready ret: %d", ret);
    return -2;
  }
  return 0;
}

static int qsh_wifi_send_dump_event(qmi_client_type qmi_handle,
    sns_std_suid *suid)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  // Before open intilization is essential.
  if (state != OPENED) {
    qsh_wifi_pr_high("QMI is not OPENED yet");
    return -1;
  }

  qsh_wifi_pr_high("Send dump event request");

  encoded_req_len = qsh_wifi_encode_open_req(&encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid,
             QSH_WIFI_MSGID_QSH_WIFI_CMD_OPEN-1);
    free(encoded_req);
  }

  return 0;
}

/**
 * Decode an attribute value.
 *
 * Once we find the attribute value we have been looking for (for example,
 * to differentiate two sensors with data type "wifi_svc"), we send an enable
 * request.
 */
static bool decode_attr_value(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  sns_buffer_arg str_data = (sns_buffer_arg){ .buf = NULL, .buf_len = 0 };

  value.str.funcs.decode = &decode_payload;
  value.str.arg = &str_data;

  value.subtype.values.funcs.decode = &decode_attr_value;

  if(!pb_decode(stream, sns_std_attr_value_data_fields, &value)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "Error decoding attribute: %s", PB_GET_ERROR(stream));
    return false;
  }

  if(value.has_flt)
    __android_log_print(ANDROID_LOG_WARN, TAG,
      "Attribute float: %f", value.flt);
  else if(value.has_sint)
    __android_log_print(ANDROID_LOG_WARN, TAG,
      "Attribute int: %"PRIi64, value.sint);
  else if(value.has_boolean)
    __android_log_print(ANDROID_LOG_WARN, TAG,
      "Attribute boolean: %i", value.boolean);
  else if(NULL != str_data.buf)
    __android_log_print(ANDROID_LOG_WARN, TAG,
      "Attribute string: %s", (char*)str_data.buf);
  else if(value.has_subtype)
    __android_log_print(ANDROID_LOG_WARN, TAG, "Attribute nested");
  else
    __android_log_print(ANDROID_LOG_WARN, TAG, "Unknown attribute type");

  return true;
}

/**
 * Decode a single attribute from the attributes array.
 */
static bool
decode_attr(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_attr attribute = sns_std_attr_init_default;
  attribute.value.values.funcs.decode = &decode_attr_value;
  attribute.value.values.arg = NULL;

  if(!pb_decode(stream, sns_std_attr_fields, &attribute)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "Error decoding attribute: %s", PB_GET_ERROR(stream));
    return false;
  }

  __android_log_print(ANDROID_LOG_WARN, TAG,
      "Attribute ID %i", attribute.attr_id);

  if(SNS_STD_SENSOR_ATTRID_NAME == attribute.attr_id) {
    // Based on some logic, choose this wifi Sensor and send enable request
    //qsh_wifi_send_open(qmi_sensors_handle, &wifi_suid);
    qsh_wifi_unblock_th();
  }

  return true;
}

/**
 * Decode the payload of the event message, i.e. the Attribute Event
 */
static bool decode_attr_event(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_attr_event attr_event = sns_std_attr_event_init_default;

  attr_event.attributes.funcs.decode = &decode_attr;
  __android_log_print(ANDROID_LOG_WARN, TAG,
      "Received Attribute Event length %zu", stream->bytes_left);

  if(!pb_decode(stream, sns_std_attr_event_fields, &attr_event)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "Error decoding Attr Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  return true;
}

/**
 * Decode the sensor event (aka a sensor sample).
 */
static bool decode_sensor_event(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_sensor_event event = sns_std_sensor_event_init_default;
  float sensor_data[3];
  uint8_t arr_index = 0;
  pb_float_arr_arg float_arg = (pb_float_arr_arg)
    { .arr = sensor_data, .arr_len = 3, .arr_index = &arr_index };

  event.data.funcs.decode = &decode_float_arr_cb;
  event.data.arg = &float_arg;

  if(!pb_decode(stream, sns_std_sensor_event_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "decode_sensor_event: Error in Attr Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  __android_log_print(ANDROID_LOG_VERBOSE, TAG,
      "Received sensor event.  (%f, %f, %f)",
      sensor_data[0], sensor_data[1], sensor_data[2]);

  return true;
}

static bool decode_demand_scan_freqs(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  qsh_wifi_scan_evt_db *evt_db = (qsh_wifi_scan_evt_db *)*arg;
  uint64_t value;

  if (!pb_decode_varint(stream, &value))
  {
    qsh_wifi_pr_high("On demand freq decoding failed");
    return false;
  }
  if (evt_db->freq_count >= MAX_FREQ_IN_SCAN_EVT )
  {
    qsh_wifi_pr_high("More than MAX Freq in scan evt %d", evt_db->freq_count);
    return false;
  }
  evt_db->freq_array[evt_db->freq_count++] = ((uint32_t)value);
  __android_log_print(ANDROID_LOG_ERROR, TAG,
      "\ndecoded freq:%u", (uint32_t)value);

  return true;
}

static bool decode_demand_scan_aps(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  qsh_wifi_scan_evt_db *evt_db = (qsh_wifi_scan_evt_db *)*arg;
  qsh_wifi_bss_info msg = qsh_wifi_bss_info_init_default;
  qsh_wifi_bss_info *ap = NULL;
  if(!pb_decode(stream, qsh_wifi_bss_info_fields, (void *)&msg)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Error while decoding demand scan ap's %s", PB_GET_ERROR(stream));
    return false;
  }

  if (evt_db->result_count >= MAX_AP_IN_SCAN_EVT )
  {
    qsh_wifi_pr_high("More than MAX AP's in scan evt %d", evt_db->result_count);
    return false;
  }
  ap = &evt_db->ap_array[evt_db->result_count];
  memset(ap, 0, sizeof(*ap));
  memcpy(ap, &msg, sizeof(*ap));

  if (ap->bssid.size)
  {
    __android_log_print(ANDROID_LOG_ERROR, TAG,"AP bssid: " MAC_ADDR_FMT,
      MACADDR(ap->bssid.bytes));
  }
  if (strlen(ap->ssid))
  {
    __android_log_print(ANDROID_LOG_ERROR, TAG,"AP Ssid len: %zu SSID: %.*s",
        strlen(ap->ssid), (int)strlen(ap->ssid), ap->ssid);
  }
  evt_db->result_count++;
  return true;
}

static void print_scan_events(qsh_wifi_scan_evt_db *evt_list,
    uint32_t list_count, bool on_console)
{
  uint32_t evt_cnt = 0;
  uint32_t total_ap = 0;

  if (on_console) {
    printf("\n\n%4s %15s %15s %8s %8s",
                 "SNO","MAC", "freq", "RSSI", "SSID");
    printf("\n----------------------------------------------------------");
  }
  for(evt_cnt = 0; evt_cnt < list_count; evt_cnt++)
  {
    uint32_t ap_cnt = 0;
    qsh_wifi_scan_evt_db *msg = &evt_list[evt_cnt];
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "status:%u last_in_series:%i result total:%i count:%i scan_type:(%i:%i) radio_chain:(%i :%i)",
      msg->status, msg->last_in_series, msg->result_total, msg->result_count,
      msg->has_scan_type, msg->scan_type,
      msg->has_radio_chain_pref, msg->radio_chain_pref);
    for (ap_cnt = 0; ap_cnt < msg->result_count; ap_cnt++)
    {
     total_ap++;
      qsh_wifi_bss_info *ap = &msg->ap_array[ap_cnt];
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "ap_no_in_evt:%i band:%i rssi:%i freq:%i width:%i cap_info %u CF0(%s %i) CF1(%s %i)",
          total_ap, ap->band, ap->rssi, ap->freq, ap->channel_width, ap->capability_info,
          (ap->has_cent_freq0 == 1) ? "T" : "F", ap->cent_freq0,
          (ap->has_cent_freq1 == 1) ? "T" : "F", ap->cent_freq1);
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "AP pairs age(%s, %u) flags(%s, %u) sec(%s %u) radio_chain(%s %d) rssi0 (%s %i) rssi1(%s %i)",
          (ap->has_age_ms == 1) ? "T" : "F", ap->age_ms,
          (ap->has_flags == 1) ? "T" : "F", ap->flags,
          (ap->has_security_mode == 1) ? "T" : "F", ap->security_mode,
          (ap->has_radio_chain == 1) ? "T" : "F", ap->radio_chain,
          (ap->has_rssi_chain0 == 1) ? "T" : "F", ap->rssi_chain0,
          (ap->has_rssi_chain1 == 1) ? "T" : "F", ap->rssi_chain1);
      if (on_console)
        printf("\n%3d) %4s"MAC_ADDR_FMT " %4s %4u %3s %4d %3s %-32s", total_ap,
             "", MACADDR(ap->bssid.bytes), "", ap->freq, "", ap->rssi, "",
             strlen(ap->ssid) ? ap->ssid : "");
    }
  }
}

static bool decode_on_demand_scan_evt(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);

  qsh_wifi_evt_scan_result msg = qsh_wifi_evt_scan_result_init_default;
  qsh_wifi_scan_evt_db *evt = NULL;
  uint32_t tmp_cnt = 0;
  bool is_monitor_results = false;

  // If flow reaches here it means we are waiting for scan
  // result surely (either demand scan or monitor scan).
  pthread_mutex_lock(&scan_mutex);
  if (is_scan_requested)   {
    evt = &g_scan_evt_list[g_scan_evt_list_count++];
    tmp_cnt = g_scan_evt_list_count;
  } else {
    evt = &g_mon_evt_list[g_mon_evt_list_count++];
    tmp_cnt = g_mon_evt_list_count;
  }
  pthread_mutex_unlock(&scan_mutex);

  if (tmp_cnt >= MAX_SCAN_EVENTS)
  {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "Recevied more than 20 max Scan events ie more than 200 AP's");
    return false;
  }
  msg.scanned_freq_list.funcs.decode = &decode_demand_scan_freqs;
  msg.scanned_freq_list.arg = evt;

  msg.ap_list.funcs.decode = &decode_demand_scan_aps;
  msg.ap_list.arg = evt;

  if(!pb_decode(stream, qsh_wifi_evt_scan_result_fields,(void *)&msg)) {
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Error decoding on demand scan evt %s", PB_GET_ERROR(stream));
      return false;
  }

  evt->status = msg.status;
  evt->result_total = msg.result_total;
  evt->result_count = msg.result_count;
  evt->last_in_series = msg.last_in_series;
  evt->has_scan_type = msg.has_scan_type;
  evt->scan_type = msg.scan_type;
  evt->has_radio_chain_pref = msg.has_radio_chain_pref;
  evt->radio_chain_pref = msg.radio_chain_pref;
  evt->has_ref_time = msg.has_ref_time;
  evt->ref_time = msg.ref_time;

  if (evt->last_in_series)
  {
    // Transfer data to tmp list so printing can be mutex free
    qsh_wifi_scan_evt_db tmp_list [MAX_SCAN_EVENTS];
    uint32_t evt_cnt = 0;
    memset(&tmp_list, 0, (sizeof(qsh_wifi_scan_evt_db)*MAX_SCAN_EVENTS));
    evt_cnt = 0;

    pthread_mutex_lock(&scan_mutex);
    if (is_scan_requested)   {
      memcpy(&tmp_list, &g_scan_evt_list, sizeof(qsh_wifi_scan_evt_db) * MAX_SCAN_EVENTS);
      evt_cnt = g_scan_evt_list_count;
      memset(&g_scan_evt_list, 0, (sizeof(qsh_wifi_scan_evt_db)*MAX_SCAN_EVENTS));
      g_scan_evt_list_count = 0;
      is_monitor_results = false;
    } else {
      memcpy(&tmp_list, &g_mon_evt_list, sizeof(qsh_wifi_scan_evt_db) * MAX_SCAN_EVENTS);
      evt_cnt = g_mon_evt_list_count;
      memset(&g_mon_evt_list, 0, (sizeof(qsh_wifi_scan_evt_db)*MAX_SCAN_EVENTS));
      g_mon_evt_list_count = 0;
      is_monitor_results = true;
    }
    pthread_mutex_unlock(&scan_mutex);

    // Dump result on console for non monitor mode
    print_scan_events((qsh_wifi_scan_evt_db *)&tmp_list, evt_cnt,
       (is_monitor_results==false) ? true : false);
    // Received last scan result message from Sesnor
    // hence unblock the main thread of client so
    // it can issue the next request.
    qsh_wifi_unblock_th();
  }
  return true;
}

bool decode_ranging_aps(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  qsh_wifi_ranging_evt_db *evt_db = (qsh_wifi_ranging_evt_db *)*arg;
  qsh_wifi_ranging_result_ap msg = qsh_wifi_ranging_result_ap_init_default;
  qsh_wifi_ranging_result_ap *ap = NULL;
  if(!pb_decode(stream, qsh_wifi_ranging_result_ap_fields, (void *)&msg)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Error while decoding ranging ap's %s", PB_GET_ERROR(stream));
    return false;
  }

  if (evt_db->result_count >= MAX_FRAG_RANGING_BSS )
  {
    qsh_wifi_pr_high("More than MAX AP's in RTT evt %d", evt_db->result_count);
    return false;
  }
  ap = &evt_db->bss_list[evt_db->result_count++];
  memset(ap, 0, sizeof(*ap));
  memcpy(ap, &msg, sizeof(*ap));

  return true;
}

void print_ranging_events(qsh_wifi_ranging_evt_db *evt_list,
    uint32_t list_count, bool on_console)
{
  uint32_t evt_cnt = 0;
  uint32_t total_ap = 0;

  if (on_console) {
    printf("\n\n%4s %15s %15s %8s",
                 "SNO","MAC", "RSSI", "Distanc(mm)");
    printf("\n-----------------------------------------------------------");
  }
  for(evt_cnt = 0; evt_cnt < list_count; evt_cnt++)
  {
    uint32_t ap_cnt = 0;
    qsh_wifi_ranging_evt_db *msg = &evt_list[evt_cnt];
    __android_log_print(ANDROID_LOG_ERROR, TAG,
        "status:%u last_in_series:%i result total:%i count:%i",
        msg->status, msg->last_in_series, msg->result_total, msg->result_count);
    for (ap_cnt = 0; ap_cnt < msg->result_count; ap_cnt++)
    {
      total_ap++;
      qsh_wifi_ranging_result_ap *ap = &msg->bss_list[ap_cnt];
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "ap_no_in_evt:%i rtt_status:%i flag:%u  rssi:%i distance_mm:%i distance_dev:%d ",
          total_ap, ap->rtt_status, ap->result_flag,
          ap->rssi, ap->distance, ap->distance_stddev);

      if (ap->result_flag & QSH_RTT_RESULT_HAS_LCI)
      {
        __android_log_print(ANDROID_LOG_ERROR, TAG,
          "lat(%s %ld) long (%s %ld) altitude(%s %i) alt_type(%s %d)  lat_unc(%s %u) long_unc(%s %u) alt_unc(%s %u)",
          (ap->has_latitude == 1) ? "T" : "F", ap->latitude,
          (ap->has_longitude == 1) ? "T" : "F", ap->longitude,
          (ap->has_altitude == 1) ? "T" : "F", ap->altitude,
          (ap->has_altitude_type == 1) ? "T" : "F", ap->altitude_type,
          (ap->has_latitude_uncertainty == 1) ? "T" : "F", ap->latitude_uncertainty,
          (ap->has_longitude_uncertainty == 1) ? "T" : "F", ap->longitude_uncertainty,
          (ap->has_altitude_uncertainty == 1) ? "T" : "F", ap->altitude_uncertainty);
      }
      if (on_console)
        printf("\n%3d) %4s"MAC_ADDR_FMT " %4s %3d %4s %3d", total_ap,
             "", MACADDR(ap->mac_addr.bytes), "", ap->rssi, "",
             ap->distance);
    }
  }
}

bool decode_ranging_evt(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);

  qsh_wifi_evt_ranging_result msg = qsh_wifi_evt_ranging_result_init_default;
  qsh_wifi_ranging_evt_db *evt = NULL;
  uint32_t tmp_cnt = 0;

  // If flow reaches here it means we are waiting for RTT results
  pthread_mutex_lock(&rtt_mutex);
  if (is_rtt_requested) {
    evt = &g_rtt_evt_list[g_rtt_evt_list_count++];
    tmp_cnt = g_rtt_evt_list_count;
  }
  pthread_mutex_unlock(&rtt_mutex);

  if (tmp_cnt >= MAX_RTT_EVENTS)
  {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "Recevied more than 5 max RTT events ie more than 50 AP's");
    return false;
  }
  msg.bss_list.funcs.decode = &decode_ranging_aps;
  msg.bss_list.arg = evt;

  if(!pb_decode(stream, qsh_wifi_evt_ranging_result_fields,(void *)&msg)) {
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Error decoding ranging evt %s", PB_GET_ERROR(stream));
      return false;
  }

  evt->status = msg.status;
  evt->result_total = msg.result_total;
  evt->result_count = msg.result_count;
  evt->last_in_series = msg.last_in_series;

  if (evt->last_in_series)
  {
    pthread_mutex_lock(&rtt_mutex);
    if (is_rtt_requested)
    {
      // Dump result on console ranging results
      print_ranging_events((qsh_wifi_ranging_evt_db *)&g_rtt_evt_list,
          g_rtt_evt_list_count, true);
      memset(&g_rtt_evt_list, 0, (sizeof(qsh_wifi_ranging_evt_db) * MAX_RTT_EVENTS));
      g_rtt_evt_list_count = 0;
      is_rtt_requested = false;
    }
    pthread_mutex_unlock(&rtt_mutex);

    // Received last scan result message from Sesnor
    // hence unblock the main thread of client so
    // it can issue the next request.
    qsh_wifi_unblock_th();
  }
  return true;
}

static bool decode_qsh_wifi_cmd_ack(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  qsh_wifi_cmd_ack ack = qsh_wifi_cmd_ack_init_default;

  if(!pb_decode(stream, qsh_wifi_cmd_ack_fields, &ack)) {
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Error decoding wifi ack: %s", PB_GET_ERROR(stream));
      return false;
  }
  __android_log_print(ANDROID_LOG_INFO, TAG,
      "Received ACK status=%u command=%s",
      ack.status, get_cmd_name(ack.command));

  switch (ack.command)
  {
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_OPEN:
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Received ack for OPEN request");
      __android_log_print(ANDROID_LOG_VERBOSE, TAG,
        "has_version=%d version=0x%" PRIx32"", ack.has_version, ack.version);
      __android_log_print(ANDROID_LOG_VERBOSE, TAG,
        "has_caps=%d caps=%u", ack.has_capabilities, ack.capabilities);
      qsh_wifi_unblock_th();
      break;
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_SCAN_REQ:
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Received Scan Req ACK, waiting for Scan Event from Sensor");
     // We will not unblock main thread untill we received last_in_series
     // message or time out happens unless sensor send ack as failure.
     if (ack.status != QSH_WIFI_ERROR_NONE)
       qsh_wifi_unblock_th();
     break;
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_RANGING_REQ:
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Received Scan Req ACK, waiting for RANGING Event from Sensor");
     // We will not unblock main thread untill we received last_in_series
     // message or time out happens unless sensor send ack as failure.
     if (ack.status != QSH_WIFI_ERROR_NONE)
       qsh_wifi_unblock_th();
     break;
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_MONITOR_REQ:
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Received Monitor Mode ACK");
      qsh_wifi_unblock_th();
      break;
    default:
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Received ack for unknown request");
      break;
  }

  return true;
}

/**
 * Decode an event message received from the wifi Sensor.
 */
static bool decode_wifi_event(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  uint32_t *msg_id = (uint32_t*)*arg;
  bool waiting_for_scan_result = false;

  __android_log_print(ANDROID_LOG_INFO, TAG,
      "*** Begin Decoding QSH Wifi event msg_id:%s", get_event_name(*msg_id));

  switch (*msg_id)
  {
    case SNS_STD_MSGID_SNS_STD_ATTR_EVENT:
      qsh_wifi_pr_high("Received SNS_STD_MSGID_SNS_STD_ATTR_EVENT");
      return decode_attr_event(stream, field, arg);
    case SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT:
      return decode_sensor_event(stream, field, arg);
    case SNS_STD_MSGID_SNS_STD_ERROR_EVENT:
      __android_log_print(ANDROID_LOG_ERROR, TAG, "Received Error event");
      return false;
    case QSH_WIFI_MSGID_QSH_WIFI_CMD_ACK:
      __android_log_print(ANDROID_LOG_VERBOSE, TAG,
          "Received QSM_WIFI_CMD_RSP");
      return decode_qsh_wifi_cmd_ack(stream, field, arg);
    case QSH_WIFI_MSGID_QSH_WIFI_EVT_SCAN_RESULTS:
      __android_log_print(ANDROID_LOG_VERBOSE, TAG,
          "Received QSH_WIFI_EVT_SCAN_RESULTS");
      pthread_mutex_lock(&scan_mutex);
      waiting_for_scan_result = is_scan_requested || is_monitor_on;
      pthread_mutex_unlock(&scan_mutex);
      if (waiting_for_scan_result == false)
      {
        __android_log_print(ANDROID_LOG_VERBOSE, TAG,
          "Ignore SCAN Event, Not waiting for monitor/demand scan result");
        return false;
      }
      else
      {
        return decode_on_demand_scan_evt(stream, field, arg);
      }
    case QSH_WIFI_MSGID_QSH_WIFI_EVT_RANGING_RESULTS:
      __android_log_print(ANDROID_LOG_VERBOSE, TAG,
          "Received QSH_WIFI_EVT_RANGING_RESULTS");
      pthread_mutex_lock(&rtt_mutex);
      if (is_rtt_requested == false)
      {
        pthread_mutex_unlock(&rtt_mutex);
        __android_log_print(ANDROID_LOG_VERBOSE, TAG,
          "Ignore RANGING Event, Not waiting for RANGING result");
        return false;
      }
      else
      {
        pthread_mutex_unlock(&rtt_mutex);
        return decode_ranging_evt(stream, field, arg);
      }
    default:
      __android_log_print(ANDROID_LOG_ERROR, TAG,
       "Received unknown event %i", *msg_id);
      return false;
  }
}

/**
 * Each SUID event contains an array of SUIDs.  This function will be called
 * once per each SUID in the array.
 *
 * At this time we will send an attributes request to the SUID.
 */
static bool
decode_suid(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_suid uid;

  if(!pb_decode(stream, sns_std_suid_fields, &uid)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "Error decoding SUID: %s", PB_GET_ERROR(stream));
    return false;
  }

  __android_log_print(ANDROID_LOG_WARN, TAG,
      "Received SUID Event with SUID %"PRIx64" %"PRIx64,
      uid.suid_low, uid.suid_high);

  send_attr_req(qmi_sensors_handle, &uid);

  return true;
}

/**
 * Decode the payload of the event message, i.e. the SUID Event
 *
 * @param[i] arg Message ID
 */
static bool decode_suid_event(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_suid_event suid_event;
  sns_buffer_arg data;

  suid_event.suid.funcs.decode = &decode_suid;
  suid_event.data_type.funcs.decode = &decode_payload;
  suid_event.data_type.arg = &data;

  if(!pb_decode(stream, sns_suid_event_fields, &suid_event)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "Error decoding SUID Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  {
    char data_type[data.buf_len + 1];
    strlcpy(data_type, (char*)data.buf, data.buf_len);

    __android_log_print(ANDROID_LOG_WARN, TAG,
        "Received SUID Event with data type '%s'", data_type);
  }

  return true;
}

/**
 * Get the message ID of the encoded event residing on stream.
 *
 * @return Message ID
 */
static uint32_t
get_msg_id(pb_istream_t *stream)
{
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "event: %s", PB_GET_ERROR(stream));
  } else {
    __android_log_print(ANDROID_LOG_INFO, TAG,
      "Decoding event with message ID %u, timestamp %"PRIu64,
        event.msg_id, event.timestamp);
    return event.msg_id;
  }

  return 0;
}

/*
 * Decode an element of sns_client_event_msg::events.  This function will be
 * called by nanopb once per each element in the array.
 *
 * @param[i] arg Sensor-specific decode function
 */
static bool
decode_events(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  bool rv = true;
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;
  pb_istream_t stream_cpy = *stream;

  __android_log_print(ANDROID_LOG_WARN, TAG,
      "Begin decoding event");
  uint32_t msg_id = get_msg_id(&stream_cpy);

  event.payload.funcs.decode = *arg;
  event.payload.arg = &msg_id;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "Error decoding Event: %s", PB_GET_ERROR(stream));
    rv = false;
  }

  __android_log_print(ANDROID_LOG_WARN, TAG,
      "Event decoding complete");
  return rv;
}

/**
 * Handle a received event message from the Sensor Service.
 *
 * This example code only ever sends requests to the SUID Lookup Sensor
 * and a wifi_svc Sensor.  Therefore we only need to check against those two
 * SUIDs below.  If this client were to use more Sensors, either:
 *  - The client would open separate QMI connections for each Sensor
 *    (and then the SUID would be known implicitly because of the indication
 *    callback function the event was received from).
 *  - Save the SUIDs in some sort of global state, so that they can be used
 *    for comparison here.
 *
 * @param[i] suid Source Sensor of this event
 * @param[i] event_msg PB-encoded message of type sns_client_event_msg
 * @param[i] event_msg_len Length (in bytes) of the encoded event_msg
 */
static void qsh_wifi_handle_events(sns_std_suid const *suid,
    void const *event_msg, size_t event_msg_len)
{
  pb_istream_t stream;
  sns_client_event_msg event = sns_client_event_msg_init_default;

  __android_log_print(ANDROID_LOG_WARN, TAG,
      "Processing events from SUID %"PRIx64" %"PRIx64 " req suid_lookup:%"PRIx64 "suid_lookup:%"PRIx64,
      suid->suid_low, suid->suid_high, suid_lookup.suid_low, suid_lookup.suid_high);

  // Initial message is send by suid sensor and
  // later on it will be send by sns wifi sensor
  if((suid->suid_high == suid_lookup.suid_high) &&
     (suid->suid_low == suid_lookup.suid_low)) {
    event.events.funcs.decode = &decode_events;
    event.events.arg = &decode_suid_event;
  } else {
    wifi_suid = *suid;

    event.events.funcs.decode = &decode_events;
    event.events.arg = &decode_wifi_event;
  }

  stream = pb_istream_from_buffer(event_msg, event_msg_len);
  if(!pb_decode(&stream, sns_client_event_msg_fields, &event)) {
     qsh_wifi_pr_high("Error decoding event list: %s", PB_GET_ERROR(&stream));
  }
}

/**
 * QMI Error callback.  This would be our notification if the QMI connection
 * was lost.  A typical implementation would re-establish the connection, and
 * resend any active requests.
 *
 * See qmi_client_error_cb. */
static void
qsh_wifi_error_cb(qmi_client_type user_handle,
    qmi_client_error_type error, void *err_cb_data)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(err_cb_data);

  qsh_wifi_pr_high("QMI error callback %i", error);
  // Let us cleanup our stuff and wait for reinit.
}

/* QMI indication callback.  See qmi_client_ind_cb. */
static void
qsh_wifi_ind_msg_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(ind_cb_data);
  size_t ind_len = sizeof(sns_client_report_ind_msg_v01);
  sns_client_report_ind_msg_v01 *ind = malloc(ind_len);
  int32_t qmi_err;

  __android_log_print(ANDROID_LOG_WARN, TAG,
                      "Received QMI Indication; len %u pid = %d threadid = %ld",
                      ind_buf_len, pid, pthread_self());

  // Extract the Protocol Buffer encoded message from the outer QMI/IDL  message
  qmi_err = qmi_idl_message_decode(SNS_CLIENT_SVC_get_service_object_v01(),
      QMI_IDL_INDICATION, msg_id, ind_buf, ind_buf_len, ind, ind_len);
  if(QMI_IDL_LIB_NO_ERR != qmi_err) {
      qsh_wifi_pr_high("QMI decode error %d", qmi_err);
  } else {
    sns_client_event_msg event = sns_client_event_msg_init_default;
      qsh_wifi_pr_high("Indication from client ID %"PRIu64, ind->client_id);

    // Decode just the sns_client_event_msg in order to get the SUID
    pb_istream_t stream = pb_istream_from_buffer(ind->payload,
                              ind->payload_len);
    if(pb_decode(&stream, sns_client_event_msg_fields, &event)) {
      qsh_wifi_handle_events(&event.suid, ind->payload, ind->payload_len);
    } else {
        qsh_wifi_pr_high("Error decoding Event msg: %s", PB_GET_ERROR(&stream));
    }
  }

  free(ind);
  qsh_wifi_pr_high("Indication processing completed");
}

/**
 * Create a QMI connection to Sensors
 *
 * @param[i] qmi_handle QMI Handle created in wait_for_service
 * @param[i] timeout Time-out in milliseconds. 0 = no timeout
 *
 * @return True on success; false otherwise
 */
static bool
qsh_wifi_client_conn(qmi_client_type *qmi_handle, uint32_t timeout)
{
  bool rv = false;
  qmi_idl_service_object_type service_obj =
      SNS_CLIENT_SVC_get_service_object_v01();
  qmi_service_instance instance_id = 0;
  qmi_client_error_type qmi_err;
  qmi_cci_os_signal_type os_params;

  qsh_wifi_pr_high("Creating client connection");

  qmi_err = qmi_client_init_instance(service_obj,
      instance_id, qsh_wifi_ind_msg_cb, NULL, &os_params, timeout, qmi_handle);

  if(QMI_NO_ERR != qmi_err) {
      qsh_wifi_pr_high("qmi_client_init_instance error %i", qmi_err);
  } else {
    qmi_err = qmi_client_register_error_cb(*qmi_handle,
                  qsh_wifi_error_cb, NULL);
    if(QMI_NO_ERR != qmi_err) {
        qsh_wifi_pr_high("qmi_client_register_error_cb error %i", qmi_err);
    } else {
      rv = true;
    }
  }

  if(!rv) {
    if(NULL != *qmi_handle) {
      qmi_client_release(*qmi_handle);
      *qmi_handle = NULL;
    }
  }

  return rv;
}

static int parse_int_element(xmlNode *pXmlNode, int *value,
                             const xmlChar * const xmlString)
{
  int retVal = -1;
  const char *payloadData = (const char *)xmlNodeGetContent(pXmlNode);
  do
  {
    if (NULL == payloadData)
    {
      __android_log_print(ANDROID_LOG_WARN, TAG,
          "%s:%s No Payload\n", __FUNCTION__, xmlString);
      break;
    }
    __android_log_print(ANDROID_LOG_WARN, TAG,
         "%s:%s Payload: %s\n", __FUNCTION__, xmlString, payloadData);
    int status = sscanf(payloadData, "%d", value);
    if (status < 1)
    {
      __android_log_print(ANDROID_LOG_WARN, TAG,
          "%s: Element %s not formed correctly. Value = %d\n",
                      __FUNCTION__, xmlString, *value);
      break;
    }
    retVal = 0;
  }
  while (0);
  return retVal;
}

static void parse_scan_elements(xmlNode *a_node,
    qsh_wifi_scan_db *cmd)
{
  xmlNode *cur_node = NULL;
  for (cur_node = a_node; cur_node; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      __android_log_print(ANDROID_LOG_WARN, TAG,
        "parse_elements: Element, name: %s\n", cur_node->name);
      if (xmlStrncmp (cur_node->name, XML_NODE_SCAN_TYPE,
                               xmlStrlen(XML_NODE_SCAN_TYPE)) == 0)
      {
        int scan_type;
        int ret = parse_int_element(cur_node, &scan_type, XML_NODE_SCAN_TYPE);
        if (ret == 0)
          cmd->scan_type = scan_type;
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_SCAN_AGE_MS,
                               xmlStrlen(XML_NODE_SCAN_AGE_MS)) == 0)
      {
        int age_ms;
        int status = parse_int_element(cur_node, &age_ms, XML_NODE_SCAN_AGE_MS);
        if (status == 0)
        {
          cmd->has_max_scan_age_ms  =true;
          cmd->max_scan_age_ms = age_ms;
        }
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_BAND,
                               xmlStrlen(XML_NODE_BAND)) == 0)
      {
        int band;
        int status = parse_int_element(cur_node, &band, XML_NODE_BAND);
        if (status == 0)
          cmd->band = band;
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_RADIO_CHAIN,
                               xmlStrlen(XML_NODE_RADIO_CHAIN)) == 0)
      {
        int radio;
        int status = parse_int_element(cur_node, &radio, XML_NODE_RADIO_CHAIN);
        if (status == 0)
        {
          cmd->has_radio_chain_pref = true;
          cmd->radio_chain_pref = radio;
        }
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_FREQS,
                               xmlStrlen(XML_NODE_FREQS)) == 0)
      {
        const char *payloadData = (const char *)xmlNodeGetContent(cur_node);
        uint32_t freq = 0;
        if (payloadData)
        {
          __android_log_print(ANDROID_LOG_WARN, TAG,
              "parse_elements: Element len: %zu value: %s\n",
              strlen(payloadData), payloadData);
          char * data = (char *)malloc(strlen(payloadData)+1);
          if (data)
          {
            strlcpy(data, payloadData, (size_t)strlen(payloadData)+1);
            char *ctx = NULL;
            char *word = strtok_r(data, ",", &ctx);
            while( word != NULL )
            {
              freq = atoi(word);
              if (freq && (cmd->freq_count < MAX_FREQ_IN_SCAN_EVT))
              {
                cmd->freq_list[cmd->freq_count] = freq;
                cmd->freq_count++;
              }
              __android_log_print(ANDROID_LOG_WARN, TAG,
                  " %s\n word len %zu freq:%u", word, strlen(word), freq);
              word = strtok_r(NULL, ",", &ctx);
            }
            free(data);
          }
        }
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_SSIDS,
                               xmlStrlen(XML_NODE_SSIDS)) == 0)
      {
        const char *payloadData = (const char *)xmlNodeGetContent(cur_node);
        if (payloadData)
        {
          __android_log_print(ANDROID_LOG_WARN, TAG,
              "parse_elements: Element len: %zu value: %s\n",
              strlen(payloadData), payloadData);
          char * data = (char *)malloc(strlen(payloadData)+1);
          if (data)
          {
            strlcpy(data, (char *)payloadData, (size_t)strlen(payloadData)+1);
          __android_log_print(ANDROID_LOG_WARN, TAG,
              "**** Data len: %zu value: %s\n", strlen(data), data);
            char *ctx = NULL;
            char *word = strtok_r(data, ",", &ctx);
            while( word != NULL )
            {
              if ((cmd->ssid_count < MAX_SSID_CNT) &&
                  strlen(word) < (MAX_SSID_SIZE+1))
              {
                strlcpy((char *)&cmd->ssid_list[cmd->ssid_count][0], word,
                        strlen(word)+1);
                cmd->ssid_count++;
              }
              __android_log_print(ANDROID_LOG_WARN, TAG,
                  "ssid %s\n len %zu", word, strlen(word));
              word = strtok_r(NULL, ",", &ctx);
            }
            free(data);
          }
        }
      }
    }
    parse_scan_elements(cur_node->children, cmd);
  }
}

/**
 * Below is the template of the SCAN Schema please put it as /data/scan.xml
 * NOTE: Heres freq's/SSID's are comma seperated values.
 *       SPACE in SSID will be treated as part of the name hence ensure
 *       No extra space are there in name.
 *
 * <scan>
 *  <scan_type>1</scan_type>
 *  <age_in_ms>1000</age_in_ms>
 *  <band>3</band>
 *  <radio_chain_pref>3</radio_chain_pref>
 *  <freqs>2412,2417,2422,2427,2432,2437,2442,2447,
 *   2452,2457,2462,2467,2472,2484
 *  </freqs>
 *  <ssids>HYDRA,Pandora,Guest</ssids>
 * </scan>
*/
static int parse_scan_xml(const char * const scan_xml,
    qsh_wifi_scan_db *cmd )
{
  // Open the xml file
  FILE *xmlFile = fopen(scan_xml, "rb");
  if (xmlFile == NULL)
  {
    fprintf(stderr, "%s:%d: Error opening file %s: %s\n",
            __func__,__LINE__, scan_xml, strerror(errno));
    return -1;
  }
  else
  {
    // File opened. Read it
    fseek(xmlFile, 0, SEEK_END);
    int xmlSize = (int)ftell(xmlFile);
    fseek(xmlFile, 0, SEEK_SET);
    __android_log_print(ANDROID_LOG_WARN, TAG,
        "scan xml file size %d", xmlSize);
    char* buffer = (char *)malloc(xmlSize);
    if (NULL == buffer)
    {
      fprintf(stderr, "Unable to allocate memory for provided xml size = %x\n",
              xmlSize);
      return -2;
    }
    fread(buffer, xmlSize, 1, xmlFile);
    fclose(xmlFile);

    xmlDoc *doc = xmlParseMemory(buffer, xmlSize);
    if (!doc)
    {
       __android_log_print(ANDROID_LOG_WARN, TAG,
           "cannot parse the xml file");
       return -3;
    }
    xmlNode *root_element = xmlDocGetRootElement(doc);
    if (!root_element)
    {
       __android_log_print(ANDROID_LOG_WARN, TAG,
           "cannot find the root element scan xml file");
       return -4;
    }

    //Ensure Parent node is <scan> node only
    if ((root_element->type == XML_ELEMENT_NODE) &&
         (xmlStrncmp (root_element->name, XML_NODE_SCAN,
                               xmlStrlen(XML_NODE_SCAN)) == 0))
    {
      parse_scan_elements(root_element, cmd);
    }
    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    // Free the buffer
    if (buffer)
      free(buffer);
  }

  return 0;
}


static void parse_rtt_elements(xmlNode *a_node,
    qsh_wifi_ranging_req_db *cmd)
{
  UNUSED_VAR(cmd);
  xmlNode *cur_node = NULL;
  qsh_wifi_ranging_req_ap *ap = NULL;

  __android_log_print(ANDROID_LOG_ERROR, TAG, "entered parse_rtt_elements");

  if (cmd->bss_count)
      ap = &cmd->bss_list[cmd->bss_count - 1];
  else
      ap = &cmd->bss_list[0];

  for (cur_node = a_node; cur_node; cur_node = cur_node->next)
  {
    //Ensure current node is <ap> node only
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      __android_log_print(ANDROID_LOG_WARN, TAG,
        "parse_elements: Element, name: %s\n", cur_node->name);
      if(xmlStrncmp (cur_node->name, XML_NODE_AP,
                               xmlStrlen(XML_NODE_AP)) == 0)
      {
        if (cmd->bss_count >= MAX_REQ_RANGING_BSS)
        {
          __android_log_print(ANDROID_LOG_WARN, TAG, "Max Allowed BSS reached");
          break;
        }
        cmd->bss_count++;
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_FREQ,
                               xmlStrlen(XML_NODE_FREQ)) == 0)
      {
        int freq;
        int ret = parse_int_element(cur_node, &freq, XML_NODE_FREQ);
        if (ret == 0 && ap)
          ap->freq = freq;
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_NUM_FRAMES,
                               xmlStrlen(XML_NODE_NUM_FRAMES)) == 0)
      {
        int frames = qsh_wifi_ranging_req_ap_num_frames_per_burst_default;
        int ret = parse_int_element(cur_node, &frames, XML_NODE_NUM_FRAMES);
        if (ret == 0 && ap)
          ap->num_frames_per_burst = frames;
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_CF0,
                               xmlStrlen(XML_NODE_CF0)) == 0)
      {
        int freq0;
        int ret = parse_int_element(cur_node, &freq0, XML_NODE_CF0);
        if (ret == 0 && ap)
        {
          ap->has_cent_freq0 = true;
          ap->cent_freq0 = freq0;
        }
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_CF1,
                               xmlStrlen(XML_NODE_CF1)) == 0)
      {
        int freq1;
        int ret = parse_int_element(cur_node, &freq1, XML_NODE_CF1);
        if (ret == 0 && ap)
        {
          ap->has_cent_freq1 = true;
          ap->cent_freq1 = freq1;
        }
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_CW,
                               xmlStrlen(XML_NODE_CW)) == 0)
      {
        int cw;
        int ret = parse_int_element(cur_node, &cw, XML_NODE_CW);
        if (ret == 0 && ap)
        {
          ap->channel_width = cw;
        }
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_LCI,
                               xmlStrlen(XML_NODE_LCI)) == 0)
      {
        int lci;
        int ret = parse_int_element(cur_node, &lci, XML_NODE_LCI);
        if (ret == 0 && ap)
        {
          ap->request_lci = lci ? true : false;
        }
      }
      else if (xmlStrncmp (cur_node->name, XML_NODE_MAC,
                               xmlStrlen(XML_NODE_MAC)) == 0)
      {
        const char *payloadData = (const char *)xmlNodeGetContent(cur_node);
        if (payloadData)
        {
          __android_log_print(ANDROID_LOG_WARN, TAG,
              "parse_elements: Element len: %zu value: %s\n",
              strlen(payloadData), payloadData);
          uint8_t bssid[6];
          memset(bssid, 0, sizeof(bssid));
          int status = sscanf(payloadData, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                              &bssid[0], &bssid[1], &bssid[2],
                              &bssid[3], &bssid[4], &bssid[5]);
          if (status < 6)
          {
            // Either the element does not contain the content or it is
            // not formatted correctly. Set all the entries to 0
            __android_log_print(ANDROID_LOG_WARN, TAG,
                " mac adress is not formetted correctly");
          }
          else
          {
            if (ap)
            {
              memcpy(ap->mac_addr.bytes, bssid, 6);
              ap->mac_addr.size = 6;
            }
          }
        }
      }
    }
    parse_rtt_elements(cur_node->children, cmd);
  }
  __android_log_print(ANDROID_LOG_ERROR, TAG, "exit parse_rtt_elements");
}

/**
 * Below is the template of the RTT Schema
 * please put it as /data/rtt.xml.
 * <rtt>
 *   <ap>
 *    <mac>aa:bb:cc:dd:ee:ff</mac>
 *    <ch_width>2</ch_width>
 *    <num_frames>8</num_frames>
 *    <freq>5745</freq>
 *    <cent_freq0>5775</cent_freq0>
 *    <cent_freq1>0</cent_freq1>
 *    <lci>1</lci>
 *   </ap>
 *   <ap>
 *    <mac>11:22:33:44:55:66</mac>
 *    <ch_width>3</ch_width>
 *    <num_frames>8</num_frames>
 *    <freq>5500</freq>
 *    <cent_freq0>5530</cent_freq0>
 *    <cent_freq1>5610</cent_freq1>
 *    <lci>1</lci>
 *   </ap>
 * </rtt>
*/
int parse_rtt_xml(const char * const rtt_xml,
    qsh_wifi_ranging_req_db *cmd )
{
  // Open the xml file
  FILE *xmlFile = fopen(rtt_xml, "rb");
  __android_log_print(ANDROID_LOG_ERROR, TAG, "enterede parse_rtt_xml");
  if (xmlFile == NULL)
  {
    fprintf(stderr, "%s:%d: Error opening file %s: %s\n",
            __func__,__LINE__, rtt_xml, strerror(errno));
    return -1;
  }
  else
  {
    // File opened. Read it
    fseek(xmlFile, 0, SEEK_END);
    int xmlSize = (int)ftell(xmlFile);
    fseek(xmlFile, 0, SEEK_SET);
    __android_log_print(ANDROID_LOG_WARN, TAG,
        "rtt xml file size %d", xmlSize);
    char* buffer = (char *)malloc(xmlSize);
    if (NULL == buffer)
    {
      fprintf(stderr, "Unable to allocate memory for provided xml size = %x\n",
              xmlSize);
      return -2;
    }
    fread(buffer, xmlSize, 1, xmlFile);
    fclose(xmlFile);

    xmlDoc *doc = xmlParseMemory(buffer, xmlSize);
    if (!doc)
    {
       __android_log_print(ANDROID_LOG_WARN, TAG,
           "cannot parse the xml file");
       return -3;
    }
    xmlNode *root_element = xmlDocGetRootElement(doc);
    if (!root_element)
    {
       __android_log_print(ANDROID_LOG_WARN, TAG,
           "cannot find the root element rtt xml file");
       return -4;
    }

    //Ensure Parent node is <rtt> node only
    if ((root_element->type == XML_ELEMENT_NODE) &&
         (xmlStrncmp (root_element->name, XML_NODE_RTT,
                               xmlStrlen(XML_NODE_RTT)) == 0))
    {
      parse_rtt_elements(root_element, cmd);
    }
    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    // Free the buffer
    if (buffer)
      free(buffer);
  }

  __android_log_print(ANDROID_LOG_ERROR, TAG, "Exit parse_rtt_xml");
  return 0;
}

/**
 * Check whether the Sensor service is available.
 *
 * @param[i] timeout Timeout period in ms; 0 infinite
 *
 * @return true if the service is available, false upon timeout
 */
static bool
qsh_wifi_wait_for_service(uint32_t timeout)
{
  qmi_idl_service_object_type service_obj =
      SNS_CLIENT_SVC_get_service_object_v01();
  qmi_client_type notifier_handle;
  qmi_client_error_type qmi_err;
  qmi_cci_os_signal_type os_params;
  bool rv = true;

  qsh_wifi_pr_high("Waiting for qmi SEE service service_obj %p", service_obj);

  qmi_err = qmi_client_notifier_init(service_obj, &os_params, &notifier_handle);
  if(QMI_NO_ERR != qmi_err) {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
        "qmi_client_notifier_init error %i", qmi_err);
    rv = false;
  } else {
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, timeout);
    if(os_params.timed_out) {
      __android_log_print(ANDROID_LOG_ERROR, TAG,
          "Service is not available after %i timeout", timeout);
      rv = false;
    }
  }

  qmi_client_release(notifier_handle);
  qsh_wifi_pr_high("Waiting complete");
  return rv;
}

int qsh_wifi_init ()
{
  int ret = -1;
  // Before open intilization is essential.
  if (state == INIT) {
    qsh_wifi_pr_high("Client is already connected to Sensor");
    return 0;
  }
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  pthread_mutex_init(&scan_mutex, NULL);
  pthread_mutex_init(&rtt_mutex, NULL);

  qsh_wifi_pr_high("Starting QSH WIFI TEST");

  if(!qsh_wifi_wait_for_service(200))
    return -1;

  if(!qsh_wifi_client_conn(&qmi_sensors_handle, 200))
    return -2;

  if(!qsh_wifi_send_suid_req(qmi_sensors_handle, "wifi_svc"))
    return -3;

  // Wait for ready open
  printf("\nwaiting for QMI ready");
  ret = qsh_wifi_timeout_block(CMD_TIMEOUT_IN_MS, &cond);
  if (ret != 0)
  {
    printf("\nError occured while waiting for QMI ready ret: %d", ret);
    return -4;
  }
  state = INIT;

  pthread_mutex_lock(&scan_mutex);
  memset(&g_scan_evt_list, 0, (sizeof(qsh_wifi_scan_evt_db)*MAX_SCAN_EVENTS));
  g_scan_evt_list_count = 0;
  is_scan_requested = false;

  memset(&g_mon_evt_list, 0, (sizeof(qsh_wifi_scan_evt_db)*MAX_SCAN_EVENTS));
  g_mon_evt_list_count = 0;
  is_monitor_on= false;
  pthread_mutex_unlock(&scan_mutex);

  pthread_mutex_lock(&rtt_mutex);
  memset(&g_rtt_evt_list, 0, (sizeof(qsh_wifi_ranging_evt_db)*MAX_RTT_EVENTS));
  g_rtt_evt_list_count = 0;
  is_rtt_requested= false;
  pthread_mutex_unlock(&rtt_mutex);

  return 0;
}

int qsh_wifi_deinit()
{
  printf("\nExiting qsh wifi example state %u\n", state);
  if (state == DEINIT)
    return -1;

  qmi_client_release(qmi_sensors_handle);
  qmi_sensors_handle = NULL;

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&scan_mutex);
  pthread_mutex_destroy(&rtt_mutex);

  state = DEINIT;
  return 0;
}

int get_wifi_caps()
{
  if (!qmi_sensors_handle)
    return-1;

  return 0;
}

void qsh_wifi_signal_handler(int signal_id)
{
  printf("\nreceived signal: %d console interrupted", signal_id);
  qsh_wifi_deinit();
  state = DEINIT;
  printf ("\n *** terminated qsh_wifi_test successfully *** \n");
  exit(0);
}

static void dump_scan_req_params(qsh_wifi_scan_db *scan_db)
{
  uint32_t cnt = 0;

  __android_log_print(ANDROID_LOG_ERROR, TAG,
      "band: %u scan_type: %u has_age: %d age_ms: %u freq_cnt: %u ssid_cnt: %u",
      scan_db->band, scan_db->scan_type, scan_db->has_max_scan_age_ms,
      scan_db->max_scan_age_ms, scan_db->freq_count, scan_db->ssid_count);

  for (cnt = 0; cnt < scan_db->freq_count && cnt < MAX_SCAN_EVENTS; cnt++)
  {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
         "\n freq  %u", scan_db->freq_list[cnt]);
  }

  for (cnt = 0; cnt  < scan_db->ssid_count; cnt++)
  {
    __android_log_print(ANDROID_LOG_ERROR, TAG,
      "ssid len %zu %-32s", strlen((char *)&scan_db->ssid_list[cnt][0]),
      (char *)&scan_db->ssid_list[cnt][0]);
  }
}

static void dump_rtt_req_params(qsh_wifi_ranging_req_db *rtt_db)
{
  uint32_t cnt = 0;
  qsh_wifi_ranging_req_ap *ap = NULL;
  __android_log_print(ANDROID_LOG_ERROR, TAG,"Total AP in RTT req: %u",
      rtt_db->bss_count);

  for (cnt = 0; cnt < rtt_db->bss_count && cnt < MAX_REQ_RANGING_BSS; cnt++)
  {
    ap = &rtt_db->bss_list[cnt];
    __android_log_print(ANDROID_LOG_ERROR, TAG,"AP bssid: " MAC_ADDR_FMT,
        MACADDR(ap->mac_addr.bytes));
    __android_log_print(ANDROID_LOG_ERROR, TAG,
        "freq:%u BW:%u CF0:%u CF1:%u lci:%d num_frames:%u",
        ap->freq, ap->channel_width, ap->cent_freq0, ap->cent_freq1,
        ap->request_lci, ap->num_frames_per_burst);
  }

}

/**
 * Handle only cmd line interrupts
 * to send close to Sensor.
 * This helps to get Abort call stack
 *
 * Ctrl+C - SIGINT
 * Ctrl+\ - SIGQUIT
 * Ctrl+Z - SIGTSTP
 */
void register_signal()
{
  // Register signal handler
  signal(SIGINT, qsh_wifi_signal_handler);
  signal(SIGQUIT, qsh_wifi_signal_handler);
  signal(SIGTSTP, qsh_wifi_signal_handler);
}

qsh_wifi_scan_db g_scan_cmd;
qsh_wifi_ranging_req_db g_rtt_cmd;

int main(int argc, char* argv[])
{
  UNUSED_VAR(argc);
  UNUSED_VAR(argv);
  int ret = -1;
  int loop = 1;

  register_signal();
  pid = getpid();
  printf("\n *** start wifi test pid = %d threadid = %ld", pid, pthread_self());
  printf("\nusage: Wifi test %s\n\n","\n[1 -> Open]\n[2 -> monitor on]"
         "\n[3 -> monitor off]\n[4 -> on demand scan]"
         "\n[5 -> on demand with scan params]\n[6 -> rtt start]\n[exit -> 9]");

  while (loop)
  {
    /**
     * Min Buf size is 3 needed for single digit commands
     * like 0, 1, 2 ..9 consumed 1 bytes, then '\0' will
     * consume one byte so \n will not ne fit in 2 byte buffer.
     */
    char buf[3], *p;
    long cmd = -1;
    printf ("\nenter command:");
    p = fgets(buf, 3, stdin);
    if (NULL == p) {
        printf("Error: fgets returned nullptr !!\n");
    }
    cmd = strtol(buf, NULL, 10);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "CMD is %ld", cmd);
    if (!cmd)
    {
      printf("You have entered invalid cmd please enter again \n");
      continue;
    }
    switch (cmd)
    {
      case OPEN:
      {
        printf ("\nOpening QSH Wifi Driver Interface");
        if (state == OPENED)
        {
          printf ("\nAlready opened");
        }
        else
        {
          // Initialize the QMI stuff with WIFI Sensor
          ret = qsh_wifi_init();
          if(ret != 0)
          {
            printf("\nError occured initlization: %d Hence Stopping it", ret);
            ret = 0;
            loop = 0;
          }
          ret = qsh_wifi_send_open(qmi_sensors_handle, &wifi_suid);
          if (ret != 0)
          {
            printf ("\nOpen failed move to deinit state");
            qsh_wifi_deinit();
          }
          else
            state = OPENED;
        }
      }
      break;
      case MONITOR_MODE_ON:
      {
        printf ("\nSend On Montior Mode on request");
        ret = qsh_wifi_send_monitor_mode_req(qmi_sensors_handle,
                  &wifi_suid, true);
      }
      break;
      case MONITOR_MODE_OFF:
      {
        printf ("\nSend On Monitor Mode off request");
        ret = qsh_wifi_send_monitor_mode_req (qmi_sensors_handle,
                  &wifi_suid, false);
      }
      break;
      case ON_DEMAND_SCAN:
      {
        char buf[8], *p;
        long iter = 1;
        long delay = 1000*1000;// one sec
        uint32_t i = 0;
        printf("\nScan start");
        printf("\nEnter number of iteration: (Default is 1 iteration): ");
        p = fgets(buf, 8, stdin);
        if (NULL == p)
        {
          printf("Error: fgets returned nullptr for iterations!!\n");
          break;
        }
        iter = strtol(buf, NULL, 10);
        if (iter && iter != 1)
        {
          printf("\nDelay between iteration in msec: (Default is 1000 msec): ");
          p = fgets(buf, 8, stdin);
          if (p)
          {
            delay = strtol(buf, NULL, 10);
           /* convert in msec  */
           delay = ((delay == 0) ? 1000*1000 : delay * 1000);
          }
          else
          {
            printf("Error: fgets returned nullptr for delay!!\n");
          }
        }
        else
        {
          if(iter == 0)
            printf("\nEnter zero iteration hence skiping sending rtt request");
        }
        for (i = 0; i < iter; i++)
        {
          printf("\niteration number :%u ", i+1);
          memset(&g_scan_cmd, 0, sizeof(g_scan_cmd));
          g_scan_cmd.band = QSH_WIFI_BAND_MASK_2P4_GHZ |
                               QSH_WIFI_BAND_MASK_5_GHZ;
          g_scan_cmd.scan_type = QSH_WIFI_SCAN_TYPE_ACTIVE;
          g_scan_cmd.has_radio_chain_pref = true;
          g_scan_cmd.radio_chain_pref = QSH_WIFI_PREF_RADIO_CHAIN_HIGH_ACCURACY;
          g_scan_cmd.has_max_scan_age_ms = true;
          g_scan_cmd.max_scan_age_ms = 4500;
          printf ("\nSend On Demand Scan Request");
          dump_scan_req_params(&g_scan_cmd);
          ret = qsh_wifi_send_on_demand_scan (qmi_sensors_handle,
                   &wifi_suid, &g_scan_cmd);
          is_scan_requested = false;
          usleep(delay);
        }
      }
      break;
      case ON_DEMAND_SCAN_USER_PARAMS:
      {
        memset(&g_scan_cmd, 0, sizeof(g_scan_cmd));
        ret = parse_scan_xml(SCAN_FILE_PATH, &g_scan_cmd);
        if (ret == 0)
        {
          printf ("\nSend On Demand Scan from %s", SCAN_FILE_PATH);
          dump_scan_req_params(&g_scan_cmd);
          ret = qsh_wifi_send_on_demand_scan (qmi_sensors_handle,
                    &wifi_suid, &g_scan_cmd);
        }
        is_scan_requested = false;
      }
      break;
      case RTT_SCAN:
      {
        memset(&g_rtt_cmd, 0, sizeof(g_rtt_cmd));
        ret = parse_rtt_xml(RTT_FILE_PATH, &g_rtt_cmd);
        if (ret != 0)
        {
          printf ("\nNot able to parse rtt xml file ret %d", ret);
          break;
        }
        char buf[8], *p;
        long iter = 1;
        long delay = 1000*1000;// one sec
        uint32_t i = 0;
        printf("\nStart ranging");
        printf("\nEnter number of iteration: (Default is 1 iteration): ");
        p = fgets(buf, 8, stdin);
        if (NULL == p)
        {
          printf("Error: fgets returned nullptr for iterations!!\n");
          break;
        }
        iter = strtol(buf, NULL, 10);
        if (iter && iter != 1)
        {
          printf("\nDelay between iteration in msec: (Default is 1000 msec): ");
          p = fgets(buf, 8, stdin);
          if (p)
          {
            delay = strtol(buf, NULL, 10);
           /* convert in msec  */
           delay = ((delay == 0) ? 1000*1000 : delay * 1000);
          }
          else
          {
            printf("Error: fgets returned nullptr for delay!!\n");
          }
        }
        else
        {
          if(iter == 0)
            printf("\nEnter zero iteration hence skiping sending rtt request");
        }
        dump_rtt_req_params(&g_rtt_cmd);
        for (i = 0; i < iter; i++)
        {
          printf("\niteration number :%u ", i+1);
          ret = qsh_wifi_send_rtt_req (qmi_sensors_handle,
                    &wifi_suid, &g_rtt_cmd);
          is_rtt_requested = false;
          usleep(delay);
        }
      }
      break;
      case DUMP_EVENT:
      {
        printf ("\nSend Dump event request");
        ret = qsh_wifi_send_dump_event(qmi_sensors_handle, &wifi_suid);
      }
      break;
      case EXIT:
      {
        printf ("\n\nterminate the app");
        ret = 0;
        loop = 0;
      }
      break;
    }
  }
  ret = qsh_wifi_deinit();
  return ret;
}
