/*
 * Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <algorithm>
#include <vector>
#include "sensors_log.h"
#include "sensor.h"
#include "sensors_hal_common.h"
#include <unistd.h>
using namespace std;
using ::android::hardware::sensors::V1_0::SensorStatus;
typedef ::android::hardware::sensors::V1_0::SensorType SensorType;
typedef ::android::hardware::sensors::V1_0::MetaDataEventType MetaDataEventType;

std::mutex sensor::_mutex;

static const uint32_t SNS_DIRECT_MIN_VERY_FAST_RATE_HZ = 440U;
static const uint32_t SNS_DIRECT_MIN_FAST_RATE_HZ = 110U;
static const uint32_t SNS_DIRECT_MIN_NORMAL_RATE_HZ = 28U;

#define SEC_PER_HOUR (60*60)
#define MSEC_TO_NS_CONVERSION 1000000

sensor::sensor(sensor_wakeup_type wakeup_type):
    _is_sync_request_enabled(false),
    _is_on_change_sp_enabled(false),
    _bwakeup(false)
{
    memset(&_sensor_info, 0x00, sizeof(sensor_t));
    set_required_permission("");
    if (wakeup_type == SENSOR_WAKEUP)
      _bwakeup = true;
    _previous_ts = 0;
    _wakelock_inst = sns_wakelock::get_instance();
    _is_deactivate_in_progress = false;
    _event_count = 0;
    _event_cb = nullptr;
}

void sensor::set_config(sensor_params params)
{
    /* clamp the sample period according to the min and max delays supported by
       sensor.*/
    uint64_t min_delay_ns = _sensor_info.minDelay < 0 ? NSEC_PER_MSEC:
                                _sensor_info.minDelay * NSEC_PER_USEC;
    uint64_t max_delay_ns = _sensor_info.maxDelay < 0 ?
                                SEC_PER_HOUR * NSEC_PER_SEC:
                                _sensor_info.maxDelay * NSEC_PER_USEC;

    sns_logd("sp_requested = %" PRIu64, params.sample_period_ns);
    sns_logd("min_delay=%" PRIu64 "ns max_delay=%" PRIu64 "ns",
             min_delay_ns, max_delay_ns);
    if ((_is_on_change_sp_enabled == true && get_reporting_mode() != SENSOR_FLAG_ON_CHANGE_MODE) ||
        (_is_on_change_sp_enabled == false)) {
       if (params.sample_period_ns < min_delay_ns) {
            params.sample_period_ns = max(min_delay_ns, (uint64_t) NSEC_PER_MSEC);
        } else if (params.sample_period_ns > max_delay_ns) {
            params.sample_period_ns = max_delay_ns;
        }
        sns_logi("sample_period_ns is adjusted to %" PRIu64 " based on min/max delay_ns", params.sample_period_ns);
    } else {
        // on-change sensor case, we will convert sample_period to batch_period
        // since, there is no min/max Delay for on-change sensor type so, accept sample_period value as it is
        sns_logi("no need to adjust sample_period_ns due to on_change_sp_enabled = %d or reporting_mode = %u",
            _is_on_change_sp_enabled, get_reporting_mode());
    }

    sns_logd("sp_set = %" PRIu64 "ns, active = %d",
             params.sample_period_ns, is_active());

    /* if new params are same as existing params, no update required. */
    if (!(params == _params)) {
        _params = params;
        /* if sensor is active, update configuration */
        if (is_active()) {
            update_config();
        }
    }
}

/*Returns True if current sensor supports direct channel, False otherwise.
 * direct_channel supported sensors list is h/coded and depends on DSP*/
bool sensor::is_direct_channel_supported_sensortype(int type)
{
    switch (type) {
        case SENSOR_TYPE_ACCELEROMETER:
        case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
        case SENSOR_TYPE_GYROSCOPE:
        case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
        case SENSOR_TYPE_MAGNETIC_FIELD:
        case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
            return true;
            break;
        default:
            return false;
            break;
    }
}
/* set direct channel flags */
void sensor::set_direct_channel_flag(bool en)
{
    /*This Method will be called for each sensor just after sensors_init.
     * but below flags can be applied only if it is valid sensor (A/G/M).
     *  but not for Others. below code filters if it is valid or not */
    if(false == en)
        return;
    if(false == is_direct_channel_supported_sensortype(_sensor_info.type)) {
        return;
    } else {
        sns_logi("sensor[%s] _is_direct_channel_supported value is 1" , _sensor_info.stringType);
    }

    if (en && _min_low_lat_delay != 0) {
        _sensor_info.flags |= SENSOR_FLAG_DIRECT_CHANNEL_GRALLOC;
    } else {
        _sensor_info.flags &=
            ~(SENSOR_FLAG_DIRECT_CHANNEL_GRALLOC | SENSOR_FLAG_DIRECT_CHANNEL_ASHMEM );
        return;
    }

    /* Set rate flag according sensor minimum delay */
    if ( (unsigned long long)_min_low_lat_delay <=
            USEC_PER_SEC / SNS_DIRECT_MIN_VERY_FAST_RATE_HZ ) {
        _sensor_info.flags |=
            SENSOR_DIRECT_RATE_VERY_FAST << SENSOR_FLAG_SHIFT_DIRECT_REPORT;
    } else if ((unsigned long long)_min_low_lat_delay <=
            USEC_PER_SEC / SNS_DIRECT_MIN_FAST_RATE_HZ ) {
        _sensor_info.flags |=
            SENSOR_DIRECT_RATE_FAST << SENSOR_FLAG_SHIFT_DIRECT_REPORT;
    } else if ((unsigned long long)_min_low_lat_delay <=
            USEC_PER_SEC / SNS_DIRECT_MIN_NORMAL_RATE_HZ ) {
        _sensor_info.flags |=
            SENSOR_DIRECT_RATE_NORMAL << SENSOR_FLAG_SHIFT_DIRECT_REPORT;
    } else {
        _sensor_info.flags &=
            ~(SENSOR_FLAG_DIRECT_CHANNEL_GRALLOC | SENSOR_FLAG_DIRECT_CHANNEL_ASHMEM);
        sns_loge("_min_low_lat_delay=(%d) not meet requirement, direct channel disabled.",
            _min_low_lat_delay);
    }
}

/* function to submit a new event to sensor HAL clients */
void sensor::submit_sensors_hal_event()
{
  if(true == _is_deactivate_in_progress) {
    events.clear();
    return;
  }
  if (events.size()) {
    submit();
  } else {
    sns_logi("%s: trying to push empty Events, count = %zu", get_sensor_info().stringType, _event_count);
  }
}

void sensor::submit()
{
    sns_logd("post events , %s %zu, %d", get_sensor_info().stringType, events.size(), _bwakeup);
    if (_event_cb == nullptr) {
        sns_loge("_event_cb is null");
        return;
    }
    try {
        //TODO: make sure we really don't need to control wake-lock here
        _event_cb(events,  _bwakeup);
        events.clear();
    } catch (const std::bad_function_call& e) {
        sns_loge("error %s", e.what());
    }
}
