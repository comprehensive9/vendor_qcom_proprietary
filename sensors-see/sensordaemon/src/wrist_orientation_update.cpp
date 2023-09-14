/*===========================================================================

Copyright (c) 2021-2022, Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifdef SNS_WEARABLES_TARGET_BG

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <stdlib.h>
#include <mutex>
#include <errno.h>
#include <chrono>
#include <filesystem>
#include <cutils/properties.h>
#include <time.h>
#include <signal.h>
#include <dirent.h>
#include <thread>
#include "sns_client.pb.h"
#include "sns_suid.pb.h"
#include "sns_registry.pb.h"
#include "wrist_orientation_update.h"
#include "ssc_utils.h"

#define MAX_FILENAME 150
#define MAX_FILE_SIZE 500
#define BIAS_VALUES 3
#define COMPENSATION_MATRIX 9
#define AXES 3
#define CORR_MAT 9
#define SIGN_FLIP(val) ((val == 0.00f) ? val : -val)

#define REGISTRY_SUID_TIMEOUT_MS 10
#define REGISTRY_SUID_TIMEOUT_MAX_RETRY 1000

#define REGISTRY_RESP_TIMEOUT_MS 10
#define REGISTRY_RESP_TIMEOUT_MAX_RETRY 1000

using string = std::string;
using stringstream = std::stringstream;

static const char* group_list_file_path = "/vendor/etc/sensors/sns_wear_wo_reg_group_list";
static string see_reg_group_path = "/mnt/vendor/persist/sensors/registry/registry/";
static string mnt_vendor_path = "/mnt/vendor/persist/sensors/";
static const char* in_progress_file = "/mnt/vendor/persist/sensors/reg_update_in_progress.txt";

ssc_connection *WristOrientation::registry_write_obj = nullptr;
sensor_uid WristOrientation::registry_suid = { 0, 0 };
timer_t WristOrientation::wrist_timer = nullptr;

WristOrientation::WristOrientation()
{
  _worker.setname("ssc_wrist_orientation");

  registry_read_obj = nullptr;
  registry_suid_obj = nullptr;
  registry_read_cb = nullptr;
  registry_write_cb = nullptr;

  // Fetch current value of orientation
  property_get("persist.sys.wrist_orientation", orientation_prop, "");
  sns_logd("%s: property received = %d", __func__, atoi(orientation_prop));

  currentOrientation = static_cast<Orientation>(atoi(orientation_prop));
  sns_logi("%s: Current Orientation = %hhu", __func__, currentOrientation);

  if(is_adsp_registry_up() == false)
  {
    sns_loge("%s: registry is not up even after 5 sec", __func__);
    return;
  }

  validate_mode_reg_consistency();
}

WristOrientation::~WristOrientation()
{
  // Delete ssc_connection objects
  clean_up();
}

void WristOrientation::create_reg_group_golden_copy(
                   string registry_file, string golden_file)
{
  struct stat orient;

  if(0 == stat(registry_file.c_str(), &orient))
  {
    sns_logd("%s: Copying file from %s to %s", __func__,
              registry_file.c_str(), golden_file.c_str());
    int fd_default = open(registry_file.c_str(), O_RDONLY);
    int fd_golden = open(golden_file.c_str(), O_WRONLY | O_CREAT, 0644);
    if((fd_default < 0) || (fd_golden < 0))
    {
      sns_loge("failed to open file with error = %d, %s", errno, strerror(errno));
    }
    else
    {
      int bytes;
      if((bytes = sendfile(fd_golden, fd_default, 0, MAX_FILE_SIZE)) < 0)
      {
        sns_loge("failed to create a copy of the file %s with error = %d, %s",
                         registry_file.c_str(), errno, strerror(errno));
      }
      else
      {
        sns_logi("successfully created copy of the file %s at %s, bytes transferred = %d",
                           registry_file.c_str(), golden_file.c_str(), bytes);

        /*  Scenario:Current mode of the system 1 or 3 and golden copy
            is generated for the first time, golden copy will have sign
            flipped values corresponding to mode 1 or 3. Golden copy
            should be in mode 0 as mode 0 is the default mode
        */
        if((int)currentOrientation == 1 || (int)currentOrientation == 3)
        {
          if(update_golden_orient_files(golden_file) == -1)
          {
            sns_loge("Copy of orient files not in sync with current mode. Removing");

            //  If the golden copy doesn't hold correct values, remove the file
            if(remove(golden_file.c_str()) != 0)
            {
              sns_loge("failed to remove %s with error %s",
                            golden_file.c_str(), strerror(errno));
            }
            else
            {
              sns_logd("%s removed successfully", golden_file.c_str());
            }
          }
        }
      }
      close(fd_golden);
      close(fd_default);
    }
  }
}

