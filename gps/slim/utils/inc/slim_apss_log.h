/*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#ifndef SLIM_APSS_LOG_H
#define SLIM_APSS_LOG_H

#include "slim_apss_diag.h"
#include <libslimcommon/slim_client_types.h>
#include <libslimcommon/slim_internal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  /* Log packets orginated only from sensorhal, VNW prov */
  LOG_LOCAL_PROVIDER_PACKETS = 1,
  /* Log packets orginated only from internal client and RP prov */
  LOG_RP_IC_PACKETS = 2,
  /* Log all incoming packets */
  LOG_ALL_PACKETS = 3,
} eSLIM_SENSOR_DIAG_LEVELS;

/**
@brief SLIM APSS diag support for sensor data.

Generic function to log diag messages from slim apss module

@param[in] msgSource     Source from where sensor data is logged
@param[in] timeSource    Time source used for sensor samples
@param[in] id            If remote message id or provider id
@param[in] currTime      Time when sensor data is logged
@param[in] pData         Sensor data payload
@param[in] datasize      payload size
@param[in] eService      SLIM service.

@return None
*/
void slimLogDiagPkt
(
    slimDiagSource msgSource,
    slimDiagTimeSource timeSource,
    uint32_t id,
    uint64_t sampledTimeUsec,
    const void *pData,
    uint32_t dataSize,
    slimServiceEnumT eService
);

#ifdef __cplusplus
}
#endif

#endif
