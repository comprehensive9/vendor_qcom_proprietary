/*============================================================================
  Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef SNS_SUPPORT_DIRECT_CHANNEL
#include "sns_direct_channel.h"
#include "sensor_direct_channel.h"
#include "AEEStdErr.h"
#include <string>
#include <stdlib.h>
#include "sns_std_sensor.pb.h"
#include <unistd.h>
#include <vector>
#include <cinttypes>
#include "sensor_common.h"
#include <string>
#include <stdint.h>
#include <cutils/properties.h>
#include <sys/stat.h>
#ifndef DIRECT_CHANNEL_SHARED_MEMORY_SIZE
#define DIRECT_CHANNEL_SHARED_MEMORY_SIZE (10*8000*104)
#endif

/* Taken from sensors_base.h, standard Android sensors */
#define SENSOR_TYPE_ACCELEROMETER                       (1)
#define SENSOR_TYPE_GEOMAGNETIC_FIELD                   (2)
#define SENSOR_TYPE_MAGNETIC_FIELD                      SENSOR_TYPE_GEOMAGNETIC_FIELD
#define SENSOR_TYPE_GYROSCOPE                           (4)
#define SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED         (14)
#define SENSOR_TYPE_GYROSCOPE_UNCALIBRATED              (16)
#define SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED          (35)

/* Taken from sensors_qti.h for dual sensor */
#define QTI_DUAL_SENSOR_TYPE_BASE                       (268369920)
#define QTI_SENSOR_TYPE_ACCELEROMETER                   (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_ACCELEROMETER)
#define QTI_SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED      (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED)
#define QTI_SENSOR_TYPE_GYROSCOPE                       (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_GYROSCOPE)
#define QTI_SENSOR_TYPE_GYROSCOPE_UNCALIBRATED          (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_GYROSCOPE_UNCALIBRATED)
#define QTI_SENSOR_TYPE_MAGNETIC_FIELD                  (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_MAGNETIC_FIELD)
#define QTI_SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED     (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED)

/* Definition for packed structures */
#define PACK(x)   x __attribute__((__packed__))

/* Taken from sensors.h */
/**
 * sensor event data
 */
/**
 * sensor event data
 */
typedef struct {
    union {
        float v[3];
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float azimuth;
            float pitch;
            float roll;
        };
    };
    int8_t status;
    uint8_t reserved[3];
} sensors_vec_t;

/* Taken from sensors.h */
/**
 * uncalibrated accelerometer, gyroscope and magnetometer event data
 */
typedef struct {
  union {
    float uncalib[3];
    struct {
      float x_uncalib;
      float y_uncalib;
      float z_uncalib;
    };
  };
  union {
    float bias[3];
    struct {
      float x_bias;
      float y_bias;
      float z_bias;
    };
  };
} uncalibrated_event_t;

/* Stripped down version of sensors_event_t from sensors.h */
/**
 * Union of the various types of sensor data
 * that can be returned.
 */
typedef PACK(struct) sensors_event_t {
    /* must be sizeof(struct sensors_event_t) */
    int32_t version;

    /* sensor identifier */
    int32_t sensor;

    /* sensor type */
    int32_t type;

    /* reserved */
    int32_t reserved0;

    /* time is in nanosecond */
    int64_t timestamp;

    union {
        union {
            float           data[16];

            /* acceleration values are in meter per second per second (m/s^2) */
            sensors_vec_t   acceleration;

            /* magnetic vector values are in micro-Tesla (uT) */
            sensors_vec_t   magnetic;

            /* orientation values are in degrees */
            sensors_vec_t   orientation;

            /* gyroscope values are in rad/s */
            sensors_vec_t   gyro;

            /* temperature is in degrees centigrade (Celsius) */
            float           temperature;

            /* distance in centimeters */
            float           distance;

            /* light in SI lux units */
            float           light;

            /* pressure in hectopascal (hPa) */
            float           pressure;

            /* relative humidity in percent */
            float           relative_humidity;

            /* uncalibrated gyroscope values are in rad/s */
            uncalibrated_event_t uncalibrated_gyro;

            /* uncalibrated magnetometer values are in micro-Teslas */
            uncalibrated_event_t uncalibrated_magnetic;

            /* uncalibrated accelerometer values
             * in  meter per second per second (m/s^2) */
            uncalibrated_event_t uncalibrated_accelerometer;
        };

        union {
            uint64_t        data[8];

            /* step-counter */
            uint64_t        step_counter;
        } u64;
    };

    /* Reserved flags for internal use. Set to zero. */
    uint32_t flags;

    uint32_t reserved1[3];
} sensors_event_t;

