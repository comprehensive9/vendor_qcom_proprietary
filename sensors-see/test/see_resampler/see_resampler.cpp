/* ===================================================================
** Copyright (c) 2017-2018,2020 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_resampler.cpp
** DESC: run resampler sensor using command line argumentss
** ================================================================ */
#include <iostream>
#include <iomanip>    // std::setprecision
#include <sstream>
#include <unistd.h>
#include <string>
#include <sys/file.h>
#include <sys/time.h>
#include "see_salt.h"

#ifdef SNS_TARGET_LE_PLATFORM
#include <chrono>
#endif
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

using namespace std;

static void common_exit(see_salt *psalt, int retcode);
static void append_suid(ostringstream &outss, sens_uid *suid);
static void append_log_time_stamp( ostringstream &outss);
static void writeline(ostringstream &outss);
static void write_result_description( string passfail);

struct app_config {
    bool  is_valid = false;
    char  data_type[ 32];           // type: accel, gyro, mag, ...

    bool  has_name = false;
    char  name[ 32];

    bool  has_rigid_body = false;
    char  rigid_body[ 32];

    bool  has_hw_id = false;
    int   hw_id;

    bool  has_batch_period = false;
    float batch_period;             // seconds

    bool  has_flush_period = false;
    float flush_period;             // seconds

    bool  has_flush_only = false;
    bool  flush_only;

    bool  has_max_batch = false;
    bool  max_batch;

    bool  has_flush_request_interval = false;
    float flush_request_interval;

    float sample_rate = 10.0;       // hz

    see_resampler_rate  rate_type = SEE_RESAMPLER_RATE_MINIMUM;

    bool  filter = false;

    bool  has_calibrated = false;
    bool  calibrated;

    bool  has_delay = false;
    float delay;

    float duration = 10.0;          // seconds

    app_config() {}
};

static app_config config;

static string newline("\n");
static string cmdline("");
static ostringstream oss;

#define SUCCESSFUL 0
#define FAILURE 4

