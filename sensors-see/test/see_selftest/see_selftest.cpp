/* ===================================================================
** Copyright (c) 2017-2018, 2020, 2022 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_selftest.cpp
** DESC: physical sensor self test
** ================================================================ */
#include <iostream>
#include <iomanip>    // std::setprecision
#include <sstream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <time.h>
#include <sys/file.h>

#ifdef SNS_TARGET_LE_PLATFORM
#include <string.h>
#include <sys/time.h>
#endif
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

#include "see_salt.h"

using namespace std;

static void common_exit(int retcode);
static void append_suid(ostringstream &outss, sens_uid *suid);
static void append_log_time_stamp( ostringstream &outss);
static void writeline(ostringstream &outss);
static void write_result_description( string passfail);

static string newline("\n");
static string cmdline("");

static ostringstream oss;

#define SUCCESSFUL               0
#define FAILURE                  4
#define EVENT_KEY_SUFFIX_OFFSET  4
#define SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT  1026

#define MSG_ID_STR        "msg_id"
#define TEST_RESULT_STR   "test_passed"

bool self_test_result = false;

/**
 * @brief parse event string and get value by given key
 * @param event[i] - event string
 * @param key[i] - key string in left of ': '
 * @param offset[i] - length of the key
 * @return value string.
 */
string get_event_value(string event, string key)
{
    string value = "";
    string::size_type key_position = 0;
    string::size_type comma_position = 0;
    string::size_type value_len = 0;
    int offset = key.length() + EVENT_KEY_SUFFIX_OFFSET;
    key_position = event.find(key);
    if(key_position != string::npos) {
        if((comma_position = event.find(",", key_position + offset)) != string::npos ) {
            value_len = comma_position - key_position - offset;
            value = event.substr(key_position + offset, value_len);
        } else {
            cout << "Can not find comma in the event string" << endl;
        }
    } else {
        cout << "No key: " << key << " in the event string" << endl;
    }
    return value;
}

int get_event_id(string event)
{
    int event_id = 0;
    string id_str = get_event_value(event, MSG_ID_STR);
    if(id_str.length()) {
        event_id = stoi(id_str);
    } else {
        cout << "Get message id string failed" <<endl;
    }
    return event_id;
}

bool get_test_result(string event)
{
    int result = 0;
    string result_str = get_event_value(event, TEST_RESULT_STR);
    if(result_str.length()) {
        result = stoi(result_str);
    } else {
        cout << "Get test result string failed" <<endl;
    }
    bool test_result = (result != 0)? true: false;
    return test_result;
}

void event_cb(string events, bool is_registry_sensor)
{
    cout << "is_registry_sensor=" << is_registry_sensor << endl;
    cout << events << endl;
    int event_id = get_event_id(events);
    if(event_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT) {
        self_test_result = get_test_result(events);
    } else {
        cout << "This event ID is "<< event_id << ", not a self test event" <<endl;
    }
}

int self_test_sensor(see_salt *psalt,
                      sens_uid *suid,
                      see_self_test_type_e testtype)
{
   int ret = 0;
   see_self_test_config payload( testtype);
   oss << "self_test_sensor() testtype " << to_string( testtype)
       << " - " << payload.get_test_type_symbolic( testtype);
   writeline(oss);

   see_std_request std_request;
   std_request.set_payload( &payload);
   see_client_request_message request( suid,
                                       MSGID_SELF_TEST_CONFIG,
                                       &std_request);
   ret = psalt->send_request(request);
   oss << "self_test_sensor()";
   writeline(oss);
   return ret;
}

void disable_sensor(see_salt *psalt, sens_uid *suid, string target)
{
   oss << "disable_sensor( " << target << ")";
   writeline(oss);
   see_std_request std_request;
   see_client_request_message request( suid,
                                       MSGID_DISABLE_REQ,
                                       &std_request);
   psalt->send_request(request);
   oss << "disable_sensor()";
   writeline(oss);
}

struct app_config {
    bool  is_valid = false;
    char  data_type[ 32];           // type: accel, gyro, mag, ...

