/*=============================================================================
  @file sensordaemon.cpp

  Background daemon process on LA to serve sensors needs.

  - Initializes and manages DIAG settings for SLPI subsystem
  - Initializes "Always On Test Sensors"
  - Provides android property access to the registry service

  Copyright (c) 2018 - 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <unordered_map>
#include <cutils/properties.h>
#include <poll.h>
#include <comdef.h>
#include <sys/stat.h>
#include <diag_lsm.h>
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif
#include "worker.h"
#include "sensors_log.h"
#include "aont.h"
#include <sys/inotify.h>
#ifdef SNS_TARGET_LE_PLATFORM
#include <algorithm>
#endif
#include <vector>
#include <iostream>
#include <fstream>
#ifdef SNS_WEARABLES_TARGET_BG
#include "wrist_orientation_update.h"
#endif

#define BUF_LEN     4096
#define MICRO_SECOND 1000000
#define POLL_TIMEOUT -1

#define MAX_DIAG_RETRY_COUNT 25
#define DIAG_RETRY_WAIT_TIME_US 200000
#define BUFFERING_MODE_WAIT_TIME_US 6000000

using namespace std;
using namespace sensors_log;

/*=============================================================================
  Macros
  ===========================================================================*/

/* "Sensors" DIAG peripheral ID.  Seems to usually refer to SLPI Root PD */
#define SNS_DIAG_PERIPHERAL_ID1         3
/* "Sensors User PD" DIAG peripheral ID */
#define SNS_DIAG_PERIPHERAL_ID2         9

#define SNS_DIAG_TX_MODE_STREAMING      0
#define SNS_DIAG_TX_MODE_CIRCULAR_BUFF  2
#define SNS_DIAG_LOW_WM_VALUE           20
#define SNS_DIAG_HIGH_WM_VALUE          70

/*=============================================================================
  Static Data
  ===========================================================================*/

static const char SENSORS_DAEMON_PROP_DEBUG[] = "persist.vendor.sensors.debug.daemon";
static const char SENSORS_AONT_ENABLE[] = "persist.vendor.sensors.debug.enable.aon_sensor";
static const char SENSORS_DIAG_BUFFER_ENABLE[] = "persist.vendor.sensors.diag_buffer_mode";

/* Named pipe for external entities to request a DIAG buffer flush */
static const char *pipe_name = "/data/vendor/sensors/sns_diag_flush";
/* Location for DIAG configuration; if not found on-device logging disabled */
static const char *diag_cfg_name = "/data/vendor/sensors/Diag.cfg";
static const char *diag_odl_file = "/data/vendor//sensors/file1";
static const char *SLPI_SSR_SYSFS_PATH = "/sys/kernel/boot_slpi/ssr";

static worker _worker_diag;
static worker _worker_registry;
static worker _worker_aon;
static worker _worker_diag_mdlog;

static string prev_command=" ";

static condition_variable diag_init_cv;
static mutex diag_init_lock;
static bool diag_init_done = false;

/* map debug property value to log_level */
static const unordered_map<char, sensors_log::log_level> log_level_map = {
  { '0', sensors_log::SILENT },
  { '1', sensors_log::INFO },
  { 'e', sensors_log::ERROR },
  { 'E', sensors_log::ERROR },
  { 'i', sensors_log::INFO },
  { 'I', sensors_log::INFO },
  { 'd', sensors_log::DEBUG },
  { 'D', sensors_log::DEBUG },
  { 'v', sensors_log::VERBOSE },
  { 'V', sensors_log::VERBOSE },
};

/*=============================================================================
  External Function Declarations
  ===========================================================================*/

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Determine whether DIAG buffering mode is enabled for Sensors.
 * Defaults to enabled.
 **/

static bool
diag_buffering_enabled(void)
{
  char debug_prop[PROPERTY_VALUE_MAX];
  int len = property_get(SENSORS_DIAG_BUFFER_ENABLE, debug_prop, "false");
  bool rv = true;

  if((len > 0) && (0 == strncmp(debug_prop, "false", strlen("false"))))
    rv = false;

  sns_logi("DIAG Buffering mode val: %d", rv);
  return rv;
}
/**
 * Determine whether ODL for Sensors ADSP is enabled.
 * Defaults to disabled.
 **/
static bool
is_odl_sensors_adsp(void)
{
  struct stat sb;
  bool adsp = true;
  if (!stat(SLPI_SSR_SYSFS_PATH, &sb))
      adsp = false;
  sns_logd(" adsp=%d", (int)adsp);
  return adsp;
}

