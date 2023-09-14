/*
 * Copyright (c) 2017-2018, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_std_sensor.pb.h"

static const char *SSC_DATATYPE_ROTV = "rotv";

class rotv : public ssc_sensor
{
public:
    rotv(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_ROTV; }
private:
    virtual void handle_sns_std_sensor_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
};

rotv::rotv(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_ROTATION_VECTOR);
    set_string_type(SENSOR_STRING_TYPE_ROTATION_VECTOR);
    if(false == is_resolution_set)
        set_resolution(0.01f);
    if(false == is_max_range_set)
        set_max_range(1.0f);
}

void rotv::handle_sns_std_sensor_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    sns_std_sensor_event pb_sensor_event;
    pb_sensor_event.ParseFromString(pb_event.payload());

    if (pb_sensor_event.data_size() != 4) {
        sns_loge("expected data_size is 4 but, actual data_size = %zu",
            (size_t)pb_sensor_event.data_size());
        return;
    }
    Event hal_event = create_sensor_hal_event(pb_event.timestamp());
    hal_event.u.data[0] = pb_sensor_event.data(0);
    hal_event.u.data[1] = pb_sensor_event.data(1);
    hal_event.u.data[2] = pb_sensor_event.data(2);
    hal_event.u.data[3] = pb_sensor_event.data(3);
    /* heading accuracy is not available so, set data[4] to -1
       refer to https://developer.android.com/reference/android/hardware/SensorEvent#sensor.type_rotation_vector */
    hal_event.u.data[4] = -1;

    if(true == can_submit_sample(hal_event))
      events.push_back(hal_event);

    sns_logv("rotv_event: ts=%llu, x = %f, y=%f, z = %f, w = %f, acc = %f",
             (unsigned long long)hal_event.timestamp,
             hal_event.u.data[0], hal_event.u.data[1],
             hal_event.u.data[2], hal_event.u.data[3],
             hal_event.u.data[4]);
}

static bool rotv_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(
        SENSOR_TYPE_ROTATION_VECTOR,
        ssc_sensor::get_available_sensors<rotv>);
    sensor_factory::request_datatype(SSC_DATATYPE_ROTV);
    return true;
}

SENSOR_MODULE_INIT(rotv_module_init);