    bool  has_name = false;
    char  name[ 32];

    bool  has_on_change = false;
    bool  on_change;

    bool  has_rigid_body = false;
    char  rigid_body[ 32];

    bool  has_hw_id = false;
    int   hw_id;

    bool  has_testtype = false;
    int   testtype = SELF_TEST_TYPE_FACTORY;

    bool  has_delay = false;
    float delay;

    float duration = 5.0;          // seconds

    bool  has_stream_type = false;
    int   stream_type;

    app_config() {}
};

static app_config config;

void usage(void)
{
    cout << "usage: see_selftest"
            " -sensor=<sensor_type>"
            " [-name=<name>]"
            " [-on_change=<0 | 1>] [-stream_type=<stream_type>\n"
            "       [-rigid_body=<display | keyboard | external>]"
            " [-hw_id=<number>]\n"
            "       -testtype=<number | sw | hw | factory | com>\n"
            "       [-delay=<seconds>]"
            " -duration=<seconds> [-help]\n"
            " where: <sensor_type> :: accel | gyro | mag | ...\n"
            "        <stream_type> :: 0 streaming | 1 on_change,"
            " 2 single_output\n";
}

/**
 * @brief parse command line argument of the form keyword=value
 * @param parg[i] - command line argument
 * @param key[io] - gets string to left of '='
 * @param value[io] - sets string to right of '='
 */
bool get_key_value(char *parg, string &key, string &value)
{
   char *pkey = parg;

   while ( char c = *parg) {
      if ( c == '=') {
         key = string( pkey, parg - pkey);
         value = string( ++parg);
         return true;
      }
      parg++;
   }
   return false;
}

void parse_testtype( string value)
{
   string lit_sw("sw");
   string lit_hw("hw");
   string lit_factory("factory");
   string lit_com("com");

   config.has_testtype = true;
   if ( value == lit_sw) {
      config.testtype = SELF_TEST_TYPE_SW;
   }
   else if ( value == lit_hw) {
      config.testtype = SELF_TEST_TYPE_HW;
   }
   else if ( value == lit_factory) {
      config.testtype = SELF_TEST_TYPE_FACTORY;
   }
   else if ( value == lit_com) {
      config.testtype = SELF_TEST_TYPE_COM;
   }
   else {
      config.testtype = atoi( value.c_str());
   }
}

int parse_arguments(int argc, char *argv[])
{
    string sensor_eq( "-sensor");
    string name_eq( "-name");
    string on_change_eq( "-on_change");
    string rigid_body_eq( "-rigid_body");
    string hw_id_eq( "-hw_id");
    string testtype_eq( "-testtype");
    string delay_eq( "-delay");
    string duration_eq( "-duration");
    string stream_type_eq( "-stream_type");

    config.is_valid = true;
    for ( int i = 1; i < argc; i++) {
        if (( 0 == strcmp( argv[i], "-h"))
             || ( 0 == strcmp( argv[i], "-help"))) {
           usage();
           exit(SUCCESSFUL);
        }

        string key;
        string value;
        if ( get_key_value( argv[i], key, value)) {
           if ( sensor_eq == key) {
              strlcpy( config.data_type,
                       value.c_str(),
                       sizeof( config.data_type));
           }
           else if ( name_eq == key) {
              config.has_name = true;
              strlcpy( config.name,
                       value.c_str(),
                       sizeof( config.name));
           }
           else if ( on_change_eq == key) {
              config.has_on_change = true;
              config.on_change = atoi( value.c_str());
           }
           else if ( rigid_body_eq == key) {
              config.has_rigid_body = true;
              strlcpy( config.rigid_body, value.c_str(),
                       sizeof( config.rigid_body));
           }
           else if ( hw_id_eq == key) {
              config.has_hw_id = true;
              config.hw_id = atoi( value.c_str());
           }
           else if ( testtype_eq == key) {
              parse_testtype( value);
           }
           else if ( delay_eq == key) {
              config.has_delay = true;
              config.delay = atof( value.c_str());
           }
           else if ( duration_eq == key) {
              config.duration = atof( value.c_str());
           }
           else if ( stream_type_eq == key) {
              config.has_stream_type = true;
              config.stream_type = atoi( value.c_str());
              if ( config.stream_type < 0 || config.stream_type > 2) {
                  cout << "FAIL invalid stream_type value " << value.c_str() << endl;
                  config.is_valid = false;
              }
           }
           else {
               config.is_valid = false;
               cout << "FAIL unrecognized arg " << argv[i] << endl;
           }
        }
        else {
            config.is_valid = false;
            cout << "FAIL unrecognized arg " << argv[i] << endl;
        }
    }
    if ( !config.is_valid) {
       usage();
       return FAILURE;
    }
    return SUCCESSFUL;
}

