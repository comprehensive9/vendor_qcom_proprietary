/*=============================================================================
  @file qsh_location_test.c

  Location test application.  This application will open a QMI connection to
  the Sensors Service (which resides on the SSC).  It will send a SUID Lookup
  Request for the data type "location".

  Copyright (c) 2016-2018, 2020 Qualcomm Technologies, Inc.
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
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "qmi_client.h"
#include "pb_decode.h"
#include "pb_encode.h"

#include "sns_client_api_v01.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "qsh_location.pb.h"
#include "sns_client.pb.h"
#include "sns_suid.pb.h"

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

#define OPEN_REQUEST            0
#define LOCATION_START_REQUEST  1
#define LOCATION_STOP_REQUEST   2
#define DATA_START_REQUEST      3
#define DATA_STOP_REQUEST       4
#define QUIT_REQUEST            5
#define BUFFERSIZE 50

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

typedef enum
{
    LOCATION_STATE_IDLE = 0,
    LOCATION_STATE_CMD_SENT = 1
} location_state;

location_state locState;

// Timer related
pid_t pid;
int signum;
struct timespec timeout;
sigset_t newmask;
siginfo_t info;

void * g_event_msg;
size_t g_event_msg_len;

bool bVerboseMode = true;

#define GNSS_MAX_MEASUREMENT  128
typedef struct {
    uint32_t number_of_measurements;
    qsh_location_clock clock;
    qsh_location_measurement *measurements;
} qsh_location_meas_and_clk;

qsh_location_meas_and_clk* p_meas_clk = NULL;

/*=============================================================================
  Static Variable Definitions
  ===========================================================================*/

/* QMI Client handle created within create_client_conn(), and used to send
 * a request to the Sensor service */
static qmi_client_type qmi_sensors_handle = NULL;

/* The SUID of the SUID Lookup Sensor is a constant value, and published within
 * sns_suid.proto. */
static sns_std_suid suid_lookup = sns_suid_sensor_init_default;

/* SUID for the location Sensor, as received from the SUID Lookup Sensor */
static sns_std_suid location_suid = (sns_std_suid){ .suid_low = 0, .suid_high = 0 };

static bool isInitialized = false;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

int wait_for_ack(uint8_t request)
{
    int ret = -1;
    sigset_t oldmask;

    pthread_sigmask(SIG_BLOCK, &newmask, &oldmask);
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
        "Wait for ack request=%d pid=%d", request, pid);
    while (true) {
        ret = sigtimedwait(&newmask, &info, &timeout);
        if (-1 == ret) {
            __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
                "Timeout waiting for ack errno=%d", errno);
            if (EAGAIN == errno) {
                break;
            } else if (EINTR == errno) {
                continue;
            }
            __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
                "Timeout waiting for ack errno=%d", errno);
            break;
        }
        if (signum == info.si_signo) {
            __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
                "Got signal from ACK");
            break;
        }
    }
    pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
    return ret;
}

/**
 * Copy an already encoded payload into a message field.  For the purpose of
 * this function, an encoded submessage is identical to a character string.
 */
static bool
encode_payload(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
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
static bool
decode_payload(pb_istream_t *stream, const pb_field_t *field, void **arg)
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
static void
resp_msg_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *resp_c_struct, unsigned int resp_c_struct_len,
    void *resp_cb_data, qmi_client_error_type transp_err)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(resp_cb_data);

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Received response; error %i, msg_id %i, size %i",
      transp_err, msg_id, resp_c_struct_len);

  if(QMI_NO_ERR == transp_err) {
    sns_client_resp_msg_v01 *resp = (sns_client_resp_msg_v01*)resp_c_struct;

    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
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
static bool
send_qmi_req(qmi_client_type qmi_handle, sns_client_req_msg_v01 const *req_msg)
{
  bool rv = false;
  size_t resp_len = sizeof(sns_client_resp_msg_v01);
  void *resp = malloc(resp_len);

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Sending request payload len %i", req_msg->payload_len);

  if(NULL == resp) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Unable to allocate sns_client_resp_msg_v01");
  } else {
    qmi_txn_handle txn_handle;

    qmi_client_error_type qmi_err = qmi_client_send_msg_async(
        qmi_handle, SNS_CLIENT_REQ_V01, (void*)req_msg, sizeof(*req_msg),
        resp, resp_len, resp_msg_cb, resp, &txn_handle);

    if(QMI_NO_ERR != qmi_err) {
      __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
        "qmi_client_send_msg_async error %i", qmi_err);
      free(resp);
    } else {
      rv = true;
    }
  }

  return rv;
}

/**
 * Send a request to the specified service.  Do not specify any batching
 * options.
 *
 * @param[i] payload Encoded Sensor-specific request message
 * @param[i] suid Destination SUID
 * @param[i] msg_id Sensor-specific message ID
 */
