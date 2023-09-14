/* ===================================================================
** Copyright (c) 2017-2019, 2021 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_salt.h
** DESC: Sensor Abstaction Layer for Testing
** ================================================================ */
#pragma once

#include <map>
#include <vector>

// including from bottom up
#include "see_salt_rc.h"
#include "see_salt_sensor.h"
#include "see_salt_payloads.h"
#include "see_salt_request.h"
#include "see_salt_dc.h"

typedef void (*event_cb_func)(std::string display_samples, bool is_registry_sensor);

/**
 * @brief sleep_and_awake() is called to sleep for milliseconds, then if
 *        necessary, wakeup the apps processor from suspend mode.
 * @param milliseconds
 */
void sleep_and_awake( uint32_t milliseconds); // see_salt_sleep.cpp

/**
 * @brief by default, USTA logging is enabled.
 * @param enabled: true == logging enabled, false == logging disabled.
 * @note can be invoked before see_salt::get_instance(). Affects logging to both
 *       logcat and /mnt/sdcard
 */
void set_usta_logging( bool enable);

/**
 * @brief: class see_salt
 *    SEE: Sensor Execution Environment
 *    SALT: Sensor Abstraction Layer Test
 */
class see_salt {
public:
   ~see_salt();
   /**
    * @brief returns the see_salt instance.
    *
    * @return see_salt*
    */
   static see_salt* get_instance(see_sensor_stream_path_e stream_path_e = REGULAR_PATH);

   /**
    * @brief - populate vector with all public sensor types, where
    * sensor_types :: 'accel', 'gyro', 'mag', 'motion_detect', 'gravity', ...
    *
    * @param [io] sensor_types vector gets ALL public sensor types
   */
   void get_sensors(std::vector<std::string> &sensor_types);

   /**
    * @brief - populate vector with all suids for the given sensor type
    *
    * @param [i] sensor_type of interest
    * @param [io] suids vector gets sens_uids for input sensor_type
   */
   void get_sensors(std::string sensor_type,
                    std::vector<sens_uid *> &suids);

   /**
    * @brief - populate vector with all public suids
    *
    * @param [io] suids vector gets pointers to all public sens_uids
    */
   void get_sensors(std::vector<sens_uid *> &);

   /**
    * @brief - send see_client_request_message to sensors core
    * @param [i] request
    * @return int 0 == successful
    */
   salt_rc send_request(see_client_request_message &request);

   /**
    * @brief - delete stream or disconnect qmi
    * @param [i] delete_or_qmi_disconnect: true == delete
    * @return int 0 == successful
    */
   salt_rc stop_request(sens_uid *suid,
                        bool delete_or_disconnect);

   /**
    * @brief - register event_cb function
    * @param [i] pointer to suid
    * @param [i] pointer to event_cb function
    */
   void update_event_cb(sens_uid *suid, event_cb_func event_cb);

   /**
    * @brief - sleep for duration
    * @param [i] duration in seconds
    */
   void sleep(float duration);

#ifdef SNS_SUPPORT_DIRECT_CHANNEL
   /**
    * @brief - create DRM channel
    * @param channel info
    * @param channel handle
    * @return int 0 == successful
    */
   salt_rc create_channel_dc(see_create_dc_channel_info &dc_channel_info, unsigned long &channel_handle);

   /**
    * @brief - close DRM channel
    * @param channel handle
    * @return int 0 == successful
    */
   salt_rc close_channel_dc(unsigned long &channel_handle);

   /**
    * @brief - send direct channel request
    * @param channel andle
    * @param request info
    * @param false: start request, true: stop request
    * @return int 0 == successful
    */
   salt_rc send_request_dc(unsigned long &channel_handle, see_dc_std_req_info &req_info, bool is_delete_request_info);

   /**
    * @brief - update direct channel offset
    * @param channel handle
    * @param offset
    * @return int 0 == successful
    */
   salt_rc update_offset_ts_dc(unsigned long &channel_handle, int64_t offset);
#endif

   /**
    * @brief - get direct channel sensor attributes
    * @param direct channel attributes structure
    */
   void get_direct_channel_attributes(direct_channel_attributes &dc_attributes);

   void append_sensor(sensor &);
   int begin( int argc, char *argv[]); //deprecated
   int begin();
   sensor *get_sensor(sens_uid *suid);
   int suid_to_handle(sens_uid *target);
   void set_debugging(bool debug) { _debug = debug;}
   bool get_debugging() { return _debug;}
   int get_instance_num() { return _salt_instance_num; }
   see_sensor_stream_path_e get_stream_path() { return _stream_path_e; }
   std::vector<sensor> _sensor_list;

private:
   bool _debug = false;
   int _salt_instance_num;
   see_sensor_stream_path_e _stream_path_e = REGULAR_PATH;
   see_salt(see_sensor_stream_path_e stream_path_e = REGULAR_PATH);
};