int main(int argc, char *argv[])
{
    cout << "see_selftest version 1.4" << endl;

    // save and display commandline
    for ( int i = 0; i < argc; i++) {
       oss << argv[i] << " ";
    }
    cmdline = oss.str();
    writeline(oss);

    if( parse_arguments(argc, argv))
       common_exit(FAILURE);

    if ( config.data_type[0] == '\0') {
       oss << "missing -sensor=<sensor_type>";
       writeline(oss);
       common_exit(FAILURE);
    }

    if ( config.has_delay) {
       oss << "delay startup " << config.delay << " seconds";
       writeline(oss);
       usleep( (int)config.delay * 1000000); // implement startup delay
    }

    see_salt *psalt = see_salt::get_instance();
    if ( psalt == nullptr) {
       oss << "FAIL see_salt::get_instance() failed";
       writeline(oss);
       common_exit(FAILURE);
    }

    psalt->begin();  // populate sensor attributes

    /* get vector of all suids for target sensor */
    vector<sens_uid *> sens_uids;
    string target_sensor = string( config.data_type);
    oss << "lookup: " << target_sensor;
    writeline(oss);
    psalt->get_sensors( target_sensor, sens_uids);
    for (size_t i = 0; i < sens_uids.size(); i++) {
       sens_uid *suid = sens_uids[i];
       oss << "found " << target_sensor << " ";
       append_suid(oss, suid);
       writeline(oss);
    }

    /* choose target_suid based on type, name, on_change, rigid_body, hw_id */
    sens_uid *target_suid = NULL;
    sensor *psensor = NULL;

    for (size_t i = 0; i < sens_uids.size(); i++) {
       sens_uid *candidate_suid = sens_uids[i];
       psensor = psalt->get_sensor( candidate_suid);
       if ( config.has_name) {
          string name = psensor->get_name();
          if ( 0 != strcmp( config.name, name.c_str())) {
             continue;
          }
       }
       if ( config.has_on_change) {
          if ( !psensor->has_stream_type()) {
             continue;
          }
          if ( config.on_change != psensor->is_stream_type_on_change()) {
             continue;
          }
       }
       if ( config.has_stream_type ) {
           if ( !psensor->has_stream_type()) {
              continue;
           }
           if ( config.stream_type == 0 ) {
               if ( !psensor->is_stream_type_streaming()) {
                   continue;
               }
           }
           else if ( config.stream_type == 1 ) {
               if ( !psensor->is_stream_type_on_change()) {
                   continue;
               }
           }
           else if ( config.stream_type == 2) {
               if ( !psensor->is_stream_type_single_output()) {
                   continue;
               }
           }
       }
       if ( config.has_rigid_body) {
          string rigid_body = psensor->get_rigid_body();
          if ( 0 != strncmp( config.rigid_body, rigid_body.c_str(),
                             rigid_body.length())) {
             continue;
          }
       }
       if ( config.has_hw_id && psensor->has_hw_id()) {
          if ( psensor->get_hw_id() != config.hw_id) {
             continue;
          }
       }
       target_suid = candidate_suid;    // found target
       break;
    }

    stringstream ss;
    ss << "-sensor=" << config.data_type << " ";
    if ( config.has_name) {
       ss << "-name=" << config.name << " ";
    }
    if ( config.has_on_change) {
       ss << "-on_change=" << to_string(config.on_change) << " ";
    }
    if ( config.has_stream_type) {
       ss << "-stream_type=" << to_string(config.stream_type) << " ";
    }
    if ( config.has_rigid_body) {
       ss << "-rigid_body=" << config.rigid_body << " ";
    }
    if ( config.has_hw_id) {
       ss << "-hw_id=" << to_string(config.hw_id) << " ";
    }
    if ( target_suid == NULL) {
       oss << "FAIL " << ss.str() << " not found";
       writeline(oss);
       common_exit(FAILURE);
    }
    oss << ss.str() << " found ";
    append_suid(oss,target_suid);
    writeline(oss);

    if ( !config.has_testtype) {
       config.testtype = SELF_TEST_TYPE_FACTORY;
    }

    int rc = self_test_sensor( psalt, target_suid,
                      (see_self_test_type_e)config.testtype);
    oss << "sleep " << config.duration << " seconds";
    writeline(oss);
    if (rc == SALT_RC_SUCCESS) {
        psalt->update_event_cb(target_suid, (event_cb_func)event_cb);
    } else {
        oss << "Start self_test_sensor failed";
        writeline(oss);
    }
    psalt->sleep( config.duration); // seconds
    disable_sensor(psalt, target_suid, target_sensor); // SENSORTEST-1414
    int retcode = self_test_result? SUCCESSFUL: FAILURE;
    common_exit(retcode);
}