typedef struct sns_sensor_event {
  uint64_t timestamp;
  uint32_t message_id;
  uint32_t event_len;
} sns_sensor_event;

vector<int> gDirectChannelFDArray ;
vector<char*> gDirectChannelMempointer ;

/*Single variable for all the channels*/
uint64_t sensor_direct_channel::fastRPC_remote_handle = -1;

bool is_direct_channel_logging = false;
static const char SENSORS_DIRECT_CHANNEL_LOGGING[] ="persist.vendor.sensors.debug.directChannelLogging";

bool is_direct_channel_supported(){
  char stats_debug[PROPERTY_VALUE_MAX];
  property_get(SENSORS_DIRECT_CHANNEL_LOGGING, stats_debug, "false");
  USTA_LOGI("is_direct_channel_logging: %s",stats_debug);
  if (!strncmp("true", stats_debug,4)) {
      USTA_LOGI("is_direct_channel_logging : %s",stats_debug);
      return true;
  }
  return false;
}

void sensor_direct_channel::fastRPC_remote_handle_init()
{
  USTA_LOGI("get_fastRPC_remote_handle Start");
  if(fastRPC_remote_handle == -1) {
    is_direct_channel_logging  = is_direct_channel_supported();
    int nErr = AEE_SUCCESS;
    string uri = sns_direct_channel_URI;
    remote_handle64 remote_handle_fd = 0;
    remote_handle64 handle_l;

    /*check for slpi or adsp*/
    struct stat sb;
    if(!stat("/sys/kernel/boot_slpi", &sb)){
      uri +="&_dom=sdsp";
      if (remote_handle64_open(ITRANSPORT_PREFIX "createstaticpd:sensorspd&_dom=sdsp", &remote_handle_fd)) {
        USTA_LOGE("failed to open remote handle for sensorspd - sdsp");
      }
    }
    else {
      uri +="&_dom=adsp";
      if (remote_handle64_open(ITRANSPORT_PREFIX "createstaticpd:sensorspd&_dom=adsp", &remote_handle_fd)) {
        USTA_LOGE("failed to open remote handle for sensorspd - adsp");
      }
    }

    if (AEE_SUCCESS == (nErr = sns_direct_channel_open(uri.c_str(), &handle_l))) {
      USTA_LOGI("sns_direct_channel_open success for sensorspd - handle_l is %ud" , (unsigned int)handle_l);
      fastRPC_remote_handle = handle_l;
    } else {
      fastRPC_remote_handle = -1;
    }
  }
  USTA_LOGI("get_fastRPC_remote_handle End");
}

sensor_direct_channel::sensor_direct_channel(
    unsigned long shared_memory_pointer,
    unsigned long shared_memory_size,
    direct_channel_type in_channel_type,
    sns_std_client_processor in_client_type
    )
{
  USTA_LOGI("sensor_direct_channel start");
  log_instance = usta_logging_util::create_instance();
  fastRPC_remote_handle_init();
  channel_handle = -1;
  current_offset = 0;
  shared_memory_thread_handle = 0;
  channel_type = in_channel_type;
  client_type = in_client_type;

  fd_ptr = shared_memory_pointer;

  sns_direct_channel_create_msg create_msg;
  sns_direct_channel_create_msg_shared_buffer_config *shared_buffer_config = create_msg.mutable_buffer_config();
  if(NULL == shared_buffer_config)
    return;

  shared_buffer_config->set_fd(shared_memory_pointer);
  shared_buffer_config->set_size(shared_memory_size);

  create_msg.set_channel_type(channel_type);
  create_msg.set_client_proc(client_type);
  string pb_encoded_direct_channel_req_msg;
  create_msg.SerializeToString(&pb_encoded_direct_channel_req_msg);

  int ret = sns_direct_channel_create(fastRPC_remote_handle, (const unsigned char*)pb_encoded_direct_channel_req_msg.c_str(), pb_encoded_direct_channel_req_msg.size(), &channel_handle);
  if(0 == ret) {
    USTA_LOGI("sensor_direct_channel sns_direct_channel_create success , and channel_handle %d ", channel_handle);
  } else {
    USTA_LOGE("sensor_direct_channel sns_direct_channel_create failed");
  }

  if(is_direct_channel_logging == true) {
    pthread_create(&shared_memory_thread_handle, NULL, &sensor_direct_channel::shared_memory_reading_thread, this);
  }

  USTA_LOGI("sensor_direct_channel End");
}

