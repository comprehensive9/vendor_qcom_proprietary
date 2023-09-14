/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2012 The Android Open Source Project
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
 */
#ifdef SNS_SUPPORT_DIRECT_CHANNEL
/* Taken from sensors_base.h, standard Android sensors */
#define SENSOR_TYPE_ACCELEROMETER                       (1)
#define SENSOR_TYPE_GEOMAGNETIC_FIELD                   (2)
#define SENSOR_TYPE_MAGNETIC_FIELD                      SENSOR_TYPE_GEOMAGNETIC_FIELD
#define SENSOR_TYPE_GYROSCOPE                           (4)
#define SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED         (14)
#define SENSOR_TYPE_GYROSCOPE_UNCALIBRATED              (16)
#define SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED          (35)

/* Taken from sensors_qti.h for dual sensor */
#define QTI_DUAL_SENSOR_TYPE_BASE                       (268369920)
#define QTI_SENSOR_TYPE_ACCELEROMETER                   (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_ACCELEROMETER)
#define QTI_SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED      (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED)
#define QTI_SENSOR_TYPE_GYROSCOPE                       (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_GYROSCOPE)
#define QTI_SENSOR_TYPE_GYROSCOPE_UNCALIBRATED          (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_GYROSCOPE_UNCALIBRATED)
#define QTI_SENSOR_TYPE_MAGNETIC_FIELD                  (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_MAGNETIC_FIELD)
#define QTI_SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED     (QTI_DUAL_SENSOR_TYPE_BASE + SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED)

/* Definition for packed structures */
#define PACK(x)   x __attribute__((__packed__))

/* Taken from sensors.h */
/**
 * sensor event data
 */
/**
 * sensor event data
 */
typedef struct {
    union {
        float v[3];
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float azimuth;
            float pitch;
            float roll;
        };
    };
    int8_t status;
    uint8_t reserved[3];
} sensors_vec_t;

/* Taken from sensors.h */
/**
 * uncalibrated accelerometer, gyroscope and magnetometer event data
 */
typedef struct {
  union {
    float uncalib[3];
    struct {
      float x_uncalib;
      float y_uncalib;
      float z_uncalib;
    };
  };
  union {
    float bias[3];
    struct {
      float x_bias;
      float y_bias;
      float z_bias;
    };
  };
} uncalibrated_event_t;

/* Stripped down version of sensors_event_t from sensors.h */
/**
 * Union of the various types of sensor data
 * that can be returned.
 */
typedef PACK(struct) sensors_event_t {
    /* must be sizeof(struct sensors_event_t) */
    int32_t version;

    /* sensor identifier */
    int32_t sensor;

    /* sensor type */
    int32_t type;

    /* reserved */
    int32_t reserved0;

    /* time is in nanosecond */
    int64_t timestamp;

    union {
        union {
            float           data[16];

            /* acceleration values are in meter per second per second (m/s^2) */
            sensors_vec_t   acceleration;

            /* magnetic vector values are in micro-Tesla (uT) */
            sensors_vec_t   magnetic;

            /* orientation values are in degrees */
            sensors_vec_t   orientation;

            /* gyroscope values are in rad/s */
            sensors_vec_t   gyro;

            /* temperature is in degrees centigrade (Celsius) */
            float           temperature;

            /* distance in centimeters */
            float           distance;

            /* light in SI lux units */
            float           light;

            /* pressure in hectopascal (hPa) */
            float           pressure;

            /* relative humidity in percent */
            float           relative_humidity;

            /* uncalibrated gyroscope values are in rad/s */
            uncalibrated_event_t uncalibrated_gyro;

            /* uncalibrated magnetometer values are in micro-Teslas */
            uncalibrated_event_t uncalibrated_magnetic;

            /* uncalibrated accelerometer values
             * in  meter per second per second (m/s^2) */
            uncalibrated_event_t uncalibrated_accelerometer;
        };

        union {
            uint64_t        data[8];

            /* step-counter */
            uint64_t        step_counter;
        } u64;
    };

    /* Reserved flags for internal use. Set to zero. */
    uint32_t flags;

    uint32_t reserved1[3];
} sensors_event_t;
#endif


