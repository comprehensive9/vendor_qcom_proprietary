/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
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
#include "sns_direct_channel.pb.h"
#include "sns_suid.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_direct_channel.h"
#include "AEEStdErr.h"
#include "rpcmem.h"
#include "remote.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "pthread.h"
#include "sns_direct_channel_buffer.h"
#include <iostream>
#include "sensors_timeutil.h"
#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

#ifndef DIRECT_CHANNEL_SHARED_MEMORY_SIZE
#define DIRECT_CHANNEL_SHARED_MEMORY_SIZE (10*8000*104)
#endif

using namespace std;
bool is_handle_available = false;
char* shared_buff_ptr = nullptr;
int channel_handle;
int total_sensors_found = 0;
remote_handle64 fastRPC_remote_handle = -1;
remote_handle64 remote_handle_fd = 0;
int ion_buffer_fd;
int adjusted_sample_rate;
pthread_t data_thread;
typedef struct sns_sensor_event {
  uint64_t timestamp;
  uint32_t message_id;
  uint32_t event_len;
} sns_sensor_event;

struct suid_info{
  uint64_t low;
  uint64_t high;
};

typedef enum sns_request_type {
  SNS_GENERIC_SUID = 0,
  SNS_GENERIC_ATTRIBUTES,
  SNS_GENERIC_SAMPLE,
  SNS_ANDROID_MUX_SAMPLE
}sns_request_type;

char sensor_data_type[20] = "";
int sample_rate_hz ;
int channel_type ;
int resample_type;
int calibrated_type;
int test_duration;
int max_sample_count;

vector<suid_info> suid_info_list;
vector<int> supported_sample_rate;

int total_sample_count = 0;

void fastRPC_remote_handle_init()
{
  printf("get_fastRPC_remote_handle Start");
  if(is_handle_available == false) {
    int nErr = AEE_SUCCESS;
    string uri = "file:///libsns_direct_channel_skel.so?sns_direct_channel_skel_handle_invoke&_modver=1.0";
    remote_handle64 handle_l;

    /*check for slpi or adsp*/
    struct stat sb;
    if(!stat("/sys/kernel/boot_slpi", &sb)){
      uri +="&_dom=sdsp";
      if (remote_handle64_open(ITRANSPORT_PREFIX "createstaticpd:sensorspd&_dom=sdsp", &remote_handle_fd)) {
        printf("failed to open remote handle for sensorspd - sdsp");
      }
    }
    else {
      uri +="&_dom=adsp";
      if (remote_handle64_open(ITRANSPORT_PREFIX "createstaticpd:sensorspd&_dom=adsp", &remote_handle_fd)) {
        printf("failed to open remote handle for sensorspd - adsp\n");
      }
    }

    if (AEE_SUCCESS == (nErr = sns_direct_channel_open(uri.c_str(), &handle_l))) {
      printf("sns_direct_channel_open success for sensorspd - handle_l is %ud\n" , (unsigned int)handle_l);
      fastRPC_remote_handle = handle_l;
      is_handle_available = true;
    } else {
      fastRPC_remote_handle = -1;
    }
  }
  printf("get_fastRPC_remote_handle End");
}

int create_rpc_memory() {
  shared_buff_ptr = (char*)rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, DIRECT_CHANNEL_SHARED_MEMORY_SIZE);
  if (NULL == shared_buff_ptr) {
    printf("open_new_direct_channel: rpcmem_alloc failed \n");
    return -1;
  }
  printf("open_new_direct_channel: rpcmem_alloc success %p \n", shared_buff_ptr);
  int fd = rpcmem_to_fd((void *)shared_buff_ptr);
  return fd;
}

void delete_direct_channel() {
  printf("delete_direct_channel Start \n");
  if(sns_direct_channel_delete(fastRPC_remote_handle, channel_handle) == 0) {
    printf("delete_direct_channel pass  \n");
  } else {
    printf("delete_direct_channel fail \n");
  }
  channel_handle = -1;
  rpcmem_free((void*)shared_buff_ptr);
  printf("delete_direct_channel end \n");
}