/**
 * @brief parse command line argument of the form keyword=value
 * @param parg[i] - one command line argument
 * @param key[io] - gets string to left of '='
 * @param value[io] - sets string to right of '='
 * @return true - successful parse. false - parse failed.
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

void usage(void)
{
    cout << "usage: see_resampler"
            " -sensor=<sensor_type>"
            " [-name=<name>]"
            " [-rigid_body=<display | keyboard>]"
            " [-hw_id=<number>]\n"
            "       -sample_rate=<hz>"
            " [-rate_type=<fixed | minimum>]"
            " [-filter=<0 | 1>\n"
            "       [-batch_period=<seconds>]"
            " [-flush_period=<seconds>]"
            " [-max_batch=<0 | 1>] [-flush_only=<0 | 1>]\n"
            "       [-flush_request_interval=<seconds>]\n"
            "       [-calibrated=<0 | 1>]"
            " [-delay=<seconds>]"
            " -duration=<seconds>\n"
            "where: <sensor_type> :: accel | gyro | ...\n";
}

int parse_arguments(int argc, char *argv[])
{
    // command line args:
    string sensor_eq( "-sensor");
    string name_eq( "-name");
    string rigid_body_eq( "-rigid_body");
    string hw_id_eq( "-hw_id");
    string sample_rate_eq( "-sample_rate");
    string rate_type_eq( "-rate_type");
    string filter_eq( "-filter");
    string batch_period_eq( "-batch_period");
    string flush_period_eq( "-flush_period");
    string flush_request_interval_eq( "-flush_request_interval");
    string flush_only_eq( "-flush_only");
    string max_batch_eq( "-max_batch");
    string calibrated_eq( "-calibrated");
    string delay_eq( "-delay");
    string duration_eq( "-duration");

    config.is_valid = true;
    for ( int i = 1; i < argc; i++) {
        string key;
        string value;
        if ( !get_key_value( argv[i], key, value)) {
           if ( 0 == strcmp( argv[i], "-h")
                ||  0 == strcmp( argv[i], "-help")) {
              usage();
              exit(SUCCESSFUL);
           }
           config.is_valid = false;
           cout << "FAIL unrecognized arg " << argv[i] << endl;
           continue;
        }
        if ( sensor_eq == key) {
           strlcpy( config.data_type,
                    value.c_str(),
                    sizeof( config.data_type));
        }
        else if ( name_eq == key) {
           config.has_name = true;
           strlcpy( config.name, value.c_str(), sizeof( config.name));
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
        else if ( max_batch_eq == key) {
           config.has_max_batch = true;
           config.max_batch = atoi( value.c_str());
        }
        else if ( flush_only_eq == key) {
           config.has_flush_only = true;
           config.flush_only = atoi( value.c_str());
        }
        else if ( sample_rate_eq == key) {
           config.sample_rate = atof( value.c_str());
        }
        else if ( rate_type_eq == key) {
           if ( value == string("fixed")) {
              config.rate_type = SEE_RESAMPLER_RATE_FIXED;
           }
           else if ( value == string("minimum")) {
              config.rate_type = SEE_RESAMPLER_RATE_MINIMUM;
           }
           else {
              config.is_valid = false;
              cout << "FAIL bad rate type value: " << value << endl;
           }
        }
        else if ( filter_eq == key) {
           config.filter = atoi( value.c_str());
        }
        else if ( batch_period_eq == key) {
           config.has_batch_period = true;
           config.batch_period = atof( value.c_str());
        }
        else if ( flush_period_eq == key) {
           config.has_flush_period = true;
           config.flush_period = atof( value.c_str());
        }
        else if ( flush_request_interval_eq == key) {
           config.has_flush_request_interval = true;
           config.flush_request_interval = atof( value.c_str());
        }
        else if ( calibrated_eq == key) {
           config.has_calibrated = true;
           config.calibrated = atoi( value.c_str());
        }
        else if ( delay_eq == key) {
           config.has_delay= true;
           config.delay = atof( value.c_str());
        }
        else if ( duration_eq == key) {
           config.duration = atof( value.c_str());
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

salt_rc on_change_sensor(see_salt *psalt, sens_uid *suid, string target)
{
   oss << "on_change_sensor( " << target << ")";
   writeline(oss);

   see_std_request std_request;
   see_client_request_message request( suid,
                                       MSGID_ON_CHANGE_CONFIG,
                                       &std_request);
   salt_rc rc = psalt->send_request(request);
   oss << "on_change_sensor() complete rc " << to_string(rc);
   writeline(oss);
   return rc;
}

void init_std_request(see_std_request *std_request, app_config *pconfig)
{
   if ( pconfig->has_batch_period) {
      oss << "batch_period: " << to_string( pconfig->batch_period)
           << " seconds";
      writeline(oss);
      std_request->set_batch_period( pconfig->batch_period * 1000000); // microsec
   }
   if ( pconfig->has_flush_period) {
      oss << "flush_period: " << to_string( pconfig->flush_period)
           << " seconds";
      writeline(oss);
      std_request->set_flush_period( pconfig->flush_period * 1000000); // microsec
   }
   if ( pconfig->has_flush_only) {
      oss << "flush_only: " << to_string( pconfig->flush_only);
      writeline(oss);
      std_request->set_flush_only( pconfig->flush_only);
   }
   if ( pconfig->has_max_batch) {
      oss << "max_batch: " << to_string( pconfig->max_batch);
      writeline(oss);
      std_request->set_max_batch( pconfig->max_batch);
   }
}

void resample_sensor(see_salt *psalt,
                     sens_uid *resampler_suid,
                     sens_uid *target_suid,
                     app_config *pconfig)
{
   oss << "resample_sensor()";
   writeline(oss);
   see_std_request std_request;
   init_std_request( &std_request, pconfig);

   see_resampler_config payload( target_suid,
                                 pconfig->sample_rate,
                                 pconfig->rate_type,
                                 pconfig->filter);

   std_request.set_payload( &payload);
   see_client_request_message request( resampler_suid,
                                       MSGID_RESAMPLER_CONFIG,
                                       &std_request);
   psalt->send_request(request);
   oss << "resample_sensor() complete";
   writeline(oss);
}

void disable_sensor(see_salt *psalt, sens_uid *suid, string sensor_type)
{
   oss << "disable_sensor( " << sensor_type << ")";
   writeline(oss);
   see_std_request std_request;
   see_client_request_message request( suid, MSGID_DISABLE_REQ, &std_request);
   psalt->send_request(request);
   oss << "disable_sensor() complete";
   writeline(oss);
}

salt_rc flush_sensor(see_salt *psalt, sens_uid *suid, string target)
{
   oss << "flush_sensor( " << target << ")";
   writeline(oss);
   see_std_request std_request;
   see_client_request_message request( suid, MSGID_FLUSH_REQ, &std_request);
   salt_rc rc = psalt->send_request(request);
   oss << "flush_sensor() complete rc " << to_string(rc);
   writeline(oss);
   return rc;
}

/**
 * does_rigid_body_match() is intended to find the mag_cal that matches
 * the rigid_body of selected mag sensor, or the gyro_cal matching the
 * selected gyro sensor.
 *
 * @param psensor
 *
 * @return true when the _cal sensor matches config.rigid_body
 */