static bool
send_basic_req(qmi_client_type qmi_handle, sns_buffer_arg const *payload, sns_std_suid suid, uint32_t msg_id)
{
  sns_client_request_msg pb_req_msg = sns_client_request_msg_init_default;
  sns_client_req_msg_v01 req_msg =
      {.payload_len = 0, .use_jumbo_report_valid = 0, .use_jumbo_report = 0};
  pb_ostream_t stream = pb_ostream_from_buffer(req_msg.payload, SNS_CLIENT_REQ_LEN_MAX_V01);

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Sending basic request msg_id = %u", msg_id);

  pb_req_msg.suid = suid;
  pb_req_msg.msg_id = msg_id;
  pb_req_msg.request.has_batching = false;
  if (NULL != payload) {
      pb_req_msg.request.payload.funcs.encode = &encode_payload;
      pb_req_msg.request.payload.arg = (void*)payload;
  }

  if(!pb_encode(&stream, sns_client_request_msg_fields, &pb_req_msg)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Error Encoding request: %s", PB_GET_ERROR(&stream));
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

  if(!pb_get_encoded_size(&encoded_req_size, sns_std_attr_req_fields, &attr_req)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_std_attr_req_fields, &attr_req)) {
       __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
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

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Sending attr request");

  encoded_req_len = get_encoded_attr_req(&encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid, SNS_STD_MSGID_SNS_STD_ATTR_REQ);
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

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Get encoded SUID request");

  suid_req.data_type.funcs.encode = &encode_payload;
  suid_req.data_type.arg = &((sns_buffer_arg)
    { .buf = data_type, .buf_len = strlen(data_type) });
  suid_req.has_register_updates = true;
  suid_req.register_updates = true;

//  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
//      "PB Get Encoded Size");

  if(!pb_get_encoded_size(&encoded_req_size, sns_suid_req_fields, &suid_req)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
//    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
//        "PB Ostream From Buffer encoded_req_size=%zu encoded_buffer=%p",
//        encoded_req_size, encoded_buffer);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_suid_req_fields, &suid_req)) {
      __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
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
static bool
send_suid_req(qmi_client_type qmi_handle, char const *data_type)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Send encoded SUID Request");

  encoded_req_len = get_encoded_suid_req(data_type, &encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, suid_lookup, SNS_SUID_MSGID_SNS_SUID_REQ);
    free(encoded_req);
  }
  return rv;
}

/**
 * Create an encoded location open request message.
 *
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t
get_encoded_open_req(void **encoded_req)
{
  size_t encoded_req_size;
  qsh_location_open open_req;
  open_req.version = 0x01000000;

  *encoded_req = NULL;

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Get encoded enable request version=0x%" PRIx32 "", open_req.version);

  if(!pb_get_encoded_size(&encoded_req_size, qsh_location_open_fields, &open_req)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST", "pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, qsh_location_open_fields, &open_req)) {
      __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
        "Error Encoding open request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

static size_t
get_encoded_update_req(void **encoded_req, bool start,
    qsh_location_request request, uint32_t interval)
{
    size_t encoded_req_size;
    qsh_location_update update_req;
    update_req.start = start;
    update_req.location_request = request;
    update_req.interval = interval;

    *encoded_req = NULL;

//    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
//        "Get encoded enable request version=0x%" PRIx32 "", open_req.version);

    if(!pb_get_encoded_size(&encoded_req_size, qsh_location_update_fields, &update_req)) {
        __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST", "pb_get_encoded_size error");
    } else {
        void *encoded_buffer = malloc(encoded_req_size);
        pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

        if(!pb_encode(&stream, qsh_location_update_fields, &update_req)) {
            __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
                "Error Encoding open request: %s", PB_GET_ERROR(&stream));
        } else {
            *encoded_req = encoded_buffer;
        }
    }

    return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
* Send an location open request for the specified SUID.
*/
static bool
send_close_req(qmi_client_type qmi_handle, sns_std_suid *suid)
{
    bool rv = false;

    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
        "Send close request");

    rv = send_basic_req(qmi_handle, NULL, *suid, QSH_LOCATION_MSGID_QSH_LOCATION_CLOSE);

    return rv;
}

/**
 * Send an location open request for the specified SUID.
 */
static bool
send_open_req(qmi_client_type qmi_handle, sns_std_suid *suid)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Send open request");

  encoded_req_len = get_encoded_open_req(&encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid, QSH_LOCATION_MSGID_QSH_LOCATION_OPEN);
    free(encoded_req);
  }
  locState = LOCATION_STATE_CMD_SENT;
  return rv;
}

static bool
send_update_req(qmi_client_type qmi_handle, sns_std_suid *suid, bool start,
    qsh_location_request request, uint32_t interval)
{
    bool rv = false;
    void *encoded_req = NULL;
    size_t encoded_req_len;

    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
        "Send update request interval = %u", interval);

    encoded_req_len = get_encoded_update_req(&encoded_req, start, request, interval);
    if(NULL != encoded_req) {
        sns_buffer_arg payload = (sns_buffer_arg)
        { .buf = encoded_req, .buf_len = encoded_req_len };
        rv = send_basic_req(qmi_handle, &payload, *suid, QSH_LOCATION_MSGID_QSH_LOCATION_UPDATE);
        free(encoded_req);
    }
    locState = LOCATION_STATE_CMD_SENT;
    return rv;
}

