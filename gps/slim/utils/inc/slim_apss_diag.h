/*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef SLIM_APSS_DIAG_H
#define SLIM_APSS_DIAG_H
#include <log.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SLIM_APSS_LOG_VERSION 0x01

/* Defined in slim/common/client/inc/slim_client_types.h */
#define SLIM_SENSOR_MAX_SAMPLE_SETS 50
#define SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS 65

#define SLIM_DIAG_3AXIS_ACCEL_MASK              0x00000001
#define SLIM_DIAG_3AXIS_GYRO_MASK               0x00000002
#define SLIM_DIAG_3AXIS_CAN_SPEED_MASK          0x00000003
#define SLIM_DIAG_VEHICLE_SPEED_MASK            0x00000004
#define SLIM_DIAG_VEHICLE_GEAR_MASK             0x00000005
#define SLIM_DIAG_TEMPERATURE_MASK              0x00000006
#define SLIM_DIAG_VEHICLE_DWS_MASK              0x00000007
#define SLIM_DIAG_VEHICLE_ACCEL_MASK            0x00000008
#define SLIM_DIAG_VEHICLE_GYRO_MASK             0x00000009
#define SLIM_DIAG_VEHICLE_STEERING_WHL_MASK     0x0000000A

typedef PACKED struct PACKED_POST
{
    log_hdr_type reqHeader;                    /**< Used by Logging Module */
    uint8_t versionInfo;                       /**< SLIM APSS Diag logging version details */
    uint8_t processorInfo;                     /**< Processor type of SLIM host. */
    uint64_t timeMsec;                         /**< diag pkt generated time in slim diag util */
} slimDiagGenericHeader;

typedef enum {
    eSLIM_LOG_SOURCE_INTERNAL_CLIENT = 0, /**< log originated from slim internal client. */
    eSLIM_LOG_SOURCE_REMOTE_PROVIDER, /**< log originated from slim remote provider. */
    eSLIM_LOG_SOURCE_OTHER, /**< log originated from any other providers like
                               sensor hal provider
                               VNW provider. */
    eSLIM_LOG_SOURCE_TYPE_MAX = 2147483647 /* To force 32 bit signed enum.
                                           Do not change or use */
} slimDiagSource;

typedef enum {
  eSLIM_BUFFERED_DATA_TYPE = 0,
  eSLIM_LIVEBUFFERED_DATA_TYPE,
  eSLIM_LIVE_DATA_TYPE,
  eSLIM_DATA_TYPE_MAX = 2147483647 /* To force 32 bit signed enum.
                                             Do not change or use */
} slimDiagDataType;

typedef enum {
    eSLIM_TIME_SOURCE_GPTP_TIME = 0,
    eSLIM_TIME_SOURCE_QTIMER_TIME,
    eSLIM_TIME_SOURCE_APSS_TIME,
    eSLIM_TIME_SOURCE_TYPE_MAX = 2147483647 /* To force 32 bit signed enum.
                                             Do not change or use */
} slimDiagTimeSource;

typedef PACKED struct PACKED_POST
{
    slimDiagTimeSource source;         /**< time source used */
    uint64_t sampledTimeUsec;          /**< data sampled time(Usec) in slim */
} slimDiagTime;

typedef PACKED struct PACKED_POST
{
    /**<Log packet source info */
    slimDiagSource     source;
    /*Based on source either remoteMsg ID or provider ID is reported*/
    union {
      /* Incase of source is remote provider and internal client */
      uint32_t remoteMsgId;
      /* Incase of source is other provider */
      uint32_t providerId;
    }id;
    slimDiagTime       time;            /**< Diag packet time info */
    slimDiagDataType   type;            /**< type of data logged */
} slimDiagContent;

/*! @brief Struct for SLIM sensor data logging */
typedef PACKED struct PACKED_POST
{
    /**<Log generic header info */
    slimDiagGenericHeader   msgHeader;
    /**<Log packet content info */
    slimDiagContent         msg;
    /**< Service info */
    uint8_t                 serviceInfo;
    /**< Number of samples reported */
    uint8_t                 samplesCount;
    /**< Sample Time base */
    uint64_t                sampleTimeBaseMsec;
    PACKED struct PACKED_POST
    {
       /*
          Offset from the absolute time above
          sampleTimeOffset is in Usec for CAN data
          sampleTimeOffset is in Nsec for IMU data
        */
       union {
         uint64_t      Usec;
         uint64_t      Nsec;
       } sampleTimeOffset;
       union {
          float         sensorXYZ[3];      // Offset reported in Nsec
          float         vehicleSpeed;      // Offset reported in Usec
          float         steeringWheel;     // Offset reported in Usec
          float         sensorTemp;        // Offset reported in Usec
          float         dwsFlFrRlRr[4];    // Offset reported in Usec
          uint8_t       vehicleGear;       // Offset reported in Usec
       } data;
    } samples[SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS];
} slimLogSensorData;

#ifdef __cplusplus
}
#endif

#endif