int WristOrientation::check_reg_files_consistency(
                               string registry_file_path, string copy_file_path)
{
  ifstream registry_file;
  ifstream golden_file;
  string reg_file_str, copy_file_str;
  std::vector<char> reg_orient_values, copy_orient_values;
  string cmp_str = "data";
  bool files_in_sync = true;
  size_t pos_reg, pos_copy;

  registry_file.open(registry_file_path);
  golden_file.open(copy_file_path);
  if(registry_file.fail() || golden_file.fail())
  {
    sns_loge("%s: not able to open file %s or %s", __func__,
                   registry_file_path.c_str(), copy_file_path.c_str());
    return -1;
  }

  //  Read the copy and current registry file
  std::getline(registry_file, reg_file_str);
  std::getline(golden_file, copy_file_str);

  registry_file.close();
  golden_file.close();

  //  Read the orientation values x,y,z of registry and golden copy file
  pos_reg = reg_file_str.find(cmp_str, 0);
  pos_copy = copy_file_str.find(cmp_str, 0);
  while(pos_reg != string::npos && pos_copy != string::npos)
  {
    if((pos_reg + 8) < reg_file_str.length() && (pos_copy + 8) < copy_file_str.length())
    {
      reg_orient_values.push_back(reg_file_str[pos_reg + 7]);
      copy_orient_values.push_back(copy_file_str[pos_copy + 7]);
      sns_logd("pushing string %hhu to reg vector", reg_file_str[pos_reg + 7]);
      sns_logd("pushing string %hhu to copy vector", copy_file_str[pos_copy + 7]);
      pos_reg = reg_file_str.find(cmp_str, pos_reg + 9);
      pos_copy = copy_file_str.find(cmp_str, pos_copy + 9);
    }
    else
    {
      sns_loge("%s or %s parsed incorrectly", reg_file_str.c_str(), copy_file_str.c_str());
      break;
    }
  }

  /*  For mode 0 and 2:
      x and y orientation should be the same for copy and current registry file
      For mode 1 and 3:
      x and y orientation should have different signs for copy and current registry file
  */
  bool ret = std::equal(reg_orient_values.begin(), reg_orient_values.begin() + 2,
                          copy_orient_values.begin());

  if(ret == false && ((int)currentOrientation == 0 || (int)currentOrientation == 2))
  {
    files_in_sync = false;
  }
  else if(ret == true && ((int)currentOrientation == 1 || (int)currentOrientation == 3))
  {
    files_in_sync = false;
  }

  if(files_in_sync == false)
  {
    wrist_mode_change();
  }

  return files_in_sync;
}

void WristOrientation::validate_mode_reg_consistency()
{
  struct stat stat_in_progress_file;
  if(!stat(in_progress_file, &stat_in_progress_file))
  {
    sns_logi("%s: looks like system reset while orient change in progress, doing recovery", __func__);
    sns_logd("%s: creating timer for removing in_progress file", __func__);
    wrist_handle_system_reset();
  }
  else
  {
    struct stat stat_buf;
    char filename[MAX_FILENAME];
    FILE *fp = nullptr;

    if(0 == stat(group_list_file_path, &stat_buf))
    {
      fp = fopen(group_list_file_path, "r");
      if(!fp)
      {
        sns_loge("%s: Not able to open the file %s", __func__, group_list_file_path);
        return;
      }

      while(!feof(fp))
      {
        if(fgets(filename, MAX_FILENAME, fp))
        {
          stringstream input(filename);
          string name;
          input >> name;
          string handler_type;
          input >> handler_type;
          sns_logd("%s: checking if file %s exists", __func__, name.c_str());

          struct stat stat_buf_file;
          string default_orient_path = see_reg_group_path + name + '\0';
          sns_logd("%s: file to be opened %s and handler name = %s",__func__,
                               default_orient_path.c_str(), handler_type.c_str());

          if(handler_type == "ORIENT")
          {
            // Checks if registry group exists
            if(0 == stat(default_orient_path.c_str(), &stat_buf_file))
            {
              string copy_orient_path = mnt_vendor_path + "wrist_" + name + '\0';
              struct stat orient_copy;
              bool files_in_sync = true;

              /*  Create a copy of mag and imu orient files for first boot
                  On latter boots, compare the copy file with the current file
                  and do error handling accordingly (if required)
              */
              if(stat(copy_orient_path.c_str(), &orient_copy))
              {
                create_reg_group_golden_copy(default_orient_path, copy_orient_path);
              }
              else
              {
                files_in_sync = check_reg_files_consistency(default_orient_path, copy_orient_path);

                /*  If files are inconsistent from previous comparison,
                    then error handling is already triggered.
                    No need to do it again.
                */
                if(files_in_sync == false)
                {
                  break;
                }
              }
            }
            else
            {
              sns_loge("%s: unable to stat registry path %s", __func__, default_orient_path.c_str());
            }
          }
        }
      }
      fclose(fp);
    }
  }
}

int WristOrientation::update_golden_orient_files(string filename)
{
  ifstream file_read;
  ofstream file_write;
  string file_str;
  string cmp_str = "data";
  size_t pos;

  file_read.open(filename);
  if(file_read.fail())
  {
    sns_loge("not able to open file %s", filename.c_str());
    return -1;
  }

  //  Read the registry file
  std::getline(file_read, file_str);

  file_read.close();

  //  Update orientation values x,y of registry file
  int val = 0;
  pos = file_str.find(cmp_str, 0);
  while(pos != string::npos && val < 2)
  {
    if((pos + 7) < file_str.length())
    {
      string str = "";
      file_str[pos + 7] = file_str[pos + 7] == '+' ? '-' : '+';
      pos = file_str.find(cmp_str, pos + 9);
      val++;
    }
    else
    {
      sns_loge("%s parsed incorrectly", file_str.c_str());
      break;
    }
  }

  file_write.open(filename);
  if(file_write.fail())
  {
    sns_loge("not able to open file %s", filename.c_str());
    return -1;
  }

  //  Update the signs of orient files
  file_write << file_str;
  sns_logd("%s updated", filename.c_str());

  file_write.close();
  return 0;
}

