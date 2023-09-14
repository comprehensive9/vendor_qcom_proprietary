/*===========================================================================

Copyright (c) 2021-2022, Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifdef SNS_WEARABLES_TARGET_BG

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <vector>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <cutils/properties.h>
#include "ssc_connection.h"
#include "worker.h"
#include "sensors_log.h"
#include "sns_suid.pb.h"
#include "ssc_utils.h"
#include "sns_registry.pb.h"
#include <vendor/google_clockwork/wristorientation/1.0/IWristOrientation.h>
#include <vendor/google_clockwork/wristorientation/1.0/types.h>
#include <vendor/google_clockwork/wristorientation/1.0/IWristOrientationCallback.h>

using ::vendor::google_clockwork::wristorientation::V1_0::IWristOrientation;
using ::vendor::google_clockwork::wristorientation::V1_0::Orientation;
using ::vendor::google_clockwork::wristorientation::V1_0::IWristOrientationCallback;
using ::android::sp;
using ::android::hardware::Return;
using ::android::hardware::Void;
using wo_read_event_cb = std::function<void(sns_registry_data reg_data, const std::string name)>;

class WristOrientation : public IWristOrientationCallback{
  private:
    /*  Requests for registry suid  */
    void request_registry_suid();

    /*  Handles registry suid event and
        assigns registry suid to a member variable  */
    void handle_registry_suid_event(const uint8_t* data , int size);

    /*  Sends request to registry for fetching the
        Contents of the regsitry item group requested
    */
    void registry_read_request(string filename);

    /*  Handles Response from registry for the registry
        read request sent for regsitry item groups
    */
    void handle_registry_read_event(const uint8_t* data , int size);

    /*  Get the handler corresponding to registry items  */
    wo_read_event_cb get_handler(const char* type);

    /*  Write request to SEE registry sensor  */
    static void send_registry_write_request(sns_registry_write_req pb_stream_registry_data_item);

    void handle_registry_write_event(const uint8_t* data, int size);

    /*  Updates the bias values received from algos
        like sns_gyro_cal, sns_mag_cal etc.
    */
    static void update_algo_bias(sns_registry_data reg_data, const std::string name);

    /*  Update bias values of driver registry items like
        lsm6dso and ak0991x biases, correlation matrix etc.
    */
    static void update_driver_bias(sns_registry_data reg_data, const std::string name);

    /*  Update compensation matrix values of sns_gyro_cal  */
    static void update_compensation_matrix(sns_registry_data reg_data, const std::string name);

    /*  Update correlation matrix values of drivers  */
    static void update_corr_matrix(sns_registry_data reg_data, const std::string name);

    /*  Update the orientation of axes from
        +axis to -axis and vice-versa for x and y
    */
    static void update_orientation(sns_registry_data reg_data, const std::string name);

    /*  Delete ssc_connection objects  */
    void clean_up();

    /*  Error handling if something goes wrong
        while registry files update
    */
    void start_wrist_recovery_timer();

    void wrist_handle_system_reset();

    void create_in_progress_file();

    static void handle_timer_expiry(union sigval param);

    /*  Checks if the registry files are consistent with current wrist mode  */
    int check_reg_files_consistency(string registry_file_path, string copy_file_path);

    /*  As part of error handling, create a copy of
        imu.orient and mag.orient files
    */
    void create_reg_group_golden_copy(string registry_path, string copy_path);

    /*  Check if registry is up on ADSP  */
    bool is_adsp_registry_up();

    /*  Checks if registry files are consistent with current wrist mode of the system  */
    void validate_mode_reg_consistency();

    /*  Update the signs of golden copy of orient file if the mode is 1 or 3  */
    int update_golden_orient_files(string filename);

  public:
    WristOrientation();
    ~WristOrientation();

    /*  This function is triggered from wrist orientation HAL
        when a new mode is selected by user.
        And updates various registry groups of sign flip of data values
    */
    void wrist_mode_change();

    /*  Callback function of wrist orientation HAL
        Notifies when user changes the mode
    */
    Return<void> onOrientationChange(const Orientation orientation)
    {
      sns_logi("%s : recieved the onOrientationChange callback %d \n", __func__, static_cast<int>(orientation));
      bool no_action = false;
      switch (orientation)
      {
        case Orientation::LEFT_WRIST_ROTATION_0:
        {
          if(currentOrientation == Orientation::RIGHT_WRIST_ROTATION_0)
          {
            no_action = true;
          }
          currentOrientation = Orientation::LEFT_WRIST_ROTATION_0;
        }
        break;
        case Orientation::LEFT_WRIST_ROTATION_180:
        {
          if(currentOrientation == Orientation::RIGHT_WRIST_ROTATION_180)
          {
            no_action = true;
          }
          currentOrientation = Orientation::LEFT_WRIST_ROTATION_180;
        }
        break;
        case Orientation::RIGHT_WRIST_ROTATION_0:
        {
          if(currentOrientation == Orientation::LEFT_WRIST_ROTATION_0)
          {
            no_action = true;
          }
          currentOrientation = Orientation::RIGHT_WRIST_ROTATION_0;
        }
        break;
        case Orientation::RIGHT_WRIST_ROTATION_180:
        {
          if(currentOrientation == Orientation::LEFT_WRIST_ROTATION_180)
          {
            no_action = true;
          }
          currentOrientation = Orientation::RIGHT_WRIST_ROTATION_180;
        }
        break;
      }
      if(no_action == false)
      {
        wrist_mode_change();
      }

      return Void();
    }

  private:
    Orientation currentOrientation;
    char orientation_prop[PROPERTY_VALUE_MAX];
    worker _worker;
    ssc_event_cb registry_suid_cb;
    ssc_connection *registry_read_obj;
    ssc_event_cb registry_read_cb;
    ssc_connection *registry_suid_obj;
    ssc_event_cb registry_write_cb;
    std::vector<std::pair<std::string, wo_read_event_cb>> file_list;
    std::mutex _mutex;
    std::atomic<bool> _registry_resp_flag;

    static sensor_uid registry_suid;
    static ssc_connection *registry_write_obj;
    static timer_t wrist_timer;
};

bool wrist_orientation_init();

#endif
