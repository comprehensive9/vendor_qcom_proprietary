/*-------------------------------------------------------------------
Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

* Not a Contribution.

*Copyright (C) 2016 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
--------------------------------------------------------------------*/

#include <errno.h>
#include <hardware/sensors.h>
#include <math.h>
#include <sys/types.h>
#include <stdint.h>
#include "sensors_hw_module.h"
#include "wakelock_utils.h"
#include <cutils/properties.h>
#include "ScopedWakelock.h"

using Result = ::android::hardware::sensors::V1_0::Result;
using RateLevel = ::android::hardware::sensors::V1_0::RateLevel;
using SharedMemInfo = ::android::hardware::sensors::V1_0::SharedMemInfo;
using SharedMemType = ::android::hardware::sensors::V1_0::SharedMemType;
using SharedMemFormat = ::android::hardware::sensors::V1_0::SharedMemFormat;
using SensorType = ::android::hardware::sensors::V1_0::SensorType;
using SensorInfo = ::android::hardware::sensors::V1_0::SensorInfo;
using ScopedWakelock = ::android::hardware::sensors::V2_0::implementation::ScopedWakelock;

#define SEC_TO_NS_CONVERSION 1000000000

/*implementation of HAL 2.0 APIs*/
namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

/*helper functions*/
static Result ResultFromStatus(android::status_t err) {
    switch (err) {
        case android::OK:
            return Result::OK;
        case android::PERMISSION_DENIED:
            return Result::PERMISSION_DENIED;
        case android::NO_MEMORY:
            return Result::NO_MEMORY;
        case android::BAD_VALUE:
            return Result::BAD_VALUE;
        default:
            return Result::INVALID_OPERATION;
    }
}

bool convertFromSharedMemInfo(const SharedMemInfo& memIn, sensors_direct_mem_t *memOut) {
    if (memOut == nullptr) {
        return false;
    }

    switch(memIn.type) {
        case SharedMemType::ASHMEM:
            memOut->type = SENSOR_DIRECT_MEM_TYPE_ASHMEM;
            break;
        case SharedMemType::GRALLOC:
            memOut->type = SENSOR_DIRECT_MEM_TYPE_GRALLOC;
            break;
        default:
            return false;
    }

    switch(memIn.format) {
        case SharedMemFormat::SENSORS_EVENT:
            memOut->format = SENSOR_DIRECT_FMT_SENSORS_EVENT;
            break;
        default:
            return false;
    }

    if (memIn.memoryHandle == nullptr) {
        return false;
    }

    memOut->size = memIn.size;
    memOut->handle = memIn.memoryHandle;
    return true;
}

int convertFromRateLevel(RateLevel rate) {
    switch(rate) {
        case RateLevel::STOP:
            return SENSOR_DIRECT_RATE_STOP;
        case RateLevel::NORMAL:
            return SENSOR_DIRECT_RATE_NORMAL;
        case RateLevel::FAST:
            return SENSOR_DIRECT_RATE_FAST;
        case RateLevel::VERY_FAST:
            return SENSOR_DIRECT_RATE_VERY_FAST;
        default:
            return -1;
    }
}

void convertFromSensor(const sensor_t &src, SensorInfo *dst) {
    if (nullptr != dst) {
        dst->name = src.name;
        dst->vendor = src.vendor;
        dst->version = src.version;
        dst->sensorHandle = src.handle;
        dst->type = (SensorType)src.type;
        dst->maxRange = src.maxRange;
        dst->resolution = src.resolution;
        dst->power = src.power;
        dst->minDelay = src.minDelay;
        dst->fifoReservedEventCount = src.fifoReservedEventCount;
        dst->fifoMaxEventCount = src.fifoMaxEventCount;
        dst->typeAsString = src.stringType;
        dst->requiredPermission = src.requiredPermission;
        dst->maxDelay = src.maxDelay;
        dst->flags = src.flags;
    }
}

/*implementation start of hal 2.0 class methods */
sensors_hw_module::sensors_hw_module()
{
    sns_logi("create sensors sub-hal instance");
    _hal = sensors_hal::get_instance();
    _is_hal_configured = false;
    _is_direct_channel_supported = _hal->is_direct_channel_supported();
    sns_logi("sensors sub-hal instance is created");
}
sensors_hw_module::~sensors_hw_module()
{
}

void sensors_hw_module::clearActiveConnections()
{
    sns_logi("clear all active direct_channel clients");
    /*Deactivate the active direct_channel clients
      if direct_channel is enabled*/
    if(_is_direct_channel_supported == true) {
        unsigned int active_direct_channel_count = active_direct_channel_list.size();
        for( unsigned int i = 0 ; i < active_direct_channel_count; i++) {
            int32_t channelHandle = active_direct_channel_list[i];
            _hal->unregister_direct_channel((int)channelHandle);
        }
        active_direct_channel_list.clear();
    }
}