void WristOrientation::wrist_mode_change()
{
  sns_logi("Inside %s", __func__);

  if(is_adsp_registry_up() == false)
  {
    sns_loge("%s: registry is not up after adsp ssr", __func__);
    return;
  }

  /*  Scenraio: Back to back wrist orient mode change  */
  struct stat stat_progress;
  if(!stat(in_progress_file, &stat_progress))
  {
    if(wrist_timer != nullptr)
    {
      timer_delete(wrist_timer);
      wrist_timer = nullptr;
    }
  }
  else
  {
    create_in_progress_file();
  }

  _worker.add_task(NULL, [this]
  {
    char filename[MAX_FILENAME];
    FILE *fp = nullptr;
    bool timeout;
    int32_t max_retry;

    struct stat stat_buf;
    if(0 == stat(group_list_file_path, &stat_buf))
    {
      fp = fopen(group_list_file_path, "r");
      if(!fp)
      {
        sns_loge("%s: Not able to open the file %s", __func__, group_list_file_path);
        return;
      }

      /* Create ssc_connection object for
         sending registry read requests
         and handling registry read events  */
      registry_read_cb = [this](const uint8_t* data , int size)
      {
        this->handle_registry_read_event(data, size);
      };

      if (NULL == (registry_read_obj
             = new ssc_connection(registry_read_cb)))
      {
        sns_loge("ssc connection failed for registry_read_cb");
        return;
      }

      /* Create ssc_connection object for
         sending registry write requests
         and handling registry write events  */
      registry_write_cb = [this](const uint8_t* data , int size)
      {
        this->handle_registry_write_event(data, size);
      };

      if (NULL == (WristOrientation::registry_write_obj
                 = new ssc_connection(registry_write_cb)))
      {
        sns_loge("ssc connection failed for registry write cb");
        // delete registry read connection object
        clean_up();
        return;
      }

      /*  Scan group list file,
          read each group data from SEE
          and do sign flip,
          write group data to persist location using SEE
      */
      while(!feof(fp))
      {
        if(fgets(filename, MAX_FILENAME, fp))
        {
          stringstream input(filename);
          string name;
          input >> name;
          string handler_type;
          input >> handler_type;
          sns_logd("%s: checking if file %s exists", __func__, name.c_str());

          struct stat stat_buf_file;
          string reg_file = see_reg_group_path + name + '\0';
          sns_logd("%s: file to be opened %s and handler name = %s",__func__,
                               reg_file.c_str(), handler_type.c_str());

          // Checks if registry group exists
          if(0 == stat(reg_file.c_str(), &stat_buf_file))
          {
            sns_logi("%s: opening file %s", __func__, name.c_str());
            name += '\0';
            auto handler_name =  get_handler(handler_type.c_str());
            if(handler_name != nullptr)
            {
              file_list.push_back(make_pair(name, handler_name));
              registry_read_request(name);

              timeout = true;
              max_retry = REGISTRY_RESP_TIMEOUT_MAX_RETRY;
              do
              {
                if(_registry_resp_flag.load())
                {
                  _registry_resp_flag.store(false);
                  timeout = false;
                  break;
                }
                else
                {
                  std::this_thread::sleep_for(std::chrono::milliseconds(REGISTRY_RESP_TIMEOUT_MS));
                }
                max_retry--;
              }while(max_retry > 0);

              if(timeout)
              {
                // Delete registry read and write connection objects
                clean_up();
                sns_loge("%s: timed out waiting for read/write event for %s",
                                       __func__, name.c_str());
                return;
              }
            }
          }
          else
          {
            sns_loge("%s: file %s does not exists", __func__, name.c_str());
          }
        }
      }

      fclose(fp);
    }
    else
    {
      sns_loge("%s: not able to stat file %s", __func__, group_list_file_path);
    }

    /* Delete ssc_connection registry read and write objects
       As we are done with sending and receiving
       registry read and write requests and events
    */
    clean_up();

    /* Trigger ADSP & BG Subsystem restart
       A. Read current restart level. And set it to "related".
       B. Trigger SSR
       C. Restore restart level
    */

    /*  Trigger ADSP and BG SSR  */
    sns_logi("%s:, trigger adsp and bg ssr", __func__);
    property_set("vendor.sensors.wrist_ssr", "adsp bg-wear");

    /*  Create a timer of 10 seconds to ensure file update
        to disk/flash is complete. In progress file
        is deleted after time expiry.
    */
    sns_logd("Creating timer event for current request");
    start_wrist_recovery_timer();
  });
}

