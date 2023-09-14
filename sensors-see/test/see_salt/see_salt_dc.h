/* ===================================================================
** Copyright (c) 2021 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_salt_dc.h
** DESC: class definitions for the SEE direct channel testing
** ================================================================ */
#pragma once

enum see_sensor_stream_path_e{
  REGULAR_PATH = 0,
  DIRECT_CHANNEL_PATH
};

enum see_sensor_channel_type_e {
   STRUCTURED_MUX = 0,
   GENERIC
};

enum see_sensor_client_type_e {
  CLIENT_SSC = 0,
  CLIENT_APSS,
  CLIENT_ADSP,
  CLIENT_MDSP,
  CLIENT_CDSP
};

typedef struct direct_channel_attributes{
    float max_sampling_rate;
    float max_report_rate;
} direct_channel_attributes;

class see_create_dc_channel_info
{
public:
    unsigned long shared_memory_ptr;
    unsigned long shared_memory_size;
    see_sensor_channel_type_e channel_type_value;
    see_sensor_client_type_e client_type_value;

    see_create_dc_channel_info()
    {
        shared_memory_ptr = -1;
        shared_memory_size = 0;
        channel_type_value = STRUCTURED_MUX;
        client_type_value = CLIENT_APSS;
    }

    void set_shared_memory_ptr(unsigned long mem_ptr)
    {
        shared_memory_ptr = mem_ptr;
    }

    void set_shared_memory_size(unsigned long mem_size)
    {
        shared_memory_size = mem_size;
    }

    void set_channel_type_value(see_sensor_channel_type_e channel_type_val)
    {
        channel_type_value = channel_type_val;
    }

    void set_client_type_value(see_sensor_client_type_e client_type_val)
    {
        client_type_value = client_type_val;
    }
};

class see_dc_std_req_info
{
public:
    unsigned int sensor_handle;
    std::string is_calibrated;
    std::string is_fixed_rate;
    std::string sensor_type;
    std::string batch_period;
    float sample_rate;

    see_dc_std_req_info()
    {
        sensor_handle = 0;
        is_calibrated = "false";
        is_fixed_rate = "false";
        sensor_type = "1";
        batch_period = "";
        sample_rate = 0;
    }

    void set_sensor_handle(unsigned int sens_handle)
    {
        sensor_handle = sens_handle;
    }

    void set_calibrated(std::string is_cal)
    {
        is_calibrated = is_cal;
    }

    void set_fixed_rate(std::string is_fixed_r)
    {
        is_fixed_rate = is_fixed_r;
    }

    void set_sensor_type(std::string sens_type)
    {
        sensor_type = sens_type;
    }

    void set_batch_period(std::string period)
    {
        batch_period = period;
    }

    void set_sample_rate(float rate)
    {
        sample_rate = rate;
    }
};

