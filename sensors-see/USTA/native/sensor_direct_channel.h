/*============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/


/************************************************************/
/*Usage of this Header file is limited to this module alone */
/************************************************************/

/*one object per channel*/
#ifdef SNS_SUPPORT_DIRECT_CHANNEL
#include "sns_direct_channel.pb.h"
#include "logging_util.h"
using namespace std;

typedef struct direct_channel_stream_info {
  unsigned long suid_low;
  unsigned long suid_high;
  bool is_calibrated;
  bool is_resampled;
  bool has_calibrated;
  bool has_fixed_rate;
}direct_channel_stream_info;

typedef struct direct_channel_mux_ch_attrb {
  unsigned long sensor_handle;
  unsigned long sensor_type;
}direct_channel_mux_channel_attributes;


typedef struct sensor_direct_channel_srd_req_fileds {
  unsigned long batch_period;
  bool has_batch_period = false;

  unsigned long flush_period;
  bool has_flush_period = false;

  bool is_flush_only;
  bool has_flush_only = false;

  bool is_max_batch;
  bool has_max_batch = false;

  bool is_passive;
  bool has_passive = false;
}sensor_direct_channel_send_req_params;

class sensor_direct_channel {
public:

  sensor_direct_channel(
      unsigned long shared_memory_pointer,
      unsigned long shared_memory_size,
      direct_channel_type in_channel_type,
      sns_std_client_processor in_client_type = SNS_STD_CLIENT_PROCESSOR_APSS
      );

  int update_offset(
      int64_t offset = 0
      );

  int send_request(
      unsigned int msg_id,
      direct_channel_stream_info stream_info,
      sensor_direct_channel_srd_req_fileds std_req_fields_info,
      direct_channel_mux_ch_attrb attributes_info,
      bool has_attributes_info = false,
      string sensor_req_encoded_payload = nullptr
      );


  int stop_request(
      direct_channel_stream_info stream_info
      );

  ~sensor_direct_channel();

private:
  static uint64_t fastRPC_remote_handle;
  static void fastRPC_remote_handle_init();
  int channel_handle;
  unsigned long current_offset;
  direct_channel_type channel_type;
  sns_std_client_processor client_type;
  usta_logging_util *log_instance;

  static void *shared_memory_reading_thread(void *param);
  pthread_t shared_memory_thread_handle;
  void run();
  unsigned long fd_ptr;
  char* getAllocPtrFromFD(int fd);
  void dump_mux_channel_data(char *start_ptr, FILE *fp);
  void dump_generic_channel_data(char *start_ptr, FILE *fp);
};
#endif