/**
 * QMI Error callback.  This would be our notification if the QMI connection
 * was lost.  A typical implementation would re-establish the connection, and
 * resend any active requests.
 *
 * See qmi_client_error_cb. */
static void
error_cb(qmi_client_type user_handle,
    qmi_client_error_type error, void *err_cb_data)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(err_cb_data);

  __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "QMI error callback %i", error);
}

/**
 * Decode an attribute value.
 *
 * Once we find the attribute value we have been looking for (for example,
 * to differentiate two sensors with data type "location"), we send an enable
 * request.
 */
static bool
decode_attr_value(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  sns_buffer_arg str_data = (sns_buffer_arg){ .buf = NULL, .buf_len = 0 };

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Decode attr value");

  value.str.funcs.decode = &decode_payload;
  value.str.arg = &str_data;

  value.subtype.values.funcs.decode = &decode_attr_value;

  if(!pb_decode(stream, sns_std_attr_value_data_fields, &value)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Error decoding attribute: %s", PB_GET_ERROR(stream));
    return false;
  }

  if(value.has_flt)
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Attribute float: %f", value.flt);
  else if(value.has_sint)
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Attribute int: %"PRIi64, value.sint);
  else if(value.has_boolean)
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Attribute boolean: %i", value.boolean);
  else if(NULL != str_data.buf)
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Attribute string: %s", (char*)str_data.buf);
  else if(value.has_subtype)
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST", "Attribute nested");
  else
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST", "Unknown attribute type");

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

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Decode attr");

  if(!pb_decode(stream, sns_std_attr_fields, &attribute)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Error decoding attribute: %s", PB_GET_ERROR(stream));
    return false;
  }

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Attribute ID %i", attribute.attr_id);

  if(SNS_STD_SENSOR_ATTRID_NAME == attribute.attr_id) {
    // Based on some logic, choose this location Sensor and send open request
    send_open_req(qmi_sensors_handle, &location_suid);
  }

  return true;
}

/**
 * Decode the payload of the event message, i.e. the Attribute Event
 */
static bool
decode_attr_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_attr_event attr_event = sns_std_attr_event_init_default;

  attr_event.attributes.funcs.decode = &decode_attr;
  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Received Attribute Event length %zu", stream->bytes_left);

  if(!pb_decode(stream, sns_std_attr_event_fields, &attr_event)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Error decoding Attr Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  return true;
}

static bool
decode_qsh_location_ack(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    UNUSED_VAR(field);
    UNUSED_VAR(arg);

    union sigval sig_value;

    qsh_location_ack location_ack = { 0 };

    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "Received location ack pid = %d threadid = %ld", pid, pthread_self());

    locState = LOCATION_STATE_IDLE;
    sig_value.sival_int = 1;
    sigqueue(pid, signum, sig_value);
    if(!pb_decode(stream, qsh_location_ack_fields, &location_ack)) {
        __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
            "Error decoding location ack: %s", PB_GET_ERROR(stream));
        return false;
    }
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "error=%d command=%d", location_ack.error, location_ack.command);
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "has_version=%d version=0x%" PRIx32"", location_ack.has_version, location_ack.version);
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "has_capabilities=%d capabilities=%u", location_ack.has_capabilities, location_ack.capabilities);

    if (true == location_ack.error) {
        switch (location_ack.command) {
        case QSH_LOCATION_MSGID_QSH_LOCATION_OPEN:
            __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
                "Received ack for OPEN request");
            break;
        case QSH_LOCATION_MSGID_QSH_LOCATION_UPDATE:
            __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
                "Received ack for UPDATE request");
            break;
        case QSH_LOCATION_MSGID_QSH_LOCATION_CLOSE:
            break;
        default:
            __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
                "Received ack for unknown request");
            break;
        }
    }
    return true;
}

static bool
decode_qsh_location_position_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    UNUSED_VAR(field);
    UNUSED_VAR(arg);

    qsh_location_position_event position_event = { 0 };

    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "Received position event");

    if(!pb_decode(stream, qsh_location_position_event_fields, &position_event)) {
        __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
            "Error decoding position event: %s", PB_GET_ERROR(stream));
        return false;
    }
    if (bVerboseMode) {
        printf("Received position event\n");
        printf("clock: t=0x%"PRIx64" lat=%d lon=%d alt=%.2f hacc=%.2f vacc=%.2f sp=%.2f sacc=%.2f b=%.2f bacc=%.2f ci=%.2f\n",
                position_event.timestamp, position_event.latitude, position_event.longitude,
                position_event.altitude, position_event.horizontal_accuracy, position_event.vertical_accuracy,
                position_event.speed, position_event.speed_accuracy, position_event.bearing,
                position_event.bearing_accuracy, position_event.conformity_index);
    }

    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "timestamp=%"PRIu64" latitude=%d", position_event.timestamp, position_event.latitude);
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "longitude=%d altitude=%f", position_event.longitude, position_event.altitude);
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "horizontal_accuracy=%f vertical_accuracy=%f", position_event.horizontal_accuracy, position_event.vertical_accuracy);
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "speed=%f speed_accuracy=%f", position_event.speed, position_event.speed_accuracy);
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "bearing=%f bearing_accuracy=%f", position_event.bearing, position_event.bearing_accuracy);
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "conformity_index=%f", position_event.conformity_index);
    return true;
}