void create_direct_channel(bool is_generic_channel) {

  int fd = create_rpc_memory();
  ion_buffer_fd = fd;
  printf("open_new_direct_channel: fd is %d for channel_type %d \n" , fd , is_generic_channel);
  sns_direct_channel_create_msg create_msg;
  sns_direct_channel_create_msg_shared_buffer_config *shared_buffer_config = create_msg.mutable_buffer_config();
  if(NULL == shared_buffer_config)
    return;
  printf("open_new_direct_channel: shared_buffer_config not null \n");

  shared_buffer_config->set_fd(fd);
  shared_buffer_config->set_size(DIRECT_CHANNEL_SHARED_MEMORY_SIZE);

  if(true == is_generic_channel)
    create_msg.set_channel_type(DIRECT_CHANNEL_TYPE_GENERIC_CHANNEL);
  else
    create_msg.set_channel_type(DIRECT_CHANNEL_TYPE_STRUCTURED_MUX_CHANNEL);

  create_msg.set_client_proc(SNS_STD_CLIENT_PROCESSOR_APSS);

  string pb_encoded_direct_channel_req_msg;
  create_msg.SerializeToString(&pb_encoded_direct_channel_req_msg);

  printf("open_new_direct_channel: before sns_direct_channel_create  \n");

  int ret = sns_direct_channel_create(fastRPC_remote_handle, (const unsigned char*)pb_encoded_direct_channel_req_msg.c_str(), pb_encoded_direct_channel_req_msg.size(), &channel_handle);
  if(0 == ret) {
    printf("sensor_direct_channel sns_direct_channel_create success , and channel_handle %d \n ", channel_handle);
  } else {
    printf("sensor_direct_channel sns_direct_channel_create failed \n");
  }
  printf("open_new_direct_channel End \n");
}

