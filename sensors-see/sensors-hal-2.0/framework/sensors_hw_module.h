/*
 * Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <hardware/sensors.h>
#include <android/hardware/sensors/2.0/ISensors.h>
#include <android/hardware/sensors/1.0/types.h>
#include <hidl/Status.h>
#include <log/log.h>
#include <thread>
#include "sensors_hal.h"
#include "SubHal.h"


using ::android::sp;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

using ::android::hardware::sensors::V1_0::SensorFlagBits;
using ::android::hardware::sensors::V1_0::SensorStatus;
using ::android::hardware::sensors::V2_0::ISensors;
using ::android::hardware::sensors::V2_0::ISensorsCallback;
using ::android::hardware::sensors::V2_0::implementation::IHalProxyCallback;



namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

class sensors_hw_module : public ISensorsSubHal, public sensors_hal_event_callback {

using Event = ::android::hardware::sensors::V1_0::Event;
using OperationMode = ::android::hardware::sensors::V1_0::OperationMode;
using RateLevel = ::android::hardware::sensors::V1_0::RateLevel;
using Result = ::android::hardware::sensors::V1_0::Result;
using SharedMemInfo = ::android::hardware::sensors::V1_0::SharedMemInfo;
using SharedMemFormat = android::hardware::sensors::V1_0::SharedMemFormat;


public:

sensors_hw_module();
~sensors_hw_module();
/**
 * Enumerate all available (static) sensors.
 */
Return<void> getSensorsList(getSensorsList_cb _hidl_cb) override;

/**
 * Place the module in a specific mode. The following modes are defined
 *
 *  SENSOR_HAL_NORMAL_MODE - Normal operation. Default state of the module.
 *
 *  SENSOR_HAL_DATA_INJECTION_MODE - Loopback mode.
 *    Data is injected for the supported sensors by the sensor service in
 *    this mode.
 *
 * @return OK on success
 *  BAD_VALUE if requested mode is not supported
 *  PERMISSION_DENIED if operation is not allowed
 */
Return<Result> setOperationMode(OperationMode mode) override;

/**
 * Activate/de-activate one sensor.
 *
 * After sensor de-activation, existing sensor events that have not
 * been picked up by poll() must be abandoned immediately so that
 * subsequent activation will not get stale sensor events (events
 * that are generated prior to the latter activation).
 *
 * @param  sensorHandle is the handle of the sensor to change.
 * @param  enabled set to true to enable, or false to disable the sensor.
 *
 * @return result OK on success, BAD_VALUE if sensorHandle is invalid.
 */
Return<Result> activate(int32_t sensorHandle, bool enabled) override;

/**
 * @return A human-readable name for use in wake locks and logging.
 */
const std::string getName() override { return "QTI-SubHAL"; }

/**
 * This is the first method invoked on the sub-HAL after it's allocated through
 * sensorsHalGetSubHal() by the HalProxy. Sub-HALs should use this to initialize any state and
 * retain the callback given in order to communicate with the HalProxy. Method will be called
 * anytime the sensors framework restarts. Therefore, this method will be responsible for
 * reseting the state of the subhal and cleaning up and reallocating any previously allocated
 * data. Initialize should ensure that the subhal has reset its operation mode to NORMAL state
 * as well.
 *
 * @param halProxyCallback callback used to inform the HalProxy when a dynamic sensor's state
 *     changes, new sensor events should be sent to the framework, and when a new ScopedWakelock
 *     should be created.
 * @return result OK on success
 */
Return<Result> initialize(const sp<IHalProxyCallback>& halProxyCallback) override;
/**
 * Sets a sensors parameters, including sampling frequency and maximum
 * report latency. This function can be called while the sensor is
 * activated, in which case it must not cause any sensor measurements to
 * be lost: transitioning from one sampling rate to the other cannot cause
 * lost events, nor can transitioning from a high maximum report latency to
 * a low maximum report latency.
 *
 * @param sensorHandle handle of sensor to be changed.
 * @param samplingPeriodNs specifies sensor sample period in nanoseconds.
 * @param maxReportLatencyNs allowed delay time before an event is sampled
 *     to time of report.
 * @return result OK on success, BAD_VALUE if any parameters are invalid.
 */

Return<Result> batch(int32_t sensorHandle, int64_t samplingPeriodNs,
                     int64_t maxReportLatencyNs) override;

/**
 * Trigger a flush of internal FIFO.
 *
 * Flush adds a FLUSH_COMPLETE metadata event to the end of the "batch mode"
 * FIFO for the specified sensor and flushes the FIFO.	If the FIFO is empty
 * or if the sensor doesn't support batching (FIFO size zero), return
 * SUCCESS and add a trivial FLUSH_COMPLETE event added to the event stream.
 * This applies to all sensors other than one-shot sensors. If the sensor
 * is a one-shot sensor, flush must return BAD_VALUE and not generate any
 * flush complete metadata.  If the sensor is not active at the time flush()
 * is called, flush() return BAD_VALUE.
 *
 * @param sensorHandle handle of sensor to be flushed.
 * @return result OK on success and BAD_VALUE if sensorHandle is invalid.
 */
Return<Result> flush(int32_t sensorHandle) override;

