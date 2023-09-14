/*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "slim_apss_log.h"
#include "slim_os_log_api.h"
#include <stdint.h>

#ifndef LOG_GNSS_SLIM_SENSOR_DATA_C
#define LOG_GNSS_SLIM_SENSOR_DATA_C (0x1CC3)
#endif

static void slimLogSensorPacket
(
  slimServiceEnumT eService,
  slimLogSensorData * pzLog,
  const void *pData,
  uint32_t dataSize
)
{
    if (NULL == pData || NULL == pzLog) {
      SLIM_LOGE("Empty Sensor packets...");
      return;
    }
    SLIM_LOGD("Logging Service %d", eService);
    slimSensorDataStructT *data = (slimSensorDataStructT*)pData;
    pzLog->samplesCount = data->samples_len;
    pzLog->sampleTimeBaseMsec = data->timeBase;
    if (data->flags & SLIM_FLAGS_MASK_SENSOR_BUFFER_DATA) {
      pzLog->msg.type = eSLIM_BUFFERED_DATA_TYPE;
    } else {
      pzLog->msg.type = eSLIM_LIVE_DATA_TYPE;
    }
    for (int index = 0; index < pzLog->samplesCount; index++) {
      switch (data->sensorType){
      case eSLIM_SENSOR_TYPE_ACCEL:
      case eSLIM_SENSOR_TYPE_GYRO:
        pzLog->samples[index].data.sensorXYZ[0] = data->samples[index].sample[0];
        pzLog->samples[index].data.sensorXYZ[1] = data->samples[index].sample[1];
        pzLog->samples[index].data.sensorXYZ[2] = data->samples[index].sample[2];
        /* Nano second time offset support */
        pzLog->samples[index].sampleTimeOffset.Nsec =
                       (data->samples[index].sampleTimeOffsetMs * 1e6) +
                       data->samples[index].sampleTimeOffsetSubMilliNs;
        break;
      case eSLIM_SENSOR_TYPE_ACCEL_TEMP:
      case eSLIM_SENSOR_TYPE_GYRO_TEMP:
        pzLog->samples[index].data.sensorTemp = data->samples[index].sample[0];
        /* Nano second time offset not required for temperature sensor */
        pzLog->samples[index].sampleTimeOffset.Usec = data->samples[index].sampleTimeOffsetMs;
        break;
      default:
        SLIM_LOGE("Unsupported sensor type..");
        break;
      }
    }
    return;
}

static void slimLogVehicleMotionPacket
(
  slimServiceEnumT eService,
  slimLogSensorData * pzLog,
  const void *pData,
  uint32_t dataSize
)
{
    if (NULL == pData || NULL == pzLog) {
      SLIM_LOGE("Empty Vehicle Motion packets...");
      return;
    }

    slimVehicleMotionDataStructT *data = (slimVehicleMotionDataStructT*)pData;
    pzLog->samplesCount = data->samples_len;
    pzLog->sampleTimeBaseMsec = data->timeBase;
    if (data->flags & SLIM_FLAGS_MASK_SENSOR_BUFFER_DATA) {
      pzLog->msg.type = eSLIM_BUFFERED_DATA_TYPE;
    } else {
      pzLog->msg.type = eSLIM_LIVE_DATA_TYPE;
    }
    for (int index = 0; index < pzLog->samplesCount; index++) {
      switch (data->sensorType){
      case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_SPEED:
        pzLog->samples[index].data.vehicleSpeed = data->samples[index].speed.data;
        pzLog->samples[index].sampleTimeOffset.Usec = data->samples[index].speed.sampleTimeOffset;
        break;
      case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS:
        pzLog->samples[index].data.dwsFlFrRlRr[0] = data->samples[index].dws.flWheel;
        pzLog->samples[index].data.dwsFlFrRlRr[1] = data->samples[index].dws.frWheel;
        pzLog->samples[index].data.dwsFlFrRlRr[2] = data->samples[index].dws.rlWheel;
        pzLog->samples[index].data.dwsFlFrRlRr[3] = data->samples[index].dws.rrWheel;
        pzLog->samples[index].sampleTimeOffset.Usec = data->samples[index].dws.sampleTimeOffset;
        break;
      case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_GEAR:
        pzLog->samples[index].data.vehicleGear = data->samples[index].gear.data;
        pzLog->samples[index].sampleTimeOffset.Usec = data->samples[index].gear.sampleTimeOffset;
        break;
      case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL:
          pzLog->samples[index].data.steeringWheel= data->samples[index].steering_wheel.data;
          pzLog->samples[index].sampleTimeOffset.Usec =
                                        data->samples[index].steering_wheel.sampleTimeOffset;
        break;
      }
    }

    return;
}