void send_config_request(sns_request_type req_type) {

  sns_direct_channel_config_msg config_msg;
  sns_direct_channel_set_client_req* req_msg = config_msg.mutable_set_client_req();
  if(NULL == req_msg)
    return;

  if(req_type == SNS_GENERIC_SUID)
    req_msg->set_msg_id(SNS_SUID_MSGID_SNS_SUID_REQ);

  if(req_type == SNS_GENERIC_ATTRIBUTES)
    req_msg->set_msg_id(SNS_STD_MSGID_SNS_STD_ATTR_REQ);

  if(req_type == SNS_GENERIC_SAMPLE || req_type == SNS_ANDROID_MUX_SAMPLE)
    req_msg->set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);

  sns_direct_channel_stream_id* stream_id_msg = req_msg->mutable_stream_id();
  if(NULL == stream_id_msg)
    return;

  sns_std_suid *suid = stream_id_msg->mutable_suid();
  if(NULL == suid)
    return;

  if(req_type == SNS_GENERIC_SUID) {
    suid->set_suid_low(12370169555311111083ull);
    suid->set_suid_high(12370169555311111083ull);
  }

  if(req_type == SNS_GENERIC_ATTRIBUTES || req_type == SNS_GENERIC_SAMPLE || req_type == SNS_ANDROID_MUX_SAMPLE) {
    suid->set_suid_low(suid_info_list.at(0).low);
    suid->set_suid_high(suid_info_list.at(0).high);
  }

  if(req_type == SNS_ANDROID_MUX_SAMPLE) {
    sns_direct_channel_set_client_req_structured_mux_channel_stream_attributes* attributes_msg = req_msg->mutable_attributes();
    if(NULL == attributes_msg)
      return;

    attributes_msg->set_sensor_handle(0);

    if(strcmp(sensor_data_type, "accel") == 0) {
      if(0 == calibrated_type)
        attributes_msg->set_sensor_type(SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED);
      else
        attributes_msg->set_sensor_type(SENSOR_TYPE_ACCELEROMETER);
    } else if (strcmp(sensor_data_type, "gyro") == 0) {
      if(0 == calibrated_type)
        attributes_msg->set_sensor_type(SENSOR_TYPE_GYROSCOPE_UNCALIBRATED);
      else
        attributes_msg->set_sensor_type(SENSOR_TYPE_GYROSCOPE);
    } else if (strcmp(sensor_data_type,"mag") == 0) {
      if(0 == calibrated_type)
        attributes_msg->set_sensor_type(SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED);
      else
        attributes_msg->set_sensor_type(SENSOR_TYPE_MAGNETIC_FIELD);
    }

  }

  sns_std_request* std_req_msg = req_msg->mutable_request();
  if(NULL == std_req_msg)
    return;

  if(req_type == SNS_GENERIC_SAMPLE || req_type == SNS_ANDROID_MUX_SAMPLE) {

    if(0 == calibrated_type)
      stream_id_msg->set_calibrated(false);
    else
      stream_id_msg->set_calibrated(true);

    if(0 == resample_type)
      stream_id_msg->set_resampled(false);
    else
      stream_id_msg->set_resampled(true);

  } else {
    stream_id_msg->set_calibrated(false);
    stream_id_msg->set_resampled(false);
  }


  string payload = "";
  if(req_type == SNS_GENERIC_SUID) {
    std::string s_name(sensor_data_type);
    sns_suid_req reg;
    reg.set_data_type(s_name);
    reg.SerializeToString(&payload);
  }
  if(req_type == SNS_GENERIC_ATTRIBUTES) {
    /*This is something like on change sensor. It doesn't have any message
      and corresponding payload. So this should be empty*/
  }
  if(req_type == SNS_GENERIC_SAMPLE || req_type == SNS_ANDROID_MUX_SAMPLE) {
    sns_std_sensor_config std_req;
    std_req.set_sample_rate(float(sample_rate_hz));
    std_req.SerializeToString(&payload);
  }
  std_req_msg->set_payload(payload);

  string pb_encoded_direct_channel_config_msg;
  config_msg.SerializeToString(&pb_encoded_direct_channel_config_msg);

  printf("pb_encoded_direct_channel_config_msg string is %s and size is %ld \n" , pb_encoded_direct_channel_config_msg.c_str() , pb_encoded_direct_channel_config_msg.size());

  int ret =sns_direct_channel_config(fastRPC_remote_handle, channel_handle, (const unsigned char*)pb_encoded_direct_channel_config_msg.c_str(), pb_encoded_direct_channel_config_msg.size());
  if(0 == ret) {
    printf("sensor_direct_channel send_request success \n");
  } else {
    printf("sensor_direct_channel send_request failed \n");
  }

}

void sr_based_sleep() {
  usleep(1000000/adjusted_sample_rate);
}

