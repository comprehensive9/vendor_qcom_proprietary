/*============================================================================
  Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_cxt.cpp

  @brief
  SensorContext class implementation.
============================================================================*/
#include "sensor_cxt.h"
#include <algorithm>  //for using sort of vector
#include <stdlib.h>

#include "sensors_timeutil.h"
bool waitForResponse(pthread_mutex_t *cb_mutex_ptr,pthread_cond_t *cond_ptr,
    bool *cond_var)
{
  bool ret = false;            /* the return value of this function */
  int rc = 0;                  /* return code from pthread calls */


  /* special case where callback is issued before the main function
       can wait on cond */
  pthread_mutex_lock(cb_mutex_ptr);
  if (*cond_var == true)
  {
    ret = true;
  }
  else
  {
    while (*cond_var != true)
    {
      rc = pthread_cond_wait(cond_ptr, cb_mutex_ptr);
    }
    ret = ( rc == 0 || *cond_var ) ? true : false;
  }
  *cond_var = false;
  pthread_mutex_unlock(cb_mutex_ptr);
  return ret;
}

SensorCxt* SensorCxt::getInstance(sensor_stream_path stream_path)
{
  SensorCxt* self = new SensorCxt(stream_path);
  return self;
}

/*============================================================================
  SensorCxt Constructor
============================================================================*/
SensorCxt::SensorCxt(sensor_stream_path in_stream_path)
{
  log_instance = usta_logging_util::get_instance();
  is_all_attributes_received = false;
  INFO_LOG(log_instance , " Unified Sensor Test Application triggered ");
  //intializing condition variable and mutex
  pthread_mutex_init(&cb_mutex, NULL);
  pthread_mutex_init(&attr_count_mutex, NULL);
  pthread_cond_init(&cb_cond, NULL);

  attributes_info.max_sampling_rate = 0.0;
  attributes_info.max_report_rate = 0.0;

  stream_path = in_stream_path;

  num_sensors =0;
  num_attr_received =0;

  is_resp_arrived=false;
  //initializing sensor event callback
  suid_event_cb = [this](uint64_t& suid_low,uint64_t& suid_high, bool flag)
    { this->handle_suid_event(suid_low,suid_high,flag);};

  attribute_event_cb =[this]()
    { this->handle_attribute_event();};

  stream_event_cb=[this](string inString , bool is_registry_sensor)
    { this->handle_stream_event(inString, is_registry_sensor);};

  stream_error_cb=[this](ssc_error_type error_code)
    { this->handle_stream_error_event(error_code);};

  attrib_error_cb=[this](ssc_error_type error_code)
    { this->handle_attrib_error_event(error_code);};

  // building descriptor pool
  if (USTA_RC_SUCCESS != build_descriptor_pool())
  {
    ERROR_LOG(log_instance , "Descriptor pool generation failed.. exiting the application ");
    assert(false);
  }

  // first SUID class object is created and request is sent for listing
  //all the sensors

  if (NULL == (suid_instance = new SuidSensor(suid_event_cb)))
  {
    ERROR_LOG(log_instance , "suid instance creation failed");
    return;
  }

  DEBUG_LOG(log_instance , " Intiating sensor_list request from SSC via SUID  ");

  string list_req_special_char = "";

  if (USTA_RC_SUCCESS !=(suid_instance->send_request(list_req_special_char)))
  {
    ERROR_LOG(log_instance , "\n SUID listing request failed ");
    return;
  }


  if (waitForResponse(&cb_mutex,&cb_cond,&is_resp_arrived) == false)
  {
    ERROR_LOG(log_instance , "\nError  ");
    pthread_mutex_unlock(&cb_mutex);
  }
  else
  {
    DEBUG_LOG(log_instance , "\nSensor list Response Received");
  }

  // removing suid_instance
  delete suid_instance;
  suid_instance = NULL;


  // sending attrib request for each sensor
  int i = 0;
  for(list<Sensor *>::iterator sensorHandle = mSensors.begin();
      sensorHandle != mSensors.end(); sensorHandle++)
  {
    is_resp_arrived=false;
    DEBUG_LOG(log_instance, "Sending Attribute request for handle " +  to_string(i));
    (*sensorHandle)->send_attrib_request();
    // wait for respective event to come
    if (waitForResponse(&cb_mutex,&cb_cond,&is_resp_arrived) == false)
    {
      ERROR_LOG(log_instance , "\nError  ");
      pthread_mutex_unlock(&cb_mutex);
    }
    else
    {
      DEBUG_LOG(log_instance , "Attribute  Response Received for handle " + to_string(i));
    }
    i++;
    // close the attribute connection as soon as its event arrives
    (*sensorHandle)->remove_attribute_connection();
  }


  DEBUG_LOG(log_instance , "Number of sensors from SSC : " + to_string(mSensors.size()) + "filter out platform sensors");
  for(list<Sensor *>::iterator sensorHandle = mSensors.begin();
      sensorHandle != mSensors.end(); )
  {
    if((*sensorHandle)->is_platform_sensor() == true)
    {
      delete *sensorHandle;
      *sensorHandle = NULL;
      sensorHandle = mSensors.erase(sensorHandle);
    }
    else
    {
      sensorHandle++;
    }
  }
#ifdef SNS_SUPPORT_DIRECT_CHANNEL
  if(stream_path == DIRECT_CHANNEL) {

    for(list<Sensor *>::iterator sensorHandle = mSensors.begin();
        sensorHandle != mSensors.end(); )
    {
      (*sensorHandle)->get_direct_channel_attributes_info(attributes_info);
      if((*sensorHandle)->is_direct_channel_supported_sensor() == false)
      {
        delete *sensorHandle;
        *sensorHandle = NULL;
        sensorHandle = mSensors.erase(sensorHandle);
      }
      else
      {
        sensorHandle++;
      }
    }

  }
#endif
  is_all_attributes_received = true;
  DEBUG_LOG(log_instance , "Number of sensors after filtering : " + to_string(mSensors.size()));


  DEBUG_LOG(log_instance , "Sensor Context instantiated");
}