typedef struct
{
    uint32_t number_of_measurements;
    qsh_location_measurement *measurements;
} sns_meas_and_clk_queued_transaction;

typedef struct meas_pb_dec_cb_args
{
    sns_meas_and_clk_queued_transaction extracted_meas;
} meas_pb_dec_cb_args;

typedef void (*sns_process_measurements)(
    qsh_location_measurement       *measurement,
    void                  *user_args);

typedef struct sns_meas_dec_cb_arg
{
    qsh_location_measurement measurement;
    sns_process_measurements callback;
    void *user_args;
} sns_meas_dec_cb_arg;

static void
extract_measurements(qsh_location_measurement       *measurement,
    sns_meas_and_clk_queued_transaction                  *user_args);

bool
sns_decode_each_measurement(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    UNUSED_VAR(field);

    meas_pb_dec_cb_args *args = (meas_pb_dec_cb_args *)*arg;
    qsh_location_measurement meas = { 0 };

//    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
//        "sns_decode_each_measurement calling pb_decode");

    if(!pb_decode(stream,qsh_location_measurement_fields,&meas))
        return false;

//    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
//        "sns_decode_each_measurement calling extract_measurements");
    extract_measurements(&meas, &args->extracted_meas);

    return true;
}

bool sns_meas_for_each_vector_decode(pb_istream_t *stream, sns_process_measurements callback, void *user_args)
{
    qsh_location_meas_and_clk_event msclk = { 0 };

    sns_meas_dec_cb_arg arg = (sns_meas_dec_cb_arg)
    {
        .measurement = { 0 },
        .callback = callback,
        .user_args = user_args,
    };
    msclk.measurements.funcs.decode = &sns_decode_each_measurement;
    msclk.measurements.arg = &arg;

    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "sns_meas_for_each_vector_decode calling pb_decode");
    if(!pb_decode(stream, qsh_location_meas_and_clk_event_fields, &msclk))
        return false;

    return true;
}

static void
extract_measurements(qsh_location_measurement       *measurement,
    sns_meas_and_clk_queued_transaction                  *user_args)
{
    sns_meas_and_clk_queued_transaction *extracted_meas =
        (sns_meas_and_clk_queued_transaction*)user_args;
    uint32_t curr_index = extracted_meas->number_of_measurements;

//    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
//        "extract_measurements extracted_meas->number_of_measurements=%d", extracted_meas->number_of_measurements);
    extracted_meas->measurements[curr_index].sv_id = measurement->sv_id;
    extracted_meas->measurements[curr_index].constellation_type = measurement->constellation_type;
    extracted_meas->measurements[curr_index].time_offset = measurement->time_offset;
    extracted_meas->measurements[curr_index].state = measurement->state;
    extracted_meas->measurements[curr_index].received_sv_time = measurement->received_sv_time;
    extracted_meas->measurements[curr_index].received_sv_time_uncertainty = measurement->received_sv_time_uncertainty;
    extracted_meas->measurements[curr_index].c_n0 = measurement->c_n0;
    extracted_meas->measurements[curr_index].pseudorange_rate = measurement->pseudorange_rate;
    extracted_meas->measurements[curr_index].pseudorange_rate_uncertainty = measurement->pseudorange_rate_uncertainty;
    extracted_meas->measurements[curr_index].has_carrier_phase = measurement->has_carrier_phase;
    extracted_meas->measurements[curr_index].carrier_phase = measurement->carrier_phase;
    extracted_meas->measurements[curr_index].has_carrier_phase_uncertainty = measurement->has_carrier_phase_uncertainty;
    extracted_meas->measurements[curr_index].carrier_phase_uncertainty = measurement->carrier_phase_uncertainty;
    extracted_meas->measurements[curr_index].has_cycle_slip_count = measurement->has_cycle_slip_count;
    extracted_meas->measurements[curr_index].cycle_slip_count = measurement->cycle_slip_count;
    extracted_meas->measurements[curr_index].has_multipath_indicator = measurement->has_multipath_indicator;
    extracted_meas->measurements[curr_index].multipath_indicator = measurement->multipath_indicator;
    extracted_meas->measurements[curr_index].has_snr = measurement->has_snr;
    extracted_meas->measurements[curr_index].snr = measurement->snr;
    extracted_meas->measurements[curr_index].has_carrier_frequency = measurement->has_carrier_frequency;
    extracted_meas->measurements[curr_index].carrier_frequency = measurement->carrier_frequency;
    extracted_meas->number_of_measurements += 1;
}