sensor_direct_channel::~sensor_direct_channel() {
  USTA_LOGI("sensor_direct_channel ~ Start ");
  if(sns_direct_channel_delete(fastRPC_remote_handle, channel_handle) == 0) {
    USTA_LOGI("sns_direct_channel_delete Pass ");
  } else {
    USTA_LOGI("sns_direct_channel_delete failed");
  }
  fd_ptr = -1;
  USTA_LOGI("sensor_direct_channel ~ End ");
}

int sensor_direct_channel::update_offset(
    int64_t offset
    )
{
  USTA_LOGI("sensor_direct_channel update_offset Start");

  sns_direct_channel_config_msg config_msg;
  sns_direct_channel_set_ts_offset *offset_msg = config_msg.mutable_set_ts_offset();
  if(NULL == offset_msg)
    return -1;

  offset_msg->set_ts_offset(offset);

  string pb_encoded_direct_channel_config_msg;
  config_msg.SerializeToString(&pb_encoded_direct_channel_config_msg);
  int ret = sns_direct_channel_config(fastRPC_remote_handle, channel_handle, (const unsigned char*)pb_encoded_direct_channel_config_msg.c_str(), pb_encoded_direct_channel_config_msg.size());
  if(0 == ret) {
    USTA_LOGI("sensor_direct_channel update_offset success");
  } else {
    USTA_LOGE("sensor_direct_channel update_offset failed");
  }
  current_offset = offset;
  USTA_LOGI("sensor_direct_channel update_offset End");
  return ret;
}

int sensor_direct_channel::send_request(
    unsigned int msg_id,
    direct_channel_stream_info stream_info,
    sensor_direct_channel_srd_req_fileds std_req_fields_info,
    direct_channel_mux_ch_attrb attributes_info,
    bool has_attributes_info,
    string sensor_req_encoded_payload
    )
{
  sns_direct_channel_config_msg config_msg;
  sns_direct_channel_set_client_req* req_msg = config_msg.mutable_set_client_req();
  if(NULL == req_msg)
    return -1;

  req_msg->set_msg_id(msg_id);

  sns_direct_channel_stream_id* stream_id_msg = req_msg->mutable_stream_id();
  if(NULL == stream_id_msg)
    return -1;

  sns_std_suid *suid = stream_id_msg->mutable_suid();
  if(NULL == suid)
    return -1;

  suid->set_suid_low(stream_info.suid_low);
  suid->set_suid_high(stream_info.suid_high);
  if(true == stream_info.has_calibrated) {
    stream_id_msg->set_calibrated(stream_info.is_calibrated);
    USTA_LOGI("sensor_direct_channel::send_request set_calibrated %d " , (int)stream_info.is_calibrated);
  }

  if(true == stream_info.has_fixed_rate) {
    stream_id_msg->set_resampled(stream_info.is_resampled);
    USTA_LOGI("sensor_direct_channel::send_request is_resampled %d " , (int)stream_info.is_resampled);
  }

  sns_std_request* std_req_msg = req_msg->mutable_request();
  if(NULL == std_req_msg)
    return -1;

  if(true == std_req_fields_info.has_batch_period) {
    sns_std_request_batch_spec* batch_spec_msg = std_req_msg->mutable_batching();
    if(NULL == batch_spec_msg)
      return -1;

    if(true == std_req_fields_info.has_batch_period)
      batch_spec_msg->set_batch_period(std_req_fields_info.batch_period);
    if(true == std_req_fields_info.has_flush_period)
      batch_spec_msg->set_flush_period(std_req_fields_info.flush_period);
    if(true == std_req_fields_info.has_flush_only)
      batch_spec_msg->set_flush_only(std_req_fields_info.is_flush_only);
    if(true == std_req_fields_info.has_max_batch)
      batch_spec_msg->set_max_batch(std_req_fields_info.is_max_batch);
  } else {
    USTA_LOGI("sensor_direct_channel::send_request has_batch_period is false - Omiting batch_spec as it is optional");
  }

  std_req_msg->set_payload(sensor_req_encoded_payload);
  if(true == std_req_fields_info.has_passive)
    std_req_msg->set_is_passive(std_req_fields_info.is_passive);

  if(true == has_attributes_info) {
    sns_direct_channel_set_client_req_structured_mux_channel_stream_attributes* attributes_msg = req_msg->mutable_attributes();
    if(NULL == attributes_msg)
      return -1;
    attributes_msg->set_sensor_handle(attributes_info.sensor_handle);
    attributes_msg->set_sensor_type(attributes_info.sensor_type);
  }

  string pb_encoded_direct_channel_config_msg;
  config_msg.SerializeToString(&pb_encoded_direct_channel_config_msg);

  USTA_LOGI("pb_encoded_direct_channel_config_msg string is %s and size is %ld" , pb_encoded_direct_channel_config_msg.c_str() , pb_encoded_direct_channel_config_msg.size());

  int ret =sns_direct_channel_config(fastRPC_remote_handle, channel_handle, (const unsigned char*)pb_encoded_direct_channel_config_msg.c_str(), pb_encoded_direct_channel_config_msg.size());
  if(0 == ret) {
    USTA_LOGI("sensor_direct_channel send_request success");
  } else {
    USTA_LOGE("sensor_direct_channel send_request failed");
  }

  return ret;
}

