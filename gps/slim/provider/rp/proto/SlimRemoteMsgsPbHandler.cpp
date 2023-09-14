/*
 * SlimRemoteMsgsPbHandler.cpp
 * Common location data types definitions, enums, structs
 *
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <loc_pla.h>
#include <log_util.h>
#include <slim_client_types.h>
#include <slim_internal_client_types.h>
#include <slim_os_api.h>
#include <loc_cfg.h>
#include <slim_os_log_api.h>
#include <slim_utils.h>

#include "SlimRemoteMsgsPbHandler.h"

pbSlimServiceEnumT SlimRemoteMsgsPbHandler::getPBEnumForSlimService(slimServiceEnumT service)
{
    pbSlimServiceEnumT pbService;
    switch (service)
    {
        case eSLIM_SERVICE_SENSOR_ACCEL:
            pbService = PB_eSLIM_SERVICE_SENSOR_ACCEL;
            break;
        case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
            pbService = PB_eSLIM_SERVICE_SENSOR_ACCEL_TEMP;
            break;
        case eSLIM_SERVICE_SENSOR_GYRO:
            pbService = PB_eSLIM_SERVICE_SENSOR_GYRO;
            break;
        case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
            pbService = PB_eSLIM_SERVICE_SENSOR_GYRO_TEMP;
            break;
        case eSLIM_SERVICE_AMD:
            pbService = PB_eSLIM_SERVICE_AMD;
            break;
        case eSLIM_SERVICE_RMD:
            pbService = PB_eSLIM_SERVICE_RMD;
            break;
        case eSLIM_SERVICE_SMD:
            pbService = PB_eSLIM_SERVICE_SMD;
            break;
        case eSLIM_SERVICE_MOTION_DATA:
            pbService = PB_eSLIM_SERVICE_MOTION_DATA;
            break;
        case eSLIM_SERVICE_PEDOMETER:
            pbService = PB_eSLIM_SERVICE_PEDOMETER;
            break;
        case eSLIM_SERVICE_DISTANCE_BOUND:
            pbService = PB_eSLIM_SERVICE_DISTANCE_BOUND;
            break;
        case eSLIM_SERVICE_VEHICLE_ACCEL:
            pbService = PB_eSLIM_SERVICE_VEHICLE_ACCEL;
            break;
        case eSLIM_SERVICE_VEHICLE_GYRO:
            pbService = PB_eSLIM_SERVICE_VEHICLE_GYRO;
            break;
        case eSLIM_SERVICE_VEHICLE_ODOMETRY:
            pbService = PB_eSLIM_SERVICE_VEHICLE_ODOMETRY;
            break;
        case eSLIM_SERVICE_SENSOR_MAG_CALIB:
            pbService = PB_eSLIM_SERVICE_SENSOR_MAG_CALIB;
            break;
        case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
            pbService = PB_eSLIM_SERVICE_SENSOR_MAG_UNCALIB;
            break;
        case eSLIM_SERVICE_PED_ALIGNMENT:
            pbService = PB_eSLIM_SERVICE_PED_ALIGNMENT;
            break;
        case eSLIM_SERVICE_SENSOR_BARO:
            pbService = PB_eSLIM_SERVICE_SENSOR_BARO;
            break;
        case eSLIM_SERVICE_MAG_FIELD_DATA:
            pbService = PB_eSLIM_SERVICE_MAG_FIELD_DATA;
            break;
        case eSLIM_SERVICE_VEHICLE_SPEED:
            pbService = PB_eSLIM_SERVICE_VEHICLE_SPEED;
            break;
        case eSLIM_SERVICE_TILT:
            pbService = PB_eSLIM_SERVICE_TILT;
            break;
        case eSLIM_SERVICE_VEHICLE_DWS:
            pbService = PB_eSLIM_SERVICE_VEHICLE_DWS;
            break;
        case eSLIM_SERVICE_VEHICLE_GEAR:
            pbService = PB_eSLIM_SERVICE_VEHICLE_GEAR;
            break;
        case eSLIM_SERVICE_VEHICLE_STEERING_WHEEL:
            pbService = PB_eSLIM_SERVICE_VEHICLE_STEERING_WHEEL;
            break;
        default:
            SLIM_LOGD("Not supported service");
            pbService = PB_eSLIM_SERVICE_NONE;
            break;
    }
    return pbService;
}

slimServiceEnumT SlimRemoteMsgsPbHandler::getSlimService(pbSlimServiceEnumT pbService)
{
    slimServiceEnumT service;
    switch (pbService)
    {
        case PB_eSLIM_SERVICE_SENSOR_ACCEL:
            service = eSLIM_SERVICE_SENSOR_ACCEL;
            break;
        case PB_eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
            service = eSLIM_SERVICE_SENSOR_ACCEL_TEMP;
            break;
        case PB_eSLIM_SERVICE_SENSOR_GYRO:
            service = eSLIM_SERVICE_SENSOR_GYRO;
            break;
        case PB_eSLIM_SERVICE_SENSOR_GYRO_TEMP:
            service = eSLIM_SERVICE_SENSOR_GYRO_TEMP;
            break;
        case PB_eSLIM_SERVICE_AMD:
            service = eSLIM_SERVICE_AMD;
            break;
        case PB_eSLIM_SERVICE_RMD:
            service = eSLIM_SERVICE_RMD;
            break;
        case PB_eSLIM_SERVICE_SMD:
            service = eSLIM_SERVICE_SMD;
            break;
        case PB_eSLIM_SERVICE_MOTION_DATA:
            service = eSLIM_SERVICE_MOTION_DATA;
            break;
        case PB_eSLIM_SERVICE_PEDOMETER:
            service = eSLIM_SERVICE_PEDOMETER;
            break;
        case PB_eSLIM_SERVICE_DISTANCE_BOUND:
            service = eSLIM_SERVICE_DISTANCE_BOUND;
            break;
        case PB_eSLIM_SERVICE_VEHICLE_ACCEL:
            service = eSLIM_SERVICE_VEHICLE_ACCEL;
            break;
        case PB_eSLIM_SERVICE_VEHICLE_GYRO:
            service = eSLIM_SERVICE_VEHICLE_GYRO;
            break;
        case PB_eSLIM_SERVICE_VEHICLE_ODOMETRY:
            service = eSLIM_SERVICE_VEHICLE_ODOMETRY;
            break;
        case PB_eSLIM_SERVICE_SENSOR_MAG_CALIB:
            service = eSLIM_SERVICE_SENSOR_MAG_CALIB;
            break;
        case PB_eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
            service = eSLIM_SERVICE_SENSOR_MAG_UNCALIB;
            break;
        case PB_eSLIM_SERVICE_PED_ALIGNMENT:
            service = eSLIM_SERVICE_PED_ALIGNMENT;
            break;
        case PB_eSLIM_SERVICE_SENSOR_BARO:
            service = eSLIM_SERVICE_SENSOR_BARO;
            break;
        case PB_eSLIM_SERVICE_MAG_FIELD_DATA:
            service = eSLIM_SERVICE_MAG_FIELD_DATA;
            break;
        case PB_eSLIM_SERVICE_VEHICLE_SPEED:
            service = eSLIM_SERVICE_VEHICLE_SPEED;
            break;
        case PB_eSLIM_SERVICE_TILT:
            service = eSLIM_SERVICE_TILT;
            break;
        case PB_eSLIM_SERVICE_VEHICLE_DWS:
            service = eSLIM_SERVICE_VEHICLE_DWS;
            break;
        case PB_eSLIM_SERVICE_VEHICLE_GEAR:
            service = eSLIM_SERVICE_VEHICLE_GEAR;
            break;
        case PB_eSLIM_SERVICE_VEHICLE_STEERING_WHEEL:
            service = eSLIM_SERVICE_VEHICLE_STEERING_WHEEL;
            break;
        default:
            SLIM_LOGE("Not supported service");
            service = eSLIM_SERVICE_NONE;
            break;
    }
    return service;
}

slimServiceStatusEnumT SlimRemoteMsgsPbHandler::getSlimServiceStatus(
            pbSlimServiceStatusEnumT pbServiceStatus
)
{
    slimServiceStatusEnumT serviceStatus;

    switch (pbServiceStatus) {
        case PB_eSLIM_SERVICE_STATUS_NONE:
            serviceStatus = eSLIM_SERVICE_STATUS_NONE;
            break;
        case PB_eSLIM_SERVICE_STATUS_AVAILABLE:
            serviceStatus = eSLIM_SERVICE_STATUS_AVAILABLE;
            break;
        case PB_eSLIM_SERVICE_STATUS_UNAVAILABLE:
            serviceStatus = eSLIM_SERVICE_STATUS_UNAVAILABLE;
            break;
        case PB_eSLIM_SERVICE_STATUS_PROVIDERS_CHANGED:
            serviceStatus = eSLIM_SERVICE_STATUS_PROVIDERS_CHANGED;
            break;
        default:
            SLIM_LOGE("Not supported serviceStatus");
            serviceStatus = eSLIM_SERVICE_STATUS_NONE;
            break;
    }
    return serviceStatus;
}

slimServiceProviderEnumT SlimRemoteMsgsPbHandler::getSlimServiceProvider(
            pbSlimServiceProviderEnumT pbProvider
)
{
    slimServiceProviderEnumT provider;
    switch (pbProvider) {
        case PB_eSLIM_SERVICE_PROVIDER_DEFAULT:
            provider = eSLIM_SERVICE_PROVIDER_DEFAULT;
            break;
        case PB_eSLIM_SERVICE_PROVIDER_SSC:
            provider = eSLIM_SERVICE_PROVIDER_SSC;
            break;
        case PB_eSLIM_SERVICE_PROVIDER_SAMLITE:
            provider = eSLIM_SERVICE_PROVIDER_SAMLITE;
            break;
        case PB_eSLIM_SERVICE_PROVIDER_NATIVE:
            provider = eSLIM_SERVICE_PROVIDER_NATIVE;
            break;
        default:
            SLIM_LOGE("Not supported provider");
            provider = eSLIM_SERVICE_PROVIDER_DEFAULT;
            break;
    }
    return provider;
}

slimTimeSourceEnumT SlimRemoteMsgsPbHandler::getSlimTimeSource(pbSlimTimeSourceEnumT pbTimeSource)
{
    slimTimeSourceEnumT timeSource;
    switch (pbTimeSource) {
        case PB_eSLIM_TIME_SOURCE_UNSPECIFIED:
            timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
            break;
        case PB_eSLIM_TIME_SOURCE_COMMON:
            timeSource = eSLIM_TIME_SOURCE_COMMON;
            break;
        case PB_eSLIM_TIME_SOURCE_GPS:
            timeSource = eSLIM_TIME_SOURCE_GPS;
            break;
        case PB_eSLIM_TIME_SOURCE_UTC:
            timeSource = eSLIM_TIME_SOURCE_UTC;
            break;
        case PB_eSLIM_TIME_SOURCE_LEGACY:
            timeSource = eSLIM_TIME_SOURCE_LEGACY;
            break;
        default:
            SLIM_LOGE("Not supported timeSource");
            timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
            break;
    }
    return timeSource;
}

slimSensorTypeEnumT SlimRemoteMsgsPbHandler::getSlimSensorType(pbSlimSensorTypeEnumT pbSensorType)
{
    slimSensorTypeEnumT sensorType;
    switch (pbSensorType) {
        case PB_eSLIM_SENSOR_TYPE_ACCEL:
            sensorType = eSLIM_SENSOR_TYPE_ACCEL;
            break;
        case PB_eSLIM_SENSOR_TYPE_GYRO:
            sensorType = eSLIM_SENSOR_TYPE_GYRO;
            break;
        case PB_eSLIM_SENSOR_TYPE_ACCEL_TEMP:
            sensorType = eSLIM_SENSOR_TYPE_ACCEL_TEMP;
            break;
        case PB_eSLIM_SENSOR_TYPE_GYRO_TEMP:
            sensorType = eSLIM_SENSOR_TYPE_GYRO_TEMP;
            break;
        case PB_eSLIM_SENSOR_TYPE_MAGNETOMETER:
            sensorType = eSLIM_SENSOR_TYPE_MAGNETOMETER;
            break;
        case PB_eSLIM_SENSOR_TYPE_BAROMETER:
            sensorType = eSLIM_SENSOR_TYPE_BAROMETER;
            break;
        case PB_eSLIM_SENSOR_TYPE_NONE:
            sensorType = eSLIM_SENSOR_TYPE_NONE;
            break;
        default:
            SLIM_LOGE("Not supported sensorType");
            sensorType = eSLIM_SENSOR_TYPE_NONE;
            break;
    }
    return sensorType;
}


slimVehicleMotionSensorTypeEnumT SlimRemoteMsgsPbHandler::getSlimVnwSensorType(
                        pbSlimVehicleMotionSensorTypeEnumT pbVnwType
)
{
    slimVehicleMotionSensorTypeEnumT vnwSensorType;
    switch (pbVnwType) {
        case PB_eSLIM_VEHICLE_MOTION_SENSOR_TYPE_SPEED:
            vnwSensorType = eSLIM_VEHICLE_MOTION_SENSOR_TYPE_SPEED;
            break;
        case PB_eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS:
            vnwSensorType = eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS;
            break;
        case PB_eSLIM_VEHICLE_MOTION_SENSOR_TYPE_GEAR:
            vnwSensorType = eSLIM_VEHICLE_MOTION_SENSOR_TYPE_GEAR;
            break;
        case PB_eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL:
            vnwSensorType = eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL;
            break;
        default:
            SLIM_LOGE("Not supported vnwSensorType");
            vnwSensorType = eSLIM_VEHICLE_MOTION_SENSOR_TYPE_MAX;
            break;
    }
    return vnwSensorType;
}

pbSlimServiceStatusEnumT SlimRemoteMsgsPbHandler::getPBEnumForSlimServiceStatus(
                    slimServiceStatusEnumT serviceStatus)
{
    pbSlimServiceStatusEnumT pbServiceStatus;

    switch (serviceStatus) {
        case eSLIM_SERVICE_STATUS_NONE:
            pbServiceStatus = PB_eSLIM_SERVICE_STATUS_NONE;
            break;
        case eSLIM_SERVICE_STATUS_AVAILABLE:
            pbServiceStatus = PB_eSLIM_SERVICE_STATUS_AVAILABLE;
            break;
        case eSLIM_SERVICE_STATUS_UNAVAILABLE:
            pbServiceStatus = PB_eSLIM_SERVICE_STATUS_UNAVAILABLE;
            break;
        case eSLIM_SERVICE_STATUS_PROVIDERS_CHANGED:
            pbServiceStatus = PB_eSLIM_SERVICE_STATUS_PROVIDERS_CHANGED;
            break;
        default:
            SLIM_LOGE("Not supported ServiceStatus");
            pbServiceStatus = PB_eSLIM_SERVICE_STATUS_NONE;
            break;
    }
    return pbServiceStatus;
}

pbSlimMessageTypeEnumT SlimRemoteMsgsPbHandler::getPBEnumForSlimMessageType(
                        slimMessageTypeEnumT msgType)
{
    pbSlimMessageTypeEnumT pbMsgType;
    switch (msgType) {
        case eSLIM_MESSAGE_TYPE_INDICATION:
            pbMsgType = PB_eSLIM_MESSAGE_TYPE_INDICATION;
            break;
        case eSLIM_MESSAGE_TYPE_RESPONSE:
            pbMsgType = PB_eSLIM_MESSAGE_TYPE_RESPONSE;
            break;
        default:
            SLIM_LOGE("Not supported MsgType");
            pbMsgType = PB_eSLIM_MESSAGE_TYPE_MAX;
            break;
    }
    return pbMsgType;
}

pbSlimErrorEnumT SlimRemoteMsgsPbHandler::getPBEnumForSlimError(slimErrorEnumT msgError)
{
    pbSlimErrorEnumT pbMsgError;
    switch (msgError)
    {
        case eSLIM_SUCCESS:
            pbMsgError = PB_eSLIM_SUCCESS;
            break;
        case eSLIM_ERROR_CLIENT_HANDLE_INVALID:
            pbMsgError = PB_eSLIM_ERROR_CLIENT_HANDLE_INVALID;
            break;
        case eSLIM_ERROR_UNSUPPORTED:
            pbMsgError = PB_eSLIM_ERROR_UNSUPPORTED;
            break;
        case eSLIM_ERROR_IPC_COMMUNICATION:
            pbMsgError = PB_eSLIM_ERROR_IPC_COMMUNICATION;
            break;
        case eSLIM_ERROR_INVALID_CALLBACK:
            pbMsgError = PB_eSLIM_ERROR_INVALID_CALLBACK;
            break;
        case eSLIM_ERROR_INTERNAL:
            pbMsgError = PB_eSLIM_ERROR_INTERNAL;
            break;
        case eSLIM_ERROR_MAX_TXN_COUNT_REACHED:
            pbMsgError = PB_eSLIM_ERROR_MAX_TXN_COUNT_REACHED;
            break;
        case eSLIM_ERROR_BAD_ARGUMENT:
            pbMsgError = PB_eSLIM_ERROR_BAD_ARGUMENT;
            break;
        case eSLIM_ERROR_QMI_INTERFACE:
            pbMsgError = PB_eSLIM_ERROR_QMI_INTERFACE;
            break;
        case eSLIM_ERROR_ALLOCATION:
            pbMsgError = PB_eSLIM_ERROR_ALLOCATION;
            break;
        case eSLIM_ERROR_SERVICE_RESERVED:
            pbMsgError = PB_eSLIM_ERROR_SERVICE_RESERVED;
            break;
        case eSLIM_ERROR_SERVICE_NOT_ENABLED:
            pbMsgError = PB_eSLIM_ERROR_SERVICE_NOT_ENABLED;
            break;
        case eSLIM_ERROR_MAX_CLIENT_COUNT_REACHED:
            pbMsgError = PB_eSLIM_ERROR_MAX_CLIENT_COUNT_REACHED;
            break;
        case eSLIM_ERROR_TIMEOUT:
            pbMsgError = PB_eSLIM_ERROR_TIMEOUT;
            break;
        case eSLIM_ERROR_NANOPB:
            pbMsgError = PB_eSLIM_ERROR_NANOPB;
            break;
        default:
            SLIM_LOGE("Not supported MsgError");
            pbMsgError = PB_eSLIM_ERROR_MIN;
            break;
    }
    return pbMsgError;
}

pbSlimMessageIdEnumT SlimRemoteMsgsPbHandler::getPBEnumForSlimMessageId(slimMessageIdEnumT msgId)
{
    pbSlimMessageIdEnumT pbMsgId;
    switch (msgId)
    {
        case eSLIM_MESSAGE_ID_NONE:
            pbMsgId = PB_eSLIM_MESSAGE_ID_NONE;
            break;
        case eSLIM_MESSAGE_ID_OPEN_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_OPEN_RESP;
            break;
        case eSLIM_MESSAGE_ID_SERVICE_STATUS_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_SERVICE_STATUS_IND;
            break;
        case eSLIM_MESSAGE_ID_SENSOR_DATA_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_SENSOR_DATA_IND;
            break;
        case eSLIM_MESSAGE_ID_MOTION_DATA_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_MOTION_DATA_IND;
            break;
        case eSLIM_MESSAGE_ID_MOTION_DATA_ENABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_MOTION_DATA_ENABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_PEDOMETER_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_PEDOMETER_IND;
            break;
        case eSLIM_MESSAGE_ID_PEDOMETER_ENABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_PEDOMETER_ENABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_QMD_DATA_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_QMD_DATA_IND;
            break;
        case eSLIM_MESSAGE_ID_QMD_DATA_ENABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_QMD_DATA_ENABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_SMD_DATA_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_SMD_DATA_IND;
            break;
        case eSLIM_MESSAGE_ID_SMD_DATA_ENABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_SMD_DATA_ENABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_IND;
            break;
        case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_ENABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_ENABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_SET_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_SET_RESP;
            break;
        case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_GET_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_GET_RESP;
            break;
        case eSLIM_MESSAGE_ID_VEHICLE_SENSOR_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_VEHICLE_SENSOR_IND;
            break;
        case eSLIM_MESSAGE_ID_VEHICLE_ODOMETRY_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_VEHICLE_ODOMETRY_IND;
            break;
        case eSLIM_MESSAGE_ID_PED_ALIGNMENT_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_PED_ALIGNMENT_IND;
            break;
        case eSLIM_MESSAGE_ID_PED_ALIGNMENT_ENABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_PED_ALIGNMENT_ENABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_MAG_FIELD_DATA_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_MAG_FIELD_DATA_IND;
            break;
        case eSLIM_MESSAGE_ID_MAG_FIELD_DATA_ENABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_MAG_FIELD_DATA_ENABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_MOTION_DATA_DISABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_MOTION_DATA_DISABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_PEDOMETER_DISABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_PEDOMETER_DISABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_QMD_DATA_DISABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_QMD_DATA_DISABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_SMD_DATA_DISABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_SMD_DATA_DISABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_DISABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_DISABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_PED_ALIGNMENT_DISABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_PED_ALIGNMENT_DISABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_MAG_FIELD_DATA_DISABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_MAG_FIELD_DATA_DISABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_SENSOR_DATA_ENABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_SENSOR_DATA_ENABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_VEHICLE_ENABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_VEHICLE_ENABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND:
            pbMsgId = PB_eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND;
            break;
        case eSLIM_MESSAGE_ID_SENSOR_DATA_DISABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_SENSOR_DATA_DISABLE_RESP;
            break;
        case eSLIM_MESSAGE_ID_VEHICLE_DISABLE_RESP:
            pbMsgId = PB_eSLIM_MESSAGE_ID_VEHICLE_DISABLE_RESP;
            break;
        default:
            SLIM_LOGE("Not supported pbMsgId");
            pbMsgId = PB_eSLIM_MESSAGE_ID_NONE;
            break;
    }
    return pbMsgId;
}

pbSlimServiceProviderEnumT SlimRemoteMsgsPbHandler::getPBEnumSlimServiceProvider(
            slimServiceProviderEnumT provider
)
{
    pbSlimServiceProviderEnumT pbProvider;
    switch (provider) {
        case eSLIM_SERVICE_PROVIDER_DEFAULT:
            pbProvider = PB_eSLIM_SERVICE_PROVIDER_DEFAULT;
            break;
        case eSLIM_SERVICE_PROVIDER_SSC:
            pbProvider = PB_eSLIM_SERVICE_PROVIDER_SSC;
            break;
        case eSLIM_SERVICE_PROVIDER_SAMLITE:
            pbProvider = PB_eSLIM_SERVICE_PROVIDER_SAMLITE;
            break;
        case eSLIM_SERVICE_PROVIDER_NATIVE:
            pbProvider = PB_eSLIM_SERVICE_PROVIDER_NATIVE;
            break;
        default:
            SLIM_LOGE("Not supported pbProvider");
            pbProvider = PB_eSLIM_SERVICE_PROVIDER_DEFAULT;
            break;
    }
    return pbProvider;
}

pbSlimTimeSourceEnumT SlimRemoteMsgsPbHandler::getPBEnumSlimTimeSource(
                slimTimeSourceEnumT timeSource
)
{
    pbSlimTimeSourceEnumT pbTimeSource;
    switch (timeSource) {
        case eSLIM_TIME_SOURCE_UNSPECIFIED:
            pbTimeSource = PB_eSLIM_TIME_SOURCE_UNSPECIFIED;
            break;
        case eSLIM_TIME_SOURCE_COMMON:
            pbTimeSource = PB_eSLIM_TIME_SOURCE_COMMON;
            break;
        case eSLIM_TIME_SOURCE_GPS:
            pbTimeSource = PB_eSLIM_TIME_SOURCE_GPS;
            break;
        case eSLIM_TIME_SOURCE_UTC:
            pbTimeSource = PB_eSLIM_TIME_SOURCE_UTC;
            break;
        case eSLIM_TIME_SOURCE_LEGACY:
            pbTimeSource = PB_eSLIM_TIME_SOURCE_LEGACY;
            break;
        default:
            SLIM_LOGE("Not supported pbTimeSource");
            pbTimeSource = PB_eSLIM_TIME_SOURCE_UNSPECIFIED;
            break;
    }
    return pbTimeSource;
}

pbSlimSensorTypeEnumT SlimRemoteMsgsPbHandler::getPBEnumSlimSensorType(
        slimSensorTypeEnumT sensorType
)
{
    pbSlimSensorTypeEnumT pbSensorType;
    switch (sensorType) {
        case eSLIM_SENSOR_TYPE_ACCEL:
            pbSensorType = PB_eSLIM_SENSOR_TYPE_ACCEL;
            break;
        case eSLIM_SENSOR_TYPE_GYRO:
            pbSensorType = PB_eSLIM_SENSOR_TYPE_GYRO;
            break;
        case eSLIM_SENSOR_TYPE_ACCEL_TEMP:
            pbSensorType = PB_eSLIM_SENSOR_TYPE_ACCEL_TEMP;
            break;
        case eSLIM_SENSOR_TYPE_GYRO_TEMP:
            pbSensorType = PB_eSLIM_SENSOR_TYPE_GYRO_TEMP;
            break;
        case eSLIM_SENSOR_TYPE_MAGNETOMETER:
            pbSensorType = PB_eSLIM_SENSOR_TYPE_MAGNETOMETER;
            break;
        case eSLIM_SENSOR_TYPE_BAROMETER:
            pbSensorType = PB_eSLIM_SENSOR_TYPE_BAROMETER;
            break;
        case eSLIM_SENSOR_TYPE_NONE:
            pbSensorType = PB_eSLIM_SENSOR_TYPE_NONE;
            break;
        default:
            SLIM_LOGE("Not supported pbSensorType");
            pbSensorType = PB_eSLIM_SENSOR_TYPE_NONE;
            break;
    }
    return pbSensorType;
}

pbSlimVehicleMotionSensorTypeEnumT SlimRemoteMsgsPbHandler::getPBEnumSlimVehicleMotionSensorType(
            slimVehicleMotionSensorTypeEnumT sensorType
)
{
    pbSlimVehicleMotionSensorTypeEnumT pbvnwSensorType;
    switch (sensorType) {
        case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_SPEED:
            pbvnwSensorType = PB_eSLIM_VEHICLE_MOTION_SENSOR_TYPE_SPEED;
            break;
        case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS:
            pbvnwSensorType = PB_eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS;
            break;
        case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_GEAR:
            pbvnwSensorType = PB_eSLIM_VEHICLE_MOTION_SENSOR_TYPE_GEAR;
            break;
        case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL:
            pbvnwSensorType = PB_eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL;
            break;
        default:
            SLIM_LOGE("Not supported pbvnwSensorType");
            pbvnwSensorType = PB_eSLIM_VEHICLE_MOTION_SENSOR_TYPE_MAX;
            break;
    }
    return pbvnwSensorType;
}

bool SlimRemoteMsgsPbHandler::getPbIntClientUpMsg(string &os)
{
    pbSlimRemoteMsg rMsg;

    rMsg.set_msgid(PB_eSLIM_CONTROL_MESSAGE_ID_SLIM_INT_CLIENT_UP);
    rMsg.set_msgclass(PB_eSLIM_MESSAGE_CLASS_CONTROL);
    if (!rMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on SERVICE_REQ failed!");
        return false;
    }
    return true;
}

bool SlimRemoteMsgsPbHandler::getPbAvailableServiceResp(
            slimServiceStatusEventStructT *pServiceStatusData,
            uint32_t &mAvailableServicesMask,
            string &os
)
{
    pbSlimRemoteMsg rMsg;
    pbSlimRemoteAvailSvcsRespCtrlMsg serivceResp;
    pbSlimServiceStatusEventStructT *pServices;
    int eService, pbCount;
    uint32_t mask;
    mask = mAvailableServicesMask;

    if (nullptr == pServiceStatusData)
        return false;

    rMsg.set_msgid(PB_eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_RESP);
    rMsg.set_msgclass(PB_eSLIM_MESSAGE_CLASS_DATA);
    serivceResp.set_mavailableservicesmask(mask);
    for (eService = 0, pbCount = 0; eService <= eSLIM_SERVICE_LAST; eService++)
    {
        if (SLIM_MASK_IS_SET(mask, eService)) {
            pServices = serivceResp.add_mservicestatus();
            if (pServices) {
                pServices->set_service(
                        getPBEnumForSlimService(pServiceStatusData[eService].service));
                pServices->set_servicestatus(
                    getPBEnumForSlimServiceStatus(pServiceStatusData[eService].serviceStatus));
                pServices->set_providerflags(pServiceStatusData[eService].providerFlags);
                pServices->set_availableservicesmask(
                        pServiceStatusData[eService].availableServicesMask);
            }
            pbCount++;
        }
    }
    string availServices;
    if (!serivceResp.SerializeToString(&availServices)) {
        SLIM_LOGV("SerializeToString on availServices failed!");
        return false;
    }
    rMsg.set_payload(availServices);
    if (!rMsg.SerializeToString(&os)) {
        SLIM_LOGV("SerializeToString on rMsg failed!");
        return false;
    }
    return true;
}

void SlimRemoteMsgsPbHandler::setPBheader(
            const slimMessageHeaderStructT *pzHeader,
            pbSlimMessageHeaderStructT *pbHeader
)
{
    pbHeader->set_service(getPBEnumForSlimService(pzHeader->service));
    pbHeader->set_msgtype(getPBEnumForSlimMessageType(pzHeader->msgType));
    pbHeader->set_msgerror(getPBEnumForSlimError(pzHeader->msgError));
    pbHeader->set_msgid(getPBEnumForSlimMessageId(pzHeader->msgId));
    pbHeader->set_size(pzHeader->size);
    pbHeader->set_txnid(pzHeader->txnId);
}

bool SlimRemoteMsgsPbHandler::getPbDataMsg(const slimMessageHeaderStructT *pzHeader, string &os)
{
    pbSlimRemoteMsg rMsg;
    pbSlimRemoteDataMsg pbDataMsg;
    pbSlimMessageHeaderStructT *pbHeader = pbDataMsg.mutable_mheader();
    pbSlimRemotePTPtime            *mGPTP = pbDataMsg.mutable_mgptp();

    if (nullptr == pzHeader || nullptr == pbHeader || nullptr == mGPTP)
        return false;

    rMsg.set_msgid(getPBEnumForSlimMessageId(pzHeader->msgId));
    rMsg.set_msgclass(PB_eSLIM_MESSAGE_CLASS_DATA);
    setPBheader(pzHeader, pbHeader);

    string msg;
    bool bRetVal = pbDataMsg.SerializeToString(&msg);
    pbDataMsg.clear_mheader();
    pbDataMsg.clear_mgptp();
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on pbDataMsg failed!");
        return false;
    }
    rMsg.set_payload(msg);
    if (!rMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on  failed!");
        return false;
    }
    return true;
}

bool SlimRemoteMsgsPbHandler::getPbSensorData(const slimMessageHeaderStructT *pzHeader,
            slimSensorDataStructT *pRecvMsg,
            SlimRemotePTPtime *pPTP,
            string &os
)
{
    pbSlimRemoteMsg rMsg;
    pbSlimRemoteSensorDataMsg pbSenDataMsg;
    pbSlimMessageHeaderStructT *pbHeader = pbSenDataMsg.mutable_mheader();
    pbSlimRemotePTPtime            *mGPTP = pbSenDataMsg.mutable_mgptp();
    pbSlimSensorDataStructT        *mSensorData = pbSenDataMsg.mutable_msensordata();

    if (nullptr == pzHeader || nullptr == pRecvMsg || nullptr == pPTP ||
                nullptr == mSensorData || nullptr == pbHeader || nullptr == mGPTP) {
        return false;
    }

    rMsg.set_msgid(getPBEnumForSlimMessageId(pzHeader->msgId));
    rMsg.set_msgclass(PB_eSLIM_MESSAGE_CLASS_DATA);
    setPBheader(pzHeader, pbHeader);
    mSensorData->set_provider(getPBEnumSlimServiceProvider(pRecvMsg->provider));
    mSensorData->set_timesource(getPBEnumSlimTimeSource(pRecvMsg->timeSource));
    mSensorData->set_timebase(pRecvMsg->timeBase);
    mSensorData->set_flags(pRecvMsg->flags);
    mSensorData->set_sensortype(getPBEnumSlimSensorType(pRecvMsg->sensorType));
    mSensorData->set_samples_len(pRecvMsg->samples_len);

    for (int i = 0; i < pRecvMsg->samples_len; ++i)
    {
        pbSlimSensorSampleStructT *pbSample = mSensorData->add_samples();
        if (pbSample) {
            pbSample->set_sampletimeoffsetms(pRecvMsg->samples[i].sampleTimeOffsetMs);
            pbSample->set_sampletimeoffsetsubmillins(
                        pRecvMsg->samples[i].sampleTimeOffsetSubMilliNs);
            pbSample->add_sample(pRecvMsg->samples[i].sample[0]);
            pbSample->add_sample(pRecvMsg->samples[i].sample[1]);
            pbSample->add_sample(pRecvMsg->samples[i].sample[2]);
        }

    }
    string sensorMsg;
    bool bRetVal = pbSenDataMsg.SerializeToString(&sensorMsg);
    pbSenDataMsg.clear_mheader();
    pbSenDataMsg.clear_mgptp();
    pbSenDataMsg.clear_msensordata();
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on pbSenDataMsg failed!");
        return false;
    }
    rMsg.set_payload(sensorMsg);
    if (!rMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on  failed!");
        return false;
    }
    return true;
}

bool SlimRemoteMsgsPbHandler::getPbVehicleMotionData(
            const slimMessageHeaderStructT *pzHeader,
            slimVehicleMotionDataStructT *pRecvMsg,
            SlimRemotePTPtime *pPTP,
            string &os
)
{
    pbSlimRemoteMsg rMsg;
    pbSlimRemoteVehicleMotionDataMsg pbVecMotionDataMsg;
    pbSlimMessageHeaderStructT *pbHeader = pbVecMotionDataMsg.mutable_mheader();
    pbSlimRemotePTPtime            *mGPTP = pbVecMotionDataMsg.mutable_mgptp();
    pbSlimVehicleMotionDataStructT  *mVechData = pbVecMotionDataMsg.mutable_mvehiclemotiondata();

    if (nullptr == pzHeader || nullptr == pRecvMsg || nullptr == pPTP ||
                nullptr == mVechData || nullptr == pbHeader || nullptr == mGPTP) {
        return false;
    }

    rMsg.set_msgid(getPBEnumForSlimMessageId(pzHeader->msgId));
    rMsg.set_msgclass(PB_eSLIM_MESSAGE_CLASS_DATA);
    setPBheader(pzHeader, pbHeader);
    mVechData->set_provider(getPBEnumSlimServiceProvider(pRecvMsg->provider));
    mVechData->set_timesource(getPBEnumSlimTimeSource(pRecvMsg->timeSource));
    mVechData->set_timebase(pRecvMsg->timeBase);
    mVechData->set_flags(pRecvMsg->flags);
    mVechData->set_sensortype(getPBEnumSlimVehicleMotionSensorType(pRecvMsg->sensorType));
    mVechData->set_samples_len(pRecvMsg->samples_len);

    if (pRecvMsg->sensorType == eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS)     {
        for (int i = 0; i < pRecvMsg->samples_len; ++i)
        {
            pbSlimVehicleMotionDataSampleUnionT *pbSample = mVechData->add_samples();
            if (pbSample) {
                pbSlimVehicleMotionDwsDataSampleStructT *pbDws = pbSample->mutable_dws();
                if (pbDws) {
                    pbDws->set_sampletimeoffset(pRecvMsg->samples[i].dws.sampleTimeOffset);
                    pbDws->set_flwheel(pRecvMsg->samples[i].dws.flWheel);
                    pbDws->set_frwheel(pRecvMsg->samples[i].dws.frWheel);
                    pbDws->set_rlwheel(pRecvMsg->samples[i].dws.rlWheel);
                    pbDws->set_rrwheel(pRecvMsg->samples[i].dws.rrWheel);
                }
            }
        }
    } else if (pRecvMsg->sensorType == eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL) {
        for (int i = 0; i < pRecvMsg->samples_len; ++i)
        {
            pbSlimVehicleMotionDataSampleUnionT *pbSample = mVechData->add_samples();
            if (pbSample) {
                pbSlimVehicleMotionSteeringWheelDataSampleStructT *pbsteering =
                                                    pbSample->mutable_steering_wheel();
                if (pbsteering) {
                    pbsteering->set_sampletimeoffset(
                                    pRecvMsg->samples[i].steering_wheel.sampleTimeOffset);
                    pbsteering->set_data(pRecvMsg->samples[i].steering_wheel.data);
                }
            }
        }
    }

    string vnwMsg;
    bool bRetVal = pbVecMotionDataMsg.SerializeToString(&vnwMsg);
    // free memory
    pbSlimVehicleMotionDataStructT vechData = pbVecMotionDataMsg.mvehiclemotiondata();
    if (pRecvMsg->sensorType == eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS)     {
        for (int i = 0; i < pRecvMsg->samples_len; ++i)
        {
            pbSlimVehicleMotionDataSampleUnionT pbSample = vechData.samples(i);
            pbSample.clear_dws();
        }
    } else if (pRecvMsg->sensorType == eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL) {
        for (int i = 0; i < pRecvMsg->samples_len; ++i)
        {
            pbSlimVehicleMotionDataSampleUnionT pbSample = vechData.samples(i);
            pbSample.clear_steering_wheel();
        }
    }
    pbVecMotionDataMsg.clear_mheader();
    pbVecMotionDataMsg.clear_mgptp();
    pbVecMotionDataMsg.clear_mvehiclemotiondata();
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on pbSenDataMsg failed!");
        return false;
    }
    rMsg.set_payload(vnwMsg);
    if (!rMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on  failed!");
        return false;
    }
    return true;
}

bool SlimRemoteMsgsPbHandler::getPbAvailableServicesReq(string &os)
{
    SLIM_LOGD("Requesting Available Services..");
    pbSlimRemoteMsg rMsg;

    rMsg.set_msgid(PB_eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_REQ);
    rMsg.set_msgclass(PB_eSLIM_MESSAGE_CLASS_DATA);
    if (!rMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on SERVICE_REQ failed!");
        return false;
    }
    return true;
}

bool SlimRemoteMsgsPbHandler::getPbSensorDataReq(
    bool         uEnable,
    uint16_t     wReportRateHz,
    uint16_t     wSampleCount,
    slimServiceEnumT eService,
    string             &os
)
{
    pbSlimRemoteMsg                     rMsg;
    pbSlimRemoteSensorDataReqCtrlMsg     sesnorReqCtrlMsg;
    pbSlimEnableSensorDataRequestStructT *psensorReq;
    pbSlimEnableServiceDataStructT          *pEnableConf;
    string      sensorReqMsg;
    int         temp = 0;

    psensorReq = sesnorReqCtrlMsg.mutable_msensordatarequest();
    if (psensorReq) {
        pEnableConf = psensorReq->mutable_enableconf();
        if (nullptr == pEnableConf)
            return false;
    } else {
        return false;
    }

    rMsg.set_msgid(PB_eSLIM_CONTROL_MESSAGE_ID_SENSOR_DATA_REQ);
    rMsg.set_msgclass(PB_eSLIM_MESSAGE_CLASS_CONTROL);

    SLIM_MASK_SET(
        temp,
        eSLIM_SERVICE_PROVIDER_NATIVE
        );
    pEnableConf->set_enable(uEnable);
    pEnableConf->set_providerflags(temp);
    psensorReq->set_reportrate(wReportRateHz);
    psensorReq->set_samplecount(wSampleCount);
    psensorReq->set_sensor(getPBEnumForSlimService(eService));
    psensorReq->set_batchalignreq_valid(true);

    bool bRetVal = sesnorReqCtrlMsg.SerializeToString(&sensorReqMsg);
    // free memory
    pbSlimEnableSensorDataRequestStructT sensorReq = sesnorReqCtrlMsg.msensordatarequest();
    sensorReq.clear_enableconf();
    sesnorReqCtrlMsg.clear_msensordatarequest();
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on SENSOR_DATA_REQ failed!");
        return false;
    }
    rMsg.set_payload(sensorReqMsg);
    if (!rMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on rMsg failed!");
        return false;
    }
    return true;
}

bool SlimRemoteMsgsPbHandler::getPbVnwDataReq(
    bool uEnable,
    slimServiceEnumT eService,
    string             &os
)
{
    pbSlimRemoteMsg                     rMsg;
    pbSlimRemoteVehicleDataReqCtrlMsg   vnwReqCtrlMsg;
    int temp = 0;
    pbSlimEnableVehicleDataRequestStructT     *pVehicleReq;
    pbSlimEnableServiceDataStructT            *pEnableConf;
    string      vnwReqMsg;
    pVehicleReq = vnwReqCtrlMsg.mutable_mvehicledatarequest();
    if (pVehicleReq) {
        pEnableConf = pVehicleReq->mutable_enableconf();
        if (nullptr == pEnableConf)
            return false;
    } else {
        return false;
    }

    rMsg.set_msgid(PB_eSLIM_CONTROL_MESSAGE_ID_VEHICLE_DATA_REQ);
    rMsg.set_msgclass(PB_eSLIM_MESSAGE_CLASS_DATA);
    SLIM_MASK_SET(
        temp,
        eSLIM_SERVICE_PROVIDER_NATIVE
        );
    pEnableConf->set_enable(uEnable);
    pEnableConf->set_providerflags(temp);
    pVehicleReq->set_service(getPBEnumForSlimService(eService));
    bool bRetVal = vnwReqCtrlMsg.SerializeToString(&vnwReqMsg);
    // free memory
    pbSlimEnableVehicleDataRequestStructT vehicleReq = vnwReqCtrlMsg.mvehicledatarequest();
    vehicleReq.clear_enableconf();
    vnwReqCtrlMsg.clear_mvehicledatarequest();
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on VNW_DATA_REQ failed!");
        return false;
    }
    rMsg.set_payload(vnwReqMsg);
    if (!rMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on rMsg failed!");
        return false;
    }
    return true;
}

bool SlimRemoteMsgsPbHandler::getSlimSensorDataReq(
            pbSlimRemoteSensorDataReqCtrlMsg &sensorReqmsg,
            slimEnableSensorDataRequestStructT &mSensorDataRequest
)
{
    if (!sensorReqmsg.has_msensordatarequest()) {
        LOC_LOGe("getSlimSensorDataReq oneof required params are not available!");
        return false;
    }
    pbSlimEnableSensorDataRequestStructT psensorReq = sensorReqmsg.msensordatarequest();
    pbSlimEnableServiceDataStructT pEnableConf = psensorReq.enableconf();

    mSensorDataRequest.enableConf.enable = pEnableConf.enable();
    mSensorDataRequest.enableConf.providerFlags = pEnableConf.providerflags();
    mSensorDataRequest.reportRate = psensorReq.reportrate();
    mSensorDataRequest.sampleCount = psensorReq.samplecount();
    mSensorDataRequest.sensor = getSlimService(psensorReq.sensor());
    mSensorDataRequest.batchAlignReq_valid = psensorReq.batchalignreq_valid();

    return true;
}

bool SlimRemoteMsgsPbHandler::getSlimVehicleMotionDataReq(
        pbSlimRemoteVehicleDataReqCtrlMsg         &vehicleReqmsg,
        slimEnableVehicleDataRequestStructT     &mVehicleataRequest
        )
{
    if (!vehicleReqmsg.has_mvehicledatarequest()) {
        LOC_LOGe("getSlimVehicleMotionDataReq oneof required params are not available!");
        return false;
    }
    pbSlimEnableVehicleDataRequestStructT pVehicleReq = vehicleReqmsg.mvehicledatarequest();
    pbSlimEnableServiceDataStructT pEnableConf = pVehicleReq.enableconf();
    mVehicleataRequest.enableConf.enable = pEnableConf.enable();
    mVehicleataRequest.enableConf.providerFlags = pEnableConf.providerflags();
    mVehicleataRequest.service = getSlimService(pVehicleReq.service());
    return true;
}

bool SlimRemoteMsgsPbHandler::storeAvailableServiceInfoPB(
        pbSlimRemoteAvailSvcsRespCtrlMsg     &pbSvcsResp,
        slimServiceStatusEventStructT        *pServiceStatusData,
        uint32_t                             &mAvailableServicesMask,
        uint32_t                             &m_serviceStatusCount
)
{
    pbSlimServiceStatusEventStructT pServices;
    int                 eService, pbCount;
    uint32_t            m_qEnabledServices;

    if (nullptr == pServiceStatusData)
        return false;

    m_qEnabledServices = pbSvcsResp.mavailableservicesmask();
    for (eService = 0, pbCount = 0; eService <= eSLIM_SERVICE_LAST; eService++)
    {
        if (SLIM_MASK_IS_SET(m_qEnabledServices, eService)) {
            pServices = pbSvcsResp.mservicestatus(pbCount);
            pServiceStatusData[eService].service = getSlimService(pServices.service());
            pServiceStatusData[eService].serviceStatus =
                                        getSlimServiceStatus(pServices.servicestatus());
            pServiceStatusData[eService].providerFlags = pServices.providerflags();
            pServiceStatusData[eService].availableServicesMask =
                                        pServices.availableservicesmask();
            pbCount++;
            m_serviceStatusCount++;
        }
    }
    mAvailableServicesMask = m_qEnabledServices;
    return true;
}

bool SlimRemoteMsgsPbHandler::getSlimSensorData(
                    pbSlimRemoteSensorDataMsg         &pbSesnorData,
                    slimSensorDataStructT             &zSensorData,
                    SlimRemotePTPtime                &gPTP
)
{
    if (!pbSesnorData.has_mheader() || !pbSesnorData.has_mgptp() ||
            !pbSesnorData.has_msensordata() ) {
        LOC_LOGe("getSlimSensorData oneof required params are not available!");
        return false;
    }

    pbSlimRemotePTPtime            pGPTP = pbSesnorData.mgptp();
    pbSlimSensorDataStructT        pSensorData = pbSesnorData.msensordata();

    zSensorData.provider = getSlimServiceProvider(pSensorData.provider());
    zSensorData.timeSource = getSlimTimeSource(pSensorData.timesource());
    zSensorData.timeBase = pSensorData.timebase();
    zSensorData.flags = pSensorData.flags();
    zSensorData.sensorType = getSlimSensorType(pSensorData.sensortype());
    zSensorData.samples_len = pSensorData.samples_len();
    gPTP.qtOffsetCalculationDone = pGPTP.qtoffsetcalcdone();
    gPTP.gPTPtime = pGPTP.gptptime();
    //samples
    for (int i = 0; i < zSensorData.samples_len; ++i)
    {
        pbSlimSensorSampleStructT pbSample = pSensorData.samples(i);
        zSensorData.samples[i].sampleTimeOffsetMs = pbSample.sampletimeoffsetms();
        zSensorData.samples[i].sampleTimeOffsetSubMilliNs =
                                            pbSample.sampletimeoffsetsubmillins();
        zSensorData.samples[i].sample[0] = pbSample.sample(0);
        zSensorData.samples[i].sample[1] = pbSample.sample(1);
        zSensorData.samples[i].sample[2] = pbSample.sample(2);
    }
    return true;
}

bool SlimRemoteMsgsPbHandler::getSlimVehicleMotionData(
    pbSlimRemoteVehicleMotionDataMsg    &pbVecMotionDataMsg,
    slimVehicleMotionDataStructT        &zVnwData,
    SlimRemotePTPtime                   &gPTP
)
{
    if (!pbVecMotionDataMsg.has_mheader() || !pbVecMotionDataMsg.has_mgptp() ||
            !pbVecMotionDataMsg.has_mvehiclemotiondata() ) {
        LOC_LOGe("getSlimVehicleMotionData oneof required params are not available!");
        return false;
    }

    pbSlimRemotePTPtime                pGPTP = pbVecMotionDataMsg.mgptp();
    pbSlimVehicleMotionDataStructT  pVnwData = pbVecMotionDataMsg.mvehiclemotiondata();

    zVnwData.provider = getSlimServiceProvider(pVnwData.provider());
    zVnwData.timeSource = getSlimTimeSource(pVnwData.timesource());
    zVnwData.timeBase = pVnwData.timebase();
    zVnwData.flags = pVnwData.flags();
    zVnwData.sensorType = getSlimVnwSensorType(pVnwData.sensortype());
    zVnwData.samples_len = pVnwData.samples_len();
    gPTP.qtOffsetCalculationDone = pGPTP.qtoffsetcalcdone();
    gPTP.gPTPtime = pGPTP.gptptime();
    //samples
    if (zVnwData.sensorType == eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS) {
        for (int i = 0; i < zVnwData.samples_len; ++i)
        {
            pbSlimVehicleMotionDataSampleUnionT pbSample = pVnwData.samples(i);
            pbSlimVehicleMotionDwsDataSampleStructT pbDws = pbSample.dws();
            zVnwData.samples[i].dws.sampleTimeOffset = pbDws.sampletimeoffset();
            zVnwData.samples[i].dws.flWheel = pbDws.flwheel();
            zVnwData.samples[i].dws.frWheel = pbDws.frwheel();
            zVnwData.samples[i].dws.rlWheel = pbDws.rlwheel();
            zVnwData.samples[i].dws.rrWheel = pbDws.rrwheel();
        }
    } else if (zVnwData.sensorType == eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL) {
        for (int i = 0; i < zVnwData.samples_len; ++i)
        {
            pbSlimVehicleMotionDataSampleUnionT pbSample = pVnwData.samples(i);
            pbSlimVehicleMotionSteeringWheelDataSampleStructT pbsteering =
                            pbSample.steering_wheel();
            zVnwData.samples[i].steering_wheel.sampleTimeOffset =
                                    pbsteering.sampletimeoffset();
            zVnwData.samples[i].steering_wheel.data = pbsteering.data();
        }
    }
    return true;
}