static vector<string>
get_odl_commands(){
    ifstream fin;
    vector<string> cmds;
    string line;
    try{
      fin.open(diag_odl_file, fstream::in);
      if(fin.is_open()){
        while(getline(fin, line)){
          sns_logd(" line(command): %s ", line.c_str());
          if(!line.empty())
            cmds.push_back(line);
        }
        fin.close();
      }else{
        sns_loge(" Failed to open %s ", diag_odl_file);
      }
    }catch(exception& e){
        sns_loge(" exception : %s ", e.what());
    }
    return cmds;
}

/**
 * Generates folder name for On Device Logging
 **/

static string
get_folder_name(void){
    time_t rawtime;
    tm* timeinfo;
    char buffer [80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if(timeinfo){
      strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
      string time_str(buffer);
      replace( time_str.begin(), time_str.end(), '-', '_' );
      return ("odl_sns_"+time_str);
    }
    else
      return "odl_sns_date_time";
}

/*
 Reads the data from diag_odl_file
 Starts or Stops the On Device Logging Instance
*/
static int
handle_events(int fd) {
  char buf[BUF_LEN];
  memset(buf, '\0', BUF_LEN);
  const struct inotify_event *event;
  ssize_t len;
  char *ptr;
  char flush[] = "1";
  int pipe_fd = open(pipe_name, O_WRONLY);
  if (pipe_fd < 0) {
    sns_loge(" Unable to open the pipe : %s", pipe_name);
    return -1;
  }

  vector<string> odl_commands = get_odl_commands();
  len = read(fd, buf, sizeof buf);
  if (len == -1 && errno != EAGAIN) {
    sns_loge(" perror read : %i ", errno);
    return -1;
  }
  if (len <= 0)
    return -1;
  /* Loop over all events in the buffer */
  for (ptr = buf; ptr < buf + len;
      ptr += sizeof(struct inotify_event) + event->len) {
    event = (const struct inotify_event *) ptr;
    if (NULL != event) {
      /* Print event type */
      if (event->mask & IN_MODIFY) {
        sns_logi("handle_event : IN_MODIFY");
        if(odl_commands.size() > 1){
          if ("stop" == odl_commands[0]) {
           if(prev_command != odl_commands[0]){
              write(pipe_fd, flush, strlen(flush));
              sns_logi("Daig : Killing current ODL instance ");
              if(true == is_odl_sensors_adsp()){
                  string stop_command = "/vendor/bin/diag_mdlog -k -p " ;
                  if ("enable_HLOS_packets" == odl_commands[1]){
                      stop_command = stop_command + "5";
                  }else{
                      stop_command = stop_command + "4";
                  }
                  system(stop_command.c_str());
                  system("/vendor/bin/diag_mdlog -k -g 16384 ");
              }
              else{
                  string stop_command = "/vendor/bin/diag_mdlog -k -p ";
                  if ("enable_HLOS_packets" == odl_commands[1]){
                      stop_command = stop_command + "17";
                  }else{
                      stop_command = stop_command + "16";
                  }
                  system(stop_command.c_str());
              }
              prev_command = odl_commands[0];
              usleep(5 * MICRO_SECOND);
          }
          }
          if ( "star" == odl_commands[0]) {
            if (0 == access(diag_cfg_name, F_OK)) {
              sns_logi("Daig : Diag.cfg present: starting diag_mdlog");
              string dir = "/data/vendor/sensors/"+get_folder_name();
              char * cdir = new char [dir.length()+1];
              strlcpy (cdir, dir.c_str(),(dir.length()+1));
              if(prev_command != odl_commands[0]){
                  if( (mkdir(cdir, S_IRWXU | S_IRWXG | S_IRWXO )) !=0 )
                      sns_loge("mkdir failed errno:%d - strerror: %s\n",errno,strerror(errno));
                  else{
                      string command;
                      string command2;
                      if(true == is_odl_sensors_adsp()) {
                           command = "/vendor/bin/diag_mdlog -g 16384 -f /data/vendor/sensors/Diag.cfg -w "+dir+" -o "+dir+" & ";
                           if ("enable_HLOS_packets" == odl_commands[1]){
                               command2 = "/vendor/bin/diag_mdlog -p 5 -f /data/vendor/sensors/Diag.cfg -w "+dir+" -o "+dir+" & ";
                           }else{
                               command2 = "/vendor/bin/diag_mdlog -p 4 -f /data/vendor/sensors/Diag.cfg -w "+dir+" -o "+dir+" & ";
                           }
                      }
                      else{
                           if ("enable_HLOS_packets" == odl_commands[1]){
                               command = "/vendor/bin/diag_mdlog -p 17 -f /data/vendor/sensors/Diag.cfg -w "+dir+" -o "+dir+" & ";
                           }else{
                               command = "/vendor/bin/diag_mdlog -p 16 -f /data/vendor/sensors/Diag.cfg -w "+dir+" -o "+dir+" & ";
                           }
                      }
                      system(command.c_str());
                      if(true == is_odl_sensors_adsp()) {
                           system(command2.c_str());
                      }
                      prev_command = odl_commands[0];
                      usleep(3 * MICRO_SECOND);
                  }
               }
            }
          }
        }
      }
      if (event->len) {
        sns_logi("event : %s ", event->name);
      }
    }
    else{
        break;
    }
  }
  close(pipe_fd);
  return 1;
}

/*
 Creates file watcher on diag_odl_file and waits on modify event
*/
static int
diag_mdlog_capture(void) {
  sns_logi(" in diag_mdlog_capture ");
  FILE *pFile = fopen(diag_odl_file, "w");
  usleep(2 * MICRO_SECOND);
  if (NULL != pFile) {
    fclose(pFile);
  }

  if (chmod(diag_odl_file, (S_IRWXU | S_IRWXG | S_IRWXO))) {
    sns_loge("fail to change permission on diag_odl_file, err = %d, %s", errno, strerror(errno));
  }
  int fd, poll_num;
  int wd;
  nfds_t nfds;
  struct pollfd fds[1];

  fd = inotify_init1(0);
  if (fd == -1) {
    sns_loge("inotify_init1 failed ");
    return -1;
  }

  wd = inotify_add_watch(fd, diag_odl_file, IN_MODIFY | IN_CREATE);
  nfds = 1;
  fds[0].fd = fd;
  fds[0].events = POLLIN;
  while (1) {
    sns_logi(" before poll - for modify event");
    poll_num = poll(fds, nfds, POLL_TIMEOUT);
    if (poll_num == -1) {
      if (errno == EINTR)
        continue;
      sns_loge("poll perror: %i", errno);
      return -1;
    }
    if (poll_num > 0) {
      if (fds[0].revents & POLLIN) {
        int ret = handle_events(fd);
        if (-1 == ret) {
          return -1;
        }
      }
    }
  }
  inotify_rm_watch(fd, wd);
  close(fd);
  return 1;
}

/**
 * Wait for and handle requests to flush the DIAG buffer.
 */
static void
wait_handle_flush(void)
{
  struct pollfd fds[1];

  if(0 != mkfifo(pipe_name, 0666) && EEXIST != errno)
  {
    sns_loge("mkfifo error: %i", errno);
  }
  else
  {
    int pipe_fd = open(pipe_name, O_RDONLY);
    if(pipe_fd < 0)
    {
      sns_loge("open error: %i", errno);
    }
    else
    {
      fds[0].fd = pipe_fd;
      fds[0].events = POLLIN;

      for(;;)
      {
        if(0 < poll(fds, 1, POLL_TIMEOUT) && fds[0].revents & POLLIN)
        {
          char read_data[10];
          int read_len;
          read_len = read(fds[0].fd, read_data, sizeof(read_data));

          sns_logi("sns_diag_flush read len: %i", read_len);

          if(1 != diag_peripheral_buffering_drain_immediate(SNS_DIAG_PERIPHERAL_ID1) ||
             1 != diag_peripheral_buffering_drain_immediate(SNS_DIAG_PERIPHERAL_ID2))
            sns_loge("diag_peripheral_buffering_drain_immediate error");
        }
      }

      close(pipe_fd);
    }
  }
}

/**
 * Initialize the DIAG module, and configure appropriate
 * buffering/streaming mode.
 */
static void
handle_diag_settings(void)
{
  int err = 0;
  int retry_count = 0;
  bool diag_init = false;
  bool buffering_mode_enabled = false;

  do {
    retry_count++;
    sns_logi("try Diag_LSM_Init, count = %d", retry_count);
    diag_init = Diag_LSM_Init(NULL);
    if (!diag_init) usleep(DIAG_RETRY_WAIT_TIME_US);
    else break;
  } while (retry_count < MAX_DIAG_RETRY_COUNT);

  if(!diag_init)
  {
    sns_loge("Diag_LSM_Init failed!");
  }
  else
  {
    sns_logi("Diag_LSM_Init is successful");
    uint32_t buffer_mode;
    usleep(BUFFERING_MODE_WAIT_TIME_US);
    if(diag_buffering_enabled())
    {
      sns_logi("Setting SNS TX MODE to CIRCULAR BUFF");
      buffer_mode = SNS_DIAG_TX_MODE_CIRCULAR_BUFF;
      buffering_mode_enabled = true;
    }
    else
    {
      sns_logi("Setting SNS TX MODE to STREAMING");
      buffer_mode = SNS_DIAG_TX_MODE_STREAMING;
    }

    err = diag_configure_peripheral_buffering_tx_mode(SNS_DIAG_PERIPHERAL_ID1,
        buffer_mode, SNS_DIAG_LOW_WM_VALUE, SNS_DIAG_HIGH_WM_VALUE);

    err = diag_configure_peripheral_buffering_tx_mode(SNS_DIAG_PERIPHERAL_ID2,
        buffer_mode, SNS_DIAG_LOW_WM_VALUE, SNS_DIAG_HIGH_WM_VALUE);

    /* 1 is success and 0 failure */
    if(1 == err)
      sns_logi("Set SNS TX MODE success and return val: %d", err);
    else
      sns_loge("Set SNS TX MODE failed and return val: %d", err);
  }

  if(buffering_mode_enabled && 0 == access(diag_cfg_name, F_OK))
  {
    sns_logi(" Diag.cfg present: starting diag_mdlog ");
    if( true == is_odl_sensors_adsp()){
        system("/vendor/bin/diag_mdlog -g 16384 -f /data/vendor/sensors/Diag.cfg -w /data/vendor/sensors -o /data/vendor/sensors & ");
        system("/vendor/bin/diag_mdlog -p 4 -f /data/vendor/sensors/Diag.cfg -w /data/vendor/sensors -o /data/vendor/sensors & ");
    }
    else
        system("/vendor/bin/diag_mdlog -p 16 -f /data/vendor/sensors/Diag.cfg -w /data/vendor/sensors -o /data/vendor/sensors & ");

    FILE *pFile = fopen(diag_odl_file, "w");
    sns_logi(" after opening file ");
    if(NULL != pFile){
        char boot_odl_started[]="start\n";
        size_t write_len = fwrite(boot_odl_started, sizeof(char), sizeof(boot_odl_started), pFile);
        sns_logi(" written %zu",write_len);
        if(write_len != sizeof(boot_odl_started)){
            sns_loge(" write error to diag_odl_file");
        }
        fclose(pFile);
    }
    else{
        sns_loge(" error opening diag_odl_file");
    }
  }

  unique_lock<mutex> lk(diag_init_lock);
  diag_init_done = true;
  diag_init_cv.notify_one();
  lk.unlock();

  wait_handle_flush();
}

/**
 * Set level of debug messages which will be delivered to logcat.
 */
static void
set_log_level(void)
{
  char debug_prop[PROPERTY_VALUE_MAX];
  int len;
  log_level loglevel;
  sensors_log::set_tag("sensors.qti");

  len = property_get(SENSORS_DAEMON_PROP_DEBUG, debug_prop, "i");
  if(len > 0)
  {
    if(log_level_map.find(debug_prop[0]) != log_level_map.end())
    {
      loglevel = log_level_map.at(debug_prop[0]);
      sns_logi("log_level set: %d", loglevel);
      sensors_log::set_level(loglevel);
    }
  }
}

/**
 * Checks whether the "Always On Test Sensor" should be enabled based on
 * Android configuration.
 *
 * PEND: What does AONT do?
 */
static void
handle_aont(aont *aontobj)
{
  char debug_prop[PROPERTY_VALUE_MAX];
  int len = property_get(SENSORS_AONT_ENABLE, debug_prop, "false");

  if((len > 0) && (0 == strncmp(debug_prop, "true", 4)))
  {
    sns_logi("sending enable aont request");
    aontobj->enable();
    sns_logi("sent enable aont request");
  }
  else
  {
    sns_logd("aont disabled");
  }
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

int main()
{

  set_log_level();
  aont aontobj;

  sns_logi("sensors daemon starting");
  _worker_diag.add_task(NULL, [] {
      handle_diag_settings();
      });

  _worker_aon.add_task(NULL, [&aontobj] {
      handle_aont(&aontobj);
      });
  unique_lock<mutex> lk(diag_init_lock);
  long max_wait_time_ms = ((MAX_DIAG_RETRY_COUNT*DIAG_RETRY_WAIT_TIME_US) + BUFFERING_MODE_WAIT_TIME_US)/1000;
  auto start_ts = chrono::steady_clock::now();
  if (!diag_init_cv.wait_for(lk, chrono::milliseconds(max_wait_time_ms), []{return diag_init_done;})) {
    sns_loge("diag is not initialized within %ld ms", max_wait_time_ms);
  } else {
    sns_logi("took %ld ms to complete diag init",
      chrono::duration_cast<chrono::duration<long, std::milli>>(chrono::steady_clock::now() - start_ts).count());
  }
  lk.unlock();
  _worker_diag_mdlog.add_task(NULL, [] {
     diag_mdlog_capture();
     });

#ifdef SNS_WEARABLES_TARGET_BG
    sns_logi("%s: wrist_orientation support init start....", __func__);
    wrist_orientation_init();
#endif

  sleep(INT_MAX);
  sns_loge("sensors daemon exiting!");

  return 0;
}
