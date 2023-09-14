/*=============================================================================
  @file sns_client_test_example.cpp

  Example client written in C++, using libssc.  Client requests Accel SUID,
  and subsequently sends an enable request to it.  Data will stream for 10
  seconds.

  Copyright (c) 2017-2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <iostream>
#include <cinttypes>
#include <unistd.h>
#include "ssc_suid_util.h"
#include <string>
#include <unordered_map>
#include <vector>

#include "sns_std_sensor.pb.h"
#include "sns_std_type.pb.h"
#include "sns_client.pb.h"

using namespace std;

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

static int total_samples_rxved = 0;
static char *my_exe_name = (char *)"";

struct app_config {
    bool  is_valid = true;
    char  data_type[32];          // type: accel, gyro...
    float sample_rate = 25;	  	  // 10 Hz
    float batch_period = 0.0;	  // 0 ms
    float test_length = 20;       // 10 Seconds

    app_config() {}

}cfgargs;

/*=============================================================================
  Static Data
  ===========================================================================*/

static ssc_connection *connection;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/
static void usage(void)
{
    cout << "usage: " << my_exe_name;
    cout << " -sensor=<sensor_type>";
    cout << " -sample_rate=<number>";
    cout << " -batch_period=<seconds>";
    cout << " -test_length=<seconds>" << endl;
    cout << "           where: <sensor_type> :: accel | gyro | ..." << endl;
}
/**
 * Event callback function, as registered with ssc_connection.
 */
static void
event_cb(const uint8_t *data, size_t size, uint64_t ts)
{
  sns_client_event_msg pb_event_msg;
  UNUSED_VAR(ts);

  sns_logv("Received QMI indication with length %zu", size);

  pb_event_msg.ParseFromArray(data, size);
  for(int i = 0; i < pb_event_msg.events_size(); i++)
  {
    const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
    sns_logv("Event[%i] msg_id=%i, ts=%llu", i, pb_event.msg_id(),
        (unsigned long long int)pb_event.timestamp());

    if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == pb_event.msg_id())
    {
      sns_std_error_event error;
      error.ParseFromString(pb_event.payload());

      sns_loge("Received error event %i", error.error());
    }
    else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT == pb_event.msg_id())
    {
      sns_std_sensor_physical_config_event config;
      config.ParseFromString(pb_event.payload());

      sns_loge("Received config event with sample rate %f", config.sample_rate());
    }
    else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == pb_event.msg_id())
    {
      sns_std_sensor_event event;
      event.ParseFromString(pb_event.payload());
      sns_logi("Received sample <%f, %f, %f>",
          event.data(0), event.data(1), event.data(2));
      total_samples_rxved++;
    }
    else
    {
      sns_loge("Received unknown message ID %i", pb_event.msg_id());
    }
  }
}

/**
 * Send a SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG to SUID
 */
static void
send_config_req(ssc_connection *conn, sensor_uid const *suid)
{
  string pb_req_msg_encoded;
  string config_encoded;
  sns_client_request_msg pb_req_msg;
  sns_std_sensor_config config;

  sns_logi("Send config request with sample rate %lf and batch period %lf",
                cfgargs.sample_rate, cfgargs.batch_period);

  config.set_sample_rate(cfgargs.sample_rate);
  config.SerializeToString(&config_encoded);

  pb_req_msg.set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
  pb_req_msg.mutable_request()->set_payload(config_encoded);
  pb_req_msg.mutable_suid()->set_suid_high(suid->high);
  pb_req_msg.mutable_suid()->set_suid_low(suid->low);
  pb_req_msg.mutable_susp_config()->
    set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->
    set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
  pb_req_msg.mutable_request()->mutable_batching()->
    set_batch_period(cfgargs.batch_period);
  pb_req_msg.SerializeToString(&pb_req_msg_encoded);
  conn->send_request(pb_req_msg_encoded);
}

/**
 * SUID callback as registered with suid_lookup.
 */
static void
suid_cb(const std::string& datatype, const std::vector<sensor_uid>& suids)
{
  sns_logv("Received SUID event with length %zu", suids.size());
  if(suids.size() > 0)
  {
    sensor_uid suid = suids.at(0);
    connection = new ssc_connection(event_cb);

    sns_logi("Received SUID %" PRIx64 "%" PRIx64 " for '%s'",
        suid.high, suid.low, datatype.c_str());

    printf("Received SUID %" PRIx64 "%" PRIx64 " for '%s' \n",
        suid.high, suid.low, datatype.c_str());
    send_config_req(connection, &suid);
  } else {
  sns_logi("%s sensor is not available", cfgargs.data_type);
  printf("%s sensor is not available \n", cfgargs.data_type);
    exit(-1);
  }
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

app_config parse_arguments(int argc, char *argv[])
{
    app_config config;

    // command line args:
    string sensor_eq( "-sensor");
    string batch_period_eq( "-batch_period");
    string sample_rate_eq( "-sample_rate");
    string test_length_eq( "-test_length");

    my_exe_name = argv[0];
    for ( int i = 1; i < argc; i++) {
        if (( 0 == strcmp( argv[i], "-h"))
              || ( 0 == strcmp( argv[i], "-help"))) {
           config.is_valid = false;
           break;
        }
        string key;
        string value;
        if ( get_key_value( argv[i], key, value)) {
           if ( sensor_eq == key) {
              strlcpy( config.data_type,
                       value.c_str(),
                       sizeof( config.data_type));
           }
           else if ( sample_rate_eq == key) {
              config.sample_rate = atof( value.c_str());
           }
           else if ( batch_period_eq == key) {
              config.batch_period = atof( value.c_str());
           }
           else if ( test_length_eq == key) {
              config.test_length = atof( value.c_str());
           }
           else {
               config.is_valid = false;
               cout << "unrecognized arg " << argv[i] << endl;
           }
        }
        else {
            config.is_valid = false;
            cout << "unrecognized arg " << argv[i] << endl;
        }
    }
    if ( !config.is_valid) {
       usage();
       exit( 4);
    }

    return config;
}

int
main(int argc, char *argv[])
{
        cfgargs = parse_arguments(argc, argv);

        printf("##################################################################################################\n");
        printf("streaming started for '%s' set SR/RR '(%f/%f)Hz' and duration '%fSec' \n", cfgargs.data_type,
                                                cfgargs.sample_rate, cfgargs.batch_period, cfgargs.test_length);
        sns_logi("streaming started for '%s' set SR/RR '(%f/%f)Hz' and duration '%fSec' ", cfgargs.data_type,
                                                cfgargs.sample_rate, cfgargs.batch_period, cfgargs.test_length);

        suid_lookup lookup(suid_cb);

        lookup.request_suid(cfgargs.data_type);
        sleep(cfgargs.test_length);
        delete connection;
        sns_logi("Received %d samples for '%s' sensor, set SR/RR '(%f/%f)Hz' and duration '%fSec'", total_samples_rxved, cfgargs.data_type,
                                                cfgargs.sample_rate, cfgargs.batch_period, cfgargs.test_length);

        printf("Received %d samples for '%s' sensor, set SR/RR '(%f/%f)Hz' and duration '%fSec' \n", total_samples_rxved, cfgargs.data_type,
                                                cfgargs.sample_rate, cfgargs.batch_period, cfgargs.test_length);
        printf("##################################################################################################\n");
        return 0;
}

