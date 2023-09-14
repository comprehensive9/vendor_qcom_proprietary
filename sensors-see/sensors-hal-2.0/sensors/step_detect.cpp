/*
 * Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cinttypes>
#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_pedometer.pb.h"
#include "sns_step_detect.pb.h"

static const char *SSC_DATATYPE_PEDOMETER = "pedometer";
static const char *SSC_DATATYPE_STEP_DETECT = "step_detect";

static const uint32_t PEDOMETER_RESERVED_FIFO_COUNT = 300;

class step_detect : public ssc_sensor
{
public:
    step_detect(sensor_uid suid, sensor_wakeup_type wakeup, bool is_using_pedometer);

private:
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
    /* flag to indicate whether pedometer or step_detect is being used */
    bool _is_using_pedometer;
};

step_detect::step_detect(sensor_uid suid, sensor_wakeup_type wakeup, bool is_using_pedometer):
    ssc_sensor(suid, wakeup),
    _is_using_pedometer(is_using_pedometer)
{
    set_type(SENSOR_TYPE_STEP_DETECTOR);
    set_string_type(SENSOR_STRING_TYPE_STEP_DETECTOR);
    set_fifo_reserved_count(PEDOMETER_RESERVED_FIFO_COUNT);
    set_reporting_mode(SENSOR_FLAG_SPECIAL_REPORTING_MODE);
    set_nowk_msgid(SNS_PEDOMETER_MSGID_SNS_STEP_EVENT_CONFIG);
    set_nowk_msgid(SNS_STEP_DETECT_MSGID_SNS_STEP_DETECT_EVENT);
    if(false == is_resolution_set)
        set_resolution(1.0f);
    if(false == is_max_range_set)
        set_max_range(1.0f);
}

void step_detect::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    bool step_detected = false;
    auto msg_id = pb_event.msg_id();

    if (_is_using_pedometer && SNS_PEDOMETER_MSGID_SNS_STEP_EVENT == msg_id) {
        sns_logd("step_event is coming from pedometer");
        step_detected = true;
    } else if (!_is_using_pedometer && SNS_STEP_DETECT_MSGID_SNS_STEP_DETECT_EVENT == msg_id) {
        sns_logd("step_event is coming from step_detect");
        step_detected = true;
    } else {
        sns_logi("unknown message %u is coming", msg_id);
    }

    if (step_detected) {
        Event hal_event =
            create_sensor_hal_event(pb_event.timestamp());
        hal_event.u.scalar = 1.0f;
        if(true == can_submit_sample(hal_event))
            events.push_back(hal_event);
        sns_logv("step_detect_event: ts=%" PRIu64, hal_event.timestamp);
    }
}

static vector<unique_ptr<sensor>> get_available_step_detect_sensors()
{
    /* first check if step_detect sensor is published if not,
       fall back to pedometer. This is to maitain the support for
       old and new SSC framework */
    const vector<sensor_uid>& step_detect_suids =
         sensor_factory::instance().get_suids(SSC_DATATYPE_STEP_DETECT);
    bool is_using_pedometer = step_detect_suids.empty()? true: false;

    const vector<sensor_uid>& suids = is_using_pedometer?
        sensor_factory::instance().get_suids(SSC_DATATYPE_PEDOMETER): step_detect_suids;

    sns_logi("step_detect sensor will use %s",
        is_using_pedometer? "pedometer" : "step_detect");
    vector<unique_ptr<sensor>> sensors;
    for (const auto& suid : suids) {
        if (!(sensor_factory::instance().get_settings()
                                & DISABLE_WAKEUP_SENSORS_FLAG)) {
            try {
                sensors.push_back(std::make_unique<step_detect>(suid, SENSOR_WAKEUP, is_using_pedometer));
            } catch (const std::exception& e) {
                sns_loge("%s", e.what());
                sns_loge("failed to create sensor step_detect, wakeup=true");
            }
        }
        try {
            sensors.push_back(std::make_unique<step_detect>(suid, SENSOR_NO_WAKEUP, is_using_pedometer));
        } catch (const std::exception& e) {
            sns_loge("%s", e.what());
            sns_loge("failed to create sensor step_detect, wakeup=false");
        }
    }
    return sensors;
}

static bool step_detect_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_STEP_DETECTOR,
        get_available_step_detect_sensors);
    sensor_factory::request_datatype(SSC_DATATYPE_PEDOMETER);
    sensor_factory::request_datatype(SSC_DATATYPE_STEP_DETECT);
    return true;
}

SENSOR_MODULE_INIT(step_detect_module_init);