SensorCxt::~SensorCxt()
{

  if (suid_instance)  delete suid_instance;
  suid_instance =NULL;


  /* Deleting sensor memory */
  for(list<Sensor *>::iterator sensorHandle = mSensors.begin();
      sensorHandle != mSensors.end(); )
  {
    delete *sensorHandle;
    *sensorHandle = NULL;
    sensorHandle = mSensors.erase(sensorHandle);
  }

  pthread_mutex_destroy(&cb_mutex);
  pthread_mutex_destroy(&attr_count_mutex);
  pthread_cond_destroy(&cb_cond);

}

usta_rc SensorCxt::get_request_list(unsigned int handle,vector<msg_info> &request_msgs)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  DEBUG_LOG(log_instance , " get_request_list for handle " + to_string(handle));
  for(sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return USTA_RC_FAILED;
  }
  return (*sensorHandle)->get_request_list(request_msgs);
}

usta_rc SensorCxt::get_attributes(unsigned int handle,string& attribute_list)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  for( sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return USTA_RC_FAILED;
  }

  return (*sensorHandle)->get_attributes(attribute_list);
}

usta_rc SensorCxt::get_sensor_list(vector<sensor_info>& sensor_list)
{

  DEBUG_LOG(log_instance , " Client requests for sensor lists");
  int i=0;
  for(auto sensorHandle = mSensors.begin();
      sensorHandle != mSensors.end(); sensorHandle++,i++)
  {
    sensor_info sensor_info_var;
    if(USTA_RC_SUCCESS!=(*sensorHandle)->get_sensor_info(sensor_info_var))
    {
      ERROR_LOG(log_instance , "Error handle "+ to_string(i) + " not part of sensor list");
    }
    else
    {
      sensor_list.push_back(sensor_info_var);
    }
  }

  return USTA_RC_SUCCESS;

}

usta_rc SensorCxt::remove_sensors(vector<int>& remove_sensor_list)
{
  // sorting the handles in acending order
  sort(remove_sensor_list.begin(), remove_sensor_list.end());
  list<Sensor *>::iterator sensorHandle;
  for(int i=remove_sensor_list.size()-1; i>=0; i--)
  {
    sensorHandle = next(mSensors.begin(), remove_sensor_list[i]);
    delete (*sensorHandle);
    mSensors.erase(sensorHandle);
  }

  return USTA_RC_SUCCESS;


}