bool does_rigid_body_match(sensor *psensor)
{
   if ( psensor && psensor->has_rigid_body()) {
      string rigid_body = psensor->get_rigid_body();
      if ( 0 != strncmp( config.rigid_body, rigid_body.c_str(),
                         rigid_body.length())) {
         return false;
      }
   }
   return true;
}

// return pointer to target_sensor's suid
sens_uid *get_sensor_suid( see_salt *psalt, string target)
{
    if ( psalt == nullptr)
       return nullptr;

    oss << "+ get_sensor_suid( " << target << ")";
    writeline(oss);

    vector<sens_uid *> sens_uids;
    psalt->get_sensors( target, sens_uids);
    if ( sens_uids.size() == 0) {
       oss << "FAIL " << target << " suid not found" << endl;
       writeline(oss);
       return nullptr;
    }

    size_t index = 0;
    if (target.find( "_cal") != string::npos) {
       for ( ; index < sens_uids.size(); index++) {
          sens_uid *candidate_suid = sens_uids[index];
          sensor *psensor = psalt->get_sensor( candidate_suid);
          // need only xxx_cal rigid_body same as xxx sensor's rigid body
          if ( !does_rigid_body_match( psensor)) {
             continue;
          }
          oss << "+ found " << target << " " << psensor->get_rigid_body();
          writeline(oss);
          break;
       }
    }
    index = ( index < sens_uids.size()) ? index : sens_uids.size() - 1;
    oss << "+ " << target << " ";
    append_suid(oss,sens_uids[index]);
    writeline(oss);

    return sens_uids[index];
}