Return<Result> sensors_hw_module::initialize(
    const sp<IHalProxyCallback>& halProxyCallback) {
    sns_logi("initialize sensors sub-hal");
    if (_is_hal_configured) {
        clearActiveConnections();
    }
    _callback = halProxyCallback;
    _hal->register_event_callback(this);
    _is_hal_configured = true;
    return Result::OK;
}

Return<Result> sensors_hw_module::activate(int32_t handle, bool en)
{
    return ResultFromStatus(_hal->activate((int)handle,(int)en));
}

Return<Result> sensors_hw_module::batch(int32_t handle,
                     int64_t samplingPeriodNs,
                     int64_t maxReportLatencyNs)
{
    return ResultFromStatus(_hal->batch((int)handle, samplingPeriodNs,
                     maxReportLatencyNs));
}

Return<Result> sensors_hw_module::flush(int32_t handle)
{
    return ResultFromStatus(_hal->flush(handle));
}

Return<Result> sensors_hw_module::injectSensorData(const Event &data)
{
    return Result::INVALID_OPERATION;
}

Return<void> sensors_hw_module::registerDirectChannel(const SharedMemInfo& mem,
                                   registerDirectChannel_cb _hidl_cb)
{
    if(_is_direct_channel_supported == false) {
        // HAL does not support
        _hidl_cb(Result::INVALID_OPERATION, -1);
        return Void();
    }  else {
        sensors_direct_mem_t m;
        if (!convertFromSharedMemInfo(mem, &m)) {
            _hidl_cb(Result::BAD_VALUE, -1);
            return Void();
        }
        int err = _hal->register_direct_channel( &m);
        if (err < 0) {
            _hidl_cb(ResultFromStatus(err), -1);
        } else {
            int32_t channelHandle = static_cast<int32_t>(err);
            active_direct_channel_list.push_back(channelHandle);
            _hidl_cb(Result::OK, channelHandle);
        }
        return Void();
    }
}

Return<Result> sensors_hw_module::unregisterDirectChannel(int32_t channelHandle)
{
    if(_is_direct_channel_supported == false) {
      // HAL does not support
        return Result::INVALID_OPERATION;
    } else {
        _hal->unregister_direct_channel((int)channelHandle);
        std::vector<int32_t>::iterator it;
        it = std::find (active_direct_channel_list.begin(), active_direct_channel_list.end(), channelHandle);
        if(it != active_direct_channel_list.end())
            active_direct_channel_list.erase(it);
        return Result::OK;
    }
}


Return<void> sensors_hw_module::configDirectReport(
                int32_t sensorHandle, int32_t channelHandle, RateLevel rate,
                configDirectReport_cb _hidl_cb)
{
  if(_is_direct_channel_supported == false) {
    // HAL does not support
    _hidl_cb(Result::INVALID_OPERATION, -1);
    return Void();
  } else {
    sensors_direct_cfg_t cfg = {
        .rate_level = convertFromRateLevel(rate)
    };
    if (cfg.rate_level < 0) {
      _hidl_cb(Result::BAD_VALUE, -1);
      return Void();
    }

    int err = _hal->config_direct_report(sensorHandle, channelHandle, &cfg);

    if (rate == RateLevel::STOP) {
      _hidl_cb(ResultFromStatus(err), -1);
    } else {
      _hidl_cb(err > 0 ? Result::OK : ResultFromStatus(err), err);
    }
    return Void();
  }
}

Return<void> sensors_hw_module::getSensorsList( getSensorsList_cb _hidl_cb)
{
    sensor_t const *list;
    size_t count = _hal->get_sensors_list(&list);
    hidl_vec<SensorInfo> out;
    out.resize(count);

    for (size_t i = 0; i < count; ++i) {
        const sensor_t *src = &list[i];
        SensorInfo *dst = &out[i];
        convertFromSensor(*src, dst);
    }
    _hidl_cb(out);
    return Void();

}

Return<Result> sensors_hw_module::setOperationMode(OperationMode mode)
{
    return Result::BAD_VALUE;
}

Return<void> sensors_hw_module::debug(const hidl_handle& fd, const hidl_vec<hidl_string>& args)
{
    return Return<void>();
}

void sensors_hw_module::post_events(const std::vector<Event>& events, bool wakeup)
{
    ScopedWakelock wakelock = _callback->createScopedWakelock(wakeup);
    _callback->postEvents(events, std::move(wakelock));
}

}
}
}
}
}
}

ISensorsSubHal* sensorsHalGetSubHal(uint32_t* version) {
    static ::android::hardware::sensors::V2_0::subhal::implementation::sensors_hw_module subHal;
    *version = SUB_HAL_2_0_VERSION;
    return &subHal;
}