/**
 * Inject a single sensor event or push operation environment parameters to
 * device.
 *
 * When device is in NORMAL mode, this function is called to push operation
 * environment data to device. In this operation, Event is always of
 * SensorType::AdditionalInfo type. See operation evironment parameters
 * section in AdditionalInfoType.
 *
 * When device is in DATA_INJECTION mode, this function is also used for
 * injecting sensor events.
 *
 * Regardless of OperationMode, injected SensorType::ADDITIONAL_INFO
 * type events should not be routed back to the sensor event queue.
 *
 * @see AdditionalInfoType
 * @see OperationMode
 * @param event sensor event to be injected
 * @return result OK on success; PERMISSION_DENIED if operation is not
 *     allowed; INVALID_OPERATION, if this functionality is unsupported;
 *     BAD_VALUE if sensor event cannot be injected.
 */
Return<Result> injectSensorData(const Event& event) override;

/**
 * Register direct report channel.
 *
 * Register a direct channel with supplied shared memory information. Upon
 * return, the sensor hardware is responsible for resetting the memory
 * content to initial value (depending on memory format settings).
 *
 * @param mem shared memory info data structure.
 * @return result OK on success; BAD_VALUE if shared memory information is
 *     not consistent; NO_MEMORY if shared memory cannot be used by sensor
 *     system; INVALID_OPERATION if functionality is not supported.
 * @return channelHandle a positive integer used for referencing registered
 *     direct channel (>0) in configureDirectReport and
 *     unregisterDirectChannel if result is OK, -1 otherwise.
 */
Return<void> registerDirectChannel(const SharedMemInfo& mem,
                                   registerDirectChannel_cb _hidl_cb) override;

/**
 * Unregister direct report channel.
 *
 * Unregister a direct channel previously registered using
 * registerDirectChannel, and remove all active sensor report configured in
 * still active sensor report configured in the direct channel.
 *
 * @param channelHandle handle of direct channel to be unregistered.
 * @return result OK if direct report is supported; INVALID_OPERATION
 *     otherwise.
 */
Return<Result> unregisterDirectChannel(int32_t channelHandle) override;

/**
 * Configure direct sensor event report in direct channel.
 *
 * This function start, modify rate or stop direct report of a sensor in a
 * certain direct channel.
 *
 * @param sensorHandle handle of sensor to be configured. When combined
 *     with STOP rate, sensorHandle can be -1 to denote all active sensors
 *     in the direct channel specified by channel Handle.
 * @param channelHandle handle of direct channel to be configured.
 * @param rate rate level, see RateLevel enum.
 * @return result OK on success; BAD_VALUE if parameter is invalid (such as
 *     rate level is not supported by sensor, channelHandle does not exist,
 *     etc); INVALID_OPERATION if functionality is not supported.
 * @return reportToken positive integer to identify multiple sensors of
 *     the same type in a single direct channel. Ignored if rate is STOP.
 *     See SharedMemFormat.
 */
Return<void> configDirectReport(int32_t sensorHandle, int32_t channelHandle,
                        RateLevel rate,
                        configDirectReport_cb _hidl_cb) override;
/**
 * Method defined in ::android::hidl::base::V1_0::IBase.
 *
 * This method should write debug information to hidl_handle that is useful for debugging
 * issues. Suggestions include:
 * - Sensor info including handle values and any other state available in the SensorInfo class
 * - List of active sensors and their current sampling period and reporting latency
 * - Information about pending flush requests
 * - Current operating mode
 * - Currently registered direct channel info
 * - A history of any of the above
 */
Return<void> debug(const hidl_handle& fd, const hidl_vec<hidl_string>& args) override;

/**
 * Thread-safe callback used to post events to the HalProxy. Sub-HALs should invoke this
 * whenever new sensor events need to be delivered to the sensors framework. Once invoked, the
 * HalProxy will attempt to send events to the sensors framework using a blocking write with a
 * 5 second timeout. This write may be done asynchronously if the queue used to communicate
 * with the framework is full to avoid blocking sub-HALs for the length of the timeout. If the
 * write fails, the events will be dropped and any wake locks held will be released.
 *
 * The provided ScopedWakelock must be locked if the events are from wakeup sensors. If it's
 * not locked accordingly, the HalProxy will crash as this indicates the sub-HAL isn't compliant
 * with the sensors HAL 2.0 specification. Additionally, since ScopedWakelock isn't copyable,
 * the HalProxy will take ownership of the wake lock given when this method is invoked. Once the
 * method returns, the HalProxy will handle holding the wake lock, if necessary, until the
 * framework has successfully processed any wakeup events.
 *
 * No return type is used for this callback to avoid sub-HALs trying to resend events when
 * writes fail. Writes should only fail when the framework is under inordinate stress which will
 * likely result in a framework restart so retrying will likely only result in overloading the
 * HalProxy. Sub-HALs should always assume that the write was a success and perform any
 * necessary cleanup. Additionally, the HalProxy will ensure it logs any errors (through ADB and
 * bug reports) it encounters during delivery to ensure it's obvious that a failure occurred.
 *
 * @param events the events that should be sent to the sensors framework
 * @param wakelock ScopedWakelock that should be locked to send events from wake sensors and
 *     unlocked otherwise.
 */
void post_events(const std::vector<Event>& events, bool wakeup) override;

private:
    /**
     * sensors hal module handle
     */
    sensors_hal* _hal;

    std::atomic<bool> _is_hal_configured;

    /* Clear all existing clients when system_server dies */
    void clearActiveConnections();

    std::vector<int32_t> active_direct_channel_list;

    /* Initialize must be thread safe and prevent concurrent calls
     * to initialize from simultaneously modifying state. */
    std::mutex _mutex;
    sp<IHalProxyCallback> _callback;

    /*True if direct_channel is supported, False otherwise */
    bool _is_direct_channel_supported;
};
}
}
}
}
}
}