void* read_mux_data_event(void *) {
  char* temp = shared_buff_ptr;
  if(NULL == shared_buff_ptr)
    return NULL;

  char *end_ptr = shared_buff_ptr + DIRECT_CHANNEL_SHARED_MEMORY_SIZE;
  int previous_counter = 0;

  while(1) {

    sensors_event_t data = *reinterpret_cast<const sensors_event_t *>(temp);
    if(previous_counter >= data.reserved0) {
      sr_based_sleep();
    } else {
      printf("Ver[%d]," , data.version);
      printf("Handle[%d]," , data.sensor);
      printf("Type[%d]," ,  data.type);
      printf("Count[%d]," ,data.reserved0);
      printf("ts_nsec[%ld]," ,data.timestamp);

      switch(data.type) {
      case SENSOR_TYPE_ACCELEROMETER:
          printf("Values x[%f],", data.acceleration.x);
          printf("y[%f],", data.acceleration.y);
          printf("z[%f],", data.acceleration.z);
          printf("status[%d]", data.acceleration.status);
        break;
      case SENSOR_TYPE_MAGNETIC_FIELD:
          printf("Values x[%f],", data.magnetic.x);
          printf("y[%f],", data.magnetic.y);
          printf("z[%f],", data.magnetic.z);
          printf("status[%d]", data.magnetic.status);
        break;
      case SENSOR_TYPE_GYROSCOPE:
          printf("Values x[%f],", data.gyro.x);
          printf("y[%f],", data.gyro.y);
          printf("z[%f],", data.gyro.z);
          printf("status[%d]", data.gyro.status);
        break;
      case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
          printf("Values uncalib x[%f],", data.uncalibrated_accelerometer.uncalib[0]);
          printf("y[%f],", data.uncalibrated_accelerometer.uncalib[1]);
          printf("z[%f],", data.uncalibrated_accelerometer.uncalib[2]);
          printf("bias x[%f],", data.uncalibrated_accelerometer.bias[0]);
          printf("y[%f],", data.uncalibrated_accelerometer.bias[1]);
          printf("z[%f]", data.uncalibrated_accelerometer.bias[2]);
        break;
      case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
          printf("Values uncalib x[%f],", data.uncalibrated_magnetic.uncalib[0]);
          printf("y[%f],", data.uncalibrated_magnetic.uncalib[1]);
          printf("z[%f],", data.uncalibrated_magnetic.uncalib[2]);
          printf("bias x[%f],", data.uncalibrated_magnetic.bias[0]);
          printf("y[%f],", data.uncalibrated_magnetic.bias[1]);
          printf("z[%f]", data.uncalibrated_magnetic.bias[2]);
        break;
      case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
          printf("Values uncalib x[%f],", data.uncalibrated_gyro.uncalib[0]);
          printf("y[%f],", data.uncalibrated_gyro.uncalib[1]);
          printf("z[%f],", data.uncalibrated_gyro.uncalib[2]);
          printf("bias x[%f],", data.uncalibrated_gyro.bias[0]);
          printf("y[%f],", data.uncalibrated_gyro.bias[1]);
          printf("z[%f]", data.uncalibrated_gyro.bias[2]);
        break;
      default:
        break;
      }
      printf(",ch#[%d],latency[%fusec]", (uint8_t)((data.reserved1[2] & 0xFF000000) >> 24), (float)(data.reserved1[2] & 0x00FFFFFF));

      total_sample_count++;
      if(max_sample_count <= total_sample_count)
        break;
      previous_counter = data.reserved0;

      temp = temp + sizeof(sensors_event_t);

      if(temp >= end_ptr) {
        printf(",\tend of circular buffer");
        temp = shared_buff_ptr;
      }
      printf("\n");
    }
  }
  return 0;
}

