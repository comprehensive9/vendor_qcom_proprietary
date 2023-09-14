/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <log.h>
#include <log_codes.h>

#define IZAT_FLP_DIAG_BATCH_SIZE_MAX            (20)
#define CLIENT_DIAG_GNSS_MEASUREMENTS_MAX  (128)
#define LOG_IZAT_FLP_DIAG_MSG_VERSION    (1)

#ifndef LOG_GNSS_IZAT_FLP_REPORT_C
#define LOG_GNSS_IZAT_FLP_REPORT_C (0x1D4C)
#endif

#define  DIAG_LOG_TAG    "LOCSDK_DIAG_JNI"
#define LOC_LOGe(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, DIAG_LOG_TAG, "%s:%d] " fmt, \
        __func__, __LINE__, ##__VA_ARGS__)
#define LOC_LOGw(fmt, ...) __android_log_print(ANDROID_LOG_WARN, DIAG_LOG_TAG, "%s:%d] " fmt, \
        __func__, __LINE__, ##__VA_ARGS__)
#define LOC_LOGd(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, DIAG_LOG_TAG, "%s:%d] " fmt, \
        __func__, __LINE__, ##__VA_ARGS__)
#define LOC_LOGi(fmt, ...) __android_log_print(ANDROID_LOG_INFO, DIAG_LOG_TAG, "%s:%d] " fmt, \
        __func__, __LINE__, ##__VA_ARGS__)
#define LOC_LOGv(fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE, DIAG_LOG_TAG, "%s:%d] " fmt, \
        __func__, __LINE__, ##__VA_ARGS__)

// Flags to indicate which values are valid in a Location
typedef uint16_t DiagLocationFlagsMask;
typedef enum {
    DIAG_LOCATION_HAS_LAT_LONG_BIT          = (1<<0), // location has valid latitude and longitude
    DIAG_LOCATION_HAS_ALTITUDE_BIT          = (1<<1), // location has valid altitude
    DIAG_LOCATION_HAS_SPEED_BIT             = (1<<2), // location has valid speed
    DIAG_LOCATION_HAS_BEARING_BIT           = (1<<3), // location has valid bearing
    DIAG_LOCATION_HAS_ACCURACY_BIT          = (1<<4), // location has valid accuracy
    DIAG_LOCATION_HAS_VERTICAL_ACCURACY_BIT = (1<<5), // location has valid vertical accuracy
    DIAG_LOCATION_HAS_SPEED_ACCURACY_BIT    = (1<<6), // location has valid speed accuracy
    DIAG_LOCATION_HAS_BEARING_ACCURACY_BIT  = (1<<7), // location has valid bearing accuracy
    DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_BIT = (1<<8), // location has valid elapsed real time
    DIAG_LOCATION_HAS_CONFORMITY_INDEX_BIT  = (1<<9), // location has valid conformity index
    DIAG_LOCATION_HAS_QUALITY_TYPE_BIT      = (1<<10), // location has valid quality type
} DiagLocationFlagsBits;
typedef uint16_t DiagLocationTechnologyMask;
// mask indicating location calculations including...
typedef enum {
    DIAG_LOCATION_TECHNOLOGY_GNSS_BIT                     = (1<<0), // using GNSS
    DIAG_LOCATION_TECHNOLOGY_CELL_BIT                     = (1<<1), // using Cell
    DIAG_LOCATION_TECHNOLOGY_WIFI_BIT                     = (1<<2), // using WiFi
    DIAG_LOCATION_TECHNOLOGY_SENSORS_BIT                  = (1<<3), // using Sensors
    DIAG_LOCATION_TECHNOLOGY_REFERENCE_LOCATION_BIT       = (1<<4), // using reference location
    DIAG_LOCATION_TECHNOLOGY_INJECTED_COARSE_POSITION_BIT = (1<<5), // using CPI
    DIAG_LOCATION_TECHNOLOGY_AFLT_BIT                     = (1<<6), // AFLT
    DIAG_LOCATION_TECHNOLOGY_HYBRID_BIT                   = (1<<7), // HYBRID
    DIAG_LOCATION_TECHNOLOGY_PPE_BIT                      = (1<<8), // PPE
    DIAG_LOCATION_TECHNOLOGY_VEH_BIT                      = (1<<9), // using vehicular data
    DIAG_LOCATION_TECHNOLOGY_VIS_BIT                      = (1<<10), // using visual data
    DIAG_LOCATION_TECHNOLOGY_DGNSS_BIT                    = (1<<11),  // DGNSS
} DiagLocationTechnologyBits;

typedef enum {
    DIAG_LOCATION_STANDALONE_QUALITY_TYPE,
    DIAG_LOCATION_DGNSS_QUALITY_TYPE,
    DIAG_LOCATION_FLOAT_QUALITY_TYPE,
    DIAG_LOCATION_FIXED_QUALITY_TYPE
} DiagLocationQualityType;

typedef PACKED struct PACKED_POST {
    uint32_t size;           // set to sizeof(DiagGnssLocationData)
    DiagLocationFlagsMask flags; // bitwise OR of LocationFlagsBits to mark which params are valid
    uint64_t timestamp;      // UTC timestamp for location fix, milliseconds since January 1, 1970
    double latitude;         // in degrees
    double longitude;        // in degrees
    double altitude;         // in meters above the WGS 84 reference ellipsoid
    float speed;             // horizontal speed, in meters per second
    float bearing;           // in degrees; range [0, 360)
    float accuracy;          // horizontal acuracy, in meters
                             // confidence level is at 68%
    float verticalAccuracy;  // in meters
                             // confidence level is at 68%
    float speedAccuracy;     // horizontal speed unc, in meters/second
                             // confidence level is at 68%
    float bearingAccuracy;   // in degrees (0 to 359.999)
                             // confidence level is at 68%
    float conformityIndex;   // in range [0, 1]
    DiagLocationTechnologyMask techMask;
    uint64_t elapsedRealTime;    // in ns
    uint64_t elapsedRealTimeUnc; // in ns
    DiagLocationQualityType qualityType;
} DiagGnssLocationData;

typedef PACKED struct PACKED_POST {
    /** Used by Logging Module
      *  Mandatory field */
    log_hdr_type logHeader;
    /** clientDiag Message Version
     *  Mandatory field */
    uint32_t version;
    /** number of locations in the batch */
    uint32_t count;
    DiagGnssLocationData locations[IZAT_FLP_DIAG_BATCH_SIZE_MAX];
} DiagGnssLocationStructType;