int sensor_direct_channel::stop_request(
    direct_channel_stream_info stream_info
    )
{
  USTA_LOGI("sensor_direct_channel stop_request Start");

  string pb_encoded_direct_channel_config_msg;
  sns_direct_channel_config_msg config_msg;
  sns_direct_channel_remove_client_req *rmv_req = config_msg.mutable_remove_client_req();
  if(NULL == rmv_req)
    return -1;

  sns_direct_channel_stream_id *stream_id_msg = rmv_req->mutable_stream_id();
  if(NULL == stream_id_msg)
    return -1;

  sns_std_suid *suid = stream_id_msg->mutable_suid();
  if(NULL == suid)
    return -1;

  suid->set_suid_low(stream_info.suid_low);
  suid->set_suid_high(stream_info.suid_high);
  USTA_LOGI("channel_handle[%d], suid_low[%ld] , suid_high[%ld]" , channel_handle, stream_info.suid_low, stream_info.suid_high);
  if(true == stream_info.has_calibrated) {
    stream_id_msg->set_calibrated(stream_info.is_calibrated);
    USTA_LOGI("is_calibrated[%d]" , stream_info.is_calibrated);
  }

  if(true == stream_info.has_fixed_rate) {
    stream_id_msg->set_resampled(stream_info.is_resampled);
    USTA_LOGI("is_resampled[%d]" , stream_info.is_resampled);
  }
  config_msg.SerializeToString(&pb_encoded_direct_channel_config_msg);

  USTA_LOGI("sensor_direct_channel stop_request before sending to DSP ");
  int ret = sns_direct_channel_config(fastRPC_remote_handle, channel_handle, (const unsigned char*)pb_encoded_direct_channel_config_msg.c_str(), pb_encoded_direct_channel_config_msg.size());
  if(0 == ret) {
    USTA_LOGI("sensor_direct_channel stop_request success");
  } else {
    USTA_LOGE("sensor_direct_channel stop_request failed");
  }

  USTA_LOGI("sensor_direct_channel stop_request end ");
  return ret;
}


/*DEBUG PURPOSE */
void *sensor_direct_channel::shared_memory_reading_thread(void *param)
{
  sensor_direct_channel *ptr = ((sensor_direct_channel *)param);
  if(nullptr != ptr)
    ptr->run();
  return NULL;
}

char* sensor_direct_channel::getAllocPtrFromFD(int fd) {
  char *ret_ptr = NULL;
  for(unsigned int i = 0 ; i < gDirectChannelFDArray.size(); i++) {
    int currentFD = gDirectChannelFDArray.at(i);
    if(currentFD == fd) {
      ret_ptr = gDirectChannelMempointer.at(i);
      break;
    }
  }
  return ret_ptr;
}