void read_generic_data_event(sns_request_type event_type) {
    char* temp = shared_buff_ptr;
    char *end_ptr = shared_buff_ptr + DIRECT_CHANNEL_SHARED_MEMORY_SIZE;
    printf("start %p , end: %p \n" , shared_buff_ptr, end_ptr);
    while(1) {

      char* pckt_header_start_ptr = temp;
      char *pckt_header_end_ptr = pckt_header_start_ptr +  sizeof(sns_sensor_event);

      if(pckt_header_end_ptr > end_ptr) {
        printf("\n\n Header itself not fit .. So moving to start of the buffer . pckt_header_start_ptr %p , pckt_header_end_ptr %p , end_ptr %p \n", pckt_header_start_ptr , pckt_header_end_ptr, end_ptr);
        temp = shared_buff_ptr;
        continue;
      } else {
        sns_sensor_event event = *reinterpret_cast<const sns_sensor_event *>(temp);

        if(0 == event.timestamp) {
          sr_based_sleep();
        } else {
          char *pckt_payload_start_ptr = pckt_header_end_ptr ;
          char *pckt_payload_end_ptr = pckt_header_end_ptr +  event.event_len;

          if(pckt_payload_end_ptr > end_ptr) {
            printf("\npayload not fit. So read the header from last and reading the payload from start. \n");
            printf("pckt_header_start_ptr %p , pckt_header_end_ptr %p , end_ptr %p \n", pckt_header_start_ptr , pckt_header_end_ptr, end_ptr);
            printf("pckt_payload_start_ptr %p , pckt_payload_end_ptr %p , end_ptr %p \n", pckt_payload_start_ptr , pckt_payload_end_ptr, end_ptr);
            printf("Circular buffer overflow scenario - Headers fits but not payload with length %d with message id as %d  \n" , event.event_len, (int)event.message_id);

            pckt_payload_start_ptr = shared_buff_ptr;
            pckt_payload_end_ptr = pckt_payload_start_ptr + event.event_len;
          }
//          /*SUID Event decoding*/
          if(event_type == SNS_GENERIC_SUID) {
            if(event.message_id == SNS_SUID_MSGID_SNS_SUID_EVENT) {
              sns_suid_event suid_event_data;
              suid_event_data.ParseFromArray(pckt_payload_start_ptr , event.event_len);
              string current_data_type = suid_event_data.data_type();
              int suid_count = suid_event_data.suid_size();
              total_sensors_found = suid_count;
              printf("suid_count is %d \n" , suid_count);
              for(int i =0 ; i < suid_count ; i++) {
                suid_info suid;
                suid.low = suid_event_data.suid(i).suid_low();
                suid.high = suid_event_data.suid(i).suid_high();
                suid_info_list.push_back(suid);
              }
              break;
            }
          }

          /*Attribute Event decoding*/
          if(event_type == SNS_GENERIC_ATTRIBUTES) {
            if(event.message_id == SNS_STD_MSGID_SNS_STD_ATTR_EVENT) {
              sns_std_attr_event attr_event_data;
              attr_event_data.ParseFromArray(pckt_payload_start_ptr, event.event_len);
              int attr_count = attr_event_data.attributes_size();
              for(int i = 0 ;i < attr_count ; i++){
                printf("attribute count %d \t and values are: ", i);
                sns_std_attr attr = attr_event_data.attributes(i);
                printf("attr_id: %d \t " , attr.attr_id());
                int current_attrib_id = attr.attr_id();
                sns_std_attr_value attr_value = attr.value();
                int attr_value_count = attr_value.values_size();
                  for(int i = 0; i < attr_value_count ; i ++) {
                    sns_std_attr_value_data val = attr_value.values(i);
                    if(val.has_flt()) {
                      printf("flt: %f" , val.flt());
                      /*Storting supported sample rates in hz in the form of vector*/
                      if(current_attrib_id == SNS_STD_SENSOR_ATTRID_RATES) {
                        supported_sample_rate.push_back((int)val.flt());
                      }
                    } else if(val.has_sint()){
                      printf("sint: %ld" , val.sint());
                    } else if(val.has_boolean()) {
                      printf("boolean %d ", (int)val.boolean());
                    } else if(val.has_str()) {
                      printf("std: %s " , val.str().c_str());
                    }
                  }
                printf("\n");
              }
              break;
            }
          }

          if(event_type == SNS_GENERIC_SAMPLE) {
            printf("time_stamp %lx with msg_ID %d\t" , event.timestamp, (int)event.message_id);
            if(event.message_id == 1025) {
              sns_std_sensor_event std_sensor_data;
              std_sensor_data.ParseFromArray(pckt_payload_start_ptr , event.event_len);
              for(int i = 0 ; i < std_sensor_data.data_size() ; i++) {
                if(i == std_sensor_data.data_size() - 1) {
                  printf("[%f]\t", std_sensor_data.data(i));
                }
                else {
                  printf("[%f],\t", std_sensor_data.data(i));
                }
              }
            }

            printf("\n");

            total_sample_count++;
            if(max_sample_count <= total_sample_count)
              break;
          }


          temp = pckt_payload_end_ptr;
          if(temp >= end_ptr) {
            printf("looping to beginning of the buffer \n");
            temp = shared_buff_ptr;
          }
        }
      }
    }

}