static void consume_meas_and_clk()
{
    uint32_t i;

    if (bVerboseMode) {
        printf("clock: t=0x%"PRIx64" tu=%.2f fb=%"PRId64" b=%.2f bu=%.2f d=%.2f du=%.2f hd=%d\n",
            p_meas_clk->clock.time, p_meas_clk->clock.time_uncertainty,
            p_meas_clk->clock.full_bias, p_meas_clk->clock.bias,
            p_meas_clk->clock.bias_uncertainty, p_meas_clk->clock.drift,
            p_meas_clk->clock.drift_uncertainty, p_meas_clk->clock.hw_clock_discontinuity_count);

        for (i = 0; i < p_meas_clk->number_of_measurements; i++) {
            printf("meas[%d] sv=%d cs=%d to=%"PRId64" st=0x%X rt=%"PRId64" ru=%"PRId64" cn=%.2f pr=%.2f pu=%.2f cf=%.2f cu=%.2f cs=%d mi=%d sn=%.2f fr=%.2f\n",
                i, p_meas_clk->measurements[i].sv_id, p_meas_clk->measurements[i].constellation_type,
                p_meas_clk->measurements[i].time_offset, p_meas_clk->measurements[i].state,
                p_meas_clk->measurements[i].received_sv_time, p_meas_clk->measurements[i].received_sv_time_uncertainty,
                p_meas_clk->measurements[i].c_n0, p_meas_clk->measurements[i].pseudorange_rate,
                p_meas_clk->measurements[i].pseudorange_rate_uncertainty, p_meas_clk->measurements[i].carrier_phase,
                p_meas_clk->measurements[i].carrier_phase_uncertainty, p_meas_clk->measurements[i].cycle_slip_count,
                p_meas_clk->measurements[i].multipath_indicator, p_meas_clk->measurements[i].snr,
                p_meas_clk->measurements[i].carrier_frequency);
        }
    }
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "clock: t=0x%"PRIx64" tu=%.2f fb=%"PRId64" b=%.2f bu=%.2f d=%.2f du=%.2f hd=%d",
        p_meas_clk->clock.time, p_meas_clk->clock.time_uncertainty,
        p_meas_clk->clock.full_bias, p_meas_clk->clock.bias,
        p_meas_clk->clock.bias_uncertainty, p_meas_clk->clock.drift,
        p_meas_clk->clock.drift_uncertainty, p_meas_clk->clock.hw_clock_discontinuity_count);

    for (i = 0; i < p_meas_clk->number_of_measurements; i++) {
        __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
            "meas[%d] sv=%d cs=%d to=%"PRId64" st=0x%X rt=%"PRId64" ru=%"PRId64" cn=%.2f pr=%.2f pu=%.2f cf=%.2f cu=%.2f cs=%d mi=%d sn=%.2f fr=%.2f",
            i, p_meas_clk->measurements[i].sv_id, p_meas_clk->measurements[i].constellation_type,
            p_meas_clk->measurements[i].time_offset, p_meas_clk->measurements[i].state,
            p_meas_clk->measurements[i].received_sv_time, p_meas_clk->measurements[i].received_sv_time_uncertainty,
            p_meas_clk->measurements[i].c_n0, p_meas_clk->measurements[i].pseudorange_rate,
            p_meas_clk->measurements[i].pseudorange_rate_uncertainty, p_meas_clk->measurements[i].carrier_phase,
            p_meas_clk->measurements[i].carrier_phase_uncertainty, p_meas_clk->measurements[i].cycle_slip_count,
            p_meas_clk->measurements[i].multipath_indicator, p_meas_clk->measurements[i].snr,
            p_meas_clk->measurements[i].carrier_frequency);
    }
    free(p_meas_clk->measurements);
    p_meas_clk->measurements = NULL;
    free(p_meas_clk);
    p_meas_clk = NULL;
}

static bool
decode_qsh_location_meas_and_clk_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    UNUSED_VAR(field);
    UNUSED_VAR(arg);

    qsh_location_meas_and_clk_event mclk_event = { 0 };

    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "Received measurement and clock event");

    qsh_location_measurement *measurements;
    measurements = (qsh_location_measurement*)malloc(sizeof(qsh_location_measurement)*mclk_event.number_of_measurements);

    meas_pb_dec_cb_args args = (meas_pb_dec_cb_args)
    {
        .extracted_meas.measurements = measurements,
        .extracted_meas.number_of_measurements = 0
    };
    mclk_event.measurements.funcs.decode = &sns_decode_each_measurement;
    mclk_event.measurements.arg = &args;
    if(!pb_decode(stream, qsh_location_meas_and_clk_event_fields, &mclk_event)) {
        __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
            "Error decoding meas_and_clk event: %s", PB_GET_ERROR(stream));
        return false;
    }
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "number_of_measurements=%d measurement_index=%d # decoded meas=%d",
        mclk_event.number_of_measurements, mclk_event.measurement_index,
        args.extracted_meas.number_of_measurements);

    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "clock.time=0x%"PRIx64"", mclk_event.clock.time);
    if (bVerboseMode) {
        printf("Received meas_and_clk event # of measurements=%d measurement_index=%d\n",
            mclk_event.number_of_measurements, mclk_event.measurement_index);
    }
    /* now copy to the big structure, initialize if first segment, ship it if last */

    if (1 == mclk_event.measurement_index) {
        if (NULL != p_meas_clk) {
            if (NULL != p_meas_clk->measurements) {
                free(p_meas_clk->measurements);
                p_meas_clk->measurements = NULL;
            }
            free(p_meas_clk);
            p_meas_clk = NULL;
        }
        p_meas_clk = (qsh_location_meas_and_clk*)malloc(sizeof(qsh_location_meas_and_clk));
        memset(p_meas_clk, 0, sizeof(qsh_location_meas_and_clk));
        p_meas_clk->number_of_measurements = mclk_event.number_of_measurements;
        p_meas_clk->measurements = (qsh_location_measurement*)malloc(p_meas_clk->number_of_measurements
                * sizeof(qsh_location_measurement));
        memcpy(&p_meas_clk->clock, &mclk_event.clock, sizeof(qsh_location_clock));
    }
    memcpy(p_meas_clk->measurements + mclk_event.measurement_index - 1,
        measurements, args.extracted_meas.number_of_measurements*sizeof(qsh_location_measurement));

    if ((mclk_event.measurement_index + args.extracted_meas.number_of_measurements - 1) ==
        mclk_event.number_of_measurements) {
        consume_meas_and_clk();
    }

    free(measurements);

    return true;
}