void sensor_direct_channel::dump_mux_channel_data(char *start_ptr, FILE *fp)
{
  if(start_ptr == NULL || fp == NULL)
    return;

  char* temp = start_ptr;
  char *end_ptr = start_ptr + DIRECT_CHANNEL_SHARED_MEMORY_SIZE;
  int previous_counter = 0;
  if(temp == NULL || end_ptr == NULL)
    return;

  while(1) {
    if(fd_ptr == -1) {
      /*Channel is getting closed -- Close the thread */
      USTA_LOGI(" ");
      break;
    }

    sensors_event_t data = *reinterpret_cast<const sensors_event_t *>(temp);
    if(previous_counter >= data.reserved0) {
      /*No Data on the buffer.. So wait for 1 sec*/
      sleep(1);
    } else {

      fprintf(fp , "Ver[%" PRId32 "]\t" , data.version);
      fprintf(fp , "Handle[%" PRId32 "]\t" , data.sensor);
      fprintf(fp, "Type[%" PRId32 "]\t" ,  data.type);
      fprintf(fp, "Count[%" PRId32 "]\t" ,data.reserved0);
      fprintf(fp, "ts_nsec[%" PRId64 "]\t" ,data.timestamp);

      switch(data.type) {
      case SENSOR_TYPE_ACCELEROMETER:
          fprintf(fp, "Values x[%f],\t", data.acceleration.x);
          fprintf(fp, "y[%f],\t", data.acceleration.y);
          fprintf(fp, "z[%f],\t", data.acceleration.z);
          fprintf(fp, "status[%" PRId8 "f],\t", data.acceleration.status);
        break;
      case SENSOR_TYPE_MAGNETIC_FIELD:
          fprintf(fp, "Values x[%f],\t", data.magnetic.x);
          fprintf(fp, "y[%f],\t", data.magnetic.y);
          fprintf(fp, "z[%f],\t", data.magnetic.z);
          fprintf(fp, "status[%" PRId8 "f],\t", data.acceleration.status);
        break;
      case SENSOR_TYPE_GYROSCOPE:
          fprintf(fp, "Values x[%f],\t", data.gyro.x);
          fprintf(fp, "y[%f],\t", data.gyro.y);
          fprintf(fp, "z[%f],\t", data.gyro.z);
          fprintf(fp, "status[%" PRId8 "f],\t", data.acceleration.status);
        break;
      case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
          fprintf(fp, "Values uncalib x[%f],\t", data.uncalibrated_accelerometer.uncalib[0]);
          fprintf(fp, "y[%f],\t", data.uncalibrated_accelerometer.uncalib[1]);
          fprintf(fp, "z[%f],\t", data.uncalibrated_accelerometer.uncalib[2]);
          fprintf(fp, "bias x[%f],\t", data.uncalibrated_accelerometer.bias[0]);
          fprintf(fp, "y[%f],\t", data.uncalibrated_accelerometer.bias[1]);
          fprintf(fp, "z[%f],\t", data.uncalibrated_accelerometer.bias[2]);
        break;
      case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
        fprintf(fp, "Values uncalib x[%f],\t", data.uncalibrated_magnetic.uncalib[0]);
        fprintf(fp, "y[%f],\t", data.uncalibrated_magnetic.uncalib[1]);
        fprintf(fp, "z[%f],\t", data.uncalibrated_magnetic.uncalib[2]);
        fprintf(fp, "bias x[%f],\t", data.uncalibrated_magnetic.bias[0]);
        fprintf(fp, "y[%f],\t", data.uncalibrated_magnetic.bias[1]);
        fprintf(fp, "z[%f],\t", data.uncalibrated_magnetic.bias[2]);
        break;
      case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
        fprintf(fp, "Values uncalib x[%f],\t", data.uncalibrated_gyro.uncalib[0]);
        fprintf(fp, "y[%f],\t", data.uncalibrated_gyro.uncalib[1]);
        fprintf(fp, "z[%f],\t", data.uncalibrated_gyro.uncalib[2]);
        fprintf(fp, "bias x[%f],\t", data.uncalibrated_gyro.bias[0]);
        fprintf(fp, "y[%f],\t", data.uncalibrated_gyro.bias[1]);
        fprintf(fp, "z[%f],\t", data.uncalibrated_gyro.bias[2]);
        break;

      default:
        break;
      }
      fprintf(fp, "ch#[%d],\t latency[%fusec]\t", (uint8_t)((data.reserved1[2] & 0xFF000000) >> 24), (float)(data.reserved1[2] & 0x00FFFFFF));
      fprintf(fp, "\n");

      previous_counter = data.reserved0;

      temp = temp + sizeof(sensors_event_t);

      if(temp >= end_ptr) {
        USTA_LOGI("end of circular buffer ");
        temp = start_ptr;
      }
    }
  }
}