usta_rc  SensorCxt::send_request(unsigned int handle, send_req_msg_info& formated_request,
    client_req_msg_fileds &std_fields_data,
    string& logfile_name ,  string usta_mode)
{
  unsigned int i=0;
  usta_rc rc=USTA_RC_SUCCESS;
  for(auto sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i)
    {
      rc=(*sensorHandle)->send_request(formated_request,std_fields_data, logfile_name, usta_mode);
      break;
    }
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return USTA_RC_FAILED;
  }

  if (rc!=USTA_RC_SUCCESS)
  {
    ERROR_LOG(log_instance , "sending request for handle "+ to_string(handle) + " failed");
    return rc;
  }
  // for testing only
#ifdef SENSOR_TEST
  if (waitForResponse(&cb_mutex,&cb_cond,&is_resp_arrived) == false)
  {
    ERROR_LOG(log_instance , "\nError  ");
    pthread_mutex_unlock(&cb_mutex);

  }
  else
  {
    DEBUG_LOG(log_instance , "\nSensor list Response Received");
  }
#endif
  return rc;

}

usta_rc  SensorCxt::stop_request(unsigned int handle , bool is_qmi_disable_api_enabled)
{
  unsigned int i=0;
  usta_rc rc=USTA_RC_SUCCESS;
  for(auto sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i)
    {
      rc=(*sensorHandle)->stop_request(is_qmi_disable_api_enabled);
      break;
    }
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return USTA_RC_FAILED;
  }

  if (rc!=USTA_RC_SUCCESS)
  {
    ERROR_LOG(log_instance , "sending request for handle "+ to_string(handle) + " failed");
    return rc;
  }

  return rc;

}


void SensorCxt::handle_suid_event(uint64_t& suid_low,uint64_t& suid_high,
    bool is_last_suid)
{

  // creating sensor to query attributes in constructor itself
  Sensor* sensorHandle = new Sensor(attribute_event_cb,
      suid_low,
      suid_high,
      stream_event_cb,
      stream_error_cb,
      attrib_error_cb);
  mSensors.push_back(sensorHandle);

  num_sensors++;

  if (is_last_suid)
  {
    pthread_mutex_lock(&cb_mutex);
    is_resp_arrived=true;
    pthread_cond_signal(&cb_cond);
    pthread_mutex_unlock(&cb_mutex);
  }
}

void SensorCxt::handle_attribute_event()
{

  pthread_mutex_lock(&attr_count_mutex);

  pthread_mutex_lock(&cb_mutex);
  is_resp_arrived=true;
  pthread_cond_signal(&cb_cond);
  pthread_mutex_unlock(&cb_mutex);

  pthread_mutex_unlock(&attr_count_mutex);

}

void SensorCxt::handle_stream_event(string inString , bool is_registry_sensor)
{

  if(ptr_display_samples_cb != NULL )
    (*ptr_display_samples_cb)(inString , is_registry_sensor);
}

void SensorCxt::register_qmi_error_cb(unsigned int handle, handle_stream_error_func client_error_func , usta_error_cb_type connection_type)
{
  unsigned int i=0;
  for(auto sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i)
    {

      if(connection_type == USTA_QMI_ERRRO_CB_FOR_ATTRIBUTES)
      {
        if(is_all_attributes_received == false)
        {
          client_attrib_error_cb = client_error_func;
          (*sensorHandle)->register_error_cb(USTA_QMI_ERRRO_CB_FOR_ATTRIBUTES);
        }
      }
      else if(connection_type == USTA_QMI_ERRRO_CB_FOR_STREAM)
      {
        client_stream_error_cb = client_error_func;
        (*sensorHandle)->register_error_cb(USTA_QMI_ERRRO_CB_FOR_STREAM);
      }
      break;
    }
  }

}

void SensorCxt::handle_stream_error_event(ssc_error_type error_code)
{
  if(client_stream_error_cb != NULL)
    client_stream_error_cb(error_code);
}

void SensorCxt::handle_attrib_error_event(ssc_error_type error_code)
{
  if(client_attrib_error_cb != NULL)
    client_attrib_error_cb(error_code);
}

void SensorCxt::enableStreamingStatus(unsigned int handle)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  for(sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return;
  }
  (*sensorHandle)->enableStreamingStatus();
}

void SensorCxt::disableStreamingStatus(unsigned int handle)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  for(sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return;
  }
  (*sensorHandle)->disableStreamingStatus();
}

void SensorCxt::update_display_samples_cb(display_samples_cb ptr_reg_callback)
{
  ptr_display_samples_cb = ptr_reg_callback;
}