static bool
decode_sns_std_error_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    UNUSED_VAR(field);
    UNUSED_VAR(arg);
    sns_std_error_event error_event = sns_std_error_event_init_default;

    if(!pb_decode(stream, sns_std_error_event_fields, &error_event)) {
        __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
            "Error decoding error event: %s", PB_GET_ERROR(stream));
        return false;
    }
    __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
        "error=%d", error_event.error);

    return true;
}

/**
 * Decode an event message received from the location Sensor.
 */
static bool
decode_location_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  uint32_t *msg_id = (uint32_t*)*arg;

  __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
      "Begin decoding location event");

  if (SNS_STD_MSGID_SNS_STD_ATTR_EVENT == *msg_id) {
      __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
          "Received SNS_STD_MSGID_SNS_STD_ATTR_EVENT");
      return decode_attr_event(stream, field, arg);
  } else if(QSH_LOCATION_MSGID_QSH_LOCATION_ACK == *msg_id) {
      __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
          "Received QSH_LOCATION_MSGID_QSH_LOCATION_ACK");
      return decode_qsh_location_ack(stream, field, arg);
  } else if(QSH_LOCATION_MSGID_QSH_LOCATION_POSITION_EVENT == *msg_id) {
      __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
          "Received QSH_LOCATION_MSGID_QSH_LOCATION_POSITION_EVENT");
      return decode_qsh_location_position_event(stream, field, arg);
  } else if(QSH_LOCATION_MSGID_QSH_LOCATION_MEAS_AND_CLK_EVENT == *msg_id) {
      __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
          "Received QSH_LOCATION_MSGID_QSH_LOCATION_MEAS_AND_CLK_EVENT");
      return decode_qsh_location_meas_and_clk_event(stream, field, arg);
  } else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == *msg_id) {
      __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
          "Received Error event");
      return decode_sns_std_error_event(stream, field, arg);
  } else {
      __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
          "Received unknown event %i", *msg_id);
      return false;
  }
  __android_log_print(ANDROID_LOG_VERBOSE, "QSH_LOCATION_TEST",
      "Done decoding location event");

  return true;
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
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Error decoding SUID: %s", PB_GET_ERROR(stream));
    return false;
  }

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
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
static bool
decode_suid_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_suid_event suid_event;
  sns_buffer_arg data;

  suid_event.suid.funcs.decode = &decode_suid;
  suid_event.data_type.funcs.decode = &decode_payload;
  suid_event.data_type.arg = &data;

  if(!pb_decode(stream, sns_suid_event_fields, &suid_event)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Error decoding SUID Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  {
    char data_type[data.buf_len + 2];
    strlcpy(data_type, (char*)data.buf, data.buf_len+1);

    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
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
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "event: %s", PB_GET_ERROR(stream));
  } else {
    __android_log_print(ANDROID_LOG_INFO, "QSH_LOCATION_TEST",
      "Decoding event with message ID %i, timestamp %"PRIu64,
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

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Begin decoding event");
  uint32_t msg_id = get_msg_id(&stream_cpy);

  event.payload.funcs.decode = *arg;
  event.payload.arg = &msg_id;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Error decoding Event: %s", PB_GET_ERROR(stream));
    rv = false;
  }

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Event decoding complete");
  return rv;
}