void get_suid() {
  printf("get_suid started for sensor %s \n" , sensor_data_type);
  create_direct_channel(true);
  send_config_request(SNS_GENERIC_SUID);
  read_generic_data_event(SNS_GENERIC_SUID);
  printf("******************\n");
  printf("SUID event received and total Number of sensors Found are %d \n" , total_sensors_found);
  for(int i = 0 ; i < total_sensors_found ; i ++) {
    printf("sensor[%d]: suid_low:%lu , suid_high:%lu \n" , i, suid_info_list.at(i).low , suid_info_list.at(i).high);
  }
  printf("******************\n");
  delete_direct_channel();
  printf("get_suid_list Ended for sensor %s \n", sensor_data_type);
}

void get_attributes() {
  printf("get_attributes_information started for sensor %s \n" , sensor_data_type);
  create_direct_channel(true);
  send_config_request(SNS_GENERIC_ATTRIBUTES);
  read_generic_data_event(SNS_GENERIC_ATTRIBUTES);
  delete_direct_channel();
  printf("get_attributes_information Ended for sensor %s \n", sensor_data_type);
}

void set_ts_offset(){
  printf("sensor_direct_channel set_ts_offset start \n");
  int64_t ts_offset;
  sensors_timeutil& timeutil = sensors_timeutil::get_instance();
  timeutil.recalculate_offset(true);
  ts_offset = timeutil.getElapsedRealtimeNanoOffset();

  sns_direct_channel_config_msg config_msg;
  sns_direct_channel_set_ts_offset *offset_msg = config_msg.mutable_set_ts_offset();
  if(NULL == offset_msg)
    return;
  offset_msg->set_ts_offset(ts_offset);
  printf("sensor_direct_channel current offset is %d \n", (int)ts_offset);
  string pb_encoded_direct_channel_config_msg;
  config_msg.SerializeToString(&pb_encoded_direct_channel_config_msg);
  int ret = sns_direct_channel_config(fastRPC_remote_handle, channel_handle, (const unsigned char*)pb_encoded_direct_channel_config_msg.c_str(), pb_encoded_direct_channel_config_msg.size());
  if(0 == ret) {
    printf("sensor_direct_channel set_ts_offset success \n");
  } else {
    printf("sensor_direct_channel set_ts_offset failed \n");
  }
  printf("sensor_direct_channel set_ts_offset End \n");
}

void start_sensor_streaming() {
  printf("sensor_streaming started for sensor %s \n" , sensor_data_type);
  if(0 == channel_type) {
    create_direct_channel(false);
    pthread_create(&data_thread, NULL, read_mux_data_event, NULL);
    set_ts_offset();
    send_config_request(SNS_ANDROID_MUX_SAMPLE);
    pthread_join(data_thread,NULL);
  } else {
    create_direct_channel(true);
    set_ts_offset();
    send_config_request(SNS_GENERIC_SAMPLE);
    read_generic_data_event(SNS_GENERIC_SAMPLE);
  }
  printf("\nsensor_streaming Ended for sensor %s \n", sensor_data_type);
}