#ifdef SNS_SUPPORT_DIRECT_CHANNEL
direct_channel_type SensorCxt::get_channel_type(sensor_channel_type in)
{
  switch(in) {
  case STRUCTURED_MUX_CHANNEL:
    return DIRECT_CHANNEL_TYPE_STRUCTURED_MUX_CHANNEL;
  case GENERIC_CHANNEL:
    return DIRECT_CHANNEL_TYPE_GENERIC_CHANNEL;
  default:
    return DIRECT_CHANNEL_TYPE_STRUCTURED_MUX_CHANNEL;
  }
}

sns_std_client_processor SensorCxt::get_client_type(sensor_client_type in)
{
  switch(in) {
  case SSC:
    return SNS_STD_CLIENT_PROCESSOR_SSC;
  case APSS:
    return SNS_STD_CLIENT_PROCESSOR_APSS;
  case ADSP:
    return SNS_STD_CLIENT_PROCESSOR_ADSP;
  case MDSP:
    return SNS_STD_CLIENT_PROCESSOR_MDSP;
  case CDSP:
    return SNS_STD_CLIENT_PROCESSOR_CDSP;
  default:
    return SNS_STD_CLIENT_PROCESSOR_APSS;
  }
}

usta_rc SensorCxt::direct_channel_open(create_channel_info create_info, unsigned long &channel_handle)
{
  INFO_LOG(log_instance, "direct_channel_open Start ");
  sensor_direct_channel *new_channel = new sensor_direct_channel(
        create_info.shared_memory_ptr,
        create_info.shared_memory_size,
        get_channel_type(create_info.channel_type_value),
        get_client_type(create_info.client_type_value)
        );
  if(NULL == new_channel)
  {
    ERROR_LOG(log_instance, "Not able to establish the channel - NULL returned ");
    return USTA_RC_FAILED;
  }
  USTA_LOGI("sensor_direct_channel created is %px " , new_channel);
  direct_channel_instance_array.push_back(new_channel);
  channel_handle = sensors_timeutil::get_instance().qtimer_get_ticks();
  INFO_LOG(log_instance, "channel_handle " + to_string(channel_handle));
  direct_channel_handle_array.push_back(channel_handle);
  INFO_LOG(log_instance, "direct_channel_open End ");
  return USTA_RC_SUCCESS;
}


usta_rc SensorCxt::direct_channel_close(unsigned long channel_handle)
{
  INFO_LOG(log_instance, "direct_channel_close Start with channel_handle " + to_string(channel_handle));
  sensor_direct_channel *direct_channel_instance = get_dc_instance(channel_handle);
  if(direct_channel_instance == NULL)
  {
    ERROR_LOG(log_instance, "direct_channel_instance is NULL");
    return USTA_RC_FAILED;
  }

  delete direct_channel_instance;
  direct_channel_instance = NULL;

  INFO_LOG(log_instance, "direct_channel_close End ");
  return USTA_RC_SUCCESS;
}


usta_rc SensorCxt::direct_channel_update_offset_ts(unsigned long channel_handle, int64_t offset)
{
  INFO_LOG(log_instance, "direct_channel_update_offset_ts Start for channel handle " + to_string(channel_handle));

  sensor_direct_channel *direct_channel_instance = get_dc_instance(channel_handle);
  if(direct_channel_instance == NULL)
  {
    ERROR_LOG(log_instance, "direct_channel_instance is NULL");
    return USTA_RC_FAILED;
  }

  int ret = direct_channel_instance->update_offset(offset);
  if(ret == 0) {
    INFO_LOG(log_instance, "direct_channel_update_offset_ts End ");
    return USTA_RC_SUCCESS;
  } else {
    INFO_LOG(log_instance, "direct_channel_update_offset_ts update_offset failed ");
    return USTA_RC_FAILED;
  }
}
sensor_direct_channel* SensorCxt::get_dc_instance(unsigned long channel_handle)
{
  INFO_LOG(log_instance, "get_direct_channel_instance_from_channel_handle  " + to_string(channel_handle));
  unsigned int current_position = 0;
  for( ; current_position < direct_channel_handle_array.size(); current_position++)
  {
    INFO_LOG(log_instance, "current " + to_string(direct_channel_handle_array.at(current_position)) + " Required " + to_string(channel_handle));
    if(direct_channel_handle_array.at(current_position) == channel_handle) {
      INFO_LOG(log_instance, "found channel at position   " + to_string(current_position));
      break;
    }
  }
  if(current_position == direct_channel_handle_array.size()) {
    ERROR_LOG(log_instance, "Channel NOT FOUND ");
    return NULL;
  }

  unsigned int i = 0;
  list<sensor_direct_channel *>::iterator direct_channel_instance;
  for(direct_channel_instance = direct_channel_instance_array.begin(); direct_channel_instance != direct_channel_instance_array.end();
      direct_channel_instance++,i++ )
  {
    if (current_position==i) break;
  }
  if(i>=direct_channel_instance_array.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(current_position) + " exceding the Channel list size " + to_string((unsigned int)direct_channel_instance_array.size()) + "direct_channel_send_request failed" );
    return NULL;
  }
  return *direct_channel_instance;

}