/**
 * Handle a received event message from the Sensor Service.
 *
 * This example code only ever sends requests to the SUID Lookup Sensor
 * and an location Sensor.  Therefore we only need to check against those two
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
static void
handle_event_msg(sns_std_suid const *suid, void const *event_msg, size_t event_msg_len)
{
  pb_istream_t stream;
  sns_client_event_msg event = sns_client_event_msg_init_default;
  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Processing events from SUID %"PRIx64" %"PRIx64,
      suid->suid_low, suid->suid_high);

  if(suid->suid_high == suid_lookup.suid_high && suid->suid_low == suid_lookup.suid_low) {
    event.events.funcs.decode = &decode_events;
    event.events.arg = &decode_suid_event;
  } else {
    location_suid = *suid;

    event.events.funcs.decode = &decode_events;
    event.events.arg = &decode_location_event;
  }

  g_event_msg = (void*)event_msg;
  g_event_msg_len = event_msg_len;
  stream = pb_istream_from_buffer(event_msg, event_msg_len);
  if(!pb_decode(&stream, sns_client_event_msg_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Error decoding event list: %s", PB_GET_ERROR(&stream));
  }
}
/* QMI indication callback.  See qmi_client_ind_cb. */
static void
ind_msg_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(ind_cb_data);
  size_t ind_len = sizeof(sns_client_report_ind_msg_v01);
  sns_client_report_ind_msg_v01 *ind = malloc(ind_len);
  int32_t qmi_err;

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Received Indication; len %i", ind_buf_len);

  // Extract the Protocol Buffer encoded message from the outer QMI/IDL  message
  qmi_err = qmi_idl_message_decode(SNS_CLIENT_SVC_get_service_object_v01(),
      QMI_IDL_INDICATION, msg_id, ind_buf, ind_buf_len, ind, ind_len);
  if(QMI_IDL_LIB_NO_ERR != qmi_err) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "QMI decode error %i", qmi_err);
  } else {
    sns_client_event_msg event = sns_client_event_msg_init_default;
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Indication from client ID %"PRIu64, ind->client_id);

    // Decode just the sns_client_event_msg in order to get the SUID
    pb_istream_t stream = pb_istream_from_buffer(ind->payload, ind->payload_len);
    if(pb_decode(&stream, sns_client_event_msg_fields, &event)) {
      handle_event_msg(&event.suid, ind->payload, ind->payload_len);
    } else {
      __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
        "Error decoding Event Message: %s", PB_GET_ERROR(&stream));
    }
  }

  free(ind);
  __android_log_print(ANDROID_LOG_INFO, "QSH_LOCATION_TEST",
      "Indication processing completed");
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
create_client_conn(qmi_client_type *qmi_handle, uint32_t timeout)
{
  bool rv = false;
  qmi_idl_service_object_type service_obj = SNS_CLIENT_SVC_get_service_object_v01();
  qmi_service_instance instance_id = 0;
  qmi_client_error_type qmi_err;
  qmi_cci_os_signal_type os_params;

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Creating client connection");

  qmi_err = qmi_client_init_instance(service_obj,
      instance_id, ind_msg_cb, NULL, &os_params, timeout, qmi_handle);

  if(QMI_NO_ERR != qmi_err) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "qmi_client_init_instance error %i", qmi_err);
  } else {
    qmi_err = qmi_client_register_error_cb(*qmi_handle, error_cb, NULL);

    if(QMI_NO_ERR != qmi_err) {
      __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
        "qmi_client_register_error_cb error %i", qmi_err);
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

/**
 * Check whether the Sensor service is available.
 *
 * @param[i] timeout Timeout period in ms; 0 infinite
 *
 * @return true if the service is available, false upon timeout
 */
static bool
wait_for_service(uint32_t timeout)
{
  qmi_idl_service_object_type service_obj = SNS_CLIENT_SVC_get_service_object_v01();
  qmi_client_type notifier_handle;
  qmi_client_error_type qmi_err;
  qmi_cci_os_signal_type os_params;
  bool rv = true;

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Waiting for service");

  qmi_err = qmi_client_notifier_init(service_obj, &os_params, &notifier_handle);
  if(QMI_NO_ERR != qmi_err) {
    __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "qmi_client_notifier_init error %i", qmi_err);
    rv = false;
  } else {
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, timeout);
    if(os_params.timed_out) {
      __android_log_print(ANDROID_LOG_ERROR, "QSH_LOCATION_TEST",
      "Service is not available after %i timeout", timeout);
      rv = false;
    }
  }

  qmi_client_release(notifier_handle);
  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Waiting complete");
  return rv;
}

int init()
{
    int ret = -1;

    if (!create_client_conn(&qmi_sensors_handle, 200)) {
        goto bail;
    }

    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
        "Send SUID Request");

    if (!send_suid_req(qmi_sensors_handle, "location")) {
        goto bail;
    }
//    ret = wait_for_ack(OPEN_REQUEST);
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
        "Wait for ack request=%d", OPEN_REQUEST);
    while (true) {
        ret = sigtimedwait(&newmask, &info, &timeout);
        if (-1 == ret) {
            //            __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
            //                "Timeout waiting for ack errno=%d", errno);
            if (EAGAIN == errno || EINTR == errno) {
                continue;
            }
            __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
                "Timeout waiting for ack errno=%d", errno);
            break;
        }
        if (signum == info.si_signo) {
            __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
                "Got signal from ACK");
            break;
        }
    }
    return 0;
bail:
    return ret;
}