void WristOrientation::start_wrist_recovery_timer()
{
    struct sigevent sigev = {};
    struct itimerspec it;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_notify_attributes = nullptr;
    sigev.sigev_value.sival_ptr = (void *) this;
    sigev.sigev_notify_function = handle_timer_expiry;
    if (timer_create(CLOCK_BOOTTIME, &sigev, &wrist_timer) != 0)
    {
      sns_loge("%s: Timer creation failed: %s", __func__, strerror(errno));
    }

    sns_logd("%s: setting 10 sec timer", __func__);
    memset(&it, 0, sizeof(it));
    it.it_value.tv_sec = 10;
    if(timer_settime(wrist_timer, 0, &it, NULL) == -1)
    {
      sns_loge("%s: failed to set timer for 10 sec", __func__);

      struct stat stat_in_progress_file;
      if(!stat(in_progress_file, &stat_in_progress_file))
      {
        if(remove(in_progress_file) == 0)
        {
          sns_logi("%s: in_progress_file removed successfully", __func__);
        }
        else
        {
          sns_loge("%s: failed to remove in_progress file with error = %s",
                        __func__, strerror(errno));
        }
      }
      else
      {
        sns_loge("%s: in_progress_file doesn't exit, unexpected", __func__);
      }
    }
    else
    {
      sns_logi("%s: Timer created for 10 sec", __func__);
    }
}

void WristOrientation::clean_up()
{
  sns_logd("Inside %s", __func__);
  if(registry_suid_obj != nullptr)
  {
    sns_logd("%s: deleting suid_obj", __func__);
    delete registry_suid_obj;
    registry_suid_obj = nullptr;
  }

  if(registry_read_obj != nullptr)
  {
    sns_logd("%s: deleting read_obj", __func__);
    delete registry_read_obj;
    registry_read_obj = nullptr;
  }

  if(registry_write_obj != nullptr)
  {
    sns_logd("%s: deleting write_obj", __func__);
    delete registry_write_obj;
    registry_write_obj = nullptr;
  }

}

bool WristOrientation::is_adsp_registry_up()
{
  bool timeout = true;
  int32_t max_retry = REGISTRY_SUID_TIMEOUT_MAX_RETRY;

  request_registry_suid();
  do
  {
    if(_registry_resp_flag.load())
    {
      _registry_resp_flag.store(false);
      timeout = false;
      break;
    }
    else
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(REGISTRY_SUID_TIMEOUT_MS));
    }
      max_retry--;
  }while(max_retry > 0);

  if(timeout)
  {
    sns_loge("%s: timed out waiting for registry suid event", __func__);
    clean_up();
    return false;
  }

  // delete registry suid connection object
  clean_up();

  return true;
}

void WristOrientation::wrist_handle_system_reset()
{
  sns_logi("remove registry files");

  struct stat stat_buf;
  FILE *fp = nullptr;
  char filename[MAX_FILENAME];

  if(!stat(group_list_file_path, &stat_buf))
  {
    fp = fopen(group_list_file_path, "r");
    if(!fp)
    {
      sns_loge("%s: Not able to open the file %s", __func__, group_list_file_path);
      return;
    }

    while(!feof(fp))
    {
      if(fgets(filename, MAX_FILENAME, fp))
      {
        stringstream input(filename);
        string name;
        input >> name;
        sns_logd("%s: checking if file %s exists", __func__, name.c_str());

        string reg_file = see_reg_group_path + name + '\0';
        sns_logd("%s: file to be removed %s", __func__, reg_file.c_str());

        if(remove(reg_file.c_str()) != 0)
        {
          sns_loge("%s: failed to remove %s with error %s",
                     __func__, reg_file.c_str(), strerror(errno));
        }
        else
        {
          sns_logd("%s: %s removed successfully", __func__, reg_file.c_str());
        }
      }
    }

    fclose(fp);
  }
  else
  {
    sns_loge("%s: could not stat %s", __func__, group_list_file_path);
    return;
  }

  sns_logi("%s: trigger adsp ssr", __func__);
  property_set("vendor.sensors.wrist_ssr", "adsp");

  /*  Wait till ADSP SSR is triggered  */
  sleep(1);

  property_get("persist.sys.wrist_orientation", orientation_prop, "");
  sns_logd("%s: current wrist mode = %d", __func__, atoi(orientation_prop));

  Orientation orient = static_cast<Orientation>(atoi(orientation_prop));
  sns_logi("%s: Current Orientation = %hhu", __func__, orient);

  currentOrientation = static_cast<Orientation>(0);
  sns_logd("%s: Setting current orientation to %hhu", __func__, currentOrientation);

  onOrientationChange(orient);
}

void WristOrientation::handle_timer_expiry(union sigval param)
{
  UNUSED(param);

  struct stat stat_in_progress_file;
  if(!stat(in_progress_file, &stat_in_progress_file))
  {
    if(remove(in_progress_file) == 0)
    {
      sns_logi("%s: in_progress_file removed successfully", __func__);
    }
    else
    {
      sns_loge("%s: failed to remove in-progress file with error = %s",
                          __func__, strerror(errno));
    }
  }
  else
  {
    sns_loge("%s, in_progress_file doesn't exit, unexpected", __func__);
  }

  if(timer_delete(wrist_timer) != 0)
  {
    sns_loge("%s: failed to delete timer", __func__);
  }
  else
  {
    sns_logi("%s, timer deleted successfully", __func__);
  }
  wrist_timer = nullptr;
}