usta_rc SensorCxt::direct_channel_send_request(unsigned long channel_handle, direct_channel_std_req_info std_req_info, send_req_msg_info sensor_payload_field)
{
  INFO_LOG(log_instance, "direct_channel_send_request Start with channel_handle " + to_string(channel_handle));

  sensor_direct_channel *direct_channel_instance = get_dc_instance(channel_handle);
  if(direct_channel_instance == NULL)
  {
    ERROR_LOG(log_instance, "direct_channel_instance is NULL");
    return USTA_RC_FAILED;
  }
  /*Formulate as per expectations */
  direct_channel_stream_info stream_info;
  INFO_LOG(log_instance, "direct_channel_send_request current sensorHandle " + to_string(std_req_info.sensor_handle));
  get_suid_info(std_req_info.sensor_handle , &stream_info);

  INFO_LOG(log_instance, "direct_channel_send_request get_suid_info Done with channel_handle " + to_string(channel_handle));

  if(!std_req_info.is_calibrated.empty()) {
    stream_info.has_calibrated = true;
    INFO_LOG(log_instance, "direct_channel_send_request has_calibrated True");
    if(std_req_info.is_calibrated.compare("true") == 0) {
      stream_info.is_calibrated = true;
      INFO_LOG(log_instance, "direct_channel_send_request is_calibrated True");
    }
    if(std_req_info.is_calibrated.compare("false") == 0) {
      stream_info.is_calibrated = false;
      INFO_LOG(log_instance, "direct_channel_send_request is_calibrated False");
    }
  }
  else
  {
    INFO_LOG(log_instance, "direct_channel_send_request has_calibrated false");
    stream_info.has_calibrated = false;
  }

  INFO_LOG(log_instance, "direct_channel_send_request is_calibrated Done with channel_handle " + to_string(channel_handle));

  if(!std_req_info.is_fixed_rate.empty()) {
    stream_info.has_fixed_rate = true;
    INFO_LOG(log_instance, "direct_channel_send_request has_fixed_rate True");
    if(std_req_info.is_fixed_rate.compare("true") == 0) {
      stream_info.is_resampled = true;
      INFO_LOG(log_instance, "direct_channel_send_request is_resampled True");
    }
    if(std_req_info.is_fixed_rate.compare("false") == 0) {
      stream_info.is_resampled = false;
      INFO_LOG(log_instance, "direct_channel_send_request is_resampled false");
    }
  }
  else
  {
    INFO_LOG(log_instance, "direct_channel_send_request has_fixed_rate True");
    stream_info.has_fixed_rate = false;
  }

  INFO_LOG(log_instance, "direct_channel_send_request is_fixed_rate Done with channel_handle " + to_string(channel_handle));
  sensor_direct_channel_srd_req_fileds std_req_fields_info;
  /*BATCH Period */
  if(!std_req_info.batch_period.empty())
  {
    std_req_fields_info.has_batch_period = true;
    std_req_fields_info.batch_period = stoi(std_req_info.batch_period);
  }
  else
  {
    std_req_fields_info.has_batch_period = false;
  }

  INFO_LOG(log_instance, "direct_channel_send_request batch_period Done with channel_handle " + to_string(channel_handle) + " and batch_period " + to_string(std_req_fields_info.batch_period));

  direct_channel_mux_ch_attrb attributes_info;
  attributes_info.sensor_handle = std_req_info.sensor_handle;
  attributes_info.sensor_type = get_sensor_type(std_req_info.sensor_handle , stream_info.is_calibrated);

  bool has_attributes_info = true;

  INFO_LOG(log_instance, "direct_channel_send_request  attributes_info Done with channel_handle " + to_string(channel_handle));

  string sensor_req_encoded_payload;
  get_payload_string(std_req_info.sensor_handle , sensor_payload_field, sensor_req_encoded_payload);


  INFO_LOG(log_instance, "direct_channel_send_request  Before send_request  with channel_handle " + to_string(channel_handle));

  int ret = direct_channel_instance->send_request(
      stoi(sensor_payload_field.msgid),
      stream_info,
      std_req_fields_info,
      attributes_info,
      has_attributes_info,
      sensor_req_encoded_payload
  );

  if(0 != ret) {
    ERROR_LOG(log_instance , "direct_channel_send_request - error in sending offest to DSP" );
    return USTA_RC_FAILED;
  }

  INFO_LOG(log_instance, "direct_channel_send_request End ");
  return USTA_RC_SUCCESS;
}

