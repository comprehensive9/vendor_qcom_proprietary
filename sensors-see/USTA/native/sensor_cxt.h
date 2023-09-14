/*============================================================================
  Copyright (c) 2017, 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_cxt.h
  @brief
  SensorCxt class definition.
============================================================================*/
#pragma once
#include"sensor_suid.h"
#include "sensor_common.h"
#include "sensor.h"
#include "sensor_descriptor_pool.h"
#include "logging_util.h"
#include <list>
#ifdef SNS_SUPPORT_DIRECT_CHANNEL
#include "sensor_direct_channel.h"
#endif

typedef void (*display_samples_cb)(string display_samples, bool is_registry_sensor);

typedef enum sensor_stream_path{
  REGULAR = 0,
  DIRECT_CHANNEL
}sensor_stream_path;

#ifdef SNS_SUPPORT_DIRECT_CHANNEL
typedef enum sensor_channel_type {
   STRUCTURED_MUX_CHANNEL = 0,
   GENERIC_CHANNEL
}sensor_channel_type;

typedef enum sensor_client_type {
  SSC = 0,
  APSS,
  ADSP,
  MDSP,
  CDSP
}sensor_client_type;

typedef struct create_channel_info {
  unsigned long shared_memory_ptr;
  unsigned long shared_memory_size;
  sensor_channel_type channel_type_value;
  sensor_client_type client_type_value;
}create_channel_info;

typedef struct direct_channel_std_req_info
{
  unsigned int sensor_handle;
  string is_calibrated;
  string is_fixed_rate;
  string sensor_type;
  string batch_period;
#if 0
  string flush_period;
  string flush_only;
  string is_max_batch;
  string is_passive;
#endif
}direct_channel_std_req_info;

typedef struct direct_channel_delete_req_info
{
  unsigned int sensor_handle;
  string is_calibrated;
  string is_fixed_rate;
}direct_channel_delete_req_info;

#endif
/**
 * @brief singleton class to manage creation of sensor objects and handling
 *        the request from the clients
 *
 */