int test_loc_start(uint32_t request, uint32_t rate)
{
    int ret = -1;

    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
        "test_loc_start");
    if (!isInitialized) {
        ret = init();
        isInitialized = true;
        if (-1 == ret) {
            goto bail;
        }
    }

    switch (request) {
    case LOCATION_START_REQUEST:
        __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST", "Start Location");

        ret = send_update_req(qmi_sensors_handle, &location_suid, true, qsh_location_request_QSH_LOCATION_POSITION_REQUEST, rate);
        ret = wait_for_ack(request);
        if (-1 == ret) {
            goto bail;
        }
        break;
    case LOCATION_STOP_REQUEST:
        __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
            "Stop Location");
        ret = send_update_req(qmi_sensors_handle, &location_suid, false, qsh_location_request_QSH_LOCATION_POSITION_REQUEST, rate);
        ret = wait_for_ack(request);
        if (-1 == ret) {
            goto bail;
        }
        break;
    case DATA_START_REQUEST:
        __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
            "Start meas and clk");
        ret = send_update_req(qmi_sensors_handle, &location_suid, true, qsh_location_request_QSH_LOCATION_MEAS_AND_CLK_REQUEST, rate);
        ret = wait_for_ack(request);
        if (-1 == ret) {
            goto bail;
        }
        break;
    case DATA_STOP_REQUEST:
        __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
            "Stop meas and clk");
        ret = send_update_req(qmi_sensors_handle, &location_suid, false, qsh_location_request_QSH_LOCATION_MEAS_AND_CLK_REQUEST, rate);
        ret = wait_for_ack(request);
        if (-1 == ret) {
            goto bail;
        }
        break;
    case QUIT_REQUEST:
        send_close_req(qmi_sensors_handle, &location_suid);
        locState = LOCATION_STATE_CMD_SENT;
        ret = wait_for_ack(request);
        ret = 0;
        goto bail;
        break;
    default:
        goto bail;
        break;
    }
    return 0;

bail:
    //  loc_pthread_deinit_timer();
    __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
        "Bye bye");
    if (-1 == ret) {
        send_close_req(qmi_sensors_handle, &location_suid);
    }
    return ret;
}

int
main(int argc, char *argv[])
{
  UNUSED_VAR(argc);
  UNUSED_VAR(argv);

  int ret = -1;
  char buf[16], *p;
  int exit_loop = 0;
  uint locRate, measRate;
  char str[BUFFERSIZE];
  char* pch;
  static int locInProgress = 0;
  static int measInProgress = 0;

  pid = getpid();
  signum = SIGRTMIN + 1;
  timeout.tv_sec = 10;
  timeout.tv_nsec = 0;
  sigemptyset(&newmask);
  sigaddset(&newmask, signum);
  sigprocmask(SIG_BLOCK, &newmask, NULL);
  sigfillset(&newmask);

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Begin Location Test threadid = %ld", pthread_self());

  locState = LOCATION_STATE_IDLE;

  if (!wait_for_service(200)) {
      return -1;
  }

  locState = LOCATION_STATE_CMD_SENT;

  while (0 == exit_loop) {
      if (bVerboseMode) {
          printf("\n\n"
              "1: start/stop location test\n"
              "2: start/stop measurement test\n"
              "3: turn off screen output\n"
              "q: quit\n"
              "\nEnter Command:");
      } else {
          printf("\n\n"
              "1: start/stop location test\n"
              "2: start/stop measurement test\n"
              "3: turn on screen output\n"
              "q: quit\n"
              "\nEnter Command:");
      }

      fflush(stdout);
      p = fgets(buf, 16, stdin);
      if (NULL == p) {
          printf("Error: fgets returned nullptr !!\n");
      }
      switch (p[0]) {
      case '1':
          if (locInProgress) {
              ret = test_loc_start(LOCATION_STOP_REQUEST, locRate);
              locInProgress = 0;
              if (0 == ret) {
                  printf("success\n");
              }
              else {
                  printf("failed\n");
                  exit_loop = 1;
              }
          }
          else {
              printf("Enter rate in milliseconds (>=1000): ");
              pch = fgets(str, BUFFERSIZE, stdin);
              locRate = atoi(pch);
              ret = test_loc_start(LOCATION_START_REQUEST, locRate);
              locInProgress = 1;
              if (0 == ret) {
                  printf("success\n");
              }
              else {
                  printf("failed\n");
                  exit_loop = 1;
              }
          }
          break;
      case '2':
          if (measInProgress) {
              ret = test_loc_start(DATA_STOP_REQUEST, measRate);
              measInProgress = 0;
              if (0 == ret) {
                  printf("success\n");
              }
              else {
                  printf("failed\n");
                  exit_loop = 1;
              }
          }
          else {
              printf("Enter rate in milliseconds (>=1000): ");
              pch = fgets(str, BUFFERSIZE, stdin);
              measRate = atoi(pch);
              ret = test_loc_start(DATA_START_REQUEST, measRate);
              measInProgress = 1;
              if (0 == ret) {
                  printf("success\n");
              }
              else {
                  printf("failed\n");
                  exit_loop = 1;
              }
          }
          break;
      case '3':
          bVerboseMode = !bVerboseMode;
          break;
      case 'q':
          if (locInProgress) {
              ret = test_loc_start(LOCATION_STOP_REQUEST, locRate);
          }
          if (measInProgress) {
              ret = test_loc_start(DATA_STOP_REQUEST, measRate);
          }
          test_loc_start(QUIT_REQUEST, 0);
          exit_loop = 1;
          break;
      default:
          break;
      }
  }

  __android_log_print(ANDROID_LOG_WARN, "QSH_LOCATION_TEST",
      "Sensors Location Test completed - Exiting");

  return ret;
}