static void slimLogServiceInfo
(
  slimServiceEnumT eService,
  slimLogSensorData * pzLog
)
{
    if (NULL == pzLog) {
      SLIM_LOGE("Empty service packets...");
      return;
    }

    switch (eService)
    {
      case eSLIM_SERVICE_SENSOR_ACCEL:
        pzLog->serviceInfo = SLIM_DIAG_3AXIS_ACCEL_MASK;
        break;
      case eSLIM_SERVICE_SENSOR_GYRO:
        pzLog->serviceInfo = SLIM_DIAG_3AXIS_GYRO_MASK;
        break;
      case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
      case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
        pzLog->serviceInfo = SLIM_DIAG_TEMPERATURE_MASK;
        break;
      case eSLIM_SERVICE_VEHICLE_SPEED:
        pzLog->serviceInfo = SLIM_DIAG_VEHICLE_SPEED_MASK;
        break;
      case eSLIM_SERVICE_VEHICLE_DWS:
        pzLog->serviceInfo = SLIM_DIAG_VEHICLE_DWS_MASK;
        break;
      case eSLIM_SERVICE_VEHICLE_GEAR:
        pzLog->serviceInfo = SLIM_DIAG_VEHICLE_GEAR_MASK;
        break;
      case eSLIM_SERVICE_VEHICLE_STEERING_WHEEL:
        pzLog->serviceInfo = SLIM_DIAG_VEHICLE_STEERING_WHL_MASK;
        break;
      default:
        SLIM_LOGE("Unsupported service...");
        return;
    }
}

static void slimLogFillHeader(slimDiagGenericHeader * pHeader)
{
    pHeader->versionInfo = SLIM_APSS_LOG_VERSION;
    pHeader->processorInfo = (uint8_t)slim_CurrentProcessor();
    pHeader->timeMsec = slim_TimeTickGetMilliseconds();
    return;
}

static bool checkDiagLevel(slimDiagSource msgSource)
{
    bool retValue = false;

    if (LOG_LOCAL_PROVIDER_PACKETS == slimGetSensorDiagLevel()&&
        eSLIM_LOG_SOURCE_OTHER == msgSource) {
      /* Log sensorhal, vnw prov only */
      retValue = true;
    } else if (LOG_RP_IC_PACKETS == slimGetSensorDiagLevel() &&
        eSLIM_LOG_SOURCE_OTHER != msgSource) {
      /* Log remote prov and internal client only */
      retValue = true;
    }  else if (LOG_ALL_PACKETS == slimGetSensorDiagLevel()) {
      /* Log all diag packets */
      retValue = true;
    }

    return retValue;
}

void slimLogDiagPkt
(
    slimDiagSource msgSource,
    slimDiagTimeSource timeSource,
    uint32_t id,
    uint64_t sampledTimeUsec,
    const void *pData,
    uint32_t dataSize,
    slimServiceEnumT eService
)
{
    if (false == checkDiagLevel(msgSource)) {
      SLIM_LOGI("Diag not enabled..");
      return;
    }

    slimLogSensorData * pzLog = NULL;
    uint32_t pktSize = sizeof(slimLogSensorData);
    pzLog = (slimLogSensorData *)log_alloc(LOG_GNSS_SLIM_SENSOR_DATA_C, pktSize);

    if (NULL == pzLog) {
      SLIM_LOGE("Error in mem alloc for slim diag pkt...");
      return;
    }

    /* fill header */
    slimLogFillHeader(&pzLog->msgHeader);

    /* Pkt Time source*/
    pzLog->msg.time.source = timeSource;
    pzLog->msg.time.sampledTimeUsec = sampledTimeUsec;

    /* msgSource and ID */
    if (eSLIM_LOG_SOURCE_INTERNAL_CLIENT == msgSource ||
            eSLIM_LOG_SOURCE_REMOTE_PROVIDER == msgSource) {
      pzLog->msg.source = msgSource;
      pzLog->msg.id.remoteMsgId = id;
    } else if (eSLIM_LOG_SOURCE_OTHER == msgSource) {
      pzLog->msg.source = msgSource;
      pzLog->msg.id.providerId = id;
    }
    slimLogServiceInfo(eService, pzLog);

    switch (eService) {
      case eSLIM_SERVICE_SENSOR_ACCEL:
      case eSLIM_SERVICE_SENSOR_GYRO:
      case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
      case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
        slimLogSensorPacket(eService, pzLog, pData, dataSize);
        break;
      case eSLIM_SERVICE_VEHICLE_SPEED:
      case eSLIM_SERVICE_VEHICLE_GEAR:
      case eSLIM_SERVICE_VEHICLE_DWS:
      case eSLIM_SERVICE_VEHICLE_STEERING_WHEEL:
        slimLogVehicleMotionPacket(eService, pzLog, pData, dataSize);
        break;
      default:
        SLIM_LOGE("Invalid service ID..");
        return;
        break;
    }

    log_commit(pzLog);

    return;
}