int SensorCxt::get_sensor_type(unsigned int handle, bool is_calibrated)
{

  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  for(sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_sensor_type failed" );
    return 0;
  }
  sensor_info sensor_info_var;
  (*sensorHandle)->get_sensor_info(sensor_info_var);

  if(sensor_info_var.data_type.compare("accel") == 0) {
      if(is_calibrated)
        return 1;
      else
        return 35;
  } else if (sensor_info_var.data_type.compare("gyro") == 0) {
    if(is_calibrated)
      return 4;
    else
      return 16;
  } else if (sensor_info_var.data_type.compare("mag") == 0) {
    if(is_calibrated)
      return 2;
    else
      return 14;
  }
  return 0;

}

void SensorCxt::get_payload_string(unsigned int handle , send_req_msg_info sensor_payload_field, string &encoded_data)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  for(sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_payload_string failed" );
  }
  string payload;
  (*sensorHandle)->get_payload_string(sensor_payload_field, payload);
  encoded_data.assign(payload);
}

usta_rc SensorCxt::direct_channel_delete_request(unsigned long channel_handle, direct_channel_delete_req_info delete_req_info)
{
  INFO_LOG(log_instance, "direct_channel_stop_request Start ");

  sensor_direct_channel *direct_channel_instance = get_dc_instance(channel_handle);
  if(direct_channel_instance == NULL)
  {
    ERROR_LOG(log_instance, "direct_channel_instance is NULL");
    return USTA_RC_FAILED;
  }


  direct_channel_stream_info stream_info;

  get_suid_info(delete_req_info.sensor_handle , &stream_info);

  if(!delete_req_info.is_calibrated.empty()) {
    stream_info.has_calibrated = true;
    if(delete_req_info.is_calibrated.compare("true") == 0)
      stream_info.is_calibrated = true;
    else if(delete_req_info.is_calibrated.compare("false") == 0)
      stream_info.is_calibrated = false;
  }
  else
  {
    stream_info.has_calibrated = false;
  }

  if(!delete_req_info.is_fixed_rate.empty()) {
    stream_info.has_fixed_rate = true;
    if(delete_req_info.is_fixed_rate.compare("true") == 0)
      stream_info.is_resampled = true;
    if(delete_req_info.is_fixed_rate.compare("false") == 0)
      stream_info.is_resampled = false;
  }
  else
  {
    stream_info.has_fixed_rate = false;
  }

  int ret = direct_channel_instance->stop_request(stream_info);
  if(0 != ret) {
    ERROR_LOG(log_instance , "direct_channel_update_offset_ts - error in sending offest to DSP" );
    return USTA_RC_FAILED;
  }

  INFO_LOG(log_instance, "direct_channel_stop_request End ");
  return USTA_RC_SUCCESS;
}

void SensorCxt::get_suid_info(unsigned int handle , direct_channel_stream_info *stream_info_out)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  for(sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return;
  }
  sensor_info sensor_info_var;
  (*sensorHandle)->get_sensor_info(sensor_info_var);

  unsigned long suid_low;
  std::istringstream(sensor_info_var.suid_low) >> std::hex >> suid_low;

  unsigned long suid_high;
  std::istringstream(sensor_info_var.suid_high) >> std::hex >> suid_high;

  stream_info_out->suid_high = suid_high;
  stream_info_out->suid_low = suid_low;

}
#endif
void SensorCxt::get_direct_channel_atttributes_info(
    direct_channel_attributes_info& out_attributes_info
    )
{
  out_attributes_info.max_sampling_rate = attributes_info.max_sampling_rate;
  out_attributes_info.max_report_rate = attributes_info.max_report_rate;
}