class SensorCxt {
public:
  ~SensorCxt();
  /**
   * @brief returns the SensorCxt instance. SensorCxt being singleton, all
   *        objects share common  instance
   */
  static SensorCxt* getInstance(sensor_stream_path stream_path = REGULAR);
  /**
   * @brief gives the list of sensors supported by SSC in predefined format
   *
   * @param sensor_list is string reference provided by the client which
   *        would be populated with the sensor list in format as below:
   *        "<datatype sensor 0>-<vendor sensor 0>:
   *        <suid_low senor 0>:
   *        <suid_high sensor 0>,
   *
   *        <details of sensor 1>,
   *        <details of sensor 2>,
   *        ..."
   *        The order of the list becomes the handle which SensorCxt will
   *        understand to identify a sensor
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc get_sensor_list(vector<sensor_info>& sensor_list);
  /**
   * @brief gives the request messages in a predefined format for all the
   *         request messages supported by a sensor
   *
   * @param handle is the unique identifier to correspond to a sensor whose
   *        request messages are desired
   *
   * @param request_vector is an empty vector reference of string provided
   *        by the client. This empty vector will be populated with all the
   *        request messages supported by a sensor corresponding to provided
   *        handle. The request_vector format will be:
   *
   *        “<msgName>:<msgID>{Name:<nameValue>;DataType:<datatypeValue>;
   *                           Label:<labelValue>;Default:<defaultValue>;
   *                            EnumValue:<enumValue1>,..;}
   *                          { <details of field 1> }
   *                          { <details of field 2> }”
   *        " <details of next message>"
   *        " <details of next message>"
   *        ...
   *
   *        msgName       : Message Name as defined in proto file
   *        msgID         : Message Id corresponding to this message as defined
   *                        in proto file
   *        nameValue     : Name of field as present in the message
   *        datatypeValue : Data type of field : int32,uint32,uint64,double,float,
   *                        bool,enum,string,message
   *        labelValue    : label of the field : required/optional/repeated
   *        EnumValue<i>  : list of comma separate possible enum values, only if
   *                        datatypeValue is enum
   *
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc get_request_list(unsigned int handle,vector<msg_info> &request_msgs);
  /**
   * @brief sends a request to a sensor in predefined request message format
   *
   * @param handle is the unique identifier to correspond to a sensor for whom
   *        the request is intended
   *
   * @param batch_period corresponds to batch period for streaming request
   *
   * @param flush_period corresponds to the flush period of streaming request
   *
   * @param client_type is the client triggering this request. one of :
   *        SNS_STD_PROCESSOR_SSC
   *        SNS_STD_PROCESSOR_APSS
   *        SNS_STD_PROCESSOR_ADSP
   *        SNS_STD_PROCESSOR_MDSP
   *
   * @param wakeup_type is a string triggering by the client. one of :
   *        SNS_STD_DELIVERY_WAKEUP
   *        SNS_STD_DELIVERY_NO_WAKEUP
   *        SNS_STD_DELIVERY_FLUSH
   *
   * @param logfile_name is the client supplied file name. The name of file
   *        will be:
   *        <datatype>-<vendor>-<logfile_name>.txt
   *        if logfile_name is empty, the file will be <datatype>-<vendor>.txt
   *
   * @param request_msg is the request message sent by the client in predefined
   *        format. The format is as follows
   *        "<msgid>:<field_value 1>:<field_value 2> ..."
   *        Note: no fields should be missing,
   *              optional fields must have some default values
   *              if a field is repeated, the number of times its repeated
   *              should preceed its values
   *              for eg.. if field_value 2 is repeated 3 times, the request
   *              msg  will be:
   *               "<msgid>:<field_value 1>
   *                       :3
   *                       :<1st fv 2>
   *                       :<2nd fv 2>
   *                       :<3rd fv 2>
   *                       :<filed_value 3>"

   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc send_request(unsigned int handle,send_req_msg_info& request_msg,client_req_msg_fileds &std_fields_data,string& logfile_name , string usta_mode = "Unknown_Mode");
  /**
   * @brief sends stop request to a sensor . This can be used to stop
   *        streaming of a sensor
   *
   * @param handle is the unique identifier to correspond to a sensor for whom
   *        this command is intended
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc stop_request(unsigned int handle, bool is_qmi_disable_api_enabled = false);
  /**
   * @brief gets the complete list of attributes of a particular sensor
   *
   * @param handle is the unique identifier to correspond to a sensor from whom
   *        the attribute is request
   *
   * @param attributes is string reference provided by the client which would
   *        be populated with the attributes in json format
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc get_attributes(unsigned int handle,string& attributes);
  /**
   * @brief removes a list of sensors from SensorCxt
   *
   * @param remove_sensor_listis a list of sensor handles to be removed
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc remove_sensors(vector<int>& remove_sensor_list);

  void enableStreamingStatus(unsigned int sensor_handle);
  void disableStreamingStatus(unsigned int sensor_handle);
  void update_display_samples_cb(display_samples_cb ptr_reg_callback);
  void register_qmi_error_cb(unsigned int sensorHandle, handle_stream_error_func client_error_func , usta_error_cb_type connection_type);

#ifdef SNS_SUPPORT_DIRECT_CHANNEL
  /**
   * @brief direct_channel_open is used to create the channel
   *
   * @param create_channel_info is the set of parameters used for creating the channel
   *        create_channel_info consists of
   *
   * @param channel_handle is the out param for this method contains the channel number
   *        Returned from DSP

   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc direct_channel_open(create_channel_info create_info, unsigned long &channel_handle);

  /**
   * @brief direct_channel_close is used to remove the channel
   *
   * @param Channel_handle which supposed to remove
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc direct_channel_close(unsigned long channel_handle);

  /**
   * @brief direct_channel_update_offset_ts is used to update the
   *        offset time between Android and DSP on need basis
   *
   * @param Time stamp offset value in nsecs between Apps and DSP
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc direct_channel_update_offset_ts(unsigned long channel_handle, int64_t offset);

  /**
   * @brief direct_channel_send_request is used to send the
   *        stream request per sensor on a existing channel
   *
   * @param channel_handle is existing channel handle details
   *        on which stream supposed to start
   *
   * @param std_req_info is set of parameters to start stream request
   *
   * @param sensor_payload_field contains the sensor
   *        request messages specific fields.
   *        This data is normal values only.
   *        Not in the encoded form.
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc direct_channel_send_request(unsigned long channel_handle, direct_channel_std_req_info std_req_info, send_req_msg_info sensor_payload_field);

  /**
   * @brief direct_channel_delete_request is used to stop the
   *        stream per sensor on a existing channel
   *
   * @param delete_req_info consists of parameters to delete stream request
   *
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc direct_channel_delete_request(unsigned long channel_handle, direct_channel_delete_req_info delete_req_info);
#endif
  void get_direct_channel_atttributes_info(direct_channel_attributes_info& out_attributes_info);
private:
  /* only one SensorsContext */
  static SensorCxt* self;
  SensorCxt(sensor_stream_path in_stream_path);
  list<Sensor *> mSensors;
  SuidSensor* suid_instance;
  pthread_cond_t      cb_cond;
  bool                is_resp_arrived;
  pthread_mutex_t     cb_mutex;
  pthread_mutex_t     attr_count_mutex;
  handle_suid_event_cb  suid_event_cb;
  handle_attribute_event_func attribute_event_cb;
  handle_stream_error_func stream_error_cb;
  handle_stream_error_func attrib_error_cb;

  int  num_sensors;
  int  num_attr_received;

  void handle_suid_event(uint64_t& suid_low,uint64_t& suid_high,
      bool is_last_suid);
  void handle_attribute_event();
  void handle_stream_event(string inString , bool is_registry_sensor);
  handle_stream_event_func stream_event_cb;
  void handle_stream_error_event(ssc_error_type error_code);
  void handle_attrib_error_event(ssc_error_type error_code);
  handle_stream_error_func client_stream_error_cb;
  handle_stream_error_func client_attrib_error_cb;
  bool is_all_attributes_received;
  usta_logging_util *log_instance;
  display_samples_cb ptr_display_samples_cb;

#ifdef SNS_SUPPORT_DIRECT_CHANNEL
  /*Private variables and methods for direct channel*/
  list<sensor_direct_channel *> direct_channel_instance_array;
  vector<unsigned long> direct_channel_handle_array;
  direct_channel_type get_channel_type(sensor_channel_type in);
  sns_std_client_processor get_client_type(sensor_client_type in);
  void get_suid_info(unsigned int handle , direct_channel_stream_info *stream_info_out);
  void get_payload_string(unsigned int handle , send_req_msg_info sensor_payload_field, string &encoded_data);
  int get_sensor_type(unsigned int handle, bool is_calibrated = false);
  sensor_direct_channel* get_dc_instance(unsigned long channel_handle);
#endif
  sensor_stream_path stream_path;
  direct_channel_attributes_info attributes_info;
};