void sensor_direct_channel::run()
{
  char *start_ptr = getAllocPtrFromFD(fd_ptr);
  if(start_ptr == NULL) {
    return;
  }
  /*memset */
  memset(start_ptr, 0, DIRECT_CHANNEL_SHARED_MEMORY_SIZE);

  string filepath = "";
  filepath = filepath + "/data/vendor/sensors/direct_channel_" + std::to_string((unsigned long)this) + ".bin";
  FILE *fp = NULL;
  fp = fopen(filepath.c_str() , "w+");
  if(fp == NULL) {
    return;
  }
  if(channel_type == DIRECT_CHANNEL_TYPE_STRUCTURED_MUX_CHANNEL) {
     dump_mux_channel_data(start_ptr, fp);
  } else {
    dump_generic_channel_data(start_ptr, fp);
  }

  fclose(fp);
}

void sensor_direct_channel::dump_generic_channel_data(char *start_ptr, FILE *fp)
{
  if(start_ptr == NULL || fp == NULL)
    return;
  char* temp = start_ptr;
  char *end_ptr = start_ptr + DIRECT_CHANNEL_SHARED_MEMORY_SIZE;
  if(temp == NULL || end_ptr == NULL)
    return;
  unsigned long previous_timestamp = 0;
  while(1) {
    if(fd_ptr == -1) {
      /*Channel is getting closed -- Close the thread */
      USTA_LOGI(" ");
      break;
    }

    char* pckt_header_start_ptr = temp;
    char *pckt_header_end_ptr = pckt_header_start_ptr +  sizeof(sns_sensor_event);

    if(pckt_header_end_ptr > end_ptr) {
      USTA_LOGI("Circular buffer overflow scenario - header not fit");
      temp = start_ptr;
      continue;
    } else {

      sns_sensor_event event = *reinterpret_cast<const sns_sensor_event *>(temp);

      if(0 == event.timestamp || event.timestamp < previous_timestamp) {
        sleep(1);
      } else {

        char *pckt_payload_start_ptr = pckt_header_end_ptr;
        char *pckt_payload_end_ptr = pckt_header_end_ptr +  event.event_len;

        /*Read the header data and dump it to the file */
        /*Reading Header Data */
        fprintf(fp , "ts_nsec[%" PRId64 "]\t" , event.timestamp);
        fprintf(fp , "msg ID[%" PRId32 "]\t" , event.message_id);

        if(pckt_payload_end_ptr > end_ptr) {
          USTA_LOGI("Circular buffer overflow scenario - Headers fits but not payload");
          pckt_payload_start_ptr = start_ptr;
          pckt_payload_end_ptr = pckt_payload_start_ptr + event.event_len;
        }

        sns_std_sensor_event std_sensor_data;
        std_sensor_data.ParseFromArray(pckt_payload_start_ptr , event.event_len);
        USTA_LOGI("ts_nsec[%" PRId64 "]\t msg ID[%" PRId32 "]\t status[%d]\t", event.timestamp, event.message_id , (int)std_sensor_data.status());

        if(event.message_id == 1025) {
          for(int i = 0 ; i < std_sensor_data.data_size() ; i++) {
            if(i == std_sensor_data.data_size() - 1) {
              fprintf(fp, "[%f]\t", std_sensor_data.data(i));
            }
            else {
              fprintf(fp, "[%f],\t", std_sensor_data.data(i));
            }
          }
        }
        fprintf(fp, "\n");

        previous_timestamp = event.timestamp;
        temp = pckt_payload_end_ptr;
        if(temp >= end_ptr) {
          temp = start_ptr;
        }
      }
    }
  } // end while
}

#endif // SNS_SUPPORT_DIRECT_CHANNEL