int main(int argc, char *argv[])
{
    cout << "see_resampler version 1.8" << endl;

    // save and display commandline
    for ( int i = 0; i < argc; i++) {
       oss << argv[i] << " ";
    }
    cmdline = oss.str();
    writeline(oss);

    if (parse_arguments(argc, argv)) {
       common_exit(nullptr, FAILURE);
    }

    if ( config.data_type[0] == '\0') {
       oss << "FAIL missing -sensor=<sensor_type>" << endl;
       writeline(oss);
       common_exit(nullptr, FAILURE);
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
        common_exit(nullptr, FAILURE);
    }

    psalt->begin();  // populate sensor attributes

    /*
    * lookup resampler's suid
    */
    sens_uid resampler_suid;
    string resampler = string( "resampler");
    oss << "lookup: " << resampler;
    writeline(oss);

    vector<sens_uid *> sens_uids;
    psalt->get_sensors( resampler, sens_uids);
    if (sens_uids.size()) {
       resampler_suid.low = sens_uids[0]->low;
       resampler_suid.high = sens_uids[0]->high;
    }
    else {
       oss << "FAIL resampler not found";
       writeline(oss);
       common_exit(psalt, FAILURE);
    }

    /*
    * lookup target sensors's suid
    */
    string target_sensor = string( config.data_type);
    oss << "lookup: " << target_sensor;
    writeline(oss);

    psalt->get_sensors( target_sensor, sens_uids);
    for (size_t i = 0; i < sens_uids.size(); i++) {
       sens_uid *suid = sens_uids[i];
       oss << target_sensor << " found ";
       append_suid(oss, suid);
       writeline(oss);
    }

    if ( !sens_uids.size()) {
       oss << "FAIL " << target_sensor << " not found";
       writeline(oss);
       common_exit(psalt, FAILURE);
    }

    sens_uid *target_suid = NULL;
    sensor *psensor = NULL;
    for (size_t i = 0; i < sens_uids.size(); i++) {
       sens_uid *candidate_suid = sens_uids[i];
       psensor = psalt->get_sensor( candidate_suid);
       if ( config.has_name) {
          if ( psensor->get_name() != config.name) {
             continue;
          }
       }
       if ( config.has_rigid_body) {
          if ( !psensor->has_rigid_body()) {
             continue;
          }
          if ( psensor->get_rigid_body() != config.rigid_body) {
             continue;
          }
       }
       if ( config.has_hw_id) {
          if ( !psensor->has_hw_id()) {
             continue;
          }
          if ( psensor->get_hw_id() != config.hw_id) {
             continue;
          }
       }
       // SENSORTEST-1119 default when rigid_body and hw_id are omitted
       if ( config.has_rigid_body == false &&  config.has_hw_id == false) {
          if ( psensor->has_rigid_body() ) { // SENSORTEST-1088
             if ( psensor->get_rigid_body() != "display") { // default
                continue;
             }
          }
       }

       target_suid = candidate_suid;    // found target
       if ( !config.has_rigid_body) {   // setup does_rigid_body_match()
          if ( psensor->has_rigid_body()) {
             string rigid_body = psensor->get_rigid_body();
             strlcpy( config.rigid_body, rigid_body.c_str(),
                      sizeof(config.rigid_body));
          }
       }
       break;
    }

    stringstream ss;
    ss << "-sensor=" << config.data_type << " ";
    if ( config.has_name) {
       ss << "-name=" << config.name << " ";
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
       common_exit(psalt, FAILURE);
    }
    oss << ss.str() << "using ";
    append_suid(oss,target_suid);
    writeline(oss);

    // conditionally lookup calibration control sensor
    sens_uid *calibrate_suid = NULL;
    string target_cal_control = "";
    if ( config.has_calibrated && config.calibrated) {
       if ( target_sensor == string("gyro")) {
          target_cal_control = "gyro_cal";
          calibrate_suid = get_sensor_suid( psalt, target_cal_control);
       }
       else if ( target_sensor == string("mag")) {
          target_cal_control = "mag_cal";
          calibrate_suid = get_sensor_suid( psalt, target_cal_control);
       }
       else {
           oss << "FAIL calibration not supported for " << target_sensor;
           writeline(oss);
       }
       if ( calibrate_suid == nullptr ) {
           common_exit(psalt, FAILURE);
       }
    }

    salt_rc rc = SALT_RC_SUCCESS;
    if ( calibrate_suid) {
       rc = on_change_sensor( psalt, calibrate_suid, target_cal_control);
    }

    resample_sensor( psalt, &resampler_suid, target_suid, &config);

    if ( config.has_flush_request_interval) { // SENSORTEST-1418
        while ( 1) {
            if ( config.duration > config.flush_request_interval) {
                psalt->sleep(config.flush_request_interval); // seconds
                rc = flush_sensor( psalt, &resampler_suid, resampler);
                config.duration -= config.flush_request_interval;
                continue;
            }
            psalt->sleep(config.duration);
            break;
        }
    }
    else {
        oss << "sleep " << to_string( config.duration) << " seconds";
        writeline(oss);
        psalt->sleep( config.duration); // seconds
    }
    disable_sensor( psalt, &resampler_suid, resampler);

    if ( rc == SALT_RC_SUCCESS && calibrate_suid) {
       disable_sensor( psalt, calibrate_suid, target_cal_control);
    }

    common_exit(psalt, SUCCESSFUL);
}

static void common_exit(see_salt *psalt, int retcode)
{
    string passfail = (retcode == SUCCESSFUL) ? "PASS" : "FAIL";

    write_result_description( passfail);
    oss << passfail << " " << cmdline;
    writeline(oss);

    if (psalt != nullptr ) { // SENSORTEST-1491
        oss << "begin delete psalt";
        writeline(oss);
        delete psalt;
        oss << "end delete psalt";
        writeline(oss);
    }

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