void stop_sensor_streaming() {
    printf("stop_streaming Start\n");

    string pb_encoded_direct_channel_config_msg;
    sns_direct_channel_config_msg config_msg;
    sns_direct_channel_remove_client_req *rmv_req = config_msg.mutable_remove_client_req();
    if(NULL == rmv_req)
      return;

    sns_direct_channel_stream_id *stream_id_msg = rmv_req->mutable_stream_id();
    if(NULL == stream_id_msg)
      return;

    sns_std_suid *suid = stream_id_msg->mutable_suid();
    if(NULL == suid)
      return;

    suid->set_suid_low(suid_info_list.at(0).low);
    suid->set_suid_high(suid_info_list.at(0).high);

    if(0 == calibrated_type)
      stream_id_msg->set_calibrated(false);
    else
      stream_id_msg->set_calibrated(true);

    if(0 == resample_type)
      stream_id_msg->set_resampled(false);
    else
      stream_id_msg->set_resampled(true);

    config_msg.SerializeToString(&pb_encoded_direct_channel_config_msg);

    int ret = sns_direct_channel_config(fastRPC_remote_handle, channel_handle, (const unsigned char*)pb_encoded_direct_channel_config_msg.c_str(), pb_encoded_direct_channel_config_msg.size());
    if(0 == ret) {
      printf("stop_streaming success \n");
    } else {
      printf("stop_streaming failed \n");
    }
    delete_direct_channel();
    printf("stop_streaming end \n");
}

void caluclate_adjusted_sample_rate() {
  int asked = sample_rate_hz;
  int adjusted =  -1;
  if(false == resample_type) {
    adjusted = asked;
  } else {
    for(unsigned int i =0 ; i < supported_sample_rate.size(); i ++) {
      if(supported_sample_rate.at(i) < asked ) {
        continue;
      } else {
        adjusted = supported_sample_rate.at(i);
        break;
      }
    }
    if(adjusted == -1 ) {
      adjusted = asked;
    }
  }
  printf("\n Resample Status: %d,\tSR Asked for %d,\tSR adjusted %d \n" , resample_type, asked,adjusted );
  adjusted_sample_rate = adjusted;
}
void resource_cleanup() {
  if(0 != remote_handle_fd)
    remote_handle64_close(remote_handle_fd);
}

int main(int argc, char *argv[])
{
  printf("Main started \n");
  if(argc != 7) {
    cout << "usage sns_direct_client_example_generic_path <sensor_data_type> <Cal/un-cal> <resample: [0/1]> <sample_rate_hz> <channel_type: [0-Mux  /  1-Generic]> <Duration in Secs>\n\n" << endl;
    cout << "\t\tsensor_data_type:\t Any one of - accel / mag / gyro " << endl;
    cout << "\t\tCalibrated Data:\t 1 - calibrated o/p , 0 - uncalibrated o/p " << endl;
    cout << "\t\tResampled Data:\t 1 - resample o/p , 0 - sensor o/p " << endl;
    cout << "\t\tSample_rate:\t should be in frequency domain " << endl;
    cout << "\t\tChannel Type:\t 1 - Generic Channel\t , 0 - Android Mux Channel " << endl;
    cout << "\t\tTestDuration in secs " << endl;
    exit(1);
  }

  strlcpy(sensor_data_type, argv[1], sizeof(sensor_data_type));
  calibrated_type = atoi(argv[2]);
  resample_type = atoi(argv[3]);
  sample_rate_hz = atoi(argv[4]);
  channel_type = atoi(argv[5]);
  test_duration = atoi(argv[6]);

  if(!((resample_type == 0 || resample_type == 1) && (calibrated_type == 0 || calibrated_type == 1) && (channel_type == 0 || channel_type == 1 ))) {
    printf("Invalid input argument - one of [resample_type | calibrated_type | channel_type ]");
    exit(1);
  }

  printf("\nRequest:: sensor_data_type: %s,\t Calib:%d,\t ,Resample:%d,\t SR:%d,\t Channel:%d\t Duration:%d \n", sensor_data_type, calibrated_type, resample_type, sample_rate_hz, channel_type,test_duration);

  fastRPC_remote_handle_init();
  get_suid();
  get_attributes();
  caluclate_adjusted_sample_rate();
  max_sample_count = test_duration * adjusted_sample_rate;
  start_sensor_streaming();
  stop_sensor_streaming();
  resource_cleanup();
  printf("Main Ended \n");
  return 0;
}
#else
int main() {
  printf("Direct channel feature is disabled on this target. Please enable it compile time\n");
  return -1;
}
#endif
