/*============================================================================
  @file direct_channel.h

  @brief
  direct_channel class definition.

  Copyright (c) 2017-2018 , 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/* |============================================|
 * |==== TARGET  >>>>>>>>  IQtiMapper_Vers =====|
 * |============================================|
 * |==== Lahaina >>>>>>>>  IQtiMapper3_0   =====|
 * |                                            |
 * |==== Kona    >>>>>>>>  IQtiMapper3_0   =====|
 * |==== Lito    >>>>>>>>  IQtiMapper3_0   =====|
 * |==== Atoll   >>>>>>>>  IQtiMapper3_0   =====|
 * |==== Bengal  >>>>>>>>       NA         =====|
 * |                                            |
 * |==== msmnile >>>>>>>>  IQtiMapper3_0   =====|
 * |==== SM6150  >>>>>>>>  IQtiMapper3_0   =====|
 * |==== Trinket >>>>>>>>       NA         =====|
 * |                                            |
 * |==== SDM845  >>>>>>>>  IQtiMapper1_1   =====|
 * |==== SDM710  >>>>>>>>  IQtiMapper1_1   =====|
 * |============================================|
 */
#ifndef SNS_SUPPORT_DIRECT_CHANNEL
#pragma once


#include <cstdlib>
#include <cutils/native_handle.h>
#include <android/hardware/graphics/mapper/3.0/IMapper.h>
#include <android/hardware/graphics/mapper/2.0/IMapper.h>
#include <vendor/qti/hardware/display/mapper/3.0/IQtiMapper.h>
#include <vendor/qti/hardware/display/mapper/1.1/IQtiMapper.h>
#include "sensor.h"
#include "sns_low_lat_stream.h"
extern "C" {
  #include "sns_fastRPC_utils.h"
}
typedef int (*direct_channel_stream_init_t)(int fd, unsigned int size, unsigned int offset, int* handle);
typedef int (*direct_channel_stream_deinit_t)(int handle);
typedef int (*direct_channel_stream_config_t)(int handle, int64 timestamp_offset,
        const sns_std_suid_t* sensor_suid, unsigned int sample_period_us,
        unsigned int flags, int sensor_handle);
typedef int (*direct_channel_stream_stop_t)(int handle);
typedef int (*direct_channel_offset_update_t)(int64 timestamp_offset, int64 slope);

using IQtiMapper3_0  = ::vendor::qti::hardware::display::mapper::V3_0::IQtiMapper;
using IQtiMapper1_1  = ::vendor::qti::hardware::display::mapper::V1_1::IQtiMapper;
using IQtiMapperExtensions1_0 = ::vendor::qti::hardware::display::mapperextensions::V1_0::IQtiMapperExtensions;

/*============================================================================
 * Class direct_channel
 *=============================================================================*/
class direct_channel {
public:
    /**
     * @brief constructor
     *
     * @param[in] mem_handle handle of share memory
     * @param[in] mem_size size of share memory block
     */

    direct_channel(const struct native_handle *mem_handle,
        const size_t mem_size);
    /**
     * @brief copy constructor
     *
     * @param[in] direct channel object to copy
     */
    direct_channel(const direct_channel &copy);

    /**
     * @brief destructor
     */
    ~direct_channel();

    /**
     * @brief get channel handle on remote side
     *
     * @return int handle
     */
    int get_low_lat_handle();

    /**
     * @brief get channel handle on Android side
     *
     * @return int handle
     */
    int get_client_channel_handle();

    /**
     * @brief get file descriptor of share memory
     *
     * @return int fd
     */
    int get_buffer_fd();

    /**
     * @brief check if memory buffer is already register to current channel object
     *
     * @param[in] handle of memory block to be validated
     *
     * @return bool true if this block of memory is already registered, false otherwise
     */
    bool is_same_memory(const struct native_handle *mem_handle);

    /**
     * @brief configure sensor in particular direct report channel.
     *
     * @param[in] handle Handle of direct report channel
     * @param[in] timestamp_offset Timestamp offset between AP and SSC.
     * @param[in] sensor_suid Sensor to configure
     * @param[in] sample_period_us Sensor sampling period, set this to zero will stop the sensor.
     * @param[in] flags Configure flag of sensor refer to "SNS_LLCM_FLAG_X"
     * @param[in] sensor_handle Sensor identifier used in Android.
     *
     * @return int 0 on success, negative on failure
     */
    int config_channel(int handle, int64 timestamp_offset,
        const sns_std_suid_t* sensor_suid, unsigned int sample_period_us,
        unsigned int flags, int sensor_handle);

    /**
     * @brief Disable all sensors in particular channel.
     *
     * @param[in] handle Handle of direct report channel
     *
     * @return int 0 on success, negative on failure
     */
    int stop_channel(int handle);

    /**
     * @brief Update the timestamp offset between AP and SSC
     *
     * @param[in] new_offset Timestamp offset
     */
    void update_offset(int64_t new_offset);

    /**
     * @brief reset direct channel resource
     *
     */
    static void reset(void);


private:
    struct native_handle* mem_native_handle;
    void* buffer_ptr;
    int buffer_size;
    int sns_low_lat_handle;

    /* llcm realted handle and function pointer */
    static void *llcmhandler;
    static direct_channel_stream_init_t direct_channel_stream_init;
    static direct_channel_stream_deinit_t direct_channel_stream_deinit;
    static direct_channel_stream_config_t direct_channel_stream_config;
    static direct_channel_stream_stop_t   direct_channel_stream_stop;
    static direct_channel_offset_update_t direct_channel_offset_update;

    static sns_remote_handles remote_handles;

    /* to indicate whether llcm and fastrpc resources are
       initialized or not */
    static bool is_llcm_initialized;

    /* Assigned by the constructor. If this field is 0, then the object is invalid */
    int client_channel_handle;

    /* Note: valid range for Android channel handles is (>0, <INT32_MAX) */
    static int client_channel_handle_counter;
    android::sp<IQtiMapper3_0> qtiMapper3;
    android::sp<IQtiMapper1_1> qtiMapper1_1;
    android::sp<IQtiMapperExtensions1_0> qtiMapperExtensions;
    buffer_handle_t importedHandle;

    /**
     * @brief initialize imapper
     *
     * @return int 0 on success, negative on failure
     */
    int init_mapper(void);

    /**
     * @brief Load direct channel fasrRPC remote symbols
     *
     * @return int 0 on success, negative on failure
     */
    int init_fastRPC_symbols(void);
};
#endif