void WristOrientation::create_in_progress_file()
{
  sns_logd("inside %s", __func__);

  int fd_in_progress = open(in_progress_file, O_CREAT, 0644);
  if(fd_in_progress < 0)
  {
    sns_loge("failed to create in_progress_file. Failed with error = %d, %s",
                              errno, strerror(errno));
  }
  else
  {
    sns_logi("temp file created successfully");
  }
}

void WristOrientation::registry_read_request(string filename)
{
  sns_logd("\n Inside %s", __func__);
  sns_client_request_msg pb_req_msg;
  sns_registry_read_req pb_stream_registry_req;
  string pb_registry_req_encoded;

  filename += '\0';
  pb_stream_registry_req.set_name(filename);
  pb_stream_registry_req.SerializeToString(&pb_registry_req_encoded);

  // Prepare the request to be sent
  pb_req_msg.mutable_suid()->set_suid_high(registry_suid.high);
  pb_req_msg.mutable_suid()->set_suid_low(registry_suid.low);

  pb_req_msg.mutable_request()->set_payload(pb_registry_req_encoded);
  pb_req_msg.set_msg_id(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ);
  pb_req_msg.mutable_susp_config()->set_delivery_type(
     SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->set_client_proc_type(
     SNS_STD_CLIENT_PROCESSOR_APSS);

  string pb_req_msg_encoded;

  pb_req_msg.SerializeToString(&pb_req_msg_encoded);

  // Sending registry read request
  sns_logi("%s: sent registry_read_request for %s", __func__, filename.c_str());

  registry_read_obj->send_request(pb_req_msg_encoded);
}

void WristOrientation::handle_registry_read_event(const uint8_t* data , int size)
{
  sns_logd("Inside %s", __func__);
  sns_client_event_msg pb_event_msg;

  // Parsing registry read event
  pb_event_msg.ParseFromArray(data, size);
  for(int i = 0; i < pb_event_msg.events_size(); i++)
  {
    const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
    sns_logd("event[%d] msg_id = %d", i, pb_event.msg_id());

    if(pb_event.msg_id() == SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT)
    {
      sns_registry_read_event reg_read;
      reg_read.ParseFromString(pb_event.payload());

      // Check if an entry for registry item recieved is present in the vector
      auto it = std::find_if(file_list.begin(), file_list.end(),
             [&reg_read] (const std::pair<std::string, wo_read_event_cb> &file_name)
             {
               return (file_name.first == reg_read.name());
             });

      if(it != file_list.end())
      {
        sns_logd("Received registry event for %s", reg_read.name().c_str());

        // Call the handler function to update the registry group
        it->second(reg_read.data(), reg_read.name());
      }
      else
      {
        sns_logi("%s: not a valid regsitry path: %s", __func__, reg_read.name().c_str());
      }
    }
  }
}

void WristOrientation::request_registry_suid()
{
  sns_logd("Inside %s", __func__);
  string registry_datatype = "registry";
  sns_client_request_msg pb_req_msg;
  sns_suid_req pb_suid_req;
  string pb_suid_req_encoded;
  string pb_req_msg_encoded;
  sensor_uid suid_lookup =
        {  12370169555311111083ull,
           12370169555311111083ull
        };

  // Create suid connection to get registry suid
  registry_suid_cb = [this](const uint8_t* data , int size)
  {
    this->handle_registry_suid_event(data, size);
  };

  if (NULL == (registry_suid_obj = new ssc_connection(registry_suid_cb)))
  {
    sns_loge("ssc connection for suid failed");
    return;
  }

  pb_suid_req.set_data_type(registry_datatype);
  pb_suid_req.set_register_updates(true);
  pb_suid_req.SerializeToString(&pb_suid_req_encoded);

  pb_req_msg.set_msg_id(SNS_SUID_MSGID_SNS_SUID_REQ);
  pb_req_msg.mutable_request()->set_payload(pb_suid_req_encoded);
  pb_req_msg.mutable_suid()->set_suid_high(suid_lookup.high);
  pb_req_msg.mutable_suid()->set_suid_low(suid_lookup.low);
  pb_req_msg.mutable_susp_config()->set_delivery_type(
      SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->set_client_proc_type(
      SNS_STD_CLIENT_PROCESSOR_APSS);

  pb_req_msg.SerializeToString(&pb_req_msg_encoded);

  sns_logi("%s: Sending registry suid request", __func__);
  registry_suid_obj->send_request(pb_req_msg_encoded);
}

wo_read_event_cb WristOrientation::get_handler(const char* type)
{
  wo_read_event_cb cb = nullptr;
  sns_logd("Handler type received = %s", type);

  if(strncmp(type, "DRIVER_BIAS", 11) == 0)
  {
    sns_logd("returning update_driver_bias");
    cb = &(WristOrientation::update_driver_bias);
  }
  else if(strncmp(type, "BIAS", 4) == 0)
  {
    sns_logd("returning update_bias");
    cb = &(WristOrientation::update_algo_bias);
  }
  else if(strncmp(type, "COMPENSATION_MATRIX", 19) == 0)
  {
    sns_logd("returning update_compensation_matrix");
    cb =  &(WristOrientation::update_compensation_matrix);
  }
  else if(strncmp(type, "CORR_MAT", 8) == 0)
  {
    sns_logd("returning update_corr_matrix");
    cb = &(WristOrientation::update_corr_matrix);
  }
  else if(strncmp(type, "ORIENT", 6) == 0)
  {
    sns_logd("returning update_orientation");
    cb = &(WristOrientation::update_orientation);
  }
  return cb;
}

void WristOrientation::handle_registry_write_event(const uint8_t* data, int size)
{
  sns_logd("Inside %s", __func__);
  sns_client_event_msg pb_event_msg;

  pb_event_msg.ParseFromArray(data, size);
  for(int i = 0; i < pb_event_msg.events_size(); i++)
  {
    const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
    sns_logd("event[%d] msg_id = %d", i, pb_event.msg_id());

    if(pb_event.msg_id() == SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_EVENT)
    {
      sns_registry_write_event reg_write;
      reg_write.ParseFromString(pb_event.payload());
      if (reg_write.status() == SNS_REGISTRY_WRITE_STATUS_SUCCESS)
      {
        sns_logi("%s was succesful", __func__);
      }
      else if(reg_write.status() == SNS_REGISTRY_WRITE_STATUS_ERROR_OTHER)
      {
        sns_logi("%s returned error", __func__);
      }
      else
      {
        sns_logi("%s returned invalid status", __func__);
      }
      _registry_resp_flag.store(true);
    }
  }
}

void WristOrientation::handle_registry_suid_event(const uint8_t* data, int size)
{
  sns_logd("Inside %s", __func__);

  sns_client_event_msg pb_event_msg;
  sns_suid_event pb_suid_event;

  pb_event_msg.ParseFromArray(data, size);
  for (int i = 0; i < pb_event_msg.events_size(); i++)
  {
    sns_logd("suid event iteration %d", i);
    auto& pb_event = pb_event_msg.events(i);

    if (pb_event.msg_id() == SNS_SUID_MSGID_SNS_SUID_EVENT)
    {
      pb_suid_event.ParseFromString(pb_event.payload());
      const string& datatype = pb_suid_event.data_type();
      for (int i = 0; i < pb_suid_event.suid_size(); i++)
      {
        if(datatype == "registry")
        {
          WristOrientation::registry_suid.low = pb_suid_event.suid(i).suid_low();
          WristOrientation::registry_suid.high = pb_suid_event.suid(i).suid_high();
          stringstream suidLow;
          suidLow << std::hex << WristOrientation::registry_suid.low;
          stringstream suidHigh;
          suidHigh << std::hex << WristOrientation::registry_suid.high;
          sns_logi("Received suid for registry %s %s",
                suidLow.str().c_str(), suidHigh.str().c_str());
          _registry_resp_flag.store(true);
        }
      }
    }
  }
}

void WristOrientation::update_algo_bias(sns_registry_data reg_data, const std::string name)
{
  sns_logd("%s: flipping signs of x and y", __func__);

  // flip signs of x and y bias values
  float values[] = { SIGN_FLIP((reg_data.items(0).flt())),
                     SIGN_FLIP((reg_data.items(1).flt())),
                     reg_data.items(2).flt()
                   };
  sns_logd("%s: Bias Values are for %s are (%f, %f, %f)", __func__, name.c_str(),
                     values[0], values[1], values[2]);


  /* Updates the version of the data field
     when the data is updated  */
  uint32_t ver[] = { (reg_data.items(0).version()) + 1,
                     (reg_data.items(1).version()) + 1,
                     (reg_data.items(2).version()) + 1
                   };

  sns_logd("%s: Version for %s = (%u, %u, %u)", __func__, name.c_str(),
                     ver[0], ver[1], ver[2]);

  // Preparing registry write request
  string bias_name[] = { "bias_0", "bias_1", "bias_2" };
  string version = "ver";

  sns_client_request_msg pb_req_msg;
  sns_registry_write_req pb_stream_registry_data_item;

  string null_terminated_name = name + '\0';
  pb_stream_registry_data_item.set_name(null_terminated_name);

  sns_registry_data* write_reg_data = pb_stream_registry_data_item.mutable_data();
  for (int i = 0; i < BIAS_VALUES; i++)
  {
    sns_registry_data_item* reg_write_item = write_reg_data->add_items();
    bias_name[i] += '\0';
    version += '\0';
    reg_write_item->set_name(bias_name[i]);
    reg_write_item->set_flt(values[i]);
    reg_write_item->set_version(ver[i]);
    sns_logd("%s: item_name = %s, item_value = %f, version = %u", name.c_str(),
                  reg_write_item->name().c_str(), reg_write_item->flt(),
                  reg_write_item->version());
  }

  // Sending registry write request
  sns_logi("%s: sending registry write request for %s", __func__, name.c_str());
  send_registry_write_request(pb_stream_registry_data_item);
}

void WristOrientation::update_driver_bias(sns_registry_data reg_data, const std::string name)
{
  sns_logd("%s: flipping signs of x and y", __func__);

  // Flip signs of x and y bias values
  float values[] = { SIGN_FLIP(reg_data.items(0).flt()),
                     SIGN_FLIP(reg_data.items(1).flt()),
                     reg_data.items(2).flt()
                   };

  sns_logd("%s: Driver Bias Values for %s are (%f, %f, %f)", __func__,
                  name.c_str(), values[0], values[1], values[2]);

  /* Updates the version of the data field
     when the data is updated  */
  uint32_t ver[] = { (reg_data.items(0).version()) + 1,
                     (reg_data.items(1).version()) + 1,
                     (reg_data.items(2).version()) + 1
                   };

  sns_logd("%s: Version for %s = (%u, %u, %u)", __func__, name.c_str(),
                     ver[0], ver[1], ver[2]);

  // Preparing registry write request
  string bias_name[] = { "x", "y", "z" };
  string version = "ver";

  sns_client_request_msg pb_req_msg;
  sns_registry_write_req pb_stream_registry_data_item;
  string pb_stream_registry_data_encoded = "";

  string null_terminated_name = name + '\0';
  pb_stream_registry_data_item.set_name(null_terminated_name);

  sns_registry_data* write_reg_data = pb_stream_registry_data_item.mutable_data();
  for (int i = 0; i < BIAS_VALUES; i++)
  {
    sns_registry_data_item* reg_write_item = write_reg_data->add_items();
    bias_name[i] += '\0';
    version += '\0';
    reg_write_item->set_name(bias_name[i]);
    reg_write_item->set_flt(values[i]);
    reg_write_item->set_version(ver[i]);
    sns_logd("%s: item_name = %s, item_value = %f, version = %u", name.c_str(),
                reg_write_item->name().c_str(), reg_write_item->flt(),
                reg_write_item->version());
  }

  // Sending registry write request
  sns_logi("%s: sending registry write request for %s", __func__, name.c_str());
  send_registry_write_request(pb_stream_registry_data_item);
}

void WristOrientation::update_compensation_matrix(sns_registry_data reg_data, const std::string name)
{
  sns_logd("%s: flipping signs of x and y", __func__);

  // Flip signs of xz, yz, zx and zy compensation matrix values
  float values[] = { reg_data.items(0).flt(),
                     reg_data.items(1).flt(),
                     SIGN_FLIP(reg_data.items(2).flt()),
                     reg_data.items(3).flt(),
                     reg_data.items(4).flt(),
                     SIGN_FLIP(reg_data.items(5).flt()),
                     SIGN_FLIP(reg_data.items(6).flt()),
                     SIGN_FLIP(reg_data.items(7).flt()),
                     reg_data.items(8).flt()
                   };

  /* Updates the version of the data field
     when the data is updated  */
  uint32_t ver[] = { reg_data.items(0).version(),
                     reg_data.items(1).version(),
                     reg_data.items(2).version(),
                     reg_data.items(3).version(),
                     reg_data.items(4).version(),
                     reg_data.items(5).version(),
                     reg_data.items(6).version(),
                     reg_data.items(7).version(),
                     reg_data.items(8).version()
                   };

  // Preparing registry write request
  string cm_name[] = { "cm_00", "cm_01", "cm_02",
                       "cm_10", "cm_11", "cm_12",
                       "cm_20", "cm_21", "cm_22"
                     };

  string version = "ver";

  sns_client_request_msg pb_req_msg;
  sns_registry_write_req pb_stream_registry_data_item;
  string pb_stream_registry_data_encoded = "";

  string null_terminated_name = name + '\0';
  pb_stream_registry_data_item.set_name(null_terminated_name);

  sns_registry_data* write_reg_data = pb_stream_registry_data_item.mutable_data();
  for (int i = 0; i < COMPENSATION_MATRIX; i++)
  {
    sns_registry_data_item* reg_write_item = write_reg_data->add_items();
    cm_name[i] += '\0';
    version += '\0';
    reg_write_item->set_name(cm_name[i]);
    reg_write_item->set_flt(values[i]);
    reg_write_item->set_version(ver[i] + 1);
    sns_logd("%s: item_name = %s, item_value = %f, version = %u", name.c_str(),
                reg_write_item->name().c_str(), reg_write_item->flt(),
                reg_write_item->version());
  }

  // Sending registry write request
  sns_logi("%s: sending registry write request for %s", __func__, name.c_str());
  send_registry_write_request(pb_stream_registry_data_item);
}

void WristOrientation::update_corr_matrix(sns_registry_data reg_data, const std::string name)
{
  sns_logd("%s: flipping signs of x and y", __func__);

  // Flip signs of xz, yz, zx and zy correlation matrix values
  float values[] = { reg_data.items(0).flt(),
                     reg_data.items(1).flt(),
                     SIGN_FLIP(reg_data.items(2).flt()),
                     reg_data.items(3).flt(),
                     reg_data.items(4).flt(),
                     SIGN_FLIP(reg_data.items(5).flt()),
                     SIGN_FLIP(reg_data.items(6).flt()),
                     SIGN_FLIP(reg_data.items(7).flt()),
                     reg_data.items(8).flt()
                   };

  /* Updates the version of the data field
     when the data is updated  */
  uint32_t ver[] = { reg_data.items(0).version(),
                     reg_data.items(1).version(),
                     reg_data.items(2).version(),
                     reg_data.items(3).version(),
                     reg_data.items(4).version(),
                     reg_data.items(5).version(),
                     reg_data.items(6).version(),
                     reg_data.items(7).version(),
                     reg_data.items(8).version()
                   };

  // Preparing registry write request
  string cm_name[] = { "0_0", "0_1", "0_2",
                       "1_0", "1_1", "1_2",
                       "2_0", "2_1", "2_2"
                     };

  string version = "ver";

  sns_client_request_msg pb_req_msg;
  sns_registry_write_req pb_stream_registry_data_item;
  string pb_stream_registry_data_encoded = "";

  string null_terminated_name = name + '\0';
  pb_stream_registry_data_item.set_name(null_terminated_name);

  sns_registry_data* write_reg_data = pb_stream_registry_data_item.mutable_data();
  for (int i = 0; i < CORR_MAT; i++)
  {
    sns_registry_data_item* reg_write_item = write_reg_data->add_items();
    cm_name[i] += '\0';
    reg_write_item->set_name(cm_name[i]);
    reg_write_item->set_flt(values[i]);
    reg_write_item->set_version(ver[i] + 1);
    sns_logd("%s: item_name = %s, item_value = %f, version = %u", name.c_str(),
                  reg_write_item->name().c_str(), reg_write_item->flt(),
                  reg_write_item->version());
  }

  sns_logi("%s: sending registry write request for %s", __func__, name.c_str());
  send_registry_write_request(pb_stream_registry_data_item);
}

void WristOrientation::update_orientation(sns_registry_data reg_data, const std::string name)
{
  sns_logd("%s: flipping orientation signs of x and y", __func__);

  // Put orientation values into local variables
  string values[] = { reg_data.items(0).str(),
                      reg_data.items(1).str(),
                      reg_data.items(2).str()
                   };

  sns_logd("%s: Original Orientation signs for %s are (%s, %s, %s)", __func__,
                  name.c_str(), values[0].c_str(), values[1].c_str(), values[2].c_str());

/*  Update the orientation of axes from
    +axis to -axis and vice-versa for x and y*/
  for(int axes = 0; axes < AXES - 1; axes++)
  {
    if(values[axes][0] == '-')
    {
      values[axes][0] = '+';
    }
    else if(values[axes][0] == '+')
    {
      values[axes][0] = '-';
    }
  }

  sns_logd("%s: Updated Orientation signs for %s are (%s, %s, %s)", __func__,
                  name.c_str(), values[0].c_str(), values[1].c_str(), values[2].c_str());

  /* Updates the version of the data field
     when the data is updated  */
  uint32_t ver[] = { (reg_data.items(0).version()) + 1,
                     (reg_data.items(1).version()) + 1,
                     (reg_data.items(2).version()) + 1
                   };

  // Preparing registry write request
  string axes[] = { "x", "y", "z" };
  string version = "ver";

  sns_client_request_msg pb_req_msg;
  sns_registry_write_req pb_stream_registry_data_item;
  string pb_stream_registry_data_encoded = "";

  string null_terminated_name = name + '\0';
  pb_stream_registry_data_item.set_name(null_terminated_name);

  sns_registry_data* write_reg_data = pb_stream_registry_data_item.mutable_data();
  for (int i = 0; i < AXES; i++)
  {
    sns_registry_data_item* reg_write_item = write_reg_data->add_items();
    axes[i] += '\0';
    values[i] += '\0';
    reg_write_item->set_name(axes[i]);
    reg_write_item->set_str(values[i]);
    reg_write_item->set_version(ver[i]);
    sns_logd("%s: item_name = %s, item_value = %s, version = %u", name.c_str(),
              reg_write_item->name().c_str(), reg_write_item->str().c_str(),
              reg_write_item->version());
  }

  // Sending registry write request
  sns_logi("%s: sending registry write request for %s", __func__, name.c_str());
  send_registry_write_request(pb_stream_registry_data_item);
}

void WristOrientation::send_registry_write_request(sns_registry_write_req pb_stream_registry_data_item)
{
  string pb_stream_registry_data_encoded = "";
  sns_client_request_msg pb_req_msg;
  pb_stream_registry_data_item.SerializeToString(&pb_stream_registry_data_encoded);

  pb_req_msg.mutable_suid()->set_suid_high(WristOrientation::registry_suid.high);
  pb_req_msg.mutable_suid()->set_suid_low(WristOrientation::registry_suid.low);
  pb_req_msg.set_msg_id(SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);
  pb_req_msg.mutable_request()->set_payload(pb_stream_registry_data_encoded);

  pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

  string pb_req_msg_encoded;
  pb_req_msg.SerializeToString(&pb_req_msg_encoded);

  // Sending registry write request
  WristOrientation::registry_write_obj->send_request(pb_req_msg_encoded);
}

/*  Fetches wrist orientation service
    Register for callbacks  */
bool wrist_orientation_init()
{
  sns_logi("%s : getting wrist_callback service\n",__func__);

  // Get wrist orientation service
  android::sp<IWristOrientation> wristService = IWristOrientation::getService();
  if(!wristService)
  {
    sns_loge("%s : did not receive wo service\n", __func__);
    return false;
  }

  sns_logi("%s : received wrist orientation service\n", __func__);

  // Register for callback
  android::sp<WristOrientation> _cb = new WristOrientation();
  auto ret = wristService->registerCallback(_cb);
  if(!(ret.isOk()))
  {
    sns_loge("%s : callbacks did not register succesfully \n",__func__);
    return false;
  }
  sns_logi("%s : callbacks registered succesfully \n",__func__);

  return true;
}

#endif