static void common_exit(int retcode)
{
    string passfail = (retcode == SUCCESSFUL) ? "PASS" : "FAIL";

    write_result_description( passfail);
    oss << passfail << " " << cmdline;
    writeline(oss);

    cout << passfail << " " << cmdline << endl;
    exit(retcode);
}

static void append_suid(ostringstream &outss, sens_uid *suid)
{
    outss << "suid = 0x" << hex << setfill('0')
                         << setw(16) << suid->high
                         << setw(16) << suid->low
                         << dec;
}

static void append_log_time_stamp( ostringstream &outss)
{
    std::chrono::system_clock::time_point start_time;
    const auto end_time = std::chrono::system_clock::now();
    auto delta_time = end_time - start_time;
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>
                                                            (delta_time);
    delta_time -= hrs;
    const auto mts = std::chrono::duration_cast<std::chrono::minutes>
                                                            (delta_time);
    delta_time -= mts;
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>
                                                            (delta_time);
    delta_time -= secs;
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>
                                                            (delta_time);
    outss  << std::setfill('0')
           << std::setw(2) << hrs.count() % 24 << ":"
           << std::setw(2) << mts.count() << ':'
           << std::setw(2) << secs.count() << '.'
           << std::setw(3) << ms.count() << ' '
           << std::setfill(' ') << std::left;
}

/**
* primary stdout writes timestamped line
*
* @param outss
*/
static void writeline(ostringstream &outss)
{
    ostringstream tss;
    append_log_time_stamp( tss);
    cout << tss.str() << outss.str() << endl;
    outss.str(""); // reset outss to ""
}

/*
** append result_description to output file
** supports multiple concurrent ssc_drva_tests
*/
static void write_result_description( string passfail)
{
    static string results_fn("/data/sensor_test_result");

    ostringstream tss;
    append_log_time_stamp( tss);
    string timestamp = tss.str();

   int fd = open( results_fn.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
   if ( fd >= 0) {
      if ( flock(fd, LOCK_EX) == 0) {
         ssize_t len = write( fd, passfail.c_str(), passfail.length());
         // keep compiler happy by using return value
         len += write( fd, ": ", 2);
         len = write( fd, timestamp.c_str(), timestamp.length());
         len = write( fd, cmdline.c_str(), cmdline.length());
         len += write( fd, newline.c_str(), newline.length());
      }
      else {
         cout << " FAIL flock failed." << endl;
      }
      close(fd);
   }
   else {
      cout << " FAIL open failed for " << results_fn << endl;
   }
}
